/**
 * @file function_manager.cpp
 * @brief (AR) تنفيذ مدير الدوال
 * @brief (EN) Function Manager implementation
 *
 * @author S Language Development Team
 * @date November 21, 2025
 */

#include "function_manager.h"
#include "expressions.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <unordered_set>

namespace
{
    // (AR) إزالة الحركات والتنوين من الأسماء العربية حتى تطابق ما يخرجه
    //      المحلل المعجمي عند تجاهل التشكيل.
    // (EN) Strip Arabic diacritics so builtin names match lexer-normalized
    //      identifiers when harakat are ignored.
    std::string stripArabicDiacritics(const std::string &text)
    {
        std::string result;
        result.reserve(text.size());

        for (size_t i = 0; i < text.size();)
        {
            if (i + 1 < text.size())
            {
                const unsigned char lead = static_cast<unsigned char>(text[i]);
                const unsigned char next = static_cast<unsigned char>(text[i + 1]);
                const bool isArabicHaraka =
                    (lead == 0xD9 && ((next >= 0x8B && next <= 0x9F) || next == 0xB0)) ||
                    (lead == 0xDA && (next >= 0x96 && next <= 0x9F));

                if (isArabicHaraka)
                {
                    i += 2;
                    continue;
                }
            }

            result.push_back(text[i]);
            ++i;
        }

        return result;
    }
}

namespace Sad
{
    namespace Data
    {

        // ============================================================================
        // FunctionDefinition Implementation
        // ============================================================================

        FunctionDefinition::FunctionDefinition(const std::string &name,
                                               const std::vector<FunctionParameter> &params,
                                               std::shared_ptr<AST::ASTNode> body)
            : name_(name), type_(FunctionType::USER_DEFINED), parameters_(params),
              body_(body), declaration_(body), functionDecl_(nullptr),
              nativeImplementationCtx_(nullptr), returnType_("auto")
        {
            // (AR) نحفظ body كـ declaration للوصول للـ Parameters
            // (EN) Save body as declaration to access Parameters
        }

        // (AR) EM-CPP: باني الدالة المضمنة بالتوقيع الموحَّد (BuiltinContext).
        FunctionDefinition::FunctionDefinition(const std::string &name,
                                               const std::vector<FunctionParameter> &params,
                                               std::function<std::shared_ptr<Data::Value>(Sad::Interpreter::BuiltinContext &)> nativeImpl)
            : name_(name), type_(FunctionType::BUILT_IN), parameters_(params),
              body_(nullptr), declaration_(nullptr), functionDecl_(nullptr),
              nativeImplementationCtx_(nativeImpl), returnType_("auto")
        {
        }

        size_t FunctionDefinition::getRequiredParameterCount() const
        {
            // (AR) حساب عدد المعاملات الإلزامية (بدون قيم افتراضية)
            // (EN) Calculate required parameters (without default values)
            size_t count = 0;
            for (const auto &param : parameters_)
            {
                if (!param.hasDefaultValue)
                {
                    count++;
                }
            }
            return count;
        }

        bool FunctionDefinition::matchesParameterCount(size_t argCount) const
        {
            // (AR) التحقق من تطابق عدد المعاملات
            // (EN) Check parameter count matching

            size_t required = getRequiredParameterCount();
            size_t total = parameters_.size();

            // (AR) يجب أن يكون عدد المعاملات بين الإلزامية والكلية
            // (EN) Argument count must be between required and total
            return argCount >= required && argCount <= total;
        }

        bool FunctionDefinition::acceptsArgumentCount(size_t argCount) const
        {
            // (AR) التحقق من قبول عدد معاملات (مع القيم الافتراضية)
            // (EN) Check if function accepts argument count (with defaults)

            size_t required = getRequiredParameterCount();
            size_t total = parameters_.size();

            // (AR) نقبل إذا كان العدد بين الإلزامية والكلية
            // (EN) Accept if count is between required and total
            return argCount >= required && argCount <= total;
        }

