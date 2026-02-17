/**
 * @file mobile_media.h
 * @brief مركبات الوسائط والرسوم المتحركة للتطبيقات المحمولة
 * 
 * يحتوي على 10 مركبات:
 * - مشغل الفيديو VideoPlayer
 * - عرض الصور ImageViewer
 * - معرض الصور PhotoGallery
 * - الكاميرا CameraView
 * - مؤشر تحميل الرفع UploadProgress
 * - رسم متحرك AnimatedWidget
 * - انتقال بين المركبات AnimatedSwitcher
 * - تأثير التموج RippleEffect
 * - تأثير التلاشي PageTransition
 * - مؤشرات لمس GestureDetector
 * 
 * @author فريق لغة ص
 * @date 2026
 */
#pragma once

#include "mobile_core.h"
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <cmath>
#include <ctime>
#include <SDL.h>

namespace sad {
namespace graphics {
namespace ui {
namespace mobile {

// ═══════════════════════════════════════════════════════════════
// 1. مشغل الفيديو - VideoPlayer
// ═══════════════════════════════════════════════════════════════

/**
 * @class VideoPlayer
 * @brief مشغل فيديو مدمج - لتشغيل مقاطع الفيديو
 * 
 * يوفر:
 * - أزرار تشغيل/إيقاف/تقديم/ترجيع
 * - شريط تقدم قابل للسحب
 * - ملء الشاشة
 * - التحكم بالصوت
 * - عنوان الفرعي (ترجمة)
 */
class VideoPlayer : public Widget {
public:
    std::string source;             // مسار أو رابط الفيديو
    bool isPlaying = false;
    bool isMuted = false;
    bool isFullscreen = false;
    float volume = 1.0f;           // 0.0 - 1.0
    float currentTime = 0.0f;      // الوقت الحالي (ثانية)
    float duration = 0.0f;         // المدة الكلية
    float playbackSpeed = 1.0f;    // سرعة التشغيل
    bool showControls = true;
    bool autoPlay = false;
    bool loop = false;
    float controlsFadeTime = 3.0f; // إخفاء شريط التحكم (ثوان)
    Color controlsBg = Color(0, 0, 0, 0.5f);
    float aspectRatio = 16.0f / 9.0f;

    // --- الترجمة ---
    bool subtitlesEnabled = false;
    std::string subtitleText;
    Color subtitleBg = Color(0, 0, 0, 0.7f);
    Color subtitleColor = Color(1, 1, 1, 1);
    float subtitleFontSize = 16.0f;

    // --- الأحداث ---
    std::function<void()> onPlay;
    std::function<void()> onPause;
    std::function<void()> onEnded;
    std::function<void(float)> onTimeUpdate;
    std::function<void(float)> onSeek;
    std::function<void(bool)> onFullscreen;

    void play() {
        isPlaying = true;
        if (onPlay) onPlay();
    }

    void pause() {
        isPlaying = false;
        if (onPause) onPause();
    }

    void togglePlay() { isPlaying ? pause() : play(); }

    void seek(float time) {
        currentTime = std::max(0.0f, std::min(time, duration));
        if (onSeek) onSeek(currentTime);
    }

    void toggleFullscreen() {
        isFullscreen = !isFullscreen;
        if (onFullscreen) onFullscreen(isFullscreen);
    }

    std::string formatTime(float seconds) const {
        int mins = (int)seconds / 60;
        int secs = (int)seconds % 60;
        return std::to_string(mins) + ":" + (secs < 10 ? "0" : "") + std::to_string(secs);
    }

    void update(float dt) {
        if (isPlaying) {
            currentTime += dt * playbackSpeed;
            if (currentTime >= duration) {
                if (loop) currentTime = 0;
                else {
                    currentTime = duration;
                    isPlaying = false;
                    if (onEnded) onEnded();
                }
            }
            if (onTimeUpdate) onTimeUpdate(currentTime);
        }
    }

    void render(Renderer2D& renderer) override {
        float h = size.x / aspectRatio;

        // رسم منطقة الفيديو (أسود)
        renderer.fillRect(position.x, position.y, size.x, h, Color(0, 0, 0, 1));

        // شريط التحكم
        if (showControls) {
            float ctrlY = position.y + h - 40;
            renderer.fillRect(position.x, ctrlY, size.x, 40, controlsBg);

            // زر تشغيل/إيقاف
            std::string playIcon = isPlaying ? "⏸" : "▶";
            renderer.drawText(playIcon, position.x + 20, ctrlY + 20, Color(1, 1, 1, 1));

            // الوقت
            std::string timeText = formatTime(currentTime) + " / " + formatTime(duration);
            renderer.drawText(timeText, position.x + 50, ctrlY + 20, Color(1, 1, 1, 1));

            // شريط التقدم
            float progressX = position.x + 140;
            float progressW = size.x - 240;
            renderer.fillRect(progressX, ctrlY + 18, progressW, 4,
                            Color(0.5f, 0.5f, 0.5f, 1));
            float progress = duration > 0 ? currentTime / duration : 0;
            renderer.fillRect(progressX, ctrlY + 18, progressW * progress, 4,
                            Color(1, 0.2f, 0.2f, 1));

            // زر ملء الشاشة
            renderer.drawText("⛶", position.x + size.x - 30, ctrlY + 20,
                            Color(1, 1, 1, 1));
        }

        // الترجمة
        if (subtitlesEnabled && !subtitleText.empty()) {
            float stY = position.y + h - 80;
            float stW = subtitleText.length() * subtitleFontSize * 0.5f + 20;
            renderer.fillRect(position.x + (size.x - stW) / 2, stY, stW, 28, subtitleBg);
            renderer.drawText(subtitleText, position.x + size.x / 2, stY + 14,
                            subtitleColor);
        }
    }

