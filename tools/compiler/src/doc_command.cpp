/**
 * =============================================================================
 * ملف: doc_command.cpp
 * الوصف: أمر CLI لتوليد التوثيق (ص وثّق)
 * المهمة: T232 - `ص وثّق` CLI command
 * المرحلة: Phase 23 - User Story 20 (Documentation System ص وثّق)
 * =============================================================================
 * 
 * 📖 دليل استخدام أمر التوثيق
 * ═══════════════════════════════
 * 
 * الأمر الأساسي:
 * ──────────────
 * 
 * ص وثّق [خيارات] [ملفات]
 * 
 * أمثلة عملية:
 * ─────────────
 * 
 * # توثيق مشروع كامل
 * ص وثّق
 * 
 * # توثيق ملف محدد
 * ص وثّق src/main.ص
 * 
 * # توثيق مع خرج HTML
 * ص وثّق --html --خرج docs/
 * 
 * # توثيق بتنسيق Markdown
 * ص وثّق --ماركداون
 * 
 * # توثيق مع البحث
 * ص وثّق --html --بحث
 * 
 * الخيارات المتاحة:
 * ─────────────────
 * 
 * --html            توليد موقع HTML
 * --ماركداون       توليد ملفات Markdown
 * --json            توليد ملف JSON
 * --خرج <مجلد>     مجلد الخرج (الافتراضي: docs/)
 * --بحث             تضمين البحث
 * --خاص             تضمين العناصر الخاصة
 * --مفصل            معلومات تفصيلية
 * --عنوان <نص>     عنوان التوثيق
 * --مساعدة          عرض هذه الرسالة
 * 
 * تنسيق التعليقات:
 * ─────────────────
 * 
 * /// هذا توثيق للدالة
 * /// @معطى اسم وصف المعطى
 * /// @أرجع وصف القيمة المرجعة
 * /// @مثال كود المثال
 * دالة مرحبا(اسم: نص) -> نص
 *     ارجع "مرحباً " + اسم
 * نهاية
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <chrono>

namespace fs = std::filesystem;

namespace sad::docs {

// =============================================================================
// تكوين أمر التوثيق
// =============================================================================

struct DocConfig {
    // تنسيق الخرج
    bool htmlOutput = true;
    bool markdownOutput = false;
    bool jsonOutput = false;
    
    // خيارات
    std::string outputDir = "docs";
    std::string title = "توثيق لغة ص";
    std::string description = "التوثيق الرسمي";
    std::string version = "1.0.0";
    bool searchEnabled = true;
    bool includePrivate = false;
    bool verbose = false;
    bool watchMode = false;
    
    // ملفات الإدخال
    std::vector<std::string> inputFiles;
    std::vector<std::string> includePaths;
    std::string configFile;
    
    // تخصيص
    std::string logoUrl;
    std::string customCss;
    std::string theme = "dark";
};

// =============================================================================
// محلل الأوامر
// =============================================================================

class DocArgParser {
public:
    DocConfig parse(int argc, char* argv[]) {
        DocConfig config;
        
        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            
            // تنسيقات الخرج
            if (arg == "--html" || arg == "--هتمل") {
                config.htmlOutput = true;
            }
            else if (arg == "--markdown" || arg == "--ماركداون") {
                config.markdownOutput = true;
            }
            else if (arg == "--json" || arg == "--جيسون") {
                config.jsonOutput = true;
            }
            
            // خيارات
            else if (arg == "-o" || arg == "--output" || arg == "--خرج") {
                if (i + 1 < argc) {
                    config.outputDir = argv[++i];
                }
            }
            else if (arg == "--title" || arg == "--عنوان") {
                if (i + 1 < argc) {
                    config.title = argv[++i];
                }
            }
            else if (arg == "--description" || arg == "--وصف") {
                if (i + 1 < argc) {
                    config.description = argv[++i];
                }
            }
            else if (arg == "--version" || arg == "--نسخة") {
                if (i + 1 < argc) {
                    config.version = argv[++i];
                }
            }
            else if (arg == "--search" || arg == "--بحث") {
                config.searchEnabled = true;
            }
            else if (arg == "--no-search" || arg == "--بدون-بحث") {
                config.searchEnabled = false;
            }
            else if (arg == "--private" || arg == "--خاص") {
                config.includePrivate = true;
            }
            else if (arg == "-v" || arg == "--verbose" || arg == "--مفصل") {
                config.verbose = true;
            }
            else if (arg == "--watch" || arg == "--راقب") {
                config.watchMode = true;
            }
            else if (arg == "--config" || arg == "--تكوين") {
                if (i + 1 < argc) {
                    config.configFile = argv[++i];
                }
            }
            else if (arg == "--theme" || arg == "--سمة") {
                if (i + 1 < argc) {
                    config.theme = argv[++i];
                }
            }
            else if (arg == "--logo" || arg == "--شعار") {
                if (i + 1 < argc) {
                    config.logoUrl = argv[++i];
                }
            }
            else if (arg == "--css") {
                if (i + 1 < argc) {
                    config.customCss = argv[++i];
                }
            }
            else if (arg == "-I" || arg == "--include") {
                if (i + 1 < argc) {
                    config.includePaths.push_back(argv[++i]);
                }
            }
            else if (arg == "-h" || arg == "--help" || arg == "--مساعدة") {
                showHelp();
                exit(0);
            }
            
            // ملف إدخال
            else if (arg[0] != '-') {
                config.inputFiles.push_back(arg);
            }
            else {
                std::cerr << "⚠️ خيار غير معروف: " << arg << "\n";
            }
        }
        
        return config;
    }
    
    void showHelp() {
        std::cout << R"(
╔═══════════════════════════════════════════════════════════════════╗
║                     📖 ص وثّق - مولد التوثيق                      ║
╠═══════════════════════════════════════════════════════════════════╣
║  يولد توثيقاً احترافياً من تعليقات الكود بلغة ص                   ║
╚═══════════════════════════════════════════════════════════════════╝

الاستخدام:
    ص وثّق [خيارات] [ملفات...]

تنسيقات الخرج:
    --html, --هتمل          توليد موقع HTML (الافتراضي)
    --markdown, --ماركداون  توليد ملفات Markdown
    --json, --جيسون         توليد ملف JSON

الخيارات الأساسية:
    -o, --خرج <مجلد>       مجلد الخرج (الافتراضي: docs/)
    --عنوان <نص>           عنوان التوثيق
    --وصف <نص>             وصف المشروع
    --نسخة <رقم>           رقم النسخة

خيارات المحتوى:
    --بحث                   تضمين البحث (مفعل افتراضياً)
    --بدون-بحث              تعطيل البحث
    --خاص                   تضمين العناصر الخاصة
    -I, --include <مسار>    إضافة مسار للتضمين

خيارات التخصيص:
    --سمة <اسم>             السمة (dark/light)
    --شعار <url>            رابط الشعار
    --css <ملف>             ملف CSS مخصص

خيارات أخرى:
    -v, --مفصل              وضع التفصيل
    --راقب                  مراقبة التغييرات
    --تكوين <ملف>          ملف التكوين
    -h, --مساعدة            عرض هذه الرسالة

أمثلة:
    # توثيق المشروع الحالي
    ص وثّق

    # توثيق ملف محدد
    ص وثّق src/main.ص

    # توثيق مع خيارات
    ص وثّق --html --خرج build/docs --عنوان "مشروعي"

    # مراقبة وتوليد تلقائي
    ص وثّق --راقب

تنسيق التعليقات:
    /// ملخص الدالة
    /// @معطى اسم وصف المعطى
    /// @أرجع وصف القيمة المرجعة
    /// @مثال كود المثال
    /// @ملاحظة ملاحظة مهمة
    /// @تحذير تحذير للمستخدم
    /// @منذ 1.0.0
    /// @مهمل استخدم الدالة الجديدة

)" << "\n";
    }
};

// =============================================================================
// نتيجة التوليد
// =============================================================================

struct GenerationResult {
    bool success = true;
    int filesProcessed = 0;
    int docsGenerated = 0;
    int warnings = 0;
    int errors = 0;
    double timeSeconds = 0;
    std::vector<std::string> errorMessages;
    std::vector<std::string> warningMessages;
    
    void print() const {
        std::cout << "\n";
        if (success) {
            std::cout << "╔═══════════════════════════════════════════╗\n";
            std::cout << "║         ✅ تم توليد التوثيق بنجاح         ║\n";
            std::cout << "╚═══════════════════════════════════════════╝\n";
        } else {
            std::cout << "╔═══════════════════════════════════════════╗\n";
            std::cout << "║         ❌ فشل توليد التوثيق              ║\n";
            std::cout << "╚═══════════════════════════════════════════╝\n";
        }
        
        std::cout << "\n📊 الإحصائيات:\n";
        std::cout << "   ملفات تمت معالجتها: " << filesProcessed << "\n";
        std::cout << "   صفحات تم توليدها: " << docsGenerated << "\n";
        std::cout << "   الوقت: " << timeSeconds << " ثانية\n";
        
        if (warnings > 0) {
            std::cout << "   ⚠️ تحذيرات: " << warnings << "\n";
        }
        if (errors > 0) {
            std::cout << "   ❌ أخطاء: " << errors << "\n";
        }
        
        // عرض الأخطاء
        if (!errorMessages.empty()) {
            std::cout << "\n❌ الأخطاء:\n";
            for (const auto& msg : errorMessages) {
                std::cout << "   " << msg << "\n";
            }
        }
        
        // عرض التحذيرات
        if (!warningMessages.empty()) {
            std::cout << "\n⚠️ التحذيرات:\n";
            for (const auto& msg : warningMessages) {
                std::cout << "   " << msg << "\n";
            }
        }
    }
};

// =============================================================================
// جامع الملفات
// =============================================================================

class FileCollector {
public:
    std::vector<std::string> collect(const std::vector<std::string>& inputs) {
        std::vector<std::string> files;
        
        if (inputs.empty()) {
            // جمع من المجلد الحالي
            collectFromDirectory(".", files);
        } else {
            for (const auto& input : inputs) {
                if (fs::is_directory(input)) {
                    collectFromDirectory(input, files);
                } else if (fs::exists(input)) {
                    files.push_back(input);
                }
            }
        }
        
        return files;
    }
    
private:
    void collectFromDirectory(const std::string& dir, std::vector<std::string>& files) {
        try {
            for (const auto& entry : fs::recursive_directory_iterator(dir)) {
                if (entry.is_regular_file()) {
                    std::string ext = entry.path().extension().string();
                    if (ext == ".ص" || ext == ".sad") {
                        files.push_back(entry.path().string());
                    }
                }
            }
        } catch (const fs::filesystem_error& e) {
            std::cerr << "⚠️ خطأ في قراءة المجلد: " << e.what() << "\n";
        }
    }
};

// =============================================================================
// عنصر توثيق
// =============================================================================

struct DocItem {
    std::string name;
    std::string type;
    std::string signature;
    std::string summary;
    std::string description;
    std::vector<std::pair<std::string, std::string>> parameters;
    std::map<std::string, std::string> paramDocs;
    std::string returnType;
    std::string returnDoc;
    std::vector<std::string> examples;
    std::vector<std::string> seeAlso;
    std::string since;
    std::string deprecated;
    std::string note;
    std::string warning;
    std::string file;
    int line = 0;
    bool isPrivate = false;
};

// =============================================================================
// محلل التوثيق
// =============================================================================

class DocParser {
public:
    std::vector<DocItem> parseFile(const std::string& filePath) {
        std::vector<DocItem> items;
        
        std::ifstream file(filePath);
        if (!file.is_open()) {
            return items;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        
        // تحليل بسيط
        std::istringstream stream(content);
        std::string line;
        std::vector<std::string> docLines;
        int lineNumber = 0;
        
        while (std::getline(stream, line)) {
            lineNumber++;
            
            // تجميع تعليقات التوثيق
            if (line.find("///") != std::string::npos) {
                size_t pos = line.find("///");
                std::string comment = line.substr(pos + 3);
                // إزالة المسافات الأمامية
                size_t start = comment.find_first_not_of(" \t");
                if (start != std::string::npos) {
                    comment = comment.substr(start);
                } else {
                    comment = "";
                }
                docLines.push_back(comment);
            }
            // التعرف على تعريف
            else if (!docLines.empty() && 
                    (line.find("دالة ") != std::string::npos ||
                     line.find("صنف ") != std::string::npos ||
                     line.find("هيكل ") != std::string::npos ||
                     line.find("ثابت ") != std::string::npos)) {
                
                DocItem item = parseDocLines(docLines);
                item.file = filePath;
                item.line = lineNumber;
                
                // تحليل التوقيع
                parseSignature(line, item);
                
                items.push_back(item);
                docLines.clear();
            }
            else if (!line.empty() && line.find("///") == std::string::npos) {
                docLines.clear();
            }
        }
        
        return items;
    }
    
private:
    DocItem parseDocLines(const std::vector<std::string>& lines) {
        DocItem item;
        std::ostringstream description;
        bool inDescription = true;
        
        for (const auto& line : lines) {
            if (line.empty()) {
                if (inDescription) description << "\n";
                continue;
            }
            
            if (line[0] == '@') {
                inDescription = false;
                parseTag(line, item);
            } else if (inDescription) {
                if (item.summary.empty()) {
                    item.summary = line;
                } else {
                    description << line << "\n";
                }
            }
        }
        
        item.description = description.str();
        return item;
    }
    
    void parseTag(const std::string& line, DocItem& item) {
        size_t spacePos = line.find(' ');
        std::string tag = (spacePos != std::string::npos) 
            ? line.substr(0, spacePos) 
            : line;
        std::string content = (spacePos != std::string::npos) 
            ? line.substr(spacePos + 1) 
            : "";
        
        if (tag == "@معطى" || tag == "@param") {
            size_t nameEnd = content.find(' ');
            if (nameEnd != std::string::npos) {
                std::string name = content.substr(0, nameEnd);
                std::string doc = content.substr(nameEnd + 1);
                item.paramDocs[name] = doc;
            }
        }
        else if (tag == "@أرجع" || tag == "@return" || tag == "@returns") {
            item.returnDoc = content;
        }
        else if (tag == "@مثال" || tag == "@example") {
            item.examples.push_back(content);
        }
        else if (tag == "@انظر" || tag == "@see") {
            item.seeAlso.push_back(content);
        }
        else if (tag == "@منذ" || tag == "@since") {
            item.since = content;
        }
        else if (tag == "@مهمل" || tag == "@deprecated") {
            item.deprecated = content;
        }
        else if (tag == "@ملاحظة" || tag == "@note") {
            item.note = content;
        }
        else if (tag == "@تحذير" || tag == "@warning") {
            item.warning = content;
        }
    }
    
    void parseSignature(const std::string& line, DocItem& item) {
        // التعرف على نوع العنصر
        if (line.find("دالة ") != std::string::npos) {
            item.type = "function";
            parseFunction(line, item);
        }
        else if (line.find("صنف ") != std::string::npos) {
            item.type = "class";
            parseClass(line, item);
        }
        else if (line.find("هيكل ") != std::string::npos) {
            item.type = "struct";
            parseStruct(line, item);
        }
        else if (line.find("ثابت ") != std::string::npos) {
            item.type = "constant";
            parseConstant(line, item);
        }
        
        item.signature = line;
    }
    
    void parseFunction(const std::string& line, DocItem& item) {
        // دالة اسم(معطيات) -> نوع
        size_t funcStart = line.find("دالة ") + 5;
        size_t parenStart = line.find('(');
        size_t parenEnd = line.find(')');
        size_t arrowPos = line.find("->");
        
        if (parenStart != std::string::npos) {
            item.name = line.substr(funcStart, parenStart - funcStart);
            // إزالة المسافات
            while (!item.name.empty() && item.name.back() == ' ') {
                item.name.pop_back();
            }
            
            // المعطيات
            if (parenEnd > parenStart + 1) {
                std::string params = line.substr(parenStart + 1, parenEnd - parenStart - 1);
                parseParameters(params, item);
            }
            
            // نوع الإرجاع
            if (arrowPos != std::string::npos) {
                item.returnType = line.substr(arrowPos + 2);
                // إزالة المسافات
                size_t start = item.returnType.find_first_not_of(" \t");
                if (start != std::string::npos) {
                    item.returnType = item.returnType.substr(start);
                }
            }
        }
    }
    
    void parseClass(const std::string& line, DocItem& item) {
        size_t start = line.find("صنف ") + 4;
        size_t end = line.find_first_of(" {:<");
        if (end == std::string::npos) end = line.length();
        item.name = line.substr(start, end - start);
    }
    
    void parseStruct(const std::string& line, DocItem& item) {
        size_t start = line.find("هيكل ") + 5;
        size_t end = line.find_first_of(" {:<");
        if (end == std::string::npos) end = line.length();
        item.name = line.substr(start, end - start);
    }
    
    void parseConstant(const std::string& line, DocItem& item) {
        size_t start = line.find("ثابت ") + 5;
        size_t end = line.find_first_of(" =:");
        if (end == std::string::npos) end = line.length();
        item.name = line.substr(start, end - start);
    }
    
    void parseParameters(const std::string& params, DocItem& item) {
        std::istringstream stream(params);
        std::string param;
        
        while (std::getline(stream, param, ',')) {
            // تنظيف المسافات
            size_t start = param.find_first_not_of(" \t");
            size_t end = param.find_last_not_of(" \t");
            if (start != std::string::npos && end != std::string::npos) {
                param = param.substr(start, end - start + 1);
            }
            
            // اسم: نوع
            size_t colonPos = param.find(':');
            if (colonPos != std::string::npos) {
                std::string name = param.substr(0, colonPos);
                std::string type = param.substr(colonPos + 1);
                
                // تنظيف
                size_t ns = name.find_first_not_of(" \t");
                size_t ne = name.find_last_not_of(" \t");
                if (ns != std::string::npos) name = name.substr(ns, ne - ns + 1);
                
                ns = type.find_first_not_of(" \t");
                ne = type.find_last_not_of(" \t");
                if (ns != std::string::npos) type = type.substr(ns, ne - ns + 1);
                
                item.parameters.push_back({name, type});
            }
        }
    }
};

// =============================================================================
// أمر التوثيق الرئيسي
// =============================================================================

class DocCommand {
public:
    int run(int argc, char* argv[]) {
        // تحليل الأوامر
        DocArgParser parser;
        config_ = parser.parse(argc, argv);
        
        // عرض البانر
        showBanner();
        
        // جمع الملفات
        FileCollector collector;
        auto files = collector.collect(config_.inputFiles);
        
        if (files.empty()) {
            std::cerr << "❌ لم يتم العثور على ملفات للتوثيق\n";
            return 1;
        }
        
        if (config_.verbose) {
            std::cout << "📁 ملفات للمعالجة: " << files.size() << "\n";
        }
        
        // البدء
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // معالجة الملفات
        GenerationResult result;
        std::vector<DocItem> allDocs;
        
        DocParser docParser;
        for (const auto& file : files) {
            if (config_.verbose) {
                std::cout << "   📄 " << file << "\n";
            }
            
            auto docs = docParser.parseFile(file);
            allDocs.insert(allDocs.end(), docs.begin(), docs.end());
            result.filesProcessed++;
        }
        
        // فلترة الخاصة
        if (!config_.includePrivate) {
            allDocs.erase(
                std::remove_if(allDocs.begin(), allDocs.end(),
                    [](const DocItem& item) { return item.isPrivate; }),
                allDocs.end()
            );
        }
        
        // توليد الخرج
        if (config_.htmlOutput) {
            generateHTML(allDocs, result);
        }
        
        if (config_.markdownOutput) {
            generateMarkdown(allDocs, result);
        }
        
        if (config_.jsonOutput) {
            generateJSON(allDocs, result);
        }
        
        // الوقت
        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = endTime - startTime;
        result.timeSeconds = diff.count();
        
        // النتيجة
        result.print();
        
        return result.success ? 0 : 1;
    }
    
private:
    DocConfig config_;
    
    void showBanner() {
        std::cout << R"(
╔═══════════════════════════════════════════════════════════════════╗
║                      📖 ص وثّق v1.0.0                             ║
║                     مولد التوثيق للغة ص                           ║
╚═══════════════════════════════════════════════════════════════════╝
)" << "\n";
    }
    
    void generateHTML(const std::vector<DocItem>& docs, GenerationResult& result) {
        std::cout << "🌐 توليد HTML...\n";
        
        // إنشاء المجلد
        fs::create_directories(config_.outputDir);
        fs::create_directories(config_.outputDir + "/api");
        fs::create_directories(config_.outputDir + "/assets");
        
        // الصفحة الرئيسية
        generateIndexHTML(docs);
        
        // صفحات API
        for (const auto& doc : docs) {
            generateDocHTML(doc);
            result.docsGenerated++;
        }
        
        // صفحات القوائم
        generateListHTML(docs, "function", "الدوال");
        generateListHTML(docs, "class", "الأصناف");
        
        // البحث
        if (config_.searchEnabled) {
            generateSearchData(docs);
        }
        
        // CSS
        generateCSS();
        
        std::cout << "   ✅ تم توليد " << result.docsGenerated << " صفحة\n";
    }
    
    void generateMarkdown(const std::vector<DocItem>& docs, GenerationResult& result) {
        std::cout << "📝 توليد Markdown...\n";
        
        fs::create_directories(config_.outputDir);
        
        // README
        std::ofstream readme(config_.outputDir + "/README.md");
        readme << "# " << config_.title << "\n\n";
        readme << config_.description << "\n\n";
        readme << "## الفهرس\n\n";
        
        for (const auto& doc : docs) {
            readme << "- [" << doc.name << "](api/" << doc.name << ".md)\n";
        }
        
        // صفحات API
        fs::create_directories(config_.outputDir + "/api");
        
        for (const auto& doc : docs) {
            std::ofstream file(config_.outputDir + "/api/" + doc.name + ".md");
            file << generateDocMarkdown(doc);
            result.docsGenerated++;
        }
        
        std::cout << "   ✅ تم توليد " << result.docsGenerated << " ملف\n";
    }
    
    void generateJSON(const std::vector<DocItem>& docs, GenerationResult& result) {
        std::cout << "📊 توليد JSON...\n";
        
        fs::create_directories(config_.outputDir);
        
        std::ofstream file(config_.outputDir + "/docs.json");
        file << "{\n";
        file << "  \"title\": \"" << config_.title << "\",\n";
        file << "  \"version\": \"" << config_.version << "\",\n";
        file << "  \"items\": [\n";
        
        bool first = true;
        for (const auto& doc : docs) {
            if (!first) file << ",\n";
            first = false;
            file << generateDocJSON(doc);
        }
        
        file << "\n  ]\n}\n";
        
        result.docsGenerated = 1;
        std::cout << "   ✅ تم توليد docs.json\n";
    }
    
    void generateIndexHTML(const std::vector<DocItem>& docs) {
        std::ofstream file(config_.outputDir + "/index.html");
        file << "<!DOCTYPE html>\n<html dir=\"rtl\" lang=\"ar\">\n<head>\n";
        file << "  <meta charset=\"UTF-8\">\n";
        file << "  <title>" << config_.title << "</title>\n";
        file << "  <link rel=\"stylesheet\" href=\"assets/style.css\">\n";
        file << "</head>\n<body>\n";
        file << "  <h1>📖 " << config_.title << "</h1>\n";
        file << "  <p>" << config_.description << "</p>\n";
        file << "  <h2>الفهرس</h2>\n";
        file << "  <ul>\n";
        
        for (const auto& doc : docs) {
            file << "    <li><a href=\"api/" << doc.name << ".html\">" 
                 << doc.name << "</a> - " << doc.summary << "</li>\n";
        }
        
        file << "  </ul>\n";
        file << "</body>\n</html>\n";
    }
    
    void generateDocHTML(const DocItem& doc) {
        std::ofstream file(config_.outputDir + "/api/" + doc.name + ".html");
        
        file << "<!DOCTYPE html>\n<html dir=\"rtl\" lang=\"ar\">\n<head>\n";
        file << "  <meta charset=\"UTF-8\">\n";
        file << "  <title>" << doc.name << " - " << config_.title << "</title>\n";
        file << "  <link rel=\"stylesheet\" href=\"../assets/style.css\">\n";
        file << "</head>\n<body>\n";
        
        file << "  <nav><a href=\"../index.html\">🏠 الرئيسية</a></nav>\n";
        file << "  <h1>" << doc.name << "</h1>\n";
        file << "  <span class=\"type\">" << doc.type << "</span>\n";
        file << "  <pre class=\"signature\"><code>" << doc.signature << "</code></pre>\n";
        file << "  <p class=\"summary\">" << doc.summary << "</p>\n";
        
        if (!doc.description.empty()) {
            file << "  <div class=\"description\">" << doc.description << "</div>\n";
        }
        
        if (!doc.parameters.empty()) {
            file << "  <h2>المعطيات</h2>\n<table>\n";
            for (const auto& [name, type] : doc.parameters) {
                file << "    <tr><td>" << name << "</td><td>" << type << "</td><td>";
                auto it = doc.paramDocs.find(name);
                if (it != doc.paramDocs.end()) file << it->second;
                file << "</td></tr>\n";
            }
            file << "  </table>\n";
        }
        
        if (!doc.returnType.empty()) {
            file << "  <h2>القيمة المرجعة</h2>\n";
            file << "  <p><code>" << doc.returnType << "</code></p>\n";
            if (!doc.returnDoc.empty()) {
                file << "  <p>" << doc.returnDoc << "</p>\n";
            }
        }
        
        if (!doc.examples.empty()) {
            file << "  <h2>أمثلة</h2>\n";
            for (const auto& ex : doc.examples) {
                file << "  <pre><code>" << ex << "</code></pre>\n";
            }
        }
        
        file << "</body>\n</html>\n";
    }
    
    void generateListHTML(
        const std::vector<DocItem>& docs,
        const std::string& type,
        const std::string& title
    ) {
        std::ofstream file(config_.outputDir + "/api/" + type + "s.html");
        file << "<!DOCTYPE html>\n<html dir=\"rtl\" lang=\"ar\">\n<head>\n";
        file << "  <meta charset=\"UTF-8\">\n";
        file << "  <title>" << title << " - " << config_.title << "</title>\n";
        file << "  <link rel=\"stylesheet\" href=\"../assets/style.css\">\n";
        file << "</head>\n<body>\n";
        file << "  <h1>" << title << "</h1>\n<ul>\n";
        
        for (const auto& doc : docs) {
            if (doc.type == type) {
                file << "  <li><a href=\"" << doc.name << ".html\">" 
                     << doc.name << "</a> - " << doc.summary << "</li>\n";
            }
        }
        
        file << "</ul>\n</body>\n</html>\n";
    }
    
    void generateSearchData(const std::vector<DocItem>& docs) {
        std::ofstream file(config_.outputDir + "/assets/search.json");
        file << "[\n";
        
        bool first = true;
        for (const auto& doc : docs) {
            if (!first) file << ",\n";
            first = false;
            file << "{\"name\":\"" << doc.name << "\",\"type\":\"" << doc.type
                 << "\",\"summary\":\"" << doc.summary << "\",\"url\":\"api/"
                 << doc.name << ".html\"}";
        }
        
        file << "\n]\n";
    }
    
    void generateCSS() {
        std::ofstream file(config_.outputDir + "/assets/style.css");
        file << R"(
* { margin: 0; padding: 0; box-sizing: border-box; }
body { font-family: 'Segoe UI', Arial, sans-serif; background: #1a1a2e; color: #eee; line-height: 1.8; padding: 20px; }
h1, h2 { color: #4dabf7; margin-bottom: 15px; }
a { color: #69b5ff; }
nav { margin-bottom: 20px; }
.type { background: #4dabf7; color: #000; padding: 3px 10px; border-radius: 5px; }
.signature { background: #0f0f23; padding: 15px; border-radius: 8px; margin: 15px 0; }
table { width: 100%; border-collapse: collapse; }
th, td { padding: 10px; border-bottom: 1px solid #333; text-align: right; }
pre { background: #0f0f23; padding: 15px; border-radius: 8px; overflow-x: auto; }
code { font-family: Consolas, monospace; }
)";
    }
    
    std::string generateDocMarkdown(const DocItem& doc) {
        std::ostringstream ss;
        
        ss << "# " << doc.name << "\n\n";
        ss << "**النوع:** " << doc.type << "\n\n";
        ss << "```sad\n" << doc.signature << "\n```\n\n";
        ss << doc.summary << "\n\n";
        
        if (!doc.description.empty()) {
            ss << doc.description << "\n\n";
        }
        
        if (!doc.parameters.empty()) {
            ss << "## المعطيات\n\n";
            ss << "| الاسم | النوع | الوصف |\n";
            ss << "|-------|------|-------|\n";
            for (const auto& [name, type] : doc.parameters) {
                ss << "| " << name << " | " << type << " | ";
                auto it = doc.paramDocs.find(name);
                if (it != doc.paramDocs.end()) ss << it->second;
                ss << " |\n";
            }
            ss << "\n";
        }
        
        if (!doc.returnType.empty()) {
            ss << "## القيمة المرجعة\n\n";
            ss << "- **النوع:** `" << doc.returnType << "`\n";
            if (!doc.returnDoc.empty()) {
                ss << "- " << doc.returnDoc << "\n";
            }
            ss << "\n";
        }
        
        if (!doc.examples.empty()) {
            ss << "## أمثلة\n\n";
            for (const auto& ex : doc.examples) {
                ss << "```sad\n" << ex << "\n```\n\n";
            }
        }
        
        return ss.str();
    }
    
    std::string generateDocJSON(const DocItem& doc) {
        std::ostringstream ss;
        ss << "    {\n";
        ss << "      \"name\": \"" << doc.name << "\",\n";
        ss << "      \"type\": \"" << doc.type << "\",\n";
        ss << "      \"summary\": \"" << doc.summary << "\",\n";
        ss << "      \"signature\": \"" << escapeJSON(doc.signature) << "\",\n";
        ss << "      \"file\": \"" << doc.file << "\",\n";
        ss << "      \"line\": " << doc.line << "\n";
        ss << "    }";
        return ss.str();
    }
    
    std::string escapeJSON(const std::string& s) {
        std::string result;
        for (char c : s) {
            if (c == '"') result += "\\\"";
            else if (c == '\\') result += "\\\\";
            else if (c == '\n') result += " ";
            else result += c;
        }
        return result;
    }
};

} // namespace sad::docs

// =============================================================================
// نقطة الدخول
// =============================================================================

int main(int argc, char* argv[]) {
    sad::docs::DocCommand cmd;
    return cmd.run(argc, argv);
}
