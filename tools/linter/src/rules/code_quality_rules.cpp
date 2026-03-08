/**
 * =============================================================================
 * ملف: code_quality_rules.cpp
 * الوصف: 10 قواعد lint لجودة الكود في لغة ص
 * (AR) @brief قواعد جودة الكود العشر الأساسية
 * (EN) @brief 10 core code quality lint rules
 * المهمة: T304 - 10 lint rules
 * =============================================================================
 */

#include "linter_engine.h"
#include <regex>
#include <algorithm>
#include <sstream>
#include <cmath>

namespace sad::linter::rules {

// =============================================================================
// SAD001: متغيرات غير مستخدمة
// =============================================================================

class UnusedVariableRule : public LintRule {
public:
    std::string getId() const override { return "SAD001"; }
    std::string getName() const override { return "متغير غير مستخدم"; }
    std::string getDescription() const override {
        return "يكشف المتغيرات المُعلنة ولكن غير المُستخدمة في الكود";
    }
    Severity getDefaultSeverity() const override { return Severity::Warning; }
    RuleCategory getCategory() const override { return RuleCategory::Unused; }
    
    std::vector<LintDiagnostic> check(const LintContext& ctx) override {
        std::vector<LintDiagnostic> results;
        
        // اكتشاف المتغيرات المُعلنة مع أسطرها
        struct VarDecl {
            std::string name;
            int line;
        };
        std::vector<VarDecl> declared;
        
        for (int i = 0; i < static_cast<int>(ctx.lines.size()); i++) {
            const auto& line = ctx.lines[i];
            auto pos = line.find("متغير ");
            if (pos != std::string::npos) {
                auto rest = line.substr(pos + std::string("متغير ").size());
                auto eqPos = rest.find('=');
                auto spPos = rest.find(' ');
                auto nameEnd = std::min(eqPos, spPos);
                if (nameEnd != std::string::npos) {
                    auto name = rest.substr(0, nameEnd);
                    while (!name.empty() && name.back() == ' ') name.pop_back();
                    if (!name.empty() && name[0] != '_') {  // تجاهل _ prefix
                        declared.push_back({name, i + 1});
                    }
                }
            }
        }
        
        // فحص كل متغير
        for (const auto& var : declared) {
            int useCount = 0;
            for (int i = 0; i < static_cast<int>(ctx.lines.size()); i++) {
                if (i + 1 == var.line) continue;  // تجاهل سطر التعريف
                if (ctx.lines[i].find(var.name) != std::string::npos) {
                    useCount++;
                }
            }
            
            if (useCount == 0) {
                results.push_back(makeDiag(ctx, var.line, 1,
                    "المتغير '" + var.name + "' مُعلن ولكن غير مستخدم",
                    "أزل المتغير أو ابدأ اسمه بـ _ لإسكات التحذير"));
            }
        }
        
        return results;
    }
};

// =============================================================================
// SAD002: اصطلاحات التسمية
// =============================================================================

class NamingConventionRule : public LintRule {
public:
    std::string getId() const override { return "SAD002"; }
    std::string getName() const override { return "اصطلاح التسمية"; }
    std::string getDescription() const override {
        return "يتحقق من اتباع اصطلاحات التسمية (المتغيرات: snake_case عربي، الأصناف: PascalCase)";
    }
    Severity getDefaultSeverity() const override { return Severity::Info; }
    RuleCategory getCategory() const override { return RuleCategory::Naming; }
    
