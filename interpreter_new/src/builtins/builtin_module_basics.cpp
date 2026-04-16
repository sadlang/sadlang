/**
 * @file builtin_module_basics.cpp
 * @brief (AR) وحدة الأساسيات — الخروج والتأكيد وعمليات الملفات والمجلدات
 * @brief (EN) Basics module — exit, assert, file I/O, directory operations
 *
 * @details
 * (AR) الأقسام:
 *   1. دوال أساسية (اخرج، تأكد)
 *   2. عمليات الملفات (اقرأ_أسطر، اكتب_ملف، نسخ_ملف...)
 *   3. عمليات المجلدات (أنشئ_مجلد، حذف_مجلد...)
 *
 * @note يتطلب: builtin_common.h (الـ headers المشتركة)
 * @see builtin_registry.cpp — التسجيل المركزي
 * @see BUILTIN_CODING_STANDARDS.md — قواعد الكتابة
 */

#include "builtin_common.h"
#include <algorithm>
#include <condition_variable>
#include <cstdlib>
#include <fstream>

namespace Sad {
namespace Interpreter {

using namespace StdLib;

void registerBuiltinsBasics(Interpreter& interpreter) {
    // exit - إنهاء البرنامج
    auto other_exit_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::exit(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("اخرج", other_exit_func);
    interpreter.getFunctionManager().registerBuiltinFunction("exit", other_exit_func);
    interpreter.getFunctionManager().registerBuiltinFunction("quit", other_exit_func);
    
    // assert - التحقق من شرط
    auto other_assert_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::assert(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("تأكد", other_assert_func);
    interpreter.getFunctionManager().registerBuiltinFunction("assert", other_assert_func);
    
    // ═══════════════════════════════════════════════════════════════
    // Filesystem Module Functions (Part 1 - File I/O & Directories)
    
    // ═══════════════════════════════════════════════════════════════
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
    // delete_file - حذف ملف
    auto fs_delete_file_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) throw std::runtime_error("(AR) احذف_ملف تتطلب مسار الملف / (EN) delete_file requires file path");
        std::string path = args[0]->toString();
        bool result = sad::stdlib::filesystem::delete_file(path);
        return std::make_shared<Data::Value>(result);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("احذف_ملف", fs_delete_file_func);
    interpreter.getFunctionManager().registerBuiltinFunction("delete_file", fs_delete_file_func);
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
    
    
    // ═══════════════════════════════════════════════════════════════
    // (AR) دوال المدى والتكرار / (EN) Range & Iteration Functions
    
    // ═══════════════════════════════════════════════════════════════
    auto range_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::range(args);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("مدى", range_func);
    interpreter.getFunctionManager().registerBuiltinFunction("range", range_func);
    
    
    // ═══════════════════════════════════════════════════════════════
    // (AR) دوال نظام الملفات / (EN) Filesystem Functions
    
    // ═══════════════════════════════════════════════════════════════
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


    

}

} // namespace Interpreter
} // namespace Sad
