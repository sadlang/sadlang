// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadWebViewBridge.kt
// الوصف: جسر Kotlin لعرض صفحات الويب
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.annotation.SuppressLint
import android.content.Context
import android.graphics.Bitmap
import android.os.Build
import android.webkit.*
import java.io.ByteArrayOutputStream
import android.util.Base64

/**
 * جسر WebView — SadWebViewBridge
 */
@SuppressLint("SetJavaScriptEnabled")
object SadWebViewBridge {
    
    private var context: Context? = null
    private var webView: WebView? = null
    
    private var onPageStartedCallback: ((String) -> Unit)? = null
    private var onPageFinishedCallback: ((String) -> Unit)? = null
    private var onErrorCallback: ((Int, String) -> Unit)? = null
    private var onProgressCallback: ((Int) -> Unit)? = null
    private var onTitleChangedCallback: ((String) -> Unit)? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    @JvmStatic
    fun createWebView(ctx: Context): WebView {
        val wv = WebView(ctx)
        setupWebView(wv)
        webView = wv
        return wv
    }
    
    @JvmStatic
    fun setupWebView(wv: WebView) {
        webView = wv
        
        wv.settings.apply {
            javaScriptEnabled = true
            domStorageEnabled = true
            databaseEnabled = true
            loadWithOverviewMode = true
            useWideViewPort = true
            builtInZoomControls = true
            displayZoomControls = false
            setSupportZoom(true)
            allowFileAccess = true
            allowContentAccess = true
            
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                mixedContentMode = WebSettings.MIXED_CONTENT_ALWAYS_ALLOW
            }
            
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                safeBrowsingEnabled = true
            }
            