    void handleEvent(const SDL_Event& event) override {
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            togglePlay();
        }
    }
};

// ═══════════════════════════════════════════════════════════════
// 2. عرض الصور - ImageViewer
// ═══════════════════════════════════════════════════════════════

/**
 * @class ImageViewer
 * @brief عارض صور تفاعلي - لعرض الصور بالحجم الكامل
 * 
 * يوفر:
 * - تكبير وتصغير بالقرص
 * - تحريك بالسحب
 * - نقر مزدوج للتكبير/التصغير
 * - تمرير بين صور متعددة
 */
class ImageViewer : public Widget {
public:
    std::vector<std::string> images;   // قائمة مسارات الصور
    int currentIndex = 0;
    float zoom = 1.0f;             // مستوى التكبير
    float minZoom = 0.5f;
    float maxZoom = 5.0f;
    Vec2 panOffset = Vec2(0, 0);   // إزاحة التحريك
    bool isVisible = false;
    Color backgroundColor = Color(0, 0, 0, 0.95f);
    bool showCounter = true;       // عداد الصور (1/5)
    bool showCloseButton = true;
    bool enableZoom = true;
    bool enableSwipe = true;       // التمرير بين الصور

    // --- التحريك ---
    float animProgress = 0.0f;
    bool isPinching = false;
    float pinchStartZoom = 1.0f;

    // --- الأحداث ---
    std::function<void()> onClose;
    std::function<void(int)> onPageChanged;

    void show(int index = 0) {
        currentIndex = index;
        zoom = 1.0f;
        panOffset = Vec2(0, 0);
        isVisible = true;
    }

    void close() {
        isVisible = false;
        if (onClose) onClose();
    }

    void zoomIn() { zoom = std::min(maxZoom, zoom * 1.5f); }
    void zoomOut() { zoom = std::max(minZoom, zoom / 1.5f); }
    void resetZoom() { zoom = 1.0f; panOffset = Vec2(0, 0); }

    void nextImage() {
        if (currentIndex < (int)images.size() - 1) {
            currentIndex++;
            resetZoom();
            if (onPageChanged) onPageChanged(currentIndex);
        }
    }

    void previousImage() {
        if (currentIndex > 0) {
            currentIndex--;
            resetZoom();
            if (onPageChanged) onPageChanged(currentIndex);
        }
    }

    void update(float dt) {
        if (isVisible && animProgress < 1.0f)
            animProgress = std::min(1.0f, animProgress + dt * 6);
        else if (!isVisible && animProgress > 0)
            animProgress = std::max(0.0f, animProgress - dt * 6);
    }

    void render(Renderer2D& renderer) override {
        if (animProgress <= 0) return;

        Color bg = backgroundColor;
        bg.a *= animProgress;
        renderer.fillRect(0, 0, size.x, size.y, bg);

        // رسم الصورة (placeholder)
        float imgW = size.x * 0.8f * zoom;
        float imgH = size.y * 0.8f * zoom;
        float imgX = (size.x - imgW) / 2 + panOffset.x;
        float imgY = (size.y - imgH) / 2 + panOffset.y;
        renderer.fillRect(imgX, imgY, imgW, imgH, Color(0.3f, 0.3f, 0.3f, animProgress));

        // عداد الصور
        if (showCounter && images.size() > 1) {
            std::string counter = std::to_string(currentIndex + 1) + " / " +
                                 std::to_string(images.size());
            renderer.drawText(counter, size.x / 2, 40, Color(1, 1, 1, animProgress));
        }

        // زر الإغلاق
        if (showCloseButton) {
            renderer.drawText("✕", size.x - 40, 30, Color(1, 1, 1, animProgress));
        }
    }

    void handleEvent(const SDL_Event& event) override {
        if (!isVisible) return;
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.button.x > size.x - 60 && event.button.y < 60) close();
        }
        if (event.type == SDL_MOUSEWHEEL && enableZoom) {
            zoom *= (event.wheel.y > 0) ? 1.2f : 0.8f;
            zoom = std::max(minZoom, std::min(maxZoom, zoom));
        }
    }
};

// ═══════════════════════════════════════════════════════════════
// 3. معرض الصور - PhotoGallery
// ═══════════════════════════════════════════════════════════════

/**
 * @class PhotoGallery
 * @brief معرض صور شبكي - لعرض مجموعة صور بشكل شبكي
 * 
 * يوفر:
 * - شبكة صور قابلة للتمرير
 * - تحديد متعدد
 * - فتح الصورة في عارض ملء الشاشة
 * - تحميل كسول للصور
 */
class PhotoGallery : public Widget {
public:
    struct Photo {
        std::string path;
        std::string thumbnail;     // صورة مصغرة
        std::string title;
        bool selected = false;
        bool loaded = false;
    };

