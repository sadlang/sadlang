// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadAdMobBridge.kt
// الوصف: جسر Kotlin لإعلانات Google AdMob
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.app.Activity
import android.content.Context
import android.view.ViewGroup
import com.google.android.gms.ads.*
import com.google.android.gms.ads.interstitial.InterstitialAd
import com.google.android.gms.ads.interstitial.InterstitialAdLoadCallback
import com.google.android.gms.ads.rewarded.RewardedAd
import com.google.android.gms.ads.rewarded.RewardedAdLoadCallback

/**
 * جسر AdMob — SadAdMobBridge
 */
object SadAdMobBridge {
    
    private var context: Context? = null
    private var isInitialized = false
    
    private var interstitialAd: InterstitialAd? = null
    private var rewardedAd: RewardedAd? = null
    
    // Test IDs
    private const val TEST_BANNER_ID = "ca-app-pub-3940256099942544/6300978111"
    private const val TEST_INTERSTITIAL_ID = "ca-app-pub-3940256099942544/1033173712"
    private const val TEST_REWARDED_ID = "ca-app-pub-3940256099942544/5224354917"
    
    // Callbacks
    private var onInterstitialLoadedCallback: (() -> Unit)? = null
    private var onInterstitialFailedCallback: ((String) -> Unit)? = null
    private var onInterstitialShownCallback: (() -> Unit)? = null
    private var onInterstitialDismissedCallback: (() -> Unit)? = null
    
    private var onRewardedLoadedCallback: (() -> Unit)? = null
    private var onRewardedFailedCallback: ((String) -> Unit)? = null
    private var onRewardedShownCallback: (() -> Unit)? = null
    private var onRewardedDismissedCallback: (() -> Unit)? = null
    private var onRewardEarnedCallback: ((String, Int) -> Unit)? = null
    
    @JvmStatic
    fun initialize(ctx: Context, onComplete: (Boolean) -> Unit) {
        context = ctx.applicationContext
        
        MobileAds.initialize(ctx) { initStatus ->
            isInitialized = true
            onComplete(true)
        }
    }
    
    @JvmStatic
    fun isInitialized(): Boolean = isInitialized
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Banner Ads
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun createBannerAd(ctx: Context, adUnitId: String? = null): AdView {
        val adView = AdView(ctx)
        adView.setAdSize(AdSize.BANNER)
        adView.adUnitId = adUnitId ?: TEST_BANNER_ID
        return adView
    }
    
    @JvmStatic
    fun createLargeBannerAd(ctx: Context, adUnitId: String? = null): AdView {
        val adView = AdView(ctx)
        adView.setAdSize(AdSize.LARGE_BANNER)
        adView.adUnitId = adUnitId ?: TEST_BANNER_ID
        return adView
    }
    
    @JvmStatic
    fun createAdaptiveBannerAd(ctx: Context, adUnitId: String? = null): AdView {
        val adView = AdView(ctx)
        val displayMetrics = ctx.resources.displayMetrics
        val adWidth = (displayMetrics.widthPixels / displayMetrics.density).toInt()
        adView.setAdSize(AdSize.getCurrentOrientationAnchoredAdaptiveBannerAdSize(ctx, adWidth))
        adView.adUnitId = adUnitId ?: TEST_BANNER_ID
        return adView
    }
    
    @JvmStatic
    fun loadBanner(
        adView: AdView,
        onLoaded: (() -> Unit)? = null,
        onFailed: ((String) -> Unit)? = null,
        onClicked: (() -> Unit)? = null
    ) {
        adView.adListener = object : AdListener() {
            override fun onAdLoaded() {
                onLoaded?.invoke()
            }
            
            override fun onAdFailedToLoad(error: LoadAdError) {
                onFailed?.invoke(getArabicError(error.code))
            }
            
            override fun onAdClicked() {
                onClicked?.invoke()
            }
        }
        
        val adRequest = AdRequest.Builder().build()
        adView.loadAd(adRequest)
    }
    
    @JvmStatic
    fun showBannerIn(container: ViewGroup, adView: AdView) {
        container.removeAllViews()
        adView.parent?.let { (it as? ViewGroup)?.removeView(adView) }
        container.addView(adView)
    }
    