        void FunctionDefinition::setFunctionDecl(std::shared_ptr<AST::ASTNode> decl)
        {
            // (AR) حفظ FunctionDecl للوصول لـ Parameters مع defaultValue
            // (EN) Save FunctionDecl to access Parameters with defaultValue
            functionDecl_ = decl;
        }

        std::string FunctionDefinition::getSignature() const
        {
            // (AR) إنشاء توقيع الدالة: functionName(param1, param2, ...)
            // (EN) Create function signature: functionName(param1, param2, ...)

            std::ostringstream oss;
            oss << name_ << "(";

            for (size_t i = 0; i < parameters_.size(); ++i)
            {
                if (i > 0)
                    oss << ", ";

                oss << parameters_[i].name;
                if (!parameters_[i].type.empty())
                {
                    oss << ": " << parameters_[i].type;
                }
                if (parameters_[i].hasDefaultValue)
                {
                    oss << " = " << parameters_[i].defaultValue;
                }
            }

            oss << ")";
            if (returnType_ != "auto")
            {
                oss << " -> " << returnType_;
            }

            return oss.str();
        }

        std::string FunctionDefinition::debugString() const
        {
            // (AR) معلومات تفصيلية للتصحيح
            // (EN) Detailed debug information

            std::ostringstream oss;
            oss << "Function: " << getSignature() << "\n";
            oss << "  Type: ";
            switch (type_)
            {
            case FunctionType::USER_DEFINED:
                oss << "USER_DEFINED";
                break;
            case FunctionType::BUILT_IN:
                oss << "BUILT_IN";
                break;
            case FunctionType::LAMBDA:
                oss << "LAMBDA";
                break;
            }
            oss << "\n";
            oss << "  Parameters: " << parameters_.size() << " total, "
                << getRequiredParameterCount() << " required\n";
            oss << "  Has body: " << (hasBody() ? "yes" : "no") << "\n";

            return oss.str();
        }

        // ============================================================================
        // FunctionManager Implementation
        // ============================================================================

        FunctionManager::FunctionManager()
        {
            // (AR) المُنشئ - لا حاجة لتهيئة خاصة
            // (EN) Constructor - no special initialization needed
        }

        FunctionManager::~FunctionManager()
        {
            // (AR) المُدمر - التنظيف التلقائي
            // (EN) Destructor - automatic cleanup
            clear();
        }

        void FunctionManager::defineFunction(const std::string &name,
                                             const std::vector<FunctionParameter> &params,
                                             std::shared_ptr<AST::ASTNode> body)
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            // (AR) تعريف دالة معرفة من المستخدم
            // (EN) Define user-defined function

            // (AR) التحقق من صحة الاسم
            // (EN) Validate name
            if (name.empty())
            {
                ::Sad::Errors::throwRuntime(::Sad::Errors::ErrorCode::INT_INTERP_NAMELESS_DEFINITION,
                                            Sad::Lexer::Position{}, {{"detail", "دالة / function"}});
            }

            // (AR) السماح بإعادة تعريف الدوال — ضروري للإغلاقات والدوال المتداخلة
            // (EN) Allow re-definition — essential for closures and nested functions
            size_t paramCount = params.size();
            auto it = functions_.find(name);
            if (it != functions_.end())
            {
                // (AR) إزالة التعريف السابق بنفس التوقيع (مضمنة أو مستخدمة)
                // (EN) Remove previous definition with same signature (built-in or user-defined)
                it->second.erase(
                    std::remove_if(it->second.begin(), it->second.end(),
                                   [paramCount](const auto &func)
                                   {
                                       return func->getParameterCount() == paramCount;
                                   }),
                    it->second.end());
            }

            // (AR) إنشاء تعريف الدالة
            // (EN) Create function definition
            auto funcDef = std::make_shared<FunctionDefinition>(name, params, body);

            // (AR) إضافة الدالة للخريطة
            // (EN) Add function to map
            functions_[name].push_back(funcDef);
        }

