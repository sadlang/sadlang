/**
 * ==========================================================
 * ملف: cloud_client.cpp
 * الوصف: عميل AI السحابي
 * المهمة: T300 - نظام ذكاء ص
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * 
 * ☁️ ما هو AI السحابي؟
 * ---------------------
 * بدلاً من تشغيل النموذج على جهازك، ترسل الطلب لخادم
 * قوي على الإنترنت يعالجه ويرسل النتيجة.
 * 
 * 🆚 متى تستخدم السحابي؟
 * ----------------------
 * ✅ للمهام المعقدة جداً
 * ✅ عندما تحتاج نموذج ضخم (70B+)
 * ✅ للحصول على أحدث النماذج
 * 
 * متى تستخدم المحلي؟
 * ------------------
 * ✅ للخصوصية (الكود لا يُرسل)
 * ✅ للعمل بدون إنترنت
 * ✅ للسرعة مع المهام البسيطة
 * 
 * 🔑 كيف يعمل؟
 * ------------
 * 1. إنشاء طلب HTTP POST
 * 2. إرفاق مفتاح API
 * 3. إرسال الـ prompt
 * 4. استلام النتيجة
 */

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <functional>
#include <memory>
#include <chrono>
#include <cstring>

namespace sad {
namespace ai {

// ==========================================================
// 📌 مزودو AI السحابي
// ==========================================================

/**
 * 🏢 مزود AI
 */
enum class AIProvider {
    OPENAI,         // OpenAI GPT
    ANTHROPIC,      // Anthropic Claude
    GOOGLE,         // Google Gemini
    MISTRAL,        // Mistral AI
    COHERE,         // Cohere
    LOCAL,          // محلي (للاختبار)
    CUSTOM          // مخصص
};

/**
 * 📝 أسماء المزودين
 */
const char* providerName(AIProvider provider) {
    switch (provider) {
        case AIProvider::OPENAI: return "OpenAI";
        case AIProvider::ANTHROPIC: return "Anthropic";
        case AIProvider::GOOGLE: return "Google";
        case AIProvider::MISTRAL: return "Mistral";
        case AIProvider::COHERE: return "Cohere";
        case AIProvider::LOCAL: return "محلي";
        case AIProvider::CUSTOM: return "مخصص";
        default: return "غير معروف";
    }
}

/**
 * 🌐 عناوين API
 */
const char* providerBaseUrl(AIProvider provider) {
    switch (provider) {
        case AIProvider::OPENAI: return "https://api.openai.com/v1";
        case AIProvider::ANTHROPIC: return "https://api.anthropic.com/v1";
        case AIProvider::GOOGLE: return "https://generativelanguage.googleapis.com/v1beta";
        case AIProvider::MISTRAL: return "https://api.mistral.ai/v1";
        case AIProvider::COHERE: return "https://api.cohere.ai/v1";
        case AIProvider::LOCAL: return "http://localhost:8080/v1";
        case AIProvider::CUSTOM: return "";
        default: return "";
    }
}

// ==========================================================
// 📌 إعدادات العميل
// ==========================================================

/**
 * ⚙️ إعدادات العميل السحابي
 */
struct CloudClientConfig {
    AIProvider provider = AIProvider::OPENAI;
    std::string apiKey = "";
    std::string baseUrl = "";
    std::string model = "";
    int timeoutSeconds = 30;
    int maxRetries = 3;
    bool stream = false;
    
    /**
     * إعدادات OpenAI الافتراضية
     */
    static CloudClientConfig openAI(const std::string& apiKey) {
        CloudClientConfig config;
        config.provider = AIProvider::OPENAI;
        config.apiKey = apiKey;
        config.baseUrl = providerBaseUrl(AIProvider::OPENAI);
        config.model = "gpt-4o-mini";
        return config;
    }
    
    /**
     * إعدادات Anthropic الافتراضية
     */
    static CloudClientConfig anthropic(const std::string& apiKey) {
        CloudClientConfig config;
        config.provider = AIProvider::ANTHROPIC;
        config.apiKey = apiKey;
        config.baseUrl = providerBaseUrl(AIProvider::ANTHROPIC);
        config.model = "claude-3-haiku-20240307";
        return config;
    }
    
