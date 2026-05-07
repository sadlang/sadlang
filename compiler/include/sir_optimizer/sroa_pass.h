/**
 * @file sroa_pass.h
 * @brief (AR) تمرير الاستبدال القيمي للتجميعات (SROA)
 * @brief (EN) Scalar Replacement of Aggregates Pass
 *
 * @details
 * (AR) يكتشف المتغيرات المُخصصة (ALLOC) التي تُستخدم فقط عبر
 *      OBJECT_GET/OBJECT_SET بحقول ثابتة معروفة، ويستبدلها بسجلات
 *      فردية لكل حقل. هذا يُزيل الوصول غير المباشر ويسمح للممرات
 *      اللاحقة (مثل CopyPropagation وDCE) بتحسين الكود أكثر.
 *
 * (EN) Detects ALLOC'd variables accessed only via OBJECT_GET/OBJECT_SET
 *      with known constant field names, and replaces them with individual
 *      registers per field. Eliminates indirect access and enables further
 *      optimization by downstream passes.
 *
 * @author SadLanguage Compiler Team
 * @date January 2026
 */

#pragma once

#include "sir_optimizer/pass.h"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Sad
{
    namespace Compiler
    {
        namespace Optimizer
        {

            /**
             * @brief (AR) ممرّ الاستبدال القيمي للتجميعات
             * @brief (EN) Scalar Replacement of Aggregates Pass
             */
            class SROAPass : public OptimizationPass
            {
            public:
                SROAPass();
                ~SROAPass() override;

                /**
                 * @brief (AR) تنفيذ التحسين على الدالة
                 * @brief (EN) Run optimization on a function
                 * @param function الدالة المراد تحسينها
                 * @return true إذا تم تعديل الدالة
                 */
                bool runOnFunction(SIR::SIRFunction *function) override;

            private:
                /**
                 * @brief (AR) معلومات حول متغير مُخصص قابل للتحليل
                 * @brief (EN) Info about an allocatable variable eligible for SROA
                 */
                struct AllocInfo
                {
                    std::string allocReg;                   ///< اسم السجل المُخصص
                    std::unordered_set<std::string> fields; ///< الحقول المُستخدمة
                    bool eligible;                          ///< هل يصلح للاستبدال
                };

                /**
                 * @brief (AR) تحليل الدالة لإيجاد التخصيصات القابلة للاستبدال
                 * @brief (EN) Analyze function to find SROA-eligible allocations
                 * @param function الدالة المراد تحليلها
                 * @return خريطة اسم_السجل → معلومات التخصيص
                 */
                std::unordered_map<std::string, AllocInfo>
                analyzeAllocations(SIR::SIRFunction *function);

                /**
                 * @brief (AR) تنفيذ الاستبدال الفعلي
                 * @brief (EN) Perform the actual replacement
                 * @param function الدالة
                 * @param allocInfo معلومات التخصيص المؤهلة
                 * @return true إذا تم تعديل
                 */
                bool replaceAggregates(SIR::SIRFunction *function,
                                       const std::unordered_map<std::string, AllocInfo> &allocInfo);

                /**
                 * @brief (AR) إنشاء اسم سجل فريد لحقل
                 * @brief (EN) Create unique register name for a field
                 * @param allocReg اسم السجل المُخصص الأصلي
                 * @param fieldName اسم الحقل
                 * @return اسم السجل الجديد
                 */
                std::string makeFieldRegName(const std::string &allocReg,
                                             const std::string &fieldName) const;

                /**
                 * @brief (AR) عدد الاستبدالات التي تمت
                 */
                int replacedCount_;
            };

        } // namespace Optimizer
    } // namespace Compiler
} // namespace Sad