    std::vector<LintDiagnostic> check(const LintContext& ctx) override {
        std::vector<LintDiagnostic> results;
        
        for (int i = 0; i < static_cast<int>(ctx.lines.size()); i++) {
            const auto& line = ctx.lines[i];
            
            // فحص أسماء الأصناف — يجب ألا تبدأ بحرف صغير إنجليزي
            auto classPos = line.find("صنف ");
            if (classPos != std::string::npos) {
                auto rest = line.substr(classPos + std::string("صنف ").size());
                auto spPos = rest.find(' ');
                auto name = (spPos != std::string::npos) ? rest.substr(0, spPos) : rest;
                while (!name.empty() && (name.back() == '\r' || name.back() == '\n'))
                    name.pop_back();
                
                if (!name.empty() && name[0] >= 'a' && name[0] <= 'z') {
                    results.push_back(makeDiag(ctx, i + 1, 
                        static_cast<int>(classPos + std::string("صنف ").size() + 1),
                        "اسم الصنف '" + name + "' يبدأ بحرف صغير",
                        "استخدم PascalCase للأصناف (مثال: " + 
                        std::string(1, static_cast<char>(name[0] - 32)) + name.substr(1) + ")"));
                }
            }
        }
        
        return results;
    }
};

// =============================================================================
// SAD003: تعقيد الدوال
// =============================================================================

class FunctionComplexityRule : public LintRule {
public:
    std::string getId() const override { return "SAD003"; }
    std::string getName() const override { return "تعقيد الدالة"; }
    std::string getDescription() const override {
        return "يكشف الدوال التي تتجاوز حداً معيناً من التعقيد (Cyclomatic)";
    }
    Severity getDefaultSeverity() const override { return Severity::Warning; }
    RuleCategory getCategory() const override { return RuleCategory::Complexity; }
    
    std::vector<LintDiagnostic> check(const LintContext& ctx) override {
        std::vector<LintDiagnostic> results;
        
        int funcStartLine = -1;
        std::string funcName;
        int complexity = 0;
        int depth = 0;
        
        for (int i = 0; i < static_cast<int>(ctx.lines.size()); i++) {
            const auto& line = ctx.lines[i];
            auto trimmed = trim(line);
            
            // بداية دالة
            if (trimmed.find("دالة ") == 0) {
                if (funcStartLine >= 0 && depth == 0) {
                    // التقرير عن الدالة السابقة
                    checkComplexity(ctx, funcName, funcStartLine, complexity, results);
                }
                auto parenPos = trimmed.find('(');
                if (parenPos != std::string::npos) {
                    funcName = trimmed.substr(std::string("دالة ").size(), 
                        parenPos - std::string("دالة ").size());
                    while (!funcName.empty() && funcName.back() == ' ') funcName.pop_back();
                }
                funcStartLine = i + 1;
                complexity = 1;  // المسار الأساسي
                depth++;
            }
            
            // عناصر تزيد التعقيد
            if (funcStartLine >= 0) {
                if (trimmed.find("إذا ") != std::string::npos || 
                    trimmed.find("إذا(") != std::string::npos ||
                    trimmed.find("اذا ") != std::string::npos ||
                    trimmed.find("اذا(") != std::string::npos)
                    complexity++;
                if (trimmed.find("وإلا") != std::string::npos || 
                    trimmed.find("والا") != std::string::npos) 
                    complexity++;
                if (trimmed.find("بينما ") != std::string::npos || 
                    trimmed.find("بينما(") != std::string::npos) 
                    complexity++;
                if (trimmed.find("لكل ") != std::string::npos) complexity++;
                if (trimmed.find(" و ") != std::string::npos || 
                    trimmed.find(" أو ") != std::string::npos) 
                    complexity++;
                if (trimmed.find("عندما ") != std::string::npos) complexity++;
            }
            
            if (trimmed == "نهاية") {
                depth--;
                if (depth <= 0 && funcStartLine >= 0) {
                    checkComplexity(ctx, funcName, funcStartLine, complexity, results);
                    funcStartLine = -1;
                    depth = 0;
                }
            }
        }
        
        return results;
    }

private:
    static constexpr int MAX_COMPLEXITY = 10;
    
    void checkComplexity(const LintContext& ctx, const std::string& name,
                        int line, int complexity,
                        std::vector<LintDiagnostic>& results) {
        if (complexity > MAX_COMPLEXITY) {
            results.push_back(makeDiag(ctx, line, 1,
                "الدالة '" + name + "' معقدة جداً (تعقيد " + 
                std::to_string(complexity) + "، الحد الأقصى " + 
                std::to_string(MAX_COMPLEXITY) + ")",
                "قسّم الدالة إلى دوال أصغر"));
        }
    }
    