    std::vector<Photo> photos;
    int columns = 3;
    float spacing = 4.0f;
    float scrollOffset = 0.0f;
    bool selectionMode = false;    // وضع التحديد المتعدد
    Color selectionColor = Color(0.25f, 0.32f, 0.71f, 0.5f);
    Color backgroundColor = Color(0, 0, 0, 1);
    bool showTitles = false;

    // --- عرض كامل ---
    bool fullscreenOpen = false;
    int fullscreenIndex = 0;

    // --- الأحداث ---
    std::function<void(int)> onPhotoTap;
    std::function<void(std::vector<int>)> onSelectionChanged;

    std::vector<int> getSelectedIndices() const {
        std::vector<int> result;
        for (size_t i = 0; i < photos.size(); i++) {
            if (photos[i].selected) result.push_back((int)i);
        }
        return result;
    }

    void selectAll() {
        for (auto& p : photos) p.selected = true;
    }

    void deselectAll() {
        for (auto& p : photos) p.selected = false;
    }

    float getItemSize() const {
        return (size.x - (columns + 1) * spacing) / columns;
    }

    void render(Renderer2D& renderer) override {
        renderer.fillRect(position.x, position.y, size.x, size.y, backgroundColor);

        float itemSize = getItemSize();
        float y = position.y + spacing - scrollOffset;

        for (size_t i = 0; i < photos.size(); i++) {
            int col = i % columns;
            int row = (int)i / columns;
            float x = position.x + spacing + col * (itemSize + spacing);
            float ry = position.y + spacing + row * (itemSize + spacing) - scrollOffset;

            if (ry + itemSize < position.y || ry > position.y + size.y) continue;

            // رسم الصورة (placeholder)
            renderer.fillRect(x, ry, itemSize, itemSize, Color(0.2f, 0.2f, 0.2f, 1));

            // علامة التحديد
            if (photos[i].selected) {
                renderer.fillRect(x, ry, itemSize, itemSize, selectionColor);
                renderer.drawText("✓", x + 8, ry + 8, Color(1, 1, 1, 1));
            }
        }
    }

    void handleEvent(const SDL_Event& event) override {
        if (event.type == SDL_MOUSEWHEEL) {
            scrollOffset -= event.wheel.y * 40.0f;
            float itemSize = getItemSize();
            int rows = ((int)photos.size() + columns - 1) / columns;
            float maxScroll = rows * (itemSize + spacing) - size.y + spacing;
            scrollOffset = std::max(0.0f, std::min(scrollOffset, maxScroll));
        }
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            float itemSize = getItemSize();
            int col = (int)((event.button.x - position.x - spacing) / (itemSize + spacing));
            int row = (int)((event.button.y - position.y - spacing + scrollOffset) /
                          (itemSize + spacing));
            int index = row * columns + col;
            if (col >= 0 && col < columns && index >= 0 && index < (int)photos.size()) {
                if (selectionMode) {
                    photos[index].selected = !photos[index].selected;
                    if (onSelectionChanged) onSelectionChanged(getSelectedIndices());
                } else {
                    if (onPhotoTap) onPhotoTap(index);
                }
            }
        }
    }
};

// ═══════════════════════════════════════════════════════════════
// 4. الكاميرا - CameraView
// ═══════════════════════════════════════════════════════════════

/**
 * @class CameraView
 * @brief عرض الكاميرا - واجهة التقاط الصور والفيديو
 * 
 * يوفر:
 * - معاينة الكاميرا
 * - التقاط صور
 * - تسجيل فيديو
 * - تبديل الكاميرا (أمامية/خلفية)
 * - فلاش وتكبير
 */
class CameraView : public Widget {
public:
    enum class CameraFacing { Front, Back };
    enum class CaptureMode { Photo, Video };
    enum class FlashMode { Off, On, Auto, Torch };

    CameraFacing facing = CameraFacing::Back;
    CaptureMode captureMode = CaptureMode::Photo;
    FlashMode flashMode = FlashMode::Off;
    float zoomLevel = 1.0f;
    float minZoom = 1.0f;
    float maxZoom = 10.0f;
    bool isRecording = false;
    float recordingTime = 0.0f;    // ثوان
    bool showGrid = false;         // شبكة ثلاثيات
    bool showControls = true;
    float aspectRatio = 4.0f / 3.0f;

    // --- ألوان الواجهة ---
    Color overlayColor = Color(0, 0, 0, 0.3f);
    Color captureButtonColor = Color(1, 1, 1, 1);
    Color recordColor = Color(1, 0, 0, 1);

    // --- الأحداث ---
    std::function<void(const std::string&)> onPhotoCaptured;
    std::function<void(const std::string&)> onVideoRecorded;
    std::function<void()> onFlipCamera;

    void capture() {
        if (captureMode == CaptureMode::Photo) {
            if (onPhotoCaptured) onPhotoCaptured("photo_" + std::to_string(time(nullptr)));
        } else {
            isRecording = !isRecording;
            if (!isRecording && onVideoRecorded) {
                onVideoRecorded("video_" + std::to_string(time(nullptr)));
            }
            recordingTime = 0;
        }
    }

    void flipCamera() {
        facing = (facing == CameraFacing::Back) ? CameraFacing::Front : CameraFacing::Back;
        if (onFlipCamera) onFlipCamera();
    }

