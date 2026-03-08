// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadWebViewBridge.swift
// الوصف: جسر متصفح الويب — iOS (WKWebView)
// ═══════════════════════════════════════════════════════════════════════════════

import WebKit

/// جسر متصفح الويب لغة ص — عرض محتوى ويب داخل التطبيق
public final class SadWebViewBridge: NSObject {
    
    public static let shared = SadWebViewBridge()
    private override init() { super.init() }
    
    private var webView: WKWebView?
    private var onPageLoaded: ((String) -> Void)?
    private var onError: ((String) -> Void)?
    
    // ═════════════════════════════════════════════════════════════════════
    // التهيئة
    // ═════════════════════════════════════════════════════════════════════
    
    /// إنشاء عرض الويب
    public func createWebView(frame: CGRect) -> WKWebView {
        let config = WKWebViewConfiguration()
        config.allowsInlineMediaPlayback = true
        
        let webView = WKWebView(frame: frame, configuration: config)
        webView.navigationDelegate = self
        webView.allowsBackForwardNavigationGestures = true
        self.webView = webView
        return webView
    }
    
    public func cleanup() {
        webView?.stopLoading()
        webView = nil
        onPageLoaded = nil
        onError = nil
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // التنقل
    // ═════════════════════════════════════════════════════════════════════
    
    /// تحميل عنوان URL
    public func loadURL(_ urlString: String) -> Bool {
        guard let url = URL(string: urlString) else { return false }
        let request = URLRequest(url: url)
        webView?.load(request)
        return true
    }
    
    /// تحميل محتوى HTML
    public func loadHTML(_ html: String, baseURL: URL? = nil) {
        webView?.loadHTMLString(html, baseURL: baseURL)
    }
    
    /// تحميل ملف محلي
    public func loadFile(_ filePath: String) -> Bool {
        let fileURL = URL(fileURLWithPath: filePath)
        let dirURL = fileURL.deletingLastPathComponent()
        webView?.loadFileURL(fileURL, allowingReadAccessTo: dirURL)
        return true
    }
    
    /// الرجوع
    public func goBack() {
        webView?.goBack()
    }
    
    /// التقدم
    public func goForward() {
        webView?.goForward()
    }
    
    /// إعادة التحميل
    public func reload() {
        webView?.reload()
    }
    
    /// إيقاف التحميل
    public func stopLoading() {
        webView?.stopLoading()
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // الحالة
    // ═════════════════════════════════════════════════════════════════════
    
    /// العنوان الحالي
    public var currentURL: String? {
        webView?.url?.absoluteString
    }
    
    /// عنوان الصفحة
    public var pageTitle: String? {
        webView?.title
    }
    
    /// هل يمكن الرجوع
    public var canGoBack: Bool {
        webView?.canGoBack ?? false
    }
    
    /// هل يمكن التقدم
    public var canGoForward: Bool {
        webView?.canGoForward ?? false
    }
    
    /// هل يجري تحميل
    public var isLoading: Bool {
        webView?.isLoading ?? false
    }
    
    /// نسبة التحميل (0.0 - 1.0)
    public var loadingProgress: Double {
        webView?.estimatedProgress ?? 0
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // تنفيذ JavaScript
    // ═════════════════════════════════════════════════════════════════════
    
    /// تنفيذ كود JavaScript
    public func evaluateJS(_ script: String,
                           completion: ((Any?, Error?) -> Void)? = nil) {
        webView?.evaluateJavaScript(script) { result, error in
            DispatchQueue.main.async {
                completion?(result, error)
            }
        }
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // الأحداث
    // ═════════════════════════════════════════════════════════════════════
    
    /// معالج اكتمال تحميل الصفحة
    public func setOnPageLoaded(_ handler: @escaping (String) -> Void) {
        onPageLoaded = handler
    }
    
    /// معالج أخطاء التحميل
    public func setOnError(_ handler: @escaping (String) -> Void) {
        onError = handler
    }
}

// ═════════════════════════════════════════════════════════════════════
// WKNavigationDelegate
// ═════════════════════════════════════════════════════════════════════

extension SadWebViewBridge: WKNavigationDelegate {
    public func webView(_ webView: WKWebView,
                        didFinish navigation: WKNavigation!) {
        let url = webView.url?.absoluteString ?? ""
        onPageLoaded?(url)
    }
    
    public func webView(_ webView: WKWebView,
                        didFail navigation: WKNavigation!,
                        withError error: Error) {
        onError?(error.localizedDescription)
    }
    
    public func webView(_ webView: WKWebView,
                        didFailProvisionalNavigation navigation: WKNavigation!,
                        withError error: Error) {
        onError?(error.localizedDescription)
    }
}
