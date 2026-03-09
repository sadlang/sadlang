/**
 * @file builtin_registry_part7.cpp
 * @brief (AR) جزء من تسجيل الدوال المضمنة
 */

#include "builtins.h"
#include "interpreter_core.h"
#include "io/io_functions.h"
#include "type_functions.h"
#include "array_functions.h"
#include "other_functions.h"
#include "string/string_functions.h"
#include "math/math_functions.h"
#include "math/advanced_math.h"
#include "system/system_functions.h"
#include "filesystem/filesystem_module.h"
#include "graphics/graphics_module.h"
#include "graphics/sad_particles.h"
#include "graphics/sad_effects.h"
#include "graphics/sad_physics.h"
#include "graphics/sad_scene3d.h"
#include "graphics/sad_ai.h"
#include "graphics/sad_camera2d.h"
#include "graphics/sad_charts.h"
#include "graphics/sad_audio.h"
#include "graphics/sad_tilemap.h"
#include "graphics/sad_islamic_art.h"
#include "graphics/sad_dhikr.h"
#include "graphics/sad_qibla.h"
#include "graphics/sad_prayer.h"
#include "graphics/sad_hijri.h"
#include "graphics/sad_android.h"
#include "mobile/mobile_module.h"
#include <memory>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <random>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <functional>
#include <thread>
#include <mutex>
#include <future>
#include <set>
#include <regex>
#include <numeric>
#include <bitset>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <atomic>
#include <array>
#include <iomanip>
#include <queue>
#include <unordered_map>
#include <condition_variable>

