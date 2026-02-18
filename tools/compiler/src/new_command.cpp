/**
 * ╔══════════════════════════════════════════════════════════════════════════════╗
 * ║                       لغة سعد - مترجم عربي الأول                              ║
 * ║                   Sad Language - First Arabic Compiler                        ║
 * ╠══════════════════════════════════════════════════════════════════════════════╣
 * ║                                                                              ║
 * ║   ملف: new_command.cpp                                                      ║
 * ║   الوصف: أمر "ص جديد" لإنشاء مشاريع جديدة                                   ║
 * ║   المهمة: T171 - Phase 16 (US14)                                            ║
 * ║   المؤلف: فريق سعد | Sad Team                                                ║
 * ╚══════════════════════════════════════════════════════════════════════════════╝
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                         دليل المبتدئين | Beginner's Guide
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * 🌟 ما هو أمر "ص جديد"؟
 * ──────────────────────────────────────
 *
 * أمر لإنشاء مشروع سعد جديد بسرعة!
 *
 * 💻 الاستخدام:
 *   ص جديد مشروعي              → إنشاء مشروع باسم "مشروعي"
 *   ص جديد تطبيقي --مكتبة      → إنشاء مكتبة
 *   ص جديد لعبتي --قالب=لعبة   → إنشاء من قالب
 *
 * 📁 الهيكل الناتج:
 *   مشروعي/
 *   ├── ص.toml           ← ملف الإعدادات
 *   ├── src/
 *   │   └── main.ص     ← الملف الرئيسي
 *   └── README.md        ← التوثيق
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace fs = std::filesystem;

namespace sad {
namespace tools {

// ═══════════════════════════════════════════════════════════════════════════════
//                        خيارات الأمر | Command Options
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ⚙️ NewCommandOptions - خيارات أمر جديد
 */
struct NewCommandOptions {
    std::string projectName;    // اسم المشروع
    std::string templateName;   // اسم القالب
    bool isLibrary;             // مكتبة أم تنفيذي
    bool useGit;                // تهيئة Git
    std::string author;         // اسم المؤلف
    std::string license;        // نوع الرخصة
    bool verbose;               // وضع مفصل
    
    NewCommandOptions()
        : isLibrary(false)
        , useGit(true)
        , license("MIT")
        , verbose(false)
    {}
};

// ═══════════════════════════════════════════════════════════════════════════════
//                        قوالب المشاريع | Project Templates
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📝 ProjectTemplate - قالب مشروع
 */
struct ProjectTemplate {
    std::string nameAr;
    std::string nameEn;
    std::string description;
    std::vector<std::pair<std::string, std::string>> files;  // path -> content
};

/**
 * 📚 TemplateLibrary - مكتبة القوالب
 */
class TemplateLibrary {
public:
    TemplateLibrary() {
        initializeTemplates();
    }
    
    const ProjectTemplate* getTemplate(const std::string& name) const {
        auto it = templates_.find(name);
        return it != templates_.end() ? &it->second : nullptr;
    }
    
    std::vector<std::string> listTemplates() const {
        std::vector<std::string> names;
        for (const auto& [name, _] : templates_) {
            names.push_back(name);
        }
        return names;
    }
    
private:
    std::unordered_map<std::string, ProjectTemplate> templates_;
    
