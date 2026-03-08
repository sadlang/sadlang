// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  sad_flutter_forms.h — النماذج والتحقق بأسلوب Flutter
//  Forms, FormFields, Validators
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "sad_flutter_core.h"
#include "sad_flutter_widgets.h"
#include "sad_flutter_input.h"
#include <regex>
#include <functional>

namespace sad {
namespace flutter {

// ═══════════════════════════════════════════════════════════════════════════════
//  FormFieldValidator — مدقق حقل النموذج
// ═══════════════════════════════════════════════════════════════════════════════

/// نوع دالة التحقق — تُرجع رسالة خطأ أو nullopt إذا كانت القيمة صحيحة
template<typename T>
using FormFieldValidator = std::function<std::optional<std::string>(const T&)>;

// مُدققات جاهزة للنصوص
namespace Validators {

/// حقل مطلوب
inline FormFieldValidator<std::string> required(
    const std::string& errorMessage = "هذا الحقل مطلوب"
) {
    return [errorMessage](const std::string& value) -> std::optional<std::string> {
        if (value.empty()) return errorMessage;
        return std::nullopt;
    };
}

/// الحد الأدنى للطول
inline FormFieldValidator<std::string> minLength(
    size_t length,
    const std::string& errorMessage = ""
) {
    return [length, errorMessage](const std::string& value) -> std::optional<std::string> {
        if (value.length() < length) {
            return errorMessage.empty() 
                ? "يجب أن يكون على الأقل " + std::to_string(length) + " أحرف"
                : errorMessage;
        }
        return std::nullopt;
    };
}

/// الحد الأقصى للطول
inline FormFieldValidator<std::string> maxLength(
    size_t length,
    const std::string& errorMessage = ""
) {
    return [length, errorMessage](const std::string& value) -> std::optional<std::string> {
        if (value.length() > length) {
            return errorMessage.empty() 
                ? "يجب أن لا يتجاوز " + std::to_string(length) + " أحرف"
                : errorMessage;
        }
        return std::nullopt;
    };
}

/// نطاق الطول
inline FormFieldValidator<std::string> lengthRange(
    size_t min, size_t max,
    const std::string& errorMessage = ""
) {
    return [min, max, errorMessage](const std::string& value) -> std::optional<std::string> {
        if (value.length() < min || value.length() > max) {
            return errorMessage.empty()
                ? "يجب أن يكون بين " + std::to_string(min) + " و " + std::to_string(max) + " أحرف"
                : errorMessage;
        }
        return std::nullopt;
    };
}

/// بريد إلكتروني صحيح
inline FormFieldValidator<std::string> email(
    const std::string& errorMessage = "بريد إلكتروني غير صحيح"
) {
    return [errorMessage](const std::string& value) -> std::optional<std::string> {
        if (value.empty()) return std::nullopt; // اختياري - استخدم required للإلزام
        
        // تعبير منتظم بسيط للبريد
        std::regex emailRegex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
        if (!std::regex_match(value, emailRegex)) {
            return errorMessage;
        }
        return std::nullopt;
    };
}

/// رقم هاتف (أرقام فقط مع اختياري +)
inline FormFieldValidator<std::string> phone(
    const std::string& errorMessage = "رقم هاتف غير صحيح"
) {
    return [errorMessage](const std::string& value) -> std::optional<std::string> {
        if (value.empty()) return std::nullopt;
        
        std::regex phoneRegex(R"(\+?[0-9]{8,15})");
        if (!std::regex_match(value, phoneRegex)) {
            return errorMessage;
        }
        return std::nullopt;
    };
}

/// أرقام فقط
inline FormFieldValidator<std::string> numeric(
    const std::string& errorMessage = "يجب إدخال أرقام فقط"
) {
    return [errorMessage](const std::string& value) -> std::optional<std::string> {
        if (value.empty()) return std::nullopt;
        
        for (char c : value) {
            if (!std::isdigit(c) && c != '.' && c != '-') {
                return errorMessage;
            }
        }
        return std::nullopt;
    };
}

/// تطابق مع نمط
inline FormFieldValidator<std::string> pattern(
    const std::string& regex,
    const std::string& errorMessage = "القيمة لا تطابق النمط المطلوب"
) {
    return [regex, errorMessage](const std::string& value) -> std::optional<std::string> {
        if (value.empty()) return std::nullopt;
        
        std::regex r(regex);
        if (!std::regex_match(value, r)) {
            return errorMessage;
        }
        return std::nullopt;
    };
}

/// تطابق مع حقل آخر (مثل تأكيد كلمة المرور)
inline FormFieldValidator<std::string> matches(
    const std::string* otherValue,
    const std::string& errorMessage = "القيمتان غير متطابقتين"
) {
    return [otherValue, errorMessage](const std::string& value) -> std::optional<std::string> {
        if (value != *otherValue) {
            return errorMessage;
        }
        return std::nullopt;
    };
}

/// نطاق رقمي
inline FormFieldValidator<std::string> range(
    double min, double max,
    const std::string& errorMessage = ""
) {
    return [min, max, errorMessage](const std::string& value) -> std::optional<std::string> {
        if (value.empty()) return std::nullopt;
        
        try {
            double num = std::stod(value);
            if (num < min || num > max) {
                return errorMessage.empty()
                    ? "يجب أن تكون القيمة بين " + std::to_string(min) + " و " + std::to_string(max)
                    : errorMessage;
            }
        } catch (...) {
            return "قيمة رقمية غير صحيحة";
        }
        return std::nullopt;
    };
}

/// دمج عدة مُدققات
inline FormFieldValidator<std::string> compose(
    std::initializer_list<FormFieldValidator<std::string>> validators
) {
    auto validatorList = std::vector<FormFieldValidator<std::string>>(validators);
    return [validatorList](const std::string& value) -> std::optional<std::string> {
        for (const auto& validator : validatorList) {
            auto result = validator(value);
            if (result.has_value()) {
                return result;
            }
        }
        return std::nullopt;
    };
}

} // namespace Validators

// ═══════════════════════════════════════════════════════════════════════════════
//  FormFieldState — حالة حقل النموذج
// ═══════════════════════════════════════════════════════════════════════════════

template<typename T>
class FormFieldState {
public:
    FormFieldState(T initialValue = T())
        : value_(initialValue), errorText_(std::nullopt), hasInteracted_(false) {}
    