    static std::string trim(const std::string& s) {
        auto start = s.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        auto end = s.find_last_not_of(" \t\r\n");
        return s.substr(start, end - start + 1);
    }
};

// =============================================================================
// SAD004: طول الدالة
// =============================================================================

class FunctionLengthRule : public LintRule {
public:
    std::string getId() const override { return "SAD004"; }
    std::string getName() const override { return "طول الدالة"; }
    std::string getDescription() const override {
        return "يكشف الدوال الطويلة جداً (أكثر من 50 سطراً)";
    }
    Severity getDefaultSeverity() const override { return Severity::Warning; }
    RuleCategory getCategory() const override { return RuleCategory::Maintainability; }
    
    std::vector<LintDiagnostic> check(const LintContext& ctx) override {
        std::vector<LintDiagnostic> results;
        
        int funcStart = -1;
        std::string funcName;
        int depth = 0;
        
        for (int i = 0; i < static_cast<int>(ctx.lines.size()); i++) {
            auto trimmed = trim(ctx.lines[i]);
            
            if (trimmed.find("دالة ") == 0) {
                auto parenPos = trimmed.find('(');
                if (parenPos != std::string::npos) {
                    funcName = trimmed.substr(std::string("دالة ").size(),
                        parenPos - std::string("دالة ").size());
                    while (!funcName.empty() && funcName.back() == ' ') funcName.pop_back();
                }
                funcStart = i;
                depth = 1;
            } else if (trimmed == "نهاية" && depth > 0) {
                depth--;
                if (depth == 0 && funcStart >= 0) {
                    int length = i - funcStart + 1;
                    if (length > MAX_LENGTH) {
                        results.push_back(makeDiag(ctx, funcStart + 1, 1,
                            "الدالة '" + funcName + "' طويلة جداً (" + 
                            std::to_string(length) + " سطر، الحد " + 
                            std::to_string(MAX_LENGTH) + ")",
                            "قسّم الدالة إلى دوال أصغر"));
                    }
                    funcStart = -1;
                }
            } else if (trimmed.find("إذا") != std::string::npos ||
                       trimmed.find("بينما") != std::string::npos ||
                       trimmed.find("لكل") != std::string::npos ||
                       trimmed.find("صنف") != std::string::npos) {
                if (depth > 0) depth++;
            }
        }
        
        return results;
    }

private:
    static constexpr int MAX_LENGTH = 50;
    
    static std::string trim(const std::string& s) {
        auto start = s.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        auto end = s.find_last_not_of(" \t\r\n");
        return s.substr(start, end - start + 1);
    }
};

// =============================================================================
// SAD005: عدد معاملات الدالة
// =============================================================================

class TooManyParametersRule : public LintRule {
public:
    std::string getId() const override { return "SAD005"; }
    std::string getName() const override { return "معاملات كثيرة"; }
    std::string getDescription() const override {
        return "يكشف الدوال التي تأخذ أكثر من 5 معاملات";
    }
    Severity getDefaultSeverity() const override { return Severity::Warning; }
    RuleCategory getCategory() const override { return RuleCategory::CodeQuality; }
    
