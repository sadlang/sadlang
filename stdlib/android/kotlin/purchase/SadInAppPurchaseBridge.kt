// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadInAppPurchaseBridge.kt
// الوصف: جسر Kotlin لعمليات الشراء داخل التطبيق (Google Play Billing)
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.app.Activity
import android.content.Context
import com.android.billingclient.api.*
import kotlinx.coroutines.*

/**
 * جسر In-App Purchase — SadInAppPurchaseBridge
 */
object SadInAppPurchaseBridge {
    
    private var context: Context? = null
    private var billingClient: BillingClient? = null
    private var isConnected = false
    
    private val productDetails = mutableMapOf<String, ProductDetails>()
    private val purchases = mutableListOf<Purchase>()
    
    private var onConnectionCallback: ((Boolean, String?) -> Unit)? = null
    private var onPurchaseCallback: ((Boolean, Map<String, Any?>?) -> Unit)? = null
    private var onPurchasesUpdatedCallback: ((List<Map<String, Any?>>) -> Unit)? = null
    
    @JvmStatic
    fun initialize(ctx: Context, onConnection: (Boolean, String?) -> Unit) {
        context = ctx.applicationContext
        onConnectionCallback = onConnection
        
        billingClient = BillingClient.newBuilder(ctx)
            .setListener(purchasesUpdatedListener)
            .enablePendingPurchases()
            .build()
        
        connect()
    }
    
    private fun connect() {
        billingClient?.startConnection(object : BillingClientStateListener {
            override fun onBillingSetupFinished(result: BillingResult) {
                if (result.responseCode == BillingClient.BillingResponseCode.OK) {
                    isConnected = true
                    onConnectionCallback?.invoke(true, null)
                } else {
                    isConnected = false
                    onConnectionCallback?.invoke(false, getArabicError(result.responseCode))
                }
            }
            
            override fun onBillingServiceDisconnected() {
                isConnected = false
                // إعادة الاتصال
                connect()
            }
        })
    }
    