    void cycleFlash() {
        switch (flashMode) {
            case FlashMode::Off: flashMode = FlashMode::On; break;
            case FlashMode::On: flashMode = FlashMode::Auto; break;
            case FlashMode::Auto: flashMode = FlashMode::Torch; break;
            case FlashMode::Torch: flashMode = FlashMode::Off; break;
        }
    }

    void update(float dt) {
        if (isRecording) recordingTime += dt;
    }

    void render(Renderer2D& renderer) override {
        // رسم خلفية الكاميرا (placeholder)
        renderer.fillRect(position.x, position.y, size.x, size.y, Color(0.1f, 0.1f, 0.1f, 1));

        // الشبكة
        if (showGrid) {
            Color gridC(1, 1, 1, 0.3f);
            float thirdW = size.x / 3;
            float thirdH = size.y / 3;
            renderer.fillRect(position.x + thirdW, position.y, 1, size.y, gridC);
            renderer.fillRect(position.x + thirdW * 2, position.y, 1, size.y, gridC);
            renderer.fillRect(position.x, position.y + thirdH, size.x, 1, gridC);
            renderer.fillRect(position.x, position.y + thirdH * 2, size.x, 1, gridC);
        }

        if (!showControls) return;

        // شريط التحكم العلوي
        renderer.fillRect(position.x, position.y, size.x, 50, overlayColor);

        // فلاش
        std::string flashText;
        switch (flashMode) {
            case FlashMode::Off: flashText = "⚡✕"; break;
            case FlashMode::On: flashText = "⚡"; break;
            case FlashMode::Auto: flashText = "⚡A"; break;
            case FlashMode::Torch: flashText = "🔦"; break;
        }
        renderer.drawText(flashText, position.x + 30, position.y + 25, Color(1, 1, 1, 1));

        // شريط التحكم السفلي
        float bottomY = position.y + size.y - 100;
        renderer.fillRect(position.x, bottomY, size.x, 100, overlayColor);

        // زر الالتقاط
        float btnR = 30;
        float btnX = position.x + size.x / 2;
        float btnY = bottomY + 50;
        Color btnColor = (captureMode == CaptureMode::Video && isRecording) ?
            recordColor : captureButtonColor;
        renderer.fillRect(btnX - btnR, btnY - btnR, btnR * 2, btnR * 2, btnColor);

        // مؤقت التسجيل
        if (isRecording) {
            int mins = (int)recordingTime / 60;
            int secs = (int)recordingTime % 60;
            std::string timeStr = std::to_string(mins) + ":" +
                (secs < 10 ? "0" : "") + std::to_string(secs);
            renderer.drawText(timeStr, size.x / 2, position.y + 25, recordColor);
        }

        // زر تبديل الكاميرا
        renderer.drawText("🔄", position.x + size.x - 60, btnY, Color(1, 1, 1, 1));
    }

    void handleEvent(const SDL_Event& event) override {
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            float bottomY = position.y + size.y - 100;
            if (event.button.y >= bottomY) {
                float mx = (float)event.button.x;
                if (mx > size.x / 2 - 40 && mx < size.x / 2 + 40) capture();
                else if (mx > size.x - 80) flipCamera();
            } else if (event.button.y < 50) {
                if (event.button.x < 60) cycleFlash();
            }
        }
        if (event.type == SDL_MOUSEWHEEL) {
            zoomLevel *= (event.wheel.y > 0) ? 1.1f : 0.9f;
            zoomLevel = std::max(minZoom, std::min(maxZoom, zoomLevel));
        }
    }
};

// ═══════════════════════════════════════════════════════════════
// 5. مؤشر تحميل الرفع - UploadProgress
// ═══════════════════════════════════════════════════════════════

/**
 * @class UploadProgress
 * @brief مؤشر تقدم الرفع/التنزيل - لعرض تقدم نقل الملفات
 * 
 * يوفر:
 * - عرض اسم الملف وحجمه
 * - شريط تقدم مع النسبة
 * - سرعة النقل والوقت المتبقي
 * - إلغاء وإيقاف مؤقت
 */
class UploadProgress : public Widget {
public:
    struct FileTransfer {
        std::string fileName;
        float fileSize = 0;        // بايت
        float transferred = 0;    // بايت
        float speed = 0;          // بايت/ثانية
        bool isPaused = false;
        bool isCancelled = false;
        bool isComplete = false;
        bool hasError = false;
        std::string errorMessage;
    };

    std::vector<FileTransfer> transfers;
    float itemHeight = 72.0f;
    Color progressColor = Color(0.25f, 0.32f, 0.71f, 1);
    Color completeColor = Color(0.3f, 0.7f, 0.3f, 1);
    Color errorColor = Color(0.9f, 0.2f, 0.2f, 1);
    Color backgroundColor = Color(1, 1, 1, 1);
    float scrollOffset = 0.0f;

    // --- الأحداث ---
    std::function<void(int)> onCancel;
    std::function<void(int)> onPause;
    std::function<void(int)> onRetry;

    std::string formatSize(float bytes) const {
        if (bytes < 1024) return std::to_string((int)bytes) + " B";
        if (bytes < 1024 * 1024)
            return std::to_string((int)(bytes / 1024)) + " KB";
        if (bytes < 1024 * 1024 * 1024)
            return std::to_string((int)(bytes / (1024 * 1024))) + " MB";
        return std::to_string((int)(bytes / (1024 * 1024 * 1024))) + " GB";
    }

