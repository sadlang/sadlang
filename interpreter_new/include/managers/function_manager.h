/**
 * @file function_manager.h
 * @brief (AR) مدير الدوال - تخزين وإدارة تعريفات الدوال
 * @brief (EN) Function Manager - Storage and management of function definitions
 *
 * الوصف بالعربية:
 * =================
 * مدير الدوال مسؤول عن:
 * - تخزين تعريفات الدوال (اسم، معاملات، جسم الدالة)
 * - إدارة دوال متعددة بنفس الاسم (Function Overloading)
 * - البحث عن الدوال المناسبة حسب عدد المعاملات
 * - دعم الدوال المضمنة (Built-in Functions)
 * - دعم دوال Lambda
 * - التكامل مع AST لحفظ جسم الدالة
 *
 * المميزات:
 * - تخزين الدوال في نطاقات مختلفة
 * - دعم Function Overloading (دوال بنفس الاسم لكن معاملات مختلفة)
 * - البحث الذكي عن الدالة المناسبة
 * - معلومات كاملة عن كل دالة (اسم، معاملات، نوع الإرجاع)
 * - رسائل خطأ ثنائية اللغة (عربي + إنجليزي)
 *
 * مثال الاستخدام:
 * ```cpp
 * FunctionManager funcMgr;
 *
 * // تعريف دالة بسيطة
 * std::vector<std::string> params = {"x", "y"};
 * funcMgr.defineFunction("add", params, functionBodyAST);
 *
 * // البحث عن دالة
 * auto func = funcMgr.getFunction("add", 2);
 * if (func) {
 *     // استدعاء الدالة
 * }
 * ```
 *
 * English Description:
 * ====================
 * Function Manager is responsible for:
 * - Storing function definitions (name, parameters, body)
 * - Managing multiple functions with same name (Function Overloading)
 * - Finding appropriate function based on parameter count
 * - Supporting built-in functions
 * - Supporting lambda functions
 * - Integration with AST to store function body
 *
 * Features:
 * - Store functions in different scopes
 * - Support function overloading (same name, different parameters)
 * - Smart function lookup
 * - Complete function information (name, parameters, return type)
 * - Bilingual error messages (Arabic + English)
 *
 * Usage Example:
 * ```cpp
 * FunctionManager funcMgr;
 *
 * // Define simple function
 * std::vector<std::string> params = {"x", "y"};
 * funcMgr.defineFunction("add", params, functionBodyAST);
 *
 * // Find function
 * auto func = funcMgr.getFunction("add", 2);
 * if (func) {
 *     // Call function
 * }
 * ```
 *
 * @author S Language Development Team
 * @date November 21, 2025
 * @version 1.0
 */

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <stdexcept>

#include "value.h"

// (AR) تصريحات أمامية لعقد AST / (EN) Forward declarations for AST nodes
namespace Sad
{
    namespace AST
    {
        class ASTNode;
        struct Parameter;
    }
}

namespace Sad
{
    namespace Data
    {

        /**
         * @brief (AR) نوع الدالة
         * @brief (EN) Function type
         */
        enum class FunctionType
        {
            USER_DEFINED, ///< (AR) دالة معرفة من المستخدم / (EN) User-defined function
            BUILT_IN,     ///< (AR) دالة مضمنة / (EN) Built-in function
            LAMBDA        ///< (AR) دالة لامدا / (EN) Lambda function
        };

        /**
         * @brief (AR) معلومات عن معامل دالة
         * @brief (EN) Function parameter information
         *
         * (AR) يحتوي على اسم المعامل، نوعه، وإذا كان النوع صنفاً (OBJECT)
         *      يُخزَّن اسم الصنف في typeName لفحص النوع في وقت التشغيل.
         * (EN) Contains parameter name, type, and if the type is a class (OBJECT),
         *      the class name is stored in typeName for runtime type checking.
         */
        struct FunctionParameter
        {
            std::string name;         ///< (AR) اسم المعامل / (EN) Parameter name
            std::string type;         ///< (AR) نوع المعامل (اختياري) / (EN) Parameter type (optional)
            std::string typeName;     ///< (AR) اسم الصنف عندما يكون النوع كائن / (EN) Class name when type is object
            bool hasDefaultValue;     ///< (AR) هل له قيمة افتراضية؟ / (EN) Has default value?
            std::string defaultValue; ///< (AR) القيمة الافتراضية / (EN) Default value