    /**
     * إعدادات المحلي
     */
    static CloudClientConfig local() {
        CloudClientConfig config;
        config.provider = AIProvider::LOCAL;
        config.baseUrl = providerBaseUrl(AIProvider::LOCAL);
        config.model = "local";
        return config;
    }
};

// ==========================================================
// 📌 طلب AI
// ==========================================================

/**
 * 📨 طلب للـ AI
 */
struct AIRequest {
    std::string prompt;                     // الطلب
    std::string systemPrompt = "";          // تعليمات النظام
    int maxTokens = 1000;                   // أقصى عدد tokens
    float temperature = 0.7f;               // درجة الإبداع
    float topP = 1.0f;                      // Top-p sampling
    std::vector<std::string> stopSequences; // متى يتوقف
    
    /**
     * طلب لتوليد كود
     */
    static AIRequest codeGeneration(const std::string& prompt) {
        AIRequest req;
        req.prompt = prompt;
        req.systemPrompt = R"(أنت مساعد برمجة خبير في لغة ص (Sad Language).
لغة ص هي لغة برمجة عربية حديثة بنظام ملكية مشابه لـ Rust.

قواعد مهمة:
1. الكتل تُنهى بـ "نهاية" وليس {}
2. الدالة تُعرف بـ "دالة" والإرجاع بـ "أرجع"
3. الشرط بـ "إذا...وإلا...نهاية"
4. الحلقة بـ "لكل...في...نهاية" أو "بينما...نهاية"
5. أنواع البيانات: عدد، عشري، نص، منطقي، قائمة، قاموس

اكتب كود ص صحيح ومُعلَّق بالعربية.)";
        req.temperature = 0.3f; // أقل إبداعية للكود
        req.maxTokens = 2000;
        return req;
    }
    
    /**
     * طلب لشرح كود
     */
    static AIRequest codeExplanation(const std::string& code) {
        AIRequest req;
        req.prompt = "اشرح هذا الكود بالتفصيل للمبتدئين:\n\n" + code;
        req.systemPrompt = "أنت معلم برمجة صبور يشرح للمبتدئين الذين لا يعرفون شيئاً عن البرمجة.";
        req.temperature = 0.5f;
        req.maxTokens = 1500;
        return req;
    }
};

// ==========================================================
// 📌 استجابة AI
// ==========================================================

/**
 * 📬 استجابة من AI
 */
struct AIResponse {
    bool success;
    std::string content;            // المحتوى المُولَّد
    std::string error;              // رسالة الخطأ (إن وجد)
    int tokensUsed = 0;             // عدد tokens المستخدمة
    double latencySeconds = 0;      // زمن الاستجابة
    std::string model;              // النموذج المستخدم
    
    /**
     * إنشاء استجابة ناجحة
     */
    static AIResponse ok(const std::string& content, int tokens, 
                         double latency, const std::string& model) {
        AIResponse resp;
        resp.success = true;
        resp.content = content;
        resp.tokensUsed = tokens;
        resp.latencySeconds = latency;
        resp.model = model;
        return resp;
    }
    
    /**
     * إنشاء استجابة فاشلة
     */
    static AIResponse fail(const std::string& error) {
        AIResponse resp;
        resp.success = false;
        resp.error = error;
        return resp;
    }
    
    /**
     * طباعة الاستجابة
     */
    std::string toString() const {
        std::ostringstream oss;
        if (success) {
            oss << "✅ نجح الطلب\n";
            oss << "النموذج: " << model << "\n";
            oss << "Tokens: " << tokensUsed << "\n";
            oss << "الزمن: " << latencySeconds << " ثانية\n";
            oss << "المحتوى:\n" << content << "\n";
        } else {
            oss << "❌ فشل الطلب: " << error << "\n";
        }
        return oss.str();
    }
};

// ==========================================================
// 📌 محاكاة HTTP (مبسطة)
// ==========================================================

/**
 * 🌐 محاكاة HTTP Client
 * 
 * ملاحظة: في التنفيذ الحقيقي، نستخدم مكتبة مثل libcurl
 */
class SimpleHttpClient {
public:
    struct Response {
        int statusCode = 0;
        std::string body = "";
        std::string error = "";
    };
    
