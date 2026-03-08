/**
 * =============================================================================
 * ملف: swiftui_modals.swift
 * الوصف: مكونات SwiftUI للنوافذ المنبثقة (Sheet, Alert, ActionSheet)
 * (AR) @brief مكتبة نوافذ منبثقة عربية لواجهات iOS
 * (EN) @brief Arabic modal components for iOS SwiftUI interfaces
 * المهمة: T401 - أحمد: SwiftUI Sheet+Alert+ActionSheet
 * =============================================================================
 */

import SwiftUI

// MARK: - ═══════════════════════════════════════════════════════════════════
// القسم الأول: نظام النوافذ المنبثقة (Sheet System)
// ═══════════════════════════════════════════════════════════════════════════

/// (AR) مدير النوافذ المنبثقة — يتحكم في عرض وإخفاء النوافذ
/// (EN) Modal manager — controls showing/hiding modals
class ModalManager: ObservableObject {
    @Published var activeSheet: SheetType?
    @Published var showAlert: Bool = false
    @Published var showActionSheet: Bool = false
    @Published var alertConfig: AlertConfig?
    @Published var actionSheetConfig: ActionSheetConfig?
    
    /// (AR) عرض نافذة منبثقة
    func showSheet(_ type: SheetType) {
        activeSheet = type
    }
    
    /// (AR) إغلاق النافذة المنبثقة
    func dismissSheet() {
        activeSheet = nil
    }
    
    /// (AR) عرض تنبيه
    func presentAlert(_ config: AlertConfig) {
        alertConfig = config
        showAlert = true
    }
    
    /// (AR) عرض ورقة إجراءات
    func presentActionSheet(_ config: ActionSheetConfig) {
        actionSheetConfig = config
        showActionSheet = true
    }
}

// MARK: - أنواع النوافذ
enum SheetType: Identifiable {
    case form(title: String)
    case detail(itemId: String)
    case settings
    case picker(options: [String])
    case custom(content: String)
    
    var id: String {
        switch self {
        case .form(let title): return "form_\(title)"
        case .detail(let id): return "detail_\(id)"
        case .settings: return "settings"
        case .picker: return "picker"
        case .custom(let content): return "custom_\(content)"
        }
    }
}

// MARK: - ═══════════════════════════════════════════════════════════════════
// القسم الثاني: تنبيهات (Alert System)
// ═══════════════════════════════════════════════════════════════════════════

/// (AR) إعدادات التنبيه
struct AlertConfig {
    let title: String
    let message: String
    let primaryButton: AlertButtonConfig
    let secondaryButton: AlertButtonConfig?
    
    /// (AR) تنبيه بسيط بزر واحد
    static func simple(title: String, message: String, buttonText: String = "حسناً", action: (() -> Void)? = nil) -> AlertConfig {
        AlertConfig(
            title: title,
            message: message,
            primaryButton: AlertButtonConfig(text: buttonText, style: .default, action: action),
            secondaryButton: nil
        )
    }
    
    /// (AR) تنبيه تأكيد بزرين
    static func confirmation(title: String, message: String,
                            confirmText: String = "تأكيد",
                            cancelText: String = "إلغاء",
                            onConfirm: (() -> Void)? = nil,
                            onCancel: (() -> Void)? = nil) -> AlertConfig {
        AlertConfig(
            title: title,
            message: message,
            primaryButton: AlertButtonConfig(text: confirmText, style: .destructive, action: onConfirm),
            secondaryButton: AlertButtonConfig(text: cancelText, style: .cancel, action: onCancel)
        )
    }
    
    /// (AR) تنبيه حذف
    static func delete(itemName: String, onDelete: (() -> Void)? = nil) -> AlertConfig {
        confirmation(
            title: "حذف \(itemName)",
            message: "هل أنت متأكد من حذف \(itemName)؟ لا يمكن التراجع عن هذا الإجراء.",
            confirmText: "حذف",
            cancelText: "إلغاء",
            onConfirm: onDelete
        )
    }
}

struct AlertButtonConfig {
    let text: String
    let style: AlertButtonStyle
    let action: (() -> Void)?
    
    enum AlertButtonStyle {
        case `default`
        case cancel
        case destructive
    }
}

// MARK: - ═══════════════════════════════════════════════════════════════════
// القسم الثالث: ورقة الإجراءات (ActionSheet System)
// ═══════════════════════════════════════════════════════════════════════════

/// (AR) إعدادات ورقة الإجراءات
struct ActionSheetConfig {
    let title: String
    let message: String?
    let buttons: [ActionSheetButton]
    