            FunctionParameter(const std::string &n = "", const std::string &t = "",
                              bool hasDefault = false, const std::string &defVal = "",
                              const std::string &tn = "")
                : name(n), type(t), typeName(tn), hasDefaultValue(hasDefault), defaultValue(defVal) {}
        };

        /**
         * @brief (AR) تعريف دالة كامل
         * @brief (EN) Complete function definition
         */
        class FunctionDefinition
        {
        public:
            /**
             * @brief (AR) إنشاء تعريف دالة معرفة من المستخدم
             * @brief (EN) Create user-defined function definition
             */
            FunctionDefinition(const std::string &name,
                               const std::vector<FunctionParameter> &params,
                               std::shared_ptr<AST::ASTNode> body);

            /**
             * @brief (AR) إنشاء تعريف دالة مضمنة
             * @brief (EN) Create built-in function definition
             */
            FunctionDefinition(const std::string &name,
                               const std::vector<FunctionParameter> &params,
                               std::function<std::shared_ptr<Data::Value>(const std::vector<std::shared_ptr<Data::Value>> &)> nativeImpl);

            // Getters
            std::string getName() const { return name_; }
            FunctionType getType() const { return type_; }
            const std::vector<FunctionParameter> &getParameters() const { return parameters_; }
            size_t getParameterCount() const { return parameters_.size(); }
            size_t getRequiredParameterCount() const;

            std::shared_ptr<AST::ASTNode> getBody() const { return body_; }
            bool hasBody() const { return body_ != nullptr; }

            /**
             * @brief (AR) التحقق من وجود تنفيذ أصلي (دالة مضمنة)
             * @brief (EN) Check if has native implementation (built-in function)
             */
            bool hasNativeImplementation() const { return nativeImplementation_ != nullptr; }

            /**
             * @brief (AR) استدعاء التنفيذ الأصلي
             * @brief (EN) Call native implementation
             */
            std::shared_ptr<Data::Value> callNative(const std::vector<std::shared_ptr<Data::Value>> &args) const
            {
                if (nativeImplementation_)
                {
                    return nativeImplementation_(args);
                }
                return nullptr;
            }

            std::string getReturnType() const { return returnType_; }
            void setReturnType(const std::string &type) { returnType_ = type; }

            /**
             * @brief (AR) التحقق من تطابق عدد المعاملات
             * @brief (EN) Check if parameter count matches
             *
             * @param argCount (AR) عدد المعاملات المُمررة / (EN) Number of arguments passed
             * @return (AR) true إذا كان العدد مطابقاً / (EN) true if count matches
             */
            bool matchesParameterCount(size_t argCount) const;

            /**
             * @brief (AR) التحقق من قبول عدد معاملات (مع القيم الافتراضية)
             * @brief (EN) Check if function accepts argument count (with defaults)
             *
             * @param argCount (AR) عدد المعاملات المُمررة / (EN) Number of arguments passed
             * @return (AR) true إذا كان العدد مقبولاً / (EN) true if count acceptable
             */
            bool acceptsArgumentCount(size_t argCount) const;

            /**
             * @brief (AR) الحصول على معلومات الدالة كنص
             * @brief (EN) Get function information as string
             */
            std::string getSignature() const;
            std::string debugString() const;

            /**
             * @brief (AR) تخزين FunctionDecl الأصلي (للوصول لـ Parameters مع ExprPtr)
             * @brief (EN) Store original FunctionDecl (for Parameters with ExprPtr access)
             */
            void setFunctionDecl(std::shared_ptr<AST::ASTNode> decl);

            /**
             * @brief (AR) الحصول على FunctionDecl الأصلي
             * @brief (EN) Get original FunctionDecl
             */
            std::shared_ptr<AST::ASTNode> getFunctionDecl() const { return functionDecl_; }

            /**
             * @brief (AR) التحقق إذا كانت الدالة مولد
             * @brief (EN) Check if function is a generator
             */
            bool isGenerator() const { return isGenerator_; }

            /**
             * @brief (AR) تعيين علامة المولد
             * @brief (EN) Set generator flag
             */
            void setIsGenerator(bool isGen) { isGenerator_ = isGen; }

            /**
             * @brief (AR) التحقق إذا كانت الدالة غير متزامنة
             * @brief (EN) Check if function is async
             */
            bool isAsync() const { return isAsync_; }

