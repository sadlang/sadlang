/**
 * ╔══════════════════════════════════════════════════════════════════════════════╗
 * ║                       لغة سعد - مترجم عربي الأول                              ║
 * ║                   Sad Language - First Arabic Compiler                        ║
 * ╠══════════════════════════════════════════════════════════════════════════════╣
 * ║                                                                              ║
 * ║   ملف: teacher_mode.cpp                                                     ║
 * ║   الوصف: وضع المعلم - شروحات تعليمية مفصلة                                  ║
 * ║   المهمة: T167 - Phase 15 (US13)                                            ║
 * ║   المؤلف: فريق سعد | Sad Team                                                ║
 * ╚══════════════════════════════════════════════════════════════════════════════╝
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                         دليل المبتدئين | Beginner's Guide
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * 🌟 ما هو وضع المعلم؟ | What is Teacher Mode?
 * ─────────────────────────────────────────────────────
 *
 * وضع خاص للمبتدئين يُقدم شروحات مفصلة وتعليمية!
 *
 * 🔹 الوضع العادي:
 *    ❌ خطأ: نوع غير متوافق (سطر 5)
 *
 * 🔹 وضع المعلم:
 *    ❌ خطأ: نوع غير متوافق (سطر 5)
 *
 *    📚 شرح المفهوم:
 *    ─────────────────
 *    في البرمجة، كل قيمة لها "نوع" يحدد ما يمكن فعله بها.
 *    مثال من الحياة: لا يمكنك صب الماء في كيس ورقي!
 *    
 *    🔍 ماذا حدث:
 *    ─────────────
 *    حاولت وضع قيمة من نوع "نص" في متغير من نوع "عدد"
 *    
 *    💡 كيف تتجنب هذا:
 *    ─────────────────
 *    1. تحقق من نوع المتغير عند التعريف
 *    2. استخدم دوال التحويل عند الحاجة
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <sstream>
#include <functional>

namespace sad {
namespace errors {

// ═══════════════════════════════════════════════════════════════════════════════
//                        مستويات الشرح | Explanation Levels
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📊 ExplanationLevel - مستوى التفصيل
 */
enum class ExplanationLevel {
    /**
     * 🟢 BRIEF - موجز
     * للمبرمجين ذوي الخبرة
     * رسالة الخطأ فقط
     */
    BRIEF,
    
    /**
     * 🟡 NORMAL - عادي
     * للمبرمجين المتوسطين
     * رسالة + اقتراحات
     */
    NORMAL,
    
    /**
     * 🔵 DETAILED - مفصل
     * للمبتدئين
     * رسالة + شرح + أمثلة
     */
    DETAILED,
    
    /**
     * 🟣 TEACHER - معلم
     * للمتعلمين الجدد
     * كل شيء + تشبيهات من الحياة
     */
    TEACHER
};

// ═══════════════════════════════════════════════════════════════════════════════
//                        محتوى تعليمي | Educational Content
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📚 ConceptExplanation - شرح مفهوم
 */
struct ConceptExplanation {
    std::string conceptAr;       // اسم المفهوم
    std::string conceptEn;      // اسم بالإنجليزية
    std::string simpleExplanation;  // شرح بسيط
    std::string analogy;        // تشبيه من الحياة
    std::string codeExample;    // مثال كود
    std::vector<std::string> keyPoints;  // نقاط مهمة
    std::vector<std::string> commonMistakes;  // أخطاء شائعة
    std::string furtherReading;  // للقراءة المزيد
    
