// ============================================================================
// AUTO-GENERATED FROM language-truth/types.yaml — DO NOT EDIT MANUALLY
// (AR) تخطيط البُنى المضمَّنة (حدث). عدّل types.yaml (حقول حدث) وأعد البناء.
// (EN) Built-in struct layouts (حدث). To modify, edit types.yaml and rebuild.
// ============================================================================

#pragma once

#include <array>
#include <cstdint>
#include <string_view>
#include "sad_type_kind_generated.h"

namespace Sad
{
    namespace Types
    {
        namespace EventLayout
        {
            // (AR) خانة الجسر ثلاثيّة الأنواع (تبسيط ABI): i64/double/مؤشّر
            // (EN) Tri-typed bridge slot (ABI simplification): i64/double/pointer
            enum class AbiSlot : int { I64, F64, PTR };

            // (AR) POD الجسر: يملؤه وقت التشغيل من EventData ويقرؤه thunk المترجم
            // (EN) Bridge POD: filled by runtime from EventData, read by compiler thunk
            struct SadEventPod
            {
                double x = {}; // س ← x
                double y = {}; // ص ← y
                double deltaX = {}; // دلتا_س ← deltaX
                double deltaY = {}; // دلتا_ص ← deltaY
                int64_t keyCode = {}; // رمز_المفتاح ← keyCode
                const char *keyName = {}; // اسم_المفتاح ← keyName
                int64_t shiftKey = {}; // مفتاح_عالي ← shiftKey
                int64_t ctrlKey = {}; // مفتاح_تحكم ← ctrlKey
                int64_t altKey = {}; // مفتاح_بديل ← altKey
                int64_t button = {}; // زر ← button
                int64_t touchId = {}; // معرف_اللمس ← touchId
                int64_t fingerId = {}; // معرف_الإصبع ← fingerId
                double pressure = {}; // ضغط ← pressure
                int64_t touchCount = {}; // عدد_اللمسات ← touchCount
                int64_t isTouch = {}; // هل_لمس ← isTouch
                double angle = {}; // زاوية ← angle
                const char *value = {}; // قيمة ← value
                const char *customData = {}; // بيانات ← customData
                int64_t eventType = {}; // نوع ← __type
            };

            // (AR) وصف حقل: الاسم العربيّ + SadTypeKind + خانة ABI — بترتيب البنية
            // (EN) Field descriptor: Arabic name + SadTypeKind + ABI slot — struct order
            struct SadEventField
            {
                std::string_view nameUtf8; // (AR) الاسم العربيّ (مفتاح خريطة المفسّر/حقل بنية المترجم)
                std::string_view source;   // (AR) عضو EventData الإنجليزيّ المصدر (أو __type لنوع الحدث)
                SadTypeKind      kind;
                AbiSlot          abi;
            };

