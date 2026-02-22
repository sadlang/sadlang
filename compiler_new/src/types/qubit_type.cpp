/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ╔═══════════════════════════════════════════════════════════════════════════╗
 * ║                        نوع الكيوبت - لغة ص                                 ║
 * ║                     Qubit Type - Sad Language                             ║
 * ╚═══════════════════════════════════════════════════════════════════════════╝
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * الملف: qubit_type.cpp
 * File: qubit_type.cpp
 * 
 * الوصف: تنفيذ نظام الحوسبة الكمومية (Quantum Computing) في لغة ص
 * Description: Implementation of Quantum Computing system with Qubits
 * 
 * المهمة: T452 - دعم الحوسبة الكمومية والكيوبتات
 * Task: T452 - Quantum Computing and Qubit Support
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 *                              المفاهيم الأساسية
 *                              Core Concepts
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * 🌟 ما هو الكيوبت (Qubit)؟
 * 
 *    الكيوبت هو البت الكمومي - وحدة المعلومات الأساسية في الحوسبة الكمومية.
 *    على عكس البت الكلاسيكي (0 أو 1)، يمكن للكيوبت أن يكون في حالة تراكب
 *    (superposition) من 0 و 1 في نفس الوقت.
 * 
 *    A Qubit is a quantum bit - the basic unit of quantum information.
 *    Unlike classical bits (0 or 1), a qubit can be in a superposition
 *    of both 0 and 1 simultaneously.
 * 
 * 📋 الحالة الكمومية:
 * 
 *    |ψ⟩ = α|0⟩ + β|1⟩
 * 
 *    حيث:
 *    - α و β أعداد مركبة (complex numbers)
 *    - |α|² + |β|² = 1 (شرط التطبيع)
 *    - |α|² = احتمال قياس 0
 *    - |β|² = احتمال قياس 1
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 *                              أمثلة الاستخدام
 *                              Usage Examples
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * // مثال 1: إنشاء كيوبت في حالة |0⟩
 * // Example 1: Create qubit in |0⟩ state
 * دع كيوبت = كيوبت::صفر()؛
 * 
 * // مثال 2: تطبيق بوابة هادامارد لإنشاء التراكب
 * // Example 2: Apply Hadamard gate to create superposition
 * كيوبت.هادامارد()؛  // الآن في الحالة (|0⟩ + |1⟩)/√2
 * 
 * // مثال 3: القياس (ينهار التراكب)
 * // Example 3: Measurement (collapses superposition)
 * دع نتيجة = كيوبت.قس()؛  // 0 أو 1 بنسبة 50%/50%
 * 
 * // مثال 4: التشابك الكمومي (Entanglement)
 * // Example 4: Quantum entanglement
 * دع كيوبت1 = كيوبت::صفر()؛
 * دع كيوبت2 = كيوبت::صفر()؛
 * كيوبت1.هادامارد()؛
 * كيوبت::سي_نوت(كيوبت1، كيوبت2)؛  // حالة بل: (|00⟩ + |11⟩)/√2
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "../../include/types/type.h"
#include <string>
#include <vector>
#include <complex>
#include <random>
#include <memory>
#include <cmath>
#include <stdexcept>

namespace Sad {
namespace TypeSystem {

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *                           ثوابت الحوسبة الكمومية
 *                           Quantum Computing Constants
 * ═══════════════════════════════════════════════════════════════════════════════
 */

constexpr double PI = 3.14159265358979323846;
constexpr double SQRT2_INV = 0.70710678118654752440;  // 1/√2

/**
 * نوع العدد المركب
 * Complex number type
 */
using ComplexNumber = std::complex<double>;

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *                              حالة الكيوبت
 *                              Qubit State
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * تمثل الحالة الكمومية: |ψ⟩ = α|0⟩ + β|1⟩
 * Represents quantum state: |ψ⟩ = α|0⟩ + β|1⟩
 */
struct QubitState {
    ComplexNumber alpha;  // معامل |0⟩ / Coefficient of |0⟩
    ComplexNumber beta;   // معامل |1⟩ / Coefficient of |1⟩
    
