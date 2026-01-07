// بسم الله الرحمن الرحيم
// مثال خادم REST API كامل
// Complete REST API Server Example

استيراد "http"
استيراد "json"

// قاعدة بيانات مؤقتة
متغير users = []
متغير next_id = 1

// دوال مساعدة
دالة find_user(id) {
    لكل user في users {
        إذا user["id"] == id {
            إرجاع user
        }
    }
    إرجاع لاشيء
}

دالة remove_user(id) {
    index = 0
    لكل user في users {
        إذا user["id"] == id {
            users = array_remove(users, index)
            إرجاع صحيح
        }
        index = index + 1
    }
    إرجاع خطأ
}

// معالجات المسارات
دالة get_all_users(طلب, رد) {
    رد_json(رد, {
        "عدد": array_length(users),
        "بيانات": users
    })
}

دالة get_user(طلب, رد) {
    id = int(path_param(طلب, "id"))
    user = find_user(id)
    
    إذا user != لاشيء {
        رد_json(رد, user)
    } وإلا {
        رد_json(رد, {"خطأ": "المستخدم غير موجود"}, 404)
    }
}

دالة create_user(طلب, رد) {
    user_data = طلب_body_json(طلب)
    
    // التحقق من البيانات
    إذا !has_key(user_data, "اسم") أو !has_key(user_data, "بريد") {
        رد_json(رد, {"خطأ": "الاسم والبريد مطلوبان"}, 400)
        إرجاع
    }
    
    // إنشاء مستخدم جديد
    user = {
        "id": next_id,
        "اسم": user_data["اسم"],
        "بريد": user_data["بريد"],
        "عمر": has_key(user_data, "عمر") ? user_data["عمر"] : 0
    }
    
    users = array_append(users, user)
    next_id = next_id + 1
    
    رد_json(رد, user, 201)
}

دالة update_user(طلب, رد) {
    id = int(path_param(طلب, "id"))
    user_data = طلب_body_json(طلب)
    user = find_user(id)
    
    إذا user == لاشيء {
        رد_json(رد, {"خطأ": "المستخدم غير موجود"}, 404)
        إرجاع
    }
    
    // تحديث البيانات
    إذا has_key(user_data, "اسم") {
        user["اسم"] = user_data["اسم"]
    }
    إذا has_key(user_data, "بريد") {
        user["بريد"] = user_data["بريد"]
    }
    إذا has_key(user_data, "عمر") {
        user["عمر"] = user_data["عمر"]
    }
    
    رد_json(رد, user)
}

دالة delete_user(طلب, رد) {
    id = int(path_param(طلب, "id"))
    
    إذا remove_user(id) {
        رد_json(رد, {"رسالة": "تم حذف المستخدم بنجاح"})
    } وإلا {
        رد_json(رد, {"خطأ": "المستخدم غير موجود"}, 404)
    }
}

// الدالة الرئيسية
دالة رئيسية() {
    خادم = خادم_http_جديد(3000)
    
    // تفعيل CORS
    خادم_http_enable_cors(خادم, "*")
    
    // Routes
    خادم_http_get(خادم, "/api/users", get_all_users)
    خادم_http_get(خادم, "/api/users/:id", get_user)
    خادم_http_post(خادم, "/api/users", create_user)
    خادم_http_put(خادم, "/api/users/:id", update_user)
    خادم_http_delete(خادم, "/api/users/:id", delete_user)
    
    // Middleware للتسجيل
    خادم_http_use(خادم, دالة(طلب, رد, next) {
        method = طلب_method(طلب)
        path = طلب_path(طلب)
        اطبع("📥 " + method + " " + path)
        next()
    })
    
    // تشغيل الخادم
    اطبع("═══════════════════════════════════")
    اطبع("🚀 REST API Server Running")
    اطبع("📍 http://localhost:3000")
    اطبع("📚 API Endpoints:")
    اطبع("   GET    /api/users       - Get all users")
    اطبع("   GET    /api/users/:id   - Get user by ID")
    اطبع("   POST   /api/users       - Create user")
    اطبع("   PUT    /api/users/:id   - Update user")
    اطبع("   DELETE /api/users/:id   - Delete user")
    اطبع("═══════════════════════════════════")
    
    خادم_http_listen(خادم)
}
