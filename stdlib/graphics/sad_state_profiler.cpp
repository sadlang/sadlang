// ═══════════════════════════════════════════════════════════════════════════════
// sad_state_profiler.cpp — تنفيذ آلة الحالة والمحلل
// State Machine & Profiler Implementation
// ═══════════════════════════════════════════════════════════════════════════════

#include "sad_state_profiler.h"
#include <map>
#include <unordered_map>
#include <vector>
#include <string>
#include <chrono>
#include <stack>

namespace sad {
namespace stdlib {
namespace graphics {

// ═══════════════════════════════════════════════════════════════
// آلة الحالة — هياكل داخلية / State Machine Internals
// ═══════════════════════════════════════════════════════════════

struct TransitionData {
    std::string from;
    std::string to;
    std::string trigger;
};

struct StateMachineData {
    int id = 0;
    std::string initialState;
    std::string currentState;
    std::vector<std::string> states;
    std::vector<TransitionData> transitions;
    std::vector<std::string> history;
};

static std::map<int, StateMachineData> g_stateMachines;
static int g_nextSmId = 1;

int statemachine_create_impl(const std::string& initialState) {
    int id = g_nextSmId++;
    auto& sm = g_stateMachines[id];
    sm.id = id;
    sm.initialState = initialState;
    sm.currentState = initialState;
    sm.states.push_back(initialState);
    sm.history.push_back(initialState);
    return id;
}

void statemachine_destroy_impl(int smId) {
    g_stateMachines.erase(smId);
}

void statemachine_add_state_impl(int smId, const std::string& stateName) {
    auto it = g_stateMachines.find(smId);
    if (it == g_stateMachines.end()) return;
    for (const auto& s : it->second.states) {
        if (s == stateName) return; // already exists
    }
    it->second.states.push_back(stateName);
}

void statemachine_add_transition_impl(int smId, const std::string& from,
                                       const std::string& to, const std::string& trigger) {
    auto it = g_stateMachines.find(smId);
    if (it == g_stateMachines.end()) return;
    it->second.transitions.push_back({from, to, trigger});
}

bool statemachine_trigger_impl(int smId, const std::string& trigger) {
    auto it = g_stateMachines.find(smId);
    if (it == g_stateMachines.end()) return false;
    for (const auto& t : it->second.transitions) {
        if (t.from == it->second.currentState && t.trigger == trigger) {
            it->second.currentState = t.to;
            it->second.history.push_back(t.to);
            return true;
        }
    }
    return false;
}

std::string statemachine_current_impl(int smId) {
    auto it = g_stateMachines.find(smId);
    if (it == g_stateMachines.end()) return "";
    return it->second.currentState;
}

bool statemachine_has_state_impl(int smId, const std::string& stateName) {
    auto it = g_stateMachines.find(smId);
    if (it == g_stateMachines.end()) return false;
    for (const auto& s : it->second.states) {
        if (s == stateName) return true;
    }
    return false;
}

int statemachine_state_count_impl(int smId) {
    auto it = g_stateMachines.find(smId);
    if (it == g_stateMachines.end()) return 0;
    return static_cast<int>(it->second.states.size());
}

void statemachine_reset_impl(int smId) {
    auto it = g_stateMachines.find(smId);
    if (it == g_stateMachines.end()) return;
    it->second.currentState = it->second.initialState;
    it->second.history.clear();
    it->second.history.push_back(it->second.initialState);
}

std::vector<std::string> statemachine_history_impl(int smId) {
    auto it = g_stateMachines.find(smId);
    if (it == g_stateMachines.end()) return {};
    return it->second.history;
}

// ═══════════════════════════════════════════════════════════════
// المحلل — هياكل داخلية / Profiler Internals
// ═══════════════════════════════════════════════════════════════

using Clock = std::chrono::high_resolution_clock;
using TimePoint = Clock::time_point;

struct SampleData {
    std::string name;
    TimePoint start;
};

struct ProfilerData {
    int id = 0;
    std::stack<SampleData> activeSamples;
    TimePoint frameStart;
    double lastFrameTime = 0.0; // ms
    int frameCount = 0;
    double totalFrameTime = 0.0;
    // ذاكرة / Memory
    int64_t currentMemory = 0;
    int64_t peakMemory = 0;
    int64_t totalAllocated = 0;
};

static std::map<int, ProfilerData> g_profilers;
static int g_nextProfilerId = 1;

int profiler_create_impl() {
    int id = g_nextProfilerId++;
    g_profilers[id].id = id;
    return id;
}

void profiler_destroy_impl(int profilerId) {
    g_profilers.erase(profilerId);
}

void profiler_begin_impl(int profilerId, const std::string& name) {
    auto it = g_profilers.find(profilerId);
    if (it == g_profilers.end()) return;
    it->second.activeSamples.push({name, Clock::now()});
}

double profiler_end_impl(int profilerId) {
    auto it = g_profilers.find(profilerId);
    if (it == g_profilers.end()) return 0.0;
    if (it->second.activeSamples.empty()) return 0.0;
    auto sample = it->second.activeSamples.top();
    it->second.activeSamples.pop();
    auto elapsed = std::chrono::duration<double, std::milli>(Clock::now() - sample.start);
    return elapsed.count();
}

void profiler_frame_begin_impl(int profilerId) {
    auto it = g_profilers.find(profilerId);
    if (it == g_profilers.end()) return;
    it->second.frameStart = Clock::now();
}

void profiler_frame_end_impl(int profilerId) {
    auto it = g_profilers.find(profilerId);
    if (it == g_profilers.end()) return;
    auto elapsed = std::chrono::duration<double, std::milli>(Clock::now() - it->second.frameStart);
    it->second.lastFrameTime = elapsed.count();
    it->second.totalFrameTime += elapsed.count();
    it->second.frameCount++;
}

double profiler_fps_impl(int profilerId) {
    auto it = g_profilers.find(profilerId);
    if (it == g_profilers.end()) return 0.0;
    if (it->second.lastFrameTime <= 0.0) return 0.0;
    return 1000.0 / it->second.lastFrameTime;
}

double profiler_frame_time_impl(int profilerId) {
    auto it = g_profilers.find(profilerId);
    if (it == g_profilers.end()) return 0.0;
    return it->second.lastFrameTime;
}

int profiler_frame_count_impl(int profilerId) {
    auto it = g_profilers.find(profilerId);
    if (it == g_profilers.end()) return 0;
    return it->second.frameCount;
}

void profiler_alloc_impl(int profilerId, int bytes, const std::string& /*tag*/) {
    auto it = g_profilers.find(profilerId);
    if (it == g_profilers.end()) return;
    it->second.currentMemory += bytes;
    it->second.totalAllocated += bytes;
    if (it->second.currentMemory > it->second.peakMemory) {
        it->second.peakMemory = it->second.currentMemory;
    }
}

void profiler_dealloc_impl(int profilerId, int bytes) {
    auto it = g_profilers.find(profilerId);
    if (it == g_profilers.end()) return;
    it->second.currentMemory -= bytes;
    if (it->second.currentMemory < 0) it->second.currentMemory = 0;
}

int profiler_memory_impl(int profilerId) {
    auto it = g_profilers.find(profilerId);
    if (it == g_profilers.end()) return 0;
    return static_cast<int>(it->second.currentMemory);
}

int profiler_peak_memory_impl(int profilerId) {
    auto it = g_profilers.find(profilerId);
    if (it == g_profilers.end()) return 0;
    return static_cast<int>(it->second.peakMemory);
}

void profiler_reset_impl(int profilerId) {
    auto it = g_profilers.find(profilerId);
    if (it == g_profilers.end()) return;
    while (!it->second.activeSamples.empty()) it->second.activeSamples.pop();
    it->second.frameCount = 0;
    it->second.totalFrameTime = 0.0;
    it->second.lastFrameTime = 0.0;
    it->second.currentMemory = 0;
    it->second.peakMemory = 0;
    it->second.totalAllocated = 0;
}

} // namespace graphics
} // namespace stdlib
} // namespace sad
