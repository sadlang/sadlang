// بسم الله الرحمن الرحيم
/**
 * @file filesystem_builtins.cpp
 * @brief Filesystem Builtin Functions for Sad Interpreter - دوال نظام الملفات المدمجة
 * 
 * @author Sad Language Team
 * @date December 2025
 * الحمد لله رب العالمين
 */

#include "stdlib/filesystem_module.h"
#include "data/types/value.h"
#include <memory>

namespace sad {
namespace stdlib {
namespace filesystem {

using namespace sad::data;

// ============================================================================
// Helper Functions - دوال مساعدة
// ============================================================================

namespace {
    std::string get_string_arg(const std::vector<Value>& args, size_t index, const std::string& name) {
        if (index >= args.size()) {
            throw std::runtime_error(name + ": Missing argument at index " + std::to_string(index));
        }
        if (args[index].type != ValueType::STRING) {
            throw std::runtime_error(name + ": Expected string argument at index " + std::to_string(index));
        }
        return args[index].string_val;
    }

    bool get_bool_arg(const std::vector<Value>& args, size_t index, bool default_value = false) {
        if (index >= args.size()) {
            return default_value;
        }
        if (args[index].type != ValueType::BOOLEAN) {
            return default_value;
        }
        return args[index].bool_val;
    }

    int64_t get_int_arg(const std::vector<Value>& args, size_t index, int64_t default_value = 0) {
        if (index >= args.size()) {
            return default_value;
        }
        if (args[index].type != ValueType::NUMBER) {
            return default_value;
        }
        return static_cast<int64_t>(args[index].number_val);
    }

    std::vector<std::string> get_string_array_arg(const std::vector<Value>& args, size_t index) {
        if (index >= args.size() || args[index].type != ValueType::ARRAY) {
            throw std::runtime_error("Expected array argument at index " + std::to_string(index));
        }

        std::vector<std::string> result;
        for (const auto& val : args[index].array_val->elements) {
            if (val.type == ValueType::STRING) {
                result.push_back(val.string_val);
            }
        }
        return result;
    }

    Value make_string_array(const std::vector<std::string>& strings) {
        Value result;
        result.type = ValueType::ARRAY;
        result.array_val = std::make_shared<ArrayValue>();
        
        for (const auto& str : strings) {
            Value element;
            element.type = ValueType::STRING;
            element.string_val = str;
            result.array_val->elements.push_back(element);
        }
        
        return result;
    }

    Value make_byte_array(const std::vector<uint8_t>& bytes) {
        Value result;
        result.type = ValueType::ARRAY;
        result.array_val = std::make_shared<ArrayValue>();
        
        for (uint8_t byte : bytes) {
            Value element;
            element.type = ValueType::NUMBER;
            element.number_val = static_cast<double>(byte);
            result.array_val->elements.push_back(element);
        }
        
        return result;
    }

    std::vector<uint8_t> value_to_bytes(const Value& val) {
        if (val.type != ValueType::ARRAY) {
            throw std::runtime_error("Expected array for byte conversion");
        }

        std::vector<uint8_t> result;
        for (const auto& element : val.array_val->elements) {
            if (element.type == ValueType::NUMBER) {
                result.push_back(static_cast<uint8_t>(element.number_val));
            }
        }
        return result;
    }
}

// ============================================================================
// Builtin Function Wrappers - أغلفة الدوال المدمجة
// ============================================================================

// File I/O - قراءة/كتابة الملفات

Value builtin_read_file(const std::vector<Value>& args) {
    std::string path = get_string_arg(args, 0, "اقرأ_ملف");
    
    try {
        std::string content = read_file(path);
        Value result;
        result.type = ValueType::STRING;
        result.string_val = content;
        return result;
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("اقرأ_ملف: ") + e.what());
    }
}

Value builtin_read_lines(const std::vector<Value>& args) {
    std::string path = get_string_arg(args, 0, "اقرأ_أسطر");
    
    try {
        std::vector<std::string> lines = read_lines(path);
        return make_string_array(lines);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("اقرأ_أسطر: ") + e.what());
    }
}

Value builtin_read_bytes(const std::vector<Value>& args) {
    std::string path = get_string_arg(args, 0, "اقرأ_بايتات");
    
    try {
        std::vector<uint8_t> bytes = read_bytes(path);
        return make_byte_array(bytes);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("اقرأ_بايتات: ") + e.what());
    }
}

