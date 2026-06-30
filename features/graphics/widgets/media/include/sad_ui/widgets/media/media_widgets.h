/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: media_widgets.h
 * المسار: features/graphics/widgets/media/include/sad_ui/widgets/media/media_widgets.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * مكونات الوسائط: Carousel، RichText، Markdown، CodeBlock، AudioPlayer، ImageGallery
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_WIDGETS_MEDIA_H
#define SAD_UI_WIDGETS_MEDIA_H

#include "sad_ui/node.h"
#include "sad_ui/style.h"
#include <string>
#include <memory>
#include <optional>
#include <vector>

namespace sad {
namespace ui {
namespace widgets {

// ── Carousel ────────────────────────────────────────────────────────

class CarouselBuilder {
public:
    CarouselBuilder();
    CarouselBuilder& autoPlay(bool a);
    CarouselBuilder& interval(float seconds);
    CarouselBuilder& showIndicators(bool s);
    std::shared_ptr<UINode> build() const;
private:
    bool autoPlay_ = false;
    float interval_ = 3.0f;
    bool showIndicators_ = true;
};

// ── RichText ────────────────────────────────────────────────────────

class RichTextBuilder {
public:
    explicit RichTextBuilder(const std::string& text = "");
    RichTextBuilder& selectable(bool s);
    RichTextBuilder& maxLines(int n);
    std::shared_ptr<UINode> build() const;
private:
    std::string text_;
    bool selectable_ = false;
    std::optional<int> maxLines_;
};

// ── Markdown ────────────────────────────────────────────────────────

class MarkdownBuilder {
public:
    explicit MarkdownBuilder(const std::string& source = "");
    MarkdownBuilder& enableLinks(bool e);
    MarkdownBuilder& enableImages(bool e);
    std::shared_ptr<UINode> build() const;
private:
    std::string source_;
    bool enableLinks_ = true;
    bool enableImages_ = true;
};

// ── CodeBlock ───────────────────────────────────────────────────────

class CodeBlockBuilder {
public:
    explicit CodeBlockBuilder(const std::string& code = "");
    CodeBlockBuilder& language(const std::string& lang);
    CodeBlockBuilder& showLineNumbers(bool s);
    CodeBlockBuilder& theme(const std::string& t);
    std::shared_ptr<UINode> build() const;
private:
    std::string code_;
    std::string language_ = "sad";
    bool showLineNumbers_ = true;
    std::string theme_ = "dark";
};

// ── AudioPlayer ─────────────────────────────────────────────────────

class AudioPlayerBuilder {
public:
    explicit AudioPlayerBuilder(const std::string& source = "");
    AudioPlayerBuilder& autoPlay(bool a);
    AudioPlayerBuilder& loop(bool l);
    AudioPlayerBuilder& showWaveform(bool s);
    std::shared_ptr<UINode> build() const;
private:
    std::string source_;
    bool autoPlay_ = false;
    bool loop_ = false;
    bool showWaveform_ = false;
};

// ── ImageGallery ────────────────────────────────────────────────────

class ImageGalleryBuilder {
public:
    ImageGalleryBuilder();
    ImageGalleryBuilder& columns(int n);
    ImageGalleryBuilder& spacing(float s);
    ImageGalleryBuilder& zoomable(bool z);
    std::shared_ptr<UINode> build() const;
private:
    int columns_ = 3;
    float spacing_ = 4.0f;
    bool zoomable_ = true;
};

} // namespace widgets
} // namespace ui
} // namespace sad

#endif // SAD_UI_WIDGETS_MEDIA_H
