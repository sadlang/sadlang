// ==============================================================================
// state_machine.h - آلة الحالة / State Machine
// ==============================================================================
// الوصف: نظام إدارة الحالات للألعاب والتطبيقات
// Description: State management system for games and applications
// ==============================================================================
// المميزات / Features:
//   - آلة حالة محدودة (FSM) / Finite State Machine
//   - آلة حالة هرمية (HFSM) / Hierarchical State Machine
//   - انتقالات مشروطة / Conditional transitions
//   - إجراءات دخول/خروج / Enter/exit actions
//   - حالات فرعية / Substates
//   - تاريخ الحالات / State history
// ==============================================================================
// الاستخدام / Usage:
//   // تعريف حالات / Define states
//   enum class PlayerState { Idle, Walking, Running, Jumping };
//
//   // إنشاء آلة الحالة / Create state machine
//   StateMachine<PlayerState> sm(PlayerState::Idle);
//
//   // تعريف الحالات / Define states
//   sm.DefineState(PlayerState::Idle)
//     .OnEnter([]() { /* ... */ })
//     .OnUpdate([](float dt) { /* ... */ })
//     .OnExit([]() { /* ... */ });
//
//   // إضافة انتقال / Add transition
//   sm.AddTransition(PlayerState::Idle, PlayerState::Walking, 
//                    []() { return IsMoving(); });
//
//   // التحديث / Update
//   sm.Update(deltaTime);
// ==============================================================================

#ifndef SAD_GRAPHICS_STATE_MACHINE_H
#define SAD_GRAPHICS_STATE_MACHINE_H

#include "types.h"
#include <functional>
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <stack>
#include <algorithm>

namespace sad {
namespace graphics {

// ==============================================================================
// واجهة الحالة / State Interface
// ==============================================================================

/// واجهة الحالة الأساسية
/// Base state interface
class IState {
public:
    virtual ~IState() = default;
    
    /// عند الدخول للحالة / Called when entering state
    virtual void OnEnter() {}
    
    /// التحديث / Update
    virtual void OnUpdate(Float32 deltaTime) {}
    
    /// الرسم / Draw
    virtual void OnDraw() {}
    
    /// عند الخروج من الحالة / Called when exiting state
    virtual void OnExit() {}
    
    /// الحصول على الاسم / Get name
    virtual const char* GetName() const { return "State"; }
};

// ==============================================================================
// تعريف الحالة / State Definition
// ==============================================================================

/// تعريف حالة مع دوال رد الاتصال
/// State definition with callbacks
template<typename StateId>
class StateDefinition {
public:
    // أنواع دوال رد الاتصال / Callback types
    using EnterCallback = std::function<void()>;
    using UpdateCallback = std::function<void(Float32)>;
    using DrawCallback = std::function<void()>;
    using ExitCallback = std::function<void()>;
    
    StateDefinition(StateId id) : m_id(id) {}
    
    /// تعيين دالة الدخول / Set enter callback
    StateDefinition& OnEnter(EnterCallback callback) {
        m_onEnter = std::move(callback);
        return *this;
    }
    
    /// تعيين دالة التحديث / Set update callback
    StateDefinition& OnUpdate(UpdateCallback callback) {
        m_onUpdate = std::move(callback);
        return *this;
    }
    
    /// تعيين دالة الرسم / Set draw callback
    StateDefinition& OnDraw(DrawCallback callback) {
        m_onDraw = std::move(callback);
        return *this;
    }
    
    /// تعيين دالة الخروج / Set exit callback
    StateDefinition& OnExit(ExitCallback callback) {
        m_onExit = std::move(callback);
        return *this;
    }
    
    /// تعيين الاسم / Set name
    StateDefinition& WithName(const std::string& name) {
        m_name = name;
        return *this;
    }
    
    // الوصول للدوال / Accessors
    StateId GetId() const { return m_id; }
    const std::string& GetName() const { return m_name; }
    
    void Enter() const { if (m_onEnter) m_onEnter(); }
    void Update(Float32 dt) const { if (m_onUpdate) m_onUpdate(dt); }
    void Draw() const { if (m_onDraw) m_onDraw(); }
    void Exit() const { if (m_onExit) m_onExit(); }

private:
    StateId m_id;
    std::string m_name;
    EnterCallback m_onEnter;
    UpdateCallback m_onUpdate;
    DrawCallback m_onDraw;
    ExitCallback m_onExit;
};

// ==============================================================================
// الانتقال / Transition
// ==============================================================================

/// شرط الانتقال / Transition condition
using Condition = std::function<bool()>;

/// تعريف الانتقال / Transition definition
template<typename StateId>
struct Transition {
    StateId from;           // من حالة / From state
    StateId to;             // إلى حالة / To state
    Condition condition;    // الشرط / Condition
    Int32 priority;         // الأولوية (أعلى = يُفحص أولاً) / Priority
    
