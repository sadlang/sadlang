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
// (AR) ثوابت أسماء الدوال المُولَّدة من YAML
#include "builtin_registry.h"
namespace Bb = Sad::Builtins::Names::Basics;
#include <algorithm>
#include <condition_variable>
#include <cstdlib>
#include <fstream>

namespace Sad {
namespace Interpreter {

using namespace StdLib;

void registerBuiltinsBasics(Interpreter& interpreter) {
    // exit - إنهاء البرنامج
    auto other_exit_func = [](Sad::Interpreter::BuiltinContext &ctx) {
                const auto &args = ctx.args(); (void)args;
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::exit(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bb::EXIT_ALT), other_exit_func);
    
    // assert - التحقق من شرط
    auto other_assert_func = [](Sad::Interpreter::BuiltinContext &ctx) {
                const auto &args = ctx.args(); (void)args;
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::assert(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bb::ASSERT), other_assert_func);
    
    // ═══════════════════════════════════════════════════════════════
    // Filesystem Module Functions (Part 1 - File I/O & Directories)
    
    // ═══════════════════════════════════════════════════════════════
    // read_lines - قراءة أسطر الملف
    auto fs_read_lines_func = [](Sad::Interpreter::BuiltinContext &ctx) {
                const auto &args = ctx.args(); (void)args;
        if (args.empty()) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        std::string path = args[0]->toString();
        auto lines = sad::stdlib::filesystem::read_lines(path);
        std::vector<Data::Value> result;
        for (const auto& line : lines) {
            result.push_back(Data::Value(line));
        }
        return std::make_shared<Data::Value>(result);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bb::READ_LINES), fs_read_lines_func);
    // append_to_file - إضافة إلى ملف
    auto fs_append_func = [](Sad::Interpreter::BuiltinContext &ctx) {
                const auto &args = ctx.args(); (void)args;
        if (args.size() < 2) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        std::string path = args[0]->toString();
        std::string content = args[1]->toString();
        sad::stdlib::filesystem::append_to_file(path, content);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bb::APPEND_FILE), fs_append_func);
    // copy_file - نسخ ملف
    auto fs_copy_file_func = [](Sad::Interpreter::BuiltinContext &ctx) {
                const auto &args = ctx.args(); (void)args;
        if (args.size() < 2) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        std::string source = args[0]->toString();
        std::string dest = args[1]->toString();
        bool overwrite = args.size() > 2 ? args[2]->toBool() : false;
        sad::stdlib::filesystem::copy_file(source, dest, overwrite);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bb::COPY_FILE), fs_copy_file_func);
    // move_file - نقل ملف
    auto fs_move_file_func = [](Sad::Interpreter::BuiltinContext &ctx) {
                const auto &args = ctx.args(); (void)args;
        if (args.size() < 2) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        std::string source = args[0]->toString();
        std::string dest = args[1]->toString();
        sad::stdlib::filesystem::move_file(source, dest);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bb::MOVE_FILE), fs_move_file_func);
    // delete_file - حذف ملف
    auto fs_delete_file_func = [](Sad::Interpreter::BuiltinContext &ctx) {
                const auto &args = ctx.args(); (void)args;
        if (args.empty()) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        std::string path = args[0]->toString();
        bool result = sad::stdlib::filesystem::delete_file(path);
        return std::make_shared<Data::Value>(result);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bb::DELETE_FILE), fs_delete_file_func);
    // create_directory - إنشاء مجلد
    auto fs_create_dir_func = [](Sad::Interpreter::BuiltinContext &ctx) {
                const auto &args = ctx.args(); (void)args;
        if (args.empty()) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        std::string path = args[0]->toString();
        bool recursive = args.size() > 1 ? args[1]->toBool() : true;
        bool result = sad::stdlib::filesystem::create_directory(path, recursive);
        return std::make_shared<Data::Value>(result);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bb::MKDIR), fs_create_dir_func);
    
    // list_directory - سرد محتويات مجلد
    auto fs_list_dir_func = [](Sad::Interpreter::BuiltinContext &ctx) {
                const auto &args = ctx.args(); (void)args;
        if (args.empty()) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        std::string path = args[0]->toString();
        auto entries = sad::stdlib::filesystem::list_directory(path);
        std::vector<Data::Value> result;
        for (const auto& entry : entries) {
            result.push_back(Data::Value(entry));
        }
        return std::make_shared<Data::Value>(result);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bb::LIST_DIR), fs_list_dir_func);
    
    // remove_directory - حذف مجلد
    auto fs_remove_dir_func = [](Sad::Interpreter::BuiltinContext &ctx) {
                const auto &args = ctx.args(); (void)args;
        if (args.empty()) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        std::string path = args[0]->toString();
        bool recursive = args.size() > 1 ? args[1]->toBool() : false;
        bool result = sad::stdlib::filesystem::remove_directory(path, recursive);
        return std::make_shared<Data::Value>(result);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bb::REMOVE_DIR), fs_remove_dir_func);
    
    // is_file - هل هو ملف
    auto fs_is_file_func = [](Sad::Interpreter::BuiltinContext &ctx) {
                const auto &args = ctx.args(); (void)args;
        if (args.empty()) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        std::string path = args[0]->toString();
        bool result = sad::stdlib::filesystem::is_file(path);
        return std::make_shared<Data::Value>(result);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bb::IS_FILE), fs_is_file_func);
    
    // is_directory - هل هو مجلد
    auto fs_is_dir_func = [](Sad::Interpreter::BuiltinContext &ctx) {
                const auto &args = ctx.args(); (void)args;
        if (args.empty()) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        std::string path = args[0]->toString();
        bool result = sad::stdlib::filesystem::is_directory(path);
        return std::make_shared<Data::Value>(result);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bb::IS_DIR), fs_is_dir_func);
    
    
    // ═══════════════════════════════════════════════════════════════
    // (AR) دوال المدى والتكرار / (EN) Range & Iteration Functions
    
    // ═══════════════════════════════════════════════════════════════
    auto range_func = [](Sad::Interpreter::BuiltinContext &ctx) {
                const auto &args = ctx.args(); (void)args;
        return BuiltinFunctions::range(args);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bb::RANGE), range_func);
    
    
    // ═══════════════════════════════════════════════════════════════
    // (AR) دوال نظام الملفات / (EN) Filesystem Functions
    
    // ═══════════════════════════════════════════════════════════════
    // دالة قراءة ملف / Read file function
    auto read_file_func = [](Sad::Interpreter::BuiltinContext &ctx) {
                const auto &args = ctx.args(); (void)args;
        if (args.empty() || !args[0]) {
            ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        }
        std::string path = args[0]->toString();
        
        std::ifstream file(path);
        if (!file.is_open()) {
            ctx.error(::Sad::Errors::ErrorCode::RUN_FILE_ERROR, {{"path", path}, {"reason", "(AR) قراءة / (EN) read"}});
        }
        
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        
        return std::make_shared<Data::Value>(content);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bb::READ_FILE), read_file_func);
    
    // دالة كتابة ملف / Write file function
    auto write_file_func = [](Sad::Interpreter::BuiltinContext &ctx) {
                const auto &args = ctx.args(); (void)args;
        if (args.size() < 2 || !args[0] || !args[1]) {
            ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        }
        
        std::string path = args[0]->toString();
        std::string content = args[1]->toString();
        
        std::ofstream file(path);
        if (!file.is_open()) {
            ctx.error(::Sad::Errors::ErrorCode::RUN_FILE_ERROR, {{"path", path}, {"reason", "(AR) كتابة / (EN) write"}});
        }
        
        file << content;
        file.close();
        
        return std::make_shared<Data::Value>();  // void
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bb::WRITE_FILE), write_file_func);

    // ═══════════════════════════════════════════════════════════════
    // (AR) كتابة بايتات خام / (EN) Write raw bytes
    //   بخلاف اكتب_ملف النصّيّة: يفتح بالوضع الثنائيّ ويكتب كلَّ عنصرٍ عددًا
    //   (0..255) بايتًا واحدًا عبر put ⇒ يكتب حتّى البايتات الصفريّة (لازمٌ لـELF).
    //   يجب أن يطابق باعثَ المترجم emitBuiltinFileWriteBytes بايتًا ببايت.
    // (EN) Unlike text write_file: opens in binary mode and writes each numeric
    //   element (0..255) as a single byte via put ⇒ writes embedded NUL bytes
    //   (required for ELF). Must match the compiler's emitBuiltinFileWriteBytes byte-for-byte.
    // ═══════════════════════════════════════════════════════════════
    auto write_bytes_func = [](Sad::Interpreter::BuiltinContext &ctx) {
        const auto &args = ctx.args(); (void)args;
        if (args.size() < 2 || !args[0] || !args[1]) {
            ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        }
        std::string path = args[0]->toString();
        std::vector<Data::Value> bytes = args[1]->toArray();

        std::ofstream file(path, std::ios::binary);
        if (!file.is_open()) {
            ctx.error(::Sad::Errors::ErrorCode::RUN_FILE_ERROR, {{"path", path}, {"reason", "(AR) كتابة بايتات / (EN) write_bytes"}});
        }

        for (const auto &el : bytes) {
            // (AR) تقنيع 0xFF يطابق اقتطاعَ الباعث؛ عددٌ خارج المدى يُلتقَط منخفضَ البايت.
            unsigned char b = static_cast<unsigned char>(el.toInt64() & 0xFF);
            file.put(static_cast<char>(b));
        }
        file.close();

        return std::make_shared<Data::Value>();  // void
    };

    interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bb::WRITE_BYTES), write_bytes_func);

    // ═══════════════════════════════════════════════════════════════
    // (AR) قراءة بايتات خام / (EN) Read raw bytes
    //   يعيد مصفوفةَ أعدادٍ صحيحة (0..255)، عنصرًا لكلّ بايت. الوضع الثنائيّ
    //   كي لا تُترجَم CRLF على ويندوز (يحفظ التطابقَ الثنائيّ مع المترجم).
    // (EN) Returns an array of integers (0..255), one per byte. Binary mode so
    //   CRLF is not translated on Windows (preserves byte-parity with the compiler).
    // ═══════════════════════════════════════════════════════════════
    auto read_bytes_func = [](Sad::Interpreter::BuiltinContext &ctx) {
        const auto &args = ctx.args(); (void)args;
        if (args.empty() || !args[0]) {
            ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        }
        std::string path = args[0]->toString();

        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            ctx.error(::Sad::Errors::ErrorCode::RUN_FILE_ERROR, {{"path", path}, {"reason", "(AR) قراءة بايتات / (EN) read_bytes"}});
        }

        std::vector<Data::Value> bytes;
        char c;
        while (file.get(c)) {
            bytes.push_back(Data::Value(static_cast<int64_t>(static_cast<unsigned char>(c))));
        }
        file.close();

        return std::make_shared<Data::Value>(std::move(bytes));
    };

    interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bb::READ_BYTES), read_bytes_func);

    // دالة إضافة لملف / Append to file function
    auto append_file_func = [](Sad::Interpreter::BuiltinContext &ctx) {
                const auto &args = ctx.args(); (void)args;
        if (args.size() < 2 || !args[0] || !args[1]) {
            ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        }
        
        std::string path = args[0]->toString();
        std::string content = args[1]->toString();
        
        std::ofstream file(path, std::ios::app);
        if (!file.is_open()) {
            ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        }
        
        file << content;
        file.close();
        
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bb::APPEND_FILE), append_file_func);
    
    // دالة حذف ملف / Delete file function
    auto delete_file_func = [](Sad::Interpreter::BuiltinContext &ctx) {
                const auto &args = ctx.args(); (void)args;
        if (args.empty() || !args[0]) {
            ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        }
        
        std::string path = args[0]->toString();
        
        if (std::filesystem::exists(path)) {
            std::filesystem::remove(path);
        } else {
            ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        }
        
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bb::DELETE_FILE), delete_file_func);
    
    // دالة التحقق من وجود ملف / Check if file exists
    auto file_exists_func = [](Sad::Interpreter::BuiltinContext &ctx) {
                const auto &args = ctx.args(); (void)args;
        if (args.empty() || !args[0]) {
            ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        }
        
        std::string path = args[0]->toString();
        bool exists = std::filesystem::exists(path);
        
        return std::make_shared<Data::Value>(exists);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bb::FILE_EXISTS), file_exists_func);


    

}

} // namespace Interpreter
} // namespace Sad