Value builtin_write_file(const std::vector<Value>& args) {
    std::string path = get_string_arg(args, 0, "اكتب_ملف");
    std::string content = get_string_arg(args, 1, "اكتب_ملف");
    bool truncate = get_bool_arg(args, 2, true);
    
    try {
        write_file(path, content, truncate);
        Value result;
        result.type = ValueType::NULL_TYPE;
        return result;
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("اكتب_ملف: ") + e.what());
    }
}

Value builtin_write_lines(const std::vector<Value>& args) {
    std::string path = get_string_arg(args, 0, "اكتب_أسطر");
    std::vector<std::string> lines = get_string_array_arg(args, 1);
    
    try {
        write_lines(path, lines);
        Value result;
        result.type = ValueType::NULL_TYPE;
        return result;
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("اكتب_أسطر: ") + e.what());
    }
}

Value builtin_write_bytes(const std::vector<Value>& args) {
    std::string path = get_string_arg(args, 0, "اكتب_بايتات");
    std::vector<uint8_t> bytes = value_to_bytes(args[1]);
    
    try {
        write_bytes(path, bytes);
        Value result;
        result.type = ValueType::NULL_TYPE;
        return result;
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("اكتب_بايتات: ") + e.what());
    }
}

Value builtin_append_to_file(const std::vector<Value>& args) {
    std::string path = get_string_arg(args, 0, "أضف_إلى_ملف");
    std::string content = get_string_arg(args, 1, "أضف_إلى_ملف");
    
    try {
        append_to_file(path, content);
        Value result;
        result.type = ValueType::NULL_TYPE;
        return result;
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("أضف_إلى_ملف: ") + e.what());
    }
}

Value builtin_copy_file(const std::vector<Value>& args) {
    std::string source = get_string_arg(args, 0, "انسخ_ملف");
    std::string dest = get_string_arg(args, 1, "انسخ_ملف");
    bool overwrite = get_bool_arg(args, 2, false);
    
    try {
        copy_file(source, dest, overwrite);
        Value result;
        result.type = ValueType::NULL_TYPE;
        return result;
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("انسخ_ملف: ") + e.what());
    }
}

Value builtin_move_file(const std::vector<Value>& args) {
    std::string source = get_string_arg(args, 0, "انقل_ملف");
    std::string dest = get_string_arg(args, 1, "انقل_ملف");
    
    try {
        move_file(source, dest);
        Value result;
        result.type = ValueType::NULL_TYPE;
        return result;
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("انقل_ملف: ") + e.what());
    }
}

Value builtin_delete_file(const std::vector<Value>& args) {
    std::string path = get_string_arg(args, 0, "احذف_ملف");
    
    try {
        bool deleted = delete_file(path);
        Value result;
        result.type = ValueType::BOOLEAN;
        result.bool_val = deleted;
        return result;
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("احذف_ملف: ") + e.what());
    }
}

// Directory Operations - عمليات المجلدات

Value builtin_create_directory(const std::vector<Value>& args) {
    std::string path = get_string_arg(args, 0, "أنشئ_مجلد");
    bool recursive = get_bool_arg(args, 1, true);
    
    try {
        bool created = create_directory(path, recursive);
        Value result;
        result.type = ValueType::BOOLEAN;
        result.bool_val = created;
        return result;
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("أنشئ_مجلد: ") + e.what());
    }
}

Value builtin_list_directory(const std::vector<Value>& args) {
    std::string path = get_string_arg(args, 0, "اسرد_مجلد");
    
    try {
        std::vector<std::string> entries = list_directory(path);
        return make_string_array(entries);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("اسرد_مجلد: ") + e.what());
    }
}

Value builtin_list_directory_full(const std::vector<Value>& args) {
    std::string path = get_string_arg(args, 0, "اسرد_مجلد_كامل");
    
    try {
        std::vector<std::string> entries = list_directory_full(path);
        return make_string_array(entries);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("اسرد_مجلد_كامل: ") + e.what());
    }
}

Value builtin_list_directory_recursive(const std::vector<Value>& args) {
    std::string path = get_string_arg(args, 0, "اسرد_مجلد_تكراري");
    
    try {
        std::vector<std::string> entries = list_directory_recursive(path);
        return make_string_array(entries);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("اسرد_مجلد_تكراري: ") + e.what());
    }
}

Value builtin_remove_directory(const std::vector<Value>& args) {
    std::string path = get_string_arg(args, 0, "احذف_مجلد");
    bool recursive = get_bool_arg(args, 1, false);
    
    try {
        bool removed = remove_directory(path, recursive);
        Value result;
        result.type = ValueType::BOOLEAN;
        result.bool_val = removed;
        return result;
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("احذف_مجلد: ") + e.what());
    }
}

