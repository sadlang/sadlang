/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: keyboard_processor.cpp
 * المسار: features/graphics/core/src/keyboard_processor.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * (AR) تنفيذ معالج أحداث لوحة المفاتيح الموحد
 *
 * المنطق الأساسي:
 *   onKeyDown → إطلاق OnKeyDown + معالجة Tab/Enter/Space/Escape + تحرير نصوص
 *   onKeyUp   → إطلاق OnKeyUp
 *   onTextInput → تمرير النص للعقدة المُركّزة عبر callback
 *
 * (EN) Implementation of the unified keyboard event processor.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/keyboard_processor.h"

namespace sad
{
    namespace ui
    {

        // ═══════════════════════════════════════════════════════════════════════════════
        // الدوال المساعدة
        // ═══════════════════════════════════════════════════════════════════════════════

        const IRNode *KeyboardEventProcessor::getTargetNode() const
        {
            // (AR) الهدف: العقدة المُركّزة إن وُجدت، وإلا جذر المحتوى
            const IRNode *focused = getFocusedNodeCb_ ? getFocusedNodeCb_() : nullptr;
            if (focused)
                return focused;
            return getContentRootCb_ ? getContentRootCb_() : nullptr;
        }

        bool KeyboardEventProcessor::isTextInputNode(const IRNode *node) const
        {
            if (!node)
                return false;
            auto type = node->getType();
            return type == UINodeType::TextField || type == UINodeType::TextArea;
        }

