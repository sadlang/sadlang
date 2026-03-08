/**
 * =============================================================================
 * ملف: swiftui_async.swift
 * الوصف: مكونات AsyncImage و ProgressView عربية
 * (AR) @brief تحميل الصور غير المتزامن وشريط التقدم لـ iOS
 * (EN) @brief Async image loading and progress views for iOS
 * المهمة: T402 - أحمد: AsyncImage + ProgressView
 * =============================================================================
 */

import SwiftUI

// MARK: - ═══════════════════════════════════════════════════════════════════
// القسم الأول: تحميل الصور غير المتزامن (AsyncImage)
// ═══════════════════════════════════════════════════════════════════════════

/// (AR) صورة غير متزامنة عربية — تحميل صور من الإنترنت مع دعم RTL
/// (EN) Arabic async image — loads images from URL with RTL support
struct ArabicAsyncImage: View {
    let url: URL?
    let placeholder: String
    let errorImage: String
    let cornerRadius: CGFloat
    let contentMode: ContentMode
    
    init(url: URL?,
         placeholder: String = "photo",
         errorImage: String = "exclamationmark.triangle",
         cornerRadius: CGFloat = 8,
         contentMode: ContentMode = .fit) {
        self.url = url
        self.placeholder = placeholder
        self.errorImage = errorImage
        self.cornerRadius = cornerRadius
        self.contentMode = contentMode
    }
    
    var body: some View {
        AsyncImage(url: url) { phase in
            switch phase {
            case .empty:
                // (AR) عرض عنصر نائب أثناء التحميل
                ZStack {
                    Color.gray.opacity(0.1)
                    VStack(spacing: 8) {
                        ProgressView()
                            .progressViewStyle(CircularProgressViewStyle())
                        Text("جاري التحميل...")
                            .font(.caption)
                            .foregroundColor(.secondary)
                    }
                }
                .cornerRadius(cornerRadius)
                
            case .success(let image):
                // (AR) عرض الصورة بنجاح
                image
                    .resizable()
                    .aspectRatio(contentMode: contentMode)
                    .cornerRadius(cornerRadius)
                
            case .failure:
                // (AR) عرض رسالة خطأ
                ZStack {
                    Color.red.opacity(0.1)
                    VStack(spacing: 8) {
                        Image(systemName: errorImage)
                            .font(.largeTitle)
                            .foregroundColor(.red)
                        Text("فشل التحميل")
                            .font(.caption)
                            .foregroundColor(.secondary)
                    }
                }
                .cornerRadius(cornerRadius)
                
            @unknown default:
                Image(systemName: placeholder)
                    .foregroundColor(.gray)
            }
        }
    }
}

/// (AR) صورة دائرية غير متزامنة (للصور الشخصية)
struct ArabicCircleAsyncImage: View {
    let url: URL?
    let size: CGFloat
    let borderColor: Color
    let borderWidth: CGFloat
    
    init(url: URL?, size: CGFloat = 60, borderColor: Color = .blue, borderWidth: CGFloat = 2) {
        self.url = url
        self.size = size
        self.borderColor = borderColor
        self.borderWidth = borderWidth
    }
    
    var body: some View {
        AsyncImage(url: url) { phase in
            switch phase {
            case .empty:
                Circle()
                    .fill(Color.gray.opacity(0.2))
                    .frame(width: size, height: size)
                    .overlay(ProgressView())
                
            case .success(let image):
                image
                    .resizable()
                    .aspectRatio(contentMode: .fill)
                    .frame(width: size, height: size)
                    .clipShape(Circle())
                    .overlay(Circle().stroke(borderColor, lineWidth: borderWidth))
                
            case .failure:
                Circle()
                    .fill(Color.gray.opacity(0.2))
                    .frame(width: size, height: size)
                    .overlay(
                        Image(systemName: "person.fill")
                            .foregroundColor(.gray)
                    )
                
            @unknown default:
                EmptyView()
            }
        }
    }
}

/// (AR) شبكة صور غير متزامنة
struct ArabicAsyncImageGrid: View {
    let urls: [URL]
    let columns: Int
    let spacing: CGFloat
    
    init(urls: [URL], columns: Int = 3, spacing: CGFloat = 4) {
        self.urls = urls
        self.columns = columns
        self.spacing = spacing
    }
    
    var body: some View {
        let gridItems = Array(repeating: GridItem(.flexible(), spacing: spacing), count: columns)
        
        LazyVGrid(columns: gridItems, spacing: spacing) {
            ForEach(urls, id: \.absoluteString) { url in
                ArabicAsyncImage(url: url, cornerRadius: 4, contentMode: .fill)
                    .frame(minHeight: 100)
                    .clipped()
            }
        }
    }
}

