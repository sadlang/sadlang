// Disable Unicode warning for Arabic comments
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4819)
#endif

/**
 * @file ownership_manager.h
 * @brief (AR) مدير الملكية للمفسر — wrapper رفيع فوق Sad::Semantic::OwnershipTracker
 * @brief (EN) Interpreter ownership manager — thin wrapper over Sad::Semantic::OwnershipTracker
 *
 * @details
 * (AR) بعد إعادة الهيكلة المعمارية (Ownership Unification)، أصبح مدير
 *      الملكية في المفسّر مجرد محول رفيع (adapter) فوق نظام الملكية
 *      الموحَّد المنقول إلى shared/ownership/.
 *
 *      الفائدة:
 *      - نفس قواعد الملكية تُطبَّق في جميع المسارات (sad, sad --vm, sadc, wasm)
 *      - رسائل الخطأ متطابقة عبر المسارات
 *      - الصيانة في مكان واحد فقط (shared/ownership/)
 *      - يمكن بناء أداة فحص ملكية مستقلة بدون تشغيل الكود
 *
 *      الواجهة العامة (`Sad::Data::OwnershipManager`) **لم تتغير** للحفاظ
 *      على التوافق الخلفي مع 18 مستهلكاً في المفسّر.
 *
 * (EN) After the Ownership Unification refactor, the interpreter's
 *      OwnershipManager is now a thin adapter over the unified ownership
 *      system in shared/ownership/.
 *
 *      Benefits:
 *      - Same ownership rules applied across all execution paths
 *      - Identical error messages across paths
 *      - Single point of maintenance (shared/ownership/)
 *      - Enables a standalone ownership-check tool
 *
 *      The public API (`Sad::Data::OwnershipManager`) is **unchanged** for
 *      backward compatibility with 18 consumers in the interpreter.
 *
 * @author Sad Language Team
 * @date 2026
 * @version 2.0  (Ownership Unification refactor)
 * @phase Architecture Refactor: Ownership Unification
 */

#pragma once

#include "ownership/ownership_tracker.h" // (AR) المحرك الموحَّد / (EN) Unified engine
#include "memory/policy/gc_mode.h"        // (AR) سياسة الذاكرة الموحَّدة / (EN) Unified memory policy

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <optional>

namespace Sad
{
    namespace Data
    {

        // ============================================================================
        // (AR) إعادة تصدير الأنواع من Sad::Semantic لتجنب تعريفها مرتين.
        // (EN) Re-export types from Sad::Semantic to avoid duplicate definitions.
        //
        // (AR) هذا يضمن أن `Sad::Data::OwnershipState` و `Sad::Semantic::OwnershipState`
        //      هما نفس النوع تماماً (وليس نوعين مختلفين بقيم متطابقة).
        // (EN) This ensures `Sad::Data::OwnershipState` and `Sad::Semantic::OwnershipState`
        //      are the SAME type (not two distinct types with matching values).
        // ============================================================================

        using OwnershipState = Sad::Semantic::OwnershipState;
        using BorrowKind = Sad::Semantic::BorrowKind;
        using OwnershipErrorKind = Sad::Semantic::OwnershipErrorKind;
        using SourceLocation = Sad::Semantic::SourceLocation;

        // ============================================================================
        // (AR) هياكل البيانات المُعاد تصديرها / (EN) Re-exported data structures
        // ============================================================================

        /**
         * @struct BorrowInfo
         * @brief (AR) معلومات الاستعارة (نسخة مبسَّطة للمفسّر — بدون SourceLocation)
         * @brief (EN) Borrow information (interpreter-simplified — no SourceLocation)
         */
        struct BorrowInfo
        {
            std::string borrowerName;
            BorrowKind kind;
            size_t scopeId;

            BorrowInfo() : kind(BorrowKind::Shared), scopeId(0) {}
            BorrowInfo(const std::string &name, BorrowKind k, size_t scope)
                : borrowerName(name), kind(k), scopeId(scope) {}
        };

        /**
         * @struct VariableOwnership
         * @brief (AR) معلومات الملكية الكاملة للمتغير (نسخة المفسّر)
         * @brief (EN) Complete ownership info (interpreter version)
         */
        struct VariableOwnership
        {
            std::string variableName;
            OwnershipState state;
            std::vector<BorrowInfo> borrows;
            size_t scopeId;
            bool isCopyType;
            std::string typeName;

            VariableOwnership()
                : state(OwnershipState::Uninitialized), scopeId(0), isCopyType(false) {}

            VariableOwnership(const std::string &name, size_t scope, bool copyType = false)
                : variableName(name), state(OwnershipState::Owned), scopeId(scope), isCopyType(copyType) {}

            bool hasActiveBorrows() const { return !borrows.empty(); }
            bool hasActiveMutableBorrow() const
            {
                for (const auto &b : borrows)
                    if (b.kind == BorrowKind::Mutable)
                        return true;
                return false;
            }
        };

        /**
         * @struct OwnershipError
         * @brief (AR) خطأ ملكية (نسخة المفسّر — لا تشمل المواقع)
         * @brief (EN) Ownership error (interpreter version — no locations)
         */
        struct OwnershipError
        {
            OwnershipErrorKind kind;
            std::string variableName;
            std::string message;
            std::string arabicMessage;
            std::string suggestion;

            std::string toArabicString() const
            {
                std::string code = std::to_string(static_cast<int>(kind));
                while (code.size() < 4)
                    code = "0" + code;
                std::string result = "\xD8\xAE\xD8\xB7\xD8\xA3[\xD8\xB5" + code + "]: " + arabicMessage;
                if (!suggestion.empty())
                    result += "\n   = \xD8\xA7\xD9\x82\xD8\xAA\xD8\xB1\xD8\xA7\xD8\xAD: " + suggestion;
                return result;
            }

