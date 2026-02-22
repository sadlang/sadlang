// بسم الله الرحمن الرحيم
/**
 * ══════════════════════════════════════════════════════════════════════════════
 * ملف: performance_analyzer_provider.cpp
 * الوصف: تنفيذ محلل الأداء الفوري للغة ص
 * ══════════════════════════════════════════════════════════════════════════════
 */

#include "performance_analyzer_provider.h"
#include <regex>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <set>

namespace sad {
namespace lsp {
namespace analysis {

// ══════════════════════════════════════════════════════════════════════════════
//  المنشئ والمدمر
// ══════════════════════════════════════════════════════════════════════════════

PerformanceAnalyzerProvider::PerformanceAnalyzerProvider() = default;
PerformanceAnalyzerProvider::~PerformanceAnalyzerProvider() = default;

// ══════════════════════════════════════════════════════════════════════════════
//  التحليل الرئيسي
// ══════════════════════════════════════════════════════════════════════════════

PerformanceReport PerformanceAnalyzerProvider::analyze(
    const std::string& document,
    const std::string& uri
) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    PerformanceReport report;
    report.document_uri = uri;
    
    // تحليل الوحدة
    ModuleAnalysis& module = report.module;
    module.path = uri;
    
    // حساب أنواع الأسطر
    std::istringstream stream(document);
    std::string line;
    while (std::getline(stream, line)) {
        module.total_lines++;
        
        // إزالة المسافات
        auto trimmed = line;
        trimmed.erase(0, trimmed.find_first_not_of(" \t"));
        
        if (trimmed.empty()) {
            module.blank_lines++;
        } else if (trimmed.find("##") == 0) {
            module.comment_lines++;
        } else {
            module.code_lines++;
        }
        
        // عد الدوال والأصناف
        if (line.find("دالة ") != std::string::npos) {
            module.function_count++;
        }
        if (line.find("صنف ") != std::string::npos) {
            module.class_count++;
        }
    }
    
    // استخراج وتحليل الدوال
    std::vector<std::string> functions = extract_functions(document);
    for (const auto& func : functions) {
        // استخراج اسم الدالة
        std::regex func_regex(R"(دالة\s+(\w+))");
        std::smatch match;
        std::string func_name = "مجهول";
        if (std::regex_search(func, match, func_regex)) {
            func_name = match[1].str();
        }
        
        FunctionAnalysis func_analysis = analyze_function(func, func_name);
        module.functions.push_back(func_analysis);
        
        // إضافة مشاكل الدالة للوحدة
        for (const auto& issue : func_analysis.issues) {
            module.issues.push_back(issue);
        }
    }
    
    // حساب متوسط التعقيد
    if (!module.functions.empty()) {
        double total = 0;
        for (const auto& f : module.functions) {
            total += static_cast<int>(f.time_complexity);
        }
        module.average_complexity = total / module.functions.size();
    }
    
    // أقصى عمق حلقات
    module.max_loop_depth = count_loop_depth(document);
    
    // تحليل الذاكرة
    if (analyze_memory_) {
        report.memory = analyze_memory_usage(document);
    }
    
    // البحث عن أنماط سيئة على مستوى الملف
    auto loop_issues = analyze_loops(document);
    auto recursion_issues = analyze_recursion(document);
    auto string_issues = detect_string_concat_in_loops(document);
    auto redundant_issues = detect_redundant_computations(document);
    auto search_issues = detect_inefficient_search(document);
    auto dead_code = find_dead_code(document);
    
    // جمع كل المشاكل
    report.all_issues.insert(report.all_issues.end(), 
        module.issues.begin(), module.issues.end());
    report.all_issues.insert(report.all_issues.end(), 
        loop_issues.begin(), loop_issues.end());
    report.all_issues.insert(report.all_issues.end(), 
        recursion_issues.begin(), recursion_issues.end());
    report.all_issues.insert(report.all_issues.end(), 
        string_issues.begin(), string_issues.end());
    report.all_issues.insert(report.all_issues.end(), 
        redundant_issues.begin(), redundant_issues.end());
    report.all_issues.insert(report.all_issues.end(), 
        search_issues.begin(), search_issues.end());
    report.all_issues.insert(report.all_issues.end(), 
        dead_code.begin(), dead_code.end());
    