            cacheMode = WebSettings.LOAD_DEFAULT
            defaultTextEncodingName = "UTF-8"
        }
        
        wv.webViewClient = object : WebViewClient() {
            override fun onPageStarted(view: WebView?, url: String?, favicon: Bitmap?) {
                super.onPageStarted(view, url, favicon)
                url?.let { onPageStartedCallback?.invoke(it) }
            }
            
            override fun onPageFinished(view: WebView?, url: String?) {
                super.onPageFinished(view, url)
                url?.let { onPageFinishedCallback?.invoke(it) }
            }
            
            override fun onReceivedError(view: WebView?, request: WebResourceRequest?, error: WebResourceError?) {
                super.onReceivedError(view, request, error)
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                    onErrorCallback?.invoke(error?.errorCode ?: -1, error?.description?.toString() ?: "خطأ غير معروف")
                }
            }
            
            @Deprecated("Deprecated in Java")
            override fun onReceivedError(view: WebView?, errorCode: Int, description: String?, failingUrl: String?) {
                super.onReceivedError(view, errorCode, description, failingUrl)
                onErrorCallback?.invoke(errorCode, description ?: "خطأ غير معروف")
            }
        }
        
        wv.webChromeClient = object : WebChromeClient() {
            override fun onProgressChanged(view: WebView?, newProgress: Int) {
                super.onProgressChanged(view, newProgress)
                onProgressCallback?.invoke(newProgress)
            }
            
            override fun onReceivedTitle(view: WebView?, title: String?) {
                super.onReceivedTitle(view, title)
                title?.let { onTitleChangedCallback?.invoke(it) }
            }
        }
    }
    
    @JvmStatic
    fun loadUrl(url: String) {
        webView?.loadUrl(url)
    }
    
    @JvmStatic
    fun loadHtml(html: String, baseUrl: String? = null) {
        webView?.loadDataWithBaseURL(baseUrl, html, "text/html", "UTF-8", null)
    }
    
    @JvmStatic
    fun loadHtmlArabic(html: String, baseUrl: String? = null) {
        val rtlHtml = """
            <!DOCTYPE html>
            <html dir="rtl" lang="ar">
            <head>
                <meta charset="UTF-8">
                <meta name="viewport" content="width=device-width, initial-scale=1.0">
                <style>
                    body { 
                        direction: rtl; 
                        text-align: right; 
                        font-family: 'Arial', sans-serif;
                    }
                </style>
            </head>
            <body>
                $html
            </body>
            </html>
        """.trimIndent()
        
        webView?.loadDataWithBaseURL(baseUrl, rtlHtml, "text/html", "UTF-8", null)
    }
    
    @JvmStatic
    fun executeJavaScript(script: String) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
            webView?.evaluateJavascript(script, null)
        } else {
            webView?.loadUrl("javascript:$script")
        }
    }
    
    @JvmStatic
    fun executeJavaScript(script: String, callback: (String?) -> Unit) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
            webView?.evaluateJavascript(script) { result ->
                callback(result)
            }
        } else {
            webView?.loadUrl("javascript:$script")
            callback(null)
        }
    }
    
    @JvmStatic
    fun goBack(): Boolean {
        return if (webView?.canGoBack() == true) {
            webView?.goBack()
            true
        } else {
            false
        }
    }
    
    @JvmStatic
    fun goForward(): Boolean {
        return if (webView?.canGoForward() == true) {
            webView?.goForward()
            true
        } else {
            false
        }
    }
    
    @JvmStatic
    fun canGoBack(): Boolean = webView?.canGoBack() == true
    
    @JvmStatic
    fun canGoForward(): Boolean = webView?.canGoForward() == true
    
    @JvmStatic
    fun reload() {
        webView?.reload()
    }
    
    @JvmStatic
    fun stopLoading() {
        webView?.stopLoading()
    }
    
    @JvmStatic
    fun getCurrentUrl(): String? = webView?.url
    
    @JvmStatic
    fun getTitle(): String? = webView?.title
    
    @JvmStatic
    fun getProgress(): Int = webView?.progress ?: 0
    
    @JvmStatic
    fun clearCache(includeDiskFiles: Boolean = true) {
        webView?.clearCache(includeDiskFiles)
    }
    
    @JvmStatic
    fun clearHistory() {
        webView?.clearHistory()
    }
    
    @JvmStatic
    fun clearFormData() {
        webView?.clearFormData()
    }
    
    @JvmStatic
    fun clearAll() {
        clearCache(true)
        clearHistory()
        clearFormData()
        CookieManager.getInstance().removeAllCookies(null)
    }
    
    @JvmStatic
    fun zoomIn(): Boolean = webView?.zoomIn() == true
    
    @JvmStatic
    fun zoomOut(): Boolean = webView?.zoomOut() == true
    
    @JvmStatic
    fun setTextZoom(percent: Int) {
        webView?.settings?.textZoom = percent
    }
    
    @JvmStatic
    fun getTextZoom(): Int = webView?.settings?.textZoom ?: 100
    
    @JvmStatic
    fun setJavaScriptEnabled(enabled: Boolean) {
        webView?.settings?.javaScriptEnabled = enabled
    }
    
    @JvmStatic
    fun setUserAgent(userAgent: String) {
        webView?.settings?.userAgentString = userAgent
    }
    
    @JvmStatic
    fun getUserAgent(): String = webView?.settings?.userAgentString ?: ""
    
    @JvmStatic
    fun setCacheMode(mode: String) {
        webView?.settings?.cacheMode = when (mode.lowercase()) {
            "no_cache" -> WebSettings.LOAD_NO_CACHE
            "cache_only" -> WebSettings.LOAD_CACHE_ONLY
            "cache_else_network" -> WebSettings.LOAD_CACHE_ELSE_NETWORK
            else -> WebSettings.LOAD_DEFAULT
        }
    }
    
    @JvmStatic
    fun captureSnapshot(): Bitmap? {
        val wv = webView ?: return null
        
        return try {
            wv.isDrawingCacheEnabled = true
            wv.buildDrawingCache()
            val bitmap = Bitmap.createBitmap(wv.drawingCache)
            wv.isDrawingCacheEnabled = false
            bitmap
        } catch (e: Exception) {
            null
        }
    }
    
    @JvmStatic
    fun captureSnapshotBase64(): String? {
        val bitmap = captureSnapshot() ?: return null
        val stream = ByteArrayOutputStream()
        bitmap.compress(Bitmap.CompressFormat.PNG, 100, stream)
        val bytes = stream.toByteArray()
        return Base64.encodeToString(bytes, Base64.NO_WRAP)
    }
    
    @JvmStatic
    fun addJavaScriptInterface(obj: Any, name: String) {
        webView?.addJavascriptInterface(obj, name)
    }
    
    @JvmStatic
    fun removeJavaScriptInterface(name: String) {
        webView?.removeJavascriptInterface(name)
    }
    
    @JvmStatic
    fun setOnPageStartedListener(callback: (String) -> Unit) {
        onPageStartedCallback = callback
    }
    
    @JvmStatic
    fun setOnPageFinishedListener(callback: (String) -> Unit) {
        onPageFinishedCallback = callback
    }
    
    @JvmStatic
    fun setOnErrorListener(callback: (Int, String) -> Unit) {
        onErrorCallback = callback
    }
    
    @JvmStatic
    fun setOnProgressListener(callback: (Int) -> Unit) {
        onProgressCallback = callback
    }
    
    @JvmStatic
    fun setOnTitleChangedListener(callback: (String) -> Unit) {
        onTitleChangedCallback = callback
    }
    
    @JvmStatic
    fun getState(): Map<String, Any?> {
        return mapOf(
            "url" to getCurrentUrl(),
            "title" to getTitle(),
            "progress" to getProgress(),
            "canGoBack" to canGoBack(),
            "canGoForward" to canGoForward(),
            "textZoom" to getTextZoom()
        )
    }
    
    @JvmStatic
    fun destroy() {
        webView?.destroy()
        webView = null
    }
}
