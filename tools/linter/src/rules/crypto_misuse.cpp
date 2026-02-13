/**
 * =============================================================================
 * ملف: crypto_misuse.cpp
 * الوصف: كاشف سوء استخدام التشفير في لغة ص
 * المهمة: T223 - Crypto misuse detector
 * المرحلة: Phase 22 - User Story 19 (Security Linting ص حارس)
 * =============================================================================
 * 
 * 🔐 دليل المبتدئ للتشفير
 * ══════════════════════════
 * 
 * ما هو التشفير؟
 * ───────────────
 * التشفير يحول رسالة مقروءة إلى رموز غير مفهومة.
 * 
 * مثال بسيط:
 * - الرسالة: "مرحبا"
 * - المفتاح: +3
 * - الرسالة المشفرة: كل حرف يتحرك 3 مواقع
 * 
 * لكن هذا التشفير البسيط سهل الكسر!
 * التشفير الحديث أعقد بكثير ويستخدم الرياضيات.
 * 
 * أخطاء شائعة في التشفير:
 * ─────────────────────────
 * 
 * 1. استخدام خوارزميات قديمة 🚫
 *    ─────────────────────────
 *    MD5، SHA1، DES = مكسورة!
 *    مثل استخدام قفل من 1950 لحماية بنك!
 * 
 * 2. مفاتيح ثابتة في الكود 🚫
 *    ────────────────────────
 *    مثل كتابة الرقم السري على الباب!
 *    المفاتيح يجب أن تكون في مكان آمن.
 * 
 * 3. IV ثابت أو متوقع 🚫
 *    ──────────────────
 *    IV = قيمة ابتدائية عشوائية
 *    إذا كانت ثابتة، نفس الرسالة تعطي نفس النتيجة!
 * 
 * 4. عدم التحقق من الأصالة 🚫
 *    ────────────────────────
 *    Encryption ≠ Authentication
 *    التشفير يخفي، لكن لا يثبت من المرسل!
 * 
 * كيف تستخدم التشفير بشكل صحيح؟
 * ───────────────────────────────
 * 1. استخدم مكتبات موثوقة (لا تكتب تشفيرك!)
 * 2. استخدم خوارزميات حديثة (AES-256، SHA-256)
 * 3. لا تضع المفاتيح في الكود
 * 4. استخدم IV عشوائي لكل رسالة
 * 5. تحقق من سلامة البيانات (HMAC، AEAD)
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <regex>
#include <map>
#include <set>
#include <sstream>
#include <iostream>
#include <memory>

namespace sad::security::rules {

// =============================================================================
// أنواع مشاكل التشفير
// =============================================================================

enum class CryptoIssueType {
    WeakAlgorithm,          // خوارزمية ضعيفة
    HardcodedKey,           // مفتاح ثابت في الكود
    HardcodedIV,            // IV ثابت
    WeakRandomness,         // عشوائية ضعيفة
    InsecureMode,           // وضع تشفير غير آمن
    NoPadding,              // بدون حشو
    NoAuthentication,       // بدون تحقق من الأصالة
    ShortKey,               // مفتاح قصير
    DeprecatedAPI,          // API قديم
    InsecureKeyStorage,     // تخزين مفاتيح غير آمن
    InsecureComparison,     // مقارنة غير آمنة
    WeakPassword            // كلمة سر ضعيفة
};

inline std::string cryptoIssueToArabic(CryptoIssueType type) {
    switch (type) {
        case CryptoIssueType::WeakAlgorithm:       return "خوارزمية ضعيفة";
        case CryptoIssueType::HardcodedKey:        return "مفتاح ثابت في الكود";
        case CryptoIssueType::HardcodedIV:         return "IV ثابت";
        case CryptoIssueType::WeakRandomness:      return "عشوائية ضعيفة";
        case CryptoIssueType::InsecureMode:        return "وضع تشفير غير آمن";
        case CryptoIssueType::NoPadding:           return "بدون حشو";
        case CryptoIssueType::NoAuthentication:    return "بدون تحقق أصالة";
        case CryptoIssueType::ShortKey:            return "مفتاح قصير";
        case CryptoIssueType::DeprecatedAPI:       return "API قديم";
        case CryptoIssueType::InsecureKeyStorage:  return "تخزين مفاتيح غير آمن";
        case CryptoIssueType::InsecureComparison:  return "مقارنة غير آمنة";
        case CryptoIssueType::WeakPassword:        return "كلمة سر ضعيفة";
    }
    return "غير معروف";
}

// =============================================================================
// قاعدة بيانات الخوارزميات
// =============================================================================

struct AlgorithmInfo {
    std::string name;
    bool secure;
    std::string alternative;
    std::string reason;
    int severity; // 1-10
};

inline std::map<std::string, AlgorithmInfo> getAlgorithmDatabase() {
    return {
        // خوارزميات التجزئة (Hash)
        {"MD5", {"MD5", false, "SHA-256 أو SHA-3", "مكسورة - يمكن إنتاج تصادمات", 9}},
        {"md5", {"MD5", false, "SHA-256 أو SHA-3", "مكسورة - يمكن إنتاج تصادمات", 9}},
        {"SHA1", {"SHA-1", false, "SHA-256 أو SHA-3", "مكسورة - تصادمات معروفة", 8}},
        {"sha1", {"SHA-1", false, "SHA-256 أو SHA-3", "مكسورة - تصادمات معروفة", 8}},
        {"SHA-1", {"SHA-1", false, "SHA-256 أو SHA-3", "مكسورة - تصادمات معروفة", 8}},
        
        // خوارزميات التشفير
        {"DES", {"DES", false, "AES-256", "مفتاح 56 بت - قابل للكسر", 9}},
        {"des", {"DES", false, "AES-256", "مفتاح 56 بت - قابل للكسر", 9}},
        {"3DES", {"3DES", false, "AES-256", "بطيء وضعيف نسبياً", 6}},
        {"RC4", {"RC4", false, "AES-GCM", "تحيزات إحصائية معروفة", 9}},
        {"rc4", {"RC4", false, "AES-GCM", "تحيزات إحصائية معروفة", 9}},
        {"Blowfish", {"Blowfish", false, "AES-256", "حجم كتلة صغير (64 بت)", 6}},
        
        // أوضاع التشفير
        {"ECB", {"ECB", false, "CBC أو GCM", "نمط يظهر في النص المشفر", 9}},
        {"ecb", {"ECB", false, "CBC أو GCM", "نمط يظهر في النص المشفر", 9}},
        
        // خوارزميات آمنة
        {"AES", {"AES", true, "", "آمن مع وضع صحيح", 0}},
        {"AES-256", {"AES-256", true, "", "آمن", 0}},
        {"AES-GCM", {"AES-GCM", true, "", "آمن مع تحقق", 0}},
        {"SHA-256", {"SHA-256", true, "", "آمن", 0}},
        {"SHA-512", {"SHA-512", true, "", "آمن", 0}},
        {"SHA-3", {"SHA-3", true, "", "آمن - الجيل الأحدث", 0}},
        {"ChaCha20", {"ChaCha20", true, "", "آمن وسريع", 0}},
        {"bcrypt", {"bcrypt", true, "", "آمن لكلمات السر", 0}},
        {"Argon2", {"Argon2", true, "", "أفضل لكلمات السر", 0}},
    };
}

// =============================================================================
// نتيجة الفحص
// =============================================================================

struct SourceLocation {
    std::string file;
    int line = 0;
    int column = 0;
    
    std::string toString() const {
        std::ostringstream ss;
        ss << file << ":" << line << ":" << column;
        return ss.str();
    }
};

struct CryptoFinding {
    CryptoIssueType type;
    SourceLocation location;
    std::string code;
    std::string message;
    std::string explanation;
    std::string unsafeCode;
    std::string safeCode;
    int severity = 7;
    std::string cweId;
    std::string algorithm;
    std::string alternative;
    
    std::string format() const {
        std::ostringstream ss;
        
        ss << "\n";
        ss << "╔══════════════════════════════════════════════════════════════╗\n";
        ss << "║  🔐 مشكلة تشفير - " << cryptoIssueToArabic(type) << "\n";
        ss << "╠══════════════════════════════════════════════════════════════╣\n";
        ss << "║  📍 الموقع: " << location.toString() << "\n";
        ss << "║  ⚠️ الخطورة: " << severity << "/10\n";
        if (!cweId.empty()) {
            ss << "║  🔗 CWE: " << cweId << "\n";
        }
        if (!algorithm.empty()) {
            ss << "║  🔑 الخوارزمية: " << algorithm << "\n";
        }
        ss << "╠──────────────────────────────────────────────────────────────╣\n";
        ss << "║  ❌ المشكلة:\n";
        ss << "║     " << message << "\n";
        ss << "╠──────────────────────────────────────────────────────────────╣\n";
        ss << "║  📖 الشرح للمبتدئين:\n";
        
        std::istringstream explStream(explanation);
        std::string line;
        while (std::getline(explStream, line)) {
            ss << "║     " << line << "\n";
        }
        
        if (!alternative.empty()) {
            ss << "╠──────────────────────────────────────────────────────────────╣\n";
            ss << "║  🔄 البديل الآمن: " << alternative << "\n";
        }
        
        if (!unsafeCode.empty()) {
            ss << "╠──────────────────────────────────────────────────────────────╣\n";
            ss << "║  ❌ الكود غير الآمن:\n";
            ss << "║     " << unsafeCode << "\n";
        }
        
        if (!safeCode.empty()) {
            ss << "╠──────────────────────────────────────────────────────────────╣\n";
            ss << "║  ✅ الكود الآمن:\n";
            std::istringstream codeStream(safeCode);
            while (std::getline(codeStream, line)) {
                ss << "║     " << line << "\n";
            }
        }
        
        ss << "╚══════════════════════════════════════════════════════════════╝\n";
        
        return ss.str();
    }
};

// =============================================================================
// كاشف سوء استخدام التشفير
// =============================================================================

class CryptoMisuseDetector {
public:
    CryptoMisuseDetector() : algorithms_(getAlgorithmDatabase()) {}
    
    /**
     * فحص سطر
     */
    std::vector<CryptoFinding> checkLine(
        const std::string& line,
        int lineNumber,
        const std::string& filePath
    ) {
        std::vector<CryptoFinding> findings;
        
        // 1. كشف خوارزميات ضعيفة
        checkWeakAlgorithms(line, lineNumber, filePath, findings);
        
        // 2. كشف مفاتيح ثابتة
        checkHardcodedKeys(line, lineNumber, filePath, findings);
        
        // 3. كشف IV ثابت
        checkHardcodedIV(line, lineNumber, filePath, findings);
        
        // 4. كشف عشوائية ضعيفة
        checkWeakRandomness(line, lineNumber, filePath, findings);
        
        // 5. كشف أوضاع تشفير غير آمنة
        checkInsecureModes(line, lineNumber, filePath, findings);
        
        // 6. كشف مقارنة غير آمنة
        checkInsecureComparison(line, lineNumber, filePath, findings);
        
        return findings;
    }
    
    /**
     * فحص ملف كامل
     */
    std::vector<CryptoFinding> checkFile(
        const std::string& content,
        const std::string& filePath
    ) {
        std::vector<CryptoFinding> allFindings;
        std::istringstream stream(content);
        std::string line;
        int lineNumber = 0;
        
        while (std::getline(stream, line)) {
            lineNumber++;
            auto findings = checkLine(line, lineNumber, filePath);
            allFindings.insert(allFindings.end(), findings.begin(), findings.end());
        }
        
        return allFindings;
    }
    