    struct ActionSheetButton {
        let text: String
        let style: ButtonStyle
        let action: (() -> Void)?
        
        enum ButtonStyle {
            case `default`
            case cancel
            case destructive
        }
    }
    
    /// (AR) ورقة مشاركة
    static func share(onShare: ((String) -> Void)? = nil) -> ActionSheetConfig {
        ActionSheetConfig(
            title: "مشاركة",
            message: "اختر طريقة المشاركة",
            buttons: [
                ActionSheetButton(text: "نسخ الرابط", style: .default, action: { onShare?("copy") }),
                ActionSheetButton(text: "مشاركة عبر الرسائل", style: .default, action: { onShare?("messages") }),
                ActionSheetButton(text: "مشاركة عبر البريد", style: .default, action: { onShare?("email") }),
                ActionSheetButton(text: "إلغاء", style: .cancel, action: nil)
            ]
        )
    }
    
    /// (AR) ورقة خيارات الصورة
    static func imagePicker(onSelect: ((String) -> Void)? = nil) -> ActionSheetConfig {
        ActionSheetConfig(
            title: "اختر صورة",
            message: nil,
            buttons: [
                ActionSheetButton(text: "الكاميرا", style: .default, action: { onSelect?("camera") }),
                ActionSheetButton(text: "مكتبة الصور", style: .default, action: { onSelect?("library") }),
                ActionSheetButton(text: "الملفات", style: .default, action: { onSelect?("files") }),
                ActionSheetButton(text: "إلغاء", style: .cancel, action: nil)
            ]
        )
    }
    
    /// (AR) ورقة حذف
    static func deleteOptions(onDelete: (() -> Void)? = nil) -> ActionSheetConfig {
        ActionSheetConfig(
            title: "حذف العنصر",
            message: "هذا الإجراء لا يمكن التراجع عنه",
            buttons: [
                ActionSheetButton(text: "حذف نهائياً", style: .destructive, action: onDelete),
                ActionSheetButton(text: "إلغاء", style: .cancel, action: nil)
            ]
        )
    }
}

// MARK: - ═══════════════════════════════════════════════════════════════════
// القسم الرابع: مكونات SwiftUI
// ═══════════════════════════════════════════════════════════════════════════

/// (AR) نافذة نموذج عربية
struct ArabicFormSheet: View {
    let title: String
    let onDismiss: () -> Void
    @State private var fieldValues: [String: String] = [:]
    let fields: [(label: String, placeholder: String)]
    
    var body: some View {
        NavigationView {
            Form {
                ForEach(fields, id: \.label) { field in
                    Section(header: Text(field.label)) {
                        TextField(field.placeholder, text: binding(for: field.label))
                            .environment(\.layoutDirection, .rightToLeft)
                    }
                }
            }
            .navigationTitle(title)
            .navigationBarTitleDisplayMode(.inline)
            .toolbar {
                ToolbarItem(placement: .cancellationAction) {
                    Button("إلغاء") { onDismiss() }
                }
                ToolbarItem(placement: .confirmationAction) {
                    Button("حفظ") { onDismiss() }
                }
            }
        }
        .environment(\.layoutDirection, .rightToLeft)
    }
    
    private func binding(for key: String) -> Binding<String> {
        Binding(
            get: { fieldValues[key, default: ""] },
            set: { fieldValues[key] = $0 }
        )
    }
}

/// (AR) نافذة تفاصيل عربية
struct ArabicDetailSheet: View {
    let itemId: String
    let title: String
    let details: [(label: String, value: String)]
    let onDismiss: () -> Void
    
    var body: some View {
        NavigationView {
            List {
                ForEach(details, id: \.label) { detail in
                    HStack {
                        Text(detail.value)
                            .frame(maxWidth: .infinity, alignment: .leading)
                        Text(detail.label)
                            .foregroundColor(.secondary)
                    }
                }
            }
            .navigationTitle(title)
            .navigationBarTitleDisplayMode(.inline)
            .toolbar {
                ToolbarItem(placement: .cancellationAction) {
                    Button("إغلاق") { onDismiss() }
                }
            }
        }
        .environment(\.layoutDirection, .rightToLeft)
    }
}

/// (AR) نافذة إعدادات عربية
struct ArabicSettingsSheet: View {
    let onDismiss: () -> Void
    @State private var darkMode = false
    @State private var notifications = true
    @State private var fontSize: Double = 16
    @State private var language = "العربية"
    