    // إحصاء المشاكل حسب الخطورة
    for (const auto& issue : report.all_issues) {
        switch (issue.severity) {
            case PerformanceSeverity::Info:
                report.info_count++;
                break;
            case PerformanceSeverity::Warning:
                report.warning_count++;
                break;
            case PerformanceSeverity::Critical:
                report.critical_count++;
                break;
        }
    }
    
    // حساب التصنيف
    int penalty = report.critical_count * 20 + report.warning_count * 5 + report.info_count;
    if (penalty == 0) {
        report.grade = 'A';
        report.summary = "أداء ممتاز! لا توجد مشاكل.";
    } else if (penalty < 10) {
        report.grade = 'B';
        report.summary = "أداء جيد مع بعض التحسينات الممكنة.";
    } else if (penalty < 25) {
        report.grade = 'C';
        report.summary = "أداء مقبول، يُنصح بمراجعة التحذيرات.";
    } else if (penalty < 50) {
        report.grade = 'D';
        report.summary = "أداء ضعيف، يتطلب تحسينات.";
    } else {
        report.grade = 'F';
        report.summary = "أداء سيء، يتطلب إعادة هيكلة.";
    }
    
    // وقت التحليل
    auto end_time = std::chrono::high_resolution_clock::now();
    report.analysis_time = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_time - start_time
    );
    