    const T& value() const { return value_; }
    void setValue(const T& v) { value_ = v; hasInteracted_ = true; }
    
    std::optional<std::string> errorText() const { return errorText_; }
    void setErrorText(std::optional<std::string> error) { errorText_ = error; }
    
    bool hasError() const { return errorText_.has_value(); }
    bool hasInteracted() const { return hasInteracted_; }
    
    /// التحقق من القيمة
    bool validate(FormFieldValidator<T> validator) {
        errorText_ = validator(value_);
        return !hasError();
    }
    
    /// إعادة التعيين
    void reset(T initialValue = T()) {
        value_ = initialValue;
        errorText_ = std::nullopt;
        hasInteracted_ = false;
    }
    
private:
    T value_;
    std::optional<std::string> errorText_;
    bool hasInteracted_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  FormState — حالة النموذج بالكامل
// ═══════════════════════════════════════════════════════════════════════════════

class FormState {
public:
    /// تسجيل حقل
    void registerField(const std::string& name, FormFieldState<std::string>* field) {
        fields_[name] = field;
    }
    
    /// إلغاء تسجيل حقل
    void unregisterField(const std::string& name) {
        fields_.erase(name);
    }
    
    /// التحقق من جميع الحقول
    bool validate() {
        bool allValid = true;
        for (auto& [name, field] : fields_) {
            auto it = validators_.find(name);
            if (it != validators_.end()) {
                if (!field->validate(it->second)) {
                    allValid = false;
                }
            }
        }
        return allValid;
    }
    
    /// تعيين مُدقق لحقل
    void setValidator(const std::string& name, FormFieldValidator<std::string> validator) {
        validators_[name] = std::move(validator);
    }
    
    /// إعادة تعيين النموذج
    void reset() {
        for (auto& [name, field] : fields_) {
            field->reset();
        }
    }
    
    /// الحصول على قيم جميع الحقول
    std::unordered_map<std::string, std::string> getValues() const {
        std::unordered_map<std::string, std::string> values;
        for (const auto& [name, field] : fields_) {
            values[name] = field->value();
        }
        return values;
    }
    
    /// هل النموذج صحيح؟
    bool isValid() const {
        for (const auto& [name, field] : fields_) {
            if (field->hasError()) return false;
        }
        return true;
    }
    
private:
    std::unordered_map<std::string, FormFieldState<std::string>*> fields_;
    std::unordered_map<std::string, FormFieldValidator<std::string>> validators_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Form Widget — مكون النموذج
// ═══════════════════════════════════════════════════════════════════════════════

class Form : public StatefulWidget {
public:
    Form(
        WidgetPtr child,
        std::function<void(FormState&)> onSaved = nullptr,
        bool autovalidateMode = false, // تحقق تلقائي عند التغيير
        Key key = Key()
    )
        : StatefulWidget(std::move(key))
        , child_(std::move(child))
        , onSaved_(std::move(onSaved))
        , autovalidateMode_(autovalidateMode)
    {}
    