// MARK: - ═══════════════════════════════════════════════════════════════════
// القسم الثاني: مؤشرات التقدم (ProgressView)
// ═══════════════════════════════════════════════════════════════════════════

/// (AR) شريط تقدم عربي خطي
struct ArabicLinearProgress: View {
    let value: Double  // 0.0 - 1.0
    let label: String
    let showPercentage: Bool
    let tintColor: Color
    let trackColor: Color
    
    init(value: Double, label: String = "",
         showPercentage: Bool = true,
         tintColor: Color = .blue,
         trackColor: Color = .gray.opacity(0.2)) {
        self.value = min(max(value, 0), 1)
        self.label = label
        self.showPercentage = showPercentage
        self.tintColor = tintColor
        self.trackColor = trackColor
    }
    
    var body: some View {
        VStack(alignment: .trailing, spacing: 4) {
            if !label.isEmpty || showPercentage {
                HStack {
                    if showPercentage {
                        Text("\(Int(value * 100))%")
                            .font(.caption)
                            .foregroundColor(.secondary)
                    }
                    Spacer()
                    if !label.isEmpty {
                        Text(label)
                            .font(.caption)
                            .foregroundColor(.primary)
                    }
                }
            }
            
            ProgressView(value: value)
                .progressViewStyle(LinearProgressViewStyle(tint: tintColor))
                .background(trackColor)
                .cornerRadius(4)
        }
        .environment(\.layoutDirection, .rightToLeft)
    }
}

/// (AR) مؤشر تحميل دائري عربي
struct ArabicCircularProgress: View {
    let label: String
    let isAnimating: Bool
    let size: CGFloat
    let lineWidth: CGFloat
    let tintColor: Color
    
    init(label: String = "جاري التحميل...",
         isAnimating: Bool = true,
         size: CGFloat = 40,
         lineWidth: CGFloat = 3,
         tintColor: Color = .blue) {
        self.label = label
        self.isAnimating = isAnimating
        self.size = size
        self.lineWidth = lineWidth
        self.tintColor = tintColor
    }
    
    var body: some View {
        VStack(spacing: 12) {
            if isAnimating {
                ProgressView()
                    .progressViewStyle(CircularProgressViewStyle(tint: tintColor))
                    .scaleEffect(size / 20)
            }
            if !label.isEmpty {
                Text(label)
                    .font(.caption)
                    .foregroundColor(.secondary)
            }
        }
    }
}

/// (AR) شريط تقدم متدرج
struct ArabicGradientProgress: View {
    let value: Double
    let label: String
    let colors: [Color]
    let height: CGFloat
    
    init(value: Double, label: String = "",
         colors: [Color] = [.blue, .purple],
         height: CGFloat = 8) {
        self.value = min(max(value, 0), 1)
        self.label = label
        self.colors = colors
        self.height = height
    }
    
    var body: some View {
        VStack(alignment: .trailing, spacing: 4) {
            if !label.isEmpty {
                HStack {
                    Text("\(Int(value * 100))%")
                        .font(.caption2)
                        .foregroundColor(.secondary)
                    Spacer()
                    Text(label)
                        .font(.caption)
                }
            }
            
            GeometryReader { geometry in
                ZStack(alignment: .trailing) {
                    RoundedRectangle(cornerRadius: height / 2)
                        .fill(Color.gray.opacity(0.2))
                    
                    RoundedRectangle(cornerRadius: height / 2)
                        .fill(LinearGradient(colors: colors, startPoint: .leading, endPoint: .trailing))
                        .frame(width: geometry.size.width * value)
                        .animation(.easeInOut(duration: 0.3), value: value)
                }
            }
            .frame(height: height)
        }
        .environment(\.layoutDirection, .rightToLeft)
    }
}

/// (AR) مؤشر تقدم خطوات
struct ArabicStepProgress: View {
    let steps: [String]
    let currentStep: Int
    let completedColor: Color
    let activeColor: Color
    let inactiveColor: Color
    
    init(steps: [String], currentStep: Int,
         completedColor: Color = .green,
         activeColor: Color = .blue,
         inactiveColor: Color = .gray.opacity(0.3)) {
        self.steps = steps
        self.currentStep = currentStep
        self.completedColor = completedColor
        self.activeColor = activeColor
        self.inactiveColor = inactiveColor
    }
    