    std::string formatETA(float bytes, float speed) const {
        if (speed <= 0) return "...";
        float seconds = bytes / speed;
        if (seconds < 60) return std::to_string((int)seconds) + " ث";
        if (seconds < 3600) return std::to_string((int)(seconds / 60)) + " د";
        return std::to_string((int)(seconds / 3600)) + " س";
    }

    void render(Renderer2D& renderer) override {
        float y = position.y - scrollOffset;

        for (size_t i = 0; i < transfers.size(); i++) {
            auto& t = transfers[i];
            if (y + itemHeight < position.y || y > position.y + size.y) {
                y += itemHeight;
                continue;
            }

            renderer.fillRect(position.x, y, size.x, itemHeight, backgroundColor);

            // اسم الملف
            renderer.drawText(t.fileName, position.x + 16, y + 16,
                            Color(0.2f, 0.2f, 0.2f, 1));

            // شريط التقدم
            float progress = t.fileSize > 0 ? t.transferred / t.fileSize : 0;
            Color barColor = t.hasError ? errorColor :
                           (t.isComplete ? completeColor : progressColor);
            renderer.fillRect(position.x + 16, y + 36, size.x - 32, 4,
                            Color(0.9f, 0.9f, 0.9f, 1));
            renderer.fillRect(position.x + 16, y + 36,
                            (size.x - 32) * progress, 4, barColor);

            // معلومات النقل
            std::string info = formatSize(t.transferred) + " / " + formatSize(t.fileSize);
            if (t.speed > 0 && !t.isComplete && !t.hasError) {
                float remaining = t.fileSize - t.transferred;
                info += " - " + formatSize(t.speed) + "/ث - " + formatETA(remaining, t.speed);
            }
            renderer.drawText(info, position.x + 16, y + 52,
                            Color(0.5f, 0.5f, 0.5f, 1));

            y += itemHeight;
        }
    }

    void handleEvent(const SDL_Event& event) override {
        if (event.type == SDL_MOUSEWHEEL) {
            scrollOffset -= event.wheel.y * 30.0f;
            scrollOffset = std::max(0.0f, scrollOffset);
        }
    }
};

// ═══════════════════════════════════════════════════════════════
// 6. مركب متحرك - AnimatedWidget
// ═══════════════════════════════════════════════════════════════

/**
 * @brief نوع منحنى التحريك
 */
enum class AnimationCurve {
    Linear,         // خطي
    EaseIn,         // تسارع
    EaseOut,        // تباطؤ
    EaseInOut,      // تسارع ثم تباطؤ
    Bounce,         // ارتداد
    Elastic,        // مطاطي
    Back,           // رجوع للخلف ثم تقدم
    Spring          // نابض
};

/**
 * @class AnimatedWidget
 * @brief محرك تحريك عام - لتحريك أي خاصية لأي مركب
 * 
 * يوفر:
 * - تحريك الموضع والحجم والشفافية واللون
 * - منحنيات تحريك متعددة (خطي، تسارع، ارتداد، مطاطي)
 * - تكرار وعكس وتسلسل
 * - تأخير البدء
 */
class AnimatedWidget : public Widget {
public:
    struct Animation {
        std::string property;       // اسم الخاصية (x, y, w, h, alpha, rotation, scale)
        float from = 0;
        float to = 1;
        float duration = 0.3f;     // ثوان
        float delay = 0.0f;        // تأخير البدء
        AnimationCurve curve = AnimationCurve::EaseInOut;
        bool repeat = false;
        bool reverse = false;      // عكس عند نهاية كل تكرار
        int repeatCount = 0;       // 0 = لا نهائي

        // --- حالة داخلية ---
        float elapsed = 0.0f;
        float currentValue = 0.0f;
        bool isPlaying = false;
        bool isReversing = false;
        int currentRepeat = 0;
    };

    std::vector<Animation> animations;
    std::shared_ptr<Widget> child;
    bool autoStart = true;

    // --- الخصائص المحركة ---
    float alpha = 1.0f;
    float rotation = 0.0f;
    float scaleX = 1.0f;
    float scaleY = 1.0f;

    // --- الأحداث ---
    std::function<void()> onComplete;
    std::function<void(const std::string&, float)> onValueChanged;

    static float applyCurve(AnimationCurve curve, float t) {
        switch (curve) {
            case AnimationCurve::Linear: return t;
            case AnimationCurve::EaseIn: return t * t;
            case AnimationCurve::EaseOut: return t * (2 - t);
            case AnimationCurve::EaseInOut:
                return t < 0.5f ? 2 * t * t : -1 + (4 - 2 * t) * t;
            case AnimationCurve::Bounce: {
                if (t < 1/2.75f) return 7.5625f * t * t;
                if (t < 2/2.75f) { t -= 1.5f/2.75f; return 7.5625f * t * t + 0.75f; }
                if (t < 2.5f/2.75f) { t -= 2.25f/2.75f; return 7.5625f * t * t + 0.9375f; }
                t -= 2.625f/2.75f; return 7.5625f * t * t + 0.984375f;
            }
            case AnimationCurve::Elastic:
                return (t == 0 || t == 1) ? t :
                    -std::pow(2, 10 * (t - 1)) * std::sin((t - 1.1f) * 5 * 3.14159f);
            case AnimationCurve::Back:
                return t * t * (2.70158f * t - 1.70158f);
            case AnimationCurve::Spring: {
                float freq = 4.71238898f; // 3π/2
                return 1 - std::cos(t * freq) * std::exp(-6 * t);
            }
            default: return t;
        }
    }

