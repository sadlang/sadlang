// ═══════════════════════════════════════════════════════════════════════════════
// sad_state_profiler.h — ربط آلة الحالة والمحلل بلغة ص
// State Machine & Profiler Binding for SAD Language
// ═══════════════════════════════════════════════════════════════════════════════
#pragma once

#include <string>
#include <vector>

namespace sad {
namespace stdlib {
namespace graphics {

// ═══════════════════════════════════════════════════════════════
// آلة الحالة / State Machine
// ═══════════════════════════════════════════════════════════════

/// إنشاء آلة حالة / Create state machine
int statemachine_create_impl(const std::string& initialState);

/// تدمير آلة حالة / Destroy state machine
void statemachine_destroy_impl(int smId);

/// إضافة حالة جديدة / Add state
void statemachine_add_state_impl(int smId, const std::string& stateName);

/// إضافة انتقال / Add transition
void statemachine_add_transition_impl(int smId, const std::string& from, 
                                       const std::string& to, const std::string& trigger);

/// تشغيل حدث / Fire trigger
bool statemachine_trigger_impl(int smId, const std::string& trigger);

/// الحصول على الحالة الحالية / Get current state
std::string statemachine_current_impl(int smId);

/// هل الحالة موجودة؟ / Does state exist?
bool statemachine_has_state_impl(int smId, const std::string& stateName);

/// عدد الحالات / State count
int statemachine_state_count_impl(int smId);

/// إعادة تعيين للحالة الأولية / Reset to initial state
void statemachine_reset_impl(int smId);

/// الحصول على تاريخ الحالات / Get state history
std::vector<std::string> statemachine_history_impl(int smId);

// ═══════════════════════════════════════════════════════════════
// محلل الأداء / Profiler
// ═══════════════════════════════════════════════════════════════

/// بدء جلسة تحليل / Start profiling session
int profiler_create_impl();

/// تدمير المحلل / Destroy profiler
void profiler_destroy_impl(int profilerId);

/// بدء عينة / Begin sample
void profiler_begin_impl(int profilerId, const std::string& name);

/// إنهاء عينة / End sample
double profiler_end_impl(int profilerId);

/// بدء إطار جديد / Begin new frame
void profiler_frame_begin_impl(int profilerId);

/// إنهاء الإطار / End frame
void profiler_frame_end_impl(int profilerId);

/// الحصول على FPS / Get FPS
double profiler_fps_impl(int profilerId);

/// الحصول على وقت الإطار / Get frame time in ms
double profiler_frame_time_impl(int profilerId);

/// الحصول على عدد الإطارات / Get frame count
int profiler_frame_count_impl(int profilerId);

/// تسجيل تخصيص ذاكرة / Record memory allocation
void profiler_alloc_impl(int profilerId, int bytes, const std::string& tag);

/// تسجيل تحرير ذاكرة / Record memory deallocation
void profiler_dealloc_impl(int profilerId, int bytes);

/// الحصول على الذاكرة المستخدمة / Get current memory usage
int profiler_memory_impl(int profilerId);

/// الحصول على ذروة الذاكرة / Get peak memory usage
int profiler_peak_memory_impl(int profilerId);

/// إعادة تعيين المحلل / Reset profiler
void profiler_reset_impl(int profilerId);

} // namespace graphics
} // namespace stdlib
} // namespace sad
