/**
 * =============================================================================
 * @file SadBilling.kt
 * @brief (AR) واجهة المشتريات داخل التطبيق للغة ص
 * @brief (EN) In-App Purchase Interface for Sad Language
 * =============================================================================
 */

package sad.lang.android

import android.app.Activity
import android.content.Context
import com.android.billingclient.api.*
import kotlinx.coroutines.*
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow

/**
 * نظام المشتريات داخل التطبيق للغة ص
 */
class SadBilling(private val context: Context) {
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Types
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * حالة الاتصال
     */
    enum class ConnectionState {
        DISCONNECTED,
        CONNECTING,
        CONNECTED,
        FAILED
    }
    
    /**
     * نوع المنتج
     */
    enum class ProductType {
        ONE_TIME,       // شراء لمرة واحدة
        SUBSCRIPTION    // اشتراك
    }
    
    /**
     * منتج
     */
    data class Product(
        val id: String,
        val type: ProductType,
        val name: String,
        val description: String,
        val price: String,
        val priceMicros: Long,
        val currencyCode: String,
        val subscriptionPeriod: String? = null,
        val freeTrialPeriod: String? = null
    )
    
    /**
     * عملية شراء
     */
    data class Purchase(
        val orderId: String,
        val productId: String,
        val purchaseToken: String,
        val purchaseTime: Long,
        val isAcknowledged: Boolean,
        val purchaseState: Int
    ) {
        val isPurchased: Boolean get() = purchaseState == com.android.billingclient.api.Purchase.PurchaseState.PURCHASED
        val isPending: Boolean get() = purchaseState == com.android.billingclient.api.Purchase.PurchaseState.PENDING
    }
    
    /**
     * نتيجة الشراء
     */
    sealed class BillingResult {
        data class Success(val purchase: Purchase) : BillingResult()
        data class Error(val code: Int, val message: String) : BillingResult()
        object Cancelled : BillingResult()
        object Pending : BillingResult()
    }
    
    /**
     * مستمع المشتريات
     */
    interface PurchaseListener {
        fun onPurchaseCompleted(purchase: Purchase)
        fun onPurchaseFailed(code: Int, message: String)
        fun onPurchaseCancelled()
        fun onPurchasePending()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Properties
    // ═══════════════════════════════════════════════════════════════════════════
    
    private var billingClient: BillingClient? = null
    private val scope = CoroutineScope(Dispatchers.Main + SupervisorJob())
    
    private val _connectionState = MutableStateFlow(ConnectionState.DISCONNECTED)
    val connectionState: StateFlow<ConnectionState> = _connectionState
    
    private val _products = MutableStateFlow<List<Product>>(emptyList())
    val products: StateFlow<List<Product>> = _products
    
    private val _purchases = MutableStateFlow<List<Purchase>>(emptyList())
    val purchases: StateFlow<List<Purchase>> = _purchases
    
    private var purchaseListener: PurchaseListener? = null
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Connection
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * بدء الاتصال
     */
    fun connect(listener: PurchaseListener? = null) {
        purchaseListener = listener
        _connectionState.value = ConnectionState.CONNECTING
        
        billingClient = BillingClient.newBuilder(context)
            .setListener { billingResult, purchases ->
                handlePurchasesUpdate(billingResult, purchases)
            }
            .enablePendingPurchases()
            .build()
        
        billingClient?.startConnection(object : BillingClientStateListener {
            override fun onBillingSetupFinished(billingResult: com.android.billingclient.api.BillingResult) {
                if (billingResult.responseCode == BillingClient.BillingResponseCode.OK) {
                    _connectionState.value = ConnectionState.CONNECTED
                    scope.launch { queryPurchases() }
                } else {
                    _connectionState.value = ConnectionState.FAILED
                }
            }
            
            override fun onBillingServiceDisconnected() {
                _connectionState.value = ConnectionState.DISCONNECTED
                // Try to reconnect
                scope.launch {
                    delay(5000)
                    reconnect()
                }
            }
        })
    }
    
    /**
     * إعادة الاتصال
     */
    private fun reconnect() {
        if (_connectionState.value == ConnectionState.DISCONNECTED) {
            connect(purchaseListener)
        }
    }
    
    /**
     * قطع الاتصال
     */
    fun disconnect() {
        billingClient?.endConnection()
        billingClient = null
        _connectionState.value = ConnectionState.DISCONNECTED
        scope.cancel()
    }
    
