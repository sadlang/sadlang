/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  sad_ui_unified.cpp — تنفيذ المدير الموحد والدوال المختصرة
 *  SadUI Unified Manager implementation + shorthand functions
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui_unified.h"
#include <stdexcept>
#include <iostream>

namespace sad {
namespace unified {

// ═══════════════════════════════════════════════════════════════════════════════
//  SadUIBackend — تنفيذ افتراضي لدوال مُركّبة (convenience)
// ═══════════════════════════════════════════════════════════════════════════════

WidgetId SadUIBackend::createButton(const std::string& text) {
    WidgetId id = createWidget(WidgetType::Button);
    setText(id, text);
    return id;
}

WidgetId SadUIBackend::createText(const std::string& text) {
    WidgetId id = createWidget(WidgetType::Text);
    setText(id, text);
    return id;
}

WidgetId SadUIBackend::createColumn() {
    return createWidget(WidgetType::Column);
}

WidgetId SadUIBackend::createRow() {
    return createWidget(WidgetType::Row);
}

WidgetId SadUIBackend::createCard() {
    return createWidget(WidgetType::Card);
}

WidgetId SadUIBackend::createTextField(const std::string& hint) {
    WidgetId id = createWidget(WidgetType::TextField);
    setText(id, hint);
    return id;
}

WidgetId SadUIBackend::createHeading(const std::string& text) {
    WidgetId id = createWidget(WidgetType::Heading);
    setText(id, text);
    return id;
}

WidgetId SadUIBackend::createScaffold(const std::string& title, WidgetId body, WidgetId bottomBar) {
    WidgetId scaffold = createWidget(WidgetType::Container);
    WidgetId appBar = createWidget(WidgetType::AppBar);
    setText(appBar, title);
    addChild(scaffold, appBar);
    if (body >= 0) addChild(scaffold, body);
    if (bottomBar >= 0) addChild(scaffold, bottomBar);
    return scaffold;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  SadUI — Singleton
// ═══════════════════════════════════════════════════════════════════════════════

SadUI& SadUI::instance() {
    static SadUI s;
    return s;
}

void SadUI::setBackend(std::unique_ptr<SadUIBackend> backend) {
    m_backend = std::move(backend);
}

SadUIBackend* SadUI::backend() {
    return m_backend.get();
}

bool SadUI::isInitialized() const {
    return m_backend != nullptr;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  الدوال المختصرة — تُمرر كل شيء للـ Backend الفعال
//  Shorthand functions — delegate to the active backend
// ═══════════════════════════════════════════════════════════════════════════════

static SadUIBackend* B() {
    auto* b = SadUI::instance().backend();
    if (!b) {
        std::cerr << "[SadUI] لم يتم تهيئة الـ Backend بعد! استدعِ SadUI::instance().setBackend() أولاً.\n";
        return nullptr;
    }
    return b;
}

// ─── التطبيق ───

int ui_app_create(const std::string& title, int w, int h) {
    auto* b = B(); return b ? b->appCreate(title, w, h) : -1;
}
bool ui_app_running(int appId) {
    auto* b = B(); return b ? b->appIsRunning(appId) : false;
}
void ui_app_update(int appId) {
    auto* b = B(); if (b) b->appUpdate(appId);
}
void ui_app_set_content(int appId, WidgetId root) {
    auto* b = B(); if (b) b->appSetContent(appId, root);
}
void ui_app_set_bg(int appId, int r, int g, int b_) {
    auto* b = B(); if (b) b->appSetBgColor(appId, r, g, b_);
}
void ui_app_close(int appId) {
    auto* b = B(); if (b) b->appClose(appId);
}

// ─── إنشاء ───

WidgetId ui_create(WidgetType type) {
    auto* b = B(); return b ? b->createWidget(type) : -1;
}
WidgetId ui_button(const std::string& text) {
    auto* b = B(); return b ? b->createButton(text) : -1;
}
WidgetId ui_text(const std::string& text) {
    auto* b = B(); return b ? b->createText(text) : -1;
}
WidgetId ui_column() {
    auto* b = B(); return b ? b->createColumn() : -1;
}
WidgetId ui_row() {
    auto* b = B(); return b ? b->createRow() : -1;
}
WidgetId ui_card() {
    auto* b = B(); return b ? b->createCard() : -1;
}
WidgetId ui_text_field(const std::string& hint) {
    auto* b = B(); return b ? b->createTextField(hint) : -1;
}
WidgetId ui_heading(const std::string& text) {
    auto* b = B(); return b ? b->createHeading(text) : -1;
}
WidgetId ui_scaffold(const std::string& title, WidgetId body, WidgetId bottomBar) {
    auto* b = B(); return b ? b->createScaffold(title, body, bottomBar) : -1;
}

// ─── تعديل ───

void ui_set_text(WidgetId id, const std::string& text) {
    auto* b = B(); if (b) b->setText(id, text);
}
void ui_add_child(WidgetId parent, WidgetId child) {
    auto* b = B(); if (b) b->addChild(parent, child);
}
void ui_set_padding(WidgetId id, float all) {
    auto* b = B(); if (b) b->setPadding(id, all, all, all, all);
}
void ui_set_padding4(WidgetId id, float t, float r, float b_, float l) {
    auto* b = B(); if (b) b->setPadding(id, t, r, b_, l);
}
void ui_set_spacing(WidgetId id, float s) {
    auto* b = B(); if (b) b->setSpacing(id, s);
}
void ui_set_alignment(WidgetId id, const std::string& main, const std::string& cross) {
    auto* b = B(); if (b) b->setAlignment(id, main, cross);
}
void ui_set_flex(WidgetId id, float f) {
    auto* b = B(); if (b) b->setFlex(id, f);
}
void ui_set_bg_color(WidgetId id, int r, int g, int b_) {
    auto* be = B(); if (be) be->setColor(id, "bg", r, g, b_);
}
void ui_set_text_color(WidgetId id, int r, int g, int b_) {
    auto* be = B(); if (be) be->setColor(id, "text", r, g, b_);
}
void ui_set_font_size(WidgetId id, float size) {
    auto* b = B(); if (b) b->setFloat(id, "fontSize", size);
}
void ui_set_border_radius(WidgetId id, float radius) {
    auto* b = B(); if (b) b->setFloat(id, "borderRadius", radius);
}
void ui_set_visible(WidgetId id, bool v) {
    auto* b = B(); if (b) b->setBool(id, "visible", v);
}
void ui_set_enabled(WidgetId id, bool e) {
    auto* b = B(); if (b) b->setBool(id, "enabled", e);
}

// ─── قراءة ───

std::string ui_get_text(WidgetId id) {
    auto* b = B(); return b ? b->getText(id) : "";
}
bool ui_was_clicked(WidgetId id) {
    auto* b = B(); return b ? b->wasClicked(id) : false;
}
std::string ui_get_input(WidgetId id) {
    auto* b = B(); return b ? b->getInputText(id) : "";
}

}} // namespace sad::unified