    void initializeTemplates() {
        // ═══════════════════════════════════════════════════════════════
        //                     قالب: أساسي (افتراضي)
        // ═══════════════════════════════════════════════════════════════
        {
            ProjectTemplate t;
            t.nameAr = "أساسي";
            t.nameEn = "basic";
            t.description = "مشروع سعد أساسي بسيط";
            
            t.files.push_back({"src/main.ص", R"(// ═══════════════════════════════════════════════════════════
//                    مشروع سعد الجديد
// ═══════════════════════════════════════════════════════════

// نقطة البداية
دالة رئيسية():
    اطبع("مرحباً بالعالم! 🌍")
    اطبع("أهلاً بك في لغة سعد العربية!")
    أرجع 0
)"});
            
            templates_["أساسي"] = t;
            templates_["basic"] = t;
        }
        
        // ═══════════════════════════════════════════════════════════════
        //                     قالب: مكتبة
        // ═══════════════════════════════════════════════════════════════
        {
            ProjectTemplate t;
            t.nameAr = "مكتبة";
            t.nameEn = "library";
            t.description = "مكتبة سعد قابلة للمشاركة";
            
            t.files.push_back({"src/lib.ص", R"(// ═══════════════════════════════════════════════════════════
//                       مكتبة سعد
// ═══════════════════════════════════════════════════════════

// دالة عامة يمكن استدعاؤها من مشاريع أخرى
عام دالة تحية(اسم: نص) -> نص:
    أرجع "مرحباً يا " + اسم + "!"

// دالة مساعدة داخلية
دالة _تنسيق(نص_: نص) -> نص:
    أرجع "[" + نص_ + "]"

// ثوابت المكتبة
عام ثابت النسخة = "0.1.0"
عام ثابت الاسم = "مكتبتي"
)"});
            
            t.files.push_back({"tests/test_lib.ص", R"(// اختبارات المكتبة
استورد "../src/lib" كـ مكتبة

دالة اختبار_تحية():
    متغير نتيجة = مكتبة.تحية("أحمد")
    تأكد(نتيجة == "مرحباً يا أحمد!")
    اطبع("✅ اختبار التحية نجح")

دالة رئيسية():
    اختبار_تحية()
    اطبع("كل الاختبارات نجحت! 🎉")
)"});
            
            templates_["مكتبة"] = t;
            templates_["library"] = t;
        }
        
        // ═══════════════════════════════════════════════════════════════
        //                     قالب: واجهة سطر أوامر
        // ═══════════════════════════════════════════════════════════════
        {
            ProjectTemplate t;
            t.nameAr = "سطر_أوامر";
            t.nameEn = "cli";
            t.description = "تطبيق سطر أوامر";
            
            t.files.push_back({"src/main.ص", R"(// ═══════════════════════════════════════════════════════════
//                  تطبيق سطر الأوامر
// ═══════════════════════════════════════════════════════════

استورد "نظام" كـ نظام

دالة عرض_المساعدة():
    اطبع("الاستخدام: برنامجي [خيارات]")
    اطبع("")
    اطبع("الخيارات:")
    اطبع("  --مساعدة, -م    عرض هذه الرسالة")
    اطبع("  --نسخة, -ن      عرض النسخة")

دالة رئيسية():
    متغير الوسائط = نظام.الوسائط()
    
    لكل وسيط في الوسائط:
        إذا وسيط == "--مساعدة" أو وسيط == "-م":
            عرض_المساعدة()
            أرجع 0
        وإلا إذا وسيط == "--نسخة" أو وسيط == "-ن":
            اطبع("النسخة 0.1.0")
            أرجع 0
    
    اطبع("مرحباً! استخدم --مساعدة للمزيد.")
    أرجع 0
)"});
            
            templates_["سطر_أوامر"] = t;
            templates_["cli"] = t;
        }
        
        // ═══════════════════════════════════════════════════════════════
        //                     قالب: لعبة بسيطة
        // ═══════════════════════════════════════════════════════════════
        {
            ProjectTemplate t;
            t.nameAr = "لعبة";
            t.nameEn = "game";
            t.description = "لعبة بسيطة بمكتبة الرسوم";
            
            t.files.push_back({"src/main.ص", R"(// ═══════════════════════════════════════════════════════════
//                       لعبة سعد
// ═══════════════════════════════════════════════════════════

استورد "رسوم" كـ ر

// إعدادات اللعبة
ثابت العرض = 800
ثابت الارتفاع = 600
ثابت العنوان = "لعبتي الأولى"

// حالة اللعبة
هيكل اللاعب:
    س: عدد
    ص: عدد
    السرعة: عدد

دالة رئيسية():
    // تهيئة النافذة
    ر.أنشئ_نافذة(العرض, الارتفاع, العنوان)
    
    متغير لاعب = اللاعب{س: 400, ص: 300, السرعة: 5}
    
    // حلقة اللعبة
    طالما ر.النافذة_مفتوحة():
        // معالجة المدخلات
        إذا ر.مضغوط("يمين"):
            لاعب.س = لاعب.س + لاعب.السرعة
        إذا ر.مضغوط("يسار"):
            لاعب.س = لاعب.س - لاعب.السرعة
        إذا ر.مضغوط("فوق"):
            لاعب.ص = لاعب.ص - لاعب.السرعة
        إذا ر.مضغوط("تحت"):
            لاعب.ص = لاعب.ص + لاعب.السرعة
        
        // الرسم
        ر.امسح(ر.أسود)
        ر.ارسم_مربع(لاعب.س, لاعب.ص, 50, 50, ر.أخضر)
        ر.اعرض()
    
    ر.أغلق()
    أرجع 0
)"});
            
            templates_["لعبة"] = t;
            templates_["game"] = t;
        }
        
        // ═══════════════════════════════════════════════════════════════
        //                     قالب: خادم ويب
        // ═══════════════════════════════════════════════════════════════
        {
            ProjectTemplate t;
            t.nameAr = "خادم_ويب";
            t.nameEn = "webserver";
            t.description = "خادم ويب HTTP بسيط";
            
            t.files.push_back({"src/main.ص", R"(// ═══════════════════════════════════════════════════════════
//                      خادم ويب سعد
// ═══════════════════════════════════════════════════════════

استورد "شبكة" كـ ش

ثابت المنفذ = 8080

// معالج الطلبات
دالة معالج(طلب: ش.طلب) -> ش.رد:
    إذا طلب.المسار == "/":
        أرجع ش.رد{
            الحالة: 200,
            المحتوى: "<h1>مرحباً بالعالم!</h1><p>خادم سعد يعمل</p>",
            النوع: "text/html; charset=utf-8"
        }
    وإلا إذا طلب.المسار == "/api/سلام":
        أرجع ش.رد{
            الحالة: 200,
            المحتوى: "{\"رسالة\": \"أهلاً وسهلاً!\"}",
            النوع: "application/json"
        }
    وإلا:
        أرجع ش.رد{
            الحالة: 404,
            المحتوى: "الصفحة غير موجودة",
            النوع: "text/plain"
        }

دالة رئيسية():
    اطبع("🚀 بدء الخادم على المنفذ " + نص(المنفذ))
    اطبع("   افتح: http://localhost:" + نص(المنفذ))
    
    متغير خادم = ش.خادم_جديد(المنفذ)
    خادم.عند_طلب(معالج)
    خادم.ابدأ()
    
    أرجع 0
)"});
            
            templates_["خادم_ويب"] = t;
            templates_["webserver"] = t;
        }
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                        منفذ الأمر | Command Executor
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🚀 NewCommandExecutor - منفذ أمر جديد
 */
class NewCommandExecutor {
public:
    NewCommandExecutor() : templates_() {}
    
    /**
     * 🎯 تنفيذ الأمر
     */
    int execute(const NewCommandOptions& options) {
        // التحقق من اسم المشروع
        if (options.projectName.empty()) {
            printError("يجب تحديد اسم المشروع");
            return 1;
        }
        
        // التحقق من عدم وجود المجلد
        fs::path projectPath = fs::current_path() / options.projectName;
        if (fs::exists(projectPath)) {
            printError("المجلد '" + options.projectName + "' موجود مسبقاً");
            return 1;
        }
        
        // اختيار القالب
        std::string templateName = options.templateName.empty() 
            ? (options.isLibrary ? "مكتبة" : "أساسي")
            : options.templateName;
        
        const ProjectTemplate* tmpl = templates_.getTemplate(templateName);
        if (!tmpl) {
            printError("القالب '" + templateName + "' غير موجود");
            printInfo("القوالب المتاحة:");
            for (const auto& name : templates_.listTemplates()) {
                std::cout << "  - " << name << "\n";
            }
            return 1;
        }
        
        if (options.verbose) {
            printInfo("إنشاء مشروع: " + options.projectName);
            printInfo("القالب: " + tmpl->nameAr);
        }
        
        // إنشاء هيكل المجلدات
        try {
            createProjectStructure(projectPath, options, *tmpl);
        }
        catch (const std::exception& e) {
            printError("فشل إنشاء المشروع: " + std::string(e.what()));
            return 1;
        }
        
        // تهيئة Git
        if (options.useGit) {
            initGit(projectPath);
        }
        
        // رسالة النجاح
        printSuccess();
        std::cout << "\n";
        std::cout << "  📁 تم إنشاء: " << options.projectName << "/\n";
        std::cout << "\n";
        std::cout << "  للبدء:\n";
        std::cout << "    cd " << options.projectName << "\n";
        std::cout << "    ص بناء\n";
        std::cout << "    ص تشغيل\n";
        std::cout << "\n";
        
        return 0;
    }
    
private:
    TemplateLibrary templates_;
    
    void createProjectStructure(
        const fs::path& projectPath,
        const NewCommandOptions& options,
        const ProjectTemplate& tmpl
    ) {
        // إنشاء المجلد الرئيسي
        fs::create_directories(projectPath);
        
        // إنشاء ص.toml
        createPackageToml(projectPath, options);
        
        // إنشاء ملفات القالب
        for (const auto& [relativePath, content] : tmpl.files) {
            fs::path filePath = projectPath / relativePath;
            fs::create_directories(filePath.parent_path());
            
            std::ofstream file(filePath);
            file << content;
        }
        
        // إنشاء README.md
        createReadme(projectPath, options);
        
        // إنشاء .gitignore
        createGitignore(projectPath);
    }
    
    void createPackageToml(const fs::path& projectPath, const NewCommandOptions& options) {
        std::ofstream file(projectPath / "ص.toml");
        
        file << "# ═══════════════════════════════════════════════════════════\n";
        file << "#              ملف إعدادات الحزمة - لغة سعد\n";
        file << "# ═══════════════════════════════════════════════════════════\n\n";
        
        file << "[الحزمة]\n";
        file << "الاسم = \"" << options.projectName << "\"\n";
        file << "النسخة = \"0.1.0\"\n";
        file << "الوصف = \"مشروع سعد جديد\"\n";
        
        if (!options.author.empty()) {
            file << "المؤلف = \"" << options.author << "\"\n";
        }
        
        file << "الرخصة = \"" << options.license << "\"\n";
        file << "\n";
        
        file << "[البناء]\n";
        if (options.isLibrary) {
            file << "الهدف = \"مكتبة\"\n";
            file << "المدخل = \"src/lib.ص\"\n";
        } else {
            file << "الهدف = \"تنفيذي\"\n";
            file << "المدخل = \"src/main.ص\"\n";
        }
        file << "\n";
        
        file << "[المتطلبات]\n";
        file << "# أضف التبعيات هنا\n";
        file << "\n";
        
        file << "[التطوير]\n";
        file << "# تبعيات التطوير\n";
    }
    
    void createReadme(const fs::path& projectPath, const NewCommandOptions& options) {
        std::ofstream file(projectPath / "README.md");
        
        file << "# " << options.projectName << "\n\n";
        file << "مشروع مكتوب بلغة سعد العربية 🌟\n\n";
        file << "## البناء والتشغيل\n\n";
        file << "```bash\n";
        file << "ص بناء\n";
        file << "ص تشغيل\n";
        file << "```\n\n";
        file << "## الهيكل\n\n";
        file << "```\n";
        file << options.projectName << "/\n";
        file << "├── ص.toml         # إعدادات الحزمة\n";
        file << "├── src/           # الكود المصدري\n";
        file << "│   └── main.ص   # نقطة البداية\n";
        file << "└── README.md      # هذا الملف\n";
        file << "```\n\n";
        file << "## الرخصة\n\n";
        file << options.license << "\n";
    }
    
    void createGitignore(const fs::path& projectPath) {
        std::ofstream file(projectPath / ".gitignore");
        
        file << "# ═══════════════════════════════════════════════════════════\n";
        file << "#                    ملفات يتم تجاهلها\n";
        file << "# ═══════════════════════════════════════════════════════════\n\n";
        
        file << "# مخرجات البناء\n";
        file << "build/\n";
        file << "dist/\n";
        file << "*.exe\n";
        file << "*.dll\n";
        file << "*.so\n";
        file << "*.dylib\n";
        file << "\n";
        
        file << "# ملفات القفل المحلية\n";
        file << "ص.قفل\n";
        file << "\n";
        
        file << "# ذاكرة التخزين المؤقت\n";
        file << ".sad_cache/\n";
        file << "__pycache__/\n";
        file << "\n";
        
        file << "# ملفات النظام\n";
        file << ".DS_Store\n";
        file << "Thumbs.db\n";
        file << "\n";
        
        file << "# بيئات التطوير\n";
        file << ".vscode/\n";
        file << ".idea/\n";
        file << "*.swp\n";
        file << "*.swo\n";
    }
    
    void initGit(const fs::path& projectPath) {
        // محاولة تهيئة Git (يتم تجاهل الفشل)
        std::string cmd = "git init \"" + projectPath.string() + "\" > nul 2>&1";
        system(cmd.c_str());
    }
    
    void printError(const std::string& msg) {
        std::cerr << "❌ خطأ: " << msg << "\n";
    }
    
    void printInfo(const std::string& msg) {
        std::cout << "ℹ️  " << msg << "\n";
    }
    
    void printSuccess() {
        std::cout << "✅ تم إنشاء المشروع بنجاح!\n";
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                        واجهة خارجية | External Interface
// ═══════════════════════════════════════════════════════════════════════════════

extern "C" {
    int sad_new_project(
        const char* name,
        const char* templateName,
        int isLibrary,
        int useGit,
        const char* author,
        const char* license
    ) {
        NewCommandOptions options;
        options.projectName = name ? name : "";
        options.templateName = templateName ? templateName : "";
        options.isLibrary = isLibrary != 0;
        options.useGit = useGit != 0;
        options.author = author ? author : "";
        options.license = license ? license : "MIT";
        
        NewCommandExecutor executor;
        return executor.execute(options);
    }
    
    const char* sad_list_templates() {
        static std::string result;
        TemplateLibrary lib;
        
        result = "";
        for (const auto& name : lib.listTemplates()) {
            if (!result.empty()) result += "\n";
            result += name;
        }
        return result.c_str();
    }
}

} // namespace tools
} // namespace sad