            /**
             * @brief (AR) تعيين علامة الدالة غير المتزامنة
             * @brief (EN) Set async flag
             */
            void setIsAsync(bool isAsync) { isAsync_ = isAsync; }

            /**
             * @brief (AR) التحقق إذا كانت الدالة خارجية (extern/FFI)
             * @brief (EN) Check if function is extern (FFI)
             */
            bool isExtern() const { return isExtern_; }

            /**
             * @brief (AR) تعيين علامة الدالة الخارجية
             * @brief (EN) Set extern flag
             */
            void setIsExtern(bool isExtern) { isExtern_ = isExtern; }

            /**
             * @brief (AR) تعيين المتغيرات الملتقطة (للإغلاقات/لامدا)
             * @brief (EN) Set captured variables (for closures/lambda)
             */
            void setCaptures(const std::unordered_map<std::string, Data::Value> &caps) { captures_ = caps; }

            /**
             * @brief (AR) الحصول على المتغيرات الملتقطة
             * @brief (EN) Get captured variables
             */
            const std::unordered_map<std::string, Data::Value> &getCaptures() const { return captures_; }

            /**
             * @brief (AR) هل لديها متغيرات ملتقطة؟
             * @brief (EN) Does it have captured variables?
             */
            bool hasCaptures() const { return !captures_.empty(); }

        private:
            std::string name_;                                                                                                    ///< (AR) اسم الدالة / (EN) Function name
            FunctionType type_;                                                                                                   ///< (AR) نوع الدالة / (EN) Function type
            std::vector<FunctionParameter> parameters_;                                                                           ///< (AR) قائمة المعاملات / (EN) Parameter list
            std::shared_ptr<AST::ASTNode> body_;                                                                                  ///< (AR) جسم الدالة (AST) / (EN) Function body (AST)
            std::shared_ptr<AST::ASTNode> declaration_;                                                                           ///< (AR) التصريح الأصلي (للوصول للـ defaults) / (EN) Original declaration (for defaults access)
            std::shared_ptr<AST::ASTNode> functionDecl_;                                                                          ///< (AR) FunctionDecl الأصلي (للوصول لـ Parameters) / (EN) Original FunctionDecl (for Parameters access)
            std::function<std::shared_ptr<Data::Value>(const std::vector<std::shared_ptr<Data::Value>> &)> nativeImplementation_; ///< (AR) تنفيذ أصلي (للدوال المضمنة) / (EN) Native implementation (for built-in)
            std::string returnType_;                                                                                              ///< (AR) نوع الإرجاع / (EN) Return type
            bool isGenerator_ = false;                                                                                            ///< (AR) هل هذه دالة مولد؟ / (EN) Is this a generator function?
            bool isAsync_ = false;                                                                                                ///< (AR) هل هذه دالة غير متزامنة؟ / (EN) Is this an async function?
            bool isExtern_ = false;                                                                                               ///< (AR) هل هذه دالة خارجية (FFI)؟ / (EN) Is this an extern function (FFI)?
            std::unordered_map<std::string, Data::Value> captures_;                                                               ///< (AR) المتغيرات الملتقطة للإغلاقات / (EN) Captured variables for closures
        };

        /**
         * @brief (AR) مدير الدوال - إدارة تعريفات الدوال في البرنامج
         * @brief (EN) Function Manager - Manages function definitions in the program
         *
         * المسؤوليات / Responsibilities:
         * --------------------------------
         * (AR) 1. تخزين تعريفات الدوال
         * (EN) 1. Store function definitions
         *
         * (AR) 2. البحث عن الدوال حسب الاسم وعدد المعاملات
         * (EN) 2. Find functions by name and parameter count
         *
         * (AR) 3. دعم Function Overloading
         * (EN) 3. Support function overloading
         *
         * (AR) 4. إدارة الدوال في نطاقات مختلفة
         * (EN) 4. Manage functions in different scopes
         */
        class FunctionManager
        {
        public:
            /**
             * @brief (AR) إنشاء مدير الدوال
             * @brief (EN) Create function manager
             */
            FunctionManager();

            /**
             * @brief (AR) المُدمر
             * @brief (EN) Destructor
             */
            ~FunctionManager();

            // ========================================
            // (AR) عمليات تعريف الدوال
            // (EN) Function Definition Operations
            // ========================================