    /**
     * إرسال طلب POST
     */
    static Response post(const std::string& url, 
                        const std::string& body,
                        const std::map<std::string, std::string>& headers,
                        int timeoutSeconds) {
        Response resp;
        
        // محاكاة - في الواقع نستخدم libcurl
        // هنا نرجع استجابة وهمية للاختبار
        
        if (url.find("localhost") != std::string::npos) {
            // استجابة محلية للاختبار
            resp.statusCode = 200;
            resp.body = R"({
                "choices": [{
                    "message": {
                        "content": "دالة مرحبا()\n    اطبع(\"مرحباً بالعالم!\")\nنهاية"
                    }
                }],
                "usage": {
                    "total_tokens": 50
                },
                "model": "local-test"
            })";
        } else {
            // محاكاة فشل (لا يوجد اتصال حقيقي)
            resp.statusCode = 0;
            resp.error = "لا يوجد اتصال - استخدم النموذج المحلي";
        }
        
        return resp;
    }
};

// ==========================================================
// 📌 محلل JSON (مبسط)
// ==========================================================

/**
 * 📄 محلل JSON بسيط
 */
class SimpleJsonParser {
public:
    /**
     * استخراج قيمة من JSON
     */
    static std::string extractString(const std::string& json, 
                                      const std::string& key) {
        std::string searchKey = "\"" + key + "\":";
        size_t pos = json.find(searchKey);
        if (pos == std::string::npos) return "";
        
        pos += searchKey.length();
        
        // تخطي المسافات
        while (pos < json.length() && (json[pos] == ' ' || json[pos] == '\n')) {
            pos++;
        }
        
        // هل القيمة string؟
        if (json[pos] == '"') {
            pos++; // تخطي "
            size_t end = json.find('"', pos);
            // التعامل مع escaped quotes
            while (end != std::string::npos && end > 0 && json[end - 1] == '\\') {
                end = json.find('"', end + 1);
            }
            if (end != std::string::npos) {
                return json.substr(pos, end - pos);
            }
        }
        
        return "";
    }
    
    /**
     * استخراج رقم من JSON
     */
    static int extractInt(const std::string& json, const std::string& key) {
        std::string searchKey = "\"" + key + "\":";
        size_t pos = json.find(searchKey);
        if (pos == std::string::npos) return 0;
        
        pos += searchKey.length();
        
        // تخطي المسافات
        while (pos < json.length() && (json[pos] == ' ' || json[pos] == '\n')) {
            pos++;
        }
        
        // قراءة الرقم
        std::string num;
        while (pos < json.length() && (std::isdigit(json[pos]) || json[pos] == '-')) {
            num += json[pos++];
        }
        
        return num.empty() ? 0 : std::stoi(num);
    }
    
    /**
     * استخراج محتوى من استجابة OpenAI
     */
    static std::string extractOpenAIContent(const std::string& json) {
        // البحث عن "content":
        size_t pos = json.find("\"content\":");
        if (pos == std::string::npos) return "";
        
        pos += 10; // طول "\"content\":"
        
        // تخطي المسافات
        while (pos < json.length() && (json[pos] == ' ' || json[pos] == '\n')) {
            pos++;
        }
        
        if (json[pos] != '"') return "";
        pos++; // تخطي "
        
        // جمع المحتوى حتى " غير مهربة
        std::string content;
        while (pos < json.length()) {
            if (json[pos] == '"' && (content.empty() || content.back() != '\\')) {
                break;
            }
            content += json[pos++];
        }
        
        // فك الهروب
        std::string result;
        for (size_t i = 0; i < content.length(); ++i) {
            if (content[i] == '\\' && i + 1 < content.length()) {
                switch (content[i + 1]) {
                    case 'n': result += '\n'; i++; break;
                    case 't': result += '\t'; i++; break;
                    case '"': result += '"'; i++; break;
                    case '\\': result += '\\'; i++; break;
                    default: result += content[i]; break;
                }
            } else {
                result += content[i];
            }
        }
        
        return result;
    }
};

// ==========================================================
// 📌 عميل AI السحابي
// ==========================================================

/**
 * ☁️ عميل AI السحابي
 */
class CloudAIClient {
private:
    CloudClientConfig config_;
    
    /**
     * بناء الـ headers
     */
    std::map<std::string, std::string> buildHeaders() {
        std::map<std::string, std::string> headers;
        headers["Content-Type"] = "application/json";
        
        switch (config_.provider) {
            case AIProvider::OPENAI:
                headers["Authorization"] = "Bearer " + config_.apiKey;
                break;
            case AIProvider::ANTHROPIC:
                headers["x-api-key"] = config_.apiKey;
                headers["anthropic-version"] = "2024-01-01";
                break;
            case AIProvider::GOOGLE:
                // API key في URL
                break;
            default:
                if (!config_.apiKey.empty()) {
                    headers["Authorization"] = "Bearer " + config_.apiKey;
                }
                break;
        }
        
        return headers;
    }
    
