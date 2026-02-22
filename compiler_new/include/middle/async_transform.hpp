#ifndef SAD_ASYNC_TRANSFORM_HPP
#define SAD_ASYNC_TRANSFORM_HPP

/**
 * @file async_transform.hpp
 * @brief (AR) تصريحات تحويل async إلى آلات حالة
 *        (EN) Async transformation declarations
 * 
 * (AR) يعتمد على ast_fwd.hpp الذي يوفّر جميع أنواع AST المطلوبة
 */

#include "ast_fwd.hpp"

namespace Sad {
namespace Optimizer {

/**
 * (AR) صنف تحويل الدوال غير المتزامنة إلى آلات حالة
 *      يعمل كجزء من مرحلة التحسين الوسطى (middle-end optimization)
 * (EN) Async Transform - transforms async functions to state machines
 */
class AsyncTransform {
public:
    AsyncTransform() = default;
    ~AsyncTransform() = default;
};

} // namespace Optimizer
} // namespace Sad

#endif // SAD_ASYNC_TRANSFORM_HPP
