// Disable Unicode warning for Arabic comments
#ifdef _MSC_VER
#pragma warning(disable : 4819)
#endif

/**
 * @file ownership_manager.cpp
 * @brief (AR) تنفيذ مدير الملكية كمحول رفيع فوق Sad::Semantic::OwnershipTracker
 * @brief (EN) OwnershipManager implementation as thin adapter over Sad::Semantic::OwnershipTracker
 *
 * @details
 * (AR) كل العمل الفعلي مفوَّض إلى المحرك الموحَّد في shared/ownership/.
 *      هذا الملف يحتوي فقط على:
 *      1. تحويل الواجهة المبسَّطة (بدون SourceLocation) إلى الواجهة الكاملة
 *      2. تحويل أخطاء الطبقة المشتركة إلى نسخة المفسّر
 *      3. الإدارة المحلية لإحصائيات وحالة التفعيل
 *
 * (EN) All real work delegated to the unified engine in shared/ownership/.
 *      This file contains only:
 *      1. Adapt simplified API (no SourceLocation) to full API
 *      2. Convert shared-layer errors to interpreter version
 *      3. Local management of stats and enable state
 *
 * @author Sad Language Team
 * @date 2026
 * @version 2.0  (Ownership Unification refactor)
 * @phase Architecture Refactor: Ownership Unification
 */

#include "ownership_manager.h"
#include <iostream>
#include <sstream>

namespace Sad
{
    namespace Data
    {

        // ============================================================================
        // بناء وهدم / Construction & Destruction
        // ============================================================================

        OwnershipManager::OwnershipManager()
            : tracker_(std::make_unique<Sad::Semantic::OwnershipTracker>()), enabled_(false), useArabicMessages_(true), debugMode_(false), totalVariables_(0), totalBorrows_(0), totalMoves_(0)
        {
            initializeCopyTypes();
        }

        OwnershipManager::~OwnershipManager() = default;

        // ============================================================================
        // (AR) دوال مساعدة / (EN) Helper functions
        // ============================================================================

        Sad::Semantic::SourceLocation OwnershipManager::makeRuntimeLocation() const
        {
            // (AR) المفسّر لا يملك معلومات موقع دقيقة في وقت التنفيذ.
            //      نُرجع موقعاً فارغاً — رسائل الخطأ ستعتمد على نوع الخطأ واسم المتغير.
            // (EN) Interpreter has no precise location info at runtime.
            //      Return empty location — error messages rely on kind + variable name.
            return Sad::Semantic::SourceLocation("<runtime>", 0, 0);
        }

        OwnershipError OwnershipManager::convertError(const Sad::Semantic::OwnershipError &sharedErr)
        {
            // (AR) ننسخ الحقول النصية من نسخة الطبقة المشتركة إلى النسخة المبسَّطة
            // (EN) Copy text fields from shared-layer error to simplified version
            OwnershipError out;
            out.kind = sharedErr.kind;
            out.variableName = sharedErr.variableName;
            out.message = sharedErr.message;
            out.arabicMessage = sharedErr.arabicMessage;
            out.suggestion = sharedErr.suggestion;
            return out;
        }

        void OwnershipManager::initializeCopyTypes()
        {
            // (AR) الأنواع العربية والإنجليزية الأساسية القابلة للنسخ
            // (EN) Basic Arabic and English Copy types
            copyTypes_ = {
                // أعداد صحيحة عربية / Arabic integers
                "\xD8\xB1\xD9\x82\xD9\x85",                 // رقم
                "\xD8\xB9\xD8\xB4\xD8\xB1\xD9\x8A",         // عشري
                "\xD9\x85\xD9\x86\xD8\xB7\xD9\x82\xD9\x8A", // منطقي
                // English aliases
                "int", "i32", "i64", "u32", "u64", "f32", "f64",
                "bool", "char", "byte"};
        }

        bool OwnershipManager::isCopyType(const std::string &typeName) const
        {
            return copyTypes_.find(typeName) != copyTypes_.end();
        }

        // ============================================================================
        // إدارة النطاقات / Scope Management
        // ============================================================================

        size_t OwnershipManager::enterScope()
        {
            if (!enabled_)
                return 0;
            return tracker_->enterScope();
        }

        void OwnershipManager::exitScope()
        {
            if (!enabled_)
                return;
            tracker_->exitScope();
        }