        EventData KeyboardEventProcessor::makeKeyEventData(
            int keyCode, const std::string &keyName,
            const KeyModifiers &mods) const
        {
            EventData data;
            data.keyCode = keyCode;
            data.keyName = keyName;
            data.shiftKey = mods.shift;
            data.ctrlKey = mods.ctrl;
            data.altKey = mods.alt;
            return data;
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        // onKeyDown — ضغط مفتاح
        // ═══════════════════════════════════════════════════════════════════════════════

        void KeyboardEventProcessor::onKeyDown(
            UnifiedKeyCode keyCode, const std::string &keyName,
            const KeyModifiers &mods)
        {
            int rawCode = static_cast<int>(keyCode);

            // ─── 1. إطلاق حدث OnKeyDown على العقدة الهدف ───
            const IRNode *target = getTargetNode();
            if (target && fireEventCb_)
            {
                EventData data = makeKeyEventData(rawCode, keyName, mods);
                for (const auto &evt : target->getEvents())
                {
                    if (evt.type == IREventType::OnKeyDown)
                    {
                        fireEventCb_(evt.type, evt.expression, target, data);
                    }
                }
            }

            // ─── 2. معالجة مفاتيح تحرير النصوص (إذا العقدة حقل إدخال) ───
            const IRNode *focused = getFocusedNodeCb_ ? getFocusedNodeCb_() : nullptr;
            if (focused && isTextInputNode(focused) && handleTextEditKeyCb_)
            {
                if (handleTextEditKeyCb_(focused, keyCode, mods))
                {
                    // تمت المعالجة — لا نتابع (إلا Escape)
                    if (keyCode != UnifiedKeyCode::Escape)
                        return;
                }
            }

            // ─── 3. Tab → تنقل التركيز ───
            if (keyCode == UnifiedKeyCode::Tab)
            {
                handleFocusNavigation(mods.shift);
                return;
            }

            // ─── 4. Enter / Space → تفعيل العنصر المُركّز ───
            if ((keyCode == UnifiedKeyCode::Return || keyCode == UnifiedKeyCode::Space) &&
                focused && !isTextInputNode(focused))
            {
                handleActivation(rawCode, keyName);
                return;
            }

            // ─── 5. Escape → إغلاق ───
            if (keyCode == UnifiedKeyCode::Escape)
            {
                if (closeWindowCb_)
                    closeWindowCb_();
            }
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        // onKeyUp — رفع مفتاح
        // ═══════════════════════════════════════════════════════════════════════════════

        void KeyboardEventProcessor::onKeyUp(
            UnifiedKeyCode keyCode, const std::string &keyName,
            const KeyModifiers &mods)
        {
            int rawCode = static_cast<int>(keyCode);
            const IRNode *target = getTargetNode();
            if (target && fireEventCb_)
            {
                EventData data = makeKeyEventData(rawCode, keyName, mods);
                for (const auto &evt : target->getEvents())
                {
                    if (evt.type == IREventType::OnKeyUp)
                    {
                        fireEventCb_(evt.type, evt.expression, target, data);
                    }
                }
            }
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        // onTextInput — إدخال نص (من IME أو لوحة المفاتيح)
        // ═══════════════════════════════════════════════════════════════════════════════

        void KeyboardEventProcessor::onTextInput(const std::string &text)
        {
            const IRNode *focused = getFocusedNodeCb_ ? getFocusedNodeCb_() : nullptr;
            if (!focused || !isTextInputNode(focused))
                return;

            // (AR) تمرير المعالجة لـ callback المنصة (Desktop: TextEditState + SDL)
            if (handleTextInputCb_)
            {
                handleTextInputCb_(focused, text);
            }

            // (AR) إطلاق حدث OnInput و OnChange
            if (fireEventCb_)
            {
                for (const auto &evt : focused->getEvents())
                {
                    if (evt.type == IREventType::OnInput || evt.type == IREventType::OnChange)
                    {
                        EventData data;
                        data.value = text;
                        fireEventCb_(evt.type, evt.expression, focused, data);
                    }
                }
            }
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        // تنقل التركيز (Tab / Shift+Tab)
        // ═══════════════════════════════════════════════════════════════════════════════

        void KeyboardEventProcessor::handleFocusNavigation(bool reverse)
        {
            if (!collectFocusableCb_ || !setFocusedNodeCb_)
                return;

            std::vector<const IRNode *> focusable;
            collectFocusableCb_(focusable);
            if (focusable.empty())
                return;

            const IRNode *focused = getFocusedNodeCb_ ? getFocusedNodeCb_() : nullptr;
            int idx = -1;
            for (int i = 0; i < static_cast<int>(focusable.size()); ++i)
            {
                if (focusable[i] == focused)
                {
                    idx = i;
                    break;
                }
            }

            if (reverse)
            {
                idx = (idx <= 0) ? static_cast<int>(focusable.size()) - 1 : idx - 1;
            }
            else
            {
                idx = (idx + 1) % static_cast<int>(focusable.size());
            }

            const IRNode *newFocus = focusable[idx];
            if (newFocus != focused)
            {
                setFocusedNodeCb_(newFocus);
                if (invalidateCb_)
                    invalidateCb_();
            }
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        // تفعيل العنصر المُركّز (Enter / Space)
        // ═══════════════════════════════════════════════════════════════════════════════

        void KeyboardEventProcessor::handleActivation(
            int keyCode, const std::string &keyName)
        {
            const IRNode *focused = getFocusedNodeCb_ ? getFocusedNodeCb_() : nullptr;
            if (!focused || !fireEventCb_)
                return;

            auto nodeType = focused->getType();

            // ─── أزرار: تفعيل OnTap ───
            if (nodeType == UINodeType::Button || nodeType == UINodeType::FAB)
            {
                for (const auto &evt : focused->getEvents())
                {
                    if (evt.type == IREventType::OnTap)
                    {
                        EventData data;
                        data.keyCode = keyCode;
                        data.keyName = keyName;
                        fireEventCb_(evt.type, evt.expression, focused, data);
                    }
                }
                if (invalidateCb_)
                    invalidateCb_();
            }
            // ─── Toggle/Checkbox: تبديل القيمة ───
            else if (nodeType == UINodeType::Toggle || nodeType == UINodeType::Checkbox)
            {
                // (AR) تبديل خاصية "مفعّل" مباشرة قبل إطلاق الحدث
                auto *mutableNode = const_cast<IRNode *>(focused);
                const auto *onProp = focused->findProperty(
                    "\xd9\x85\xd9\x81\xd8\xb9\xd9\x84"); // مفعّل
                bool isOn = false;
                if (onProp)
                {
                    if (auto *b = std::get_if<bool>(&onProp->value))
                        isOn = *b;
                }
                mutableNode->setProperty("\xd9\x85\xd9\x81\xd8\xb9\xd9\x84", !isOn);

                for (const auto &evt : focused->getEvents())
                {
                    if (evt.type == IREventType::OnChange)
                    {
                        EventData data;
                        data.keyCode = keyCode;
                        fireEventCb_(evt.type, evt.expression, focused, data);
                    }
                }
                if (invalidateCb_)
                    invalidateCb_();
            }
        }

    } // namespace ui
} // namespace sad
