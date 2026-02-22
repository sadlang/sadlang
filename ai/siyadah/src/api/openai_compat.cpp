// ملف: openai_compat.cpp
// الوصف: واجهة REST متوافقة مع OpenAI: نقاط /chat/completions و /models
// المشروع: سيادة - محرك الذكاء الاصطناعي للغة صاد
// المرحلة: 54 - الذكاء الاصطناعي

#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <functional>
#include <chrono>
#include <cstdint>

namespace sad::ai {

struct ChatMessage {
    std::string role;     // system, user, assistant
    std::string content;
};

struct ChatCompletionRequest {
    std::string model;
    std::vector<ChatMessage> messages;
    float temperature = 1.0f;
    float top_p = 1.0f;
    int max_tokens = 256;
    bool stream = false;
    std::vector<std::string> stop;
    float presence_penalty = 0.0f;
    float frequency_penalty = 0.0f;
};

struct ChatChoice {
    int index;
    ChatMessage message;
    std::string finish_reason;  // stop, length, content_filter
};

struct UsageInfo {
    int prompt_tokens = 0;
    int completion_tokens = 0;
    int total_tokens = 0;
};

struct ChatCompletionResponse {
    std::string id;
    std::string object = "chat.completion";
    int64_t created = 0;
    std::string model;
    std::vector<ChatChoice> choices;
    UsageInfo usage;
};

struct ModelInfo {
    std::string id;
    std::string object = "model";
    int64_t created = 0;
    std::string owned_by = "sad-ai";
};

using GenerateFn = std::function<std::string(const std::vector<ChatMessage>&, float temp, int max_tok)>;

class OpenAICompatAPI {
    std::unordered_map<std::string, ModelInfo> models_;
    GenerateFn generate_fn_;
    int64_t request_counter_;

public:
    OpenAICompatAPI() : request_counter_(0) {}

    // تسجيل نموذج متاح
    void register_model(const std::string& model_id, const std::string& owner = "sad-ai") {
        ModelInfo info;
        info.id = model_id;
        info.owned_by = owner;
        info.created = current_timestamp();
        models_[model_id] = info;
    }

    void set_generate_fn(GenerateFn fn) { generate_fn_ = std::move(fn); }

    // معالجة طلب /chat/completions
    ChatCompletionResponse handle_chat_completion(const ChatCompletionRequest& req) {
        ChatCompletionResponse resp;
        resp.id = "chatcmpl-" + std::to_string(++request_counter_);
        resp.created = current_timestamp();
        resp.model = req.model;

        std::string generated;
        if (generate_fn_) {
            generated = generate_fn_(req.messages, req.temperature, req.max_tokens);
        } else {
            generated = "[سيادة] لم يتم تحميل نموذج بعد";
        }

        ChatChoice choice;
        choice.index = 0;
        choice.message = {"assistant", generated};
        choice.finish_reason = "stop";
        resp.choices.push_back(choice);

        resp.usage.prompt_tokens = count_tokens(req.messages);
        resp.usage.completion_tokens = estimate_tokens(generated);
        resp.usage.total_tokens = resp.usage.prompt_tokens + resp.usage.completion_tokens;

        return resp;
    }

    // معالجة طلب /models
    std::vector<ModelInfo> handle_list_models() {
        std::vector<ModelInfo> result;
        for (auto& [id, info] : models_) result.push_back(info);
        return result;
    }

    // معالجة طلب /models/{model_id}
    ModelInfo handle_get_model(const std::string& model_id) {
        auto it = models_.find(model_id);
        if (it != models_.end()) return it->second;
        return ModelInfo{"not_found", "error", 0, ""};
    }

    // تحويل الاستجابة إلى JSON
    std::string to_json(const ChatCompletionResponse& resp) {
        std::ostringstream ss;
        ss << "{\"id\":\"" << resp.id << "\","
           << "\"object\":\"" << resp.object << "\","
           << "\"created\":" << resp.created << ","
           << "\"model\":\"" << resp.model << "\","
           << "\"choices\":[";
        for (size_t i = 0; i < resp.choices.size(); ++i) {
            auto& c = resp.choices[i];
            if (i > 0) ss << ",";
            ss << "{\"index\":" << c.index
               << ",\"message\":{\"role\":\"" << c.message.role
               << "\",\"content\":\"" << escape_json(c.message.content) << "\"}"
               << ",\"finish_reason\":\"" << c.finish_reason << "\"}";
        }
        ss << "],\"usage\":{\"prompt_tokens\":" << resp.usage.prompt_tokens
           << ",\"completion_tokens\":" << resp.usage.completion_tokens
           << ",\"total_tokens\":" << resp.usage.total_tokens << "}}";
        return ss.str();
    }

private:
    int64_t current_timestamp() {
        return std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    }

    int count_tokens(const std::vector<ChatMessage>& msgs) {
        int count = 0;
        for (auto& m : msgs) count += static_cast<int>(m.content.size() / 4) + 4;
        return count;
    }

    int estimate_tokens(const std::string& text) {
        return static_cast<int>(text.size() / 4) + 1;
    }

    std::string escape_json(const std::string& s) {
        std::string result;
        for (char c : s) {
            if (c == '"') result += "\\\"";
            else if (c == '\\') result += "\\\\";
            else if (c == '\n') result += "\\n";
            else result += c;
        }
        return result;
    }
};

} // namespace sad::ai
