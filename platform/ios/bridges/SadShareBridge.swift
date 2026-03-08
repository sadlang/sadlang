// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadShareBridge.swift
// الوصف: جسر المشاركة — iOS (UIActivityViewController)
// ═══════════════════════════════════════════════════════════════════════════════

import UIKit

/// جسر المشاركة لغة ص — مشاركة المحتوى مع تطبيقات أخرى
public final class SadShareBridge {
    
    public static let shared = SadShareBridge()
    private init() {}
    
    // ═════════════════════════════════════════════════════════════════════
    // مشاركة النص
    // ═════════════════════════════════════════════════════════════════════
    
    /// مشاركة نص
    public func shareText(_ text: String) {
        share(items: [text])
    }
    
    /// مشاركة رابط
    public func shareURL(_ urlString: String) {
        guard let url = URL(string: urlString) else { return }
        share(items: [url])
    }
    
    /// مشاركة نص مع رابط
    public func shareTextAndURL(_ text: String, url: String) {
        var items: [Any] = [text]
        if let urlObj = URL(string: url) {
            items.append(urlObj)
        }
        share(items: items)
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // مشاركة الصور
    // ═════════════════════════════════════════════════════════════════════
    
    /// مشاركة صورة
    public func shareImage(_ image: UIImage) {
        share(items: [image])
    }
    
    /// مشاركة صورة من مسار ملف
    public func shareImageFile(_ filePath: String) {
        guard let image = UIImage(contentsOfFile: filePath) else { return }
        share(items: [image])
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // مشاركة الملفات
    // ═════════════════════════════════════════════════════════════════════
    
    /// مشاركة ملف
    public func shareFile(_ filePath: String) {
        let fileURL = URL(fileURLWithPath: filePath)
        share(items: [fileURL])
    }
    
    /// مشاركة عدة ملفات
    public func shareFiles(_ filePaths: [String]) {
        let urls = filePaths.map { URL(fileURLWithPath: $0) }
        share(items: urls)
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // مشاركة مخصصة
    // ═════════════════════════════════════════════════════════════════════
    
    /// مشاركة عناصر متعددة
    public func share(items: [Any], excludedTypes: [UIActivity.ActivityType]? = nil) {
        DispatchQueue.main.async {
            let controller = UIActivityViewController(activityItems: items,
                                                       applicationActivities: nil)
            controller.excludedActivityTypes = excludedTypes
            
            if let viewController = self.topViewController() {
                if let popover = controller.popoverPresentationController {
                    popover.sourceView = viewController.view
                    popover.sourceRect = CGRect(x: viewController.view.bounds.midX,
                                                y: viewController.view.bounds.midY,
                                                width: 0, height: 0)
                }
                viewController.present(controller, animated: true)
            }
        }
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // الحافظة (Clipboard)
    // ═════════════════════════════════════════════════════════════════════
    
    /// نسخ نص إلى الحافظة
    public func copyToClipboard(_ text: String) {
        UIPasteboard.general.string = text
    }
    
    /// لصق من الحافظة
    public func pasteFromClipboard() -> String? {
        UIPasteboard.general.string
    }
    
    /// نسخ صورة إلى الحافظة
    public func copyImageToClipboard(_ image: UIImage) {
        UIPasteboard.general.image = image
    }
    
    /// هل الحافظة تحتوي على نص
    public var hasTextInClipboard: Bool {
        UIPasteboard.general.hasStrings
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // مساعدات
    // ═════════════════════════════════════════════════════════════════════
    
    private func topViewController() -> UIViewController? {
        guard let window = UIApplication.shared.connectedScenes
                .compactMap({ $0 as? UIWindowScene })
                .flatMap({ $0.windows })
                .first(where: { $0.isKeyWindow }) else {
            return nil
        }
        
        var top = window.rootViewController
        while let presented = top?.presentedViewController {
            top = presented
        }
        return top
    }
}
