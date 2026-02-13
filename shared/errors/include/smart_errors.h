/**
 * @file smart_errors.h
 * @brief (AR) نظام رسائل الخطأ الذكية - الرأس الرئيسي
 *        (EN) Smart Error Messages System - Main Header
 * 
 * (AR) يجمع جميع مكونات نظام الأخطاء الذكية:
 *      - اقتراحات الإصلاح (SuggestionEngine)
 *      - شرح الأنواع (TypeExplainer)
 *      - جمع الأخطاء المتعددة (MultiErrorCollector)
 *      - اقتراحات الإصلاح السريع (FixSuggestionManager)
 *      - وضع المعلم (TeacherMode)
 *      - استرداد الأخطاء (ErrorRecovery)
 *      - منع التسلسل (CascadePrevention)
 * 
 * @author Sad Language Development Team
 * @date Phase 15 - US13
 * @version 1.0
 */

#pragma once

#include "suggestions.h"
#include "type_explanations.h"
#include "multi_error.h"
#include "fix_suggestions.h"
#include "teacher_mode.h"
#include "error_recovery.h"
#include "cascade_prevention.h"

namespace Sad {
namespace Errors {

/**
 * @class SmartErrorSystem
 * @brief (AR) نظام الأخطاء الذكية المتكامل
 *        (EN) Integrated Smart Error System
 * 
 * (AR) يوفر واجهة موحدة لجميع مكونات الأخطاء الذكية
 *      يتكامل مع ErrorManager الحالي لإثراء رسائل الخطأ
 */
class SmartErrorSystem {
public:
    static SmartErrorSystem& getInstance() {
        static SmartErrorSystem instance;
        return instance;
    }

    /// (AR) الحصول على مولد الاقتراحات
    SuggestionEngine& getSuggestionEngine() { return suggestionEngine_; }
    
    /// (AR) الحصول على شارح الأنواع
    TypeExplainer& getTypeExplainer() { return typeExplainer_; }
    
    /// (AR) الحصول على جامع الأخطاء
    MultiErrorCollector& getMultiErrorCollector() { return multiError_; }
    
    /// (AR) الحصول على مقترح الإصلاحات
    FixSuggestionManager& getFixSuggestionManager() { return fixSuggestions_; }
    
    /// (AR) الحصول على وضع المعلم  
    TeacherMode& getTeacherMode() { return teacherMode_; }
    
    /// (AR) الحصول على نظام منع التسلسل
    CascadePrevention& getCascadePrevention() { return cascadePrevention_; }

    /// (AR) تفعيل/تعطيل النظام الذكي
    void setEnabled(bool enabled) { enabled_ = enabled; }
    bool isEnabled() const { return enabled_; }

    /// (AR) تفعيل وضع المعلم
    void setTeacherModeLevel(ExplanationLevel level) {
        teacherMode_.setLevel(level);
    }

    /// (AR) إعادة تعيين النظام
    void reset() {
        multiError_.clear();
        cascadePrevention_.reset();
    }

    /**
     * @brief (AR) إثراء تشخيص بمعلومات ذكية
     *        (EN) Enrich a diagnostic with smart information
     * 
     * (AR) يضيف اقتراحات إصلاح وشروحات تعليمية للتشخيص
     */
    Diagnostic enrichDiagnostic(const Diagnostic& diag,
                                 const std::string& sourceCode = "",
                                 const std::string& context = "");

    /**
     * @brief (AR) طباعة إحصائيات النظام الذكي
     */
    void printStats() const;

private:
    SmartErrorSystem() : enabled_(true) {}
    SmartErrorSystem(const SmartErrorSystem&) = delete;
    SmartErrorSystem& operator=(const SmartErrorSystem&) = delete;

    SuggestionEngine suggestionEngine_;
    TypeExplainer typeExplainer_;
    MultiErrorCollector multiError_;
    FixSuggestionManager fixSuggestions_;
    TeacherMode teacherMode_;
    CascadePrevention cascadePrevention_;
    bool enabled_;
};

} // namespace Errors
} // namespace Sad
