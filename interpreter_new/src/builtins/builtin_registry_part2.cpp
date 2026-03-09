/**
 * @file builtin_registry_part2.cpp
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
#include <queue>
#include <unordered_map>
#include <condition_variable>

namespace Sad {
namespace Interpreter {

using namespace StdLib;

void registerBuiltinsPart2(Interpreter& interpreter) {
    // exit - إنهاء البرنامج
    auto other_exit_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::exit(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("اخرج", other_exit_func);
    interpreter.getFunctionManager().registerBuiltinFunction("exit", other_exit_func);
    interpreter.getFunctionManager().registerBuiltinFunction("أنه", other_exit_func);
    interpreter.getFunctionManager().registerBuiltinFunction("quit", other_exit_func);
    
    // assert - التحقق من شرط
    auto other_assert_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::assert(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("تأكد", other_assert_func);
    interpreter.getFunctionManager().registerBuiltinFunction("assert", other_assert_func);
    interpreter.getFunctionManager().registerBuiltinFunction("تحقق", other_assert_func);
    
    // ===================================================================
    // Phase 22: Filesystem Module Functions (Part 1 - File I/O & Directories)
    // ===================================================================
    
    // read_lines - قراءة أسطر الملف
    auto fs_read_lines_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) throw std::runtime_error("(AR) اقرأ_أسطر تتطلب مسار الملف / (EN) read_lines requires file path");
        std::string path = args[0]->toString();
        auto lines = sad::stdlib::filesystem::read_lines(path);
        std::vector<Data::Value> result;
        for (const auto& line : lines) {
            result.push_back(Data::Value(line));
        }
        return std::make_shared<Data::Value>(result);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("اقرأ_أسطر", fs_read_lines_func);
    interpreter.getFunctionManager().registerBuiltinFunction("read_lines", fs_read_lines_func);
    interpreter.getFunctionManager().registerBuiltinFunction("قراءة_أسطر", fs_read_lines_func);
    
    // append_to_file - إضافة إلى ملف
    auto fs_append_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 2) throw std::runtime_error("(AR) أضف_إلى_ملف تتطلب مسار ومحتوى / (EN) append_to_file requires path and content");
        std::string path = args[0]->toString();
        std::string content = args[1]->toString();
        sad::stdlib::filesystem::append_to_file(path, content);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("أضف_إلى_ملف", fs_append_func);
    interpreter.getFunctionManager().registerBuiltinFunction("append_to_file", fs_append_func);
    interpreter.getFunctionManager().registerBuiltinFunction("أضف_لملف", fs_append_func);
    
    // copy_file - نسخ ملف
    auto fs_copy_file_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 2) throw std::runtime_error("(AR) انسخ_ملف تتطلب مصدراً ووجهة / (EN) copy_file requires source and destination");
        std::string source = args[0]->toString();
        std::string dest = args[1]->toString();
        bool overwrite = args.size() > 2 ? args[2]->toBool() : false;
        sad::stdlib::filesystem::copy_file(source, dest, overwrite);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("انسخ_ملف", fs_copy_file_func);
    interpreter.getFunctionManager().registerBuiltinFunction("copy_file", fs_copy_file_func);
    interpreter.getFunctionManager().registerBuiltinFunction("نسخ_ملف", fs_copy_file_func);
    
    // move_file - نقل ملف
    auto fs_move_file_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 2) throw std::runtime_error("(AR) انقل_ملف تتطلب مصدراً ووجهة / (EN) move_file requires source and destination");
        std::string source = args[0]->toString();
        std::string dest = args[1]->toString();
        sad::stdlib::filesystem::move_file(source, dest);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("انقل_ملف", fs_move_file_func);
    interpreter.getFunctionManager().registerBuiltinFunction("move_file", fs_move_file_func);
    interpreter.getFunctionManager().registerBuiltinFunction("نقل_ملف", fs_move_file_func);
    
    // delete_file - حذف ملف
    auto fs_delete_file_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) throw std::runtime_error("(AR) احذف_ملف تتطلب مسار الملف / (EN) delete_file requires file path");
        std::string path = args[0]->toString();
        bool result = sad::stdlib::filesystem::delete_file(path);
        return std::make_shared<Data::Value>(result);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("احذف_ملف", fs_delete_file_func);
    interpreter.getFunctionManager().registerBuiltinFunction("delete_file", fs_delete_file_func);
    interpreter.getFunctionManager().registerBuiltinFunction("حذف_ملف", fs_delete_file_func);
    
    // create_directory - إنشاء مجلد
    auto fs_create_dir_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) throw std::runtime_error("(AR) أنشئ_مجلد تتطلب مسار المجلد / (EN) create_directory requires path");
        std::string path = args[0]->toString();
        bool recursive = args.size() > 1 ? args[1]->toBool() : true;
        bool result = sad::stdlib::filesystem::create_directory(path, recursive);
        return std::make_shared<Data::Value>(result);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("أنشئ_مجلد", fs_create_dir_func);
    interpreter.getFunctionManager().registerBuiltinFunction("create_directory", fs_create_dir_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mkdir", fs_create_dir_func);
    
    // list_directory - سرد محتويات مجلد
    auto fs_list_dir_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) throw std::runtime_error("(AR) اسرد_مجلد تتطلب مسار المجلد / (EN) list_directory requires path");
        std::string path = args[0]->toString();
        auto entries = sad::stdlib::filesystem::list_directory(path);
        std::vector<Data::Value> result;
        for (const auto& entry : entries) {
            result.push_back(Data::Value(entry));
        }
        return std::make_shared<Data::Value>(result);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("اسرد_مجلد", fs_list_dir_func);
    interpreter.getFunctionManager().registerBuiltinFunction("list_directory", fs_list_dir_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ls", fs_list_dir_func);
    
    // remove_directory - حذف مجلد
    auto fs_remove_dir_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) throw std::runtime_error("(AR) احذف_مجلد تتطلب مسار المجلد / (EN) remove_directory requires path");
        std::string path = args[0]->toString();
        bool recursive = args.size() > 1 ? args[1]->toBool() : false;
        bool result = sad::stdlib::filesystem::remove_directory(path, recursive);
        return std::make_shared<Data::Value>(result);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("احذف_مجلد", fs_remove_dir_func);
    interpreter.getFunctionManager().registerBuiltinFunction("remove_directory", fs_remove_dir_func);
    interpreter.getFunctionManager().registerBuiltinFunction("rmdir", fs_remove_dir_func);
    
    // is_file - هل هو ملف
    auto fs_is_file_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) throw std::runtime_error("(AR) هل_ملف تتطلب مسار الملف / (EN) is_file requires path");
        std::string path = args[0]->toString();
        bool result = sad::stdlib::filesystem::is_file(path);
        return std::make_shared<Data::Value>(result);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("هل_ملف", fs_is_file_func);
    interpreter.getFunctionManager().registerBuiltinFunction("is_file", fs_is_file_func);
    
    // is_directory - هل هو مجلد
    auto fs_is_dir_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) throw std::runtime_error("(AR) هل_مجلد تتطلب مسار المجلد / (EN) is_directory requires path");
        std::string path = args[0]->toString();
        bool result = sad::stdlib::filesystem::is_directory(path);
        return std::make_shared<Data::Value>(result);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("هل_مجلد", fs_is_dir_func);
    interpreter.getFunctionManager().registerBuiltinFunction("is_directory", fs_is_dir_func);
    interpreter.getFunctionManager().registerBuiltinFunction("is_dir", fs_is_dir_func);
    
    // ===================================================================
    // (AR) دوال المدى والتكرار / (EN) Range & Iteration Functions
    // ===================================================================
    
    auto range_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::range(args);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("مدى", range_func);
    interpreter.getFunctionManager().registerBuiltinFunction("range", range_func);
    
    // ===================================================================
    // (AR) دوال نظام الملفات / (EN) Filesystem Functions
    // ===================================================================
    
    // دالة قراءة ملف / Read file function
    auto read_file_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty() || !args[0]) {
            throw std::runtime_error("(AR) اقرأ_ملف: المعامل الأول مطلوب (مسار الملف) / (EN) read_file: First argument required (file path)");
        }
        std::string path = args[0]->toString();
        
        std::ifstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("(AR) اقرأ_ملف: فشل فتح الملف '" + path + "' / (EN) read_file: Failed to open file '" + path + "'");
        }
        
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        
        return std::make_shared<Data::Value>(content);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("اقرأ_ملف", read_file_func);
    interpreter.getFunctionManager().registerBuiltinFunction("read_file", read_file_func);
    
    // دالة كتابة ملف / Write file function
    auto write_file_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 2 || !args[0] || !args[1]) {
            throw std::runtime_error("(AR) اكتب_ملف: معاملان مطلوبان (مسار، محتوى) / (EN) write_file: Two arguments required (path, content)");
        }
        
        std::string path = args[0]->toString();
        std::string content = args[1]->toString();
        
        std::ofstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("(AR) اكتب_ملف: فشل فتح الملف '" + path + "' للكتابة / (EN) write_file: Failed to open file '" + path + "' for writing");
        }
        
        file << content;
        file.close();
        
        return std::make_shared<Data::Value>();  // void
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("اكتب_ملف", write_file_func);
    interpreter.getFunctionManager().registerBuiltinFunction("write_file", write_file_func);
    
    // دالة إضافة لملف / Append to file function
    auto append_file_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 2 || !args[0] || !args[1]) {
            throw std::runtime_error("(AR) أضف_إلى_ملف: معاملان مطلوبان / (EN) append_to_file: Two arguments required");
        }
        
        std::string path = args[0]->toString();
        std::string content = args[1]->toString();
        
        std::ofstream file(path, std::ios::app);
        if (!file.is_open()) {
            throw std::runtime_error("(AR) أضف_إلى_ملف: فشل فتح الملف / (EN) append_to_file: Failed to open file");
        }
        
        file << content;
        file.close();
        
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("أضف_إلى_ملف", append_file_func);
    interpreter.getFunctionManager().registerBuiltinFunction("append_to_file", append_file_func);
    
    // دالة حذف ملف / Delete file function
    auto delete_file_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty() || !args[0]) {
            throw std::runtime_error("(AR) احذف_ملف: المعامل الأول مطلوب / (EN) delete_file: First argument required");
        }
        
        std::string path = args[0]->toString();
        
        if (std::filesystem::exists(path)) {
            std::filesystem::remove(path);
        } else {
            throw std::runtime_error("(AR) احذف_ملف: الملف غير موجود / (EN) delete_file: File not found");
        }
        
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("احذف_ملف", delete_file_func);
    interpreter.getFunctionManager().registerBuiltinFunction("delete_file", delete_file_func);
    
    // دالة التحقق من وجود ملف / Check if file exists
    auto file_exists_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty() || !args[0]) {
            throw std::runtime_error("(AR) هل_موجود: المعامل الأول مطلوب / (EN) exists: First argument required");
        }
        
        std::string path = args[0]->toString();
        bool exists = std::filesystem::exists(path);
        
        return std::make_shared<Data::Value>(exists);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("هل_موجود", file_exists_func);
    interpreter.getFunctionManager().registerBuiltinFunction("exists", file_exists_func);

#ifdef HAS_GRAPHICS
    // ===================================================================
    // (AR) دوال الرسومات / (EN) Graphics Functions
    // ===================================================================
    
    // ===== Window Management / إدارة النوافذ =====
    
    // Create window / إنشاء نافذة
    auto window_create_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 3) {
            throw std::runtime_error("(AR) نافذة_جديد: 3 معاملات مطلوبة (عنوان، عرض، ارتفاع) / (EN) window_new: 3 arguments required (title, width, height)");
        }
        
        std::string title = args[0]->toString();
        int width = args[1]->toInt();
        int height = args[2]->toInt();
        
        int windowId = sad::stdlib::graphics::window_create_impl(title, width, height);
        return std::make_shared<Data::Value>(windowId);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("نافذة_جديد", window_create_func);
    interpreter.getFunctionManager().registerBuiltinFunction("نافذة_جديدة", window_create_func);
    interpreter.getFunctionManager().registerBuiltinFunction("window_new", window_create_func);
    
    // Show window / عرض النافذة
    auto window_show_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) {
            throw std::runtime_error("(AR) نافذة_عرض: معامل النافذة مطلوب / (EN) window_show: Window argument required");
        }
        
        int windowId = args[0]->toInt();
        sad::stdlib::graphics::window_show_impl(windowId);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("نافذة_عرض", window_show_func);
    interpreter.getFunctionManager().registerBuiltinFunction("window_show", window_show_func);
    
    // Hide window / إخفاء النافذة
    auto window_hide_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) {
            throw std::runtime_error("(AR) نافذة_إخفاء: معامل النافذة مطلوب / (EN) window_hide: Window argument required");
        }
        
        int windowId = args[0]->toInt();
        sad::stdlib::graphics::window_hide_impl(windowId);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("نافذة_إخفاء", window_hide_func);
    interpreter.getFunctionManager().registerBuiltinFunction("window_hide", window_hide_func);
    
    // Close window / إغلاق النافذة
    auto window_close_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) {
            throw std::runtime_error("(AR) نافذة_إغلاق: معامل النافذة مطلوب / (EN) window_close: Window argument required");
        }
        
        int windowId = args[0]->toInt();
        sad::stdlib::graphics::window_close_impl(windowId);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("نافذة_إغلاق", window_close_func);
    interpreter.getFunctionManager().registerBuiltinFunction("window_close", window_close_func);
    
    // Should close window / هل يجب إغلاق النافذة
    auto window_should_close_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) {
            throw std::runtime_error("(AR) نافذة_يجب_الإغلاق: معامل النافذة مطلوب / (EN) window_should_close: Window argument required");
        }
        
        int windowId = args[0]->toInt();
        bool shouldClose = sad::stdlib::graphics::window_should_close_impl(windowId);
        return std::make_shared<Data::Value>(shouldClose);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("نافذة_يجب_الإغلاق", window_should_close_func);
    interpreter.getFunctionManager().registerBuiltinFunction("window_should_close", window_should_close_func);
    
    // Poll events / استقبال الأحداث
    auto window_poll_events_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) {
            throw std::runtime_error("(AR) نافذة_استقبال_أحداث: معامل النافذة مطلوب / (EN) window_poll_events: Window argument required");
        }
        
        int windowId = args[0]->toInt();
        sad::stdlib::graphics::window_poll_events_impl(windowId);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("نافذة_استقبال_أحداث", window_poll_events_func);
    interpreter.getFunctionManager().registerBuiltinFunction("window_poll_events", window_poll_events_func);
    
    // Swap buffers / تبديل buffers
    auto window_swap_buffers_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) {
            throw std::runtime_error("(AR) نافذة_تبديل_buffers: معامل النافذة مطلوب / (EN) window_swap_buffers: Window argument required");
        }
        
        int windowId = args[0]->toInt();
        sad::stdlib::graphics::window_swap_buffers_impl(windowId);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("نافذة_تبديل_buffers", window_swap_buffers_func);
    interpreter.getFunctionManager().registerBuiltinFunction("window_swap_buffers", window_swap_buffers_func);
    
    // ===== Renderer Management / إدارة الرسام =====
    
    // Create renderer / إنشاء رسام
    auto renderer_create_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) {
            throw std::runtime_error("(AR) رسام_ثنائي_جديد: معامل النافذة مطلوب / (EN) renderer_new: Window argument required");
        }
        
        int windowId = args[0]->toInt();
        int rendererId = sad::stdlib::graphics::renderer_create_impl(windowId);
        return std::make_shared<Data::Value>(rendererId);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("رسام_ثنائي_جديد", renderer_create_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_new", renderer_create_func);
    
    // Begin frame / بدء إطار
    auto renderer_begin_frame_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) {
            throw std::runtime_error("(AR) رسام_بدء_إطار: معامل الرسام مطلوب / (EN) renderer_begin_frame: Renderer argument required");
        }
        
        int rendererId = args[0]->toInt();
        sad::stdlib::graphics::renderer_begin_frame_impl(rendererId);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("رسام_بدء_إطار", renderer_begin_frame_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_begin_frame", renderer_begin_frame_func);
    
    // End frame / إنهاء إطار
    auto renderer_end_frame_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) {
            throw std::runtime_error("(AR) رسام_إنهاء_إطار: معامل الرسام مطلوب / (EN) renderer_end_frame: Renderer argument required");
        }
        
        int rendererId = args[0]->toInt();
        sad::stdlib::graphics::renderer_end_frame_impl(rendererId);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("رسام_إنهاء_إطار", renderer_end_frame_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_end_frame", renderer_end_frame_func);
    
    // Clear screen / مسح الشاشة
    auto renderer_clear_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 5) {
            throw std::runtime_error("(AR) رسام_مسح: 5 معاملات مطلوبة (رسام، أحمر، أخضر، أزرق، شفاف) / (EN) renderer_clear: 5 arguments required (renderer, r, g, b, a)");
        }
        
        int rendererId = args[0]->toInt();
        int r = args[1]->toInt();
        int g = args[2]->toInt();
        int b = args[3]->toInt();
        int a = args[4]->toInt();
        
        sad::stdlib::graphics::renderer_clear_impl(rendererId, r, g, b, a);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("رسام_مسح", renderer_clear_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_clear", renderer_clear_func);
    
    // Draw line / رسم خط
    auto renderer_draw_line_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 9) {
            throw std::runtime_error("(AR) رسام_خط: 9 معاملات مطلوبة / (EN) renderer_draw_line: 9 arguments required");
        }
        
        int rendererId = args[0]->toInt();
        float x1 = static_cast<float>(args[1]->toDouble());
        float y1 = static_cast<float>(args[2]->toDouble());
        float x2 = static_cast<float>(args[3]->toDouble());
        float y2 = static_cast<float>(args[4]->toDouble());
        int r = args[5]->toInt();
        int g = args[6]->toInt();
        int b = args[7]->toInt();
        int a = args[8]->toInt();
        
        sad::stdlib::graphics::renderer_draw_line_impl(rendererId, x1, y1, x2, y2, r, g, b, a);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("رسام_خط", renderer_draw_line_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_draw_line", renderer_draw_line_func);
    
    // Draw rectangle / رسم مستطيل
    auto renderer_draw_rect_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 10) {
            throw std::runtime_error("(AR) رسام_مستطيل: 10 معاملات مطلوبة / (EN) renderer_draw_rect: 10 arguments required");
        }
        
        int rendererId = args[0]->toInt();
        float x = static_cast<float>(args[1]->toDouble());
        float y = static_cast<float>(args[2]->toDouble());
        float width = static_cast<float>(args[3]->toDouble());
        float height = static_cast<float>(args[4]->toDouble());
        int r = args[5]->toInt();
        int g = args[6]->toInt();
        int b = args[7]->toInt();
        int a = args[8]->toInt();
        bool filled = args[9]->toBool();
        
        sad::stdlib::graphics::renderer_draw_rect_impl(rendererId, x, y, width, height, r, g, b, a, filled);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("رسام_مستطيل", renderer_draw_rect_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_draw_rect", renderer_draw_rect_func);
    
    // Draw circle / رسم دائرة
    auto renderer_draw_circle_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 9) {
            throw std::runtime_error("(AR) رسام_دائرة: 9 معاملات مطلوبة / (EN) renderer_draw_circle: 9 arguments required");
        }
        
        int rendererId = args[0]->toInt();
        float x = static_cast<float>(args[1]->toDouble());
        float y = static_cast<float>(args[2]->toDouble());
        float radius = static_cast<float>(args[3]->toDouble());
        int r = args[4]->toInt();
        int g = args[5]->toInt();
        int b = args[6]->toInt();
        int a = args[7]->toInt();
        bool filled = args[8]->toBool();
        
        sad::stdlib::graphics::renderer_draw_circle_impl(rendererId, x, y, radius, r, g, b, a, filled);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("رسام_دائرة", renderer_draw_circle_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_draw_circle", renderer_draw_circle_func);
    
    // Draw text / رسم نص
    auto renderer_draw_text_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 9) {
            throw std::runtime_error("(AR) رسام_نص: 9 معاملات مطلوبة / (EN) renderer_draw_text: 9 arguments required");
        }
        
        int rendererId = args[0]->toInt();
        std::string text = args[1]->toString();
        float x = static_cast<float>(args[2]->toDouble());
        float y = static_cast<float>(args[3]->toDouble());
        float size = static_cast<float>(args[4]->toDouble());
        int r = args[5]->toInt();
        int g = args[6]->toInt();
        int b = args[7]->toInt();
        int a = args[8]->toInt();
        
        sad::stdlib::graphics::renderer_draw_text_impl(rendererId, text, x, y, size, r, g, b, a);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("رسام_نص", renderer_draw_text_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_draw_text", renderer_draw_text_func);
    
    // ===== UI Widgets / عناصر الواجهة =====
    
    // Create label / إنشاء تسمية
    auto label_create_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 5) {
            throw std::runtime_error("(AR) تسمية_جديدة: 5 معاملات مطلوبة (نص، x، y، عرض، ارتفاع) / (EN) label_new: 5 arguments required (text, x, y, width, height)");
        }
        
        std::string text = args[0]->toString();
        float x = static_cast<float>(args[1]->toDouble());
        float y = static_cast<float>(args[2]->toDouble());
        float width = static_cast<float>(args[3]->toDouble());
        float height = static_cast<float>(args[4]->toDouble());
        
        int labelId = sad::stdlib::graphics::label_create_impl(text, x, y, width, height);
        return std::make_shared<Data::Value>(labelId);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("تسمية_جديدة", label_create_func);
    interpreter.getFunctionManager().registerBuiltinFunction("label_new", label_create_func);
    
    // Set label text / تعيين نص التسمية
    auto label_set_text_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 2) {
            throw std::runtime_error("(AR) تسمية_تعيين_نص: معاملان مطلوبان / (EN) label_set_text: 2 arguments required");
        }
        
        int labelId = args[0]->toInt();
        std::string text = args[1]->toString();
        sad::stdlib::graphics::label_set_text_impl(labelId, text);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("تسمية_تعيين_نص", label_set_text_func);
    interpreter.getFunctionManager().registerBuiltinFunction("label_set_text", label_set_text_func);
    
    // Get label text / الحصول على نص التسمية
    auto label_get_text_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) {
            throw std::runtime_error("(AR) تسمية_الحصول_على_نص: معامل التسمية مطلوب / (EN) label_get_text: Label argument required");
        }
        
        int labelId = args[0]->toInt();
        std::string text = sad::stdlib::graphics::label_get_text_impl(labelId);
        return std::make_shared<Data::Value>(text);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("تسمية_الحصول_على_نص", label_get_text_func);
    interpreter.getFunctionManager().registerBuiltinFunction("label_get_text", label_get_text_func);
    
    // Create button / إنشاء زر
    auto button_create_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 5) {
            throw std::runtime_error("(AR) زر_جديد: 5 معاملات مطلوبة / (EN) button_new: 5 arguments required");
        }
        
        std::string text = args[0]->toString();
        float x = static_cast<float>(args[1]->toDouble());
        float y = static_cast<float>(args[2]->toDouble());
        float width = static_cast<float>(args[3]->toDouble());
        float height = static_cast<float>(args[4]->toDouble());
        
        int buttonId = sad::stdlib::graphics::button_create_impl(text, x, y, width, height);
        return std::make_shared<Data::Value>(buttonId);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("زر_جديد", button_create_func);
    interpreter.getFunctionManager().registerBuiltinFunction("button_new", button_create_func);
    
    // Check button clicked / فحص النقر على الزر
    auto button_is_clicked_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) {
            throw std::runtime_error("(AR) زر_تم_النقر: معامل الزر مطلوب / (EN) button_is_clicked: Button argument required");
        }
        
        int buttonId = args[0]->toInt();
        bool clicked = sad::stdlib::graphics::button_is_clicked_impl(buttonId);
        return std::make_shared<Data::Value>(clicked);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("زر_تم_النقر", button_is_clicked_func);
    interpreter.getFunctionManager().registerBuiltinFunction("button_is_clicked", button_is_clicked_func);
    
    // Set button text / تعيين نص الزر
    auto button_set_text_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 2) {
            throw std::runtime_error("(AR) زر_تعيين_نص: معاملان مطلوبان / (EN) button_set_text: 2 arguments required");
        }
        
        int buttonId = args[0]->toInt();
        std::string text = args[1]->toString();
        sad::stdlib::graphics::button_set_text_impl(buttonId, text);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("زر_تعيين_نص", button_set_text_func);
    interpreter.getFunctionManager().registerBuiltinFunction("button_set_text", button_set_text_func);
    
    // Create text input / إنشاء حقل نص
    auto textinput_create_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 5) {
            throw std::runtime_error("(AR) حقل_نص_جديد: 5 معاملات مطلوبة / (EN) textinput_new: 5 arguments required");
        }
        
        std::string placeholder = args[0]->toString();
        float x = static_cast<float>(args[1]->toDouble());
        float y = static_cast<float>(args[2]->toDouble());
        float width = static_cast<float>(args[3]->toDouble());
        float height = static_cast<float>(args[4]->toDouble());
        
        int inputId = sad::stdlib::graphics::textinput_create_impl(placeholder, x, y, width, height);
        return std::make_shared<Data::Value>(inputId);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("حقل_نص_جديد", textinput_create_func);
    interpreter.getFunctionManager().registerBuiltinFunction("textinput_new", textinput_create_func);
    
    // Get text input value / الحصول على قيمة حقل النص
    auto textinput_get_value_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) {
            throw std::runtime_error("(AR) حقل_نص_الحصول_على_قيمة: معامل حقل النص مطلوب / (EN) textinput_get_value: TextInput argument required");
        }
        
        int inputId = args[0]->toInt();
        std::string value = sad::stdlib::graphics::textinput_get_value_impl(inputId);
        return std::make_shared<Data::Value>(value);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("حقل_نص_الحصول_على_قيمة", textinput_get_value_func);
    interpreter.getFunctionManager().registerBuiltinFunction("textinput_get_value", textinput_get_value_func);
    
    // Set text input value / تعيين قيمة حقل النص
    auto textinput_set_value_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 2) {
            throw std::runtime_error("(AR) حقل_نص_تعيين_قيمة: معاملان مطلوبان / (EN) textinput_set_value: 2 arguments required");
        }
        
        int inputId = args[0]->toInt();
        std::string value = args[1]->toString();
        sad::stdlib::graphics::textinput_set_value_impl(inputId, value);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("حقل_نص_تعيين_قيمة", textinput_set_value_func);
    interpreter.getFunctionManager().registerBuiltinFunction("textinput_set_value", textinput_set_value_func);
    
    // Create checkbox / إنشاء مربع اختيار
    auto checkbox_create_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 5) {
            throw std::runtime_error("(AR) مربع_اختيار_جديد: 5 معاملات مطلوبة / (EN) checkbox_new: 5 arguments required");
        }
        
        std::string label = args[0]->toString();
        float x = static_cast<float>(args[1]->toDouble());
        float y = static_cast<float>(args[2]->toDouble());
        float width = static_cast<float>(args[3]->toDouble());
        float height = static_cast<float>(args[4]->toDouble());
        
        int checkboxId = sad::stdlib::graphics::checkbox_create_impl(label, x, y, width, height);
        return std::make_shared<Data::Value>(checkboxId);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("مربع_اختيار_جديد", checkbox_create_func);
    interpreter.getFunctionManager().registerBuiltinFunction("checkbox_new", checkbox_create_func);
#endif // HAS_GRAPHICS
    

}

} // namespace Interpreter
} // namespace Sad