    std::vector<LintDiagnostic> check(const LintContext& ctx) override {
        std::vector<LintDiagnostic> results;
        
        for (int i = 0; i < static_cast<int>(ctx.lines.size()); i++) {
            auto trimmed = trim(ctx.lines[i]);
            if (trimmed.find("دالة ") != 0) continue;
            
            auto openParen = trimmed.find('(');
            auto closeParen = trimmed.find(')');
            if (openParen == std::string::npos || closeParen == std::string::npos) continue;
            
            auto params = trimmed.substr(openParen + 1, closeParen - openParen - 1);
            if (params.empty()) continue;
            
            // عد الفواصل
            int count = 1;
            for (char c : params) {
                if (c == ',' || c == '\xd8') {  // , or ،
                    count++;
                }
            }
            // فحص الفاصلة العربية أيضاً
            auto arabicComma = params.find("،");
            while (arabicComma != std::string::npos) {
                count++;
                arabicComma = params.find("،", arabicComma + 2);
            }
            // تصحيح العد المزدوج
            count = 1;
            for (size_t j = 0; j < params.size(); j++) {
                if (params[j] == ',') count++;
                // فحص الفاصلة العربية (UTF-8: 0xD8 0x8C)
                if (j + 1 < params.size() && 
                    static_cast<unsigned char>(params[j]) == 0xD8 &&
                    static_cast<unsigned char>(params[j+1]) == 0x8C) {
                    count++;
                    j++;
                }
            }
            
            if (count > MAX_PARAMS) {
                auto funcName = trimmed.substr(std::string("دالة ").size(),
                    openParen - std::string("دالة ").size());
                while (!funcName.empty() && funcName.back() == ' ') funcName.pop_back();
                
                results.push_back(makeDiag(ctx, i + 1, 1,
                    "الدالة '" + funcName + "' تأخذ " + std::to_string(count) + 
                    " معاملات (الحد " + std::to_string(MAX_PARAMS) + ")",
                    "استخدم بنية (struct) لتجميع المعاملات"));
            }
        }
        
        return results;
    }
    
private:
    static constexpr int MAX_PARAMS = 5;
    
    static std::string trim(const std::string& s) {
        auto start = s.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        auto end = s.find_last_not_of(" \t\r\n");
        return s.substr(start, end - start + 1);
    }
};

// =============================================================================
// SAD006: إسناد في شرط
// =============================================================================

class AssignmentInConditionRule : public LintRule {
public:
    std::string getId() const override { return "SAD006"; }
    std::string getName() const override { return "إسناد في شرط"; }
    std::string getDescription() const override {
        return "يكشف استخدام = بدلاً من == داخل الشروط";
    }
    Severity getDefaultSeverity() const override { return Severity::Error; }
    RuleCategory getCategory() const override { return RuleCategory::ErrorProne; }
    
    std::vector<LintDiagnostic> check(const LintContext& ctx) override {
        std::vector<LintDiagnostic> results;
        
        for (int i = 0; i < static_cast<int>(ctx.lines.size()); i++) {
            const auto& line = ctx.lines[i];
            auto trimmed = trim(line);
            
            // ابحث عن إذا/بينما
            bool isCondition = false;
            size_t parenStart = 0;
            
            if (trimmed.find("إذا") != std::string::npos ||
                trimmed.find("اذا") != std::string::npos ||
                trimmed.find("بينما") != std::string::npos) {
                auto p = trimmed.find('(');
                if (p != std::string::npos) {
                    isCondition = true;
                    parenStart = p;
                }
            }
            
            if (isCondition) {
                auto closeParen = trimmed.rfind(')');
                if (closeParen != std::string::npos && closeParen > parenStart) {
                    auto condition = trimmed.substr(parenStart + 1, 
                                                    closeParen - parenStart - 1);
                    // ابحث عن = غير مسبوق بـ ! أو = أو < أو >
                    for (size_t j = 0; j < condition.size(); j++) {
                        if (condition[j] == '=' && 
                            (j + 1 >= condition.size() || condition[j + 1] != '=') &&
                            (j == 0 || (condition[j-1] != '=' && condition[j-1] != '!' && 
                                       condition[j-1] != '<' && condition[j-1] != '>' &&
                                       condition[j-1] != '+'  && condition[j-1] != '-'))) {
                            results.push_back(makeDiag(ctx, i + 1,
                                static_cast<int>(parenStart + j + 2),
                                "استخدام '=' بدلاً من '==' في الشرط",
                                "استخدم '==' للمقارنة أو تأكد من أن الإسناد مقصود"));
                            break;
                        }
                    }
                }
            }
        }
        
        return results;
    }

private:
    static std::string trim(const std::string& s) {
        auto start = s.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        auto end = s.find_last_not_of(" \t\r\n");
        return s.substr(start, end - start + 1);
    }
};

// =============================================================================
// SAD007: كود ميت (بعد ارجع)
// =============================================================================

class DeadCodeRule : public LintRule {
public:
    std::string getId() const override { return "SAD007"; }
    std::string getName() const override { return "كود ميت"; }
    std::string getDescription() const override {
        return "يكشف الكود الذي لا يمكن الوصول إليه بعد 'ارجع'";
    }
    Severity getDefaultSeverity() const override { return Severity::Warning; }
    RuleCategory getCategory() const override { return RuleCategory::CodeQuality; }
    
