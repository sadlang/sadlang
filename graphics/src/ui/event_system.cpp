/**
 * =============================================================================
 * ملف: event_system.cpp
 * الوصف: تنفيذ نظام الأحداث
 * (AR) @brief تنفيذ EventManager مع انتشار الأحداث وتتبع السحب
 * (EN) @brief EventManager implementation with event bubbling and drag tracking
 * المهمة: T308 - كامل: Event system
 * =============================================================================
 */

#include "../include/ui/event_system.h"
#include <algorithm>
#include <chrono>

namespace sad {
namespace graphics {
namespace ui {

EventManager& EventManager::getInstance() {
    static EventManager instance;
    return instance;
}

int EventManager::addEventListener(Widget* widget, EventType type,
                                    EventHandler handler, int priority) {
    EventListener listener;
    listener.type = type;
    listener.handler = handler;
    listener.priority = priority;
    listener.id = nextId_++;
    listener.once = false;
    
    auto& list = listeners_[widget];
    list.push_back(listener);
    
    // Sort by priority descending
    std::sort(list.begin(), list.end(),
              [](const EventListener& a, const EventListener& b) {
                  return a.priority > b.priority;
              });
    
    return listener.id;
}

int EventManager::addOnceListener(Widget* widget, EventType type,
                                   EventHandler handler) {
    EventListener listener;
    listener.type = type;
    listener.handler = handler;
    listener.priority = 0;
    listener.id = nextId_++;
    listener.once = true;
    listeners_[widget].push_back(listener);
    return listener.id;
}

void EventManager::removeEventListener(int listenerId) {
    for (auto& [widget, list] : listeners_) {
        list.erase(
            std::remove_if(list.begin(), list.end(),
                [listenerId](const EventListener& l) { return l.id == listenerId; }),
            list.end());
    }
}

void EventManager::removeAllListeners(Widget* widget) {
    listeners_.erase(widget);
}

void EventManager::dispatchEvent(Event& event) {
    if (event.target) {
        bubbleEvent(event);
    }
}

void EventManager::emitCustomEvent(Widget* target, const std::string& eventName) {
    Event event;
    event.type = EventType::Custom;
    event.target = target;
    event.customType = eventName;
    dispatchEvent(event);
}

Widget* EventManager::hitTest(Widget* root, int x, int y) {
    if (!root || !root->IsVisible() || !root->IsEnabled()) return nullptr;
    
    // Check children first (front to back, last child is on top)
    const auto& children = root->GetChildren();
    for (int i = static_cast<int>(children.size()) - 1; i >= 0; i--) {
        Widget* hit = hitTest(children[i].get(), x, y);
        if (hit) return hit;
    }
    
    // Check self
    if (root->ContainsPoint(static_cast<float>(x), static_cast<float>(y))) {
        return root;
    }
    
    return nullptr;
}

void EventManager::bubbleEvent(Event& event) {
    Widget* current = event.target;
    while (current && event.propagate && !event.handled) {
        fireListeners(current, event);
        current = current->GetParent();
    }
}

void EventManager::fireListeners(Widget* widget, Event& event) {
    auto it = listeners_.find(widget);
    if (it == listeners_.end()) return;
    
    auto& list = it->second;
    std::vector<int> toRemove;
    
    for (auto& listener : list) {
        if (listener.type != event.type) continue;
        listener.handler(event);
        if (listener.once) toRemove.push_back(listener.id);
        if (event.handled) break;
    }
    
    // Remove once listeners
    for (int id : toRemove) {
        list.erase(
            std::remove_if(list.begin(), list.end(),
                [id](const EventListener& l) { return l.id == id; }),
            list.end());
    }
}

// ========================================================================
// معالجة إدخال عالية المستوى
// ========================================================================

void EventManager::processMouseMove(Widget* root, int x, int y,
                                     bool shift, bool ctrl, bool alt) {
    Widget* hovered = hitTest(root, x, y);
    
    // HoverLeave old
    if (lastHovered_ && lastHovered_ != hovered) {
        Event leave;
        leave.type = EventType::HoverLeave;
        leave.target = lastHovered_;
        leave.mouse = {x, y, 0, 0, shift, ctrl, alt};
        dispatchEvent(leave);
        lastHovered_->SetState(WidgetState::Normal);
    }
    
    // HoverEnter new
    if (hovered && hovered != lastHovered_) {
        Event enter;
        enter.type = EventType::HoverEnter;
        enter.target = hovered;
        enter.mouse = {x, y, 0, 0, shift, ctrl, alt};
        dispatchEvent(enter);
        hovered->SetState(WidgetState::Hovered);
    }
    
    lastHovered_ = hovered;
    
    // MouseMove event
    if (hovered) {
        Event move;
        move.type = EventType::MouseMove;
        move.target = hovered;
        move.mouse = {x, y, 0, 0, shift, ctrl, alt};
        dispatchEvent(move);
    }
    
    // Drag processing
    if (dragging_) processDrag(x, y);
}

void EventManager::processMouseDown(Widget* root, int x, int y, int button) {
    Widget* target = hitTest(root, x, y);
    if (!target) return;
    
    target->SetState(WidgetState::Pressed);
    
    Event down;
    down.type = EventType::MouseDown;
    down.target = target;
    down.mouse = {x, y, button, 0, false, false, false};
    dispatchEvent(down);
    
    // Start drag tracking
    dragging_ = true;
    dragState_.startX = x;
    dragState_.startY = y;
    dragState_.currentX = x;
    dragState_.currentY = y;
    dragState_.source = target;
}

void EventManager::processMouseUp(Widget* root, int x, int y, int button) {
    Widget* target = hitTest(root, x, y);
    
    // MouseUp
    if (target) {
        Event up;
        up.type = EventType::MouseUp;
        up.target = target;
        up.mouse = {x, y, button, 0, false, false, false};
        dispatchEvent(up);
        
        target->SetState(WidgetState::Hovered);
    }
    
    // Click (if released on same widget as pressed)
    if (target && dragging_ && target == dragState_.source) {
        int dx = x - dragState_.startX;
        int dy = y - dragState_.startY;
        if (dx * dx + dy * dy < 25) { // < 5px moved = click
            EventType clickType = EventType::Click;
            
            // Double-click detection
            auto now = std::chrono::steady_clock::now();
            float nowMs = std::chrono::duration<float, std::milli>(
                now.time_since_epoch()).count();
            if (nowMs - lastClickTime_ < 400 &&
                std::abs(x - lastClickX_) < 5 &&
                std::abs(y - lastClickY_) < 5) {
                clickType = EventType::DoubleClick;
            }
            lastClickTime_ = nowMs;
            lastClickX_ = x;
            lastClickY_ = y;
            
            Event click;
            click.type = clickType;
            click.target = target;
            click.mouse = {x, y, button, 0, false, false, false};
            dispatchEvent(click);
        }
    }
    
    // DragEnd
    if (dragging_) {
        Event dragEnd;
        dragEnd.type = EventType::DragEnd;
        dragEnd.target = dragState_.source;
        dragEnd.drag = dragState_;
        dragEnd.drag.target = target;
        dispatchEvent(dragEnd);
        
        // Drop
        if (target && target != dragState_.source) {
            Event drop;
            drop.type = EventType::Drop;
            drop.target = target;
            drop.drag = dragState_;
            drop.drag.target = target;
            dispatchEvent(drop);
        }
        
        dragging_ = false;
    }
}

void EventManager::processDrag(int x, int y) {
    if (!dragging_) return;
    
    int dx = x - dragState_.startX;
    int dy = y - dragState_.startY;
    
    // Only emit DragStart if moved enough
    if (dx * dx + dy * dy >= 25 && dragState_.currentX == dragState_.startX &&
        dragState_.currentY == dragState_.startY) {
        Event start;
        start.type = EventType::DragStart;
        start.target = dragState_.source;
        start.drag = dragState_;
        dispatchEvent(start);
    }
    
    dragState_.currentX = x;
    dragState_.currentY = y;
    
    Event move;
    move.type = EventType::DragMove;
    move.target = dragState_.source;
    move.drag = dragState_;
    dispatchEvent(move);
}

void EventManager::processKeyDown(Widget* focused, int keyCode,
                                   bool shift, bool ctrl, bool alt) {
    if (!focused) return;
    
    Event event;
    event.type = EventType::KeyDown;
    event.target = focused;
    event.key = {keyCode, 0, shift, ctrl, alt, ""};
    dispatchEvent(event);
}

void EventManager::processKeyUp(Widget* focused, int keyCode) {
    if (!focused) return;
    
    Event event;
    event.type = EventType::KeyUp;
    event.target = focused;
    event.key = {keyCode, 0, false, false, false, ""};
    dispatchEvent(event);
}

} // namespace ui
} // namespace graphics
} // namespace sad