    /**
     * هل متصل؟
     */
    fun isConnected(): Boolean {
        return billingClient?.isReady == true
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Products
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * استعلام المنتجات
     */
    suspend fun queryProducts(
        productIds: List<String>,
        type: ProductType = ProductType.ONE_TIME
    ): List<Product> = withContext(Dispatchers.IO) {
        val client = billingClient ?: return@withContext emptyList()
        
        if (!client.isReady) return@withContext emptyList()
        
        val productType = when (type) {
            ProductType.ONE_TIME -> BillingClient.ProductType.INAPP
            ProductType.SUBSCRIPTION -> BillingClient.ProductType.SUBS
        }
        
        val productList = productIds.map { productId ->
            QueryProductDetailsParams.Product.newBuilder()
                .setProductId(productId)
                .setProductType(productType)
                .build()
        }
        
        val params = QueryProductDetailsParams.newBuilder()
            .setProductList(productList)
            .build()
        
        suspendCancellableCoroutine { continuation ->
            client.queryProductDetailsAsync(params) { billingResult, productDetailsList ->
                if (billingResult.responseCode == BillingClient.BillingResponseCode.OK) {
                    val products = productDetailsList.map { details ->
                        val pricing = details.oneTimePurchaseOfferDetails
                            ?: details.subscriptionOfferDetails?.firstOrNull()
                            ?.pricingPhases?.pricingPhaseList?.firstOrNull()
                        
                        Product(
                            id = details.productId,
                            type = type,
                            name = details.name,
                            description = details.description,
                            price = pricing?.let {
                                when (it) {
                                    is ProductDetails.OneTimePurchaseOfferDetails -> it.formattedPrice
                                    is ProductDetails.PricingPhase -> it.formattedPrice
                                    else -> ""
                                }
                            } ?: "",
                            priceMicros = pricing?.let {
                                when (it) {
                                    is ProductDetails.OneTimePurchaseOfferDetails -> it.priceAmountMicros
                                    is ProductDetails.PricingPhase -> it.priceAmountMicros
                                    else -> 0L
                                }
                            } ?: 0L,
                            currencyCode = pricing?.let {
                                when (it) {
                                    is ProductDetails.OneTimePurchaseOfferDetails -> it.priceCurrencyCode
                                    is ProductDetails.PricingPhase -> it.priceCurrencyCode
                                    else -> ""
                                }
                            } ?: "",
                            subscriptionPeriod = details.subscriptionOfferDetails?.firstOrNull()
                                ?.pricingPhases?.pricingPhaseList?.firstOrNull()?.billingPeriod,
                            freeTrialPeriod = details.subscriptionOfferDetails?.firstOrNull()
                                ?.pricingPhases?.pricingPhaseList?.find { it.priceAmountMicros == 0L }
                                ?.billingPeriod
                        )
                    }
                    _products.value = products
                    continuation.resume(products) {}
                } else {
                    continuation.resume(emptyList()) {}
                }
            }
        }
    }
    
    /**
     * الحصول على منتج
     */
    fun getProduct(productId: String): Product? {
        return _products.value.find { it.id == productId }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Purchases
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * بدء عملية الشراء
     */
    suspend fun purchase(
        activity: Activity,
        productId: String,
        type: ProductType = ProductType.ONE_TIME
    ): BillingResult = withContext(Dispatchers.Main) {
        val client = billingClient ?: return@withContext BillingResult.Error(-1, "غير متصل")
        
        if (!client.isReady) {
            return@withContext BillingResult.Error(-1, "غير متصل")
        }
        
        // Query product details first
        val productType = when (type) {
            ProductType.ONE_TIME -> BillingClient.ProductType.INAPP
            ProductType.SUBSCRIPTION -> BillingClient.ProductType.SUBS
        }
        
        val productList = listOf(
            QueryProductDetailsParams.Product.newBuilder()
                .setProductId(productId)
                .setProductType(productType)
                .build()
        )
        
        val params = QueryProductDetailsParams.newBuilder()
            .setProductList(productList)
            .build()
        
        suspendCancellableCoroutine { continuation ->
            client.queryProductDetailsAsync(params) { billingResult, productDetailsList ->
                if (billingResult.responseCode != BillingClient.BillingResponseCode.OK) {
                    continuation.resume(
                        BillingResult.Error(billingResult.responseCode, billingResult.debugMessage)
                    ) {}
                    return@queryProductDetailsAsync
                }
                
                val productDetails = productDetailsList.firstOrNull()
                if (productDetails == null) {
                    continuation.resume(BillingResult.Error(-1, "المنتج غير موجود")) {}
                    return@queryProductDetailsAsync
                }
                
                val productDetailsParamsList = listOf(
                    BillingFlowParams.ProductDetailsParams.newBuilder()
                        .setProductDetails(productDetails)
                        .apply {
                            productDetails.subscriptionOfferDetails?.firstOrNull()?.let { offer ->
                                setOfferToken(offer.offerToken)
                            }
                        }
                        .build()
                )
                
                val billingFlowParams = BillingFlowParams.newBuilder()
                    .setProductDetailsParamsList(productDetailsParamsList)
                    .build()
                
                val flowResult = client.launchBillingFlow(activity, billingFlowParams)
                
                when (flowResult.responseCode) {
                    BillingClient.BillingResponseCode.OK -> {
                        // Purchase will be handled in PurchasesUpdatedListener
                        // Don't resume here - wait for callback
                    }
                    BillingClient.BillingResponseCode.USER_CANCELED -> {
                        continuation.resume(BillingResult.Cancelled) {}
                    }
                    else -> {
                        continuation.resume(
                            BillingResult.Error(flowResult.responseCode, flowResult.debugMessage)
                        ) {}
                    }
                }
            }
        }
    }
    
    /**
     * استعلام المشتريات
     */
    suspend fun queryPurchases(): List<Purchase> = withContext(Dispatchers.IO) {
        val client = billingClient ?: return@withContext emptyList()
        
        if (!client.isReady) return@withContext emptyList()
        
        val inAppPurchases = queryPurchasesOfType(BillingClient.ProductType.INAPP)
        val subPurchases = queryPurchasesOfType(BillingClient.ProductType.SUBS)
        
        val allPurchases = inAppPurchases + subPurchases
        _purchases.value = allPurchases
        
        // Acknowledge pending purchases
        allPurchases.filter { it.isPurchased && !it.isAcknowledged }.forEach { purchase ->
            acknowledgePurchase(purchase.purchaseToken)
        }
        
        allPurchases
    }
    
    private suspend fun queryPurchasesOfType(productType: String): List<Purchase> {
        val client = billingClient ?: return emptyList()
        
        val params = QueryPurchasesParams.newBuilder()
            .setProductType(productType)
            .build()
        
        return suspendCancellableCoroutine { continuation ->
            client.queryPurchasesAsync(params) { billingResult, purchasesList ->
                if (billingResult.responseCode == BillingClient.BillingResponseCode.OK) {
                    val purchases = purchasesList.map { purchase ->
                        Purchase(
                            orderId = purchase.orderId ?: "",
                            productId = purchase.products.firstOrNull() ?: "",
                            purchaseToken = purchase.purchaseToken,
                            purchaseTime = purchase.purchaseTime,
                            isAcknowledged = purchase.isAcknowledged,
                            purchaseState = purchase.purchaseState
                        )
                    }
                    continuation.resume(purchases) {}
                } else {
                    continuation.resume(emptyList()) {}
                }
            }
        }
    }
    
    /**
     * تأكيد الشراء
     */
    suspend fun acknowledgePurchase(purchaseToken: String): Boolean = withContext(Dispatchers.IO) {
        val client = billingClient ?: return@withContext false
        
        val params = AcknowledgePurchaseParams.newBuilder()
            .setPurchaseToken(purchaseToken)
            .build()
        
        suspendCancellableCoroutine { continuation ->
            client.acknowledgePurchase(params) { billingResult ->
                continuation.resume(billingResult.responseCode == BillingClient.BillingResponseCode.OK) {}
            }
        }
    }
    
    /**
     * استهلاك الشراء (للمنتجات القابلة للاستهلاك)
     */
    suspend fun consumePurchase(purchaseToken: String): Boolean = withContext(Dispatchers.IO) {
        val client = billingClient ?: return@withContext false
        
        val params = ConsumeParams.newBuilder()
            .setPurchaseToken(purchaseToken)
            .build()
        
        suspendCancellableCoroutine { continuation ->
            client.consumeAsync(params) { billingResult, _ ->
                continuation.resume(billingResult.responseCode == BillingClient.BillingResponseCode.OK) {}
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Verification
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * هل المنتج مشترى؟
     */
    fun isPurchased(productId: String): Boolean {
        return _purchases.value.any { it.productId == productId && it.isPurchased }
    }
    
    /**
     * هل الاشتراك فعال؟
     */
    fun hasActiveSubscription(productId: String): Boolean {
        return _purchases.value.any { 
            it.productId == productId && it.isPurchased && it.isAcknowledged 
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Purchase Updates Handler
    // ═══════════════════════════════════════════════════════════════════════════
    
    private fun handlePurchasesUpdate(
        billingResult: com.android.billingclient.api.BillingResult,
        purchases: List<com.android.billingclient.api.Purchase>?
    ) {
        when (billingResult.responseCode) {
            BillingClient.BillingResponseCode.OK -> {
                purchases?.forEach { purchase ->
                    val sadPurchase = Purchase(
                        orderId = purchase.orderId ?: "",
                        productId = purchase.products.firstOrNull() ?: "",
                        purchaseToken = purchase.purchaseToken,
                        purchaseTime = purchase.purchaseTime,
                        isAcknowledged = purchase.isAcknowledged,
                        purchaseState = purchase.purchaseState
                    )
                    
                    when {
                        sadPurchase.isPurchased -> {
                            purchaseListener?.onPurchaseCompleted(sadPurchase)
                            // Acknowledge
                            scope.launch {
                                acknowledgePurchase(sadPurchase.purchaseToken)
                                queryPurchases() // Refresh
                            }
                        }
                        sadPurchase.isPending -> {
                            purchaseListener?.onPurchasePending()
                        }
                    }
                }
            }
            BillingClient.BillingResponseCode.USER_CANCELED -> {
                purchaseListener?.onPurchaseCancelled()
            }
            else -> {
                purchaseListener?.onPurchaseFailed(
                    billingResult.responseCode,
                    billingResult.debugMessage
                )
            }
        }
    }
}