    var body: some View {
        HStack(spacing: 0) {
            ForEach(Array(steps.enumerated().reversed()), id: \.offset) { index, step in
                VStack(spacing: 4) {
                    ZStack {
                        Circle()
                            .fill(colorForStep(index))
                            .frame(width: 30, height: 30)
                        
                        if index < currentStep {
                            Image(systemName: "checkmark")
                                .font(.caption)
                                .foregroundColor(.white)
                        } else {
                            Text("\(index + 1)")
                                .font(.caption)
                                .foregroundColor(index == currentStep ? .white : .gray)
                        }
                    }
                    
                    Text(step)
                        .font(.caption2)
                        .multilineTextAlignment(.center)
                        .frame(maxWidth: 60)
                }
                
                if index > 0 {
                    Rectangle()
                        .fill(index <= currentStep ? completedColor : inactiveColor)
                        .frame(height: 2)
                        .frame(maxWidth: .infinity)
                }
            }
        }
        .padding(.horizontal)
    }
    
    private func colorForStep(_ index: Int) -> Color {
        if index < currentStep { return completedColor }
        if index == currentStep { return activeColor }
        return inactiveColor
    }
}

/// (AR) شريط تحميل ملف
struct ArabicFileUploadProgress: View {
    let fileName: String
    let progress: Double
    let fileSize: String
    let status: UploadStatus
    
    enum UploadStatus {
        case uploading
        case completed
        case failed
        case paused
        
        var icon: String {
            switch self {
            case .uploading: return "arrow.up.circle"
            case .completed: return "checkmark.circle.fill"
            case .failed: return "xmark.circle.fill"
            case .paused: return "pause.circle"
            }
        }
        
        var color: Color {
            switch self {
            case .uploading: return .blue
            case .completed: return .green
            case .failed: return .red
            case .paused: return .orange
            }
        }
        
        var label: String {
            switch self {
            case .uploading: return "جاري الرفع..."
            case .completed: return "تم الرفع"
            case .failed: return "فشل الرفع"
            case .paused: return "متوقف مؤقتاً"
            }
        }
    }
    
    var body: some View {
        VStack(spacing: 8) {
            HStack {
                Image(systemName: status.icon)
                    .foregroundColor(status.color)
                
                VStack(alignment: .trailing, spacing: 2) {
                    Text(fileName)
                        .font(.subheadline)
                        .lineLimit(1)
                    Text("\(fileSize) • \(status.label)")
                        .font(.caption)
                        .foregroundColor(.secondary)
                }
                .frame(maxWidth: .infinity, alignment: .trailing)
            }
            
            if status == .uploading {
                ArabicLinearProgress(
                    value: progress,
                    showPercentage: true,
                    tintColor: .blue
                )
            }
        }
        .padding()
        .background(Color(.systemBackground))
        .cornerRadius(12)
        .shadow(radius: 1)
        .environment(\.layoutDirection, .rightToLeft)
    }
}

// MARK: - ═══════════════════════════════════════════════════════════════════
// القسم الثالث: ربط مع لغة ص
// ═══════════════════════════════════════════════════════════════════════════

/// (AR) خريطة ربط أوامر لغة ص بمكونات AsyncImage و ProgressView
struct SadAsyncBindings {
    /// صورة_من_رابط("https://...")       →  ArabicAsyncImage(url: ...)
    /// صورة_شخصية(رابط: "...")           →  ArabicCircleAsyncImage(url: ...)
    /// شبكة_صور(روابط: [...])            →  ArabicAsyncImageGrid(urls: ...)
    /// شريط_تقدم(قيمة: 0.5, عنوان: "..") →  ArabicLinearProgress(value: ...)
    /// تحميل_دائري(عنوان: "...")          →  ArabicCircularProgress(label: ...)
    /// تقدم_خطوات(خطوات: [...], حالي: 1)  →  ArabicStepProgress(...)
    /// رفع_ملف(اسم: "...", تقدم: 0.7)     →  ArabicFileUploadProgress(...)
    
    static let sadToSwiftUI: [String: String] = [
        "صورة_من_رابط": "ArabicAsyncImage",
        "صورة_شخصية": "ArabicCircleAsyncImage",
        "شبكة_صور": "ArabicAsyncImageGrid",
        "شريط_تقدم": "ArabicLinearProgress",
        "تحميل_دائري": "ArabicCircularProgress",
        "تقدم_متدرج": "ArabicGradientProgress",
        "تقدم_خطوات": "ArabicStepProgress",
        "رفع_ملف": "ArabicFileUploadProgress"
    ]
}