    /**
     * المُنشئ الافتراضي: حالة |0⟩
     * Default constructor: |0⟩ state
     */
    QubitState() : alpha(1.0, 0.0), beta(0.0, 0.0) {}
    
    /**
     * المُنشئ مع معاملات محددة
     * Constructor with specific coefficients
     */
    QubitState(ComplexNumber a, ComplexNumber b) : alpha(a), beta(b) {
        normalize();
    }
    
    /**
     * تطبيع الحالة (التأكد من أن |α|² + |β|² = 1)
     * Normalize state (ensure |α|² + |β|² = 1)
     */
    void normalize() {
        double norm = std::sqrt(std::norm(alpha) + std::norm(beta));
        if (norm > 1e-10) {
            alpha /= norm;
            beta /= norm;
        }
    }
    
    /**
     * احتمال قياس |0⟩
     * Probability of measuring |0⟩
     */
    double probability0() const {
        return std::norm(alpha);
    }
    
    /**
     * احتمال قياس |1⟩
     * Probability of measuring |1⟩
     */
    double probability1() const {
        return std::norm(beta);
    }
};

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *                            نوع الكيوبت
 *                            Qubit Type
 * ═══════════════════════════════════════════════════════════════════════════════
 */
class QubitType : public Type {
public:
    /**
     * المُنشئ
     * Constructor
     */
    QubitType() : Type(TypeKind::Any) {}  // سنضيف TypeKind::Qubit لاحقاً
    
    /**
     * المُدمّر الافتراضي
     * Default destructor
     */
    virtual ~QubitType() = default;
    
    /**
     * الحصول على الاسم العربي
     * Get Arabic name
     */
    std::string getArabicName() const override {
        return "كيوبت";
    }
    
    /**
     * الحصول على الاسم الإنجليزي
     * Get English name
     */
    std::string getEnglishName() const override {
        return "Qubit";
    }
    
    /**
     * تحويل إلى نص
     * Convert to string
     */
    std::string toString() const override {
        return "كيوبت";
    }
    
    /**
     * التحقق من التساوي
     * Check equality
     */
    bool equals(const Type* other) const override {
        return other && other->getKind() == getKind();
    }
    
    /**
     * استنساخ النوع
     * Clone type
     */
    std::shared_ptr<Type> clone() const override {
        return std::make_shared<QubitType>();
    }
};

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *                          حالات الكيوبت الأساسية
 *                          Basic Qubit States
 * ═══════════════════════════════════════════════════════════════════════════════
 */
namespace QubitStates {
    /**
     * حالة |0⟩ (الحالة الأرضية)
     * |0⟩ state (ground state)
     */
    const QubitState Zero(ComplexNumber(1.0, 0.0), ComplexNumber(0.0, 0.0));
    
    /**
     * حالة |1⟩ (الحالة المثارة)
     * |1⟩ state (excited state)
     */
    const QubitState One(ComplexNumber(0.0, 0.0), ComplexNumber(1.0, 0.0));
    
    /**
     * حالة |+⟩ = (|0⟩ + |1⟩)/√2
     * |+⟩ state (equal superposition)
     */
    const QubitState Plus(
        ComplexNumber(SQRT2_INV, 0.0),
        ComplexNumber(SQRT2_INV, 0.0)
    );
    
    /**
     * حالة |-⟩ = (|0⟩ - |1⟩)/√2
     * |-⟩ state (negative superposition)
     */
    const QubitState Minus(
        ComplexNumber(SQRT2_INV, 0.0),
        ComplexNumber(-SQRT2_INV, 0.0)
    );
}

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *                          البوابات الكمومية
 *                          Quantum Gates
 * ═══════════════════════════════════════════════════════════════════════════════
 */
namespace QuantumGates {
    /**
     * بوابة باولي X (NOT الكمومية)
     * Pauli X gate (Quantum NOT)
     * |0⟩ → |1⟩, |1⟩ → |0⟩
     */
    void PauliX(QubitState& state) {
        std::swap(state.alpha, state.beta);
    }
    