            std::string toEnglishString() const
            {
                std::string code = std::to_string(static_cast<int>(kind));
                while (code.size() < 4)
                    code = "0" + code;
                std::string result = "error[S" + code + "]: " + message;
                if (!suggestion.empty())
                    result += "\n   = suggestion: " + suggestion;
                return result;
            }
        };

        // ============================================================================
        // (AR) مدير الملكية — wrapper فوق Sad::Semantic::OwnershipTracker
        // (EN) OwnershipManager — wrapper over Sad::Semantic::OwnershipTracker
        // ============================================================================

        /**
         * @class OwnershipManager
         * @brief (AR) مدير الملكية للمفسر / (EN) Ownership manager for interpreter
         *
         * (AR) محول رفيع فوق المحرك الموحَّد. يحتفظ بنفس الواجهة العامة السابقة
         *      ولكنه يفوّض كل العمل الفعلي إلى Sad::Semantic::OwnershipTracker.
         * (EN) Thin adapter over the unified engine. Keeps the same prior public
         *      API but delegates all real work to Sad::Semantic::OwnershipTracker.
         */
        class OwnershipManager
        {
        public:
            OwnershipManager();
            ~OwnershipManager();

            // ==================================================================
            // تفعيل/تعطيل / Enable/Disable
            // ==================================================================
            void enable() { enabled_ = true; }
            void disable() { enabled_ = false; }
            bool isEnabled() const { return enabled_; }
            void setArabicMessages(bool arabic) { useArabicMessages_ = arabic; }
            void setDebugMode(bool debug) { debugMode_ = debug; }

            // ==================================================================
            // (AR) ضبط الصرامة من سياسة الذاكرة الموحَّدة (Phase A2)
            // (EN) Apply strictness from the unified memory policy (Phase A2)
            //
            // (AR) يحوّل OwnershipMode (Disabled/Warnings/Strict/UltraStrict) إلى
            //      حالة المدير: Disabled → disable()؛ غير ذلك → enable() + ضبط
            //      علم warningsOnly_ ليُترجم الأخطاء إلى تحذيرات في وضع التعلم.
            // (EN) Converts OwnershipMode → manager state: Disabled → disable();
            //      otherwise enable() + warningsOnly_ flag so errors become
            //      warnings in learn mode without blocking execution.
            // ==================================================================
            void setStrictness(::Sad::Memory::OwnershipMode mode);
            ::Sad::Memory::OwnershipMode getStrictness() const { return strictness_; }
            bool isWarningsOnly() const { return warningsOnly_; }

            // ==================================================================
            // إدارة النطاقات / Scope Management
            // ==================================================================
            size_t enterScope();
            void exitScope();

            // ==================================================================
            // تصريح واستخدام المتغيرات / Variable Declaration & Use
            // ==================================================================
            bool declareVariable(const std::string &name, const std::string &typeName = "");
            std::optional<OwnershipError> useVariable(const std::string &name);
            std::optional<OwnershipError> moveVariable(const std::string &name);
            std::optional<OwnershipError> mutateVariable(const std::string &name);

            // ==================================================================
            // الاستعارة / Borrowing
            // ==================================================================
            std::optional<OwnershipError> createBorrow(
                const std::string &ownerName,
                const std::string &borrowerName,
                BorrowKind kind);

            void endBorrow(const std::string &ownerName, const std::string &borrowerName);

            // ==================================================================
            // استعلامات / Queries
            // ==================================================================
            bool variableExists(const std::string &name) const;
            bool canMove(const std::string &name) const;
            std::optional<OwnershipState> getState(const std::string &name) const;
            const std::vector<OwnershipError> &getErrors() const { return errors_; }
            void clearErrors() { errors_.clear(); }
            bool hasErrors() const { return !errors_.empty(); }
            void reset();
            void dump() const;
            std::string getSummary() const;

        private:
            // ==================================================================
            // (AR) المحرك الموحَّد المُغلَّف / (EN) Wrapped unified engine
            // ==================================================================
            std::unique_ptr<Sad::Semantic::OwnershipTracker> tracker_;

            // ==================================================================
            // (AR) إعدادات وحالة محلية / (EN) Local settings & state
            // ==================================================================
            bool enabled_;
            bool useArabicMessages_;
            bool debugMode_;

            // (AR) صرامة الملكية المُستمدَّة من --dev/--prod/--learn (Phase A2)
            // (EN) Ownership strictness derived from --dev/--prod/--learn
            ::Sad::Memory::OwnershipMode strictness_{::Sad::Memory::OwnershipMode::Strict};
            bool warningsOnly_{false};

            // (AR) كاش لأخطاء الواجهة المبسَّطة / (EN) Cache of simplified errors
            std::vector<OwnershipError> errors_;

            // (AR) إحصائيات / (EN) Statistics
            size_t totalVariables_;
            size_t totalBorrows_;
            size_t totalMoves_;

            // (AR) أنواع قابلة للنسخ / (EN) Copy types
            std::unordered_set<std::string> copyTypes_;

            // ==================================================================
            // دوال مساعدة / Helper functions
            // ==================================================================
            OwnershipError convertError(const Sad::Semantic::OwnershipError &sharedErr);
            bool isCopyType(const std::string &typeName) const;
            void initializeCopyTypes();
            Sad::Semantic::SourceLocation makeRuntimeLocation() const;
        };

    } // namespace Data
} // namespace Sad

#ifdef _MSC_VER
#pragma warning(pop)
#endif