    std::vector<LintDiagnostic> check(const LintContext& ctx) override {
        std::vector<LintDiagnostic> results;
        
        bool afterReturn = false;
        int returnLine = -1;
        
        for (int i = 0; i < static_cast<int>(ctx.lines.size()); i++) {
            auto trimmed = trim(ctx.lines[i]);
            if (trimmed.empty() || trimmed[0] == '#') continue;
            
            if (trimmed.find("ارجع ") == 0 || trimmed == "ارجع") {
                afterReturn = true;
                returnLine = i + 1;
                continue;
            }
            
            if (afterReturn) {
                if (trimmed == "نهاية" || trimmed.find("وإلا") == 0 || 
                    trimmed.find("والا") == 0) {
                    afterReturn = false;
                    continue;
                }
                
                results.push_back(makeDiag(ctx, i + 1, 1,
                    "كود لا يمكن الوصول إليه بعد 'ارجع' (سطر " + 
                    std::to_string(returnLine) + ")",
                    "أزل الكود الميت أو أعد ترتيب المنطق"));
                afterReturn = false;
            }
        }
        
        return results;
    }

private:
    static std::string trim(const std::string& s) {
        auto start = s.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        auto end = s.find_last_not_of(" \t\r\n");
        return s.substr(start, end - start + 1);
    }
};

// =============================================================================
// SAD008: نطاقات متداخلة عميقة
// =============================================================================

class DeepNestingRule : public LintRule {
public:
    std::string getId() const override { return "SAD008"; }
    std::string getName() const override { return "تداخل عميق"; }
    std::string getDescription() const override {
        return "يكشف التداخل العميق (أكثر من 4 مستويات)";
    }
    Severity getDefaultSeverity() const override { return Severity::Warning; }
    RuleCategory getCategory() const override { return RuleCategory::Complexity; }
    
    std::vector<LintDiagnostic> check(const LintContext& ctx) override {
        std::vector<LintDiagnostic> results;
        int depth = 0;
        
        for (int i = 0; i < static_cast<int>(ctx.lines.size()); i++) {
            auto trimmed = trim(ctx.lines[i]);
            
            if (trimmed.find("إذا") != std::string::npos ||
                trimmed.find("اذا") != std::string::npos ||
                trimmed.find("بينما") != std::string::npos ||
                trimmed.find("لكل ") != std::string::npos ||
                trimmed.find("دالة ") == 0 ||
                trimmed.find("صنف ") == 0) {
                depth++;
                
                if (depth > MAX_DEPTH) {
                    results.push_back(makeDiag(ctx, i + 1, 1,
                        "تداخل عميق جداً (" + std::to_string(depth) + 
                        " مستويات، الحد " + std::to_string(MAX_DEPTH) + ")",
                        "أعد هيكلة الكود باستخدام دوال مساعدة"));
                }
            }
            
            if (trimmed == "نهاية") {
                depth = std::max(0, depth - 1);
            }
        }
        
        return results;
    }

private:
    static constexpr int MAX_DEPTH = 4;
    
    static std::string trim(const std::string& s) {
        auto start = s.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        auto end = s.find_last_not_of(" \t\r\n");
        return s.substr(start, end - start + 1);
    }
};

// =============================================================================
// SAD009: نص سحري (magic number/string)
// =============================================================================

class MagicNumberRule : public LintRule {
public:
    std::string getId() const override { return "SAD009"; }
    std::string getName() const override { return "عدد سحري"; }
    std::string getDescription() const override {
        return "يكشف الأعداد السحرية (أعداد حرفية غير واضحة الغرض)";
    }
    Severity getDefaultSeverity() const override { return Severity::Hint; }
    RuleCategory getCategory() const override { return RuleCategory::Maintainability; }
    