    void addAnimation(const std::string& prop, float from, float to,
                     float duration = 0.3f, AnimationCurve curve = AnimationCurve::EaseInOut) {
        Animation anim;
        anim.property = prop;
        anim.from = from;
        anim.to = to;
        anim.duration = duration;
        anim.curve = curve;
        anim.isPlaying = autoStart;
        animations.push_back(anim);
    }

    void playAll() {
        for (auto& a : animations) a.isPlaying = true;
    }

    void stopAll() {
        for (auto& a : animations) a.isPlaying = false;
    }

    void resetAll() {
        for (auto& a : animations) {
            a.elapsed = 0;
            a.currentRepeat = 0;
            a.isReversing = false;
        }
    }

    void update(float dt) {
        bool allDone = true;
        for (auto& a : animations) {
            if (!a.isPlaying) continue;
            allDone = false;

            a.elapsed += dt;
            if (a.elapsed < a.delay) continue;

            float t = (a.elapsed - a.delay) / a.duration;
            if (t > 1.0f) {
                if (a.repeat && (a.repeatCount == 0 || a.currentRepeat < a.repeatCount)) {
                    if (a.reverse) a.isReversing = !a.isReversing;
                    a.elapsed = a.delay;
                    a.currentRepeat++;
                    t = 0;
                } else {
                    t = 1.0f;
                    a.isPlaying = false;
                }
            }

            float curvedT = applyCurve(a.curve, a.isReversing ? 1.0f - t : t);
            a.currentValue = a.from + (a.to - a.from) * curvedT;

            // تطبيق القيمة
            if (a.property == "x") position.x = a.currentValue;
            else if (a.property == "y") position.y = a.currentValue;
            else if (a.property == "alpha") alpha = a.currentValue;
            else if (a.property == "rotation") rotation = a.currentValue;
            else if (a.property == "scaleX") scaleX = a.currentValue;
            else if (a.property == "scaleY") scaleY = a.currentValue;

            if (onValueChanged) onValueChanged(a.property, a.currentValue);
        }

        if (allDone && onComplete) onComplete();
    }

    void render(Renderer2D& renderer) override {
        if (child && alpha > 0) {
            child->position = position;
            child->size = Vec2(size.x * scaleX, size.y * scaleY);
            child->render(renderer);
        }
    }

    void handleEvent(const SDL_Event& event) override {
        if (child) child->handleEvent(event);
    }
};

// ═══════════════════════════════════════════════════════════════
// 7. انتقال بين المركبات - AnimatedSwitcher
// ═══════════════════════════════════════════════════════════════

/**
 * @brief نوع تأثير الانتقال للمُبدل
 */
enum class SwitchTransition {
    Fade,       // تلاشي
    Scale,      // تكبير/تصغير
    Slide,      // انزلاق
    Rotation    // دوران
};

/**
 * @class AnimatedSwitcher
 * @brief مبدل متحرك - ينتقل بسلاسة بين مركبين
 * 
 * عند تغيير المركب الأبن، يعرض تأثير انتقال بينهما
 */
class AnimatedSwitcher : public Widget {
public:
    std::shared_ptr<Widget> currentChild;
    std::shared_ptr<Widget> previousChild;
    SwitchTransition transition = SwitchTransition::Fade;
    float duration = 0.3f;
    AnimationCurve curve = AnimationCurve::EaseInOut;

    // --- حالة الانتقال ---
    float transitionProgress = 1.0f; // 1 = مكتمل
    bool isTransitioning = false;

    void switchTo(std::shared_ptr<Widget> newChild) {
        if (currentChild == newChild) return;
        previousChild = currentChild;
        currentChild = newChild;
        transitionProgress = 0.0f;
        isTransitioning = true;
    }

    void update(float dt) {
        if (isTransitioning) {
            transitionProgress += dt / duration;
            if (transitionProgress >= 1.0f) {
                transitionProgress = 1.0f;
                isTransitioning = false;
                previousChild = nullptr;
            }
        }
    }

    void render(Renderer2D& renderer) override {
        float t = AnimatedWidget::applyCurve(curve, transitionProgress);

        if (isTransitioning && previousChild) {
            switch (transition) {
                case SwitchTransition::Fade:
                    // المركب السابق يتلاشى
                    previousChild->render(renderer);
                    break;
                case SwitchTransition::Slide:
                    previousChild->position.x = position.x - size.x * t;
                    previousChild->render(renderer);
                    break;
                default:
                    previousChild->render(renderer);
                    break;
            }
        }

        if (currentChild) {
            if (isTransitioning) {
                switch (transition) {
                    case SwitchTransition::Scale:
                        currentChild->size = Vec2(size.x * t, size.y * t);
                        break;
                    case SwitchTransition::Slide:
                        currentChild->position.x = position.x + size.x * (1 - t);
                        break;
                    default: break;
                }
            }
            currentChild->render(renderer);
        }
    }

    void handleEvent(const SDL_Event& event) override {
        if (currentChild) currentChild->handleEvent(event);
    }
};

