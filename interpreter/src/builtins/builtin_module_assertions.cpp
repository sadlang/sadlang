/**
 * @file builtin_module_assertions.cpp
 * @brief (AR) وحدة التأكيدات — دوال الاختبار والتجزئة والتشفير وC stdlib
 * @brief (EN) Assertions module — test assertions, hashing, encryption, C stdlib wrappers
 *
 * @details
 * (AR) الأقسام:
 *   1. دوال التأكيد (تأكد_يساوي، تأكد_صحيح...)
 *   2. التجزئة (هاش، SHA-256...)
 *   3. التشفير (base64، تشفير CTR...)
 *   4. أغلفة C stdlib (atoi، atof، system...)
 *
 * @note يتطلب: builtin_common.h (الـ headers المشتركة)
 * @see builtin_registry.cpp — التسجيل المركزي
 * @see BUILTIN_CODING_STANDARDS.md — قواعد الكتابة
 */

#include "builtin_common.h"
#include "builtin_registry.h"
namespace Bss = Sad::Builtins::Names::Assertions;
namespace Bffi = Sad::Builtins::Names::FFI;
#include <algorithm>
#include <array>
#include <condition_variable>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <sstream>
#include <unordered_map>

#include "safe_arithmetic.h" // (AR) تحويل آمن مع كشف الفيض / (EN) bounds-checked size_t->int
#include "bounds_checker.h" // (AR) فحص حدود موحَّد / (EN) unified bounds checking
#include "builtin_error.h" // (AR) EM-CPP: حامل خطأ الطبقة الأدنى
namespace Sad
{
    namespace Interpreter
    {

        using namespace StdLib;

        void registerBuiltinsAssertions(Interpreter &interpreter)
        {

            // ═══════════════════════════════════════════════════════════════
            // (AR) دوال الأمان / (EN) Security Functions

            // ═══════════════════════════════════════════════════════════════
            // تأكد / assert — يتحقق من صحة شرط ويرمي خطأ إذا فشل

            // تحقق / verify — مثل تأكد لكن يعيد منطقي بدل رمي خطأ

            // آمن / is_safe — يتحقق إذا كانت القيمة آمنة (ليست null/فارغة)
            auto is_safe_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                    return std::make_shared<Data::Value>(false);
                auto &val = args[0];
                if (!val)
                    return std::make_shared<Data::Value>(false);
                if (val->isVoid())
                    return std::make_shared<Data::Value>(false);
                return std::make_shared<Data::Value>(true);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bss::SAFE_CHECK), is_safe_func);