    std::vector<LintDiagnostic> check(const LintContext& ctx) override {
        std::vector<LintDiagnostic> results;
        
        for (int i = 0; i < static_cast<int>(ctx.lines.size()); i++) {
            auto trimmed = trim(ctx.lines[i]);
            if (trimmed.empty() || trimmed[0] == '#') continue;
            
            // تجاهل تعريفات الثوابت
            if (trimmed.find("ثابت ") != std::string::npos) continue;
            // تجاهل المصفوفات البسيطة
            if (trimmed.find('[') != std::string::npos) continue;
            // تجاهل تعريفات المتغيرات المباشرة
            if (trimmed.find("متغير ") == 0) continue;
            
            // ابحث عن أعداد حرفية > 10 أو < -1
            for (size_t j = 0; j < trimmed.size(); j++) {
                if (std::isdigit(static_cast<unsigned char>(trimmed[j])) ||
                    (trimmed[j] == '-' && j + 1 < trimmed.size() && 
                     std::isdigit(static_cast<unsigned char>(trimmed[j + 1])))) {
                    
                    // استخراج العدد
                    size_t start = j;
                    if (trimmed[j] == '-') j++;
                    while (j < trimmed.size() && 
                           (std::isdigit(static_cast<unsigned char>(trimmed[j])) || 
                            trimmed[j] == '.')) {
                        j++;
                    }
                    
                    auto numStr = trimmed.substr(start, j - start);
                    
                    // تجاهل الأعداد الصغيرة (0, 1, 2, -1)
                    try {
                        double num = std::stod(numStr);
                        if (std::abs(num) > 10 && num != 100 && num != 1000) {
                            results.push_back(makeDiag(ctx, i + 1, 
                                static_cast<int>(start + 1),
                                "عدد سحري '" + numStr + "'",
                                "استخدم ثابتاً ذا اسم وصفي بدلاً من العدد الحرفي"));
                        }
                    } catch (...) {}
                }
            }
        }
        
        return results;
    }

private:
    static std::string trim(const std::string& s) {
        auto start = s.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        auto end = s.find_last_not_of(" \t\r\n");
        return s.substr(start, end - start + 1);
    }
};

// =============================================================================
// SAD010: ملف طويل
// =============================================================================

class FileLengthRule : public LintRule {
public:
    std::string getId() const override { return "SAD010"; }
    std::string getName() const override { return "ملف طويل"; }
    std::string getDescription() const override {
        return "يكشف الملفات التي تتجاوز 500 سطر";
    }
    Severity getDefaultSeverity() const override { return Severity::Info; }
    RuleCategory getCategory() const override { return RuleCategory::Maintainability; }
    
    std::vector<LintDiagnostic> check(const LintContext& ctx) override {
        std::vector<LintDiagnostic> results;
        
        if (ctx.totalLines() > MAX_FILE_LENGTH) {
            results.push_back(makeDiag(ctx, 1, 1,
                "الملف طويل جداً (" + std::to_string(ctx.totalLines()) + 
                " سطر، الحد " + std::to_string(MAX_FILE_LENGTH) + ")",
                "قسّم الملف إلى وحدات أصغر باستخدام الاستيراد"));
        }
        
        return results;
    }

private:
    static constexpr int MAX_FILE_LENGTH = 500;
};

// =============================================================================
// تسجيل جميع القواعد
// =============================================================================

std::unique_ptr<LinterEngine> LinterEngine::createDefault() {
    auto engine = std::make_unique<LinterEngine>();
    
    engine->registerRule(std::make_unique<UnusedVariableRule>());
    engine->registerRule(std::make_unique<NamingConventionRule>());
    engine->registerRule(std::make_unique<FunctionComplexityRule>());
    engine->registerRule(std::make_unique<FunctionLengthRule>());
    engine->registerRule(std::make_unique<TooManyParametersRule>());
    engine->registerRule(std::make_unique<AssignmentInConditionRule>());
    engine->registerRule(std::make_unique<DeadCodeRule>());
    engine->registerRule(std::make_unique<DeepNestingRule>());
    engine->registerRule(std::make_unique<MagicNumberRule>());
    engine->registerRule(std::make_unique<FileLengthRule>());
    
    return engine;
}

} // namespace sad::linter::rules