    bool Check() const {
        return condition ? condition() : false;
    }
};

// ==============================================================================
// آلة الحالة المحدودة / Finite State Machine
// ==============================================================================

/// آلة الحالة المحدودة
/// Finite State Machine (FSM)
/// 
/// نظام بسيط وقوي لإدارة الحالات
/// يدعم الانتقالات المشروطة والدوال المخصصة
template<typename StateId>
class StateMachine {
public:
    // ==========================================================================
    // البناء / Construction
    // ==========================================================================
    
    /// منشئ مع الحالة الابتدائية / Constructor with initial state
    explicit StateMachine(StateId initialState)
        : m_currentState(initialState)
        , m_previousState(initialState)
        , m_started(false)
    {}
    
    // ==========================================================================
    // تعريف الحالات / State Definition
    // ==========================================================================
    
    /// تعريف حالة جديدة / Define new state
    StateDefinition<StateId>& DefineState(StateId id) {
        m_states[id] = std::make_unique<StateDefinition<StateId>>(id);
        return *m_states[id];
    }
    
    /// الحصول على تعريف حالة / Get state definition
    StateDefinition<StateId>* GetState(StateId id) {
        auto it = m_states.find(id);
        return (it != m_states.end()) ? it->second.get() : nullptr;
    }
    
    // ==========================================================================
    // الانتقالات / Transitions
    // ==========================================================================
    
    /// إضافة انتقال مشروط / Add conditional transition
    void AddTransition(StateId from, StateId to, Condition condition, Int32 priority = 0) {
        m_transitions.push_back({from, to, std::move(condition), priority});
        
        // ترتيب حسب الأولوية / Sort by priority
        std::sort(m_transitions.begin(), m_transitions.end(),
            [](const auto& a, const auto& b) {
                return a.priority > b.priority;
            });
    }
    
    /// إضافة انتقال فوري (بدون شرط) / Add immediate transition (no condition)
    void AddTransition(StateId from, StateId to) {
        AddTransition(from, to, []() { return true; }, -1000);
    }
    
    /// إضافة انتقال ثنائي الاتجاه / Add bidirectional transition
    void AddBidirectionalTransition(StateId a, StateId b, 
                                     Condition aToB, Condition bToA,
                                     Int32 priority = 0) {
        AddTransition(a, b, std::move(aToB), priority);
        AddTransition(b, a, std::move(bToA), priority);
    }
    
    /// إضافة انتقال من أي حالة / Add transition from any state
    void AddGlobalTransition(StateId to, Condition condition, Int32 priority = 100) {
        m_globalTransitions.push_back({StateId{}, to, std::move(condition), priority});
        
        std::sort(m_globalTransitions.begin(), m_globalTransitions.end(),
            [](const auto& a, const auto& b) {
                return a.priority > b.priority;
            });
    }
    
    // ==========================================================================
    // التحكم / Control
    // ==========================================================================
    
    /// البدء / Start
    void Start() {
        if (m_started) return;
        m_started = true;
        
        EnterState(m_currentState);
    }
    
    /// التحديث / Update
    void Update(Float32 deltaTime) {
        if (!m_started) {
            Start();
        }
        
        // فحص الانتقالات العامة أولاً / Check global transitions first
        for (const auto& trans : m_globalTransitions) {
            if (trans.Check() && trans.to != m_currentState) {
                ChangeState(trans.to);
                break;
            }
        }
        
        // فحص الانتقالات العادية / Check normal transitions
        for (const auto& trans : m_transitions) {
            if (trans.from == m_currentState && trans.Check()) {
                ChangeState(trans.to);
                break;
            }
        }
        
        // تحديث الحالة الحالية / Update current state
        auto* state = GetState(m_currentState);
        if (state) {
            state->Update(deltaTime);
        }
    }
    
    /// الرسم / Draw
    void Draw() {
        auto* state = GetState(m_currentState);
        if (state) {
            state->Draw();
        }
    }
    
