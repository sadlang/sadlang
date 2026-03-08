// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  sad_flutter_forms.cpp — تطبيق النماذج والتحقق
//  Forms Implementation
// ═══════════════════════════════════════════════════════════════════════════════

#include "sad_flutter_forms.h"
#include "sad_flutter_layout.h"

namespace sad {
namespace flutter {

// ═══════════════════════════════════════════════════════════════════════════════
//  Form Widget State
// ═══════════════════════════════════════════════════════════════════════════════

class FormWidgetState : public State<Form> {
public:
    void initState() override {
        formState_ = std::make_unique<FormState>();
    }
    
    FormState* formState() { return formState_.get(); }
    
    WidgetPtr build(BuildContext* ctx) override {
        return widget()->child();
    }
    
private:
    std::unique_ptr<FormState> formState_;
    
    Form* widget() const { return static_cast<Form*>(widget_); }
};

std::unique_ptr<State_Base> Form::createState() {
    return std::make_unique<FormWidgetState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  TextFormField State
// ═══════════════════════════════════════════════════════════════════════════════

class TextFormFieldState : public State<TextFormField> {
public:
    void initState() override {
        auto* w = widget();
        
        // إنشاء أو استخدام Controller
        if (w->controller_) {
            controller_ = w->controller_;
        } else {
            controller_ = std::make_shared<TextEditingController>(w->initialValue_);
        }
        
        fieldState_.setValue(controller_->text());
    }
    
    void didUpdateWidget(TextFormField* oldWidget) override {
        // تحديث الـ Controller إذا تغير
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        auto* w = widget();
        
        // تحديث الزخرفة مع رسالة الخطأ
        InputDecoration decoration = w->decoration_;
        if (fieldState_.hasError()) {
            decoration.errorText = fieldState_.errorText();
        }
        
        return std::make_shared<TextField>(
            controller_,
            [this](const std::string& value) {
                fieldState_.setValue(value);
                
                // تحقق إذا كان مفعّلاً
                if (widget()->validator_) {
                    fieldState_.validate(widget()->validator_);
                    setState([]{});
                }
                
                if (widget()->onChanged_) {
                    widget()->onChanged_(value);
                }
            },
            decoration,
            w->keyboardType_,
            w->obscureText_,
            w->maxLines_,
            w->maxLength_,
            w->enabled_,
            w->readOnly_,
            w->autofocus_
        );
    }
    
    /// التحقق من الحقل
    bool validate() {
        auto* w = widget();
        if (w->validator_) {
            bool valid = fieldState_.validate(w->validator_);
            setState([]{});
            return valid;
        }
        return true;
    }
    
    /// حفظ القيمة
    void save() {
        auto* w = widget();
        if (w->onSaved_) {
            w->onSaved_(fieldState_.value());
        }
    }
    
    /// إعادة تعيين الحقل
    void reset() {
        fieldState_.reset(widget()->initialValue_);
        controller_->setText(widget()->initialValue_);
        setState([]{});
    }
    
    const std::string& value() const { return fieldState_.value(); }
    
private:
    TextFormField* widget() const { 
        return static_cast<TextFormField*>(widget_); 
    }
    
    std::shared_ptr<TextEditingController> controller_;
    FormFieldState<std::string> fieldState_;
};

std::unique_ptr<State_Base> TextFormField::createState() {
    return std::make_unique<TextFormFieldState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  CheckboxFormField State
// ═══════════════════════════════════════════════════════════════════════════════

class CheckboxFormFieldState : public State<CheckboxFormField> {
public:
    void initState() override {
        fieldState_.setValue(widget()->initialValue_);
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        auto* w = widget();
        
        std::vector<WidgetPtr> rowChildren;
        
        // الـ Checkbox
        rowChildren.push_back(std::make_shared<Checkbox>(
            fieldState_.value(),
            [this](bool value) {
                fieldState_.setValue(value);
                
                if (widget()->validator_) {
                    fieldState_.validate(widget()->validator_);
                }
                
                if (widget()->onChanged_) {
                    widget()->onChanged_(value);
                }
                
                setState([]{});
            },
            w->enabled_
        ));
        
        // العنوان (إن وجد)
        if (w->title_) {
            rowChildren.push_back(std::make_shared<Expanded>(w->title_));
        }
        
        // بناء الصف
        WidgetPtr checkboxRow = std::make_shared<Row>(
            std::move(rowChildren),
            MainAxisAlignment::Start,
            MainAxisSize::Min
        );
        
        // إضافة رسالة الخطأ
        if (fieldState_.hasError()) {
            std::vector<WidgetPtr> columnChildren;
            columnChildren.push_back(checkboxRow);
            columnChildren.push_back(
                std::make_shared<Text>(
                    *fieldState_.errorText(),
                    TextStyle{.color = Color(244, 67, 54), .fontSize = 12}
                )
            );
            return std::make_shared<Column>(
                std::move(columnChildren),
                MainAxisAlignment::Start,
                MainAxisSize::Min,
                CrossAxisAlignment::Start
            );
        }
        
        return checkboxRow;
    }
    
    bool validate() {
        auto* w = widget();
        if (w->validator_) {
            bool valid = fieldState_.validate(w->validator_);
            setState([]{});
            return valid;
        }
        return true;
    }
    
    void reset() {
        fieldState_.reset(widget()->initialValue_);
        setState([]{});
    }
    
private:
    CheckboxFormField* widget() const {
        return static_cast<CheckboxFormField*>(widget_);
    }
    
    FormFieldState<bool> fieldState_;
};

std::unique_ptr<State_Base> CheckboxFormField::createState() {
    return std::make_unique<CheckboxFormFieldState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  DropdownButtonFormField State
// ═══════════════════════════════════════════════════════════════════════════════

template<typename T>
class DropdownButtonFormFieldState : public State<DropdownButtonFormField<T>> {
public:
    void initState() override {
        selectedValue_ = this->widget()->value_;
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        auto* w = static_cast<DropdownButtonFormField<T>*>(this->widget_);
        
        // بناء عناصر القائمة
        std::vector<DropdownMenuItem<T>> menuItems;
        for (const auto& [value, label] : w->items_) {
            menuItems.push_back({value, std::make_shared<Text>(label)});
        }
        
        WidgetPtr dropdown = std::make_shared<DropdownButton<T>>(
            menuItems,
            selectedValue_,
            [this, w](T value) {
                selectedValue_ = value;
                
                if (w->onChanged_) {
                    w->onChanged_(value);
                }
                
                this->setState([]{});
            }
        );
        
        // إضافة الزخرفة
        std::vector<WidgetPtr> columnChildren;
        
        // العنوان
        if (!w->decoration_.labelText.empty()) {
            columnChildren.push_back(
                std::make_shared<Text>(
                    w->decoration_.labelText,
                    TextStyle{.fontSize = 12, .color = Color(100, 100, 100)}
                )
            );
        }
        
        columnChildren.push_back(dropdown);
        
        // رسالة الخطأ
        if (errorText_.has_value()) {
            columnChildren.push_back(
                std::make_shared<Text>(
                    *errorText_,
                    TextStyle{.color = Color(244, 67, 54), .fontSize = 12}
                )
            );
        }
        
        return std::make_shared<Column>(
            std::move(columnChildren),
            MainAxisAlignment::Start,
            MainAxisSize::Min,
            CrossAxisAlignment::Start
        );
    }
    
    bool validate() {
        auto* w = static_cast<DropdownButtonFormField<T>*>(this->widget_);
        if (w->validator_) {
            errorText_ = w->validator_(selectedValue_);
            this->setState([]{});
            return !errorText_.has_value();
        }
        return true;
    }
    
private:
    std::optional<T> selectedValue_;
    std::optional<std::string> errorText_;
};

template<typename T>
std::unique_ptr<State_Base> DropdownButtonFormField<T>::createState() {
    return std::make_unique<DropdownButtonFormFieldState<T>>();
}

// Explicit instantiation for common types
template class DropdownButtonFormField<int>;
template class DropdownButtonFormField<std::string>;

// ═══════════════════════════════════════════════════════════════════════════════
//  Form Helper Functions
// ═══════════════════════════════════════════════════════════════════════════════

/// إنشاء نموذج تسجيل دخول جاهز
WidgetPtr createLoginForm(
    std::function<void(const std::string& email, const std::string& password)> onSubmit,
    const std::string& emailLabel = "البريد الإلكتروني",
    const std::string& passwordLabel = "كلمة المرور",
    const std::string& submitLabel = "تسجيل الدخول"
) {
    // سنُخزن القيم في closures
    auto emailController = std::make_shared<TextEditingController>();
    auto passwordController = std::make_shared<TextEditingController>();
    
    std::vector<WidgetPtr> formChildren;
    
    // حقل البريد
    InputDecoration emailDecoration;
    emailDecoration.labelText = emailLabel;
    emailDecoration.prefixIcon = std::make_shared<Icon>(IconData{"email", 0xE0BE});
    
    formChildren.push_back(
        std::make_shared<TextFormField>(
            "",
            Validators::compose({Validators::required(), Validators::email()}),
            nullptr,
            nullptr,
            nullptr,
            emailDecoration,
            TextInputType::EmailAddress,
            false, // obscureText
            1,     // maxLines
            -1,    // maxLength
            true,  // enabled
            false, // readOnly
            true,  // autofocus
            emailController
        )
    );
    
    formChildren.push_back(std::make_shared<SizedBox>(std::nullopt, 16.0f));
    
    // حقل كلمة المرور
    InputDecoration passwordDecoration;
    passwordDecoration.labelText = passwordLabel;
    passwordDecoration.prefixIcon = std::make_shared<Icon>(IconData{"lock", 0xE897});
    
    formChildren.push_back(
        std::make_shared<TextFormField>(
            "",
            Validators::compose({
                Validators::required("كلمة المرور مطلوبة"),
                Validators::minLength(6)
            }),
            nullptr,
            nullptr,
            nullptr,
            passwordDecoration,
            TextInputType::VisiblePassword,
            true, // obscureText
            1,
            -1,
            true,
            false,
            false,
            passwordController
        )
    );
    
    formChildren.push_back(std::make_shared<SizedBox>(std::nullopt, 24.0f));
    
    // زر الإرسال
    formChildren.push_back(
        std::make_shared<ElevatedButton>(
            std::make_shared<Text>(submitLabel),
            [onSubmit, emailController, passwordController]() {
                if (onSubmit) {
                    onSubmit(emailController->text(), passwordController->text());
                }
            }
        )
    );
    
    return std::make_shared<Form>(
        std::make_shared<Column>(
            std::move(formChildren),
            MainAxisAlignment::Start,
            MainAxisSize::Min,
            CrossAxisAlignment::Stretch
        )
    );
}

/// إنشاء نموذج تسجيل جديد
WidgetPtr createRegistrationForm(
    std::function<void(const std::string& name, const std::string& email, 
                      const std::string& password)> onSubmit,
    const std::string& nameLabel = "الاسم الكامل",
    const std::string& emailLabel = "البريد الإلكتروني",
    const std::string& passwordLabel = "كلمة المرور",
    const std::string& confirmPasswordLabel = "تأكيد كلمة المرور",
    const std::string& submitLabel = "إنشاء حساب"
) {
    auto nameController = std::make_shared<TextEditingController>();
    auto emailController = std::make_shared<TextEditingController>();
    auto passwordController = std::make_shared<TextEditingController>();
    auto confirmPasswordController = std::make_shared<TextEditingController>();
    
    std::vector<WidgetPtr> formChildren;
    
    // حقل الاسم
    InputDecoration nameDecoration;
    nameDecoration.labelText = nameLabel;
    nameDecoration.prefixIcon = std::make_shared<Icon>(IconData{"person", 0xE7FD});
    
    formChildren.push_back(
        std::make_shared<TextFormField>(
            "",
            Validators::compose({
                Validators::required("الاسم مطلوب"),
                Validators::minLength(3, "الاسم قصير جداً")
            }),
            nullptr, nullptr, nullptr,
            nameDecoration,
            TextInputType::Text,
            false, 1, -1, true, false, true,
            nameController
        )
    );
    
    formChildren.push_back(std::make_shared<SizedBox>(std::nullopt, 16.0f));
    
    // حقل البريد
    InputDecoration emailDecoration;
    emailDecoration.labelText = emailLabel;
    emailDecoration.prefixIcon = std::make_shared<Icon>(IconData{"email", 0xE0BE});
    
    formChildren.push_back(
        std::make_shared<TextFormField>(
            "",
            Validators::compose({Validators::required(), Validators::email()}),
            nullptr, nullptr, nullptr,
            emailDecoration,
            TextInputType::EmailAddress,
            false, 1, -1, true, false, false,
            emailController
        )
    );
    
    formChildren.push_back(std::make_shared<SizedBox>(std::nullopt, 16.0f));
    
    // حقل كلمة المرور
    InputDecoration passwordDecoration;
    passwordDecoration.labelText = passwordLabel;
    passwordDecoration.prefixIcon = std::make_shared<Icon>(IconData{"lock", 0xE897});
    
    formChildren.push_back(
        std::make_shared<TextFormField>(
            "",
            Validators::compose({
                Validators::required(),
                Validators::minLength(8, "يجب أن تكون كلمة المرور 8 أحرف على الأقل")
            }),
            nullptr, nullptr, nullptr,
            passwordDecoration,
            TextInputType::VisiblePassword,
            true, 1, -1, true, false, false,
            passwordController
        )
    );
    
    formChildren.push_back(std::make_shared<SizedBox>(std::nullopt, 16.0f));
    
    // حقل تأكيد كلمة المرور
    InputDecoration confirmDecoration;
    confirmDecoration.labelText = confirmPasswordLabel;
    confirmDecoration.prefixIcon = std::make_shared<Icon>(IconData{"lock_outline", 0xE899});
    
    // المُدقق يحتاج للرجوع لـ passwordController
    auto passwordPtr = std::make_shared<std::string>("");
    
    formChildren.push_back(
        std::make_shared<TextFormField>(
            "",
            [passwordController](const std::string& value) -> std::optional<std::string> {
                if (value.empty()) return "تأكيد كلمة المرور مطلوب";
                if (value != passwordController->text()) {
                    return "كلمتا المرور غير متطابقتين";
                }
                return std::nullopt;
            },
            nullptr, nullptr, nullptr,
            confirmDecoration,
            TextInputType::VisiblePassword,
            true, 1, -1, true, false, false,
            confirmPasswordController
        )
    );
    
    formChildren.push_back(std::make_shared<SizedBox>(std::nullopt, 24.0f));
    
    // زر الإرسال
    formChildren.push_back(
        std::make_shared<ElevatedButton>(
            std::make_shared<Text>(submitLabel),
            [onSubmit, nameController, emailController, passwordController]() {
                if (onSubmit) {
                    onSubmit(
                        nameController->text(),
                        emailController->text(),
                        passwordController->text()
                    );
                }
            }
        )
    );
    
    return std::make_shared<Form>(
        std::make_shared<Column>(
            std::move(formChildren),
            MainAxisAlignment::Start,
            MainAxisSize::Min,
            CrossAxisAlignment::Stretch
        )
    );
}

} // namespace flutter
} // namespace sad