Value builtin_get_current_directory(const std::vector<Value>& args) {
    try {
        std::string dir = get_current_directory();
        Value result;
        result.type = ValueType::STRING;
        result.string_val = dir;
        return result;
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("احصل_على_مجلد_العمل: ") + e.what());
    }
}

Value builtin_change_directory(const std::vector<Value>& args) {
    std::string path = get_string_arg(args, 0, "غير_مجلد_العمل");
    
    try {
        bool changed = change_directory(path);
        Value result;
        result.type = ValueType::BOOLEAN;
        result.bool_val = changed;
        return result;
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("غير_مجلد_العمل: ") + e.what());
    }
}

// Path Manipulation - معالجة المسارات

Value builtin_join_path(const std::vector<Value>& args) {
    std::vector<std::string> components = get_string_array_arg(args, 0);
    
    try {
        std::string path = join_path(components);
        Value result;
        result.type = ValueType::STRING;
        result.string_val = path;
        return result;
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("اجمع_مسار: ") + e.what());
    }
}

Value builtin_get_absolute_path(const std::vector<Value>& args) {
    std::string path = get_string_arg(args, 0, "احصل_على_مسار_مطلق");
    
    try {
        std::string absolute = get_absolute_path(path);
        Value result;
        result.type = ValueType::STRING;
        result.string_val = absolute;
        return result;
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("احصل_على_مسار_مطلق: ") + e.what());
    }
}

Value builtin_get_filename(const std::vector<Value>& args) {
    std::string path = get_string_arg(args, 0, "احصل_على_اسم_ملف");
    
    try {
        std::string filename = get_filename(path);
        Value result;
        result.type = ValueType::STRING;
        result.string_val = filename;
        return result;
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("احصل_على_اسم_ملف: ") + e.what());
    }
}

Value builtin_get_extension(const std::vector<Value>& args) {
    std::string path = get_string_arg(args, 0, "احصل_على_امتداد");
    
    try {
        std::string ext = get_extension(path);
        Value result;
        result.type = ValueType::STRING;
        result.string_val = ext;
        return result;
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("احصل_على_امتداد: ") + e.what());
    }
}

Value builtin_get_parent_path(const std::vector<Value>& args) {
    std::string path = get_string_arg(args, 0, "احصل_على_مجلد_أب");
    
    try {
        std::string parent = get_parent_path(path);
        Value result;
        result.type = ValueType::STRING;
        result.string_val = parent;
        return result;
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("احصل_على_مجلد_أب: ") + e.what());
    }
}

// File Information - معلومات الملف

Value builtin_exists(const std::vector<Value>& args) {
    std::string path = get_string_arg(args, 0, "هل_موجود");
    
    try {
        bool exists_val = exists(path);
        Value result;
        result.type = ValueType::BOOLEAN;
        result.bool_val = exists_val;
        return result;
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("هل_موجود: ") + e.what());
    }
}

Value builtin_is_file(const std::vector<Value>& args) {
    std::string path = get_string_arg(args, 0, "هل_ملف");
    
    try {
        bool is_file_val = is_file(path);
        Value result;
        result.type = ValueType::BOOLEAN;
        result.bool_val = is_file_val;
        return result;
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("هل_ملف: ") + e.what());
    }
}

Value builtin_is_directory(const std::vector<Value>& args) {
    std::string path = get_string_arg(args, 0, "هل_مجلد");
    
    try {
        bool is_dir = is_directory(path);
        Value result;
        result.type = ValueType::BOOLEAN;
        result.bool_val = is_dir;
        return result;
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("هل_مجلد: ") + e.what());
    }
}

Value builtin_get_file_size(const std::vector<Value>& args) {
    std::string path = get_string_arg(args, 0, "احصل_على_حجم_ملف");
    
    try {
        uint64_t size = get_file_size(path);
        Value result;
        result.type = ValueType::NUMBER;
        result.number_val = static_cast<double>(size);
        return result;
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("احصل_على_حجم_ملف: ") + e.what());
    }
}

Value builtin_glob(const std::vector<Value>& args) {
    std::string pattern = get_string_arg(args, 0, "طابق_نمط");
    std::string base_path = args.size() > 1 ? get_string_arg(args, 1, "طابق_نمط") : ".";
    
    try {
        std::vector<std::string> matches = glob(pattern, base_path);
        return make_string_array(matches);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("طابق_نمط: ") + e.what());
    }
}