        void FunctionManager::defineFunction(const std::string &name,
                                             const std::vector<FunctionParameter> &params,
                                             std::shared_ptr<AST::ASTNode> body,
                                             std::shared_ptr<AST::ASTNode> decl)
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            // (AR) تعريف دالة معرفة من المستخدم مع FunctionDecl
            // (EN) Define user-defined function with FunctionDecl

            // (AR) التحقق من صحة الاسم
            // (EN) Validate name
            if (name.empty())
            {
                ::Sad::Errors::throwRuntime(::Sad::Errors::ErrorCode::INT_INTERP_NAMELESS_DEFINITION,
                                            Sad::Lexer::Position{}, {{"detail", "دالة / function"}});
            }

            // (AR) السماح بإعادة تعريف الدوال — ضروري للإغلاقات والدوال المتداخلة
            // (EN) Allow re-definition — essential for closures and nested functions
            size_t paramCount = params.size();
            auto it = functions_.find(name);
            if (it != functions_.end())
            {
                // (AR) إزالة التعريف السابق بنفس التوقيع (مضمنة أو مستخدمة)
                // (EN) Remove previous definition with same signature (built-in or user-defined)
                it->second.erase(
                    std::remove_if(it->second.begin(), it->second.end(),
                                   [paramCount](const auto &func)
                                   {
                                       return func->getParameterCount() == paramCount;
                                   }),
                    it->second.end());
            }

            // (AR) إنشاء تعريف الدالة
            // (EN) Create function definition
            auto funcDef = std::make_shared<FunctionDefinition>(name, params, body);

            // (AR) حفظ FunctionDecl للوصول لـ default parameters
            // (EN) Save FunctionDecl for default parameters access
            funcDef->setFunctionDecl(decl);

