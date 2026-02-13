// ملف: arabic.cpp
// الوصف: محلل رموز BPE واعٍ بالصرف العربي مع تطبيع التشكيل
// المشروع: سيادة - محرك الذكاء الاصطناعي للغة صاد
// المرحلة: 54 - الذكاء الاصطناعي

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <utility>
#include <sstream>

namespace sad::ai {

// نطاقات يونيكود للحروف العربية والتشكيل
struct ArabicRange {
    uint32_t start;
    uint32_t end;
    const char* name;
};

static const ArabicRange TASHKEEL_RANGE = {0x064B, 0x065F, "تشكيل"};
static const ArabicRange ARABIC_LETTERS = {0x0621, 0x064A, "حروف عربية"};
static const ArabicRange ARABIC_NUMS = {0x0660, 0x0669, "أرقام عربية"};

class ArabicBPETokenizer {
    std::unordered_map<std::string, int> vocab_;
    std::unordered_map<int, std::string> id_to_token_;
    std::vector<std::pair<std::string, std::string>> merges_;
    int vocab_size_;
    int next_id_;
    bool normalize_tashkeel_;

public:
    ArabicBPETokenizer(int vocab_size = 32000, bool normalize_tashkeel = true)
        : vocab_size_(vocab_size), next_id_(0), normalize_tashkeel_(normalize_tashkeel) {
        add_special_tokens();
    }

    // إضافة الرموز الخاصة
    void add_special_tokens() {
        add_token("<pad>"); add_token("<unk>"); add_token("<bos>");
        add_token("<eos>"); add_token("<mask>"); add_token("<sep>");
    }

    void add_token(const std::string& token) {
        if (vocab_.find(token) == vocab_.end()) {
            vocab_[token] = next_id_;
            id_to_token_[next_id_] = token;
            next_id_++;
        }
    }

    // إزالة التشكيل من النص العربي للتطبيع
    std::string remove_tashkeel(const std::string& text) {
        std::string result;
        for (size_t i = 0; i < text.size(); ++i) {
            unsigned char c = text[i];
            if ((c & 0xC0) == 0xC0) {
                size_t len = (c & 0xE0) == 0xC0 ? 2 : (c & 0xF0) == 0xE0 ? 3 : 4;
                uint32_t cp = 0;
                if (len == 2) cp = (c & 0x1F) << 6 | (text[i+1] & 0x3F);
                else if (len == 3) cp = (c & 0x0F) << 12 | (text[i+1] & 0x3F) << 6 | (text[i+2] & 0x3F);
                else cp = (c & 0x07) << 18 | (text[i+1] & 0x3F) << 12 | (text[i+2] & 0x3F) << 6 | (text[i+3] & 0x3F);
                if (cp < TASHKEEL_RANGE.start || cp > TASHKEEL_RANGE.end) {
                    result.append(text, i, len);
                }
                i += len - 1;
            } else {
                result += c;
            }
        }
        return result;
    }

    // تقسيم مبدئي بناءً على الصرف العربي
    std::vector<std::string> morphology_split(const std::string& word) {
        std::vector<std::string> parts;
        // فصل البادئات الشائعة: ال، و، ف، ب، ك، ل
        static const std::vector<std::string> prefixes = {
            "\xd8\xa7\xd9\x84", "\xd9\x88", "\xd9\x81", "\xd8\xa8", "\xd9\x83", "\xd9\x84"
        };
        std::string remaining = word;
        for (const auto& prefix : prefixes) {
            if (remaining.size() > prefix.size() && remaining.substr(0, prefix.size()) == prefix) {
                parts.push_back(prefix);
                remaining = remaining.substr(prefix.size());
                break;
            }
        }
        if (!remaining.empty()) parts.push_back(remaining);
        return parts;
    }

    // ترميز النص إلى معرّفات الرموز
    std::vector<int> encode(const std::string& text) {
        std::string processed = normalize_tashkeel_ ? remove_tashkeel(text) : text;
        std::vector<int> ids;
        ids.push_back(vocab_["<bos>"]);
        std::istringstream stream(processed);
        std::string word;
        while (stream >> word) {
            auto parts = morphology_split(word);
            for (const auto& part : parts) {
                auto it = vocab_.find(part);
                ids.push_back(it != vocab_.end() ? it->second : vocab_["<unk>"]);
            }
        }
        ids.push_back(vocab_["<eos>"]);
        return ids;
    }

    // فك ترميز المعرّفات إلى نص
    std::string decode(const std::vector<int>& ids) {
        std::string result;
        for (int id : ids) {
            auto it = id_to_token_.find(id);
            if (it != id_to_token_.end() && it->second[0] != '<') {
                if (!result.empty()) result += " ";
                result += it->second;
            }
        }
        return result;
    }

    int get_vocab_size() const { return next_id_; }
    bool has_token(const std::string& t) const { return vocab_.count(t) > 0; }
};

} // namespace sad::ai