    std::string format(bool useArabic = true) const {
        std::ostringstream oss;
        
        oss << "\n📚 " << (useArabic ? conceptAr : conceptEn) << "\n";
        oss << "   ════════════════════════════════════════════════════════\n";
        
        if (!simpleExplanation.empty()) {
            oss << "\n   📖 الشرح | Explanation:\n";
            oss << "   ────────────────────────────────────────────────────\n";
            oss << "   " << simpleExplanation << "\n";
        }
        
        if (!analogy.empty()) {
            oss << "\n   🎯 تشبيه من الحياة | Real-life Analogy:\n";
            oss << "   ────────────────────────────────────────────────────\n";
            oss << "   " << analogy << "\n";
        }
        
        if (!codeExample.empty()) {
            oss << "\n   💻 مثال | Example:\n";
            oss << "   ────────────────────────────────────────────────────\n";
            std::istringstream stream(codeExample);
            std::string line;
            while (std::getline(stream, line)) {
                oss << "   " << line << "\n";
            }
        }
        
        if (!keyPoints.empty()) {
            oss << "\n   ⭐ نقاط مهمة | Key Points:\n";
            oss << "   ────────────────────────────────────────────────────\n";
            for (const auto& point : keyPoints) {
                oss << "   • " << point << "\n";
            }
        }
        
        if (!commonMistakes.empty()) {
            oss << "\n   ⚠️ أخطاء شائعة | Common Mistakes:\n";
            oss << "   ────────────────────────────────────────────────────\n";
            for (const auto& mistake : commonMistakes) {
                oss << "   ✗ " << mistake << "\n";
            }
        }
        
        return oss.str();
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                   قاعدة بيانات المفاهيم | Concepts Database
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📚 ConceptsDatabase - قاعدة بيانات المفاهيم
 */
class ConceptsDatabase {
public:
    ConceptsDatabase() {
        initializeConcepts();
    }
    
    /**
     * 🔍 الحصول على شرح مفهوم
     */
    const ConceptExplanation* getConcept(const std::string& key) const {
        auto it = concepts_.find(key);
        return it != concepts_.end() ? &it->second : nullptr;
    }
    
private:
    std::unordered_map<std::string, ConceptExplanation> concepts_;
    
    void initializeConcepts() {
        // ═══════════════════════════════════════════════════════════════
        //                          مفهوم: الأنواع
        // ═══════════════════════════════════════════════════════════════
        {
            ConceptExplanation entry;
            entry.conceptAr = "الأنواع (Types)";
            entry.conceptEn = "Types";
            
            entry.simpleExplanation = 
                "في البرمجة، كل قيمة لها نوع يحدد طبيعتها.\n"
                "مثل: الأرقام لها نوع 'عدد'، والكلمات لها نوع 'نص'.";
            
            entry.analogy = 
                "🏠 تخيل أنواع البيانات كأنواع الحاويات:\n"
                "   📦 صندوق للأرقام: يحتوي 1, 2, 3...\n"
                "   📦 صندوق للكلمات: يحتوي \"مرحبا\", \"أهلاً\"...\n"
                "   📦 صندوق للقرارات: يحتوي صحيح أو خطأ\n\n"
                "لا يمكنك وضع تفاحة في صندوق الأرقام!\n"
                "كذلك، لا يمكنك وضع نص في متغير من نوع عدد.";
            
            entry.codeExample = 
                "// أنواع مختلفة في سعد:\n"
                "متغير عمري: عدد = 25          // عدد صحيح\n"
                "متغير اسمي: نص = \"أحمد\"       // نص\n"
                "متغير متزوج: منطقي = صحيح    // منطقي\n"
                "متغير درجات: قائمة = [90, 85, 95]  // قائمة";
            
            entry.keyPoints = {
                "كل متغير له نوع محدد",
                "الأنواع تساعد على اكتشاف الأخطاء مبكراً",
                "يمكن تحويل بين الأنواع باستخدام دوال التحويل",
                "اختر النوع المناسب لطبيعة البيانات"
            };
            
            entry.commonMistakes = {
                "جمع نص مع عدد مباشرة: \"العمر: \" + 25 ❌",
                "مقارنة نص برقم: \"5\" == 5 ⚠️",
                "نسيان التحويل: عدد(\"123\") ✅"
            };
            
            concepts_["types"] = entry;
        }
        
        // ═══════════════════════════════════════════════════════════════
        //                          مفهوم: المتغيرات
        // ═══════════════════════════════════════════════════════════════
        {
            ConceptExplanation entry;
            entry.conceptAr = "المتغيرات (Variables)";
            entry.conceptEn = "Variables";
            
            entry.simpleExplanation = 
                "المتغير هو اسم نعطيه لمكان في الذاكرة لتخزين قيمة.\n"
                "يمكننا تغيير قيمته لاحقاً (لذلك يسمى 'متغير').";
            
            entry.analogy = 
                "🏷️ تخيل المتغير كصندوق مُسمّى:\n"
                "   \n"
                "   ┌─────────┐\n"
                "   │   25    │  ← القيمة\n"
                "   └─────────┘\n"
                "     عمري    ← الاسم\n"
                "   \n"
                "يمكنك:\n"
                "   • قراءة ما بداخله: اطبع(عمري) → 25\n"
                "   • تغيير محتواه: عمري = 26\n"
                "   • استخدامه في حسابات: عمري + 5 → 30";
            
            entry.codeExample = 
                "// تعريف متغير:\n"
                "متغير اسم = \"أحمد\"\n"
                "\n"
                "// قراءة قيمته:\n"
                "اطبع(اسم)  // أحمد\n"
                "\n"
                "// تغيير قيمته:\n"
                "اسم = \"محمد\"\n"
                "اطبع(اسم)  // محمد\n"
                "\n"
                "// ثابت (لا يتغير):\n"
                "ثابت PI = 3.14159";
            
            entry.keyPoints = {
                "استخدم 'متغير' للقيم التي قد تتغير",
                "استخدم 'ثابت' للقيم الثابتة",
                "اختر أسماء واضحة ومعبرة",
                "عرّف المتغير قبل استخدامه"
            };
            
            entry.commonMistakes = {
                "استخدام متغير قبل تعريفه",
                "أسماء غير واضحة مثل: س، ص، ع",
                "نسيان كلمة 'متغير' في التعريف"
            };
            
            concepts_["variables"] = entry;
        }
        
        // ═══════════════════════════════════════════════════════════════
        //                          مفهوم: الدوال
        // ═══════════════════════════════════════════════════════════════
        {
            ConceptExplanation entry;
            entry.conceptAr = "الدوال (Functions)";
            entry.conceptEn = "Functions";
            
            entry.simpleExplanation = 
                "الدالة هي مجموعة أوامر مُسماة يمكن استدعاؤها.\n"
                "تأخذ مدخلات (معاملات) وقد تُرجع مخرجات (نتيجة).";
            
            entry.analogy = 
                "🏭 تخيل الدالة كآلة في مصنع:\n"
                "   \n"
                "        المدخلات\n"
                "           ↓\n"
                "   ┌───────────────┐\n"
                "   │  الدالة       │\n"
                "   │  (المعالجة)   │\n"
                "   └───────────────┘\n"
                "           ↓\n"
                "       المخرجات\n"
                "   \n"
                "مثال: آلة العصير\n"
                "   • المدخلات: برتقال 🍊\n"
                "   • المعالجة: عصر\n"
                "   • المخرجات: عصير برتقال 🥤";
            
            entry.codeExample = 
                "// تعريف دالة بسيطة:\n"
                "دالة تحية(اسم):\n"
                "    أرجع \"مرحباً يا \" + اسم\n"
                "\n"
                "// استدعاء الدالة:\n"
                "متغير رسالة = تحية(\"أحمد\")\n"
                "اطبع(رسالة)  // مرحباً يا أحمد\n"
                "\n"
                "// دالة بعدة معاملات:\n"
                "دالة اجمع(أ، ب):\n"
                "    أرجع أ + ب\n"
                "\n"
                "اطبع(اجمع(5, 3))  // 8";
            
            entry.keyPoints = {
                "الدالة تنظم الكود وتمنع التكرار",
                "اختر اسماً يصف ما تفعله الدالة",
                "استخدم 'أرجع' لإرجاع قيمة",
                "المعاملات اختيارية"
            };
            
            entry.commonMistakes = {
                "نسيان كلمة 'أرجع' في نهاية الدالة",
                "عدد المعاملات غير صحيح عند الاستدعاء",
                "استدعاء دالة قبل تعريفها"
            };
            
            concepts_["functions"] = entry;
        }
        
        // ═══════════════════════════════════════════════════════════════
        //                          مفهوم: الشروط
        // ═══════════════════════════════════════════════════════════════
        {
            ConceptExplanation entry;
            entry.conceptAr = "الشروط (Conditions)";
            entry.conceptEn = "Conditions";
            
            entry.simpleExplanation = 
                "الشروط تسمح للبرنامج باتخاذ قرارات.\n"
                "إذا تحقق الشرط، نفّذ كوداً معيناً.";
            
            entry.analogy = 
                "🚦 تخيل الشرط كإشارة المرور:\n"
                "   \n"
                "   إذا اللون == أخضر:\n"
                "       سِر!\n"
                "   وإلا إذا اللون == أصفر:\n"
                "       استعد للتوقف\n"
                "   وإلا:\n"
                "       توقف!\n"
                "   \n"
                "البرنامج يختار مساراً واحداً فقط حسب الشرط.";
            
            entry.codeExample = 
                "متغير عمر = 20\n"
                "\n"
                "// شرط بسيط:\n"
                "إذا عمر >= 18:\n"
                "    اطبع(\"أنت بالغ\")\n"
                "\n"
                "// شرط مع بديل:\n"
                "إذا عمر >= 18:\n"
                "    اطبع(\"يمكنك القيادة\")\n"
                "وإلا:\n"
                "    اطبع(\"انتظر حتى تبلغ 18\")\n"
                "\n"
                "// شروط متعددة:\n"
                "إذا عمر < 13:\n"
                "    اطبع(\"طفل\")\n"
                "وإلا إذا عمر < 20:\n"
                "    اطبع(\"مراهق\")\n"
                "وإلا:\n"
                "    اطبع(\"بالغ\")";
            
            entry.keyPoints = {
                "الشرط يُنتج قيمة منطقية (صحيح/خطأ)",
                "استخدم ':' بعد الشرط",
                "المسافات البادئة مهمة!",
                "'وإلا' اختياري"
            };
            
            entry.commonMistakes = {
                "نسيان النقطتين بعد الشرط",
                "مسافات بادئة غير متسقة",
                "استخدام = بدلاً من == للمقارنة"
            };
            
            concepts_["conditions"] = entry;
        }
        
        // ═══════════════════════════════════════════════════════════════
        //                          مفهوم: الحلقات
        // ═══════════════════════════════════════════════════════════════
        {
            ConceptExplanation entry;
            entry.conceptAr = "الحلقات (Loops)";
            entry.conceptEn = "Loops";
            
            entry.simpleExplanation = 
                "الحلقة تكرر كوداً معيناً عدة مرات.\n"
                "مفيدة للتعامل مع قوائم أو تكرار عملية.";
            
            entry.analogy = 
                "🔄 تخيل الحلقة كجولة في الحديقة:\n"
                "   \n"
                "   لكل شجرة في الحديقة:\n"
                "       اسقِ الشجرة\n"
                "   \n"
                "   ┌─────────────────────────┐\n"
                "   │ 🌳  🌳  🌳  🌳  🌳 │\n"
                "   │  ↑   ↑   ↑   ↑   ↑  │\n"
                "   │ 💧 💧 💧 💧 💧 │\n"
                "   └─────────────────────────┘\n"
                "   \n"
                "   تسقي كل شجرة واحدة تلو الأخرى.";
            
            entry.codeExample = 
                "// حلقة على قائمة:\n"
                "متغير فواكه = [\"تفاح\", \"برتقال\", \"موز\"]\n"
                "\n"
                "لكل فاكهة في فواكه:\n"
                "    اطبع(\"أحب \" + فاكهة)\n"
                "// أحب تفاح\n"
                "// أحب برتقال\n"
                "// أحب موز\n"
                "\n"
                "// حلقة بعداد:\n"
                "لكل ع في نطاق(1، 6):\n"
                "    اطبع(ع)\n"
                "// 1, 2, 3, 4, 5\n"
                "\n"
                "// حلقة طالما:\n"
                "متغير عداد = 0\n"
                "طالما عداد < 3:\n"
                "    اطبع(عداد)\n"
                "    عداد = عداد + 1";
            
            entry.keyPoints = {
                "'لكل' للتكرار على مجموعة",
                "'طالما' للتكرار بشرط",
                "تجنب الحلقات اللانهائية!",
                "استخدم 'توقف' للخروج مبكراً"
            };
            
            entry.commonMistakes = {
                "نسيان زيادة العداد (حلقة لانهائية)",
                "شرط لا يتحقق أبداً",
                "تعديل القائمة أثناء التكرار عليها"
            };
            
            concepts_["loops"] = entry;
        }
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                       وضع المعلم | Teacher Mode
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🎓 TeacherMode - نظام وضع المعلم
 */
class TeacherMode {
public:
    TeacherMode()
        : level_(ExplanationLevel::NORMAL)
        , db_()
    {}
    
    /**
     * ⚙️ تعيين مستوى الشرح
     */
    void setLevel(ExplanationLevel level) {
        level_ = level;
    }
    
    /**
     * 📊 الحصول على المستوى الحالي
     */
    ExplanationLevel getLevel() const {
        return level_;
    }
    
    /**
     * 📝 توليد شرح لخطأ
     */
    std::string explainError(
        const std::string& errorCode,
        const std::string& errorMessage,
        const std::string& context,
        const std::unordered_map<std::string, std::string>& details
    ) {
        std::ostringstream explanation;
        
        // الرسالة الأساسية (كل المستويات)
        explanation << "❌ " << errorMessage << "\n";
        
        if (level_ == ExplanationLevel::BRIEF) {
            return explanation.str();
        }
        
        // شرح عادي ومفصل
        explanation << "\n";
        explanation << generateBasicExplanation(errorCode, details);
        
        if (level_ == ExplanationLevel::NORMAL) {
            return explanation.str();
        }
        
        // شرح مفصل ومعلم
        explanation << "\n";
        explanation << generateDetailedExplanation(errorCode, context, details);
        
        if (level_ == ExplanationLevel::DETAILED) {
            return explanation.str();
        }
        
        // وضع المعلم - شرح كامل
        explanation << "\n";
        explanation << generateTeacherExplanation(errorCode, context, details);
        
        return explanation.str();
    }
    
    /**
     * 📚 الحصول على شرح مفهوم
     */
    std::string getConceptExplanation(const std::string& conceptKey) {
        const ConceptExplanation* concept = db_.getConcept(conceptKey);
        if (concept) {
            return concept->format(true);
        }
        return "";
    }
    
private:
    ExplanationLevel level_;
    ConceptsDatabase db_;
    
    std::string generateBasicExplanation(
        const std::string& errorCode,
        const std::unordered_map<std::string, std::string>& details
    ) {
        std::ostringstream oss;
        
        // شرح حسب رمز الخطأ
        if (errorCode.starts_with("T")) {
            oss << "🔍 هذا خطأ في الأنواع.\n";
            oss << "   القيمة المستخدمة لا تتوافق مع النوع المتوقع.\n";
        }
        else if (errorCode.starts_with("S")) {
            oss << "🔍 هذا خطأ دلالي.\n";
            oss << "   هناك مشكلة في معنى الكود، ليس في كتابته.\n";
        }
        else if (errorCode.starts_with("P")) {
            oss << "🔍 هذا خطأ نحوي.\n";
            oss << "   هناك مشكلة في طريقة كتابة الكود.\n";
        }
        
        return oss.str();
    }
    
    std::string generateDetailedExplanation(
        const std::string& errorCode,
        const std::string& context,
        const std::unordered_map<std::string, std::string>& details
    ) {
        std::ostringstream oss;
        
        oss << "📖 شرح مفصل:\n";
        oss << "   ────────────────────────────────────────────────────\n";
        
        // شرح حسب نوع الخطأ
        if (details.count("expected") && details.count("actual")) {
            oss << "   كان متوقعاً: " << details.at("expected") << "\n";
            oss << "   لكن وجدت: " << details.at("actual") << "\n";
        }
        
        if (!context.empty()) {
            oss << "\n   السياق:\n";
            oss << "   " << context << "\n";
        }
        
        return oss.str();
    }
    
    std::string generateTeacherExplanation(
        const std::string& errorCode,
        const std::string& context,
        const std::unordered_map<std::string, std::string>& details
    ) {
        std::ostringstream oss;
        
        // تحديد المفهوم المرتبط
        std::string relatedConcept;
        if (errorCode.starts_with("T")) {
            relatedConcept = "types";
        }
        else if (errorCode == "S001") {
            relatedConcept = "variables";
        }
        else if (errorCode == "S002") {
            relatedConcept = "functions";
        }
        
        // إضافة شرح المفهوم
        if (!relatedConcept.empty()) {
            const ConceptExplanation* concept = db_.getConcept(relatedConcept);
            if (concept) {
                oss << concept->format(true);
            }
        }
        
        // نصائح إضافية
        oss << "\n";
        oss << "💪 نصيحة للمبتدئين:\n";
        oss << "   ────────────────────────────────────────────────────\n";
        oss << "   • لا تقلق! الأخطاء جزء طبيعي من التعلم\n";
        oss << "   • اقرأ رسالة الخطأ بعناية\n";
        oss << "   • جرب الاقتراحات المقدمة\n";
        oss << "   • إذا احتجت مساعدة، اسأل!\n";
        
        return oss.str();
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                        واجهة خارجية | External Interface
// ═══════════════════════════════════════════════════════════════════════════════

static std::unique_ptr<TeacherMode> globalTeacher;

extern "C" {
    void sad_teacher_mode_init() {
        if (!globalTeacher) {
            globalTeacher = std::make_unique<TeacherMode>();
        }
    }
    
    void sad_teacher_mode_set_level(int level) {
        if (!globalTeacher) sad_teacher_mode_init();
        globalTeacher->setLevel(static_cast<ExplanationLevel>(level));
    }
    
    int sad_teacher_mode_get_level() {
        if (!globalTeacher) sad_teacher_mode_init();
        return static_cast<int>(globalTeacher->getLevel());
    }
    
    const char* sad_teacher_mode_explain(
        const char* errorCode,
        const char* errorMessage,
        const char* context
    ) {
        if (!globalTeacher) sad_teacher_mode_init();
        
        std::unordered_map<std::string, std::string> details;
        
        static std::string result;
        result = globalTeacher->explainError(
            errorCode ? errorCode : "",
            errorMessage ? errorMessage : "",
            context ? context : "",
            details
        );
        return result.c_str();
    }
    
    const char* sad_teacher_mode_get_concept(const char* conceptKey) {
        if (!globalTeacher) sad_teacher_mode_init();
        
        static std::string result;
        result = globalTeacher->getConceptExplanation(conceptKey ? conceptKey : "");
        return result.c_str();
    }
}

} // namespace errors
} // namespace sad