// ═══════════════════════════════════════════════════════════════
// 8. تأثير التموج - RippleEffect
// ═══════════════════════════════════════════════════════════════

/**
 * @class RippleEffect
 * @brief تأثير تموج عند اللمس - Material Design ripple
 * 
 * يضاف فوق أي مركب لإضافة تأثير لمس مرئي
 */
class RippleEffect : public Widget {
public:
    struct Ripple {
        Vec2 center;               // نقطة البدء
        float radius = 0;
        float maxRadius = 0;
        float alpha = 0.3f;
        bool active = true;
    };

    std::vector<Ripple> ripples;
    std::shared_ptr<Widget> child;
    Color rippleColor = Color(0, 0, 0, 0.12f); // لون التموج
    float expandSpeed = 800.0f;    // سرعة التوسع (بكسل/ثانية)
    float fadeSpeed = 2.0f;        // سرعة التلاشي
    bool clipped = true;           // قص التموج داخل الحدود
    float borderRadius = 0;        // شكل القص

    void addRipple(float x, float y) {
        Ripple r;
        r.center = Vec2(x, y);
        // أقصى نصف قطر = أبعد زاوية
        float dx = std::max(x - position.x, position.x + size.x - x);
        float dy = std::max(y - position.y, position.y + size.y - y);
        r.maxRadius = std::sqrt(dx * dx + dy * dy);
        ripples.push_back(r);
    }

    void update(float dt) {
        for (auto it = ripples.begin(); it != ripples.end();) {
            it->radius += expandSpeed * dt;
            if (it->radius >= it->maxRadius) {
                it->alpha -= fadeSpeed * dt;
                if (it->alpha <= 0) {
                    it = ripples.erase(it);
                    continue;
                }
            }
            ++it;
        }
    }

    void render(Renderer2D& renderer) override {
        if (child) child->render(renderer);

        for (const auto& r : ripples) {
            Color c = rippleColor;
            c.a = r.alpha;
            // رسم دائرة التموج (تقريب بمربع)
            float drawR = std::min(r.radius, r.maxRadius);
            renderer.fillRect(r.center.x - drawR, r.center.y - drawR,
                            drawR * 2, drawR * 2, c);
        }
    }

    void handleEvent(const SDL_Event& event) override {
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            addRipple((float)event.button.x, (float)event.button.y);
        }
        if (child) child->handleEvent(event);
    }
};

// ═══════════════════════════════════════════════════════════════
// 9. انتقال الصفحات - PageTransition
// ═══════════════════════════════════════════════════════════════

/**
 * @brief نوع تأثير انتقال الصفحات
 */
enum class PageTransitionType {
    Fade,           // تلاشي
    SlideRight,     // انزلاق يمين
    SlideLeft,      // انزلاق يسار
    SlideUp,        // انزلاق أعلى
    SlideDown,      // انزلاق أسفل
    Scale,          // تكبير
    ScaleAndFade,   // تكبير مع تلاشي
    Rotation,       // دوران
    SharedAxis,     // محور مشترك
    FadeTrough      // تلاشي عبر
};

/**
 * @class PageTransition
 * @brief انتقال بين الصفحات - لتطبيق تأثيرات عند التنقل
 * 
 * يُلف حول مركب لتطبيق تأثير انتقال عند الدخول والخروج
 */
class PageTransition : public Widget {
public:
    PageTransitionType type = PageTransitionType::Fade;
    float duration = 0.3f;
    AnimationCurve curve = AnimationCurve::EaseInOut;
    bool isEntering = true;        // دخول أم خروج
    float progress = 0.0f;        // 0 = بداية، 1 = نهاية
    bool isComplete = false;
    std::shared_ptr<Widget> child;

    // --- الأحداث ---
    std::function<void()> onComplete;

    void startEnter() {
        isEntering = true;
        progress = 0;
        isComplete = false;
    }

    void startExit() {
        isEntering = false;
        progress = 0;
        isComplete = false;
    }

    void update(float dt) {
        if (isComplete) return;
        progress += dt / duration;
        if (progress >= 1.0f) {
            progress = 1.0f;
            isComplete = true;
            if (onComplete) onComplete();
        }
    }

    void render(Renderer2D& renderer) override {
        if (!child) return;

        float t = AnimatedWidget::applyCurve(curve, progress);
        if (!isEntering) t = 1.0f - t;

        // تطبيق التأثير حسب النوع
        float offsetX = 0, offsetY = 0;
        float scale = 1.0f;
        float alpha = 1.0f;

        switch (type) {
            case PageTransitionType::Fade:
                alpha = t;
                break;
            case PageTransitionType::SlideRight:
                offsetX = (1.0f - t) * size.x;
                break;
            case PageTransitionType::SlideLeft:
                offsetX = -(1.0f - t) * size.x;
                break;
            case PageTransitionType::SlideUp:
                offsetY = -(1.0f - t) * size.y;
                break;
            case PageTransitionType::SlideDown:
                offsetY = (1.0f - t) * size.y;
                break;
            case PageTransitionType::Scale:
                scale = t;
                break;
            case PageTransitionType::ScaleAndFade:
                scale = 0.8f + 0.2f * t;
                alpha = t;
                break;
            default:
                alpha = t;
                break;
        }

        child->position = Vec2(position.x + offsetX + size.x * (1 - scale) / 2,
                              position.y + offsetY + size.y * (1 - scale) / 2);
        child->size = Vec2(size.x * scale, size.y * scale);
        child->render(renderer);
    }