private:
    std::map<std::string, AlgorithmInfo> algorithms_;
    
    /**
     * كشف خوارزميات ضعيفة
     */
    void checkWeakAlgorithms(
        const std::string& line,
        int lineNumber,
        const std::string& filePath,
        std::vector<CryptoFinding>& findings
    ) {
        for (const auto& [name, info] : algorithms_) {
            if (!info.secure) {
                // بحث عن الخوارزمية
                std::regex pattern("\\b" + name + "\\b", std::regex::icase);
                std::smatch match;
                
                if (std::regex_search(line, match, pattern)) {
                    CryptoFinding finding;
                    finding.type = CryptoIssueType::WeakAlgorithm;
                    finding.location = {filePath, lineNumber, static_cast<int>(match.position()) + 1};
                    finding.code = line;
                    finding.algorithm = info.name;
                    finding.alternative = info.alternative;
                    finding.severity = info.severity;
                    finding.cweId = "CWE-327";
                    finding.message = "استخدام خوارزمية " + info.name + " غير آمنة";
                    finding.explanation = info.reason + "\n" +
                        "هذه الخوارزمية لم تعد تُعتبر آمنة في 2024.\n"
                        "يمكن للمهاجمين استغلال نقاط ضعفها.";
                    finding.unsafeCode = "شفر(بيانات, \"" + info.name + "\")";
                    finding.safeCode = "شفر(بيانات, \"" + info.alternative + "\")";
                    
                    findings.push_back(finding);
                }
            }
        }
    }
    
    /**
     * كشف مفاتيح ثابتة في الكود
     */
    void checkHardcodedKeys(
        const std::string& line,
        int lineNumber,
        const std::string& filePath,
        std::vector<CryptoFinding>& findings
    ) {
        // أنماط مفاتيح ثابتة
        std::vector<std::pair<std::regex, std::string>> patterns = {
            {std::regex(R"(مفتاح\s*=\s*["'][^"']{8,}["'])"), "مفتاح تشفير"},
            {std::regex(R"(key\s*=\s*["'][^"']{8,}["'])"), "encryption key"},
            {std::regex(R"(سر\s*=\s*["'][^"']+["'])"), "سر"},
            {std::regex(R"(secret\s*=\s*["'][^"']+["'])"), "secret"},
            {std::regex(R"(password\s*=\s*["'][^"']+["'])"), "password"},
            {std::regex(R"(كلمة_السر\s*=\s*["'][^"']+["'])"), "كلمة السر"},
            {std::regex(R"(api_key\s*=\s*["'][^"']+["'])"), "API key"},
            {std::regex(R"(token\s*=\s*["'][^"']+["'])"), "token"},
        };
        
        for (const auto& [pattern, keyType] : patterns) {
            std::smatch match;
            if (std::regex_search(line, match, pattern)) {
                // تجاهل التعليقات والثوابت الفارغة
                if (line.find("#") != std::string::npos && 
                    line.find("#") < line.find(match.str())) {
                    continue;
                }
                
                CryptoFinding finding;
                finding.type = CryptoIssueType::HardcodedKey;
                finding.location = {filePath, lineNumber, static_cast<int>(match.position()) + 1};
                finding.code = line;
                finding.severity = 9;
                finding.cweId = "CWE-798";
                finding.message = "مفتاح/سر ثابت في الكود: " + keyType;
                finding.explanation = 
                    "وضع المفاتيح والأسرار في الكود المصدري خطير جداً!\n"
                    "\n"
                    "المشاكل:\n"
                    "1. أي شخص يرى الكود يرى المفتاح\n"
                    "2. المفتاح يُحفظ في Git للأبد\n"
                    "3. صعوبة تغيير المفتاح لاحقاً\n"
                    "\n"
                    "الحل: استخدم متغيرات البيئة أو خزنة أسرار";
                finding.unsafeCode = "مفتاح = \"abc123xyz789\"";
                finding.safeCode = R"(# استخدم متغيرات البيئة
مفتاح = بيئة.احصل("ENCRYPTION_KEY")
إذا مفتاح.فارغ()
    خطأ("المفتاح غير موجود في البيئة")
نهاية)";
                
                findings.push_back(finding);
            }
        }
    }
    
    /**
     * كشف IV ثابت
     */
    void checkHardcodedIV(
        const std::string& line,
        int lineNumber,
        const std::string& filePath,
        std::vector<CryptoFinding>& findings
    ) {
        std::regex ivPattern(R"(\b(iv|IV|vector|متجه)\s*=\s*["'][^"']+["']|bytes\s*\([^)]*\d+[^)]*\))");
        std::smatch match;
        
        if (std::regex_search(line, match, ivPattern)) {
            CryptoFinding finding;
            finding.type = CryptoIssueType::HardcodedIV;
            finding.location = {filePath, lineNumber, static_cast<int>(match.position()) + 1};
            finding.code = line;
            finding.severity = 8;
            finding.cweId = "CWE-329";
            finding.message = "قيمة ابتدائية (IV) ثابتة";
            finding.explanation = 
                "IV (Initialization Vector) يجب أن يكون عشوائياً لكل تشفير.\n"
                "\n"
                "إذا كان ثابتاً:\n"
                "- نفس الرسالة ← نفس النص المشفر\n"
                "- المهاجم يمكنه كشف الأنماط\n"
                "- يضعف التشفير بشكل كبير";
            finding.unsafeCode = "iv = \"1234567890123456\"";
            finding.safeCode = R"(# IV عشوائي لكل عملية تشفير
iv = عشوائي.بايتات(16)
نص_مشفر = شفر(نص, مفتاح, iv))";
            
            findings.push_back(finding);
        }
    }
    
    /**
     * كشف عشوائية ضعيفة
     */
    void checkWeakRandomness(
        const std::string& line,
        int lineNumber,
        const std::string& filePath,
        std::vector<CryptoFinding>& findings
    ) {
        // دوال عشوائية ضعيفة
        std::vector<std::pair<std::regex, std::string>> patterns = {
            {std::regex(R"(\b(rand|عشوائي)\s*\(\s*\))"), "rand()"},
            {std::regex(R"(\bmath\.random\s*\()"), "Math.random()"},
            {std::regex(R"(\brandom\.random\s*\()"), "random.random()"},
            {std::regex(R"(\bsrand\s*\(\s*time)"), "srand(time())"},
        };
        
        for (const auto& [pattern, funcName] : patterns) {
            std::smatch match;
            if (std::regex_search(line, match, pattern)) {
                // تحقق إذا كان في سياق تشفير
                bool cryptoContext = 
                    line.find("مفتاح") != std::string::npos ||
                    line.find("key") != std::string::npos ||
                    line.find("تشفير") != std::string::npos ||
                    line.find("encrypt") != std::string::npos ||
                    line.find("iv") != std::string::npos ||
                    line.find("token") != std::string::npos ||
                    line.find("secret") != std::string::npos;
                
                if (cryptoContext) {
                    CryptoFinding finding;
                    finding.type = CryptoIssueType::WeakRandomness;
                    finding.location = {filePath, lineNumber, static_cast<int>(match.position()) + 1};
                    finding.code = line;
                    finding.severity = 8;
                    finding.cweId = "CWE-338";
                    finding.message = "استخدام " + funcName + " لتوليد قيم تشفير";
                    finding.explanation = 
                        "دوال العشوائية العادية ليست آمنة للتشفير!\n"
                        "\n"
                        "المشاكل:\n"
                        "- يمكن التنبؤ بالقيم التالية\n"
                        "- تعتمد على seed يمكن تخمينه\n"
                        "- ليست عشوائية حقاً\n"
                        "\n"
                        "للتشفير، استخدم مولد أرقام عشوائية آمن (CSPRNG)";
                    finding.unsafeCode = "مفتاح = عشوائي()";
                    finding.safeCode = R"(# استخدم عشوائية آمنة للتشفير
مفتاح = عشوائي_آمن.بايتات(32)  # 256 بت)";
                    
                    findings.push_back(finding);
                }
            }
        }
    }
    
    /**
     * كشف أوضاع تشفير غير آمنة
     */
    void checkInsecureModes(
        const std::string& line,
        int lineNumber,
        const std::string& filePath,
        std::vector<CryptoFinding>& findings
    ) {
        // وضع ECB غير آمن
        std::regex ecbPattern(R"(\bECB\b|mode\s*=\s*["']ECB["']|وضع\s*=\s*["']ECB["'])");
        std::smatch match;
        
        if (std::regex_search(line, match, ecbPattern)) {
            CryptoFinding finding;
            finding.type = CryptoIssueType::InsecureMode;
            finding.location = {filePath, lineNumber, static_cast<int>(match.position()) + 1};
            finding.code = line;
            finding.algorithm = "ECB";
            finding.alternative = "GCM, CBC, CTR";
            finding.severity = 9;
            finding.cweId = "CWE-327";
            finding.message = "استخدام وضع ECB غير الآمن";
            finding.explanation = 
                "وضع ECB (Electronic Codebook) يشفر كل كتلة بشكل مستقل.\n"
                "\n"
                "المشكلة: الكتل المتشابهة ← نص مشفر متشابه!\n"
                "\n"
                "مثال شهير: صورة البطريق\n"
                "- الصورة الأصلية واضحة بعد تشفير ECB\n"
                "- لأن الألوان المتشابهة تشفر بنفس الطريقة\n"
                "\n"
                "استخدم GCM (الأفضل) أو CBC أو CTR";
            finding.unsafeCode = "شفر(بيانات, مفتاح, وضع=\"ECB\")";
            finding.safeCode = R"(# استخدم GCM للتشفير مع التحقق
شفر_آمن(بيانات, مفتاح, وضع="GCM"))";
            
            findings.push_back(finding);
        }
    }
    
    /**
     * كشف مقارنة غير آمنة (timing attack)
     */
    void checkInsecureComparison(
        const std::string& line,
        int lineNumber,
        const std::string& filePath,
        std::vector<CryptoFinding>& findings
    ) {
        // مقارنة عادية لـ hash أو token
        std::regex compPattern(R"((تجزئة|hash|token|mac|hmac|توقيع|signature)\s*==)");
        std::smatch match;
        
        if (std::regex_search(line, match, compPattern)) {
            CryptoFinding finding;
            finding.type = CryptoIssueType::InsecureComparison;
            finding.location = {filePath, lineNumber, static_cast<int>(match.position()) + 1};
            finding.code = line;
            finding.severity = 7;
            finding.cweId = "CWE-208";
            finding.message = "مقارنة عادية لقيمة تشفيرية";
            finding.explanation = 
                "المقارنة العادية (==) تتوقف عند أول اختلاف.\n"
                "\n"
                "هجوم التوقيت (Timing Attack):\n"
                "- المهاجم يقيس وقت المقارنة\n"
                "- كلما طال الوقت، الأحرف الأولى صحيحة\n"
                "- يخمن حرفاً حرفاً!\n"
                "\n"
                "الحل: استخدم مقارنة ثابتة الوقت";
            finding.unsafeCode = "إذا تجزئة == تجزئة_متوقعة";
            finding.safeCode = R"(# مقارنة آمنة ثابتة الوقت
إذا قارن_آمن(تجزئة, تجزئة_متوقعة))";
            
            findings.push_back(finding);
        }
    }
};

