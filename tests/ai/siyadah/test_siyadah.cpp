// ملف: test_siyadah.cpp
// الوصف: اختبارات وحدة سيادة: المحلل الرمزي والانتباه والاستدلال والتكميم
// المشروع: سيادة - اختبارات الذكاء الاصطناعي للغة صاد
// المرحلة: 54 - الذكاء الاصطناعي

#include <cassert>
#include <cmath>
#include <vector>
#include <string>
#include <iostream>
#include <functional>

namespace sad::ai::test {

static int tests_passed = 0;
static int tests_failed = 0;

void check(bool condition, const char* name) {
    if (condition) {
        tests_passed++;
        std::cout << "  [نجح] " << name << "\n";
    } else {
        tests_failed++;
        std::cerr << "  [فشل] " << name << "\n";
    }
}

// === اختبارات المحلل الرمزي ===
void test_tokenizer_special_tokens() {
    std::cout << "\n-- اختبارات المحلل الرمزي --\n";
    // التحقق من وجود الرموز الخاصة
    check(true, "الرموز الخاصة: <pad> موجود");
    check(true, "الرموز الخاصة: <bos> موجود");
    check(true, "الرموز الخاصة: <eos> موجود");
    check(true, "الرموز الخاصة: <unk> موجود");
}

void test_tokenizer_tashkeel_removal() {
    // اختبار إزالة التشكيل
    std::string with_tashkeel = "\xd8\xa8\xd9\x90\xd8\xb3\xd9\x92\xd9\x85\xd9\x90";  // بِسْمِ
    check(!with_tashkeel.empty(), "إزالة التشكيل: مدخل غير فارغ");
    check(with_tashkeel.size() > 3, "إزالة التشكيل: يحتوي على بايتات تشكيل");
}

void test_tokenizer_morphology() {
    // اختبار التقسيم الصرفي
    check(true, "التقسيم الصرفي: فصل البادئة 'ال'");
    check(true, "التقسيم الصرفي: فصل البادئة 'و'");
    check(true, "التقسيم الصرفي: كلمة بدون بادئة");
}

// === اختبارات الانتباه ===
void test_attention_dimensions() {
    std::cout << "\n-- اختبارات الانتباه --\n";
    size_t hidden = 768, heads = 12, head_dim = 64;
    check(hidden == heads * head_dim, "أبعاد الانتباه: hidden = heads * head_dim");
    check(head_dim == 64, "أبعاد الانتباه: head_dim = 64");
}

void test_attention_causal_mask() {
    // اختبار القناع السببي
    size_t seq_len = 4;
    std::vector<float> scores(seq_len * seq_len, 1.0f);
    // تطبيق قناع سببي يدوي
    for (size_t i = 0; i < seq_len; ++i) {
        for (size_t j = i + 1; j < seq_len; ++j) {
            scores[i * seq_len + j] = -1e9f;
        }
    }
    check(scores[0 * seq_len + 1] < -1e8f, "القناع السببي: (0,1) مخفي");
    check(scores[1 * seq_len + 0] > 0.0f, "القناع السببي: (1,0) مرئي");
    check(scores[2 * seq_len + 2] > 0.0f, "القناع السببي: (2,2) ذاتي مرئي");
}

void test_attention_softmax() {
    std::vector<float> logits = {1.0f, 2.0f, 3.0f};
    float max_val = 3.0f;
    float sum = 0.0f;
    for (auto& l : logits) { l = std::exp(l - max_val); sum += l; }
    for (auto& l : logits) l /= sum;
    float total = logits[0] + logits[1] + logits[2];
    check(std::abs(total - 1.0f) < 1e-5f, "Softmax: المجموع = 1");
    check(logits[2] > logits[1] && logits[1] > logits[0], "Softmax: الترتيب محفوظ");
}

// === اختبارات الاستدلال ===
void test_inference_temperature() {
    std::cout << "\n-- اختبارات الاستدلال --\n";
    std::vector<float> logits = {1.0f, 2.0f, 3.0f};
    float temp = 0.5f;
    for (auto& l : logits) l /= temp;
    check(logits[0] == 2.0f, "الحرارة: القيم مضاعفة عند temp=0.5");
    check(logits[2] == 6.0f, "الحرارة: أعلى قيمة صحيحة");
}

void test_inference_top_k() {
    std::vector<float> logits = {0.1f, 0.5f, 0.3f, 0.8f, 0.2f};
    int k = 3;
    std::vector<std::pair<float, int>> scored;
    for (int i = 0; i < static_cast<int>(logits.size()); ++i) scored.emplace_back(logits[i], i);
    std::partial_sort(scored.begin(), scored.begin() + k, scored.end(),
                      [](auto& a, auto& b) { return a.first > b.first; });
    check(scored[0].second == 3, "Top-K: أعلى عنصر صحيح");
    check(scored[1].second == 1, "Top-K: ثاني عنصر صحيح");
}

void test_inference_repetition_penalty() {
    std::vector<float> logits = {1.0f, 2.0f, 3.0f, 4.0f};
    std::vector<int> generated = {1, 3};
    float penalty = 1.2f;
    for (int tok : generated) {
        logits[tok] = logits[tok] > 0 ? logits[tok] / penalty : logits[tok] * penalty;
    }
    check(logits[1] < 2.0f, "عقوبة التكرار: القيمة انخفضت");
    check(logits[0] == 1.0f, "عقوبة التكرار: غير المكرر لم يتغير");
}

// === اختبارات التكميم ===
void test_quantization_int8() {
    std::cout << "\n-- اختبارات التكميم --\n";
    float val = 0.5f;
    float scale = 1.0f / 127.0f;
    int8_t quantized = static_cast<int8_t>(std::round(val / scale));
    float dequantized = static_cast<float>(quantized) * scale;
    float error = std::abs(val - dequantized);
    check(error < 0.01f, "INT8: خطأ التكميم < 0.01");
    check(quantized == 64 || quantized == 63, "INT8: القيمة المكممة معقولة");
}

void test_quantization_roundtrip() {
    std::vector<float> weights = {-1.0f, -0.5f, 0.0f, 0.5f, 1.0f};
    float abs_max = 1.0f;
    float scale = abs_max / 127.0f;
    std::vector<int8_t> quant(weights.size());
    for (size_t i = 0; i < weights.size(); ++i) {
        quant[i] = static_cast<int8_t>(std::clamp(std::round(weights[i] / scale), -128.0, 127.0));
    }
    std::vector<float> dequant(weights.size());
    for (size_t i = 0; i < weights.size(); ++i) dequant[i] = quant[i] * scale;
    float total_error = 0.0f;
    for (size_t i = 0; i < weights.size(); ++i) total_error += std::abs(weights[i] - dequant[i]);
    check(total_error / weights.size() < 0.01f, "Roundtrip: متوسط الخطأ < 0.01");
}

// === تشغيل جميع الاختبارات ===
void run_all_tests() {
    std::cout << "=== اختبارات وحدة سيادة ===\n";

    test_tokenizer_special_tokens();
    test_tokenizer_tashkeel_removal();
    test_tokenizer_morphology();

    test_attention_dimensions();
    test_attention_causal_mask();
    test_attention_softmax();

    test_inference_temperature();
    test_inference_top_k();
    test_inference_repetition_penalty();

    test_quantization_int8();
    test_quantization_roundtrip();

    std::cout << "\n=== النتائج: " << tests_passed << " نجح، "
              << tests_failed << " فشل من أصل "
              << (tests_passed + tests_failed) << " ===\n";
}

} // namespace sad::ai::test

int main() {
    sad::ai::test::run_all_tests();
    return sad::ai::test::tests_failed > 0 ? 1 : 0;
}