        // ============================================================================
        // تصريح واستخدام المتغيرات / Variable Declaration & Use
        // ============================================================================

        bool OwnershipManager::declareVariable(const std::string &name, const std::string &typeName)
        {
            if (!enabled_)
                return true;

            bool isCopy = isCopyType(typeName);
            bool ok = tracker_->declareVariable(name, typeName, makeRuntimeLocation(), isCopy);
            if (ok)
                ++totalVariables_;
            return ok;
        }

        std::optional<OwnershipError> OwnershipManager::useVariable(const std::string &name)
        {
            if (!enabled_)
                return std::nullopt;

            auto sharedErr = tracker_->useVariable(name, makeRuntimeLocation());
            if (sharedErr.has_value())
            {
                OwnershipError err = convertError(sharedErr.value());
                errors_.push_back(err);
                return err;
            }
            return std::nullopt;
        }

        std::optional<OwnershipError> OwnershipManager::moveVariable(const std::string &name)
        {
            if (!enabled_)
                return std::nullopt;

            auto sharedErr = tracker_->moveVariable(name, makeRuntimeLocation());
            if (sharedErr.has_value())
            {
                OwnershipError err = convertError(sharedErr.value());
                errors_.push_back(err);
                return err;
            }
            ++totalMoves_;
            return std::nullopt;
        }

        std::optional<OwnershipError> OwnershipManager::mutateVariable(const std::string &name)
        {
            if (!enabled_)
                return std::nullopt;

            auto sharedErr = tracker_->mutateVariable(name, makeRuntimeLocation());
            if (sharedErr.has_value())
            {
                OwnershipError err = convertError(sharedErr.value());
                errors_.push_back(err);
                return err;
            }
            return std::nullopt;
        }

        // ============================================================================
        // الاستعارة / Borrowing
        // ============================================================================

        std::optional<OwnershipError> OwnershipManager::createBorrow(
            const std::string &ownerName,
            const std::string &borrowerName,
            BorrowKind kind)
        {
            if (!enabled_)
                return std::nullopt;

            auto sharedErr = tracker_->createBorrow(ownerName, borrowerName, kind, makeRuntimeLocation());
            if (sharedErr.has_value())
            {
                OwnershipError err = convertError(sharedErr.value());
                errors_.push_back(err);
                return err;
            }
            ++totalBorrows_;
            return std::nullopt;
        }

        void OwnershipManager::endBorrow(const std::string &ownerName, const std::string &borrowerName)
        {
            if (!enabled_)
                return;
            tracker_->endBorrow(ownerName, borrowerName);
        }

        // ============================================================================
        // استعلامات / Queries
        // ============================================================================

        bool OwnershipManager::variableExists(const std::string &name) const
        {
            if (!enabled_)
                return false;
            return tracker_->variableExists(name);
        }

        bool OwnershipManager::canMove(const std::string &name) const
        {
            if (!enabled_)
                return true;
            return tracker_->canMove(name);
        }

        std::optional<OwnershipState> OwnershipManager::getState(const std::string &name) const
        {
            if (!enabled_)
                return std::nullopt;
            auto info = tracker_->getOwnershipInfo(name);
            if (!info.has_value())
                return std::nullopt;
            return info->state;
        }

        void OwnershipManager::reset()
        {
            tracker_ = std::make_unique<Sad::Semantic::OwnershipTracker>();
            errors_.clear();
            totalVariables_ = 0;
            totalBorrows_ = 0;
            totalMoves_ = 0;
        }

        void OwnershipManager::dump() const
        {
            if (!debugMode_)
                return;
            std::cout << "=== OwnershipManager (wrapper) ===\n";
            std::cout << "  enabled: " << (enabled_ ? "yes" : "no") << "\n";
            std::cout << "  vars:    " << totalVariables_ << "\n";
            std::cout << "  moves:   " << totalMoves_ << "\n";
            std::cout << "  borrows: " << totalBorrows_ << "\n";
            std::cout << "  errors:  " << errors_.size() << "\n";
            if (tracker_)
                tracker_->dump();
        }

        std::string OwnershipManager::getSummary() const
        {
            std::ostringstream oss;
            oss << "OwnershipManager: vars=" << totalVariables_
                << " moves=" << totalMoves_
                << " borrows=" << totalBorrows_
                << " errors=" << errors_.size();
            return oss.str();
        }

    } // namespace Data
} // namespace Sad