            // ذعر / panic — يرمي خطأ ذعر (غير قابل للتعافي عادة)
            auto panic_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                std::string message = "(AR) ذعر! / (EN) Panic!";
                if (!args.empty())
                {
                    message = "❌ ذعر: " + args[0]->toString() + " / Panic: " + args[0]->toString();
                }
                ctx.error(::Sad::Errors::ErrorCode::RUN_PANIC, {{"message", message}});
                return std::make_shared<Data::Value>(); // unreachable
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bss::PANIC), panic_func);

            // ═══════════════════════════════════════════════════════════════
            // (AR) دوال التأكيد المتقدمة للاختبارات
            // (EN) Advanced assertion functions for testing

            // ═══════════════════════════════════════════════════════════════
            // تأكد_يساوي / assert_equal — يتحقق أن قيمتين متساويتان
            auto assert_eq_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                auto &actual = args[0];
                auto &expected = args[1];
                if (actual->toString() != expected->toString())
                {
                    std::string message = "فشل التأكيد: توقعت [" + expected->toString() + "] لكن حصلت على [" + actual->toString() + "]";
                    if (args.size() >= 3)
                    {
                        message = args[2]->toString() + ": توقعت [" + expected->toString() + "] لكن حصلت على [" + actual->toString() + "]";
                    }
                    ctx.error(::Sad::Errors::ErrorCode::RUN_ASSERTION_FAILED, {{"detail", message}});
                }
                return std::make_shared<Data::Value>(true);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bss::ASSERT_EQ), assert_eq_func);

            // تأكد_لا_يساوي / assert_not_equal — يتحقق أن قيمتين غير متساويتين
            auto assert_neq_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                auto &a = args[0];
                auto &b = args[1];
                if (a->toString() == b->toString())
                {
                    std::string message = "فشل التأكيد: القيمتان متساويتان [" + a->toString() + "]";
                    if (args.size() >= 3)
                    {
                        message = args[2]->toString() + ": القيمتان متساويتان [" + a->toString() + "]";
                    }
                    ctx.error(::Sad::Errors::ErrorCode::RUN_ASSERTION_FAILED, {{"detail", message}});
                }
                return std::make_shared<Data::Value>(true);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bss::ASSERT_NEQ), assert_neq_func);

            // تأكد_صحيح / assert_true — يتحقق أن القيمة صحيحة
            auto assert_true_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                if (!args[0]->toBool())
                {
                    std::string message = "فشل التأكيد: القيمة ليست صحيحة";
                    if (args.size() >= 2)
                    {
                        message = args[1]->toString();
                    }
                    ctx.error(::Sad::Errors::ErrorCode::RUN_ASSERTION_FAILED, {{"detail", message}});
                }
                return std::make_shared<Data::Value>(true);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bss::ASSERT_TRUE), assert_true_func);

            // تأكد_خطأ / assert_false — يتحقق أن القيمة خاطئة
            auto assert_false_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                if (args[0]->toBool())
                {
                    std::string message = "فشل التأكيد: القيمة ليست خاطئة";
                    if (args.size() >= 2)
                    {
                        message = args[1]->toString();
                    }
                    ctx.error(::Sad::Errors::ErrorCode::RUN_ASSERTION_FAILED, {{"detail", message}});
                }
                return std::make_shared<Data::Value>(true);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bss::ASSERT_FALSE), assert_false_func);

            // تأكد_لاشيء / assert_null — يتحقق أن القيمة لاشيء
            auto assert_null_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                if (!args[0]->isVoid())
                {
                    std::string message = "فشل التأكيد: القيمة ليست لاشيء — [" + args[0]->toString() + "]";
                    if (args.size() >= 2)
                    {
                        message = args[1]->toString() + ": القيمة ليست لاشيء — [" + args[0]->toString() + "]";
                    }
                    ctx.error(::Sad::Errors::ErrorCode::RUN_ASSERTION_FAILED, {{"detail", message}});
                }
                return std::make_shared<Data::Value>(true);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bss::ASSERT_NULL), assert_null_func);

            // تأكد_ليس_لاشيء / assert_not_null — يتحقق أن القيمة ليست لاشيء
            auto assert_not_null_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                if (args[0]->isVoid())
                {
                    std::string message = "فشل التأكيد: القيمة لاشيء";
                    if (args.size() >= 2)
                    {
                        message = args[1]->toString();
                    }
                    ctx.error(::Sad::Errors::ErrorCode::RUN_ASSERTION_FAILED, {{"detail", message}});
                }
                return std::make_shared<Data::Value>(true);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bss::ASSERT_NOT_NULL), assert_not_null_func);

            // تأكد_أكبر / assert_greater — يتحقق أن القيمة الأولى أكبر
            auto assert_gt_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                double a = args[0]->toDouble();
                double b = args[1]->toDouble();
                if (!(a > b))
                {
                    std::string message = "فشل التأكيد: " + args[0]->toString() + " ليس أكبر من " + args[1]->toString();
                    if (args.size() >= 3)
                        message = args[2]->toString();
                    ctx.error(::Sad::Errors::ErrorCode::RUN_ASSERTION_FAILED, {{"detail", message}});
                }
                return std::make_shared<Data::Value>(true);
            };

            // تأكد_أصغر / assert_less — يتحقق أن القيمة الأولى أصغر
            auto assert_lt_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                double a = args[0]->toDouble();
                double b = args[1]->toDouble();
                if (!(a < b))
                {
                    std::string message = "فشل التأكيد: " + args[0]->toString() + " ليس أصغر من " + args[1]->toString();
                    if (args.size() >= 3)
                        message = args[2]->toString();
                    ctx.error(::Sad::Errors::ErrorCode::RUN_ASSERTION_FAILED, {{"detail", message}});
                }
                return std::make_shared<Data::Value>(true);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bss::ASSERT_LT), assert_lt_func);

            // ═══════════════════════════════════════════════════════════════
            // (AR) SHA-256 نقي بلغة C++ — بدون اعتماد على OpenSSL
            // (EN) Pure C++ SHA-256 — no OpenSSL dependency

            // ═══════════════════════════════════════════════════════════════
            // هاش / hash — يحسب هاش SHA-256 حقيقي لنص
            auto hash_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string input = args[0]->toString();

                // ────────────────────────────────────────────────────────
                // (AR) تنفيذ SHA-256 حسب FIPS 180-4
                // (EN) SHA-256 implementation per FIPS 180-4
                // ────────────────────────────────────────────────────────
                auto sha256 = [](const std::string &msg) -> std::string
                {
                    // (AR) ثوابت SHA-256 الأولية — أول 32 بت من الجذور التربيعية لأول 8 أعداد أولية
                    uint32_t h0 = 0x6a09e667, h1 = 0xbb67ae85, h2 = 0x3c6ef372, h3 = 0xa54ff53a;
                    uint32_t h4 = 0x510e527f, h5 = 0x9b05688c, h6 = 0x1f83d9ab, h7 = 0x5be0cd19;
                    static const uint32_t k[64] = {
                        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
                        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
                        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
                        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
                        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
                        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
                        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
                        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};
                    // (AR) دوران يميني — عملية بتية لخوارزمية SHA-256
                    // (EN) right rotate — bitwise operation for SHA-256
                    auto rotr = [](uint32_t x, uint32_t n) -> uint32_t
                    { return (x >> n) | (x << (32 - n)); };

                    // (AR) الحشو — إضافة 1 ثم أصفار ثم الطول بالبتات
                    uint64_t bitLen = (uint64_t)msg.size() * 8;
                    std::vector<uint8_t> data(msg.begin(), msg.end());
                    data.push_back(0x80);
                    while ((data.size() % 64) != 56)
                        data.push_back(0x00);
                    for (int i = 7; i >= 0; --i)
                        data.push_back((uint8_t)(bitLen >> (i * 8)));

                    // (AR) معالجة الكتل 512 بت
                    for (size_t offset = 0; offset < data.size(); offset += 64)
                    {
                        uint32_t w[64];
                        for (int i = 0; i < 16; ++i)
                            w[i] = ((uint32_t)data[offset + i * 4] << 24) | ((uint32_t)data[offset + i * 4 + 1] << 16) |
                                   ((uint32_t)data[offset + i * 4 + 2] << 8) | (uint32_t)data[offset + i * 4 + 3];
                        for (int i = 16; i < 64; ++i)
                        {
                            uint32_t s0 = rotr(w[i - 15], 7) ^ rotr(w[i - 15], 18) ^ (w[i - 15] >> 3);
                            uint32_t s1 = rotr(w[i - 2], 17) ^ rotr(w[i - 2], 19) ^ (w[i - 2] >> 10);
                            w[i] = w[i - 16] + s0 + w[i - 7] + s1;
                        }
                        uint32_t a = h0, b = h1, c = h2, d = h3, e = h4, f = h5, g = h6, hh = h7;
                        for (int i = 0; i < 64; ++i)
                        {
                            uint32_t S1 = rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25);
                            uint32_t ch = (e & f) ^ (~e & g);
                            uint32_t temp1 = hh + S1 + ch + k[i] + w[i];
                            uint32_t S0 = rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22);
                            uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
                            uint32_t temp2 = S0 + maj;
                            hh = g;
                            g = f;
                            f = e;
                            e = d + temp1;
                            d = c;
                            c = b;
                            b = a;
                            a = temp1 + temp2;
                        }
                        h0 += a;
                        h1 += b;
                        h2 += c;
                        h3 += d;
                        h4 += e;
                        h5 += f;
                        h6 += g;
                        h7 += hh;
                    }
                    // (AR) تحويل الناتج إلى سلسلة hex
                    char buf[65];
                    snprintf(buf, sizeof(buf),
                             "%08x%08x%08x%08x%08x%08x%08x%08x",
                             h0, h1, h2, h3, h4, h5, h6, h7);
                    return std::string(buf);
                };
                return std::make_shared<Data::Value>(sha256(input));
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bss::HASH), hash_func);

            // ═══════════════════════════════════════════════════════════════
            // (AR) تشفير SHA-256-CTR — تشفير تيار آمن مبني على SHA-256
            // (EN) SHA-256-CTR encryption — secure stream cipher based on SHA-256

            // ═══════════════════════════════════════════════════════════════
            // (AR) دالة مساعدة: SHA-256 خام تُرجع 32 بايت
            auto sha256_raw = [](const std::vector<uint8_t> &data) -> std::array<uint8_t, 32>
            {
                uint32_t h0 = 0x6a09e667, h1 = 0xbb67ae85, h2 = 0x3c6ef372, h3 = 0xa54ff53a;
                uint32_t h4 = 0x510e527f, h5 = 0x9b05688c, h6 = 0x1f83d9ab, h7 = 0x5be0cd19;
                static const uint32_t k[64] = {
                    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
                    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
                    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
                    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
                    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
                    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
                    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
                    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};
                // (AR) دوران يميني — عملية بتية لخوارزمية SHA-256
                // (EN) right rotate — bitwise operation for SHA-256
                auto rotr = [](uint32_t x, uint32_t n) -> uint32_t
                { return (x >> n) | (x << (32 - n)); };
                uint64_t bitLen = (uint64_t)data.size() * 8;
                std::vector<uint8_t> padded(data);
                padded.push_back(0x80);
                while ((padded.size() % 64) != 56)
                    padded.push_back(0x00);
                for (int i = 7; i >= 0; --i)
                    padded.push_back((uint8_t)(bitLen >> (i * 8)));
                for (size_t off = 0; off < padded.size(); off += 64)
                {
                    uint32_t w[64];
                    for (int i = 0; i < 16; ++i)
                        w[i] = ((uint32_t)padded[off + i * 4] << 24) | ((uint32_t)padded[off + i * 4 + 1] << 16) | ((uint32_t)padded[off + i * 4 + 2] << 8) | (uint32_t)padded[off + i * 4 + 3];
                    for (int i = 16; i < 64; ++i)
                    {
                        uint32_t s0 = rotr(w[i - 15], 7) ^ rotr(w[i - 15], 18) ^ (w[i - 15] >> 3);
                        uint32_t s1 = rotr(w[i - 2], 17) ^ rotr(w[i - 2], 19) ^ (w[i - 2] >> 10);
                        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
                    }
                    uint32_t a = h0, b = h1, c = h2, d = h3, e = h4, f = h5, g = h6, hh = h7;
                    for (int i = 0; i < 64; ++i)
                    {
                        uint32_t S1 = rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25);
                        uint32_t ch = (e & f) ^ (~e & g);
                        uint32_t t1 = hh + S1 + ch + k[i] + w[i];
                        uint32_t S0 = rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22);
                        uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
                        uint32_t t2 = S0 + maj;
                        hh = g;
                        g = f;
                        f = e;
                        e = d + t1;
                        d = c;
                        c = b;
                        b = a;
                        a = t1 + t2;
                    }
                    h0 += a;
                    h1 += b;
                    h2 += c;
                    h3 += d;
                    h4 += e;
                    h5 += f;
                    h6 += g;
                    h7 += hh;
                }
                std::array<uint8_t, 32> result;
                uint32_t hs[8] = {h0, h1, h2, h3, h4, h5, h6, h7};
                for (int i = 0; i < 8; ++i)
                {
                    result[i * 4] = (uint8_t)(hs[i] >> 24);
                    result[i * 4 + 1] = (uint8_t)(hs[i] >> 16);
                    result[i * 4 + 2] = (uint8_t)(hs[i] >> 8);
                    result[i * 4 + 3] = (uint8_t)hs[i];
                }
                return result;
            };

            // (AR) دالة مساعدة: تحويل hex إلى بايتات
            auto hexToBytes = [](const std::string &hex) -> std::string
            {
                if (hex.size() % 2 != 0)
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED); // (AR) helper داخلي بلا ctx
                std::string bytes;
                bytes.reserve(hex.size() / 2);
                for (size_t i = 0; i + 1 < hex.size(); i += 2)
                {
                    std::string hb = hex.substr(i, 2);
                    for (char c : hb)
                        if (!std::isxdigit(static_cast<unsigned char>(c)))
                            throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED); // (AR) helper داخلي بلا ctx
                    bytes += (char)std::stoi(hb, nullptr, 16);
                }
                return bytes;
            };

            // شفّر / encrypt — تشفير SHA-256-CTR (تشفير تيار آمن)
            auto encrypt_func = [sha256_raw](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string text = args[0]->toString();
                std::string key = args[1]->toString();
                if (key.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);

                // (AR) توليد nonce عشوائي 8 بايت
                std::random_device rd;
                std::mt19937_64 rng(rd());
                uint64_t nonce = rng();

                // (AR) تشفير CTR — SHA-256(key || nonce || counter) كتيار مفاتيح
                std::string cipher;
                cipher.reserve(8 + text.size());
                for (int i = 7; i >= 0; --i)
                    cipher += (char)((nonce >> (i * 8)) & 0xFF);

                size_t pos = 0;
                uint64_t ctr = 0;
                while (pos < text.size())
                {
                    std::vector<uint8_t> input(key.begin(), key.end());
                    for (int i = 7; i >= 0; --i)
                        input.push_back((uint8_t)((nonce >> (i * 8)) & 0xFF));
                    for (int i = 7; i >= 0; --i)
                        input.push_back((uint8_t)((ctr >> (i * 8)) & 0xFF));
                    auto block = sha256_raw(input);
                    for (size_t j = 0; j < 32 && pos < text.size(); ++j, ++pos)
                        cipher += (char)((uint8_t)text[pos] ^ block[j]);
                    ++ctr;
                }
                std::stringstream ss;
                for (unsigned char c : cipher)
                    ss << std::hex << std::setfill('0') << std::setw(2) << (int)c;
                return std::make_shared<Data::Value>(ss.str());
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bss::ENCRYPT), encrypt_func);

            // فك_تشفير / decrypt — فك تشفير SHA-256-CTR
            auto decrypt_func = [sha256_raw, hexToBytes](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string hex_text = args[0]->toString();
                std::string key = args[1]->toString();
                if (key.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);

                std::string raw = hexToBytes(hex_text);
                if (raw.size() < 8)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);

                // (AR) استخراج nonce من أول 8 بايت
                uint64_t nonce = 0;
                for (int i = 0; i < 8; ++i)
                    nonce = (nonce << 8) | (uint8_t)raw[i];

                // (AR) فك التشفير CTR
                std::string result;
                result.reserve(raw.size() - 8);
                size_t pos = 8;
                uint64_t ctr = 0;
                while (pos < raw.size())
                {
                    std::vector<uint8_t> input(key.begin(), key.end());
                    for (int i = 7; i >= 0; --i)
                        input.push_back((uint8_t)((nonce >> (i * 8)) & 0xFF));
                    for (int i = 7; i >= 0; --i)
                        input.push_back((uint8_t)((ctr >> (i * 8)) & 0xFF));
                    auto block = sha256_raw(input);
                    for (size_t j = 0; j < 32 && pos < raw.size(); ++j, ++pos)
                        result += (char)((uint8_t)raw[pos] ^ block[j]);
                    ++ctr;
                }
                return std::make_shared<Data::Value>(result);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bss::DECRYPT), decrypt_func);

            // تأكد_نوع / assert_type — يتحقق من نوع القيمة
            auto assert_type_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string actual_type = args[0]->getTypeName();
                std::string expected_type = args[1]->toString();
                // خريطة ترجمة الأنواع العربية ↔ الإنجليزية
                static const std::unordered_map<std::string, std::string> type_map = {
                    {"رقم", "INTEGER"}, {"عدد", "INTEGER"}, {"صحيح", "INTEGER"}, {"عشري", "DOUBLE"}, {"حقيقي", "DOUBLE"}, {"نص", "STRING"}, {"سلسلة", "STRING"}, {"منطقي", "BOOLEAN"}, {"مصفوفة", "ARRAY"}, {"قائمة", "ARRAY"}, {"قاموس", "MAP"}, {"خريطة", "MAP"}, {"فراغ", "VOID"}, {"عدم", "NULL"}, {"لاشيء", "NULL"}};
                std::string normalized = expected_type;
                auto it = type_map.find(expected_type);
                if (it != type_map.end())
                {
                    normalized = it->second;
                }
                // أيضاً تحويل actual_type لكبيرة للمقارنة
                std::string actual_upper = actual_type;
                for (auto &c : actual_upper)
                    c = toupper(c);
                if (actual_upper != normalized && actual_type != expected_type)
                {
                    ctx.error(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED,
                              {{"expected", expected_type}, {"actual", actual_type}});
                }
                return std::make_shared<Data::Value>(true);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bss::ASSERT_TYPE), assert_type_func);

            // (AR) الأسماء القديمة تُوجّه للدوال الجديدة (تأكد_يساوي → تأكد_مساواة, assert_gt → تأكد_أكبر)
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bss::ASSERT_GT), assert_gt_func);

            // نظّف / sanitize — تنظيف نص من الأحرف الخطيرة (XSS/SQL Injection)
            auto sanitize_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string input = args[0]->toString();
                std::string result;
                for (char c : input)
                {
                    switch (c)
                    {
                    case '<':
                        result += "&lt;";
                        break;
                    case '>':
                        result += "&gt;";
                        break;
                    case '&':
                        result += "&amp;";
                        break;
                    case '"':
                        result += "&quot;";
                        break;
                    case '\'':
                        result += "&#39;";
                        break;
                    case ';':
                        result += "";
                        break; // Remove semicolons (SQL injection)
                    case '-':
                        if (!result.empty() && result.back() == '-')
                        {
                            result.pop_back();
                            break;
                        } // -- comments
                        result += c;
                        break;
                    default:
                        result += c;
                        break;
                    }
                }
                return std::make_shared<Data::Value>(result);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bss::SANITIZE), sanitize_func);

            // وقت_الآن / timestamp — الوقت الحالي (Unix timestamp)

            // عشوائي_آمن / secure_random — عدد عشوائي آمن (crypto-grade)
            auto secure_random_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                int min_val = 0, max_val = 100;
                if (args.size() >= 1)
                    min_val = static_cast<int>(args[0]->toDouble());
                if (args.size() >= 2)
                    max_val = static_cast<int>(args[1]->toDouble());
                // Use random_device for crypto-grade randomness
                std::random_device rd;
                std::uniform_int_distribution<int> dist(min_val, max_val);
                return std::make_shared<Data::Value>(static_cast<double>(dist(rd)));
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bss::SECURE_RANDOM), secure_random_func);

            // base64_encode / ترميز_64
            auto base64_encode_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
                std::string input = args[0]->toString();
                std::string result;
                int val = 0, valb = -6;
                for (unsigned char c : input)
                {
                    val = (val << 8) + c;
                    valb += 8;
                    while (valb >= 0)
                    {
                        result.push_back(chars[(val >> valb) & 0x3F]);
                        valb -= 6;
                    }
                }
                if (valb > -6)
                    result.push_back(chars[((val << 8) >> (valb + 8)) & 0x3F]);
                while (result.size() % 4)
                    result.push_back('=');
                return std::make_shared<Data::Value>(result);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bss::BASE64_ENCODE), base64_encode_func);

            // ═══════════════════════════════════════════════════════════════
            // القسم 13: دوال FFI (واجهة الدوال الخارجية C/C++)
            // Section 13: FFI Functions (C/C++ Foreign Function Interface)

            // ═══════════════════════════════════════════════════════════════

            // ═══════════════════════════════════════════════════════════════════
            // (AR) جدول مؤشرات مُدار — يخزّن العناوين الحقيقية بمعرّف int آمن
            //      بدلاً من تخزين uintptr_t كـ double (يفقد الدقة فوق 2^52)
            // (EN) Managed pointer table — stores real addresses with safe int IDs
            //      instead of storing uintptr_t as double (loses precision above 2^52)

            // ═══════════════════════════════════════════════════════════════════
            static std::unordered_map<int, void *> ptrTable;
            static int nextPtrId = 1;
            static constexpr size_t MAX_FFI_ALLOC_SIZE = 256 * 1024 * 1024; // 256MB حد أقصى للحجز
            static constexpr size_t MAX_FFI_ALLOC_COUNT = 10000;            // حد أقصى لعدد الحجوزات

            // (AR) دالة مساعدة لتسجيل مؤشر وإرجاع معرّفه
            // (EN) Helper to register a pointer and return its ID
            auto registerPtr = [](void *ptr) -> int
            {
                if (!ptr)
                    return 0;
                int id = nextPtrId++;
                ptrTable[id] = ptr;
                return id;
            };

            // (AR) دالة مساعدة لجلب المؤشر من معرّفه
            // (EN) Helper to get pointer from its ID
            auto getPtr = [](int id) -> void *
            {
                auto it = ptrTable.find(id);
                return (it != ptrTable.end()) ? it->second : nullptr;
            };

            // (AR) دالة مساعدة لإزالة مؤشر من الجدول
            // (EN) Helper to remove pointer from table
            auto removePtr = [](int id) -> void *
            {
                auto it = ptrTable.find(id);
                if (it == ptrTable.end())
                    return nullptr;
                void *ptr = it->second;
                ptrTable.erase(it);
                return ptr;
            };

            // 1. printf — طباعة_تنسيق / formatted print
            auto ffi_printf_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string fmt = args[0]->toString();
                // Simple printf simulation: replace %d, %s, %f, %p with argument values
                std::string result;
                size_t argIdx = 1;
                for (size_t i = 0; i < fmt.size(); i++)
                {
                    if (fmt[i] == '%' && i + 1 < fmt.size())
                    {
                        char spec = fmt[i + 1];
                        if (spec == 'd' || spec == 'i')
                        {
                            if (Sad::Security::BoundsChecker::checkArrayIndex(argIdx, args.size()))
                                result += std::to_string(static_cast<int>(args[argIdx++]->toDouble()));
                            else
                                result += "0";
                            i++;
                        }
                        else if (spec == 'f')
                        {
                            if (Sad::Security::BoundsChecker::checkArrayIndex(argIdx, args.size()))
                            {
                                char buf[64];
                                std::snprintf(buf, sizeof(buf), "%f", args[argIdx++]->toDouble());
                                result += buf;
                            }
                            else
                                result += "0.000000";
                            i++;
                        }
                        else if (spec == 's')
                        {
                            if (Sad::Security::BoundsChecker::checkArrayIndex(argIdx, args.size()))
                                result += args[argIdx++]->toString();
                            else
                                result += "(null)";
                            i++;
                        }
                        else if (spec == 'p')
                        {
                            if (Sad::Security::BoundsChecker::checkArrayIndex(argIdx, args.size()))
                            {
                                char buf[32];
                                std::snprintf(buf, sizeof(buf), "%p", (void *)(uintptr_t)static_cast<int64_t>(args[argIdx++]->toDouble()));
                                result += buf;
                            }
                            else
                                result += "0x0";
                            i++;
                        }
                        else if (spec == '%')
                        {
                            result += '%';
                            i++;
                        }
                        else
                        {
                            result += '%';
                        }
                    }
                    else if (fmt[i] == '\\' && i + 1 < fmt.size() && fmt[i + 1] == 'n')
                    {
                        result += '\n';
                        i++;
                    }
                    else
                    {
                        result += fmt[i];
                    }
                }
                std::cout << result;
                return std::make_shared<Data::Value>(::Sad::Security::SafeArithmetic::assertSafeCast<int>(result.size(), "builtin_module_assertions_size"));
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bffi::C_PRINTF), ffi_printf_func);

            // 2. malloc — حجز / allocate memory (managed pointer table)
            auto ffi_malloc_func = [&interpreter, registerPtr](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                // (AR) فحص أمني — حظر في وضع الأمان
                if (interpreter.getOptions().enableSecurity)
                {
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                }
                size_t size = static_cast<size_t>(args[0]->toDouble());
                if (size > MAX_FFI_ALLOC_SIZE)
                {
                    ctx.error(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED,
                              {{"expected", "size <= 256MB"}, {"actual", std::to_string(size)}});
                }
                if (ptrTable.size() >= MAX_FFI_ALLOC_COUNT)
                {
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                }
                void *ptr = std::malloc(size);
                if (!ptr)
                    return std::make_shared<Data::Value>(0);
                return std::make_shared<Data::Value>(registerPtr(ptr));
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bffi::C_MALLOC), ffi_malloc_func);

            // 3. free — حرر / free memory (managed pointer table)
            auto ffi_free_func = [&interpreter, removePtr](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                if (interpreter.getOptions().enableSecurity)
                {
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                }
                int id = static_cast<int>(args[0]->toDouble());
                void *ptr = removePtr(id);
                if (ptr)
                    std::free(ptr);
                return std::make_shared<Data::Value>();
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bffi::C_FREE), ffi_free_func);

            // 4. realloc — اعد_حجز (managed pointer table)
            auto ffi_realloc_func = [&interpreter, removePtr, registerPtr](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                if (interpreter.getOptions().enableSecurity)
                {
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                }
                int id = static_cast<int>(args[0]->toDouble());
                size_t size = static_cast<size_t>(args[1]->toDouble());
                if (size > MAX_FFI_ALLOC_SIZE)
                {
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                }
                void *oldPtr = removePtr(id);
                void *ptr = std::realloc(oldPtr, size);
                return std::make_shared<Data::Value>(registerPtr(ptr));
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bffi::C_REALLOC), ffi_realloc_func);

            // 5. calloc — حجز_صفري (managed pointer table)
            auto ffi_calloc_func = [&interpreter, registerPtr](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                if (interpreter.getOptions().enableSecurity)
                {
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                }
                size_t count = static_cast<size_t>(args[0]->toDouble());
                size_t size = static_cast<size_t>(args[1]->toDouble());
                if (count * size > MAX_FFI_ALLOC_SIZE)
                {
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                }
                if (ptrTable.size() >= MAX_FFI_ALLOC_COUNT)
                {
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                }
                void *ptr = std::calloc(count, size);
                return std::make_shared<Data::Value>(registerPtr(ptr));
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bffi::C_CALLOC), ffi_calloc_func);

            // 6. strlen — طول_نص_س
            auto ffi_strlen_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string s = args[0]->toString();
                return std::make_shared<Data::Value>(static_cast<double>(s.length()));
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bffi::C_STRLEN), ffi_strlen_func);

            // 7. strcpy — انسخ_نص_س
            auto ffi_strcpy_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                // In interpreted mode, just return a copy of the string
                return std::make_shared<Data::Value>(args[0]->toString());
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bffi::C_STRCPY), ffi_strcpy_func);

            // 8. strcmp — قارن_نص_س
            auto ffi_strcmp_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                int result = args[0]->toString().compare(args[1]->toString());
                return std::make_shared<Data::Value>(static_cast<double>(result));
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bffi::C_STRCMP), ffi_strcmp_func);

            // 9. strcat — الحق_نص_س
            auto ffi_strcat_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                return std::make_shared<Data::Value>(args[0]->toString() + args[1]->toString());
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bffi::C_STRCAT), ffi_strcat_func);

            // 10. memcpy — انسخ_ذاكرة_س (simulated with string copy in interpreter)

            // 11. memset — عبئ_ذاكرة_س (simulated)

            // 12. fopen — افتح_ملف_س (managed pointer table + security check)
            auto ffi_fopen_func = [&interpreter, registerPtr](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string filename = args[0]->toString();
                std::string mode = args[1]->toString();
                // (AR) فحص أمني — حظر مسارات خطيرة في الوضع الآمن
                // (EN) Security check — block dangerous paths in secure mode
                if (interpreter.getOptions().enableSecurity)
                {
                    // (AR) حظر المسارات التي تحتوي على تصعيد (..)
                    if (filename.find("..") != std::string::npos)
                    {
                        ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                    }
                    // (AR) حظر المسارات المطلقة في Unix/Windows
                    if (filename[0] == '/' || (filename.size() >= 2 && filename[1] == ':'))
                    {
                        ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                    }
                }
                FILE *fp = std::fopen(filename.c_str(), mode.c_str());
                if (!fp)
                    return std::make_shared<Data::Value>(0);
                return std::make_shared<Data::Value>(registerPtr(fp));
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bffi::C_FOPEN), ffi_fopen_func);

            // 13. fclose — اغلق_ملف_س (managed pointer table)
            auto ffi_fclose_func = [removePtr](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                int id = static_cast<int>(args[0]->toDouble());
                if (id == 0)
                    return std::make_shared<Data::Value>(-1);
                void *ptr = removePtr(id);
                if (!ptr)
                    return std::make_shared<Data::Value>(-1);
                int result = std::fclose(reinterpret_cast<FILE *>(ptr));
                return std::make_shared<Data::Value>(static_cast<double>(result));
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bffi::C_FCLOSE), ffi_fclose_func);

            // 14. fputs — اكتب_ملف_س (managed pointer table)
            auto ffi_fputs_func = [getPtr](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string text = args[0]->toString();
                int id = static_cast<int>(args[1]->toDouble());
                if (id == 0)
                    return std::make_shared<Data::Value>(-1);
                void *ptr = getPtr(id);
                if (!ptr)
                    return std::make_shared<Data::Value>(-1);
                int result = std::fputs(text.c_str(), reinterpret_cast<FILE *>(ptr));
                return std::make_shared<Data::Value>(static_cast<double>(result));
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bffi::C_FPUTS), ffi_fputs_func);

            // 15. fgets — اقرأ_ملف_س
            auto ffi_fgets_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                int size = static_cast<int>(args[0]->toDouble());
                uintptr_t addr = static_cast<uintptr_t>(args[1]->toDouble());
                if (addr == 0 || size <= 0)
                    return std::make_shared<Data::Value>(std::string(""));
                std::vector<char> buf(size);
                char *result = std::fgets(buf.data(), size, reinterpret_cast<FILE *>(addr));
                if (!result)
                    return std::make_shared<Data::Value>(std::string(""));
                return std::make_shared<Data::Value>(std::string(buf.data()));
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bffi::C_FGETS), ffi_fgets_func);

            // 16. system — نفذ_امر
            // (AR) أضيفت حماية أمنية: يُمنع تنفيذ أوامر النظام في الوضع الآمن
            // (EN) Added security: system commands blocked in secure mode
            auto ffi_system_func = [&interpreter](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                // (AR) فحص وضع الأمان / (EN) Check security mode
                if (interpreter.getOptions().enableSecurity)
                {
                    ctx.error(::Sad::Errors::ErrorCode::RUN_PERMISSION_DENIED,
                              {{"resource", "نظام/system"}, {"reason", "(AR) الوضع الآمن يمنع أوامر النظام (استخدم --no-security) / (EN) secure-mode blocks system commands"}});
                }
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string cmd = args[0]->toString();

                // (AR) فحص أوامر خطرة حتى خارج الوضع الآمن
                // (EN) Check dangerous commands even outside secure mode
                static const std::vector<std::string> dangerousPatterns = {
                    "rm -rf /", "del /f /s /q C:\\", "format ", "mkfs.",
                    ":(){ :|:", "shutdown", "reboot"};
                for (const auto &pattern : dangerousPatterns)
                {
                    if (cmd.find(pattern) != std::string::npos)
                    {
                        ctx.error(::Sad::Errors::ErrorCode::RUN_PERMISSION_DENIED,
                                  {{"resource", "نظام/system"}, {"reason", "(AR) نمط خطير / (EN) dangerous pattern: " + pattern}});
                    }
                }

                int result = std::system(cmd.c_str());
                return std::make_shared<Data::Value>(static_cast<double>(result));
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bffi::C_SYSTEM), ffi_system_func);

            // 17. getenv — قيمة_بيئة
            auto ffi_getenv_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string varName = args[0]->toString();
                const char *val = std::getenv(varName.c_str());
                if (val)
                {
                    return std::make_shared<Data::Value>(std::string(val));
                }
                return std::make_shared<Data::Value>(); // (AR) فراغ — بحث بلا مطابقة (type.void)، لا «لاشيء/عدم»
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bffi::C_GETENV), ffi_getenv_func);

            // 18. atoi — نص_لعدد
            auto ffi_atoi_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                int result = std::atoi(args[0]->toString().c_str());
                return std::make_shared<Data::Value>(static_cast<double>(result));
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bffi::C_ATOI), ffi_atoi_func);

            // 19. atof — نص_لعشري
            auto ffi_atof_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                double result = std::atof(args[0]->toString().c_str());
                return std::make_shared<Data::Value>(result);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bffi::C_ATOF), ffi_atof_func);

            // 20. snprintf — تنسيق_نص
            auto ffi_snprintf_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string fmt = args[0]->toString();
                // Reuse printf-style formatting to produce a string
                std::string result;
                size_t argIdx = 1;
                for (size_t i = 0; i < fmt.size(); i++)
                {
                    if (fmt[i] == '%' && i + 1 < fmt.size())
                    {
                        char spec = fmt[i + 1];
                        if (spec == 'd' || spec == 'i')
                        {
                            if (Sad::Security::BoundsChecker::checkArrayIndex(argIdx, args.size()))
                                result += std::to_string(static_cast<int>(args[argIdx++]->toDouble()));
                            else
                                result += "0";
                            i++;
                        }
                        else if (spec == 'f')
                        {
                            if (Sad::Security::BoundsChecker::checkArrayIndex(argIdx, args.size()))
                            {
                                char buf[64];
                                std::snprintf(buf, sizeof(buf), "%f", args[argIdx++]->toDouble());
                                result += buf;
                            }
                            else
                                result += "0.000000";
                            i++;
                        }
                        else if (spec == 's')
                        {
                            if (Sad::Security::BoundsChecker::checkArrayIndex(argIdx, args.size()))
                                result += args[argIdx++]->toString();
                            else
                                result += "(null)";
                            i++;
                        }
                        else if (spec == '%')
                        {
                            result += '%';
                            i++;
                        }
                        else
                        {
                            result += '%';
                        }
                    }
                    else if (fmt[i] == '\\' && i + 1 < fmt.size() && fmt[i + 1] == 'n')
                    {
                        result += '\n';
                        i++;
                    }
                    else
                    {
                        result += fmt[i];
                    }
                }
                return std::make_shared<Data::Value>(result);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bffi::C_SNPRINTF), ffi_snprintf_func);
        }

    } // namespace Interpreter
} // namespace Sad