            /**
             * @brief (AR) تعريف دالة جديدة
             * @brief (EN) Define new function
             *
             * (AR) تسجيل دالة جديدة في المدير. إذا كان هناك دالة بنفس الاسم
             *      وعدد المعاملات، سيتم تحديثها (أو رمي خطأ إذا لم يُسمح بالتحديث).
             *
             * (EN) Register new function in manager. If function with same name
             *      and parameter count exists, will update (or throw error if not allowed).
             *
             * @param name (AR) اسم الدالة / (EN) Function name
             * @param params (AR) قائمة المعاملات / (EN) Parameter list
             * @param body (AR) جسم الدالة (AST) / (EN) Function body (AST)
             *
             * @throws std::runtime_error (AR) إذا كانت الدالة معرفة مسبقاً / (EN) if already defined
             */
            void defineFunction(const std::string &name,
                                const std::vector<FunctionParameter> &params,
                                std::shared_ptr<AST::ASTNode> body);

            /**
             * @brief (AR) تعريف دالة جديدة مع FunctionDecl (للوصول لـ default parameters)
             * @brief (EN) Define new function with FunctionDecl (for default parameters access)
             *
             * @param name (AR) اسم الدالة / (EN) Function name
             * @param params (AR) قائمة المعاملات / (EN) Parameter list
             * @param body (AR) جسم الدالة (AST) / (EN) Function body (AST)
             * @param decl (AR) FunctionDecl الأصلي / (EN) Original FunctionDecl
             *
             * @throws std::runtime_error (AR) إذا كانت الدالة معرفة مسبقاً / (EN) if already defined
             */
            void defineFunction(const std::string &name,
                                const std::vector<FunctionParameter> &params,
                                std::shared_ptr<AST::ASTNode> body,
                                std::shared_ptr<AST::ASTNode> decl);

            /**
             * @brief (AR) تعريف دالة مضمنة
             * @brief (EN) Define built-in function
             *
             * @param name (AR) اسم الدالة / (EN) Function name
             * @param params (AR) قائمة المعاملات / (EN) Parameter list
             * @param impl (AR) التنفيذ الأصلي / (EN) Native implementation
             */
            void defineBuiltInFunction(const std::string &name,
                                       const std::vector<FunctionParameter> &params,
                                       std::function<std::shared_ptr<Value>(const std::vector<std::shared_ptr<Value>> &)> impl);

            /**
             * @brief (AR) تسجيل دالة مضمنة جديدة (مع معالج ValuePtr)
             * @brief (EN) Register new built-in function (with ValuePtr handler)
             *
             * @param name (AR) اسم الدالة / (EN) Function name
             * @param func (AR) الدالة / (EN) Function implementation
             */
            void registerBuiltinFunction(const std::string &name,
                                         const std::function<std::shared_ptr<Value>(const std::vector<std::shared_ptr<Value>> &)> &func);

            /**
             * @brief (AR) حذف دالة
             * @brief (EN) Remove function
             *
             * @param name (AR) اسم الدالة / (EN) Function name
             * @param paramCount (AR) عدد المعاملات (اختياري، -1 لحذف الكل) / (EN) Parameter count (optional, -1 for all)
             * @return (AR) عدد الدوال المحذوفة / (EN) Number of functions removed
             */
            size_t removeFunction(const std::string &name, int paramCount = -1);

            // ========================================
            // (AR) عمليات البحث والاستعلام
            // (EN) Search and Query Operations
            // ========================================

            /**
             * @brief (AR) البحث عن دالة حسب الاسم وعدد المعاملات
             * @brief (EN) Find function by name and parameter count
             *
             * @param name (AR) اسم الدالة / (EN) Function name
             * @param argCount (AR) عدد المعاملات / (EN) Number of arguments
             * @return (AR) مؤشر للدالة أو nullptr / (EN) Pointer to function or nullptr
             */
            std::shared_ptr<FunctionDefinition> getFunction(const std::string &name,
                                                            size_t argCount) const;