// =============================================================================
// تقرير التشفير
// =============================================================================

class CryptoReport {
public:
    void addFindings(const std::vector<CryptoFinding>& findings) {
        findings_.insert(findings_.end(), findings.begin(), findings.end());
    }
    
    std::string generate() const {
        std::ostringstream ss;
        
        ss << "\n";
        ss << "╔══════════════════════════════════════════════════════════════════════╗\n";
        ss << "║                   🔐 تقرير سوء استخدام التشفير                        ║\n";
        ss << "║                     Cryptographic Misuse Report                        ║\n";
        ss << "╚══════════════════════════════════════════════════════════════════════╝\n";
        
        if (findings_.empty()) {
            ss << "\n✅ لم يُكتشف سوء استخدام للتشفير - ممتاز!\n";
            return ss.str();
        }
        
        // إحصائيات
        std::map<CryptoIssueType, int> issueCounts;
        int critical = 0, high = 0, medium = 0;
        
        for (const auto& f : findings_) {
            issueCounts[f.type]++;
            if (f.severity >= 9) critical++;
            else if (f.severity >= 7) high++;
            else medium++;
        }
        
        ss << "\n📊 الإحصائيات:\n";
        ss << "  • إجمالي المشاكل: " << findings_.size() << "\n";
        ss << "  • حرج (9-10): " << critical << "\n";
        ss << "  • عالي (7-8): " << high << "\n";
        ss << "  • متوسط (5-6): " << medium << "\n";
        ss << "\n  حسب النوع:\n";
        for (const auto& [type, count] : issueCounts) {
            ss << "  • " << cryptoIssueToArabic(type) << ": " << count << "\n";
        }
        
        // التفاصيل
        for (const auto& finding : findings_) {
            ss << finding.format();
        }
        
        // الخوارزميات الموصى بها
        ss << "\n";
        ss << "╔══════════════════════════════════════════════════════════════════════╗\n";
        ss << "║                  💡 الخوارزميات الموصى بها (2024)                     ║\n";
        ss << "╠══════════════════════════════════════════════════════════════════════╣\n";
        ss << "║  التشفير المتماثل:                                                    ║\n";
        ss << "║    • AES-256-GCM (الأفضل)                                             ║\n";
        ss << "║    • ChaCha20-Poly1305                                                ║\n";
        ss << "║                                                                       ║\n";
        ss << "║  التجزئة (Hashing):                                                   ║\n";
        ss << "║    • SHA-256, SHA-384, SHA-512                                        ║\n";
        ss << "║    • SHA-3                                                            ║\n";
        ss << "║    • BLAKE3                                                           ║\n";
        ss << "║                                                                       ║\n";
        ss << "║  كلمات السر:                                                          ║\n";
        ss << "║    • Argon2id (الأفضل)                                                ║\n";
        ss << "║    • bcrypt                                                           ║\n";
        ss << "║    • scrypt                                                           ║\n";
        ss << "║                                                                       ║\n";
        ss << "║  التوقيع الرقمي:                                                      ║\n";
        ss << "║    • Ed25519                                                          ║\n";
        ss << "║    • ECDSA (P-256 أو P-384)                                           ║\n";
        ss << "╚══════════════════════════════════════════════════════════════════════╝\n";
        
        return ss.str();
    }
    