// ============================================================================
// Registration Function - دالة التسجيل
// ============================================================================

void register_filesystem_functions(sad::interpreter::Interpreter& interp) {
    // File I/O - قراءة/كتابة الملفات
    interp.register_builtin("اقرأ_ملف", builtin_read_file);
    interp.register_builtin("read_file", builtin_read_file);
    
    interp.register_builtin("اقرأ_أسطر", builtin_read_lines);
    interp.register_builtin("read_lines", builtin_read_lines);
    
    interp.register_builtin("اقرأ_بايتات", builtin_read_bytes);
    interp.register_builtin("read_bytes", builtin_read_bytes);
    
    interp.register_builtin("اكتب_ملف", builtin_write_file);
    interp.register_builtin("write_file", builtin_write_file);
    
    interp.register_builtin("اكتب_أسطر", builtin_write_lines);
    interp.register_builtin("write_lines", builtin_write_lines);
    
    interp.register_builtin("اكتب_بايتات", builtin_write_bytes);
    interp.register_builtin("write_bytes", builtin_write_bytes);
    
    interp.register_builtin("أضف_إلى_ملف", builtin_append_to_file);
    interp.register_builtin("append_to_file", builtin_append_to_file);
    
    interp.register_builtin("انسخ_ملف", builtin_copy_file);
    interp.register_builtin("copy_file", builtin_copy_file);
    
    interp.register_builtin("انقل_ملف", builtin_move_file);
    interp.register_builtin("move_file", builtin_move_file);
    
    interp.register_builtin("احذف_ملف", builtin_delete_file);
    interp.register_builtin("delete_file", builtin_delete_file);
    
    // Directory Operations - عمليات المجلدات
    interp.register_builtin("أنشئ_مجلد", builtin_create_directory);
    interp.register_builtin("create_directory", builtin_create_directory);
    
    interp.register_builtin("اسرد_مجلد", builtin_list_directory);
    interp.register_builtin("list_directory", builtin_list_directory);
    
    interp.register_builtin("اسرد_مجلد_كامل", builtin_list_directory_full);
    interp.register_builtin("list_directory_full", builtin_list_directory_full);
    
    interp.register_builtin("اسرد_مجلد_تكراري", builtin_list_directory_recursive);
    interp.register_builtin("list_directory_recursive", builtin_list_directory_recursive);
    
    interp.register_builtin("احذف_مجلد", builtin_remove_directory);
    interp.register_builtin("remove_directory", builtin_remove_directory);
    
    interp.register_builtin("احصل_على_مجلد_العمل", builtin_get_current_directory);
    interp.register_builtin("get_current_directory", builtin_get_current_directory);
    
    interp.register_builtin("غير_مجلد_العمل", builtin_change_directory);
    interp.register_builtin("change_directory", builtin_change_directory);
    
    // Path Manipulation - معالجة المسارات
    interp.register_builtin("اجمع_مسار", builtin_join_path);
    interp.register_builtin("join_path", builtin_join_path);
    
    interp.register_builtin("احصل_على_مسار_مطلق", builtin_get_absolute_path);
    interp.register_builtin("get_absolute_path", builtin_get_absolute_path);
    
    interp.register_builtin("احصل_على_اسم_ملف", builtin_get_filename);
    interp.register_builtin("get_filename", builtin_get_filename);
    
    interp.register_builtin("احصل_على_امتداد", builtin_get_extension);
    interp.register_builtin("get_extension", builtin_get_extension);
    
    interp.register_builtin("احصل_على_مجلد_أب", builtin_get_parent_path);
    interp.register_builtin("get_parent_path", builtin_get_parent_path);
    
    // File Information - معلومات الملف
    interp.register_builtin("هل_موجود", builtin_exists);
    interp.register_builtin("exists", builtin_exists);
    
    interp.register_builtin("هل_ملف", builtin_is_file);
    interp.register_builtin("is_file", builtin_is_file);
    
    interp.register_builtin("هل_مجلد", builtin_is_directory);
    interp.register_builtin("is_directory", builtin_is_directory);
    
    interp.register_builtin("احصل_على_حجم_ملف", builtin_get_file_size);
    interp.register_builtin("get_file_size", builtin_get_file_size);
    
    // Pattern Matching - مطابقة الأنماط
    interp.register_builtin("طابق_نمط", builtin_glob);
    interp.register_builtin("glob", builtin_glob);
}

} // namespace filesystem
} // namespace stdlib
} // namespace sad