    std::unique_ptr<State_Base> createState() override;
    std::string typeName() const override { return "Form"; }
    
    WidgetPtr child() const { return child_; }
    
private:
    WidgetPtr child_;
    std::function<void(FormState&)> onSaved_;
    bool autovalidateMode_;
    
    friend class FormWidgetState;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  TextFormField — حقل نص مع تحقق
// ═══════════════════════════════════════════════════════════════════════════════

class TextFormField : public StatefulWidget {
public:
    TextFormField(
        std::string initialValue = "",
        FormFieldValidator<std::string> validator = nullptr,
        std::function<void(const std::string&)> onChanged = nullptr,
        std::function<void(const std::string&)> onSaved = nullptr,
        std::function<void()> onEditingComplete = nullptr,
        InputDecoration decoration = InputDecoration(),
        TextInputType keyboardType = TextInputType::Text,
        bool obscureText = false,
        int maxLines = 1,
        int maxLength = -1,
        bool enabled = true,
        bool readOnly = false,
        bool autofocus = false,
        std::shared_ptr<TextEditingController> controller = nullptr,
        Key key = Key()
    )
        : StatefulWidget(std::move(key))
        , initialValue_(std::move(initialValue))
        , validator_(std::move(validator))
        , onChanged_(std::move(onChanged))
        , onSaved_(std::move(onSaved))
        , onEditingComplete_(std::move(onEditingComplete))
        , decoration_(std::move(decoration))
        , keyboardType_(keyboardType)
        , obscureText_(obscureText)
        , maxLines_(maxLines)
        , maxLength_(maxLength)
        , enabled_(enabled)
        , readOnly_(readOnly)
        , autofocus_(autofocus)
        , controller_(std::move(controller))
    {}
    
    std::unique_ptr<State_Base> createState() override;
    std::string typeName() const override { return "TextFormField"; }
    
private:
    std::string initialValue_;
    FormFieldValidator<std::string> validator_;
    std::function<void(const std::string&)> onChanged_;
    std::function<void(const std::string&)> onSaved_;
    std::function<void()> onEditingComplete_;
    InputDecoration decoration_;
    TextInputType keyboardType_;
    bool obscureText_;
    int maxLines_, maxLength_;
    bool enabled_, readOnly_, autofocus_;
    std::shared_ptr<TextEditingController> controller_;
    
    friend class TextFormFieldState;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  DropdownButtonFormField — قائمة منسدلة مع تحقق
// ═══════════════════════════════════════════════════════════════════════════════

template<typename T>
class DropdownButtonFormField : public StatefulWidget {
public:
    DropdownButtonFormField(
        std::vector<std::pair<T, std::string>> items,
        std::optional<T> value = std::nullopt,
        FormFieldValidator<std::optional<T>> validator = nullptr,
        std::function<void(T)> onChanged = nullptr,
        InputDecoration decoration = InputDecoration(),
        Key key = Key()
    )
        : StatefulWidget(std::move(key))
        , items_(std::move(items))
        , value_(value)
        , validator_(std::move(validator))
        , onChanged_(std::move(onChanged))
        , decoration_(std::move(decoration))
    {}
    
    std::unique_ptr<State_Base> createState() override;
    std::string typeName() const override { return "DropdownButtonFormField"; }
    
private:
    std::vector<std::pair<T, std::string>> items_;
    std::optional<T> value_;
    FormFieldValidator<std::optional<T>> validator_;
    std::function<void(T)> onChanged_;
    InputDecoration decoration_;
    
    template<typename U> friend class DropdownButtonFormFieldState;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  CheckboxFormField — خانة اختيار مع تحقق
// ═══════════════════════════════════════════════════════════════════════════════

class CheckboxFormField : public StatefulWidget {
public:
    CheckboxFormField(
        bool initialValue = false,
        FormFieldValidator<bool> validator = nullptr,
        std::function<void(bool)> onChanged = nullptr,
        WidgetPtr title = nullptr,
        bool enabled = true,
        Key key = Key()
    )
        : StatefulWidget(std::move(key))
        , initialValue_(initialValue)
        , validator_(std::move(validator))
        , onChanged_(std::move(onChanged))
        , title_(std::move(title))
        , enabled_(enabled)
    {}
    
    std::unique_ptr<State_Base> createState() override;
    std::string typeName() const override { return "CheckboxFormField"; }
    
private:
    bool initialValue_;
    FormFieldValidator<bool> validator_;
    std::function<void(bool)> onChanged_;
    WidgetPtr title_;
    bool enabled_;
    
