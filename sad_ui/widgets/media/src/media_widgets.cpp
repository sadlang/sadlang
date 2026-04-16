/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: media_widgets.cpp
 * المسار: sad_ui/widgets/media/src/media_widgets.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * تنفيذ مكونات الوسائط: Carousel، RichText، Markdown، CodeBlock، AudioPlayer، ImageGallery
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/widgets/media/media_widgets.h"

namespace sad {
namespace ui {
namespace widgets {

// ── Carousel ────────────────────────────────────────────────────────

CarouselBuilder::CarouselBuilder() = default;
CarouselBuilder& CarouselBuilder::autoPlay(bool a) { autoPlay_ = a; return *this; }
CarouselBuilder& CarouselBuilder::interval(float seconds) { interval_ = seconds; return *this; }
CarouselBuilder& CarouselBuilder::showIndicators(bool s) { showIndicators_ = s; return *this; }

std::shared_ptr<UINode> CarouselBuilder::build() const {
    auto node = UINode::create(UINodeType::Carousel);
    return node;
}

// ── RichText ────────────────────────────────────────────────────────

RichTextBuilder::RichTextBuilder(const std::string& text) : text_(text) {}
RichTextBuilder& RichTextBuilder::selectable(bool s) { selectable_ = s; return *this; }
RichTextBuilder& RichTextBuilder::maxLines(int n) { maxLines_ = n; return *this; }

std::shared_ptr<UINode> RichTextBuilder::build() const {
    auto node = UINode::create(UINodeType::RichText);
    node->setPrimaryArg(text_);
    if (maxLines_) node->addModifier(Modifier::numeric(ModifierType::MaxLines, static_cast<float>(*maxLines_)));
    return node;
}

// ── Markdown ────────────────────────────────────────────────────────

MarkdownBuilder::MarkdownBuilder(const std::string& source) : source_(source) {}
MarkdownBuilder& MarkdownBuilder::enableLinks(bool e) { enableLinks_ = e; return *this; }
MarkdownBuilder& MarkdownBuilder::enableImages(bool e) { enableImages_ = e; return *this; }

std::shared_ptr<UINode> MarkdownBuilder::build() const {
    auto node = UINode::create(UINodeType::Markdown);
    node->setPrimaryArg(source_);
    return node;
}

// ── CodeBlock ───────────────────────────────────────────────────────

CodeBlockBuilder::CodeBlockBuilder(const std::string& code) : code_(code) {}
CodeBlockBuilder& CodeBlockBuilder::language(const std::string& lang) { language_ = lang; return *this; }
CodeBlockBuilder& CodeBlockBuilder::showLineNumbers(bool s) { showLineNumbers_ = s; return *this; }
CodeBlockBuilder& CodeBlockBuilder::theme(const std::string& t) { theme_ = t; return *this; }

std::shared_ptr<UINode> CodeBlockBuilder::build() const {
    auto node = UINode::create(UINodeType::CodeBlock);
    node->setPrimaryArg(code_);
    return node;
}

// ── AudioPlayer ─────────────────────────────────────────────────────

AudioPlayerBuilder::AudioPlayerBuilder(const std::string& source) : source_(source) {}
AudioPlayerBuilder& AudioPlayerBuilder::autoPlay(bool a) { autoPlay_ = a; return *this; }
AudioPlayerBuilder& AudioPlayerBuilder::loop(bool l) { loop_ = l; return *this; }
AudioPlayerBuilder& AudioPlayerBuilder::showWaveform(bool s) { showWaveform_ = s; return *this; }

std::shared_ptr<UINode> AudioPlayerBuilder::build() const {
    auto node = UINode::create(UINodeType::AudioPlayer);
    node->setPrimaryArg(source_);
    return node;
}

// ── ImageGallery ────────────────────────────────────────────────────

ImageGalleryBuilder::ImageGalleryBuilder() = default;
ImageGalleryBuilder& ImageGalleryBuilder::columns(int n) { columns_ = n; return *this; }
ImageGalleryBuilder& ImageGalleryBuilder::spacing(float s) { spacing_ = s; return *this; }
ImageGalleryBuilder& ImageGalleryBuilder::zoomable(bool z) { zoomable_ = z; return *this; }

std::shared_ptr<UINode> ImageGalleryBuilder::build() const {
    auto node = UINode::create(UINodeType::ImageGallery);
    node->addModifier(Modifier::numeric(ModifierType::Spacing, spacing_));
    return node;
}

} // namespace widgets
} // namespace ui
} // namespace sad
