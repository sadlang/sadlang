# بسم الله الرحمن الرحيم
# مثال على التوثيق في لغة Sad
# Example of documentation in Sad language

###
# @brief حساب مجموع عددين / Calculate sum of two numbers
# @brief_ar حساب مجموع عددين
# @description This function takes two numbers and returns their sum.
#              It demonstrates basic arithmetic operations in Sad.
# @description_ar هذه الدالة تأخذ عددين وترجع مجموعهما.
#                 توضح العمليات الحسابية الأساسية في Sad.
# @param أ العدد الأول / First number
# @param ب العدد الثاني / Second number
# @return مجموع العددين / Sum of the two numbers
# @example
#   رقم نتيجة = جمع(٥، ٣)
#   اطبع(نتيجة)  # سيطبع: ٨
# @since 1.0.0
# @author Sad Team
###
دالة جمع(رقم أ، رقم ب) {
    إرجاع أ + ب
}

###
# @brief طرح عددين / Subtract two numbers
# @brief_ar طرح عددين
# @param أ العدد الأول / First number
# @param ب العدد الثاني / Second number
# @return الفرق بين العددين / Difference between numbers
###
دالة طرح(رقم أ، رقم ب) {
    إرجاع أ - ب
}

###
# @brief ضرب عددين / Multiply two numbers
# @brief_ar ضرب عددين
# @param أ العدد الأول
# @param ب العدد الثاني
# @return حاصل الضرب / Product
# @example
#   رقم حاصل = ضرب(٤، ٥)
#   اطبع(حاصل)  # سيطبع: ٢٠
###
دالة ضرب(رقم أ، رقم ب) {
    إرجاع أ * ب
}

###
# @brief قسمة عددين / Divide two numbers
# @brief_ar قسمة عددين مع التحقق من القسمة على صفر
# @description Divides two numbers with zero-check protection.
# @param أ البسط / Numerator
# @param ب المقام / Denominator
# @return نتيجة القسمة أو صفر في حالة القسمة على صفر
# @example
#   رقم ناتج = قسمة(١٠، ٢)
#   اطبع(ناتج)  # سيطبع: ٥
###
دالة قسمة(رقم أ، رقم ب) {
    إذا (ب == ٠) {
        اطبع("خطأ: لا يمكن القسمة على صفر")
        إرجاع ٠
    }
    إرجاع أ / ب
}

###
# @brief صنف لتمثيل نقطة في مستوى ثنائي الأبعاد
# @brief_ar صنف النقطة / Point class
# @description Represents a point in 2D space with x and y coordinates.
# @since 1.0.0
###
صنف نقطة {
    رقم س
    رقم ص
    
    ###
    # @brief إنشاء نقطة جديدة / Create new point
    # @param س الإحداثي الأفقي / X coordinate
    # @param ص الإحداثي العمودي / Y coordinate
    ###
    دالة بناء(رقم س، رقم ص) {
        هذا.س = س
        هذا.ص = ص
    }
    
    ###
    # @brief حساب المسافة من نقطة أخرى / Calculate distance from another point
    # @param نقطة_أخرى النقطة الأخرى / Other point
    # @return المسافة الإقليدية / Euclidean distance
    # @example
    #   نقطة ن١ = جديد نقطة(٠، ٠)
    #   نقطة ن٢ = جديد نقطة(٣، ٤)
    #   رقم مسافة = ن١.احسب_مسافة(ن٢)
    #   اطبع(مسافة)  # سيطبع: ٥
    ###
    دالة احسب_مسافة(نقطة نقطة_أخرى) {
        رقم فرق_س = هذا.س - نقطة_أخرى.س
        رقم فرق_ص = هذا.ص - نقطة_أخرى.ص
        إرجاع جذر(فرق_س * فرق_س + فرق_ص * فرق_ص)
    }
}

###
# @brief ثابت باي / Pi constant
# @brief_ar ثابت رياضي يمثل النسبة بين محيط الدائرة وقطرها
# @description Mathematical constant pi (π) ≈ 3.14159
# @since 1.0.0
###
ثابت رقم باي = ٣.١٤١٥٩٢٦٥٣٥٨٩٧٩٣

###
# @brief حساب مساحة دائرة / Calculate circle area
# @brief_ar حساب مساحة الدائرة من نصف القطر
# @param نصف_القطر نصف قطر الدائرة / Circle radius
# @return المساحة / Area
# @example
#   رقم مساحة = مساحة_دائرة(٥)
#   اطبع(مساحة)  # سيطبع: ٧٨.٥٣٩٨...
# @see محيط_دائرة
###
دالة مساحة_دائرة(رقم نصف_القطر) {
    إرجاع باي * نصف_القطر * نصف_القطر
}

###
# @brief حساب محيط دائرة / Calculate circle circumference
# @brief_ar حساب محيط الدائرة من نصف القطر
# @param نصف_القطر نصف قطر الدائرة / Circle radius
# @return المحيط / Circumference
# @see مساحة_دائرة
###
دالة محيط_دائرة(رقم نصف_القطر) {
    إرجاع ٢ * باي * نصف_القطر
}

# الحمد لله - مثال كامل على التوثيق!
# Complete documentation example!