    private val purchasesUpdatedListener = PurchasesUpdatedListener { result, purchasesList ->
        if (result.responseCode == BillingClient.BillingResponseCode.OK && purchasesList != null) {
            for (purchase in purchasesList) {
                handlePurchase(purchase)
            }
            
            val purchaseDataList = purchasesList.map { it.toMap() }
            onPurchasesUpdatedCallback?.invoke(purchaseDataList)
            
            if (purchasesList.isNotEmpty()) {
                onPurchaseCallback?.invoke(true, purchasesList.first().toMap())
            }
        } else if (result.responseCode == BillingClient.BillingResponseCode.USER_CANCELED) {
            onPurchaseCallback?.invoke(false, mapOf("error" to "تم إلغاء الشراء"))
        } else {
            onPurchaseCallback?.invoke(false, mapOf("error" to getArabicError(result.responseCode)))
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Query Products
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun queryInAppProducts(
        productIds: List<String>,
        onSuccess: (List<Map<String, Any?>>) -> Unit,
        onError: (String) -> Unit
    ) {
        queryProducts(productIds, BillingClient.ProductType.INAPP, onSuccess, onError)
    }
    
    @JvmStatic
    fun querySubscriptions(
        productIds: List<String>,
        onSuccess: (List<Map<String, Any?>>) -> Unit,
        onError: (String) -> Unit
    ) {
        queryProducts(productIds, BillingClient.ProductType.SUBS, onSuccess, onError)
    }
    
    private fun queryProducts(
        productIds: List<String>,
        productType: String,
        onSuccess: (List<Map<String, Any?>>) -> Unit,
        onError: (String) -> Unit
    ) {
        if (!isConnected) {
            onError("غير متصل بخدمة الفوترة")
            return
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
        
        billingClient?.queryProductDetailsAsync(params) { result, productDetailsList ->
            if (result.responseCode == BillingClient.BillingResponseCode.OK) {
                productDetailsList.forEach { details ->
                    productDetails[details.productId] = details
                }
                
                val products = productDetailsList.map { details ->
                    mapOf(
                        "productId" to details.productId,
                        "name" to details.name,
                        "title" to details.title,
                        "description" to details.description,
                        "productType" to details.productType,
                        "oneTimePurchaseOfferDetails" to details.oneTimePurchaseOfferDetails?.let { offer ->
                            mapOf(
                                "price" to offer.formattedPrice,
                                "priceMicros" to offer.priceAmountMicros,
                                "currencyCode" to offer.priceCurrencyCode
                            )
                        },
                        "subscriptionOfferDetails" to details.subscriptionOfferDetails?.map { offer ->
                            mapOf(
                                "offerId" to offer.offerId,
                                "basePlanId" to offer.basePlanId,
                                "offerToken" to offer.offerToken,
                                "pricingPhases" to offer.pricingPhases.pricingPhaseList.map { phase ->
                                    mapOf(
                                        "price" to phase.formattedPrice,
                                        "priceMicros" to phase.priceAmountMicros,
                                        "currencyCode" to phase.priceCurrencyCode,
                                        "billingPeriod" to phase.billingPeriod,
                                        "billingCycleCount" to phase.billingCycleCount,
                                        "recurrenceMode" to phase.recurrenceMode
                                    )
                                }
                            )
                        }
                    )
                }
                
                onSuccess(products)
            } else {
                onError(getArabicError(result.responseCode))
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Purchase
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun purchaseProduct(
        activity: Activity,
        productId: String,
        onPurchase: (Boolean, Map<String, Any?>?) -> Unit
    ) {
        onPurchaseCallback = onPurchase
        
        val productDetail = productDetails[productId]
        if (productDetail == null) {
            onPurchase(false, mapOf("error" to "المنتج غير موجود، يرجى استعلام المنتجات أولاً"))
            return
        }
        
        val productDetailsParamsList = listOf(
            BillingFlowParams.ProductDetailsParams.newBuilder()
                .setProductDetails(productDetail)
                .build()
        )
        
        val billingFlowParams = BillingFlowParams.newBuilder()
            .setProductDetailsParamsList(productDetailsParamsList)
            .build()
        
        val result = billingClient?.launchBillingFlow(activity, billingFlowParams)
        if (result?.responseCode != BillingClient.BillingResponseCode.OK) {
            onPurchase(false, mapOf("error" to getArabicError(result?.responseCode ?: -1)))
        }
    }
    
    @JvmStatic
    fun purchaseSubscription(
        activity: Activity,
        productId: String,
        offerToken: String,
        onPurchase: (Boolean, Map<String, Any?>?) -> Unit
    ) {
        onPurchaseCallback = onPurchase
        
        val productDetail = productDetails[productId]
        if (productDetail == null) {
            onPurchase(false, mapOf("error" to "المنتج غير موجود، يرجى استعلام المنتجات أولاً"))
            return
        }
        
        val productDetailsParamsList = listOf(
            BillingFlowParams.ProductDetailsParams.newBuilder()
                .setProductDetails(productDetail)
                .setOfferToken(offerToken)
                .build()
        )
        
        val billingFlowParams = BillingFlowParams.newBuilder()
            .setProductDetailsParamsList(productDetailsParamsList)
            .build()
        
        val result = billingClient?.launchBillingFlow(activity, billingFlowParams)
        if (result?.responseCode != BillingClient.BillingResponseCode.OK) {
            onPurchase(false, mapOf("error" to getArabicError(result?.responseCode ?: -1)))
        }
    }
    
    private fun handlePurchase(purchase: Purchase) {
        if (purchase.purchaseState == Purchase.PurchaseState.PURCHASED) {
            if (!purchase.isAcknowledged) {
                acknowledgePurchase(purchase)
            }
        }
        purchases.add(purchase)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Acknowledge & Consume
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun acknowledgePurchase(
        purchase: Purchase,
        onSuccess: (() -> Unit)? = null,
        onError: ((String) -> Unit)? = null
    ) {
        if (purchase.isAcknowledged) {
            onSuccess?.invoke()
            return
        }
        
        val params = AcknowledgePurchaseParams.newBuilder()
            .setPurchaseToken(purchase.purchaseToken)
            .build()
        
        billingClient?.acknowledgePurchase(params) { result ->
            if (result.responseCode == BillingClient.BillingResponseCode.OK) {
                onSuccess?.invoke()
            } else {
                onError?.invoke(getArabicError(result.responseCode))
            }
        }
    }
    
    @JvmStatic
    fun consumePurchase(
        purchaseToken: String,
        onSuccess: () -> Unit,
        onError: (String) -> Unit
    ) {
        val params = ConsumeParams.newBuilder()
            .setPurchaseToken(purchaseToken)
            .build()
        
        billingClient?.consumeAsync(params) { result, _ ->
            if (result.responseCode == BillingClient.BillingResponseCode.OK) {
                onSuccess()
            } else {
                onError(getArabicError(result.responseCode))
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Query Purchases
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun queryPurchases(
        productType: String = BillingClient.ProductType.INAPP,
        onSuccess: (List<Map<String, Any?>>) -> Unit,
        onError: (String) -> Unit
    ) {
        if (!isConnected) {
            onError("غير متصل بخدمة الفوترة")
            return
        }
        
        val params = QueryPurchasesParams.newBuilder()
            .setProductType(productType)
            .build()
        
        billingClient?.queryPurchasesAsync(params) { result, purchasesList ->
            if (result.responseCode == BillingClient.BillingResponseCode.OK) {
                purchases.clear()
                purchases.addAll(purchasesList)
                onSuccess(purchasesList.map { it.toMap() })
            } else {
                onError(getArabicError(result.responseCode))
            }
        }
    }
    
    @JvmStatic
    fun queryAllPurchases(
        onSuccess: (List<Map<String, Any?>>) -> Unit,
        onError: (String) -> Unit
    ) {
        val allPurchases = mutableListOf<Map<String, Any?>>()
        
        queryPurchases(BillingClient.ProductType.INAPP, { inAppPurchases ->
            allPurchases.addAll(inAppPurchases)
            queryPurchases(BillingClient.ProductType.SUBS, { subPurchases ->
                allPurchases.addAll(subPurchases)
                onSuccess(allPurchases)
            }, onError)
        }, onError)
    }
    
    @JvmStatic
    fun hasPurchased(productId: String): Boolean {
        return purchases.any { purchase ->
            purchase.products.contains(productId) &&
            purchase.purchaseState == Purchase.PurchaseState.PURCHASED
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Purchase History
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun queryPurchaseHistory(
        productType: String = BillingClient.ProductType.INAPP,
        onSuccess: (List<Map<String, Any?>>) -> Unit,
        onError: (String) -> Unit
    ) {
        val params = QueryPurchaseHistoryParams.newBuilder()
            .setProductType(productType)
            .build()
        
        billingClient?.queryPurchaseHistoryAsync(params) { result, historyList ->
            if (result.responseCode == BillingClient.BillingResponseCode.OK) {
                val history = historyList?.map { record ->
                    mapOf(
                        "purchaseToken" to record.purchaseToken,
                        "purchaseTime" to record.purchaseTime,
                        "products" to record.products,
                        "quantity" to record.quantity,
                        "signature" to record.signature,
                        "originalJson" to record.originalJson
                    )
                } ?: emptyList()
                onSuccess(history)
            } else {
                onError(getArabicError(result.responseCode))
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Listeners
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun setOnPurchasesUpdatedListener(callback: (List<Map<String, Any?>>) -> Unit) {
        onPurchasesUpdatedCallback = callback
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Connection
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun isConnected(): Boolean = isConnected
    
    @JvmStatic
    fun disconnect() {
        billingClient?.endConnection()
        isConnected = false
    }
    
    @JvmStatic
    fun reconnect() {
        if (!isConnected) {
            connect()
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Utilities
    // ═══════════════════════════════════════════════════════════════════════════════
    
    private fun Purchase.toMap(): Map<String, Any?> {
        return mapOf(
            "orderId" to orderId,
            "purchaseToken" to purchaseToken,
            "purchaseTime" to purchaseTime,
            "purchaseState" to getPurchaseStateName(purchaseState),
            "products" to products,
            "quantity" to quantity,
            "isAcknowledged" to isAcknowledged,
            "isAutoRenewing" to isAutoRenewing,
            "signature" to signature,
            "originalJson" to originalJson
        )
    }
    
    private fun getPurchaseStateName(state: Int): String {
        return when (state) {
            Purchase.PurchaseState.PURCHASED -> "مُشترى"
            Purchase.PurchaseState.PENDING -> "قيد الانتظار"
            else -> "غير مُشترى"
        }
    }
    
    private fun getArabicError(code: Int): String {
        return when (code) {
            BillingClient.BillingResponseCode.SERVICE_TIMEOUT -> "انتهت مهلة الخدمة"
            BillingClient.BillingResponseCode.FEATURE_NOT_SUPPORTED -> "الميزة غير مدعومة"
            BillingClient.BillingResponseCode.SERVICE_DISCONNECTED -> "الخدمة منفصلة"
            BillingClient.BillingResponseCode.USER_CANCELED -> "تم الإلغاء"
            BillingClient.BillingResponseCode.SERVICE_UNAVAILABLE -> "الخدمة غير متاحة"
            BillingClient.BillingResponseCode.BILLING_UNAVAILABLE -> "الفوترة غير متاحة"
            BillingClient.BillingResponseCode.ITEM_UNAVAILABLE -> "العنصر غير متاح"
            BillingClient.BillingResponseCode.DEVELOPER_ERROR -> "خطأ في التطوير"
            BillingClient.BillingResponseCode.ERROR -> "خطأ عام"
            BillingClient.BillingResponseCode.ITEM_ALREADY_OWNED -> "تم شراء العنصر مسبقاً"
            BillingClient.BillingResponseCode.ITEM_NOT_OWNED -> "لم يتم شراء العنصر"
            else -> "خطأ غير معروف: $code"
        }
    }
}