    /// تغيير الحالة يدوياً / Change state manually
    void SetState(StateId newState) {
        if (newState == m_currentState) return;
        ChangeState(newState);
    }
    
    /// إعادة التعيين / Reset
    void Reset(StateId initialState) {
        if (m_started) {
            ExitState(m_currentState);
        }
        
        m_currentState = initialState;
        m_previousState = initialState;
        m_started = false;
        
        while (!m_stateHistory.empty()) m_stateHistory.pop();
    }
    
    // ==========================================================================
    // الاستعلامات / Queries
    // ==========================================================================
    
    /// الحالة الحالية / Current state
    StateId GetCurrentState() const { return m_currentState; }
    
    /// الحالة السابقة / Previous state
    StateId GetPreviousState() const { return m_previousState; }
    
    /// هل في حالة معينة / Is in specific state
    bool IsInState(StateId state) const { return m_currentState == state; }
    
    /// هل بدأت / Has started
    bool HasStarted() const { return m_started; }
    
    /// الوقت في الحالة الحالية / Time in current state
    Float32 GetTimeInCurrentState() const { return m_timeInState; }
    
    // ==========================================================================
    // التاريخ / History
    // ==========================================================================
    
    /// تفعيل التاريخ / Enable history
    void EnableHistory(bool enabled, size_t maxSize = 10) {
        m_historyEnabled = enabled;
        m_historyMaxSize = maxSize;
    }
    
    /// العودة للحالة السابقة / Go back to previous state
    bool GoBack() {
        if (m_stateHistory.empty()) return false;
        
        StateId prev = m_stateHistory.top();
        m_stateHistory.pop();
        
        ChangeState(prev, false);  // لا نضيف للتاريخ
        return true;
    }

private:
    /// الدخول لحالة / Enter state
    void EnterState(StateId state) {
        auto* s = GetState(state);
        if (s) {
            s->Enter();
        }
        m_timeInState = 0.0f;
    }
    
    /// الخروج من حالة / Exit state
    void ExitState(StateId state) {
        auto* s = GetState(state);
        if (s) {
            s->Exit();
        }
    }
    
    /// تغيير الحالة / Change state
    void ChangeState(StateId newState, bool addToHistory = true) {
        if (m_historyEnabled && addToHistory) {
            if (m_stateHistory.size() >= m_historyMaxSize) {
                // إزالة الأقدم (نحتاج تحويل لـ vector)
                std::stack<StateId> temp;
                while (m_stateHistory.size() > 1) {
                    temp.push(m_stateHistory.top());
                    m_stateHistory.pop();
                }
                m_stateHistory.pop(); // إزالة الأقدم
                while (!temp.empty()) {
                    m_stateHistory.push(temp.top());
                    temp.pop();
                }
            }
            m_stateHistory.push(m_currentState);
        }
        
        ExitState(m_currentState);
        
        m_previousState = m_currentState;
        m_currentState = newState;
        
        EnterState(m_currentState);
    }
    
    // الحالات / States
    std::unordered_map<StateId, std::unique_ptr<StateDefinition<StateId>>> m_states;
    
    // الانتقالات / Transitions
    std::vector<Transition<StateId>> m_transitions;
    std::vector<Transition<StateId>> m_globalTransitions;
    
    // الحالة الحالية والسابقة / Current and previous state
    StateId m_currentState;
    StateId m_previousState;
    
    // التتبع / Tracking
    Float32 m_timeInState = 0.0f;
    bool m_started = false;
    
    // التاريخ / History
    std::stack<StateId> m_stateHistory;
    bool m_historyEnabled = false;
    size_t m_historyMaxSize = 10;
};

// ==============================================================================
// آلة الحالة الهرمية / Hierarchical State Machine
// ==============================================================================

/// حالة هرمية يمكن أن تحتوي على حالات فرعية
/// Hierarchical state that can contain substates
template<typename StateId>
class HierarchicalState {
public:
    HierarchicalState(StateId id) : m_id(id) {}
    
    /// إضافة حالة فرعية / Add substate
    HierarchicalState& AddSubstate(StateId substateId) {
        m_substates.push_back(substateId);
        return *this;
    }
    
    /// تعيين الحالة الفرعية الافتراضية / Set default substate
    HierarchicalState& SetDefaultSubstate(StateId substateId) {
        m_defaultSubstate = substateId;
        m_hasDefaultSubstate = true;
        return *this;
    }
    