    @JvmStatic
    fun destroyBanner(adView: AdView) {
        adView.destroy()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Interstitial Ads
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun loadInterstitial(
        adUnitId: String? = null,
        onLoaded: () -> Unit,
        onFailed: (String) -> Unit
    ) {
        val ctx = context ?: run {
            onFailed("لم يتم تهيئة السياق")
            return
        }
        
        onInterstitialLoadedCallback = onLoaded
        onInterstitialFailedCallback = onFailed
        
        val adRequest = AdRequest.Builder().build()
        
        InterstitialAd.load(
            ctx,
            adUnitId ?: TEST_INTERSTITIAL_ID,
            adRequest,
            object : InterstitialAdLoadCallback() {
                override fun onAdLoaded(ad: InterstitialAd) {
                    interstitialAd = ad
                    setupInterstitialCallbacks(ad)
                    onInterstitialLoadedCallback?.invoke()
                }
                
                override fun onAdFailedToLoad(error: LoadAdError) {
                    interstitialAd = null
                    onInterstitialFailedCallback?.invoke(getArabicError(error.code))
                }
            }
        )
    }
    
    private fun setupInterstitialCallbacks(ad: InterstitialAd) {
        ad.fullScreenContentCallback = object : FullScreenContentCallback() {
            override fun onAdShowedFullScreenContent() {
                onInterstitialShownCallback?.invoke()
            }
            
            override fun onAdDismissedFullScreenContent() {
                interstitialAd = null
                onInterstitialDismissedCallback?.invoke()
            }
            
            override fun onAdFailedToShowFullScreenContent(error: AdError) {
                interstitialAd = null
            }
        }
    }
    
    @JvmStatic
    fun showInterstitial(
        activity: Activity,
        onShown: (() -> Unit)? = null,
        onDismissed: (() -> Unit)? = null
    ): Boolean {
        onInterstitialShownCallback = onShown
        onInterstitialDismissedCallback = onDismissed
        
        return if (interstitialAd != null) {
            interstitialAd?.show(activity)
            true
        } else {
            false
        }
    }
    
    @JvmStatic
    fun isInterstitialReady(): Boolean = interstitialAd != null
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Rewarded Ads
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun loadRewarded(
        adUnitId: String? = null,
        onLoaded: () -> Unit,
        onFailed: (String) -> Unit
    ) {
        val ctx = context ?: run {
            onFailed("لم يتم تهيئة السياق")
            return
        }
        
        onRewardedLoadedCallback = onLoaded
        onRewardedFailedCallback = onFailed
        
        val adRequest = AdRequest.Builder().build()
        
        RewardedAd.load(
            ctx,
            adUnitId ?: TEST_REWARDED_ID,
            adRequest,
            object : RewardedAdLoadCallback() {
                override fun onAdLoaded(ad: RewardedAd) {
                    rewardedAd = ad
                    setupRewardedCallbacks(ad)
                    onRewardedLoadedCallback?.invoke()
                }
                
                override fun onAdFailedToLoad(error: LoadAdError) {
                    rewardedAd = null
                    onRewardedFailedCallback?.invoke(getArabicError(error.code))
                }
            }
        )
    }
    
    private fun setupRewardedCallbacks(ad: RewardedAd) {
        ad.fullScreenContentCallback = object : FullScreenContentCallback() {
            override fun onAdShowedFullScreenContent() {
                onRewardedShownCallback?.invoke()
            }
            
            override fun onAdDismissedFullScreenContent() {
                rewardedAd = null
                onRewardedDismissedCallback?.invoke()
            }
            
            override fun onAdFailedToShowFullScreenContent(error: AdError) {
                rewardedAd = null
            }
        }
    }
    
    @JvmStatic
    fun showRewarded(
        activity: Activity,
        onShown: (() -> Unit)? = null,
        onDismissed: (() -> Unit)? = null,
        onRewardEarned: (String, Int) -> Unit
    ): Boolean {
        onRewardedShownCallback = onShown
        onRewardedDismissedCallback = onDismissed
        onRewardEarnedCallback = onRewardEarned
        
        return if (rewardedAd != null) {
            rewardedAd?.show(activity) { reward ->
                onRewardEarnedCallback?.invoke(reward.type, reward.amount)
            }
            true
        } else {
            false
        }
    }
    
    @JvmStatic
    fun isRewardedReady(): Boolean = rewardedAd != null
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Configuration
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun setTestDeviceIds(deviceIds: List<String>) {
        val configuration = RequestConfiguration.Builder()
            .setTestDeviceIds(deviceIds)
            .build()
        MobileAds.setRequestConfiguration(configuration)
    }
    
    @JvmStatic
    fun setChildDirected(enabled: Boolean) {
        val configuration = RequestConfiguration.Builder()
            .setTagForChildDirectedTreatment(
                if (enabled) RequestConfiguration.TAG_FOR_CHILD_DIRECTED_TREATMENT_TRUE
                else RequestConfiguration.TAG_FOR_CHILD_DIRECTED_TREATMENT_FALSE
            )
            .build()
        MobileAds.setRequestConfiguration(configuration)
    }
    
    @JvmStatic
    fun setMaxAdContentRating(rating: String) {
        val maxRating = when (rating.uppercase()) {
            "G" -> RequestConfiguration.MAX_AD_CONTENT_RATING_G
            "PG" -> RequestConfiguration.MAX_AD_CONTENT_RATING_PG
            "T" -> RequestConfiguration.MAX_AD_CONTENT_RATING_T
            "MA" -> RequestConfiguration.MAX_AD_CONTENT_RATING_MA
            else -> RequestConfiguration.MAX_AD_CONTENT_RATING_G
        }
        
        val configuration = RequestConfiguration.Builder()
            .setMaxAdContentRating(maxRating)
            .build()
        MobileAds.setRequestConfiguration(configuration)
    }
    
    @JvmStatic
    fun setAppMuted(muted: Boolean) {
        MobileAds.setAppMuted(muted)
    }
    
    @JvmStatic
    fun setAppVolume(volume: Float) {
        MobileAds.setAppVolume(volume.coerceIn(0f, 1f))
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Utilities
    // ═══════════════════════════════════════════════════════════════════════════════
    
    private fun getArabicError(code: Int): String {
        return when (code) {
            AdRequest.ERROR_CODE_APP_ID_MISSING -> "معرّف التطبيق مفقود"
            AdRequest.ERROR_CODE_INTERNAL_ERROR -> "خطأ داخلي"
            AdRequest.ERROR_CODE_INVALID_REQUEST -> "طلب غير صالح"
            AdRequest.ERROR_CODE_NETWORK_ERROR -> "خطأ في الشبكة"
            AdRequest.ERROR_CODE_NO_FILL -> "لا توجد إعلانات متاحة"
            else -> "خطأ غير معروف: $code"
        }
    }
    
    @JvmStatic
    fun getTestBannerId(): String = TEST_BANNER_ID
    
    @JvmStatic
    fun getTestInterstitialId(): String = TEST_INTERSTITIAL_ID
    
    @JvmStatic
    fun getTestRewardedId(): String = TEST_REWARDED_ID
}
