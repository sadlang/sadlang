// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadFirebaseAnalyticsBridge.kt
// الوصف: جسر Kotlin لتحليلات Firebase
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import android.os.Bundle
import com.google.firebase.analytics.FirebaseAnalytics
import com.google.firebase.analytics.ktx.analytics
import com.google.firebase.ktx.Firebase

/**
 * جسر Firebase Analytics — SadFirebaseAnalyticsBridge
 */
object SadFirebaseAnalyticsBridge {
    
    private var context: Context? = null
    private val analytics: FirebaseAnalytics by lazy { Firebase.analytics }
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Event Logging
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun logEvent(eventName: String, params: Map<String, Any?>? = null) {
        val bundle = params?.toBundle()
        analytics.logEvent(eventName, bundle)
    }
    
    @JvmStatic
    fun logScreenView(screenName: String, screenClass: String? = null) {
        val bundle = Bundle().apply {
            putString(FirebaseAnalytics.Param.SCREEN_NAME, screenName)
            screenClass?.let { putString(FirebaseAnalytics.Param.SCREEN_CLASS, it) }
        }
        analytics.logEvent(FirebaseAnalytics.Event.SCREEN_VIEW, bundle)
    }
    
    @JvmStatic
    fun logLogin(method: String) {
        val bundle = Bundle().apply {
            putString(FirebaseAnalytics.Param.METHOD, method)
        }
        analytics.logEvent(FirebaseAnalytics.Event.LOGIN, bundle)
    }
    
    @JvmStatic
    fun logSignUp(method: String) {
        val bundle = Bundle().apply {
            putString(FirebaseAnalytics.Param.METHOD, method)
        }
        analytics.logEvent(FirebaseAnalytics.Event.SIGN_UP, bundle)
    }
    
    @JvmStatic
    fun logSearch(searchTerm: String) {
        val bundle = Bundle().apply {
            putString(FirebaseAnalytics.Param.SEARCH_TERM, searchTerm)
        }
        analytics.logEvent(FirebaseAnalytics.Event.SEARCH, bundle)
    }
    
    @JvmStatic
    fun logShare(contentType: String, itemId: String, method: String? = null) {
        val bundle = Bundle().apply {
            putString(FirebaseAnalytics.Param.CONTENT_TYPE, contentType)
            putString(FirebaseAnalytics.Param.ITEM_ID, itemId)
            method?.let { putString(FirebaseAnalytics.Param.METHOD, it) }
        }
        analytics.logEvent(FirebaseAnalytics.Event.SHARE, bundle)
    }
    
