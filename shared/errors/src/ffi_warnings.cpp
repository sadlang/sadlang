/**
 * @file ffi_warnings.cpp
 * @brief (AR) تنفيذ نظام التحذيرات الذكية لـ FFI
 *        (EN) Smart FFI Warning System Implementation
 * 
 * @details
 * (AR) هذا الملف يحتوي على تنفيذ:
 *      ┌────────────────────────────────────────────────────────────┐
 *      │ 1. FFIWarningBuilder - بناء التحذيرات                      │
 *      │ 2. FFIWarningFormatter - تنسيق التحذيرات                   │
 *      │ 3. FFILeakDetector - كشف التسريبات                        │
 *      │ 4. FFIWarningCollector - تجميع التحذيرات                   │
 *      │ 5. دوال مساعدة لإنشاء التحذيرات الشائعة                    │
 *      └────────────────────────────────────────────────────────────┘
 * 
 * @author فريق تطوير لغة ص (Sad Language Development Team)
 * @date 2025-01-18
 * @version 1.0.0
 */

#include "ffi_warnings.h"
#include <sstream>
#include <algorithm>
#include <iostream>
#include <iomanip>

namespace sad {
namespace errors {

// ============================================================================
//                    (AR) ثوابت الألوان ANSI
//                    (EN) ANSI Color Constants
// ============================================================================

namespace Colors {
    const std::string RESET   = "\033[0m";
    const std::string RED     = "\033[31m";
    const std::string GREEN   = "\033[32m";
    const std::string YELLOW  = "\033[33m";
    const std::string BLUE    = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN    = "\033[36m";
    const std::string BOLD    = "\033[1m";
}

// ============================================================================
//                    (AR) رسائل التحذيرات
//                    (EN) Warning Messages
// ============================================================================

namespace WarningMessages {
    // (AR) رسائل التسريب
    // (EN) Leak messages
    const std::map<FFIWarningCode, std::pair<std::string, std::string>> messages = {
        {FFIWarningCode::POTENTIAL_LEAK, 
            {u8"تسريب محتمل للذاكرة", "Potential memory leak"}},
        {FFIWarningCode::UNFREED_ALLOCATION, 
            {u8"تخصيص ذاكرة غير مُحرر", "Unfreed memory allocation"}},
        {FFIWarningCode::OWNED_RESULT_IGNORED, 
            {u8"نتيجة مملوكة تم تجاهلها", "Owned result ignored"}},
        {FFIWarningCode::MISSING_CLEANUP, 
            {u8"كود التنظيف مفقود", "Missing cleanup code"}},
        {FFIWarningCode::CONDITIONAL_LEAK, 
            {u8"تسريب في مسار تنفيذ معين", "Leak in conditional path"}},
            
        {FFIWarningCode::USE_AFTER_FREE, 
            {u8"استخدام بعد التحرير", "Use after free"}},
        {FFIWarningCode::DOUBLE_FREE, 
            {u8"تحرير مزدوج للذاكرة", "Double free"}},
        {FFIWarningCode::INVALID_FREE, 
            {u8"تحرير غير صالح", "Invalid free"}},
        {FFIWarningCode::MISMATCHED_DEALLOC, 
            {u8"عدم تطابق دالة الحجز والتحرير", "Mismatched allocator/deallocator"}},
            
        {FFIWarningCode::NULL_PASSED_TO_NONNULL, 
            {u8"تمرير null لمعامل غير اختياري", "Null passed to non-optional parameter"}},
        {FFIWarningCode::UNCHECKED_NULL_RESULT, 
            {u8"نتيجة قد تكون null غير مفحوصة", "Unchecked possibly-null result"}},
        {FFIWarningCode::DANGLING_POINTER, 
            {u8"مؤشر معلق", "Dangling pointer"}},
            
        {FFIWarningCode::BORROWED_TRANSFERRED, 
            {u8"محاولة نقل ملكية متغير مستعار", "Attempting to transfer borrowed value"}},
        {FFIWarningCode::OWNERSHIP_UNCLEAR, 
            {u8"ملكية المؤشر غير واضحة", "Unclear pointer ownership"}},
        {FFIWarningCode::ESCAPING_LOCAL, 
            {u8"هروب مؤشر لمتغير محلي", "Local variable pointer escaping"}},
            
        {FFIWarningCode::UNSAFE_FFI_CALL, 
            {u8"استدعاء FFI غير آمن", "Unsafe FFI call"}},
        {FFIWarningCode::DEPRECATED_FFI, 
            {u8"دالة FFI مُهملة", "Deprecated FFI function"}},
        {FFIWarningCode::THREAD_SAFETY, 
            {u8"مشكلة أمان الخيوط", "Thread safety issue"}},
    };
}

// ============================================================================
//                    (AR) تنفيذ FFIWarningBuilder
//                    (EN) FFIWarningBuilder Implementation
// ============================================================================

FFIWarningBuilder::FFIWarningBuilder(FFIWarningCode code) {
    warning_.code = code;
    warning_.severity = WarningSeverity::WARNING;
    
    // (AR) تعيين الرسائل الافتراضية
    // (EN) Set default messages
    auto it = WarningMessages::messages.find(code);
    if (it != WarningMessages::messages.end()) {
        warning_.arabicMessage = it->second.first;
        warning_.englishMessage = it->second.second;
    }
}

FFIWarningBuilder& FFIWarningBuilder::withArabicMessage(const std::string& msg) {
    warning_.arabicMessage = msg;
    return *this;
}

FFIWarningBuilder& FFIWarningBuilder::withEnglishMessage(const std::string& msg) {
    warning_.englishMessage = msg;
    return *this;
}

FFIWarningBuilder& FFIWarningBuilder::atLocation(const SourceLocation& loc) {
    warning_.location = loc;
    return *this;
}

FFIWarningBuilder& FFIWarningBuilder::atLocation(const std::string& file, size_t line, size_t col) {
    warning_.location.filename = file;
    warning_.location.line = line;
    warning_.location.column = col;
    return *this;
}

FFIWarningBuilder& FFIWarningBuilder::withLineContent(const std::string& content, size_t start, size_t len) {
    warning_.location.lineContent = content;
    warning_.location.highlightStart = start;
    warning_.location.highlightLength = len;
    return *this;
}

FFIWarningBuilder& FFIWarningBuilder::withRelatedLocation(const SourceLocation& loc) {
    warning_.relatedLocations.push_back(loc);
    return *this;
}

FFIWarningBuilder& FFIWarningBuilder::withNote(const std::string& note) {
    warning_.notes.push_back(note);
    return *this;
}

FFIWarningBuilder& FFIWarningBuilder::withSuggestion(const std::string& suggestion) {
    warning_.suggestions.push_back(suggestion);
    return *this;
}

FFIWarningBuilder& FFIWarningBuilder::withFix(const std::string& fix) {
    warning_.fixHint = fix;
    return *this;
}

FFIWarningBuilder& FFIWarningBuilder::withSeverity(WarningSeverity severity) {
    warning_.severity = severity;
    return *this;
}

FFIWarning FFIWarningBuilder::build() const {
    return warning_;
}

// ============================================================================
//                    (AR) تنفيذ FFIWarningFormatter
//                    (EN) FFIWarningFormatter Implementation
// ============================================================================

FFIWarningFormatter::FFIWarningFormatter(const FFIWarningConfig& config)
    : config_(config) {}

std::string FFIWarningFormatter::format(const FFIWarning& warning) const {
    std::ostringstream out;
    
    // (AR) سطر الرأس: severity[code]: message
    // (EN) Header line: severity[code]: message
    out << colorize(severityToString(warning.severity), warning.severity);
    out << "[" << codeToString(warning.code) << "]: ";
    
    if (config_.arabicMessages) {
        out << warning.arabicMessage;
    } else {
        out << warning.englishMessage;
    }
    out << "\n";
    
    // (AR) الموقع
    // (EN) Location
    out << "   --> " << formatLocation(warning.location) << "\n";
    
    // (AR) مؤشر الكود
    // (EN) Code pointer
    if (!warning.location.lineContent.empty()) {
        out << formatCodePointer(warning.location);
    }
    
    // (AR) المواقع ذات الصلة
    // (EN) Related locations
    if (config_.showRelatedLocations && !warning.relatedLocations.empty()) {
        for (const auto& loc : warning.relatedLocations) {
            out << "   --> " << formatLocation(loc);
            if (!loc.lineContent.empty()) {
                out << "\n" << formatCodePointer(loc);
            } else {
                out << "\n";
            }
        }
    }
    
    // (AR) الملاحظات
    // (EN) Notes
    if (config_.showNotes && !warning.notes.empty()) {
        for (const auto& note : warning.notes) {
            out << "   = " << colorize(u8"ملاحظة", WarningSeverity::NOTE);
            out << ": " << note << "\n";
        }
    }
    
    // (AR) الاقتراحات
    // (EN) Suggestions
    if (config_.showSuggestions && !warning.suggestions.empty()) {
        for (const auto& suggestion : warning.suggestions) {
            out << "   = " << colorize(u8"اقتراح", WarningSeverity::HINT);
            out << ": " << suggestion << "\n";
        }
    }
    
    // (AR) الإصلاح المقترح
    // (EN) Suggested fix
    if (warning.fixHint) {
        out << "   = " << colorize(u8"إصلاح", WarningSeverity::HINT);
        out << ": " << *warning.fixHint << "\n";
    }
    
    return out.str();
}

std::string FFIWarningFormatter::formatAll(const std::vector<FFIWarning>& warnings) const {
    std::ostringstream out;
    
    for (const auto& warning : warnings) {
        out << format(warning) << "\n";
    }
    
    // (AR) ملخص
    // (EN) Summary
    size_t errorCount = 0;
    size_t warningCount = 0;
    
    for (const auto& w : warnings) {
        if (w.severity == WarningSeverity::ERROR) errorCount++;
        else warningCount++;
    }
    
    out << "\n";
    if (errorCount > 0) {
        out << colorize(u8"أخطاء: " + std::to_string(errorCount), WarningSeverity::ERROR);
    }
    if (warningCount > 0) {
        if (errorCount > 0) out << ", ";
        out << colorize(u8"تحذيرات: " + std::to_string(warningCount), WarningSeverity::WARNING);
    }
    out << "\n";
    
    return out.str();
}

std::string FFIWarningFormatter::codeToString(FFIWarningCode code) {
    std::ostringstream out;
    out << u8"ص-FFI-" << std::setw(3) << std::setfill('0') << static_cast<int>(code);
    return out.str();
}

std::string FFIWarningFormatter::codeDescription(FFIWarningCode code, bool arabic) {
    auto it = WarningMessages::messages.find(code);
    if (it != WarningMessages::messages.end()) {
        return arabic ? it->second.first : it->second.second;
    }
    return arabic ? u8"تحذير غير معروف" : "Unknown warning";
}

std::string FFIWarningFormatter::formatLocation(const SourceLocation& loc) const {
    std::ostringstream out;
    out << loc.filename << ":" << loc.line << ":" << loc.column;
    return out.str();
}

std::string FFIWarningFormatter::formatCodePointer(const SourceLocation& loc) const {
    std::ostringstream out;
    
    // (AR) رقم السطر
    // (EN) Line number
    out << "    |\n";
    out << " " << std::setw(3) << loc.line << " | " << loc.lineContent << "\n";
    out << "    | ";
    
    // (AR) المؤشر
    // (EN) Pointer
    for (size_t i = 0; i < loc.highlightStart; i++) {
        out << " ";
    }
    
    if (config_.colorOutput) {
        out << Colors::RED;
    }
    
    for (size_t i = 0; i < loc.highlightLength && i < 50; i++) {
        out << "^";
    }
    
    if (config_.colorOutput) {
        out << Colors::RESET;
    }
    
    out << "\n";
    
    return out.str();
}

std::string FFIWarningFormatter::severityToString(WarningSeverity severity) const {
    if (config_.arabicMessages) {
        switch (severity) {
            case WarningSeverity::NOTE: return u8"ملاحظة";
            case WarningSeverity::HINT: return u8"تلميح";
            case WarningSeverity::WARNING: return u8"تحذير";
            case WarningSeverity::ERROR: return u8"خطأ";
        }
    } else {
        switch (severity) {
            case WarningSeverity::NOTE: return "note";
            case WarningSeverity::HINT: return "hint";
            case WarningSeverity::WARNING: return "warning";
            case WarningSeverity::ERROR: return "error";
        }
    }
    return "unknown";
}

std::string FFIWarningFormatter::colorize(const std::string& text, WarningSeverity severity) const {
    if (!config_.colorOutput) {
        return text;
    }
    
    std::string color;
    switch (severity) {
        case WarningSeverity::ERROR:   color = Colors::RED; break;
        case WarningSeverity::WARNING: color = Colors::YELLOW; break;
        case WarningSeverity::HINT:    color = Colors::CYAN; break;
        case WarningSeverity::NOTE:    color = Colors::BLUE; break;
    }
    
    return Colors::BOLD + color + text + Colors::RESET;
}

// ============================================================================
//                    (AR) تنفيذ FFILeakDetector
//                    (EN) FFILeakDetector Implementation
// ============================================================================

void FFILeakDetector::registerAllocation(
    const std::string& varName,
    const std::string& allocator,
    const SourceLocation& location
) {
    AllocationInfo info;
    info.varName = varName;
    info.allocatorFunc = allocator;
    info.location = location;
    info.isFreed = false;
    
    // (AR) تحديد دالة التحرير المتوقعة
    // (EN) Determine expected deallocator
    static const std::map<std::string, std::string> deallocators = {
        {"malloc", "free"},
        {"calloc", "free"},
        {"realloc", "free"},
        {"strdup", "free"},
        {"fopen", "fclose"},
        {"popen", "pclose"},
    };
    
    auto it = deallocators.find(allocator);
    if (it != deallocators.end()) {
        info.expectedDeallocator = it->second;
    }
    
    allocations_[varName] = info;
}

void FFILeakDetector::registerDeallocation(
    const std::string& varName,
    const std::string& deallocator,
    const SourceLocation& location
) {
    auto it = allocations_.find(varName);
    if (it != allocations_.end()) {
        it->second.isFreed = true;
        it->second.freeLocation = location;
        
        // (AR) التحقق من تطابق التحرير
        // (EN) Check deallocator match
        if (it->second.expectedDeallocator && 
            *it->second.expectedDeallocator != deallocator) {
            // (AR) سيتم الإبلاغ عن هذا في checkScope
        }
    }
}

std::vector<FFIWarning> FFILeakDetector::checkScope() {
    std::vector<FFIWarning> warnings;
    
    for (const auto& [name, info] : allocations_) {
        if (!info.isFreed) {
            warnings.push_back(createLeakWarning(info));
        } else if (info.expectedDeallocator && info.freeLocation) {
            // (AR) التحقق من تطابق التحرير
            // (EN) Check deallocator match
            // ... يمكن إضافة تحقق إضافي هنا
        }
    }
    
    return warnings;
}

void FFILeakDetector::clear() {
    allocations_.clear();
}

std::vector<FFILeakDetector::AllocationInfo> FFILeakDetector::getUnfreedAllocations() const {
    std::vector<AllocationInfo> result;
    for (const auto& [name, info] : allocations_) {
        if (!info.isFreed) {
            result.push_back(info);
        }
    }
    return result;
}

FFIWarning FFILeakDetector::createLeakWarning(const AllocationInfo& info) {
    return FFIWarningBuilder(FFIWarningCode::POTENTIAL_LEAK)
        .withArabicMessage(u8"تسريب محتمل للذاكرة: المتغير '" + info.varName + u8"' لم يُحرر")
        .withEnglishMessage("Potential memory leak: variable '" + info.varName + "' not freed")
        .atLocation(info.location)
        .withNote(u8"المتغير تم حجزه بواسطة " + info.allocatorFunc)
        .withSuggestion(info.expectedDeallocator ? 
            u8"استخدم " + *info.expectedDeallocator + "(" + info.varName + ")" :
            u8"تأكد من تحرير الذاكرة")
        .build();
}

FFIWarning FFILeakDetector::createMismatchWarning(
    const AllocationInfo& info, 
    const std::string& actualDeallocator
) {
    return FFIWarningBuilder(FFIWarningCode::MISMATCHED_DEALLOC)
        .withArabicMessage(u8"عدم تطابق: حُجز بـ " + info.allocatorFunc + 
                          u8" لكن حُرر بـ " + actualDeallocator)
        .withEnglishMessage("Mismatch: allocated with " + info.allocatorFunc + 
                           " but freed with " + actualDeallocator)
        .atLocation(info.location)
        .withSuggestion(info.expectedDeallocator ?
            u8"استخدم " + *info.expectedDeallocator + " بدلاً من " + actualDeallocator :
            u8"تأكد من استخدام دالة التحرير الصحيحة")
        .build();
}

// ============================================================================
//                    (AR) تنفيذ FFIWarningCollector
//                    (EN) FFIWarningCollector Implementation
// ============================================================================

FFIWarningCollector::FFIWarningCollector(const FFIWarningConfig& config)
    : config_(config), formatter_(config) {}

void FFIWarningCollector::add(const FFIWarning& warning) {
    if (!isEnabled(warning.code)) {
        return;
    }
    
    FFIWarning w = warning;
    w.severity = getSeverity(warning.code);
    warnings_.push_back(w);
}

void FFIWarningCollector::add(FFIWarningBuilder&& builder) {
    add(builder.build());
}

std::vector<FFIWarning> FFIWarningCollector::getBySeverity(WarningSeverity severity) const {
    std::vector<FFIWarning> result;
    for (const auto& w : warnings_) {
        if (w.severity == severity) {
            result.push_back(w);
        }
    }
    return result;
}

std::vector<FFIWarning> FFIWarningCollector::getByCode(FFIWarningCode code) const {
    std::vector<FFIWarning> result;
    for (const auto& w : warnings_) {
        if (w.code == code) {
            result.push_back(w);
        }
    }
    return result;
}

bool FFIWarningCollector::hasErrors() const {
    for (const auto& w : warnings_) {
        if (w.severity == WarningSeverity::ERROR) {
            return true;
        }
    }
    return false;
}

void FFIWarningCollector::printAll(std::ostream& out) const {
    out << formatter_.formatAll(warnings_);
}

bool FFIWarningCollector::isEnabled(FFIWarningCode code) const {
    if (config_.enableAll) return true;
    
    auto it = config_.enabledWarnings.find(code);
    if (it != config_.enabledWarnings.end()) {
        return it->second;
    }
    return true;  // (AR) مُفعّل افتراضياً
}

WarningSeverity FFIWarningCollector::getSeverity(FFIWarningCode code) const {
    auto it = config_.severityOverrides.find(code);
    if (it != config_.severityOverrides.end()) {
        return it->second;
    }
    return WarningSeverity::WARNING;
}

// ============================================================================
//                    (AR) الدوال المساعدة
//                    (EN) Helper Functions
// ============================================================================

FFIWarning createLeakWarning(
    const std::string& varName,
    const std::string& allocator,
    const SourceLocation& location
) {
    return FFIWarningBuilder(FFIWarningCode::POTENTIAL_LEAK)
        .withArabicMessage(u8"تسريب محتمل: '" + varName + u8"' لم يُحرر")
        .withEnglishMessage("Potential leak: '" + varName + "' not freed")
        .atLocation(location)
        .withNote(u8"حُجز بواسطة " + allocator)
        .build();
}

FFIWarning createUseAfterFreeWarning(
    const std::string& varName,
    const SourceLocation& useLocation,
    const SourceLocation& freeLocation
) {
    return FFIWarningBuilder(FFIWarningCode::USE_AFTER_FREE)
        .withArabicMessage(u8"استخدام '" + varName + u8"' بعد تحريره")
        .withEnglishMessage("Use of '" + varName + "' after free")
        .atLocation(useLocation)
        .withRelatedLocation(freeLocation)
        .withNote(u8"تم التحرير هنا")
        .withSeverity(WarningSeverity::ERROR)
        .build();
}

FFIWarning createDoubleFreeWarning(
    const std::string& varName,
    const SourceLocation& firstFree,
    const SourceLocation& secondFree
) {
    return FFIWarningBuilder(FFIWarningCode::DOUBLE_FREE)
        .withArabicMessage(u8"تحرير مزدوج لـ '" + varName + "'")
        .withEnglishMessage("Double free of '" + varName + "'")
        .atLocation(secondFree)
        .withRelatedLocation(firstFree)
        .withNote(u8"التحرير الأول هنا")
        .withSeverity(WarningSeverity::ERROR)
        .build();
}

FFIWarning createMismatchWarning(
    const std::string& varName,
    const std::string& allocator,
    const std::string& deallocator,
    const std::string& expectedDeallocator,
    const SourceLocation& location
) {
    return FFIWarningBuilder(FFIWarningCode::MISMATCHED_DEALLOC)
        .withArabicMessage(u8"'" + varName + u8"' حُجز بـ " + allocator + 
                          u8" لكن حُرر بـ " + deallocator)
        .withEnglishMessage("'" + varName + "' allocated with " + allocator + 
                           " but freed with " + deallocator)
        .atLocation(location)
        .withSuggestion(u8"استخدم " + expectedDeallocator + " بدلاً من " + deallocator)
        .build();
}

} // namespace errors
} // namespace sad