            inline constexpr std::array<SadEventField, 19> SAD_EVENT_FIELDS = {{
                SadEventField{ "\xd8\xb3", "x", SadTypeKind::Float, AbiSlot::F64 }, // س ← x
                SadEventField{ "\xd8\xb5", "y", SadTypeKind::Float, AbiSlot::F64 }, // ص ← y
                SadEventField{ "\xd8\xaf\xd9\x84\xd8\xaa\xd8\xa7\x5f\xd8\xb3", "deltaX", SadTypeKind::Float, AbiSlot::F64 }, // دلتا_س ← deltaX
                SadEventField{ "\xd8\xaf\xd9\x84\xd8\xaa\xd8\xa7\x5f\xd8\xb5", "deltaY", SadTypeKind::Float, AbiSlot::F64 }, // دلتا_ص ← deltaY
                SadEventField{ "\xd8\xb1\xd9\x85\xd8\xb2\x5f\xd8\xa7\xd9\x84\xd9\x85\xd9\x81\xd8\xaa\xd8\xa7\xd8\xad", "keyCode", SadTypeKind::Integer, AbiSlot::I64 }, // رمز_المفتاح ← keyCode
                SadEventField{ "\xd8\xa7\xd8\xb3\xd9\x85\x5f\xd8\xa7\xd9\x84\xd9\x85\xd9\x81\xd8\xaa\xd8\xa7\xd8\xad", "keyName", SadTypeKind::String, AbiSlot::PTR }, // اسم_المفتاح ← keyName
                SadEventField{ "\xd9\x85\xd9\x81\xd8\xaa\xd8\xa7\xd8\xad\x5f\xd8\xb9\xd8\xa7\xd9\x84\xd9\x8a", "shiftKey", SadTypeKind::Boolean, AbiSlot::I64 }, // مفتاح_عالي ← shiftKey
                SadEventField{ "\xd9\x85\xd9\x81\xd8\xaa\xd8\xa7\xd8\xad\x5f\xd8\xaa\xd8\xad\xd9\x83\xd9\x85", "ctrlKey", SadTypeKind::Boolean, AbiSlot::I64 }, // مفتاح_تحكم ← ctrlKey
                SadEventField{ "\xd9\x85\xd9\x81\xd8\xaa\xd8\xa7\xd8\xad\x5f\xd8\xa8\xd8\xaf\xd9\x8a\xd9\x84", "altKey", SadTypeKind::Boolean, AbiSlot::I64 }, // مفتاح_بديل ← altKey
                SadEventField{ "\xd8\xb2\xd8\xb1", "button", SadTypeKind::Integer, AbiSlot::I64 }, // زر ← button
                SadEventField{ "\xd9\x85\xd8\xb9\xd8\xb1\xd9\x81\x5f\xd8\xa7\xd9\x84\xd9\x84\xd9\x85\xd8\xb3", "touchId", SadTypeKind::Integer, AbiSlot::I64 }, // معرف_اللمس ← touchId
                SadEventField{ "\xd9\x85\xd8\xb9\xd8\xb1\xd9\x81\x5f\xd8\xa7\xd9\x84\xd8\xa5\xd8\xb5\xd8\xa8\xd8\xb9", "fingerId", SadTypeKind::Integer, AbiSlot::I64 }, // معرف_الإصبع ← fingerId
                SadEventField{ "\xd8\xb6\xd8\xba\xd8\xb7", "pressure", SadTypeKind::Float, AbiSlot::F64 }, // ضغط ← pressure
                SadEventField{ "\xd8\xb9\xd8\xaf\xd8\xaf\x5f\xd8\xa7\xd9\x84\xd9\x84\xd9\x85\xd8\xb3\xd8\xa7\xd8\xaa", "touchCount", SadTypeKind::Integer, AbiSlot::I64 }, // عدد_اللمسات ← touchCount
                SadEventField{ "\xd9\x87\xd9\x84\x5f\xd9\x84\xd9\x85\xd8\xb3", "isTouch", SadTypeKind::Boolean, AbiSlot::I64 }, // هل_لمس ← isTouch
                SadEventField{ "\xd8\xb2\xd8\xa7\xd9\x88\xd9\x8a\xd8\xa9", "angle", SadTypeKind::Float, AbiSlot::F64 }, // زاوية ← angle
                SadEventField{ "\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9", "value", SadTypeKind::String, AbiSlot::PTR }, // قيمة ← value
                SadEventField{ "\xd8\xa8\xd9\x8a\xd8\xa7\xd9\x86\xd8\xa7\xd8\xaa", "customData", SadTypeKind::String, AbiSlot::PTR }, // بيانات ← customData
                SadEventField{ "\xd9\x86\xd9\x88\xd8\xb9", "__type", SadTypeKind::Integer, AbiSlot::I64 }, // نوع ← __type
            }};

            inline constexpr int SAD_EVENT_FIELD_COUNT = 19;

            // (AR) اسم البنية بلغة ص (من types.yaml: word) — يُشتقّ منه المترجم
            //      صنفَ «حدث» المضمَّن فلا يُكتب الاسم حرفيًّا في كود المترجم.
            // (EN) Sad-language struct name (types.yaml: word) — the compiler
            //      derives its built-in event class from this (no literal in code).
            inline constexpr std::string_view SAD_EVENT_STRUCT_NAME = "\xd8\xad\xd8\xaf\xd8\xab"; // حدث

            // (AR) تعبئة POD من EventData — للتضمين في وقت التشغيل فقط:
            //      عرّف SAD_EVENT_POD_WITH_EVENTDATA وضمّن types.h قبل هذا الرأس.
            // (EN) Fill POD from EventData — runtime-side only: define
            //      SAD_EVENT_POD_WITH_EVENTDATA and include types.h before this header.
#ifdef SAD_EVENT_POD_WITH_EVENTDATA
            inline void sadFillEventPod(const ::sad::ui::EventData &e,
                                        int64_t eventType, SadEventPod &pod)
            {
                pod.x = static_cast<double>(e.x);
                pod.y = static_cast<double>(e.y);
                pod.deltaX = static_cast<double>(e.deltaX);
                pod.deltaY = static_cast<double>(e.deltaY);
                pod.keyCode = static_cast<int64_t>(e.keyCode);
                pod.keyName = e.keyName.c_str();
                pod.shiftKey = e.shiftKey ? 1 : 0;
                pod.ctrlKey = e.ctrlKey ? 1 : 0;
                pod.altKey = e.altKey ? 1 : 0;
                pod.button = static_cast<int64_t>(e.button);
                pod.touchId = static_cast<int64_t>(e.touchId);
                pod.fingerId = static_cast<int64_t>(e.fingerId);
                pod.pressure = static_cast<double>(e.pressure);
                pod.touchCount = static_cast<int64_t>(e.touchCount);
                pod.isTouch = e.isTouch ? 1 : 0;
                pod.angle = static_cast<double>(e.angle);
                pod.value = e.value.c_str();
                pod.customData = e.customData.c_str();
                pod.eventType = eventType;
            }
#endif // SAD_EVENT_POD_WITH_EVENTDATA

        } // namespace EventLayout
    } // namespace Types
} // namespace Sad