    var body: some View {
        NavigationView {
            Form {
                Section(header: Text("المظهر")) {
                    Toggle("الوضع الداكن", isOn: $darkMode)
                    HStack {
                        Text("حجم الخط")
                        Slider(value: $fontSize, in: 12...24, step: 1)
                        Text("\(Int(fontSize))")
                    }
                }
                Section(header: Text("الإشعارات")) {
                    Toggle("تفعيل الإشعارات", isOn: $notifications)
                }
                Section(header: Text("اللغة")) {
                    Picker("اللغة", selection: $language) {
                        Text("العربية").tag("العربية")
                        Text("English").tag("English")
                    }
                }
            }
            .navigationTitle("الإعدادات")
            .navigationBarTitleDisplayMode(.inline)
            .toolbar {
                ToolbarItem(placement: .confirmationAction) {
                    Button("تم") { onDismiss() }
                }
            }
        }
        .environment(\.layoutDirection, .rightToLeft)
    }
}

/// (AR) نافذة اختيار عربية
struct ArabicPickerSheet: View {
    let title: String
    let options: [String]
    let onSelect: (String) -> Void
    let onDismiss: () -> Void
    @State private var selected: String = ""
    
    var body: some View {
        NavigationView {
            List {
                ForEach(options, id: \.self) { option in
                    Button(action: {
                        selected = option
                        onSelect(option)
                        onDismiss()
                    }) {
                        HStack {
                            if selected == option {
                                Image(systemName: "checkmark")
                                    .foregroundColor(.blue)
                            }
                            Text(option)
                                .frame(maxWidth: .infinity, alignment: .trailing)
                        }
                    }
                    .foregroundColor(.primary)
                }
            }
            .navigationTitle(title)
            .navigationBarTitleDisplayMode(.inline)
            .toolbar {
                ToolbarItem(placement: .cancellationAction) {
                    Button("إلغاء") { onDismiss() }
                }
            }
        }
        .environment(\.layoutDirection, .rightToLeft)
    }
}

// MARK: - ═══════════════════════════════════════════════════════════════════
// القسم الخامس: View Modifiers للتكامل مع لغة ص
// ═══════════════════════════════════════════════════════════════════════════

/// (AR) معدّل تنبيه عربي
struct ArabicAlertModifier: ViewModifier {
    @Binding var isPresented: Bool
    let config: AlertConfig?
    
    func body(content: Content) -> some View {
        content.alert(isPresented: $isPresented) {
            if let config = config {
                if let secondary = config.secondaryButton {
                    return Alert(
                        title: Text(config.title),
                        message: Text(config.message),
                        primaryButton: alertButton(config.primaryButton),
                        secondaryButton: alertButton(secondary)
                    )
                } else {
                    return Alert(
                        title: Text(config.title),
                        message: Text(config.message),
                        dismissButton: alertButton(config.primaryButton)
                    )
                }
            }
            return Alert(title: Text(""))
        }
    }
    
    private func alertButton(_ config: AlertButtonConfig) -> Alert.Button {
        switch config.style {
        case .default: return .default(Text(config.text), action: config.action)
        case .cancel: return .cancel(Text(config.text), action: config.action)
        case .destructive: return .destructive(Text(config.text), action: config.action)
        }
    }
}

extension View {
    /// (AR) إضافة تنبيه عربي
    func arabicAlert(isPresented: Binding<Bool>, config: AlertConfig?) -> some View {
        modifier(ArabicAlertModifier(isPresented: isPresented, config: config))
    }
}

// MARK: - ═══════════════════════════════════════════════════════════════════
// القسم السادس: رموز لغة ص → SwiftUI
// ═══════════════════════════════════════════════════════════════════════════

/// (AR) خريطة ربط أوامر لغة ص بمكونات SwiftUI
/// (EN) Mapping Sad language commands to SwiftUI components
struct SadModalBindings {
    /// اعرض_نافذة("نموذج", عنوان: "...")  →  .sheet { ArabicFormSheet }
    /// اعرض_تنبيه(عنوان: "...", رسالة: "...")  →  .alert { ... }
    /// اعرض_خيارات(عنوان: "...", خيارات: [...])  →  .actionSheet { ... }
    
    static let sadToSwiftUI: [String: String] = [
        "اعرض_نافذة": ".sheet",
        "اعرض_تنبيه": ".alert",
        "اعرض_خيارات": ".actionSheet",
        "اغلق_نافذة": "dismiss()",
        "نافذة_نموذج": "ArabicFormSheet",
        "نافذة_تفاصيل": "ArabicDetailSheet",
        "نافذة_إعدادات": "ArabicSettingsSheet",
        "نافذة_اختيار": "ArabicPickerSheet"
    ]
}
