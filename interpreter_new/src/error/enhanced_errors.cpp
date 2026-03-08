/**
 * @file enhanced_errors.cpp
 * @brief تنفيذ نظام رسائل الأخطاء المحسّنة
 * @brief (EN) Enhanced error messages implementation
 * 
 * @author أحمد
 * @date أكتوبر 2026
 */

#include "enhanced_errors.h"
#include <algorithm>
#include &lt;sstream&gt;
#include &lt;algorithm&gt;
#include &lt;iomanip&gt;
#include &lt;cmath&gt;

namespace Sad {
namespace Errors {

//==============================================================================
// EnhancedError::format
//==============================================================================

std::string EnhancedError::format(bool useColors, bool teacherMode) const {
    std::ostringstream ss;
    
    // رأس الخطأ
    std::string sevColor = useColors ? Colors::ERROR_COLOR : "";
    std::string reset = useColors ? Colors::RESET : "";
    std::string noteColor = useColors ? Colors::NOTE_COLOR : "";
    std::string suggColor = useColors ? Colors::SUGGESTION_COLOR : "";
    std::string hintColor = useColors ? Colors::HINT_COLOR : "";
    
    switch (severity) {
        case Severity::ERROR:
            sevColor = useColors ? Colors::ERROR_COLOR : "";
            ss &lt;&lt; sevColor &lt;&lt; "خطأ[" &lt;&lt; code &lt;&lt; "]: " &lt;&lt; reset;
            break;
        case Severity::WARNING:
            sevColor = useColors ? Colors::WARNING_COLOR : "";
            ss &lt;&lt; sevColor &lt;&lt; "تحذير[" &lt;&lt; code &lt;&lt; "]: " &lt;&lt; reset;
            break;
        case Severity::HINT:
            sevColor = useColors ? Colors::HINT_COLOR : "";
            ss &lt;&lt; sevColor &lt;&lt; "تلميح[" &lt;&lt; code &lt;&lt; "]: " &lt;&lt; reset;
            break;
        case Severity::INFO:
            sevColor = useColors ? Colors::CYAN : "";
            ss &lt;&lt; sevColor &lt;&lt; "معلومة: " &lt;&lt; reset;
            break;
        case Severity::FATAL:
            sevColor = useColors ? (Colors::BG_RED + Colors::WHITE) : "";
            ss &lt;&lt; sevColor &lt;&lt; "خطأ فادح[" &lt;&lt; code &lt;&lt; "]: " &lt;&lt; reset;
            break;
    }
    
    // العنوان
    ss &lt;&lt; (useColors ? Colors::BOLD : "") &lt;&lt; title &lt;&lt; reset &lt;&lt; "\n";
    
    // الموقع
    if (!location.filename.empty() &amp;&amp; location.line &gt; 0) {
        ss &lt;&lt; "   --&gt; " &lt;&lt; location.toString() &lt;&lt; "\n";
        
        // عرض السطر المصدري
        if (!location.sourceLine.empty()) {
            int lineNumWidth = std::to_string(location.line).length();
            
            ss &lt;&lt; "    " &lt;&lt; std::setw(lineNumWidth) &lt;&lt; " " &lt;&lt; " |\n";
            ss &lt;&lt; "    " &lt;&lt; location.line &lt;&lt; " | " &lt;&lt; location.sourceLine &lt;&lt; "\n";
            ss &lt;&lt; "    " &lt;&lt; std::setw(lineNumWidth) &lt;&lt; " " &lt;&lt; " | ";
            
            // مؤشر الموقع
            for (int i = 1; i &lt; location.column; ++i) {
                ss &lt;&lt; " ";
            }
            ss &lt;&lt; sevColor;
            int underlineLen = std::max(1, location.endColumn - location.column + 1);
            for (int i = 0; i &lt; underlineLen; ++i) {
                ss &lt;&lt; "^";
            }
            ss &lt;&lt; reset &lt;&lt; "\n";
        }
    }
    
    // الرسالة التفصيلية
    if (!message.empty()) {
        ss &lt;&lt; "\n" &lt;&lt; message &lt;&lt; "\n";
    }
    
    // الملاحظات
    for (const auto&amp; note : notes) {
        ss &lt;&lt; "\n" &lt;&lt; noteColor &lt;&lt; "ملاحظة: " &lt;&lt; reset &lt;&lt; note.message;
        if (note.location.has_value()) {
            ss &lt;&lt; "\n   --&gt; " &lt;&lt; note.location-&gt;toString();
        }
        ss &lt;&lt; "\n";
    }
    
    // الاقتراحات
    if (!suggestions.empty()) {
        ss &lt;&lt; "\n" &lt;&lt; suggColor &lt;&lt; "اقتراحات الإصلاح:" &lt;&lt; reset &lt;&lt; "\n";
        for (size_t i = 0; i &lt; suggestions.size(); ++i) {
            const auto&amp; sug = suggestions[i];
            ss &lt;&lt; "  " &lt;&lt; (i + 1) &lt;&lt; ". " &lt;&lt; sug.description;
            if (!sug.replacement.empty()) {
                ss &lt;&lt; "\n     " &lt;&lt; hintColor &lt;&lt; "استبدل بـ: " &lt;&lt; reset 
                   &lt;&lt; "`" &lt;&lt; sug.replacement &lt;&lt; "`";
            }
            ss &lt;&lt; "\n";
        }
    }
    
    // شرح وضع المعلم
    if (teacherMode &amp;&amp; !explanation.empty()) {
        ss &lt;&lt; "\n" &lt;&lt; (useColors ? Colors::MAGENTA : "") 
           &lt;&lt; "━━━ شرح (وضع المعلم) ━━━" &lt;&lt; reset &lt;&lt; "\n";
        ss &lt;&lt; explanation &lt;&lt; "\n";
    }
    
    // مراجع
    if (!seeAlso.empty()) {
        ss &lt;&lt; "\n" &lt;&lt; hintColor &lt;&lt; "انظر أيضاً:" &lt;&lt; reset &lt;&lt; "\n";
        for (const auto&amp; ref : seeAlso) {
            ss &lt;&lt; "  • " &lt;&lt; ref &lt;&lt; "\n";
        }
    }
    
    return ss.str();
}

std::string EnhancedError::formatJson() const {
    std::ostringstream ss;
    
    ss &lt;&lt; "{\n";
    ss &lt;&lt; "  \"code\": \"" &lt;&lt; code &lt;&lt; "\",\n";
    ss &lt;&lt; "  \"severity\": \"" &lt;&lt; static_cast&lt;int&gt;(severity) &lt;&lt; "\",\n";
    ss &lt;&lt; "  \"title\": \"" &lt;&lt; title &lt;&lt; "\",\n";
    ss &lt;&lt; "  \"message\": \"" &lt;&lt; message &lt;&lt; "\",\n";
    ss &lt;&lt; "  \"location\": {\n";
    ss &lt;&lt; "    \"file\": \"" &lt;&lt; location.filename &lt;&lt; "\",\n";
    ss &lt;&lt; "    \"line\": " &lt;&lt; location.line &lt;&lt; ",\n";
    ss &lt;&lt; "    \"column\": " &lt;&lt; location.column &lt;&lt; "\n";
    ss &lt;&lt; "  },\n";
    ss &lt;&lt; "  \"suggestions\": [\n";
    for (size_t i = 0; i &lt; suggestions.size(); ++i) {
        ss &lt;&lt; "    {\"description\": \"" &lt;&lt; suggestions[i].description &lt;&lt; "\", ";
        ss &lt;&lt; "\"replacement\": \"" &lt;&lt; suggestions[i].replacement &lt;&lt; "\"}";
        if (i &lt; suggestions.size() - 1) ss &lt;&lt; ",";
        ss &lt;&lt; "\n";
    }
    ss &lt;&lt; "  ]\n";
    ss &lt;&lt; "}";
    
    return ss.str();
}

//==============================================================================
// ErrorFactory implementations
//==============================================================================

EnhancedError ErrorFactory::undefinedVariable(const std::string&amp; name,
                                               const ErrorLocation&amp; loc,
                                               const std::vector&lt;std::string&gt;&amp; similar) {
    EnhancedError err(
        ErrorKind::SEMANTIC_UNDEFINED_VARIABLE,
        Severity::ERROR,
        ErrorCodes::UNDEFINED_VARIABLE,
        "متغير غير معرّف: `" + name + "`",
        "لم يتم العثور على متغير باسم `" + name + "` في النطاق الحالي."
    );
    err.location = loc;
    
    // اقتراحات بناءً على أسماء مشابهة
    if (!similar.empty()) {
        for (const auto&amp; s : similar) {
            err.addSuggestion(FixSuggestion(
                "هل تقصد `" + s + "`؟",
                s,
                loc,
                0.8
            ));
        }
    }
    
    // شرح وضع المعلم
    err.setExplanation(
        "يحدث هذا الخطأ عندما تحاول استخدام متغير لم يتم تعريفه مسبقاً.\n\n"
        "لتعريف متغير، استخدم:\n"
        "  متغير " + name + " = قيمة\n\n"
        "تأكد من:\n"
        "  • كتابة اسم المتغير بشكل صحيح\n"
        "  • تعريف المتغير قبل استخدامه\n"
        "  • أن المتغير في النطاق الصحيح (داخل الدالة أو الكتلة المناسبة)"
    );
    
    err.addReference("docs/02_المتغيرات_والأنواع.md");
    
    return err;
}

EnhancedError ErrorFactory::duplicateVariable(const std::string&amp; name,
                                               const ErrorLocation&amp; loc,
                                               const ErrorLocation&amp; firstDef) {
    EnhancedError err(
        ErrorKind::SEMANTIC_DUPLICATE_DEFINITION,
        Severity::ERROR,
        ErrorCodes::DUPLICATE_DEFINITION,
        "إعادة تعريف متغير: `" + name + "`",
        "المتغير `" + name + "` معرّف مسبقاً في هذا النطاق."
    );
    err.location = loc;
    
    err.addNote(ErrorNote("التعريف الأول هنا:", firstDef));
    
    err.addSuggestion(FixSuggestion(
        "استخدم اسماً مختلفاً للمتغير الجديد",
        name + "_جديد",
        loc,
        0.7
    ));
    
    err.addSuggestion(FixSuggestion(
        "أو عدّل المتغير الموجود بدلاً من إعادة تعريفه",
        name + " = قيمة_جديدة",
        loc,
        0.9
    ));
    
    err.setExplanation(
        "لا يمكن تعريف متغيرين بنفس الاسم في نفس النطاق.\n\n"
        "الحلول الممكنة:\n"
        "  1. استخدم اسماً مختلفاً للمتغير الجديد\n"
        "  2. إذا كنت تريد تغيير قيمة المتغير، استخدم الإسناد مباشرة:\n"
        "     " + name + " = قيمة_جديدة\n"
        "  3. إذا كنت في نطاق مختلف (داخل دالة مثلاً)، يمكنك استخدام نفس الاسم"
    );
    
    return err;
}

EnhancedError ErrorFactory::undefinedFunction(const std::string&amp; name,
                                               const ErrorLocation&amp; loc,
                                               const std::vector&lt;std::string&gt;&amp; similar) {
    EnhancedError err(
        ErrorKind::SEMANTIC_UNDEFINED_FUNCTION,
        Severity::ERROR,
        ErrorCodes::UNDEFINED_FUNCTION,
        "دالة غير معرّفة: `" + name + "`",
        "لم يتم العثور على دالة باسم `" + name + "`."
    );
    err.location = loc;
    
    if (!similar.empty()) {
        for (const auto&amp; s : similar) {
            err.addSuggestion(FixSuggestion(
                "هل تقصد `" + s + "`؟",
                s,
                loc,
                0.8
            ));
        }
    }
    
    err.setExplanation(
        "يحدث هذا الخطأ عند استدعاء دالة غير موجودة.\n\n"
        "لتعريف دالة:\n"
        "  دالة " + name + "(معاملات)\n"
        "      # كود الدالة\n"
        "  نهاية\n\n"
        "تأكد من:\n"
        "  • كتابة اسم الدالة بشكل صحيح\n"
        "  • تعريف الدالة قبل استدعائها\n"
        "  • استيراد الوحدة إذا كانت الدالة في ملف آخر"
    );
    
    err.addReference("docs/04_الدوال.md");
    
    return err;
}

EnhancedError ErrorFactory::wrongArgumentCount(const std::string&amp; funcName,
                                                int expected, int actual,
                                                const ErrorLocation&amp; loc) {
    EnhancedError err(
        ErrorKind::SEMANTIC_TYPE_MISMATCH,
        Severity::ERROR,
        ErrorCodes::TYPE_MISMATCH,
        "عدد وسائط خاطئ للدالة `" + funcName + "`",
        "الدالة تتوقع " + std::to_string(expected) + " وسيط(وسائط)، "
        "لكن تم تمرير " + std::to_string(actual) + "."
    );
    err.location = loc;
    
    std::string hint = actual &lt; expected 
        ? "أضف " + std::to_string(expected - actual) + " وسيط(وسائط) ناقصة"
        : "احذف " + std::to_string(actual - expected) + " وسيط(وسائط) زائدة";
    
    err.addSuggestion(FixSuggestion(hint, "", loc, 1.0));
    
    err.setExplanation(
        "كل دالة لها عدد محدد من الوسائط (المعاملات).\n\n"
        "مثال:\n"
        "  دالة جمع(أ، ب)  # تتوقع 2 وسيط\n"
        "      ارجع أ + ب\n"
        "  نهاية\n\n"
        "  جمع(1، 2)      # ✓ صحيح\n"
        "  جمع(1)         # ✗ خطأ: وسيط ناقص\n"
        "  جمع(1، 2، 3)   # ✗ خطأ: وسيط زائد"
    );
    
    return err;
}

EnhancedError ErrorFactory::unexpectedToken(const std::string&amp; found,
                                             const std::string&amp; expected,
                                             const ErrorLocation&amp; loc) {
    EnhancedError err(
        ErrorKind::PARSER_UNEXPECTED_TOKEN,
        Severity::ERROR,
        ErrorCodes::UNEXPECTED_TOKEN,
        "رمز غير متوقع",
        "وجدت `" + found + "` بينما المتوقع `" + expected + "`."
    );
    err.location = loc;
    
    err.addSuggestion(FixSuggestion(
        "استبدل `" + found + "` بـ `" + expected + "`",
        expected,
        loc,
        0.9
    ));
    
    err.setExplanation(
        "المحلل النحوي وجد رمزاً في موقع لا يتوقعه.\n\n"
        "الأسباب الشائعة:\n"
        "  • نسيان قوس أو فاصلة\n"
        "  • خطأ إملائي في كلمة مفتاحية\n"
        "  • ترتيب خاطئ للتعابير"
    );
    
    return err;
}

EnhancedError ErrorFactory::missingEnd(const std::string&amp; blockType,
                                        const ErrorLocation&amp; blockStart,
                                        const ErrorLocation&amp; loc) {
    EnhancedError err(
        ErrorKind::PARSER_MISSING_END,
        Severity::ERROR,
        ErrorCodes::MISSING_END,
        "كلمة `نهاية` مفقودة",
        "الكتلة `" + blockType + "` لم تُغلق بكلمة `نهاية`."
    );
    err.location = loc;
    
    err.addNote(ErrorNote("الكتلة بدأت هنا:", blockStart));
    
    err.addSuggestion(FixSuggestion(
        "أضف `نهاية` لإغلاق الكتلة",
        "نهاية",
        loc,
        1.0
    ));
    
    err.setExplanation(
        "في لغة ص، كل كتلة يجب أن تنتهي بكلمة `نهاية`.\n\n"
        "أمثلة:\n"
        "  دالة اسم()    →  نهاية\n"
        "  صنف اسم       →  نهاية\n"
        "  إذا (شرط)     →  نهاية\n"
        "  بينما (شرط)   →  نهاية\n"
        "  لكل عنصر في   →  نهاية\n\n"
        "تأكد من أن كل كتلة مفتوحة لها `نهاية` مقابلة."
    );
    
    err.addReference("docs/01_مقدمة.md#بنية-الكتل");
    
    return err;
}

EnhancedError ErrorFactory::typeMismatch(const std::string&amp; expected,
                                          const std::string&amp; actual,
                                          const ErrorLocation&amp; loc) {
    EnhancedError err(
        ErrorKind::SEMANTIC_TYPE_MISMATCH,
        Severity::ERROR,
        ErrorCodes::TYPE_MISMATCH,
        "عدم تطابق الأنواع",
        "النوع المتوقع `" + expected + "`، لكن وُجد `" + actual + "`."
    );
    err.location = loc;
    
    // اقتراح تحويل النوع إذا أمكن
    if (expected == "نص" &amp;&amp; (actual == "رقم" || actual == "عشري")) {
        err.addSuggestion(FixSuggestion(
            "حوّل الرقم إلى نص باستخدام `نص()`",
            "نص(القيمة)",
            loc,
            0.9
        ));
    } else if ((expected == "رقم" || expected == "عشري") &amp;&amp; actual == "نص") {
        err.addSuggestion(FixSuggestion(
            "حوّل النص إلى رقم باستخدام `رقم()`",
            "رقم(القيمة)",
            loc,
            0.9
        ));
    }
    
    err.setExplanation(
        "لغة ص تتحقق من توافق الأنواع في بعض العمليات.\n\n"
        "الأنواع الأساسية:\n"
        "  • رقم: أعداد صحيحة (1، 2، -5)\n"
        "  • عشري: أعداد عشرية (3.14)\n"
        "  • نص: سلاسل نصية (\"مرحبا\")\n"
        "  • منطقي: صحيح أو خطأ\n\n"
        "دوال التحويل:\n"
        "  • نص(قيمة) - تحويل إلى نص\n"
        "  • رقم(قيمة) - تحويل إلى رقم\n"
        "  • عشري(قيمة) - تحويل إلى عشري"
    );
    
    return err;
}

EnhancedError ErrorFactory::divisionByZero(const ErrorLocation&amp; loc) {
    EnhancedError err(
        ErrorKind::RUNTIME_DIVISION_BY_ZERO,
        Severity::ERROR,
        ErrorCodes::DIVISION_BY_ZERO,
        "قسمة على صفر",
        "محاولة القسمة على صفر غير مسموحة."
    );
    err.location = loc;
    
    err.addSuggestion(FixSuggestion(
        "تحقق من قيمة المقسوم عليه قبل القسمة",
        "إذا (المقسوم != 0)\n    النتيجة = العدد / المقسوم\nوإلا\n    # تعامل مع الحالة\nنهاية",
        loc,
        1.0
    ));
    
    err.setExplanation(
        "القسمة على صفر غير معرّفة رياضياً.\n\n"
        "لتجنب هذا الخطأ:\n"
        "  إذا (المقسوم != 0)\n"
        "      النتيجة = العدد / المقسوم\n"
        "  وإلا\n"
        "      اطبع_سطر(\"خطأ: لا يمكن القسمة على صفر\")\n"
        "  نهاية"
    );
    
    return err;
}

EnhancedError ErrorFactory::indexOutOfBounds(int index, int size,
                                              const ErrorLocation&amp; loc) {
    EnhancedError err(
        ErrorKind::RUNTIME_INDEX_OUT_OF_BOUNDS,
        Severity::ERROR,
        ErrorCodes::INDEX_OUT_OF_BOUNDS,
        "فهرس خارج النطاق",
        "الفهرس " + std::to_string(index) + " خارج نطاق المصفوفة "
        "(الحجم: " + std::to_string(size) + ")."
    );
    err.location = loc;
    
    if (size &gt; 0) {
        err.addNote(ErrorNote(
            "الفهارس الصالحة: من 0 إلى " + std::to_string(size - 1)
        ));
    }
    
    err.addSuggestion(FixSuggestion(
        "تحقق من حدود المصفوفة قبل الوصول",
        "إذا (ف &lt; م.الطول())\n    القيمة = م[ف]\nنهاية",
        loc,
        1.0
    ));
    
    err.setExplanation(
        "المصفوفات في لغة ص تبدأ من الفهرس 0.\n\n"
        "مثال:\n"
        "  متغير م = [\"أ\"، \"ب\"، \"ج\"]\n"
        "  # الفهارس الصالحة: 0، 1، 2\n"
        "  # م[0] = \"أ\"\n"
        "  # م[2] = \"ج\"\n"
        "  # م[3] = خطأ! (خارج النطاق)\n\n"
        "للحصول على آخر عنصر:\n"
        "  م[م.الطول() - 1]"
    );
    
    return err;
}

EnhancedError ErrorFactory::nullReference(const std::string&amp; varName,
                                           const ErrorLocation&amp; loc) {
    EnhancedError err(
        ErrorKind::RUNTIME_NULL_REFERENCE,
        Severity::ERROR,
        ErrorCodes::NULL_REFERENCE,
        "مرجع فارغ (لاشيء)",
        "محاولة الوصول إلى خاصية أو طريقة من قيمة `لاشيء`."
    );
    err.location = loc;
    
    err.addNote(ErrorNote("المتغير `" + varName + "` يحتوي على `لاشيء`"));
    
    err.addSuggestion(FixSuggestion(
        "تحقق من أن المتغير ليس فارغاً أولاً",
        "إذا (" + varName + " != لاشيء)\n    # استخدم " + varName + "\nنهاية",
        loc,
        1.0
    ));
    
    err.setExplanation(
        "القيمة `لاشيء` تعني أن المتغير لا يحتوي على كائن صالح.\n\n"
        "أسباب شائعة:\n"
        "  • المتغير لم يُعيَّن له قيمة\n"
        "  • الدالة أرجعت `لاشيء`\n"
        "  • البحث في مصفوفة/خريطة لم يجد نتيجة\n\n"
        "للتحقق من القيمة:\n"
        "  إذا (" + varName + " != لاشيء)\n"
        "      # آمن للاستخدام\n"
        "  نهاية"
    );
    
    return err;
}

//==============================================================================
// ErrorFactory::levenshteinDistance
//==============================================================================

int ErrorFactory::levenshteinDistance(const std::string&amp; s1, const std::string&amp; s2) {
    const size_t m = s1.size();
    const size_t n = s2.size();
    
    if (m == 0) return n;
    if (n == 0) return m;
    
    std::vector&lt;std::vector&lt;int&gt;&gt; dp(m + 1, std::vector&lt;int&gt;(n + 1));
    
    for (size_t i = 0; i &lt;= m; ++i) dp[i][0] = i;
    for (size_t j = 0; j &lt;= n; ++j) dp[0][j] = j;
    
    for (size_t i = 1; i &lt;= m; ++i) {
        for (size_t j = 1; j &lt;= n; ++j) {
            int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            dp[i][j] = std::min({
                dp[i - 1][j] + 1,      // حذف
                dp[i][j - 1] + 1,      // إدراج
                dp[i - 1][j - 1] + cost // استبدال
            });
        }
    }
    
    return dp[m][n];
}

std::vector&lt;std::string&gt; ErrorFactory::findSimilar(const std::string&amp; name,
                                                    const std::vector&lt;std::string&gt;&amp; candidates,
                                                    int maxResults,
                                                    int maxDistance) {
    std::vector&lt;std::pair&lt;int, std::string&gt;&gt; scored;
    
    for (const auto&amp; candidate : candidates) {
        int dist = levenshteinDistance(name, candidate);
        if (dist &lt;= maxDistance) {
            scored.push_back({dist, candidate});
        }
    }
    
    std::sort(scored.begin(), scored.end());
    
    std::vector&lt;std::string&gt; result;
    for (size_t i = 0; i &lt; std::min((size_t)maxResults, scored.size()); ++i) {
        result.push_back(scored[i].second);
    }
    
    return result;
}

//==============================================================================
// ErrorCollector
//==============================================================================

void ErrorCollector::addError(const EnhancedError&amp; error) {
    errors_.push_back(error);
    hasErrors_ = true;
}

void ErrorCollector::addWarning(const EnhancedError&amp; warning) {
    warnings_.push_back(warning);
}

void ErrorCollector::addHint(const EnhancedError&amp; hint) {
    hints_.push_back(hint);
}

void ErrorCollector::printAll(std::ostream&amp; out) const {
    // طباعة الأخطاء أولاً
    for (const auto&amp; err : errors_) {
        out &lt;&lt; err.format(useColors_, teacherMode_) &lt;&lt; "\n";
    }
    
    // ثم التحذيرات
    for (const auto&amp; warn : warnings_) {
        out &lt;&lt; warn.format(useColors_, teacherMode_) &lt;&lt; "\n";
    }
    
    // وأخيراً التلميحات
    for (const auto&amp; hint : hints_) {
        out &lt;&lt; hint.format(useColors_, teacherMode_) &lt;&lt; "\n";
    }
    
    // ملخص
    if (errors_.size() &gt; 0 || warnings_.size() &gt; 0) {
        out &lt;&lt; "\n";
        if (errors_.size() &gt; 0) {
            out &lt;&lt; (useColors_ ? Colors::RED : "") 
                &lt;&lt; "إجمالي الأخطاء: " &lt;&lt; errors_.size()
                &lt;&lt; (useColors_ ? Colors::RESET : "") &lt;&lt; "\n";
        }
        if (warnings_.size() &gt; 0) {
            out &lt;&lt; (useColors_ ? Colors::YELLOW : "")
                &lt;&lt; "إجمالي التحذيرات: " &lt;&lt; warnings_.size()
                &lt;&lt; (useColors_ ? Colors::RESET : "") &lt;&lt; "\n";
        }
    }
}

void ErrorCollector::clear() {
    errors_.clear();
    warnings_.clear();
    hints_.clear();
    hasErrors_ = false;
}

std::string ErrorCollector::toJson() const {
    std::ostringstream ss;
    ss &lt;&lt; "{\n  \"errors\": [\n";
    
    for (size_t i = 0; i &lt; errors_.size(); ++i) {
        ss &lt;&lt; "    " &lt;&lt; errors_[i].formatJson();
        if (i &lt; errors_.size() - 1) ss &lt;&lt; ",";
        ss &lt;&lt; "\n";
    }
    
    ss &lt;&lt; "  ],\n  \"warnings\": [\n";
    
    for (size_t i = 0; i &lt; warnings_.size(); ++i) {
        ss &lt;&lt; "    " &lt;&lt; warnings_[i].formatJson();
        if (i &lt; warnings_.size() - 1) ss &lt;&lt; ",";
        ss &lt;&lt; "\n";
    }
    
    ss &lt;&lt; "  ]\n}";
    return ss.str();
}

} // namespace Errors
} // namespace Sad