    @JvmStatic
    fun logSelectContent(contentType: String, itemId: String) {
        val bundle = Bundle().apply {
            putString(FirebaseAnalytics.Param.CONTENT_TYPE, contentType)
            putString(FirebaseAnalytics.Param.ITEM_ID, itemId)
        }
        analytics.logEvent(FirebaseAnalytics.Event.SELECT_CONTENT, bundle)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // E-commerce Events
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun logAddToCart(itemId: String, itemName: String, value: Double, currency: String = "SAR") {
        val bundle = Bundle().apply {
            putString(FirebaseAnalytics.Param.ITEM_ID, itemId)
            putString(FirebaseAnalytics.Param.ITEM_NAME, itemName)
            putDouble(FirebaseAnalytics.Param.VALUE, value)
            putString(FirebaseAnalytics.Param.CURRENCY, currency)
        }
        analytics.logEvent(FirebaseAnalytics.Event.ADD_TO_CART, bundle)
    }
    
    @JvmStatic
    fun logRemoveFromCart(itemId: String, itemName: String, value: Double, currency: String = "SAR") {
        val bundle = Bundle().apply {
            putString(FirebaseAnalytics.Param.ITEM_ID, itemId)
            putString(FirebaseAnalytics.Param.ITEM_NAME, itemName)
            putDouble(FirebaseAnalytics.Param.VALUE, value)
            putString(FirebaseAnalytics.Param.CURRENCY, currency)
        }
        analytics.logEvent(FirebaseAnalytics.Event.REMOVE_FROM_CART, bundle)
    }
    
    @JvmStatic
    fun logViewItem(itemId: String, itemName: String, value: Double? = null, currency: String = "SAR") {
        val bundle = Bundle().apply {
            putString(FirebaseAnalytics.Param.ITEM_ID, itemId)
            putString(FirebaseAnalytics.Param.ITEM_NAME, itemName)
            value?.let {
                putDouble(FirebaseAnalytics.Param.VALUE, it)
                putString(FirebaseAnalytics.Param.CURRENCY, currency)
            }
        }
        analytics.logEvent(FirebaseAnalytics.Event.VIEW_ITEM, bundle)
    }
    
    @JvmStatic
    fun logViewItemList(itemListId: String, itemListName: String) {
        val bundle = Bundle().apply {
            putString(FirebaseAnalytics.Param.ITEM_LIST_ID, itemListId)
            putString(FirebaseAnalytics.Param.ITEM_LIST_NAME, itemListName)
        }
        analytics.logEvent(FirebaseAnalytics.Event.VIEW_ITEM_LIST, bundle)
    }
    
    @JvmStatic
    fun logBeginCheckout(value: Double, currency: String = "SAR", coupon: String? = null) {
        val bundle = Bundle().apply {
            putDouble(FirebaseAnalytics.Param.VALUE, value)
            putString(FirebaseAnalytics.Param.CURRENCY, currency)
            coupon?.let { putString(FirebaseAnalytics.Param.COUPON, it) }
        }
        analytics.logEvent(FirebaseAnalytics.Event.BEGIN_CHECKOUT, bundle)
    }
    
    @JvmStatic
    fun logPurchase(
        transactionId: String,
        value: Double,
        currency: String = "SAR",
        coupon: String? = null,
        shipping: Double? = null,
        tax: Double? = null
    ) {
        val bundle = Bundle().apply {
            putString(FirebaseAnalytics.Param.TRANSACTION_ID, transactionId)
            putDouble(FirebaseAnalytics.Param.VALUE, value)
            putString(FirebaseAnalytics.Param.CURRENCY, currency)
            coupon?.let { putString(FirebaseAnalytics.Param.COUPON, it) }
            shipping?.let { putDouble(FirebaseAnalytics.Param.SHIPPING, it) }
            tax?.let { putDouble(FirebaseAnalytics.Param.TAX, it) }
        }
        analytics.logEvent(FirebaseAnalytics.Event.PURCHASE, bundle)
    }
    
    @JvmStatic
    fun logRefund(transactionId: String, value: Double, currency: String = "SAR") {
        val bundle = Bundle().apply {
            putString(FirebaseAnalytics.Param.TRANSACTION_ID, transactionId)
            putDouble(FirebaseAnalytics.Param.VALUE, value)
            putString(FirebaseAnalytics.Param.CURRENCY, currency)
        }
        analytics.logEvent(FirebaseAnalytics.Event.REFUND, bundle)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // App Events
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun logAppOpen() {
        analytics.logEvent(FirebaseAnalytics.Event.APP_OPEN, null)
    }
    
    @JvmStatic
    fun logTutorialBegin() {
        analytics.logEvent(FirebaseAnalytics.Event.TUTORIAL_BEGIN, null)
    }
    
    @JvmStatic
    fun logTutorialComplete() {
        analytics.logEvent(FirebaseAnalytics.Event.TUTORIAL_COMPLETE, null)
    }
    
    @JvmStatic
    fun logLevelStart(levelName: String) {
        val bundle = Bundle().apply {
            putString(FirebaseAnalytics.Param.LEVEL_NAME, levelName)
        }
        analytics.logEvent(FirebaseAnalytics.Event.LEVEL_START, bundle)
    }
    
    @JvmStatic
    fun logLevelEnd(levelName: String, success: Boolean) {
        val bundle = Bundle().apply {
            putString(FirebaseAnalytics.Param.LEVEL_NAME, levelName)
            putLong(FirebaseAnalytics.Param.SUCCESS, if (success) 1 else 0)
        }
        analytics.logEvent(FirebaseAnalytics.Event.LEVEL_END, bundle)
    }
    
    @JvmStatic
    fun logUnlockAchievement(achievementId: String) {
        val bundle = Bundle().apply {
            putString(FirebaseAnalytics.Param.ACHIEVEMENT_ID, achievementId)
        }
        analytics.logEvent(FirebaseAnalytics.Event.UNLOCK_ACHIEVEMENT, bundle)
    }
    
    @JvmStatic
    fun logPostScore(score: Long, level: Long? = null, character: String? = null) {
        val bundle = Bundle().apply {
            putLong(FirebaseAnalytics.Param.SCORE, score)
            level?.let { putLong(FirebaseAnalytics.Param.LEVEL, it) }
            character?.let { putString(FirebaseAnalytics.Param.CHARACTER, it) }
        }
        analytics.logEvent(FirebaseAnalytics.Event.POST_SCORE, bundle)
    }
    
    @JvmStatic
    fun logSpendVirtualCurrency(itemName: String, virtualCurrencyName: String, value: Double) {
        val bundle = Bundle().apply {
            putString(FirebaseAnalytics.Param.ITEM_NAME, itemName)
            putString(FirebaseAnalytics.Param.VIRTUAL_CURRENCY_NAME, virtualCurrencyName)
            putDouble(FirebaseAnalytics.Param.VALUE, value)
        }
        analytics.logEvent(FirebaseAnalytics.Event.SPEND_VIRTUAL_CURRENCY, bundle)
    }
    
    @JvmStatic
    fun logEarnVirtualCurrency(virtualCurrencyName: String, value: Double) {
        val bundle = Bundle().apply {
            putString(FirebaseAnalytics.Param.VIRTUAL_CURRENCY_NAME, virtualCurrencyName)
            putDouble(FirebaseAnalytics.Param.VALUE, value)
        }
        analytics.logEvent(FirebaseAnalytics.Event.EARN_VIRTUAL_CURRENCY, bundle)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // User Properties
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun setUserProperty(name: String, value: String?) {
        analytics.setUserProperty(name, value)
    }
    
    @JvmStatic
    fun setUserId(userId: String?) {
        analytics.setUserId(userId)
    }
    
    @JvmStatic
    fun setUserLanguage(language: String) {
        setUserProperty("language", language)
    }
    
    @JvmStatic
    fun setUserCountry(country: String) {
        setUserProperty("country", country)
    }
    
    @JvmStatic
    fun setUserAge(age: Int) {
        setUserProperty("age", age.toString())
    }
    
    @JvmStatic
    fun setUserGender(gender: String) {
        setUserProperty("gender", gender)
    }
    
    @JvmStatic
    fun setUserSubscription(subscriptionType: String?) {
        setUserProperty("subscription_type", subscriptionType)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Settings
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun setAnalyticsCollectionEnabled(enabled: Boolean) {
        analytics.setAnalyticsCollectionEnabled(enabled)
    }
    
    @JvmStatic
    fun setSessionTimeoutDuration(milliseconds: Long) {
        analytics.setSessionTimeoutDuration(milliseconds)
    }
    
    @JvmStatic
    fun resetAnalyticsData() {
        analytics.resetAnalyticsData()
    }
    
    @JvmStatic
    fun setDefaultEventParameters(params: Map<String, Any?>?) {
        analytics.setDefaultEventParameters(params?.toBundle())
    }
    
    @JvmStatic
    fun getAppInstanceId(callback: (String?) -> Unit) {
        analytics.appInstanceId.addOnSuccessListener { id ->
            callback(id)
        }.addOnFailureListener {
            callback(null)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Custom Arabic Events
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun logArabicEvent(eventName: String, params: Map<String, Any?>? = null) {
        // ترجمة اسم الحدث للإنجليزية (لأن Firebase لا يدعم الأسماء العربية)
        val translatedName = eventName
            .replace(" ", "_")
            .replace(Regex("[^a-zA-Z0-9_]"), "")
            .take(40)
        
        val bundle = params?.toBundle()
        analytics.logEvent(translatedName, bundle)
    }
    
    @JvmStatic
    fun logButtonClick(buttonName: String, screenName: String? = null) {
        logEvent("button_click", mapOf(
            "button_name" to buttonName,
            "screen_name" to screenName
        ))
    }
    
    @JvmStatic
    fun logError(errorType: String, errorMessage: String, screenName: String? = null) {
        logEvent("app_error", mapOf(
            "error_type" to errorType,
            "error_message" to errorMessage,
            "screen_name" to screenName
        ))
    }
    
    @JvmStatic
    fun logFeatureUsed(featureName: String) {
        logEvent("feature_used", mapOf(
            "feature_name" to featureName
        ))
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Utilities
    // ═══════════════════════════════════════════════════════════════════════════════
    
    private fun Map<String, Any?>.toBundle(): Bundle {
        val bundle = Bundle()
        forEach { (key, value) ->
            when (value) {
                is String -> bundle.putString(key, value)
                is Int -> bundle.putInt(key, value)
                is Long -> bundle.putLong(key, value)
                is Double -> bundle.putDouble(key, value)
                is Float -> bundle.putFloat(key, value)
                is Boolean -> bundle.putBoolean(key, value)
                is Array<*> -> {
                    if (value.isArrayOf<String>()) {
                        @Suppress("UNCHECKED_CAST")
                        bundle.putStringArray(key, value as Array<String>)
                    }
                }
                null -> { /* تجاهل القيم الفارغة */ }
            }
        }
        return bundle
    }
}