    /**
     * بناء جسم الطلب
     */
    std::string buildRequestBody(const AIRequest& request) {
        std::ostringstream oss;
        
        switch (config_.provider) {
            case AIProvider::OPENAI:
            case AIProvider::MISTRAL:
            case AIProvider::LOCAL:
                oss << "{\n";
                oss << "  \"model\": \"" << config_.model << "\",\n";
                oss << "  \"messages\": [\n";
                if (!request.systemPrompt.empty()) {
                    oss << "    {\"role\": \"system\", \"content\": \"" 
                        << escapeJson(request.systemPrompt) << "\"},\n";
                }
                oss << "    {\"role\": \"user\", \"content\": \"" 
                    << escapeJson(request.prompt) << "\"}\n";
                oss << "  ],\n";
                oss << "  \"max_tokens\": " << request.maxTokens << ",\n";
                oss << "  \"temperature\": " << request.temperature << "\n";
                oss << "}\n";
                break;
                
            case AIProvider::ANTHROPIC:
                oss << "{\n";
                oss << "  \"model\": \"" << config_.model << "\",\n";
                oss << "  \"max_tokens\": " << request.maxTokens << ",\n";
                if (!request.systemPrompt.empty()) {
                    oss << "  \"system\": \"" << escapeJson(request.systemPrompt) << "\",\n";
                }
                oss << "  \"messages\": [\n";
                oss << "    {\"role\": \"user\", \"content\": \"" 
                    << escapeJson(request.prompt) << "\"}\n";
                oss << "  ]\n";
                oss << "}\n";
                break;
                
            default:
                oss << "{\"prompt\": \"" << escapeJson(request.prompt) << "\"}\n";
                break;
        }
        
        return oss.str();
    }
    
    /**
     * تهريب JSON
     */
    std::string escapeJson(const std::string& str) {
        std::string result;
        for (char c : str) {
            switch (c) {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\n': result += "\\n"; break;
                case '\t': result += "\\t"; break;
                case '\r': result += "\\r"; break;
                default: result += c; break;
            }
        }
        return result;
    }
    
    /**
     * بناء URL
     */
    std::string buildUrl() {
        std::string url = config_.baseUrl;
        
        switch (config_.provider) {
            case AIProvider::OPENAI:
            case AIProvider::MISTRAL:
            case AIProvider::LOCAL:
                url += "/chat/completions";
                break;
            case AIProvider::ANTHROPIC:
                url += "/messages";
                break;
            case AIProvider::GOOGLE:
                url += "/models/" + config_.model + ":generateContent";
                url += "?key=" + config_.apiKey;
                break;
            default:
                url += "/generate";
                break;
        }
        
        return url;
    }
    
    /**
     * تحليل الاستجابة
     */
    AIResponse parseResponse(const SimpleHttpClient::Response& httpResp,
                             double latency) {
        if (httpResp.statusCode == 0) {
            return AIResponse::fail(httpResp.error);
        }
        
        if (httpResp.statusCode != 200) {
            return AIResponse::fail("خطأ HTTP: " + std::to_string(httpResp.statusCode));
        }
        
        // استخراج المحتوى
        std::string content = SimpleJsonParser::extractOpenAIContent(httpResp.body);
        int tokens = SimpleJsonParser::extractInt(httpResp.body, "total_tokens");
        std::string model = SimpleJsonParser::extractString(httpResp.body, "model");
        
        if (content.empty()) {
            return AIResponse::fail("فشل تحليل الاستجابة");
        }
        
        return AIResponse::ok(content, tokens, latency, model);
    }
    
public:
    CloudAIClient() {}
    
    explicit CloudAIClient(const CloudClientConfig& config) 
        : config_(config) {}
    
    /**
     * تعيين الإعدادات
     */
    void setConfig(const CloudClientConfig& config) {
        config_ = config;
    }
    
    /**
     * الحصول على الإعدادات
     */
    const CloudClientConfig& getConfig() const {
        return config_;
    }
    
    /**
     * إرسال طلب
     */
    AIResponse send(const AIRequest& request) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // بناء الطلب
        std::string url = buildUrl();
        std::string body = buildRequestBody(request);
        auto headers = buildHeaders();
        
