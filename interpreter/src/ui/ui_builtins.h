/**
 * @file ui_builtins.h
 * @brief (AR) تسجيل دوال واجهات المستخدم المضمّنة — جزء من مكتبة الرسوميات
 * @brief (EN) UI builtin function registration — part of the graphics library
 *
 * جميع دوال الرسوميات والواجهات تُسجَّل من هنا وليس من المفسر مباشرة.
 * All graphics/UI functions are registered here, not from the interpreter core.
 */

#ifndef SAD_UI_BUILTINS_H
#define SAD_UI_BUILTINS_H

namespace Sad {
namespace Interpreter {

class Interpreter;

/**
 * @brief (AR) تسجيل جميع دوال واجهات المستخدم المضمّنة
 * @brief (EN) Register all UI builtin functions
 *
 * يُسجِّل: _محرك_واجهات، انتقل، عودة، عودة_للبداية، استبدل،
 *          تبديل_الثيم، وضع_داكن، وضع_فاتح، هل_داكن
 */
void registerUIBuiltins(Interpreter& interpreter);

} // namespace Interpreter
} // namespace Sad

#endif // SAD_UI_BUILTINS_H