    int getCount() const { return static_cast<int>(findings_.size()); }
    
private:
    std::vector<CryptoFinding> findings_;
};

} // namespace sad::security::rules

// =============================================================================
// واجهة C
// =============================================================================

extern "C" {

struct SadCryptoDetector {
    sad::security::rules::CryptoMisuseDetector* detector;
    sad::security::rules::CryptoReport* report;
};

SadCryptoDetector* sad_crypto_detector_new() {
    auto* ctx = new SadCryptoDetector();
    ctx->detector = new sad::security::rules::CryptoMisuseDetector();
    ctx->report = new sad::security::rules::CryptoReport();
    return ctx;
}

void sad_crypto_detector_free(SadCryptoDetector* ctx) {
    if (ctx) {
        delete ctx->detector;
        delete ctx->report;
        delete ctx;
    }
}

int sad_crypto_check_file(SadCryptoDetector* ctx, const char* content, const char* file) {
    if (!ctx) return -1;
    auto findings = ctx->detector->checkFile(content, file);
    ctx->report->addFindings(findings);
    return static_cast<int>(findings.size());
}

void sad_crypto_print_report(SadCryptoDetector* ctx) {
    if (!ctx) return;
    std::cout << ctx->report->generate();
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef CRYPTO_MISUSE_TEST

#include <cassert>

void testWeakAlgorithm() {
    sad::security::rules::CryptoMisuseDetector detector;
    
    std::string code = "تجزئة = MD5(كلمة_السر)";
    auto findings = detector.checkLine(code, 1, "test.ص");
    
    assert(findings.size() >= 1);
    assert(findings[0].type == sad::security::rules::CryptoIssueType::WeakAlgorithm);
    
    std::cout << "✅ اختبار كشف الخوارزمية الضعيفة نجح\n";
}

void testHardcodedKey() {
    sad::security::rules::CryptoMisuseDetector detector;
    
    std::string code = R"(مفتاح = "secret123456")";
    auto findings = detector.checkLine(code, 1, "test.ص");
    
    assert(findings.size() >= 1);
    assert(findings[0].type == sad::security::rules::CryptoIssueType::HardcodedKey);
    
    std::cout << "✅ اختبار كشف المفتاح الثابت نجح\n";
}

void testSafeCode() {
    sad::security::rules::CryptoMisuseDetector detector;
    
    std::string code = "تجزئة = SHA256(بيانات)";
    auto findings = detector.checkLine(code, 1, "test.ص");
    
    // SHA256 آمن
    bool hasWeakAlgo = false;
    for (const auto& f : findings) {
        if (f.type == sad::security::rules::CryptoIssueType::WeakAlgorithm) {
            hasWeakAlgo = true;
        }
    }
    assert(!hasWeakAlgo);
    
    std::cout << "✅ اختبار الكود الآمن نجح\n";
}

void testFullReport() {
    sad::security::rules::CryptoMisuseDetector detector;
    
    std::string code = R"(
دالة تسجيل_دخول(مستخدم: نص, كلمة_سر: نص)
    # مشاكل متعددة!
    مفتاح = "hardcoded_secret_key"
    تجزئة = MD5(كلمة_سر)
    iv = "1234567890123456"
    
    مشفر = شفر(بيانات, مفتاح, وضع="ECB")
    
    إذا تجزئة == تجزئة_مخزنة
        نجح()
    نهاية
نهاية
)";
    
    auto findings = detector.checkFile(code, "test.ص");
    
    sad::security::rules::CryptoReport report;
    report.addFindings(findings);
    std::cout << report.generate();
    
    assert(findings.size() >= 3);
    
    std::cout << "✅ اختبار التقرير الكامل نجح\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات كاشف سوء استخدام التشفير\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testWeakAlgorithm();
    testHardcodedKey();
    testSafeCode();
    testFullReport();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // CRYPTO_MISUSE_TEST