namespace Sad {
namespace Interpreter {

using namespace StdLib;

void registerBuiltinsPart7(Interpreter& interpreter) {
    // (AR) إنشاء زر واجهة — واجهة_زر(عنوان)
    auto ui_button_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("واجهة_زر: يحتاج معامل واحد على الأقل");
        auto elem = sad::stdlib::mobile::create_button(args[0]->toString());
        return std::make_shared<Data::Value>(sad::stdlib::mobile::render_ui_element(elem));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("واجهة_زر", ui_button_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ui_button", ui_button_func);

    // (AR) إنشاء صورة واجهة — واجهة_صورة(مصدر)
    auto ui_image_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("واجهة_صورة: يحتاج معامل واحد على الأقل");
        auto elem = sad::stdlib::mobile::create_image(args[0]->toString());
        return std::make_shared<Data::Value>(sad::stdlib::mobile::render_ui_element(elem));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("واجهة_صورة", ui_image_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ui_image", ui_image_func);

    // (AR) تخطيط عمودي — واجهة_عمود()
    auto ui_column_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto elem = sad::stdlib::mobile::create_column({});
        return std::make_shared<Data::Value>(sad::stdlib::mobile::render_ui_element(elem));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("واجهة_عمود", ui_column_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ui_column", ui_column_func);

    // (AR) تخطيط أفقي — واجهة_صف()
    auto ui_row_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto elem = sad::stdlib::mobile::create_row({});
        return std::make_shared<Data::Value>(sad::stdlib::mobile::render_ui_element(elem));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("واجهة_صف", ui_row_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ui_row", ui_row_func);

    // (AR) توليد كود Compose — واجهة_كومبوز(نوع، محتوى)
    auto ui_compose_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("واجهة_كومبوز: يحتاج معامل واحد على الأقل");
        auto elem = sad::stdlib::mobile::create_text(args[0]->toString());
        return std::make_shared<Data::Value>(sad::stdlib::mobile::render_to_compose(elem));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("واجهة_كومبوز", ui_compose_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ui_compose", ui_compose_func);

    // (AR) توليد كود SwiftUI — واجهة_سويفت(نوع، محتوى)
    auto ui_swiftui_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("واجهة_سويفت: يحتاج معامل واحد على الأقل");
        auto elem = sad::stdlib::mobile::create_text(args[0]->toString());
        return std::make_shared<Data::Value>(sad::stdlib::mobile::render_to_swiftui(elem));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("واجهة_سويفت", ui_swiftui_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ui_swiftui", ui_swiftui_func);

    // (AR) التقاط صورة — كاميرا_التقط()
    auto camera_capture_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        return std::make_shared<Data::Value>(sad::stdlib::mobile::capture_photo());
    };
    interpreter.getFunctionManager().registerBuiltinFunction("كاميرا_التقط", camera_capture_func);
    interpreter.getFunctionManager().registerBuiltinFunction("camera_capture", camera_capture_func);

    // (AR) فحص الكاميرا — كاميرا_متوفرة()
    auto camera_available_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        return std::make_shared<Data::Value>(sad::stdlib::mobile::is_camera_available());
    };
    interpreter.getFunctionManager().registerBuiltinFunction("كاميرا_متوفرة", camera_available_func);
    interpreter.getFunctionManager().registerBuiltinFunction("camera_available", camera_available_func);

    // (AR) الموقع الحالي — موقع_حالي()
    auto location_current_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto loc = sad::stdlib::mobile::get_current_location();
        // إرجاع نص يحتوي على إحداثيات الموقع
        std::string result = std::to_string(loc.latitude) + "," + std::to_string(loc.longitude);
        return std::make_shared<Data::Value>(result);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("موقع_حالي", location_current_func);
    interpreter.getFunctionManager().registerBuiltinFunction("location_current", location_current_func);

    // (AR) حساب المسافة — موقع_مسافة(خ1، ط1، خ2، ط2)
    auto location_distance_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 4) throw std::runtime_error("موقع_مسافة: يحتاج 4 معاملات (خط_عرض1، خط_طول1، خط_عرض2، خط_طول2)");
        double lat1 = args[0]->toDouble();
        double lon1 = args[1]->toDouble();
        double lat2 = args[2]->toDouble();
        double lon2 = args[3]->toDouble();
        double distance = sad::stdlib::mobile::calculate_distance(lat1, lon1, lat2, lon2);
        return std::make_shared<Data::Value>(distance);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("موقع_مسافة", location_distance_func);
    interpreter.getFunctionManager().registerBuiltinFunction("location_distance", location_distance_func);

    // (AR) مستشعر التسارع — مستشعر_تسارع()
    auto accelerometer_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto data = sad::stdlib::mobile::get_accelerometer();
        std::string result = "x:" + std::to_string(data.x) + ",y:" + std::to_string(data.y) + ",z:" + std::to_string(data.z);
        return std::make_shared<Data::Value>(result);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("مستشعر_تسارع", accelerometer_func);
    interpreter.getFunctionManager().registerBuiltinFunction("accelerometer", accelerometer_func);

    // (AR) مستشعر الدوران — مستشعر_دوران()
    auto gyroscope_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto data = sad::stdlib::mobile::get_gyroscope();
        std::string result = "x:" + std::to_string(data.x) + ",y:" + std::to_string(data.y) + ",z:" + std::to_string(data.z);
        return std::make_shared<Data::Value>(result);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("مستشعر_دوران", gyroscope_func);
    interpreter.getFunctionManager().registerBuiltinFunction("gyroscope", gyroscope_func);

    // (AR) إنشاء مشروع هاتف — هاتف_مشروع_جديد(اسم، منصة)
    auto mobile_create_project_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("هاتف_مشروع_جديد: يحتاج معاملين (اسم، منصة)");
        std::string name = args[0]->toString();
        std::string platform = args[1]->toString();
        bool result = sad::stdlib::mobile::create_mobile_project(name, platform);
        return std::make_shared<Data::Value>(result);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("هاتف_مشروع_جديد", mobile_create_project_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mobile_create_project", mobile_create_project_func);

    // (AR) Android Manifest — هاتف_مانيفست(معرف_حزمة، اسم_تطبيق)
    auto android_manifest_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("هاتف_مانيفست: يحتاج معاملين (معرف_حزمة، اسم_تطبيق)");
        return std::make_shared<Data::Value>(
            sad::stdlib::mobile::generate_android_manifest(args[0]->toString(), args[1]->toString()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("هاتف_مانيفست", android_manifest_func);
    interpreter.getFunctionManager().registerBuiltinFunction("android_manifest", android_manifest_func);

    // (AR) iOS Info.plist — هاتف_بلست(اسم_تطبيق، معرف_حزمة)
    auto ios_plist_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("هاتف_بلست: يحتاج معاملين (اسم_تطبيق، معرف_حزمة)");
        return std::make_shared<Data::Value>(
            sad::stdlib::mobile::generate_ios_plist(args[0]->toString(), args[1]->toString()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("هاتف_بلست", ios_plist_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ios_plist", ios_plist_func);

    // ===================================================================
    // (AR) دوال الأمان / (EN) Security Functions
    // ===================================================================

    // تأكد / assert — يتحقق من صحة شرط ويرمي خطأ إذا فشل
    auto assert_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) تأكد: يحتاج معامل واحد على الأقل / (EN) assert: needs at least 1 argument");
        bool condition = args[0]->toBool();
        if (!condition) {
            std::string message = "(AR) فشل التأكيد / (EN) Assertion failed";
            if (args.size() >= 2) {
                message = "(AR) فشل التأكيد: " + args[1]->toString() + " / (EN) Assertion failed: " + args[1]->toString();
            }
            throw std::runtime_error(message);
        }
        return std::make_shared<Data::Value>(true);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("تأكد", assert_func);
    interpreter.getFunctionManager().registerBuiltinFunction("assert", assert_func);

    // تحقق / verify — مثل تأكد لكن يعيد منطقي بدل رمي خطأ
    auto verify_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) تحقق: يحتاج معامل واحد على الأقل / (EN) verify: needs at least 1 argument");
        return std::make_shared<Data::Value>(args[0]->toBool());
    };
    interpreter.getFunctionManager().registerBuiltinFunction("تحقق", verify_func);
    interpreter.getFunctionManager().registerBuiltinFunction("verify", verify_func);

    // آمن / is_safe — يتحقق إذا كانت القيمة آمنة (ليست null/فارغة)
    auto is_safe_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(false);
        auto& val = args[0];
        if (!val) return std::make_shared<Data::Value>(false);
        if (val->isVoid()) return std::make_shared<Data::Value>(false);
        return std::make_shared<Data::Value>(true);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("آمن", is_safe_func);
    interpreter.getFunctionManager().registerBuiltinFunction("is_safe", is_safe_func);

    // ذعر / panic — يرمي خطأ ذعر (غير قابل للتعافي عادة)
    auto panic_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string message = "(AR) ذعر! / (EN) Panic!";
        if (!args.empty()) {
            message = "❌ ذعر: " + args[0]->toString() + " / Panic: " + args[0]->toString();
        }
        throw std::runtime_error(message);
        return std::make_shared<Data::Value>(); // unreachable
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ذعر", panic_func);
    interpreter.getFunctionManager().registerBuiltinFunction("panic", panic_func);

    // ===================================================================
    // (AR) SHA-256 نقي بلغة C++ — بدون اعتماد على OpenSSL
    // (EN) Pure C++ SHA-256 — no OpenSSL dependency
    // ===================================================================
    // هاش / hash — يحسب هاش SHA-256 حقيقي لنص
    auto hash_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) هاش: يحتاج نص / (EN) hash: needs a string");
        std::string input = args[0]->toString();

        // ────────────────────────────────────────────────────────
        // (AR) تنفيذ SHA-256 حسب FIPS 180-4
        // (EN) SHA-256 implementation per FIPS 180-4
        // ────────────────────────────────────────────────────────
        auto sha256 = [](const std::string& msg) -> std::string {
            // (AR) ثوابت SHA-256 الأولية — أول 32 بت من الجذور التربيعية لأول 8 أعداد أولية
            uint32_t h0 = 0x6a09e667, h1 = 0xbb67ae85, h2 = 0x3c6ef372, h3 = 0xa54ff53a;
            uint32_t h4 = 0x510e527f, h5 = 0x9b05688c, h6 = 0x1f83d9ab, h7 = 0x5be0cd19;
            static const uint32_t k[64] = {
                0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
                0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
                0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
                0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
                0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
                0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
                0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
                0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
            };
            auto rotr = [](uint32_t x, uint32_t n) -> uint32_t { return (x >> n) | (x << (32 - n)); };

            // (AR) الحشو — إضافة 1 ثم أصفار ثم الطول بالبتات
            uint64_t bitLen = (uint64_t)msg.size() * 8;
            std::vector<uint8_t> data(msg.begin(), msg.end());
            data.push_back(0x80);
            while ((data.size() % 64) != 56) data.push_back(0x00);
            for (int i = 7; i >= 0; --i) data.push_back((uint8_t)(bitLen >> (i * 8)));

            // (AR) معالجة الكتل 512 بت
            for (size_t offset = 0; offset < data.size(); offset += 64) {
                uint32_t w[64];
                for (int i = 0; i < 16; ++i)
                    w[i] = ((uint32_t)data[offset+i*4]<<24) | ((uint32_t)data[offset+i*4+1]<<16) |
                            ((uint32_t)data[offset+i*4+2]<<8) | (uint32_t)data[offset+i*4+3];
                for (int i = 16; i < 64; ++i) {
                    uint32_t s0 = rotr(w[i-15],7) ^ rotr(w[i-15],18) ^ (w[i-15]>>3);
                    uint32_t s1 = rotr(w[i-2],17) ^ rotr(w[i-2],19)  ^ (w[i-2]>>10);
                    w[i] = w[i-16] + s0 + w[i-7] + s1;
                }
                uint32_t a=h0, b=h1, c=h2, d=h3, e=h4, f=h5, g=h6, hh=h7;
                for (int i = 0; i < 64; ++i) {
                    uint32_t S1 = rotr(e,6) ^ rotr(e,11) ^ rotr(e,25);
                    uint32_t ch = (e & f) ^ (~e & g);
                    uint32_t temp1 = hh + S1 + ch + k[i] + w[i];
                    uint32_t S0 = rotr(a,2) ^ rotr(a,13) ^ rotr(a,22);
                    uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
                    uint32_t temp2 = S0 + maj;
                    hh=g; g=f; f=e; e=d+temp1; d=c; c=b; b=a; a=temp1+temp2;
                }
                h0+=a; h1+=b; h2+=c; h3+=d; h4+=e; h5+=f; h6+=g; h7+=hh;
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
    interpreter.getFunctionManager().registerBuiltinFunction("هاش", hash_func);
    interpreter.getFunctionManager().registerBuiltinFunction("hash", hash_func);
    interpreter.getFunctionManager().registerBuiltinFunction("sha256", hash_func);
    interpreter.getFunctionManager().registerBuiltinFunction("تجزئة", hash_func);

    // ===================================================================
    // (AR) تشفير SHA-256-CTR — تشفير تيار آمن مبني على SHA-256
    // (EN) SHA-256-CTR encryption — secure stream cipher based on SHA-256
    // ===================================================================

    // (AR) دالة مساعدة: SHA-256 خام تُرجع 32 بايت
    auto sha256_raw = [](const std::vector<uint8_t>& data) -> std::array<uint8_t, 32> {
        uint32_t h0=0x6a09e667,h1=0xbb67ae85,h2=0x3c6ef372,h3=0xa54ff53a;
        uint32_t h4=0x510e527f,h5=0x9b05688c,h6=0x1f83d9ab,h7=0x5be0cd19;
        static const uint32_t k[64]={
            0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
            0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
            0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
            0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
            0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
            0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
            0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
            0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
        };
        auto rotr=[](uint32_t x,uint32_t n)->uint32_t{return(x>>n)|(x<<(32-n));};
        uint64_t bitLen=(uint64_t)data.size()*8;
        std::vector<uint8_t> padded(data);
        padded.push_back(0x80);
        while((padded.size()%64)!=56) padded.push_back(0x00);
        for(int i=7;i>=0;--i) padded.push_back((uint8_t)(bitLen>>(i*8)));
        for(size_t off=0;off<padded.size();off+=64){
            uint32_t w[64];
            for(int i=0;i<16;++i) w[i]=((uint32_t)padded[off+i*4]<<24)|((uint32_t)padded[off+i*4+1]<<16)|((uint32_t)padded[off+i*4+2]<<8)|(uint32_t)padded[off+i*4+3];
            for(int i=16;i<64;++i){uint32_t s0=rotr(w[i-15],7)^rotr(w[i-15],18)^(w[i-15]>>3);uint32_t s1=rotr(w[i-2],17)^rotr(w[i-2],19)^(w[i-2]>>10);w[i]=w[i-16]+s0+w[i-7]+s1;}
            uint32_t a=h0,b=h1,c=h2,d=h3,e=h4,f=h5,g=h6,hh=h7;
            for(int i=0;i<64;++i){uint32_t S1=rotr(e,6)^rotr(e,11)^rotr(e,25);uint32_t ch=(e&f)^(~e&g);uint32_t t1=hh+S1+ch+k[i]+w[i];uint32_t S0=rotr(a,2)^rotr(a,13)^rotr(a,22);uint32_t maj=(a&b)^(a&c)^(b&c);uint32_t t2=S0+maj;hh=g;g=f;f=e;e=d+t1;d=c;c=b;b=a;a=t1+t2;}
            h0+=a;h1+=b;h2+=c;h3+=d;h4+=e;h5+=f;h6+=g;h7+=hh;
        }
        std::array<uint8_t,32> result;
        uint32_t hs[8]={h0,h1,h2,h3,h4,h5,h6,h7};
        for(int i=0;i<8;++i){result[i*4]=(uint8_t)(hs[i]>>24);result[i*4+1]=(uint8_t)(hs[i]>>16);result[i*4+2]=(uint8_t)(hs[i]>>8);result[i*4+3]=(uint8_t)hs[i];}
        return result;
    };

    // (AR) دالة مساعدة: تحويل hex إلى بايتات
    auto hexToBytes = [](const std::string& hex) -> std::string {
        if (hex.size() % 2 != 0)
            throw std::runtime_error("(AR) النص المشفر غير صالح: طول غير زوجي / (EN) Invalid encrypted text: odd length");
        std::string bytes;
        bytes.reserve(hex.size() / 2);
        for (size_t i = 0; i + 1 < hex.size(); i += 2) {
            std::string hb = hex.substr(i, 2);
            for (char c : hb)
                if (!std::isxdigit(static_cast<unsigned char>(c)))
                    throw std::runtime_error("(AR) حرف hex غير صالح / (EN) Invalid hex char");
            bytes += (char)std::stoi(hb, nullptr, 16);
        }
        return bytes;
    };

    // شفّر / encrypt — تشفير SHA-256-CTR (تشفير تيار آمن)
    auto encrypt_func = [sha256_raw](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) شفّر: يحتاج نص ومفتاح / (EN) encrypt: needs text and key");
        std::string text = args[0]->toString();
        std::string key = args[1]->toString();
        if (key.empty()) throw std::runtime_error("(AR) المفتاح فارغ / (EN) Key is empty");

        // (AR) توليد nonce عشوائي 8 بايت
        std::random_device rd;
        std::mt19937_64 rng(rd());
        uint64_t nonce = rng();

        // (AR) تشفير CTR — SHA-256(key || nonce || counter) كتيار مفاتيح
        std::string cipher;
        cipher.reserve(8 + text.size());
        for (int i = 7; i >= 0; --i) cipher += (char)((nonce >> (i * 8)) & 0xFF);

        size_t pos = 0;
        uint64_t ctr = 0;
        while (pos < text.size()) {
            std::vector<uint8_t> input(key.begin(), key.end());
            for (int i = 7; i >= 0; --i) input.push_back((uint8_t)((nonce >> (i * 8)) & 0xFF));
            for (int i = 7; i >= 0; --i) input.push_back((uint8_t)((ctr >> (i * 8)) & 0xFF));
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
    interpreter.getFunctionManager().registerBuiltinFunction("شفّر", encrypt_func);
    interpreter.getFunctionManager().registerBuiltinFunction("encrypt", encrypt_func);

    // فك_تشفير / decrypt — فك تشفير SHA-256-CTR
    auto decrypt_func = [sha256_raw, hexToBytes](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) فك_تشفير: يحتاج نص_مشفر ومفتاح / (EN) decrypt: needs encrypted_text and key");
        std::string hex_text = args[0]->toString();
        std::string key = args[1]->toString();
        if (key.empty()) throw std::runtime_error("(AR) المفتاح فارغ / (EN) Key is empty");

        std::string raw = hexToBytes(hex_text);
        if (raw.size() < 8) throw std::runtime_error("(AR) البيانات المشفرة قصيرة جداً / (EN) Encrypted data too short");

        // (AR) استخراج nonce من أول 8 بايت
        uint64_t nonce = 0;
        for (int i = 0; i < 8; ++i) nonce = (nonce << 8) | (uint8_t)raw[i];

        // (AR) فك التشفير CTR
        std::string result;
        result.reserve(raw.size() - 8);
        size_t pos = 8;
        uint64_t ctr = 0;
        while (pos < raw.size()) {
            std::vector<uint8_t> input(key.begin(), key.end());
            for (int i = 7; i >= 0; --i) input.push_back((uint8_t)((nonce >> (i * 8)) & 0xFF));
            for (int i = 7; i >= 0; --i) input.push_back((uint8_t)((ctr >> (i * 8)) & 0xFF));
            auto block = sha256_raw(input);
            for (size_t j = 0; j < 32 && pos < raw.size(); ++j, ++pos)
                result += (char)((uint8_t)raw[pos] ^ block[j]);
            ++ctr;
        }
        return std::make_shared<Data::Value>(result);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("فك_تشفير", decrypt_func);
    interpreter.getFunctionManager().registerBuiltinFunction("decrypt", decrypt_func);

    // تأكد_نوع / assert_type — يتحقق من نوع القيمة
    auto assert_type_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) تأكد_نوع: يحتاج قيمة واسم_نوع / (EN) assert_type: needs value and type_name");
        std::string actual_type = args[0]->getTypeName();
        std::string expected_type = args[1]->toString();
        // خريطة ترجمة الأنواع العربية ↔ الإنجليزية
        static const std::unordered_map<std::string, std::string> type_map = {
            {"رقم", "INTEGER"}, {"عدد", "INTEGER"}, {"صحيح", "INTEGER"}, {"integer", "INTEGER"}, {"int", "INTEGER"},
            {"عشري", "DOUBLE"}, {"حقيقي", "DOUBLE"}, {"double", "DOUBLE"}, {"float", "DOUBLE"},
            {"نص", "STRING"}, {"سلسلة", "STRING"}, {"string", "STRING"}, {"str", "STRING"},
            {"منطقي", "BOOLEAN"}, {"bool", "BOOLEAN"}, {"boolean", "BOOLEAN"},
            {"مصفوفة", "ARRAY"}, {"array", "ARRAY"}, {"قائمة", "ARRAY"},
            {"قاموس", "MAP"}, {"خريطة", "MAP"}, {"map", "MAP"},
            {"فراغ", "VOID"}, {"لاشيء", "VOID"}, {"void", "VOID"}, {"none", "VOID"}
        };
        std::string normalized = expected_type;
        auto it = type_map.find(expected_type);
        if (it != type_map.end()) {
            normalized = it->second;
        }
        // أيضاً تحويل actual_type لكبيرة للمقارنة
        std::string actual_upper = actual_type;
        for (auto& c : actual_upper) c = toupper(c);
        if (actual_upper != normalized && actual_type != expected_type) {
            throw std::runtime_error("(AR) خطأ نوع: توقعت '" + expected_type + "' لكن وجدت '" + actual_type + "' / (EN) Type error: expected '" + expected_type + "' but got '" + actual_type + "'");
        }
        return std::make_shared<Data::Value>(true);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("تأكد_نوع", assert_type_func);
    interpreter.getFunctionManager().registerBuiltinFunction("assert_type", assert_type_func);

    // تأكد_مساواة / assert_equal — يتحقق من مساواة قيمتين
    auto assert_equal_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) تأكد_مساواة: يحتاج قيمتين / (EN) assert_equal: needs 2 values");
        bool equal = (args[0]->toString() == args[1]->toString());
        if (!equal) {
            std::string msg = "(AR) فشل التأكيد: '" + args[0]->toString() + "' لا يساوي '" + args[1]->toString() + "' / (EN) Assertion failed: '" + args[0]->toString() + "' != '" + args[1]->toString() + "'";
            if (args.size() >= 3) {
                msg = "(AR) فشل: " + args[2]->toString() + " / (EN) Failed: " + args[2]->toString();
            }
            throw std::runtime_error(msg);
        }
        return std::make_shared<Data::Value>(true);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("تأكد_مساواة", assert_equal_func);
    interpreter.getFunctionManager().registerBuiltinFunction("assert_equal", assert_equal_func);

    // تأكد_أكبر / assert_greater — يتحقق أن القيمة أكبر
    auto assert_greater_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) تأكد_أكبر: يحتاج قيمتين / (EN) assert_greater: needs 2 values");
        double a = args[0]->toDouble();
        double b = args[1]->toDouble();
        if (!(a > b)) {
            throw std::runtime_error("(AR) فشل التأكيد: " + args[0]->toString() + " ليس أكبر من " + args[1]->toString() + " / (EN) Assertion failed: " + args[0]->toString() + " not greater than " + args[1]->toString());
        }
        return std::make_shared<Data::Value>(true);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("تأكد_أكبر", assert_greater_func);
    interpreter.getFunctionManager().registerBuiltinFunction("assert_greater", assert_greater_func);

    // نظّف / sanitize — تنظيف نص من الأحرف الخطيرة (XSS/SQL Injection)
    auto sanitize_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) نظّف: يحتاج نص / (EN) sanitize: needs a string");
        std::string input = args[0]->toString();
        std::string result;
        for (char c : input) {
            switch (c) {
                case '<': result += "&lt;"; break;
                case '>': result += "&gt;"; break;
                case '&': result += "&amp;"; break;
                case '"': result += "&quot;"; break;
                case '\'': result += "&#39;"; break;
                case ';': result += ""; break;  // Remove semicolons (SQL injection)
                case '-': 
                    if (!result.empty() && result.back() == '-') { result.pop_back(); break; } // -- comments
                    result += c; break;
                default: result += c; break;
            }
        }
        return std::make_shared<Data::Value>(result);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("نظّف", sanitize_func);
    interpreter.getFunctionManager().registerBuiltinFunction("sanitize", sanitize_func);

    // وقت_الآن / timestamp — الوقت الحالي (Unix timestamp)
    auto timestamp_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto now = std::chrono::system_clock::now();
        auto epoch = now.time_since_epoch();
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(epoch).count();
        return std::make_shared<Data::Value>(static_cast<double>(seconds));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("وقت_الآن", timestamp_func);
    interpreter.getFunctionManager().registerBuiltinFunction("timestamp", timestamp_func);

    // عشوائي_آمن / secure_random — عدد عشوائي آمن (crypto-grade)
    auto secure_random_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int min_val = 0, max_val = 100;
        if (args.size() >= 1) min_val = static_cast<int>(args[0]->toDouble());
        if (args.size() >= 2) max_val = static_cast<int>(args[1]->toDouble());
        // Use random_device for crypto-grade randomness
        std::random_device rd;
        std::uniform_int_distribution<int> dist(min_val, max_val);
        return std::make_shared<Data::Value>(static_cast<double>(dist(rd)));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عشوائي_آمن", secure_random_func);
    interpreter.getFunctionManager().registerBuiltinFunction("secure_random", secure_random_func);

    // base64_encode / ترميز_64
    auto base64_encode_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) ترميز_64: يحتاج نص / (EN) base64_encode: needs a string");
        const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        std::string input = args[0]->toString();
        std::string result;
        int val = 0, valb = -6;
        for (unsigned char c : input) {
            val = (val << 8) + c;
            valb += 8;
            while (valb >= 0) {
                result.push_back(chars[(val >> valb) & 0x3F]);
                valb -= 6;
            }
        }
        if (valb > -6) result.push_back(chars[((val << 8) >> (valb + 8)) & 0x3F]);
        while (result.size() % 4) result.push_back('=');
        return std::make_shared<Data::Value>(result);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ترميز_64", base64_encode_func);
    interpreter.getFunctionManager().registerBuiltinFunction("base64_encode", base64_encode_func);

    // ===================================================================
    // القسم 13: دوال FFI (واجهة الدوال الخارجية C/C++)
    // Section 13: FFI Functions (C/C++ Foreign Function Interface)
    // ===================================================================

    // ═══════════════════════════════════════════════════════════════════
    // (AR) جدول مؤشرات مُدار — يخزّن العناوين الحقيقية بمعرّف int آمن
    //      بدلاً من تخزين uintptr_t كـ double (يفقد الدقة فوق 2^52)
    // (EN) Managed pointer table — stores real addresses with safe int IDs
    //      instead of storing uintptr_t as double (loses precision above 2^52)
    // ═══════════════════════════════════════════════════════════════════
    static std::unordered_map<int, void*> ptrTable;
    static int nextPtrId = 1;
    static constexpr size_t MAX_FFI_ALLOC_SIZE = 256 * 1024 * 1024; // 256MB حد أقصى للحجز
    static constexpr size_t MAX_FFI_ALLOC_COUNT = 10000;            // حد أقصى لعدد الحجوزات

    // (AR) دالة مساعدة لتسجيل مؤشر وإرجاع معرّفه
    // (EN) Helper to register a pointer and return its ID
    auto registerPtr = [](void* ptr) -> int {
        if (!ptr) return 0;
        int id = nextPtrId++;
        ptrTable[id] = ptr;
        return id;
    };

    // (AR) دالة مساعدة لجلب المؤشر من معرّفه
    // (EN) Helper to get pointer from its ID
    auto getPtr = [](int id) -> void* {
        auto it = ptrTable.find(id);
        return (it != ptrTable.end()) ? it->second : nullptr;
    };

    // (AR) دالة مساعدة لإزالة مؤشر من الجدول
    // (EN) Helper to remove pointer from table
    auto removePtr = [](int id) -> void* {
        auto it = ptrTable.find(id);
        if (it == ptrTable.end()) return nullptr;
        void* ptr = it->second;
        ptrTable.erase(it);
        return ptr;
    };

    // 1. printf — طباعة_تنسيق / formatted print
    auto ffi_printf_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) printf: يحتاج نص التنسيق / (EN) printf: needs format string");
        std::string fmt = args[0]->toString();
        // Simple printf simulation: replace %d, %s, %f, %p with argument values
        std::string result;
        size_t argIdx = 1;
        for (size_t i = 0; i < fmt.size(); i++) {
            if (fmt[i] == '%' && i + 1 < fmt.size()) {
                char spec = fmt[i + 1];
                if (spec == 'd' || spec == 'i') {
                    if (argIdx < args.size()) result += std::to_string(static_cast<int>(args[argIdx++]->toDouble()));
                    else result += "0";
                    i++;
                } else if (spec == 'f') {
                    if (argIdx < args.size()) {
                        char buf[64]; std::snprintf(buf, sizeof(buf), "%f", args[argIdx++]->toDouble());
                        result += buf;
                    } else result += "0.000000";
                    i++;
                } else if (spec == 's') {
                    if (argIdx < args.size()) result += args[argIdx++]->toString();
                    else result += "(null)";
                    i++;
                } else if (spec == 'p') {
                    if (argIdx < args.size()) {
                        char buf[32]; std::snprintf(buf, sizeof(buf), "%p", (void*)(uintptr_t)static_cast<int64_t>(args[argIdx++]->toDouble()));
                        result += buf;
                    } else result += "0x0";
                    i++;
                } else if (spec == '%') {
                    result += '%'; i++;
                } else {
                    result += '%';
                }
            } else if (fmt[i] == '\\' && i + 1 < fmt.size() && fmt[i + 1] == 'n') {
                result += '\n'; i++;
            } else {
                result += fmt[i];
            }
        }
        std::cout << result;
        return std::make_shared<Data::Value>(static_cast<int>(result.size()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("طباعة_تنسيق", ffi_printf_func);
    interpreter.getFunctionManager().registerBuiltinFunction("printf", ffi_printf_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_printf", ffi_printf_func);

    // 2. malloc — حجز / allocate memory (managed pointer table)
    auto ffi_malloc_func = [&interpreter, registerPtr](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) malloc: يحتاج الحجم / (EN) malloc: needs size");
        // (AR) فحص أمني — حظر في وضع الأمان
        if (interpreter.getOptions().enableSecurity) {
            throw std::runtime_error("(AR) malloc محظورة في الوضع الآمن — استخدم مصفوفة بدلاً منها / (EN) malloc blocked in secure mode — use arrays instead");
        }
        size_t size = static_cast<size_t>(args[0]->toDouble());
        if (size > MAX_FFI_ALLOC_SIZE) {
            throw std::runtime_error("(AR) malloc: الحجم " + std::to_string(size) + " يتجاوز الحد 256MB / (EN) malloc: size exceeds 256MB limit");
        }
        if (ptrTable.size() >= MAX_FFI_ALLOC_COUNT) {
            throw std::runtime_error("(AR) malloc: تجاوز الحد الأقصى لعدد الحجوزات / (EN) malloc: exceeded max allocation count");
        }
        void* ptr = std::malloc(size);
        if (!ptr) return std::make_shared<Data::Value>(0);
        return std::make_shared<Data::Value>(registerPtr(ptr));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("حجز", ffi_malloc_func);
    interpreter.getFunctionManager().registerBuiltinFunction("malloc", ffi_malloc_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_malloc", ffi_malloc_func);

    // 3. free — حرر / free memory (managed pointer table)
    auto ffi_free_func = [&interpreter, removePtr](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) free: يحتاج مؤشر / (EN) free: needs pointer");
        if (interpreter.getOptions().enableSecurity) {
            throw std::runtime_error("(AR) free محظورة في الوضع الآمن / (EN) free blocked in secure mode");
        }
        int id = static_cast<int>(args[0]->toDouble());
        void* ptr = removePtr(id);
        if (ptr) std::free(ptr);
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("حرر", ffi_free_func);
    interpreter.getFunctionManager().registerBuiltinFunction("free", ffi_free_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_free", ffi_free_func);

    // 4. realloc — اعد_حجز (managed pointer table)
    auto ffi_realloc_func = [&interpreter, removePtr, registerPtr](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) realloc: يحتاج مؤشر وحجم / (EN) realloc: needs ptr and size");
        if (interpreter.getOptions().enableSecurity) {
            throw std::runtime_error("(AR) realloc محظورة في الوضع الآمن / (EN) realloc blocked in secure mode");
        }
        int id = static_cast<int>(args[0]->toDouble());
        size_t size = static_cast<size_t>(args[1]->toDouble());
        if (size > MAX_FFI_ALLOC_SIZE) {
            throw std::runtime_error("(AR) realloc: الحجم يتجاوز الحد 256MB / (EN) realloc: size exceeds 256MB limit");
        }
        void* oldPtr = removePtr(id);
        void* ptr = std::realloc(oldPtr, size);
        return std::make_shared<Data::Value>(registerPtr(ptr));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("اعد_حجز", ffi_realloc_func);
    interpreter.getFunctionManager().registerBuiltinFunction("realloc", ffi_realloc_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_realloc", ffi_realloc_func);

    // 5. calloc — حجز_صفري (managed pointer table)
    auto ffi_calloc_func = [&interpreter, registerPtr](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) calloc: يحتاج العدد والحجم / (EN) calloc: needs count and size");
        if (interpreter.getOptions().enableSecurity) {
            throw std::runtime_error("(AR) calloc محظورة في الوضع الآمن / (EN) calloc blocked in secure mode");
        }
        size_t count = static_cast<size_t>(args[0]->toDouble());
        size_t size = static_cast<size_t>(args[1]->toDouble());
        if (count * size > MAX_FFI_ALLOC_SIZE) {
            throw std::runtime_error("(AR) calloc: الحجم الكلي يتجاوز الحد 256MB / (EN) calloc: total size exceeds 256MB limit");
        }
        if (ptrTable.size() >= MAX_FFI_ALLOC_COUNT) {
            throw std::runtime_error("(AR) calloc: تجاوز الحد الأقصى لعدد الحجوزات / (EN) calloc: exceeded max allocation count");
        }
        void* ptr = std::calloc(count, size);
        return std::make_shared<Data::Value>(registerPtr(ptr));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("حجز_صفري", ffi_calloc_func);
    interpreter.getFunctionManager().registerBuiltinFunction("calloc", ffi_calloc_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_calloc", ffi_calloc_func);

    // 6. strlen — طول_نص_س
    auto ffi_strlen_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) strlen: يحتاج نص / (EN) strlen: needs string");
        std::string s = args[0]->toString();
        return std::make_shared<Data::Value>(static_cast<double>(s.length()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("طول_نص_س", ffi_strlen_func);
    interpreter.getFunctionManager().registerBuiltinFunction("strlen", ffi_strlen_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_strlen", ffi_strlen_func);

    // 7. strcpy — انسخ_نص_س
    auto ffi_strcpy_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) strcpy: يحتاج نص / (EN) strcpy: needs string");
        // In interpreted mode, just return a copy of the string
        return std::make_shared<Data::Value>(args[0]->toString());
    };
    interpreter.getFunctionManager().registerBuiltinFunction("انسخ_نص_س", ffi_strcpy_func);
    interpreter.getFunctionManager().registerBuiltinFunction("strcpy", ffi_strcpy_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_strcpy", ffi_strcpy_func);

    // 8. strcmp — قارن_نص_س
    auto ffi_strcmp_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) strcmp: يحتاج نصين / (EN) strcmp: needs two strings");
        int result = args[0]->toString().compare(args[1]->toString());
        return std::make_shared<Data::Value>(static_cast<double>(result));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("قارن_نص_س", ffi_strcmp_func);
    interpreter.getFunctionManager().registerBuiltinFunction("strcmp", ffi_strcmp_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_strcmp", ffi_strcmp_func);

    // 9. strcat — الحق_نص_س
    auto ffi_strcat_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) strcat: يحتاج نصين / (EN) strcat: needs two strings");
        return std::make_shared<Data::Value>(args[0]->toString() + args[1]->toString());
    };
    interpreter.getFunctionManager().registerBuiltinFunction("الحق_نص_س", ffi_strcat_func);
    interpreter.getFunctionManager().registerBuiltinFunction("strcat", ffi_strcat_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_strcat", ffi_strcat_func);

    // 10. memcpy — انسخ_ذاكرة_س (simulated with string copy in interpreter)
    auto ffi_memcpy_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) memcpy: يحتاج مصدر وحجم / (EN) memcpy: needs source and size");
        // In interpreted mode, copy string bytes
        std::string src = args[0]->toString();
        size_t n = args.size() >= 3 ? static_cast<size_t>(args[2]->toDouble()) : src.size();
        return std::make_shared<Data::Value>(src.substr(0, n));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("انسخ_ذاكرة_س", ffi_memcpy_func);
    interpreter.getFunctionManager().registerBuiltinFunction("memcpy", ffi_memcpy_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_memcpy", ffi_memcpy_func);

    // 11. memset — عبئ_ذاكرة_س (simulated)
    auto ffi_memset_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 3) throw std::runtime_error("(AR) memset: يحتاج مؤشر وقيمة وحجم / (EN) memset: needs ptr, value, size");
        char ch = static_cast<char>(static_cast<int>(args[1]->toDouble()));
        size_t n = static_cast<size_t>(args[2]->toDouble());
        return std::make_shared<Data::Value>(std::string(n, ch));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عبئ_ذاكرة_س", ffi_memset_func);
    interpreter.getFunctionManager().registerBuiltinFunction("memset", ffi_memset_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_memset", ffi_memset_func);

    // 12. fopen — افتح_ملف_س (managed pointer table + security check)
    auto ffi_fopen_func = [&interpreter, registerPtr](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) fopen: يحتاج اسم الملف والوضع / (EN) fopen: needs filename and mode");
        std::string filename = args[0]->toString();
        std::string mode = args[1]->toString();
        // (AR) فحص أمني — حظر مسارات خطيرة في الوضع الآمن
        // (EN) Security check — block dangerous paths in secure mode
        if (interpreter.getOptions().enableSecurity) {
            // (AR) حظر المسارات التي تحتوي على تصعيد (..)
            if (filename.find("..") != std::string::npos) {
                throw std::runtime_error("(AR) fopen: مسار غير مسموح في الوضع الآمن (تصعيد مسار) / (EN) fopen: path not allowed in secure mode (path traversal)");
            }
            // (AR) حظر المسارات المطلقة في Unix/Windows
            if (filename[0] == '/' || (filename.size() >= 2 && filename[1] == ':')) {
                throw std::runtime_error("(AR) fopen: مسارات مطلقة محظورة في الوضع الآمن / (EN) fopen: absolute paths blocked in secure mode");
            }
        }
        FILE* fp = std::fopen(filename.c_str(), mode.c_str());
        if (!fp) return std::make_shared<Data::Value>(0);
        return std::make_shared<Data::Value>(registerPtr(fp));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("افتح_ملف_س", ffi_fopen_func);
    interpreter.getFunctionManager().registerBuiltinFunction("fopen", ffi_fopen_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_fopen", ffi_fopen_func);

    // 13. fclose — اغلق_ملف_س (managed pointer table)
    auto ffi_fclose_func = [removePtr](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) fclose: يحتاج مؤشر ملف / (EN) fclose: needs file pointer");
        int id = static_cast<int>(args[0]->toDouble());
        if (id == 0) return std::make_shared<Data::Value>(-1);
        void* ptr = removePtr(id);
        if (!ptr) return std::make_shared<Data::Value>(-1);
        int result = std::fclose(reinterpret_cast<FILE*>(ptr));
        return std::make_shared<Data::Value>(static_cast<double>(result));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("اغلق_ملف_س", ffi_fclose_func);
    interpreter.getFunctionManager().registerBuiltinFunction("fclose", ffi_fclose_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_fclose", ffi_fclose_func);

    // 14. fputs — اكتب_ملف_س (managed pointer table)
    auto ffi_fputs_func = [getPtr](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) fputs: يحتاج نص ومؤشر ملف / (EN) fputs: needs string and file pointer");
        std::string text = args[0]->toString();
        int id = static_cast<int>(args[1]->toDouble());
        if (id == 0) return std::make_shared<Data::Value>(-1);
        void* ptr = getPtr(id);
        if (!ptr) return std::make_shared<Data::Value>(-1);
        int result = std::fputs(text.c_str(), reinterpret_cast<FILE*>(ptr));
        return std::make_shared<Data::Value>(static_cast<double>(result));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("اكتب_ملف_س", ffi_fputs_func);
    interpreter.getFunctionManager().registerBuiltinFunction("fputs", ffi_fputs_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_fputs", ffi_fputs_func);

    // 15. fgets — اقرأ_ملف_س
    auto ffi_fgets_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) fgets: يحتاج حجم ومؤشر ملف / (EN) fgets: needs size and file pointer");
        int size = static_cast<int>(args[0]->toDouble());
        uintptr_t addr = static_cast<uintptr_t>(args[1]->toDouble());
        if (addr == 0 || size <= 0) return std::make_shared<Data::Value>(std::string(""));
        std::vector<char> buf(size);
        char* result = std::fgets(buf.data(), size, reinterpret_cast<FILE*>(addr));
        if (!result) return std::make_shared<Data::Value>(std::string(""));
        return std::make_shared<Data::Value>(std::string(buf.data()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("اقرأ_ملف_س", ffi_fgets_func);
    interpreter.getFunctionManager().registerBuiltinFunction("fgets", ffi_fgets_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_fgets", ffi_fgets_func);

    // 16. system — نفذ_امر
    // (AR) أضيفت حماية أمنية: يُمنع تنفيذ أوامر النظام في الوضع الآمن
    // (EN) Added security: system commands blocked in secure mode
    auto ffi_system_func = [&interpreter](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        // (AR) فحص وضع الأمان / (EN) Check security mode
        if (interpreter.getOptions().enableSecurity) {
            throw std::runtime_error(
                "(AR) خطأ أمني: تنفيذ أوامر النظام محظور في الوضع الآمن. استخدم --no-security لتعطيل الحماية.\n"
                "(EN) Security error: System command execution is blocked in secure mode. Use --no-security to disable."
            );
        }
        if (args.empty()) throw std::runtime_error("(AR) system: يحتاج أمر / (EN) system: needs command");
        std::string cmd = args[0]->toString();
        
        // (AR) فحص أوامر خطرة حتى خارج الوضع الآمن
        // (EN) Check dangerous commands even outside secure mode
        static const std::vector<std::string> dangerousPatterns = {
            "rm -rf /", "del /f /s /q C:\\", "format ", "mkfs.", 
            ":(){ :|:", "shutdown", "reboot"
        };
        for (const auto& pattern : dangerousPatterns) {
            if (cmd.find(pattern) != std::string::npos) {
                throw std::runtime_error(
                    "(AR) خطأ أمني: الأمر يحتوي على نمط خطير: " + pattern + 
                    "\n(EN) Security error: Command contains dangerous pattern: " + pattern
                );
            }
        }
        
        int result = std::system(cmd.c_str());
        return std::make_shared<Data::Value>(static_cast<double>(result));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("نفذ_امر", ffi_system_func);
    interpreter.getFunctionManager().registerBuiltinFunction("system", ffi_system_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_system", ffi_system_func);

    // 17. getenv — قيمة_بيئة
    auto ffi_getenv_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) getenv: يحتاج اسم المتغير / (EN) getenv: needs variable name");
        const char* val = std::getenv(args[0]->toString().c_str());
        if (!val) return std::make_shared<Data::Value>(std::string(""));
        return std::make_shared<Data::Value>(std::string(val));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("قيمة_بيئة", ffi_getenv_func);
    interpreter.getFunctionManager().registerBuiltinFunction("getenv", ffi_getenv_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_getenv", ffi_getenv_func);

    // 18. atoi — نص_لعدد
    auto ffi_atoi_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) atoi: يحتاج نص / (EN) atoi: needs string");
        int result = std::atoi(args[0]->toString().c_str());
        return std::make_shared<Data::Value>(static_cast<double>(result));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("نص_لعدد", ffi_atoi_func);
    interpreter.getFunctionManager().registerBuiltinFunction("atoi", ffi_atoi_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_atoi", ffi_atoi_func);

    // 19. atof — نص_لعشري
    auto ffi_atof_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) atof: يحتاج نص / (EN) atof: needs string");
        double result = std::atof(args[0]->toString().c_str());
        return std::make_shared<Data::Value>(result);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("نص_لعشري", ffi_atof_func);
    interpreter.getFunctionManager().registerBuiltinFunction("atof", ffi_atof_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_atof", ffi_atof_func);

    // 20. snprintf — تنسيق_نص
    auto ffi_snprintf_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) snprintf: يحتاج نص التنسيق / (EN) snprintf: needs format string");
        std::string fmt = args[0]->toString();
        // Reuse printf-style formatting to produce a string
        std::string result;
        size_t argIdx = 1;
        for (size_t i = 0; i < fmt.size(); i++) {
            if (fmt[i] == '%' && i + 1 < fmt.size()) {
                char spec = fmt[i + 1];
                if (spec == 'd' || spec == 'i') {
                    if (argIdx < args.size()) result += std::to_string(static_cast<int>(args[argIdx++]->toDouble()));
                    else result += "0";
                    i++;
                } else if (spec == 'f') {
                    if (argIdx < args.size()) {
                        char buf[64]; std::snprintf(buf, sizeof(buf), "%f", args[argIdx++]->toDouble());
                        result += buf;
                    } else result += "0.000000";
                    i++;
                } else if (spec == 's') {
                    if (argIdx < args.size()) result += args[argIdx++]->toString();
                    else result += "(null)";
                    i++;
                } else if (spec == '%') {
                    result += '%'; i++;
                } else {
                    result += '%';
                }
            } else if (fmt[i] == '\\' && i + 1 < fmt.size() && fmt[i + 1] == 'n') {
                result += '\n'; i++;
            } else {
                result += fmt[i];
            }
        }
        return std::make_shared<Data::Value>(result);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("تنسيق_نص", ffi_snprintf_func);
    interpreter.getFunctionManager().registerBuiltinFunction("snprintf", ffi_snprintf_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_snprintf", ffi_snprintf_func);

}

} // namespace Interpreter
} // namespace Sad