    void handleEvent(const SDL_Event& event) override {
        if (child && isComplete) child->handleEvent(event);
    }
};

// ═══════════════════════════════════════════════════════════════
// 10. كاشف الإيماءات - GestureDetector
// ═══════════════════════════════════════════════════════════════

/**
 * @class GestureDetector
 * @brief كاشف إيماءات اللمس - يتعرف على إيماءات اللمس المختلفة
 * 
 * يوفر:
 * - نقر واحد ومزدوج
 * - ضغط مطوّل
 * - سحب (أفقي/رأسي)
 * - قرص للتكبير
 * - تدوير
 */
class GestureDetector : public Widget {
public:
    /**
     * @brief بيانات إيماءة السحب
     */
    struct DragDetails {
        Vec2 startPosition;
        Vec2 currentPosition;
        Vec2 delta;                 // الفرق من الإطار السابق
        Vec2 totalDelta;           // الفرق الكلي من البداية
        float velocity = 0;
    };

    /**
     * @brief بيانات إيماءة القرص
     */
    struct PinchDetails {
        float scale = 1.0f;
        float rotation = 0.0f;
        Vec2 focalPoint;
    };

    std::shared_ptr<Widget> child;

    // --- الأحداث ---
    std::function<void()> onTap;
    std::function<void()> onDoubleTap;
    std::function<void()> onLongPress;
    std::function<void(DragDetails)> onDragStart;
    std::function<void(DragDetails)> onDragUpdate;
    std::function<void(DragDetails)> onDragEnd;
    std::function<void(DragDetails)> onHorizontalDrag;
    std::function<void(DragDetails)> onVerticalDrag;
    std::function<void(PinchDetails)> onPinchStart;
    std::function<void(PinchDetails)> onPinchUpdate;
    std::function<void(PinchDetails)> onPinchEnd;
    std::function<void(float)> onScaleUpdate;

    // --- الإعدادات ---
    float longPressDelay = 500.0f; // ملي ثانية
    float doubleTapDelay = 300.0f;
    float dragThreshold = 8.0f;    // بكسل
    bool excludeFromSemantics = false;

    // --- حالة داخلية ---
    bool isPressed = false;
    bool isDragging = false;
    Vec2 pressPosition;
    float pressTime = 0;
    float lastTapTime = 0;
    DragDetails currentDrag;

    bool hitTest(float x, float y) const {
        return x >= position.x && x <= position.x + size.x &&
               y >= position.y && y <= position.y + size.y;
    }

    void update(float dt) {
        if (isPressed && !isDragging) {
            pressTime += dt * 1000;
            if (pressTime >= longPressDelay && onLongPress) {
                onLongPress();
                isPressed = false;
            }
        }
    }

    void render(Renderer2D& renderer) override {
        if (child) child->render(renderer);
    }

    void handleEvent(const SDL_Event& event) override {
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            float mx = (float)event.button.x;
            float my = (float)event.button.y;
            if (hitTest(mx, my)) {
                isPressed = true;
                pressPosition = Vec2(mx, my);
                pressTime = 0;
                currentDrag.startPosition = pressPosition;
                currentDrag.currentPosition = pressPosition;
                currentDrag.totalDelta = Vec2(0, 0);
            }
        }

        if (event.type == SDL_MOUSEMOTION && isPressed) {
            Vec2 current((float)event.motion.x, (float)event.motion.y);
            Vec2 delta = Vec2(current.x - currentDrag.currentPosition.x,
                            current.y - currentDrag.currentPosition.y);
            currentDrag.delta = delta;
            currentDrag.currentPosition = current;
            currentDrag.totalDelta = Vec2(current.x - currentDrag.startPosition.x,
                                         current.y - currentDrag.startPosition.y);

            float dist = std::sqrt(currentDrag.totalDelta.x * currentDrag.totalDelta.x +
                                  currentDrag.totalDelta.y * currentDrag.totalDelta.y);
            if (!isDragging && dist > dragThreshold) {
                isDragging = true;
                if (onDragStart) onDragStart(currentDrag);
            }

            if (isDragging) {
                if (onDragUpdate) onDragUpdate(currentDrag);
                if (onHorizontalDrag && std::abs(delta.x) > std::abs(delta.y))
                    onHorizontalDrag(currentDrag);
                if (onVerticalDrag && std::abs(delta.y) > std::abs(delta.x))
                    onVerticalDrag(currentDrag);
            }
        }

        if (event.type == SDL_MOUSEBUTTONUP && isPressed) {
            if (isDragging) {
                if (onDragEnd) onDragEnd(currentDrag);
                isDragging = false;
            } else {
                // نقر
                float now = pressTime; // تقريب
                if (now - lastTapTime < doubleTapDelay && onDoubleTap) {
                    onDoubleTap();
                    lastTapTime = 0;
                } else {
                    if (onTap) onTap();
                    lastTapTime = now;
                }
            }
            isPressed = false;
        }

        if (child) child->handleEvent(event);
    }
};

} // namespace mobile
} // namespace ui
} // namespace graphics
} // namespace sad