        // إرسال الطلب
        auto httpResp = SimpleHttpClient::post(url, body, headers, 
                                               config_.timeoutSeconds);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double latency = std::chrono::duration<double>(endTime - startTime).count();
        
        // تحليل الاستجابة
        return parseResponse(httpResp, latency);
    }
    
    /**
     * توليد كود
     */
    AIResponse generateCode(const std::string& prompt) {
        return send(AIRequest::codeGeneration(prompt));
    }
    
    /**
     * شرح كود
     */
    AIResponse explainCode(const std::string& code) {
        return send(AIRequest::codeExplanation(code));
    }
    
    /**
     * اختبار الاتصال
     */
    bool testConnection() {
        AIRequest testReq;
        testReq.prompt = "قل 'مرحبا' فقط";
        testReq.maxTokens = 10;
        
        auto resp = send(testReq);
        return resp.success;
    }
};

// ==========================================================
// 📌 مدير مزودي AI
// ==========================================================

/**
 * 🎛️ مدير مزودي AI
 */
class AIProviderManager {
private:
    std::map<AIProvider, CloudAIClient> clients_;
    AIProvider currentProvider_ = AIProvider::LOCAL;
    
public:
    /**
     * إضافة مزود
     */
    void addProvider(AIProvider provider, const CloudClientConfig& config) {
        clients_[provider] = CloudAIClient(config);
    }
    
    /**
     * تعيين المزود الحالي
     */
    void setCurrentProvider(AIProvider provider) {
        currentProvider_ = provider;
    }
    
    /**
     * الحصول على العميل الحالي
     */
    CloudAIClient* getCurrentClient() {
        auto it = clients_.find(currentProvider_);
        if (it != clients_.end()) {
            return &it->second;
        }
        return nullptr;
    }
    
    /**
     * توليد كود من أفضل مزود متاح
     */
    AIResponse generateWithFallback(const std::string& prompt) {
        // محاولة المزود الحالي
        auto* client = getCurrentClient();
        if (client) {
            auto resp = client->generateCode(prompt);
            if (resp.success) return resp;
        }
        
        // محاولة المزودين الآخرين
        for (auto& [provider, client] : clients_) {
            if (provider != currentProvider_) {
                auto resp = client.generateCode(prompt);
                if (resp.success) {
                    return resp;
                }
            }
        }
        
        return AIResponse::fail("لم يتوفر أي مزود AI");
    }
};

} // namespace ai
} // namespace sad

// ==========================================================
// 📌 C API
// ==========================================================

extern "C" {

/**
 * إنشاء عميل سحابي
 */
void* sad_cloud_client_new(int provider, const char* apiKey) {
    sad::ai::CloudClientConfig config;
    config.provider = static_cast<sad::ai::AIProvider>(provider);
    config.apiKey = apiKey ? apiKey : "";
    config.baseUrl = sad::ai::providerBaseUrl(config.provider);
    
    return new sad::ai::CloudAIClient(config);
}

/**
 * تحرير العميل
 */
void sad_cloud_client_free(void* client) {
    delete static_cast<sad::ai::CloudAIClient*>(client);
}

/**
 * توليد كود
 */
char* sad_cloud_generate_code(void* client, const char* prompt) {
    auto* c = static_cast<sad::ai::CloudAIClient*>(client);
    auto resp = c->generateCode(prompt);
    
    std::string result = resp.success ? resp.content : ("خطأ: " + resp.error);
    char* output = new char[result.size() + 1];
    std::strcpy(output, result.c_str());
    return output;
}

/**
 * شرح كود
 */
char* sad_cloud_explain_code(void* client, const char* code) {
    auto* c = static_cast<sad::ai::CloudAIClient*>(client);
    auto resp = c->explainCode(code);
    
    std::string result = resp.success ? resp.content : ("خطأ: " + resp.error);
    char* output = new char[result.size() + 1];
    std::strcpy(output, result.c_str());
    return output;
}

/**
 * اختبار الاتصال
 */
int sad_cloud_test_connection(void* client) {
    return static_cast<sad::ai::CloudAIClient*>(client)->testConnection() ? 1 : 0;
}

/**
 * تحرير نص
 */
void sad_cloud_free_string(char* str) {
    delete[] str;
}

} // extern "C"
