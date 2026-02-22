// ملف: rafiq.cpp
// الوصف: تكامل رفيق - إكمال الكود والشرح والمراجعة
// المشروع: سيادة - محرك الذكاء الاصطناعي للغة صاد
// المرحلة: 54 - الذكاء الاصطناعي

#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <functional>
#include <algorithm>

namespace sad::ai {

enum class RafiqTask { CodeCompletion, CodeExplanation, CodeReview, BugDetection, Refactoring };

struct CodeContext {
    std::string file_path;
    std::string language;       // "sad", "cpp", "python"
    std::string code_before;    // الكود قبل المؤشر
    std::string code_after;     // الكود بعد المؤشر
    int cursor_line = 0;
    int cursor_col = 0;
    std::vector<std::string> imports;
    std::vector<std::string> symbols_in_scope;
};

struct CompletionResult {
    std::string text;
    float confidence;
    std::string explanation;
    int tokens_used;
};

struct ReviewComment {
    int line;
    std::string severity;  // info, warning, error
    std::string message;
    std::string suggestion;
};

struct ReviewResult {
    std::vector<ReviewComment> comments;
    float quality_score;
    std::string summary;
};

using ModelInferenceFn = std::function<std::string(const std::string& prompt, int max_tokens)>;

class RafiqIntegration {
    ModelInferenceFn inference_fn_;
    std::unordered_map<std::string, std::string> prompt_templates_;
    int max_context_tokens_;
    bool arabic_mode_;

public:
    RafiqIntegration(int max_context = 4096, bool arabic = true)
        : max_context_tokens_(max_context), arabic_mode_(arabic) {
        init_templates();
    }

    void set_inference_fn(ModelInferenceFn fn) { inference_fn_ = std::move(fn); }

    // تهيئة قوالب الأوامر
    void init_templates() {
        prompt_templates_["completion"] =
            "أكمل الكود التالي بلغة {lang}:\n```\n{code_before}\n```\n"
            "الرموز المتاحة: {symbols}\nالإكمال:";
        prompt_templates_["explanation"] =
            "اشرح الكود التالي بلغة {lang} بالعربية:\n```\n{code}\n```\nالشرح:";
        prompt_templates_["review"] =
            "راجع الكود التالي بلغة {lang} وقدم ملاحظات:\n```\n{code}\n```\n"
            "قدم الملاحظات بصيغة: [سطر] [مستوى] [رسالة] [اقتراح]";
        prompt_templates_["bug_detect"] =
            "ابحث عن الأخطاء المحتملة في الكود التالي:\n```\n{code}\n```\nالأخطاء:";
    }

    // إكمال الكود
    CompletionResult complete(const CodeContext& ctx) {
        std::string prompt = build_prompt("completion", ctx);
        std::string result = invoke_model(prompt, 256);

        CompletionResult completion;
        completion.text = result;
        completion.confidence = estimate_confidence(result, ctx);
        completion.tokens_used = static_cast<int>(result.size() / 4);
        return completion;
    }

    // شرح الكود
    std::string explain(const std::string& code, const std::string& language) {
        CodeContext ctx;
        ctx.code_before = code;
        ctx.language = language;
        std::string prompt = build_prompt("explanation", ctx);
        return invoke_model(prompt, 512);
    }

    // مراجعة الكود
    ReviewResult review(const std::string& code, const std::string& language) {
        CodeContext ctx;
        ctx.code_before = code;
        ctx.language = language;
        std::string prompt = build_prompt("review", ctx);
        std::string raw = invoke_model(prompt, 1024);
        return parse_review(raw);
    }

    // كشف الأخطاء
    std::vector<ReviewComment> detect_bugs(const std::string& code, const std::string& language) {
        CodeContext ctx;
        ctx.code_before = code;
        ctx.language = language;
        std::string prompt = build_prompt("bug_detect", ctx);
        std::string raw = invoke_model(prompt, 512);
        auto result = parse_review(raw);
        return result.comments;
    }

private:
    // بناء الأمر من القالب والسياق
    std::string build_prompt(const std::string& task, const CodeContext& ctx) {
        std::string tmpl = prompt_templates_[task];
        replace_all(tmpl, "{lang}", ctx.language.empty() ? "sad" : ctx.language);
        replace_all(tmpl, "{code_before}", ctx.code_before);
        replace_all(tmpl, "{code}", ctx.code_before);
        std::string syms;
        for (const auto& s : ctx.symbols_in_scope) {
            if (!syms.empty()) syms += ", ";
            syms += s;
        }
        replace_all(tmpl, "{symbols}", syms);
        return tmpl;
    }

    // استدعاء النموذج
    std::string invoke_model(const std::string& prompt, int max_tokens) {
        if (inference_fn_) return inference_fn_(prompt, max_tokens);
        return "[رفيق] النموذج غير محمّل";
    }

    // تقدير الثقة في الإكمال
    float estimate_confidence(const std::string& completion, const CodeContext& ctx) {
        float score = 0.5f;
        if (!completion.empty()) score += 0.1f;
        if (completion.size() > 10) score += 0.1f;
        if (!ctx.symbols_in_scope.empty()) score += 0.1f;
        return std::min(score, 1.0f);
    }

    // تحليل نتيجة المراجعة
    ReviewResult parse_review(const std::string& raw) {
        ReviewResult result;
        result.quality_score = 0.7f;
        result.summary = raw.substr(0, std::min(raw.size(), size_t(200)));
        return result;
    }

    void replace_all(std::string& str, const std::string& from, const std::string& to) {
        size_t pos = 0;
        while ((pos = str.find(from, pos)) != std::string::npos) {
            str.replace(pos, from.length(), to);
            pos += to.length();
        }
    }
};

} // namespace sad::ai