    /// هل لها حالات فرعية / Has substates
    bool HasSubstates() const { return !m_substates.empty(); }
    
    /// الحالات الفرعية / Substates
    const std::vector<StateId>& GetSubstates() const { return m_substates; }
    
    /// الحالة الفرعية الافتراضية / Default substate  
    StateId GetDefaultSubstate() const { return m_defaultSubstate; }
    bool HasDefaultSubstate() const { return m_hasDefaultSubstate; }
    
    StateId GetId() const { return m_id; }

private:
    StateId m_id;
    std::vector<StateId> m_substates;
    StateId m_defaultSubstate;
    bool m_hasDefaultSubstate = false;
};

/// آلة الحالة الهرمية
/// Hierarchical State Machine (HFSM)
/// 
/// تدعم الحالات المتداخلة للأنظمة المعقدة
template<typename StateId>
class HierarchicalStateMachine {
public:
    explicit HierarchicalStateMachine(StateId initialState)
        : m_fsm(initialState)
    {}
    
    /// تعريف حالة هرمية / Define hierarchical state
    HierarchicalState<StateId>& DefineHierarchicalState(StateId id) {
        m_hierarchy[id] = std::make_unique<HierarchicalState<StateId>>(id);
        return *m_hierarchy[id];
    }
    
    /// تعريف حالة عادية / Define regular state
    StateDefinition<StateId>& DefineState(StateId id) {
        return m_fsm.DefineState(id);
    }
    
    /// إضافة انتقال / Add transition
    void AddTransition(StateId from, StateId to, Condition condition, Int32 priority = 0) {
        m_fsm.AddTransition(from, to, std::move(condition), priority);
    }
    
    /// التحديث / Update
    void Update(Float32 deltaTime) {
        m_fsm.Update(deltaTime);
        
        // تحديث الحالة الفرعية الفعالة / Update active substate
        StateId current = m_fsm.GetCurrentState();
        auto* hier = GetHierarchicalState(current);
        
        if (hier && hier->HasSubstates()) {
            // إذا لم تكن هناك حالة فرعية فعالة، استخدم الافتراضية
            if (!m_activeSubstates.count(current)) {
                if (hier->HasDefaultSubstate()) {
                    m_activeSubstates[current] = hier->GetDefaultSubstate();
                }
            }
        }
    }
    
    /// الرسم / Draw
    void Draw() {
        m_fsm.Draw();
    }
    
    /// الحالة الحالية / Current state
    StateId GetCurrentState() const { return m_fsm.GetCurrentState(); }
    
    /// الحالة الفرعية الفعالة / Active substate
    StateId GetActiveSubstate(StateId parent) const {
        auto it = m_activeSubstates.find(parent);
        return (it != m_activeSubstates.end()) ? it->second : StateId{};
    }

private:
    HierarchicalState<StateId>* GetHierarchicalState(StateId id) {
        auto it = m_hierarchy.find(id);
        return (it != m_hierarchy.end()) ? it->second.get() : nullptr;
    }
    
    StateMachine<StateId> m_fsm;
    std::unordered_map<StateId, std::unique_ptr<HierarchicalState<StateId>>> m_hierarchy;
    std::unordered_map<StateId, StateId> m_activeSubstates;
};

// ==============================================================================
// آلة حالة بسيطة بدون قوالب / Simple State Machine without templates
// ==============================================================================

/// آلة حالة تستخدم سلاسل نصية كمعرفات
/// State machine using strings as identifiers
class SimpleStateMachine {
public:
    using Callback = std::function<void()>;
    using UpdateCallback = std::function<void(Float32)>;
    using Condition = std::function<bool()>;
    
    /// تعريف حالة / Define state
    SimpleStateMachine& State(const std::string& name) {
        m_currentDefinition = name;
        m_states[name] = StateData{};
        return *this;
    }
    
    /// عند الدخول / On enter
    SimpleStateMachine& OnEnter(Callback callback) {
        m_states[m_currentDefinition].onEnter = std::move(callback);
        return *this;
    }
    
    /// عند التحديث / On update
    SimpleStateMachine& OnUpdate(UpdateCallback callback) {
        m_states[m_currentDefinition].onUpdate = std::move(callback);
        return *this;
    }
    
    /// عند الخروج / On exit
    SimpleStateMachine& OnExit(Callback callback) {
        m_states[m_currentDefinition].onExit = std::move(callback);
        return *this;
    }
    
