// بسم الله الرحمن الرحيم
/**
 * @file lib.s
 * @brief {{PROJECT_NAME}} - Reusable Library
 * @author {{AUTHOR_NAME}}
 * @date {{DATE}}
 */

/**
 * Example function - Add two numbers
 * @param a First number
 * @param b Second number
 * @return Sum of a and b
 */
صدّر دالة جمع(أ: عدد، ب: عدد) -> عدد {
    أرجع أ + ب;
}

/**
 * Example function - Subtract two numbers
 * @param a First number
 * @param b Second number
 * @return Difference of a and b
 */
صدّر دالة طرح(أ: عدد، ب: عدد) -> عدد {
    أرجع أ - ب;
}

/**
 * Example function - Multiply two numbers
 * @param a First number
 * @param b Second number
 * @return Product of a and b
 */
صدّر دالة ضرب(أ: عدد، ب: عدد) -> عدد {
    أرجع أ * ب;
}

/**
 * Example function - Divide two numbers
 * @param a First number
 * @param b Second number
 * @return Quotient of a and b
 */
صدّر دالة قسمة(أ: عدد، ب: عدد) -> عدد {
    إذا (ب == 0) {
        ارمِ خطأ("القسمة على صفر غير مسموحة");
    }
    أرجع أ / ب;
}

/**
 * Example class - Calculator
 */
صدّر صنف آلة_حاسبة {
    نتيجة: عدد;
    
    دالة __إنشاء__() {
        هذا.نتيجة = 0;
    }
    
    دالة جمع(قيمة: عدد) {
        هذا.نتيجة += قيمة;
        أرجع هذا;
    }
    
    دالة طرح(قيمة: عدد) {
        هذا.نتيجة -= قيمة;
        أرجع هذا;
    }
    
    دالة احصل_النتيجة() -> عدد {
        أرجع هذا.نتيجة;
    }
    
    دالة إعادة_تعيين() {
        هذا.نتيجة = 0;
    }
}