            // (AR) إضافة الدالة للخريطة
            // (EN) Add function to map
            functions_[name].push_back(funcDef);
        }

        void FunctionManager::redefineFunction(const std::string &name,
                                               const std::vector<FunctionParameter> &params,
                                               std::shared_ptr<AST::ASTNode> body)
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            // (AR) إعادة تعريف دالة موجودة — يُستخدم للمزخرفات
            // (EN) Redefine existing function — used for decorators
            size_t paramCount = params.size();

            auto it = functions_.find(name);
            if (it != functions_.end())
            {
                // (AR) إزالة التعريف القديم بنفس عدد المعاملات
                // (EN) Remove old definition with same parameter count
                it->second.erase(
                    std::remove_if(it->second.begin(), it->second.end(),
                                   [paramCount](const auto &func)
                                   {
                                       return func->getParameterCount() == paramCount;
                                   }),
                    it->second.end());
            }

            // (AR) إنشاء التعريف الجديد
            // (EN) Create new definition
            auto funcDef = std::make_shared<FunctionDefinition>(name, params, body);
            functions_[name].push_back(funcDef);
        }

        // (AR) EM-CPP: تعريف دالة مضمنة بالتوقيع الموحَّد (BuiltinContext&).
        // (EN) EM-CPP: define a built-in with the unified (BuiltinContext&) signature.
        void FunctionManager::defineBuiltInFunction(const std::string &name,
                                                    const std::vector<FunctionParameter> &params,
                                                    std::function<std::shared_ptr<Data::Value>(Sad::Interpreter::BuiltinContext &)> impl)
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            if (name.empty())
            {
                ::Sad::Errors::throwRuntime(::Sad::Errors::ErrorCode::INT_INTERP_NAMELESS_DEFINITION,
                                            Sad::Lexer::Position{}, {{"detail", "دالة مضمنة / builtin"}});
            }
            auto funcDef = std::make_shared<FunctionDefinition>(name, params, impl);
            functions_[name].push_back(funcDef);
        }

        FunctionManager::FunctionTableSnapshot FunctionManager::snapshotFunctionTable() const
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            return functions_;
        }

        void FunctionManager::restoreFunctionFromSnapshot(const std::string &name,
                                                          const FunctionTableSnapshot &snapshot)
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            auto previous = snapshot.find(name);
            if (previous == snapshot.end())
                functions_.erase(name);
            else
                functions_[name] = previous->second;
        }

        size_t FunctionManager::removeFunction(const std::string &name, int paramCount)
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            // (AR) حذف دالة أو مجموعة دوال
            // (EN) Remove function or group of functions

            auto it = functions_.find(name);
            if (it == functions_.end())
            {
                return 0; // (AR) لا توجد دوال بهذا الاسم / (EN) No functions with this name
            }

            if (paramCount == -1)
            {
                // (AR) حذف جميع الدوال بهذا الاسم
                // (EN) Remove all functions with this name
                size_t count = it->second.size();
                functions_.erase(it);
                return count;
            }
            else
            {
                // (AR) حذف الدالة بعدد معاملات محدد
                // (EN) Remove function with specific parameter count
                auto &overloads = it->second;
                auto funcIt = std::remove_if(overloads.begin(), overloads.end(),
                                             [paramCount](const std::shared_ptr<FunctionDefinition> &func)
                                             {
                                                 return func->getParameterCount() == static_cast<size_t>(paramCount);
                                             });

                if (funcIt != overloads.end())
                {
                    overloads.erase(funcIt, overloads.end());

                    // (AR) إذا لم يبق أي دالة، احذف المفتاح
                    // (EN) If no functions left, remove key
                    if (overloads.empty())
                    {
                        functions_.erase(it);
                    }
                    return 1;
                }
                return 0;
            }
        }

        std::shared_ptr<FunctionDefinition> FunctionManager::getFunction(
            const std::string &name, size_t argCount) const
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            // (AR) البحث عن دالة بالاسم وعدد المعاملات
            // (EN) Find function by name and argument count

            auto it = functions_.find(name);
            if (it == functions_.end())
            {
                return nullptr; // (AR) لا توجد دالة بهذا الاسم / (EN) No function with this name
            }

            // (AR) البحث عن أفضل تطابق
            // (EN) Find best match
            const auto &overloads = it->second;
            for (const auto &func : overloads)
            {
                if (func->matchesParameterCount(argCount))
                {
                    return func;
                }
            }

            return nullptr; // (AR) لا يوجد تطابق / (EN) No match
        }

        bool FunctionManager::hasFunction(const std::string &name, int argCount) const
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            // (AR) التحقق من وجود دالة
            // (EN) Check function existence

            auto it = functions_.find(name);
            if (it == functions_.end())
            {
                return false;
            }

            if (argCount == -1)
            {
                // (AR) البحث عن أي دالة بهذا الاسم
                // (EN) Look for any function with this name
                return !it->second.empty();
            }
            else
            {
                // (AR) البحث عن دالة بعدد معاملات محدد
                // (EN) Look for function with specific parameter count
                return getFunction(name, argCount) != nullptr;
            }
        }

        bool FunctionManager::hasUserFunction(const std::string &name) const
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            // (AR) دالّة مستخدم = تحمل الاسم ولا تملك تنفيذًا أصليًّا (ليست مُدمَجة).
            // (EN) User function = named overload without a native implementation (not a builtin).
            auto it = functions_.find(name);
            if (it == functions_.end())
            {
                return false;
            }
            for (const auto &def : it->second)
            {
                if (def && !def->hasNativeImplementation())
                {
                    return true;
                }
            }
            return false;
        }

        std::vector<std::shared_ptr<FunctionDefinition>> FunctionManager::getFunctionOverloads(
            const std::string &name) const
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            // (AR) الحصول على جميع الدوال بنفس الاسم
            // (EN) Get all functions with same name

            auto it = functions_.find(name);
            if (it == functions_.end())
            {
                return {}; // (AR) قائمة فارغة / (EN) Empty list
            }

            return it->second;
        }

        std::vector<std::string> FunctionManager::getFunctionNames() const
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            // (AR) الحصول على قائمة بأسماء جميع الدوال
            // (EN) Get list of all function names

            std::vector<std::string> names;
            names.reserve(functions_.size());

            for (const auto &pair : functions_)
            {
                names.push_back(pair.first);
            }

            // (AR) ترتيب أبجدي
            // (EN) Sort alphabetically
            std::sort(names.begin(), names.end());

            return names;
        }

        size_t FunctionManager::getFunctionCount() const
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            // (AR) حساب عدد جميع الدوال (بما فيها Overloads)
            // (EN) Count all functions (including overloads)

            size_t count = 0;
            for (const auto &pair : functions_)
            {
                count += pair.second.size();
            }
            return count;
        }

        void FunctionManager::clear()
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            // (AR) مسح جميع الدوال
            // (EN) Clear all functions
            functions_.clear();
        }

        void FunctionManager::printAllFunctions() const
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            // (AR) طباعة جميع الدوال (للتصحيح)
            // (EN) Print all functions (for debugging)

            std::cout << "╔════════════════════════════════════════════════════╗\n";
            std::cout << "║  FUNCTION MANAGER - ALL FUNCTIONS                 ║\n";
            std::cout << "║  (AR) مدير الدوال - جميع الدوال                 ║\n";
            std::cout << "╚════════════════════════════════════════════════════╝\n\n";

            if (functions_.empty())
            {
                std::cout << "(AR) لا توجد دوال معرفة\n";
                std::cout << "(EN) No functions defined\n";
                return;
            }

            std::cout << "Total functions: " << getFunctionCount()
                      << " (" << functions_.size() << " unique names)\n\n";

            for (const auto &pair : functions_)
            {
                std::cout << "Function name: '" << pair.first << "' "
                          << "(" << pair.second.size() << " overload(s))\n";

                for (const auto &func : pair.second)
                {
                    std::cout << "  " << func->getSignature() << "\n";
                }
                std::cout << "\n";
            }
        }

        std::string FunctionManager::getFunctionInfo(const std::string &name,
                                                     size_t argCount) const
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            // (AR) الحصول على معلومات عن دالة
            // (EN) Get information about function

            auto func = getFunction(name, argCount);
            if (!func)
            {
                std::ostringstream oss;
                oss << "(AR) دالة '" << name << "' بعدد معاملات " << argCount
                    << " غير موجودة\n";
                oss << "(EN) Function '" << name << "' with " << argCount
                    << " parameters not found";
                return oss.str();
            }

            return func->debugString();
        }

        std::string FunctionManager::debugString() const
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            // (AR) نص تصحيح شامل
            // (EN) Comprehensive debug string

            std::ostringstream oss;
            oss << "FunctionManager Debug Info:\n";
            oss << "  Total functions: " << getFunctionCount() << "\n";
            oss << "  Unique names: " << functions_.size() << "\n";

            if (!functions_.empty())
            {
                oss << "\nFunction List:\n";
                for (const auto &pair : functions_)
                {
                    oss << "  - " << pair.first << " (" << pair.second.size()
                        << " overload(s))\n";
                    for (const auto &func : pair.second)
                    {
                        oss << "    * " << func->getSignature() << "\n";
                    }
                }
            }

            return oss.str();
        }

        // (AR) EM-CPP: حُذف throwError — كل المستدعين رُحِّلوا إلى الكتالوج (throwRuntime).
        // (EN) EM-CPP: throwError removed — all callers migrated to the catalog.

        // ============================================================================
        // (AR) تسجيل الدوال المضمنة
        // (EN) Register Built-in Functions
        // ============================================================================

        // (AR) EM-CPP: تسجيل دالة مضمنة بالتوقيع الموحَّد (BuiltinContext&).
        // (EN) EM-CPP: register a built-in with the unified (BuiltinContext&) signature.
        void FunctionManager::registerBuiltinFunction(
            const std::string &name,
            const std::function<std::shared_ptr<Data::Value>(Sad::Interpreter::BuiltinContext &)> &func)
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);

            // (AR) 🔴 قيدٌ مقيسٌ **مُعلَنٌ لا مسدود**: المدمَجةُ تدهس دالّةَ المستخدمِ
            //      التي تشاركها الاسمَ عند تحميلِ وحدةٍ مضمَّنة — ولو جاء التحميلُ من
            //      **داخلِ مكتبةٍ** يستوردها المستخدم. قِيس: `دالة نص حرف_من_رمز(...)`
            //      تُعطي «MINE» ثمّ «A» بعد `استورد حلل من جيسون`.
            //      جرّبتُ سدَّه بحارسٍ يمنع دهسَ تعريفٍ غيرِ مدمَجٍ، **فأنتج عطبًا أسوأ**:
            //      يُلغى تسجيلُ الاسمِ فيُخفِق استيرادُ المكتبةِ نفسِها («الرمز غير موجود
            //      في الوحدة المضمنة») — أي أنّ دالّةً يكتبها المستخدمُ تكسر مكتبةً لا
            //      علاقةَ لها به. ولو مرّرنا التحقّقَ لَحُلَّ الاسمُ داخلَ المكتبةِ إلى
            //      دالّةِ المستخدمِ: فسادٌ صامتٌ محلَّ إخفاقٍ صاخب.
            //      فالعلاجُ الصحيحُ **نطاقُ أسماءٍ لكلِّ ملفّ/وحدة** لا حارسٌ هنا:
            //      المدمَجاتُ اليومَ في جدولٍ عامٍّ واحدٍ بلا نطاق. قرارُ بنيةٍ يخصّ المالك.
            // (EN) 🔴 A MEASURED, DECLARED limitation, not a fix: a builtin clobbers a
            //      user function of the same name when a builtin module loads — even when
            //      the load comes from INSIDE a library the user imported. Guarding against
            //      it produced a WORSE defect: the name stops being registered, so the
            //      library's own import fails; and letting the verification pass would
            //      resolve the name inside the library to the user's function — silent
            //      corruption instead of a loud failure. The real fix is per-file/module
            //      name scoping, not a guard here. An architecture decision for the owner.
            removeFunction(name);

            std::vector<FunctionParameter> params;
            auto funcDef = std::make_shared<FunctionDefinition>(name, params, func);
            functions_[name].push_back(funcDef);

            // (AR) أضف alias بلا تشكيل إذا كان الاسم يحتوي شدة/تنوين (يزيله lexer).
            // (EN) Add a diacritics-free alias if the name contains harakat.
            const std::string normalizedName = stripArabicDiacritics(name);
            if (!normalizedName.empty() && normalizedName != name && !hasFunction(normalizedName))
            {
                auto normalizedDef = std::make_shared<FunctionDefinition>(normalizedName, params, func);
                functions_[normalizedName].push_back(normalizedDef);
                if (trackingRegistrations_)
                {
                    trackedRegistrations_.push_back(normalizedName);
                }
            }

            if (trackingRegistrations_)
            {
                trackedRegistrations_.push_back(name);
            }
        }

        // ============================================================================
        // (AR) تتبع تسجيل الدوال — آلية تتبع لدعم loadModule
        // (EN) Registration tracking — tracking mechanism to support loadModule
        // ============================================================================

        void FunctionManager::startRegistrationTracking()
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            trackingRegistrations_ = true;
            trackedRegistrations_.clear();
        }

        std::vector<std::string> FunctionManager::stopRegistrationTracking()
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            trackingRegistrations_ = false;

            // (AR) إزالة التكرارات مع الحفاظ على الترتيب
            // (EN) Deduplicate while preserving order
            std::unordered_set<std::string> seen;
            std::vector<std::string> unique;
            unique.reserve(trackedRegistrations_.size());
            for (auto &name : trackedRegistrations_)
            {
                if (seen.insert(name).second)
                {
                    unique.push_back(std::move(name));
                }
            }
            trackedRegistrations_.clear();
            return unique;
        }

    } // namespace Data
} // namespace Sad
