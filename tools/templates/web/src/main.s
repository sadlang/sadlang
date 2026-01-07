// بسم الله الرحمن الرحيم
/**
 * @file main.s
 * @brief {{PROJECT_NAME}} - Web Application
 * @author {{AUTHOR_NAME}}
 * @date {{DATE}}
 */

استورد "http_server" كـ HTTP;
استورد "router" كـ موجه;
استورد "json" كـ JSON;
استورد "logger" كـ مسجل;

/**
 * Home page handler
 */
دالة معالج_الرئيسية(طلب، استجابة) {
    استجابة.أرسل_HTML("""
    <!DOCTYPE html>
    <html dir="rtl" lang="ar">
    <head>
        <meta charset="UTF-8">
        <title>{{PROJECT_NAME}}</title>
        <style>
            body {
                font-family: Arial, sans-serif;
                max-width: 800px;
                margin: 50px auto;
                padding: 20px;
                text-align: center;
            }
            h1 { color: #333; }
            p { color: #666; }
        </style>
    </head>
    <body>
        <h1>مرحباً بك في {{PROJECT_NAME}}! 🎉</h1>
        <p>تطبيق ويب مبني بلغة ص</p>
        <p>استخدم /api/data للحصول على بيانات JSON</p>
    </body>
    </html>
    """);
}

/**
 * API endpoint
 */
دالة معالج_API(طلب، استجابة) {
    بيانات = {
        "message": "مرحباً من {{PROJECT_NAME}}!",
        "version": "0.1.0",
        "timestamp": الوقت_الحالي()
    };
    
    استجابة.أرسل_JSON(بيانات);
}

/**
 * Health check endpoint
 */
دالة معالج_الصحة(طلب، استجابة) {
    استجابة.أرسل_JSON({
        "status": "healthy",
        "uptime": وقت_التشغيل()
    });
}

/**
 * Main entry point
 */
دالة رئيسية(وسائط: [نص]) {
    // Setup logger
    مسجل.إعداد({
        "level": "info",
        "format": "json"
    });
    
    // Create server
    خادم = HTTP.إنشاء();
    
    // Setup router
    مُوجه = موجه.جديد();
    مُوجه.احصل("/", معالج_الرئيسية);
    مُوجه.احصل("/api/data", معالج_API);
    مُوجه.احصل("/health", معالج_الصحة);
    
    // Parse port from arguments
    منفذ = 3000;
    لكل وسيط في وسائط {
        إذا (وسيط.يبدأ_بـ("--port=")) {
            منفذ = عدد(وسيط.قسم("=")[1]);
        }
    }
    
    // Start server
    مسجل.معلومات("بدء الخادم على المنفذ " + نص(منفذ));
    خادم.استمع(منفذ, مُوجه);
    
    أرجع 0;
}