    /// إضافة انتقال / Add transition
    SimpleStateMachine& Transition(const std::string& from, 
                                   const std::string& to, 
                                   Condition condition) {
        m_transitions.push_back({from, to, std::move(condition)});
        return *this;
    }
    
    /// تعيين الحالة الابتدائية / Set initial state
    SimpleStateMachine& InitialState(const std::string& state) {
        m_currentState = state;
        return *this;
    }
    
    /// البدء / Start
    void Start() {
        if (m_started) return;
        m_started = true;
        
        auto it = m_states.find(m_currentState);
        if (it != m_states.end() && it->second.onEnter) {
            it->second.onEnter();
        }
    }
    
    /// التحديث / Update
    void Update(Float32 deltaTime) {
        if (!m_started) Start();
        
        // فحص الانتقالات / Check transitions
        for (const auto& trans : m_transitions) {
            if (trans.from == m_currentState && trans.condition()) {
                SetState(trans.to);
                break;
            }
        }
        
        // تحديث الحالة / Update state
        auto it = m_states.find(m_currentState);
        if (it != m_states.end() && it->second.onUpdate) {
            it->second.onUpdate(deltaTime);
        }
    }
    
    /// تغيير الحالة / Change state
    void SetState(const std::string& newState) {
        if (newState == m_currentState) return;
        
        // خروج / Exit
        auto oldIt = m_states.find(m_currentState);
        if (oldIt != m_states.end() && oldIt->second.onExit) {
            oldIt->second.onExit();
        }
        
        m_previousState = m_currentState;
        m_currentState = newState;
        
        // دخول / Enter
        auto newIt = m_states.find(m_currentState);
        if (newIt != m_states.end() && newIt->second.onEnter) {
            newIt->second.onEnter();
        }
    }
    
    /// الحالة الحالية / Current state
    const std::string& GetCurrentState() const { return m_currentState; }
    
    /// الحالة السابقة / Previous state
    const std::string& GetPreviousState() const { return m_previousState; }
    
    /// هل في حالة معينة / Is in state
    bool IsInState(const std::string& state) const { 
        return m_currentState == state; 
    }

private:
    struct StateData {
        Callback onEnter;
        UpdateCallback onUpdate;
        Callback onExit;
    };
    
    struct TransitionData {
        std::string from;
        std::string to;
        Condition condition;
    };
    
    std::unordered_map<std::string, StateData> m_states;
    std::vector<TransitionData> m_transitions;
    
    std::string m_currentDefinition;
    std::string m_currentState;
    std::string m_previousState;
    bool m_started = false;
};

// ==============================================================================
// حالات شائعة للألعاب / Common Game States
// ==============================================================================

/// حالات اللعبة الشائعة / Common game states
enum class GameState {
    MainMenu,       // القائمة الرئيسية / Main menu
    Loading,        // التحميل / Loading
    Playing,        // اللعب / Playing
    Paused,         // متوقف مؤقتاً / Paused
    GameOver,       // انتهت اللعبة / Game over
    Victory,        // الفوز / Victory
    Settings,       // الإعدادات / Settings
    Credits         // التقديرات / Credits
};

/// حالات الشخصية الشائعة / Common character states
enum class CharacterState {
    Idle,           // خامل / Idle
    Walking,        // يمشي / Walking
    Running,        // يجري / Running
    Jumping,        // يقفز / Jumping
    Falling,        // يسقط / Falling
    Attacking,      // يهاجم / Attacking
    Defending,      // يدافع / Defending
    TakingDamage,   // يتلقى ضرراً / Taking damage
    Dying,          // يموت / Dying
    Dead            // ميت / Dead
};

/// حالات العدو الشائعة / Common enemy states
enum class EnemyState {
    Idle,           // خامل / Idle
    Patrol,         // دورية / Patrol
    Chase,          // مطاردة / Chase
    Attack,         // هجوم / Attack
    Retreat,        // انسحاب / Retreat
    Stunned,        // مصعوق / Stunned
    Dead            // ميت / Dead
};

/// حالات واجهة المستخدم / UI states
enum class UIState {
    Hidden,         // مخفي / Hidden
    Showing,        // يظهر / Showing
    Visible,        // مرئي / Visible
    Hiding,         // يختفي / Hiding
    Disabled        // معطل / Disabled
};

} // namespace graphics
} // namespace sad

#endif // SAD_GRAPHICS_STATE_MACHINE_H