    return report;
}

std::vector<PerformanceIssue> PerformanceAnalyzerProvider::analyze_visible_range(
    const std::string& document,
    int start_line,
    int end_line
) {
    std::vector<PerformanceIssue> issues;
    
    // استخراج الأسطر المرئية
    std::vector<std::string> lines;
    std::istringstream stream(document);
    std::string line;
    int line_num = 0;
    std::string visible_code;
    
    while (std::getline(stream, line)) {
        if (line_num >= start_line && line_num <= end_line) {
            visible_code += line + "\n";
            lines.push_back(line);
        }
        line_num++;
    }
    
    // تحليل سريع للمنطقة المرئية
    auto loop_issues = analyze_loops(visible_code);
    auto string_issues = detect_string_concat_in_loops(visible_code);
    
    issues.insert(issues.end(), loop_issues.begin(), loop_issues.end());
    issues.insert(issues.end(), string_issues.begin(), string_issues.end());
    
    // تعديل أرقام الأسطر
    for (auto& issue : issues) {
        issue.range.start_line += start_line;
        issue.range.end_line += start_line;
    }
    
    return issues;
}

FunctionAnalysis PerformanceAnalyzerProvider::analyze_function(
    const std::string& function_code,
    const std::string& function_name
) {
    FunctionAnalysis analysis;
    analysis.name = function_name;
    
    // حساب التعقيد
    analysis.time_complexity = calculate_time_complexity(function_code);
    analysis.space_complexity = calculate_space_complexity(function_code);
    
    // عمق الحلقات
    analysis.loop_depth = count_loop_depth(function_code);
    
    // عدد الأسطر
    analysis.line_count = std::count(function_code.begin(), function_code.end(), '\n') + 1;
    
    // عدد الفروع
    analysis.branch_count = 0;
    for (const auto& keyword : {"إذا", "وإلا_إذا", "وإلا"}) {
        size_t pos = 0;
        while ((pos = function_code.find(keyword, pos)) != std::string::npos) {
            analysis.branch_count++;
            pos += strlen(keyword);
        }
    }
    
    // عدد المتغيرات
    std::regex var_regex(R"(متغير\s+\w+)");
    analysis.variable_count = std::distance(
        std::sregex_iterator(function_code.begin(), function_code.end(), var_regex),
        std::sregex_iterator()
    );
    
    // فحص التكرار
    analysis.uses_recursion = function_code.find(function_name + "(") != std::string::npos;
    if (analysis.uses_recursion) {
        analysis.has_tail_recursion = is_tail_recursive(function_code);
    }
    
    // إنشاء مشاكل
    
    // دالة طويلة
    if (analysis.line_count > max_function_lines_) {
        PerformanceIssue issue;
        issue.id = "long_function_" + function_name;
        issue.type = PerformanceIssueType::RedundantComputation;
        issue.severity = PerformanceSeverity::Warning;
        issue.title = "دالة طويلة";
        issue.description = "الدالة '" + function_name + "' تحتوي على " + 
                           std::to_string(analysis.line_count) + " سطر";
        issue.suggestion = "فكر في تقسيم الدالة لدوال أصغر";
        analysis.issues.push_back(issue);
    }
    
    // تعقيد عالي
    if (analysis.time_complexity >= max_acceptable_complexity_) {
        PerformanceIssue issue;
        issue.id = "high_complexity_" + function_name;
        issue.type = PerformanceIssueType::NestedLoops;
        issue.severity = analysis.time_complexity >= TimeComplexity::Exponential 
            ? PerformanceSeverity::Critical 
            : PerformanceSeverity::Warning;
        issue.title = "تعقيد زمني مرتفع";
        issue.description = "التعقيد الزمني " + 
                           time_complexity_to_string(analysis.time_complexity);
        issue.time_complexity = analysis.time_complexity;
        analysis.issues.push_back(issue);
    }
    
    // حلقات عميقة
    if (analysis.loop_depth > max_loop_depth_) {
        PerformanceIssue issue;
        issue.id = "deep_loops_" + function_name;
        issue.type = PerformanceIssueType::NestedLoops;
        issue.severity = PerformanceSeverity::Warning;
        issue.title = "حلقات متداخلة بعمق";
        issue.description = "عمق الحلقات: " + std::to_string(analysis.loop_depth);
        issue.suggestion = "استخدم خريطة أو فهرس لتقليل التداخل";
        analysis.issues.push_back(issue);
    }
    
    // تكرار بدون تحسين
    if (analysis.uses_recursion && !analysis.has_tail_recursion) {
        PerformanceIssue issue;
        issue.id = "no_tail_recursion_" + function_name;
        issue.type = PerformanceIssueType::UnoptimizedRecursion;
        issue.severity = PerformanceSeverity::Warning;
        issue.title = "تكرار غير ذيلي";
        issue.description = "التكرار ليس ذيلياً وقد يسبب stack overflow";
        issue.suggestion = "حول إلى تكرار ذيلي أو استخدم حلقة";
        analysis.issues.push_back(issue);
    }
    
    return analysis;
}

// ══════════════════════════════════════════════════════════════════════════════
//  حساب التعقيد
// ══════════════════════════════════════════════════════════════════════════════

TimeComplexity PerformanceAnalyzerProvider::calculate_time_complexity(
    const std::string& code
) {
    int loop_depth = count_loop_depth(code);
    
    // فحص التكرار
    std::regex func_regex(R"(دالة\s+(\w+))");
    std::smatch match;
    std::string func_name;
    if (std::regex_search(code, match, func_regex)) {
        func_name = match[1].str();
    }
    
    bool has_recursion = !func_name.empty() && 
                         code.find(func_name + "(") != std::string::npos;
    
    // التكرار الأسي (مثل فيبوناتشي البسيط)
    if (has_recursion) {
        // البحث عن استدعاءين تكراريين
        size_t first_call = code.find(func_name + "(");
        if (first_call != std::string::npos) {
            size_t second_call = code.find(func_name + "(", first_call + 1);
            if (second_call != std::string::npos) {
                return TimeComplexity::Exponential;
            }
        }
    }
    
    // بناءً على عمق الحلقات
    switch (loop_depth) {
        case 0: return TimeComplexity::Constant;
        case 1: return TimeComplexity::Linear;
        case 2: return TimeComplexity::Quadratic;
        case 3: return TimeComplexity::Cubic;
        default: return TimeComplexity::Exponential;
    }
}

SpaceComplexity PerformanceAnalyzerProvider::calculate_space_complexity(
    const std::string& code
) {
    // حساب القوائم والخرائط المُنشأة
    int list_count = 0;
    int map_count = 0;
    
    // قوائم جديدة
    std::regex list_regex(R"(\[\]|قائمة\(\))");
    list_count = std::distance(
        std::sregex_iterator(code.begin(), code.end(), list_regex),
        std::sregex_iterator()
    );
    
    // خرائط جديدة
    std::regex map_regex(R"(\{\}|خريطة\(\))");
    map_count = std::distance(
        std::sregex_iterator(code.begin(), code.end(), map_regex),
        std::sregex_iterator()
    );
    
    // فحص إضافة للقوائم في حلقات
    bool grows_in_loop = false;
    int loop_depth = count_loop_depth(code);
    
    if (loop_depth > 0) {
        if (code.find(".أضف(") != std::string::npos ||
            code.find(".دفع(") != std::string::npos) {
            grows_in_loop = true;
        }
    }
    
    if (grows_in_loop && loop_depth >= 2) {
        return SpaceComplexity::Quadratic;
    } else if (grows_in_loop || list_count > 0 || map_count > 0) {
        return SpaceComplexity::Linear;
    } else {
        return SpaceComplexity::Constant;
    }
}

// ══════════════════════════════════════════════════════════════════════════════
//  تحليل الحلقات والتكرار
// ══════════════════════════════════════════════════════════════════════════════

std::vector<PerformanceIssue> PerformanceAnalyzerProvider::analyze_loops(
    const std::string& code
) {
    std::vector<PerformanceIssue> issues;
    
    // البحث عن حلقات متداخلة
    std::vector<std::string> lines;
    std::istringstream stream(code);
    std::string line;
    int line_num = 0;
    int loop_depth = 0;
    int max_depth = 0;
    int nested_start = -1;
    
    while (std::getline(stream, line)) {
        if (line.find("لكل ") != std::string::npos ||
            line.find("طالما ") != std::string::npos ||
            line.find("كرر ") != std::string::npos) {
            loop_depth++;
            if (loop_depth > 1 && nested_start == -1) {
                nested_start = line_num;
            }
            if (loop_depth > max_depth) {
                max_depth = loop_depth;
            }
        }
        
        if (line.find("نهاية") != std::string::npos && loop_depth > 0) {
            loop_depth--;
        }
        
        line_num++;
    }
    
    if (max_depth > max_loop_depth_) {
        PerformanceIssue issue;
        issue.id = "nested_loops";
        issue.type = PerformanceIssueType::NestedLoops;
        issue.severity = max_depth >= 4 
            ? PerformanceSeverity::Critical 
            : PerformanceSeverity::Warning;
        issue.title = "حلقات متداخلة بعمق " + std::to_string(max_depth);
        issue.description = "الحلقات المتداخلة تزيد التعقيد الزمني بشكل أسي";
        issue.suggestion = "استخدم خريطة للفهرسة أو أعد هيكلة الخوارزمية";
        issue.range.start_line = nested_start;
        issues.push_back(issue);
    }
    
    return issues;
}

std::vector<PerformanceIssue> PerformanceAnalyzerProvider::analyze_recursion(
    const std::string& code
) {
    std::vector<PerformanceIssue> issues;
    
    // استخراج أسماء الدوال
    std::regex func_regex(R"(دالة\s+(\w+))");
    std::sregex_iterator it(code.begin(), code.end(), func_regex);
    std::sregex_iterator end;
    
    while (it != end) {
        std::string func_name = (*it)[1].str();
        size_t func_start = it->position();
        
        // البحث عن نهاية الدالة
        size_t func_end = code.find("نهاية", func_start);
        if (func_end != std::string::npos) {
            std::string func_body = code.substr(func_start, func_end - func_start);
            
            // عد الاستدعاءات التكرارية
            size_t call_count = 0;
            size_t pos = 0;
            while ((pos = func_body.find(func_name + "(", pos)) != std::string::npos) {
                call_count++;
                pos += func_name.length();
            }
            
            // أكثر من استدعاء = استدعاءان على الأقل (التعريف + استدعاء واحد)
            if (call_count > 1) {
                // أكثر من استدعاء تكراري واحد = تعقيد أسي محتمل
                if (call_count > 2) {
                    PerformanceIssue issue;
                    issue.id = "multiple_recursive_calls_" + func_name;
                    issue.type = PerformanceIssueType::UnoptimizedRecursion;
                    issue.severity = PerformanceSeverity::Critical;
                    issue.title = "استدعاءات تكرارية متعددة";
                    issue.description = "الدالة '" + func_name + "' تستدعي نفسها " +
                                       std::to_string(call_count - 1) + " مرات";
                    issue.suggestion = "استخدم التذكير (memoization) أو أعد كتابة بحلقة";
                    issues.push_back(issue);
                }
            }
        }
        
        ++it;
    }
    
    return issues;
}

MemoryProfile PerformanceAnalyzerProvider::analyze_memory_usage(
    const std::string& code
) {
    MemoryProfile profile;
    profile.allocation_count = 0;
    profile.deallocation_count = 0;
    
    // عد التخصيصات
    std::regex alloc_regex(R"((قائمة|خريطة|مجموعة)\s*\(|(\[\]|\{\}))");
    profile.allocation_count = std::distance(
        std::sregex_iterator(code.begin(), code.end(), alloc_regex),
        std::sregex_iterator()
    );
    
    // البحث عن تخصيصات كبيرة
    std::regex large_alloc_regex(R"(\[.{50,}\])");
    std::sregex_iterator it(code.begin(), code.end(), large_alloc_regex);
    while (it != std::sregex_iterator()) {
        profile.large_allocations.push_back(it->str().substr(0, 50) + "...");
        ++it;
    }
    
    // فحص تسربات محتملة (موارد مفتوحة بدون إغلاق)
    if (code.find("افتح(") != std::string::npos &&
        code.find("اغلق(") == std::string::npos) {
        profile.potential_leaks.push_back("ملف مفتوح بدون إغلاق");
    }
    
    return profile;
}

// ══════════════════════════════════════════════════════════════════════════════
//  اكتشاف الأنماط السيئة
// ══════════════════════════════════════════════════════════════════════════════

std::vector<PerformanceIssue> PerformanceAnalyzerProvider::detect_string_concat_in_loops(
    const std::string& code
) {
    std::vector<PerformanceIssue> issues;
    
    bool in_loop = false;
    int line_num = 0;
    
    std::istringstream stream(code);
    std::string line;
    
    while (std::getline(stream, line)) {
        if (line.find("لكل ") != std::string::npos ||
            line.find("طالما ") != std::string::npos) {
            in_loop = true;
        }
        
        if (in_loop && line.find("نهاية") != std::string::npos) {
            in_loop = false;
        }
        
        // دمج نصوص في حلقة
        if (in_loop && (line.find("+ \"") != std::string::npos ||
                        line.find("\" +") != std::string::npos)) {
            PerformanceIssue issue;
            issue.id = "string_concat_loop_" + std::to_string(line_num);
            issue.type = PerformanceIssueType::StringConcatInLoop;
            issue.severity = PerformanceSeverity::Warning;
            issue.title = "دمج نصوص في حلقة";
            issue.description = "دمج النصوص في الحلقات بطيء O(n²)";
            issue.suggestion = "استخدم قائمة ثم انضم()";
            issue.range.start_line = line_num;
            issue.range.end_line = line_num;
            issues.push_back(issue);
        }
        
        line_num++;
    }
    
    return issues;
}

std::vector<PerformanceIssue> PerformanceAnalyzerProvider::detect_redundant_computations(
    const std::string& code
) {
    std::vector<PerformanceIssue> issues;
    
    // البحث عن استدعاءات متكررة لنفس الدالة
    std::map<std::string, int> call_counts;
    std::regex call_regex(R"((\w+)\([^)]*\))");
    
    std::sregex_iterator it(code.begin(), code.end(), call_regex);
    while (it != std::sregex_iterator()) {
        std::string call = it->str();
        call_counts[call]++;
        ++it;
    }
    
    for (const auto& [call, count] : call_counts) {
        if (count > 3 && call.find("اطبع") == std::string::npos) {
            PerformanceIssue issue;
            issue.id = "redundant_call_" + call;
            issue.type = PerformanceIssueType::RedundantComputation;
            issue.severity = PerformanceSeverity::Info;
            issue.title = "استدعاء متكرر";
            issue.description = "'" + call + "' يُستدعى " + 
                               std::to_string(count) + " مرات";
            issue.suggestion = "خزن النتيجة في متغير";
            issues.push_back(issue);
        }
    }
    
    return issues;
}

std::vector<PerformanceIssue> PerformanceAnalyzerProvider::detect_inefficient_search(
    const std::string& code
) {
    std::vector<PerformanceIssue> issues;
    
    // البحث عن بحث خطي يمكن تحسينه
    bool has_linear_search = false;
    bool in_loop = false;
    int line_num = 0;
    
    std::istringstream stream(code);
    std::string line;
    
    while (std::getline(stream, line)) {
        if (line.find("لكل ") != std::string::npos) {
            in_loop = true;
        }
        
        if (in_loop && (line.find(" == ") != std::string::npos ||
                        line.find(" في ") != std::string::npos)) {
            has_linear_search = true;
            
            PerformanceIssue issue;
            issue.id = "linear_search_" + std::to_string(line_num);
            issue.type = PerformanceIssueType::IneffientSearch;
            issue.severity = PerformanceSeverity::Info;
            issue.title = "بحث خطي O(n)";
            issue.description = "البحث الخطي يمكن تحسينه باستخدام خريطة أو مجموعة";
            issue.suggestion = "استخدم خريطة للبحث O(1)";
            issue.range.start_line = line_num;
            issues.push_back(issue);
        }
        
        if (line.find("نهاية") != std::string::npos && in_loop) {
            in_loop = false;
        }
        
        line_num++;
    }
    
    return issues;
}

std::vector<PerformanceIssue> PerformanceAnalyzerProvider::detect_unoptimized_recursion(
    const std::string& code
) {
    return analyze_recursion(code);
}

std::vector<PerformanceIssue> PerformanceAnalyzerProvider::find_dead_code(
    const std::string& code
) {
    std::vector<PerformanceIssue> issues;
    
    // البحث عن كود بعد إرجاع
    int line_num = 0;
    bool after_return = false;
    int return_line = -1;
    
    std::istringstream stream(code);
    std::string line;
    
    while (std::getline(stream, line)) {
        auto trimmed = line;
        trimmed.erase(0, trimmed.find_first_not_of(" \t"));
        
        if (after_return && !trimmed.empty() && 
            trimmed.find("نهاية") != 0 &&
            trimmed.find("##") != 0) {
            
            PerformanceIssue issue;
            issue.id = "dead_code_" + std::to_string(line_num);
            issue.type = PerformanceIssueType::DeadCode;
            issue.severity = PerformanceSeverity::Warning;
            issue.title = "كود ميت";
            issue.description = "هذا الكود لن يُنفذ أبداً (بعد إرجاع)";
            issue.suggestion = "احذف هذا الكود";
            issue.range.start_line = line_num;
            issues.push_back(issue);
        }
        
        if (trimmed.find("إرجاع ") == 0) {
            after_return = true;
            return_line = line_num;
        }
        
        if (trimmed.find("نهاية") == 0) {
            after_return = false;
        }
        
        line_num++;
    }
    
    // البحث عن متغيرات غير مستخدمة
    auto var_usage = count_variable_usage(code);
    for (const auto& [var, count] : var_usage) {
        if (count == 1) {  // مُعرّف فقط ولم يُستخدم
            PerformanceIssue issue;
            issue.id = "unused_var_" + var;
            issue.type = PerformanceIssueType::UnusedVariable;
            issue.severity = PerformanceSeverity::Info;
            issue.title = "متغير غير مستخدم";
            issue.description = "المتغير '" + var + "' مُعرّف لكن غير مستخدم";
            issue.suggestion = "احذف المتغير أو استخدمه";
            issues.push_back(issue);
        }
    }
    
    return issues;
}

// ══════════════════════════════════════════════════════════════════════════════
//  الاقتراحات والتحسينات
// ══════════════════════════════════════════════════════════════════════════════

std::string PerformanceAnalyzerProvider::suggest_improvement(
    const PerformanceIssue& issue
) {
    std::ostringstream suggestion;
    
    switch (issue.type) {
        case PerformanceIssueType::NestedLoops:
            suggestion << "## تحسين الحلقات المتداخلة\n\n";
            suggestion << "بدلاً من:\n```\n";
            suggestion << "لكل أ في قائمة1\n";
            suggestion << "    لكل ب في قائمة2\n";
            suggestion << "        إذا أ == ب\n";
            suggestion << "            ## ...\n```\n\n";
            suggestion << "استخدم:\n```\n";
            suggestion << "متغير مجموعة2 = مجموعة(قائمة2)\n";
            suggestion << "لكل أ في قائمة1\n";
            suggestion << "    إذا أ في مجموعة2\n";
            suggestion << "        ## ...\n```\n";
            break;
            
        case PerformanceIssueType::StringConcatInLoop:
            suggestion << "## تحسين دمج النصوص\n\n";
            suggestion << "بدلاً من:\n```\n";
            suggestion << "متغير نتيجة = \"\"\n";
            suggestion << "لكل س في القائمة\n";
            suggestion << "    نتيجة = نتيجة + س\n```\n\n";
            suggestion << "استخدم:\n```\n";
            suggestion << "متغير أجزاء = []\n";
            suggestion << "لكل س في القائمة\n";
            suggestion << "    أجزاء.أضف(س)\n";
            suggestion << "متغير نتيجة = انضم(أجزاء, \"\")\n```\n";
            break;
            
        case PerformanceIssueType::UnoptimizedRecursion:
            suggestion << "## تحسين التكرار\n\n";
            suggestion << "استخدم التذكير (memoization):\n```\n";
            suggestion << "متغير ذاكرة = {}\n\n";
            suggestion << "دالة فيب(ن)\n";
            suggestion << "    إذا ن في ذاكرة\n";
            suggestion << "        إرجاع ذاكرة[ن]\n";
            suggestion << "    \n";
            suggestion << "    متغير نتيجة = فيب(ن - 1) + فيب(ن - 2)\n";
            suggestion << "    ذاكرة[ن] = نتيجة\n";
            suggestion << "    إرجاع نتيجة\n```\n";
            break;
            
        default:
            suggestion << issue.suggestion;
            break;
    }
    
    return suggestion.str();
}

std::optional<std::string> PerformanceAnalyzerProvider::generate_optimized_code(
    const std::string& original_code,
    const PerformanceIssue& issue
) {
    // حسب نوع المشكلة
    switch (issue.type) {
        case PerformanceIssueType::StringConcatInLoop:
            // تحويل دمج نصوص لقائمة
            {
                std::string optimized = original_code;
                // هذا تبسيط - التحويل الحقيقي يحتاج تحليل أعمق
                return optimized;
            }
            
        default:
            return std::nullopt;
    }
}

std::string PerformanceAnalyzerProvider::explain_issue(
    const PerformanceIssue& issue
) {
    std::ostringstream explanation;
    
    explanation << "## " << issue.title << "\n\n";
    explanation << "**الخطورة:** " << severity_to_string(issue.severity) << "\n\n";
    explanation << "**المشكلة:** " << issue.description << "\n\n";
    
    if (issue.time_complexity != TimeComplexity::Unknown) {
        explanation << "**التعقيد الزمني:** " 
                   << time_complexity_to_string(issue.time_complexity) << "\n\n";
    }
    
    explanation << "**الحل:** " << issue.suggestion << "\n";
    
    return explanation.str();
}

// ══════════════════════════════════════════════════════════════════════════════
//  أدوات داخلية
// ══════════════════════════════════════════════════════════════════════════════

int PerformanceAnalyzerProvider::count_loop_depth(const std::string& code) {
    int max_depth = 0;
    int current_depth = 0;
    
    std::istringstream stream(code);
    std::string line;
    
    while (std::getline(stream, line)) {
        if (line.find("لكل ") != std::string::npos ||
            line.find("طالما ") != std::string::npos ||
            line.find("كرر ") != std::string::npos) {
            current_depth++;
            if (current_depth > max_depth) {
                max_depth = current_depth;
            }
        }
        
        // نهاية قد تكون لأي بلوك
        // نحتاج تتبع أدق - تبسيط هنا
        if (line.find("نهاية") != std::string::npos && current_depth > 0) {
            current_depth--;
        }
    }
    
    return max_depth;
}

bool PerformanceAnalyzerProvider::is_tail_recursive(const std::string& code) {
    // تحقق بسيط: الاستدعاء التكراري هو آخر شيء قبل نهاية
    // هذا تبسيط - التحليل الحقيقي يحتاج AST
    
    std::regex func_regex(R"(دالة\s+(\w+))");
    std::smatch match;
    if (!std::regex_search(code, match, func_regex)) {
        return false;
    }
    
    std::string func_name = match[1].str();
    
    // البحث عن إرجاع func_name(
    std::string pattern = "إرجاع " + func_name + "(";
    return code.find(pattern) != std::string::npos;
}

std::vector<std::string> PerformanceAnalyzerProvider::extract_functions(
    const std::string& code
) {
    std::vector<std::string> functions;
    
    size_t pos = 0;
    while ((pos = code.find("دالة ", pos)) != std::string::npos) {
        // إيجاد نهاية الدالة
        int depth = 1;
        size_t end_pos = pos + 5;
        
        while (end_pos < code.size() && depth > 0) {
            if (code.substr(end_pos, 5) == "دالة " ||
                code.substr(end_pos, 4) == "إذا " ||
                code.substr(end_pos, 4) == "لكل " ||
                code.substr(end_pos, 6) == "طالما ") {
                depth++;
            }
            if (code.substr(end_pos, 5) == "نهاية") {
                depth--;
            }
            end_pos++;
        }
        
        if (depth == 0) {
            functions.push_back(code.substr(pos, end_pos - pos + 4));
        }
        
        pos = end_pos;
    }
    
    return functions;
}

std::map<std::string, int> PerformanceAnalyzerProvider::count_variable_usage(
    const std::string& code
) {
    std::map<std::string, int> usage;
    
    // استخراج تعريفات المتغيرات
    std::regex def_regex(R"(متغير\s+(\w+))");
    std::sregex_iterator it(code.begin(), code.end(), def_regex);
    
    while (it != std::sregex_iterator()) {
        std::string var_name = (*it)[1].str();
        
        // عد مرات الظهور
        size_t count = 0;
        size_t pos = 0;
        while ((pos = code.find(var_name, pos)) != std::string::npos) {
            count++;
            pos += var_name.length();
        }
        
        usage[var_name] = count;
        ++it;
    }
    
    return usage;
}

// ══════════════════════════════════════════════════════════════════════════════
//  دوال مساعدة عامة
// ══════════════════════════════════════════════════════════════════════════════

std::string time_complexity_to_string(TimeComplexity complexity) {
    switch (complexity) {
        case TimeComplexity::Constant: return "O(1) - ثابت";
        case TimeComplexity::Logarithmic: return "O(log n) - لوغاريتمي";
        case TimeComplexity::Linear: return "O(n) - خطي";
        case TimeComplexity::Linearithmic: return "O(n log n) - خطي-لوغاريتمي";
        case TimeComplexity::Quadratic: return "O(n²) - تربيعي";
        case TimeComplexity::Cubic: return "O(n³) - تكعيبي";
        case TimeComplexity::Exponential: return "O(2^n) - أسي";
        case TimeComplexity::Factorial: return "O(n!) - عاملي";
        default: return "غير معروف";
    }
}

std::string space_complexity_to_string(SpaceComplexity complexity) {
    switch (complexity) {
        case SpaceComplexity::Constant: return "O(1) - ثابت";
        case SpaceComplexity::Logarithmic: return "O(log n) - لوغاريتمي";
        case SpaceComplexity::Linear: return "O(n) - خطي";
        case SpaceComplexity::Quadratic: return "O(n²) - تربيعي";
        default: return "غير معروف";
    }
}

std::string severity_to_string(PerformanceSeverity severity) {
    switch (severity) {
        case PerformanceSeverity::Info: return "معلومة";
        case PerformanceSeverity::Warning: return "تحذير";
        case PerformanceSeverity::Critical: return "حرج";
        default: return "غير معروف";
    }
}

std::string issue_type_to_string(PerformanceIssueType type) {
    switch (type) {
        case PerformanceIssueType::NestedLoops: return "حلقات متداخلة";
        case PerformanceIssueType::IneffientSearch: return "بحث غير فعال";
        case PerformanceIssueType::RedundantComputation: return "حساب مكرر";
        case PerformanceIssueType::UnoptimizedRecursion: return "تكرار غير محسن";
        case PerformanceIssueType::MemoryLeak: return "تسرب ذاكرة";
        case PerformanceIssueType::LargeAllocation: return "تخصيص كبير";
        case PerformanceIssueType::UnreleasedResources: return "موارد غير محررة";
        case PerformanceIssueType::CyclicReference: return "مرجع دائري";
        case PerformanceIssueType::StringConcatInLoop: return "دمج نصوص في حلقة";
        case PerformanceIssueType::UnneededStringCopy: return "نسخ نص غير ضروري";
        case PerformanceIssueType::SynchronousIO: return "إدخال/إخراج متزامن";
        case PerformanceIssueType::UnbufferedIO: return "إدخال/إخراج غير مخزن";
        case PerformanceIssueType::PrematureOptimization: return "تحسين مبكر";
        case PerformanceIssueType::DeadCode: return "كود ميت";
        case PerformanceIssueType::UnusedVariable: return "متغير غير مستخدم";
        default: return "غير معروف";
    }
}

std::string grade_emoji(char grade) {
    switch (grade) {
        case 'A': return "🌟";
        case 'B': return "✅";
        case 'C': return "⚠️";
        case 'D': return "❌";
        case 'F': return "🚨";
        default: return "❓";
    }
}

} // namespace analysis
} // namespace lsp
} // namespace sad