            /**
             * @brief (AR) إعادة تعريف دالة موجودة (للمزخرفات)
             * @brief (EN) Redefine existing function (for decorators)
             *
             * (AR) يستبدل تعريف دالة موجودة بتعريف جديد. يُستخدم عند تطبيق المزخرفات.
             * (EN) Replaces existing function definition with new one. Used when applying decorators.
             *
             * @param name (AR) اسم الدالة / (EN) Function name
             * @param params (AR) المعاملات الجديدة / (EN) New parameters
             * @param body (AR) الجسم الجديد / (EN) New body
             */
            void redefineFunction(const std::string &name,
                                  const std::vector<FunctionParameter> &params,
                                  std::shared_ptr<AST::ASTNode> body);

            /**
             * @brief (AR) التحقق من وجود دالة
             * @brief (EN) Check if function exists
             *
             * @param name (AR) اسم الدالة / (EN) Function name
             * @param argCount (AR) عدد المعاملات (اختياري، -1 للبحث عن أي عدد) / (EN) Argument count (optional, -1 for any)
             * @return (AR) true إذا كانت موجودة / (EN) true if exists
             */
            bool hasFunction(const std::string &name, int argCount = -1) const;

            /**
             * @brief (AR) الحصول على جميع الدوال بنفس الاسم (Overloads)
             * @brief (EN) Get all functions with same name (overloads)
             *
             * @param name (AR) اسم الدالة / (EN) Function name
             * @return (AR) قائمة الدوال / (EN) List of functions
             */
            std::vector<std::shared_ptr<FunctionDefinition>> getFunctionOverloads(
                const std::string &name) const;

            /**
             * @brief (AR) الحصول على قائمة بأسماء جميع الدوال
             * @brief (EN) Get list of all function names
             *
             * @return (AR) قائمة الأسماء / (EN) List of names
             */
            std::vector<std::string> getFunctionNames() const;

            /**
             * @brief (AR) عدد الدوال المُعرفة
             * @brief (EN) Number of defined functions
             *
             * @return (AR) العدد الكلي / (EN) Total count
             */
            size_t getFunctionCount() const;

            // ========================================
            // (AR) عمليات مساعدة
            // (EN) Utility Operations
            // ========================================

            /**
             * @brief (AR) مسح جميع الدوال
             * @brief (EN) Clear all functions
             */
            void clear();

            /**
             * @brief (AR) طباعة جميع الدوال (للتصحيح)
             * @brief (EN) Print all functions (for debugging)
             */
            void printAllFunctions() const;

            /**
             * @brief (AR) الحصول على معلومات عن دالة
             * @brief (EN) Get information about function
             *
             * @param name (AR) اسم الدالة / (EN) Function name
             * @param argCount (AR) عدد المعاملات / (EN) Argument count
             * @return (AR) معلومات الدالة / (EN) Function information
             */
            std::string getFunctionInfo(const std::string &name, size_t argCount) const;

            /**
             * @brief (AR) الحصول على نص تصحيح
             * @brief (EN) Get debug string
             */
            std::string debugString() const;

            // ========================================
            // (AR) تتبع تسجيل الدوال — لدعم loadModule بشكل صحيح
            // (EN) Registration tracking — to support loadModule correctly
            // ========================================

            /**
             * @brief (AR) بدء تتبع تسجيل الدوال (لالتقاط كل ما يسجله المسجّل)
             * @brief (EN) Start tracking function registrations
             */
            void startRegistrationTracking();

            /**
             * @brief (AR) إيقاف التتبع وإرجاع قائمة الأسماء المُسجّلة (بدون تكرار)
             * @brief (EN) Stop tracking and return list of registered names (deduplicated)
             */
            std::vector<std::string> stopRegistrationTracking();

        private:
            // (AR) خريطة الدوال: الاسم -> قائمة من الدوال (للدعم Overloading)
            // (EN) Function map: name -> list of functions (for overloading support)
            std::unordered_map<std::string, std::vector<std::shared_ptr<FunctionDefinition>>> functions_;

            // (AR) متغيرات تتبع التسجيل — تُستخدم بواسطة loadModule لمعرفة
            //      الدوال التي سجّلها المسجّل فعلاً (حتى لو كانت موجودة مسبقاً)
            // (EN) Registration tracking variables — used by loadModule to know
            //      which functions the registrar actually registered (even if pre-existing)
            bool trackingRegistrations_ = false;
            std::vector<std::string> trackedRegistrations_;

            /**
             * @brief (AR) رمي خطأ ثنائي اللغة
             * @brief (EN) Throw bilingual error
             */
            void throwError(const std::string &messageAr, const std::string &messageEn) const;
        };

    } // namespace Data
} // namespace Sad