    friend class CheckboxFormFieldState;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  FormKey — مفتاح للوصول إلى حالة النموذج
// ═══════════════════════════════════════════════════════════════════════════════

class FormKey {
public:
    FormKey() : state_(nullptr) {}
    
    FormState* currentState() const { return state_; }
    void setCurrentState(FormState* state) { state_ = state; }
    
    /// التحقق من النموذج
    bool validate() {
        return state_ ? state_->validate() : false;
    }
    
    /// إعادة تعيين النموذج
    void reset() {
        if (state_) state_->reset();
    }
    
    /// هل النموذج صحيح؟
    bool isValid() const {
        return state_ ? state_->isValid() : false;
    }
    
private:
    FormState* state_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Arabic Aliases — أسماء عربية
// ═══════════════════════════════════════════════════════════════════════════════

using نموذج = Form;
using حالة_نموذج = FormState;
using مفتاح_نموذج = FormKey;
using حقل_نص_نموذج = TextFormField;
using حقل_اختيار_نموذج = CheckboxFormField;

// مُدققات عربية
namespace مدققات {
    inline auto مطلوب(const std::string& رسالة = "هذا الحقل مطلوب") {
        return Validators::required(رسالة);
    }
    
    inline auto حد_ادنى_طول(size_t طول, const std::string& رسالة = "") {
        return Validators::minLength(طول, رسالة);
    }
    
    inline auto حد_اقصى_طول(size_t طول, const std::string& رسالة = "") {
        return Validators::maxLength(طول, رسالة);
    }
    
    inline auto بريد_الكتروني(const std::string& رسالة = "بريد إلكتروني غير صحيح") {
        return Validators::email(رسالة);
    }
    
    inline auto هاتف(const std::string& رسالة = "رقم هاتف غير صحيح") {
        return Validators::phone(رسالة);
    }
    
    inline auto رقمي(const std::string& رسالة = "يجب إدخال أرقام فقط") {
        return Validators::numeric(رسالة);
    }
    
    inline auto نطاق(double حد_ادنى, double حد_اقصى, const std::string& رسالة = "") {
        return Validators::range(حد_ادنى, حد_اقصى, رسالة);
    }
    
    template<typename... V>
    inline auto دمج(V... مدققات) {
        return Validators::compose({مدققات...});
    }
}

// دوال مساعدة
inline WidgetPtr حقل_نص(
    const std::string& تسمية,
    FormFieldValidator<std::string> مدقق = nullptr,
    std::function<void(const std::string&)> عند_التغيير = nullptr,
    bool كلمة_سر = false
) {
    InputDecoration زخرفة;
    زخرفة.labelText = تسمية;
    
    return std::make_shared<TextFormField>(
        "",
        std::move(مدقق),
        std::move(عند_التغيير),
        nullptr,
        nullptr,
        زخرفة,
        TextInputType::Text,
        كلمة_سر
    );
}

inline WidgetPtr حقل_بريد(
    const std::string& تسمية = "البريد الإلكتروني",
    bool اختياري = false
) {
    InputDecoration زخرفة;
    زخرفة.labelText = تسمية;
    
    auto مدقق = اختياري 
        ? Validators::email()
        : Validators::compose({Validators::required(), Validators::email()});
    
    return std::make_shared<TextFormField>(
        "",
        مدقق,
        nullptr,
        nullptr,
        nullptr,
        زخرفة,
        TextInputType::EmailAddress
    );
}

inline WidgetPtr حقل_كلمة_سر(
    const std::string& تسمية = "كلمة المرور",
    size_t حد_ادنى = 6
) {
    InputDecoration زخرفة;
    زخرفة.labelText = تسمية;
    
    return std::make_shared<TextFormField>(
        "",
        Validators::compose({
            Validators::required("كلمة المرور مطلوبة"),
            Validators::minLength(حد_ادنى)
        }),
        nullptr,
        nullptr,
        nullptr,
        زخرفة,
        TextInputType::VisiblePassword,
        true // obscureText
    );
}

inline WidgetPtr حقل_هاتف(
    const std::string& تسمية = "رقم الهاتف"
) {
    InputDecoration زخرفة;
    زخرفة.labelText = تسمية;
    
    return std::make_shared<TextFormField>(
        "",
        Validators::compose({
            Validators::required(),
            Validators::phone()
        }),
        nullptr,
        nullptr,
        nullptr,
        زخرفة,
        TextInputType::Phone
    );
}

} // namespace flutter
} // namespace sad