    /**
     * بوابة باولي Y
     * Pauli Y gate
     * |0⟩ → i|1⟩, |1⟩ → -i|0⟩
     */
    void PauliY(QubitState& state) {
        ComplexNumber temp = state.alpha;
        state.alpha = ComplexNumber(0, -1) * state.beta;
        state.beta = ComplexNumber(0, 1) * temp;
    }
    
    /**
     * بوابة باولي Z
     * Pauli Z gate
     * |0⟩ → |0⟩, |1⟩ → -|1⟩
     */
    void PauliZ(QubitState& state) {
        state.beta = -state.beta;
    }
    
    /**
     * بوابة هادامارد (Hadamard)
     * تنشئ التراكب المتساوي
     * Creates equal superposition
     * |0⟩ → (|0⟩ + |1⟩)/√2
     * |1⟩ → (|0⟩ - |1⟩)/√2
     */
    void Hadamard(QubitState& state) {
        ComplexNumber new_alpha = SQRT2_INV * (state.alpha + state.beta);
        ComplexNumber new_beta = SQRT2_INV * (state.alpha - state.beta);
        state.alpha = new_alpha;
        state.beta = new_beta;
    }
    
    /**
     * بوابة الدوران حول المحور Z
     * Rotation around Z axis
     * R_z(θ) |0⟩ = e^(-iθ/2) |0⟩
     * R_z(θ) |1⟩ = e^(iθ/2) |1⟩
     */
    void RotateZ(QubitState& state, double theta) {
        double half_theta = theta / 2.0;
        state.alpha *= std::exp(ComplexNumber(0, -half_theta));
        state.beta *= std::exp(ComplexNumber(0, half_theta));
    }
    
    /**
     * بوابة الطور (Phase gate)
     * S = R_z(π/2)
     */
    void Phase(QubitState& state) {
        RotateZ(state, PI / 2.0);
    }
    
    /**
     * بوابة T (π/8 gate)
     * T = R_z(π/4)
     */
    void TGate(QubitState& state) {
        RotateZ(state, PI / 4.0);
    }
}

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *                          عمليات القياس
 *                          Measurement Operations
 * ═══════════════════════════════════════════════════════════════════════════════
 */
class QubitMeasurement {
private:
    static std::mt19937& getRandomGenerator() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        return gen;
    }
    
public:
    /**
     * قياس الكيوبت في الأساس الحسابي
     * Measure qubit in computational basis
     * 
     * @param state الحالة الكمومية
     * @return 0 أو 1
     * 
     * ملاحظة: هذه العملية تُنهي التراكب وتُغير الحالة!
     * Note: This operation collapses superposition and changes state!
     */
    static int measure(QubitState& state) {
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        double random = dist(getRandomGenerator());
        double prob0 = state.probability0();
        
        if (random < prob0) {
            // انهيار إلى |0⟩
            // Collapse to |0⟩
            state.alpha = ComplexNumber(1.0, 0.0);
            state.beta = ComplexNumber(0.0, 0.0);
            return 0;
        } else {
            // انهيار إلى |1⟩
            // Collapse to |1⟩
            state.alpha = ComplexNumber(0.0, 0.0);
            state.beta = ComplexNumber(1.0, 0.0);
            return 1;
        }
    }
    
    /**
     * قياس بدون تعديل الحالة (للمحاكاة فقط)
     * Measure without modifying state (simulation only)
     */
    static int measureNonDestructive(const QubitState& state) {
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        double random = dist(getRandomGenerator());
        return (random < state.probability0()) ? 0 : 1;
    }
};

} // namespace TypeSystem
} // namespace Sad
