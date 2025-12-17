// بسم الله الرحمن الرحيم
/**
 * @file complete_example.s
 * @brief Complete JSON/XML Module Examples
 * مثال شامل لوحدة JSON/XML
 * 
 * الحمد لله رب العالمين
 */

// ============================================================================
// JSON Examples - أمثلة JSON
// ============================================================================

اطبع("=== JSON Module Examples / أمثلة وحدة JSON ===\n")

// 1. Parse JSON / تحليل JSON
اطبع("\n1. Parse JSON / تحليل JSON:\n")

متغير نص_json = "{\"الاسم\": \"أحمد\", \"العمر\": 25, \"المهارات\": [\"C++\", \"Python\"]}"
متغير بيانات = حلل_json(نص_json)

اطبع("الاسم: " + بيانات["الاسم"] + "\n")
اطبع("العمر: " + نوع_نص(بيانات["العمر"]) + "\n")

// 2. Create JSON Object / إنشاء كائن JSON
اطبع("\n2. Create JSON Object / إنشاء كائن JSON:\n")

متغير شخص = {}
شخص["الاسم"] = "علي"
شخص["العمر"] = 30
شخص["المدينة"] = "القاهرة"
شخص["نشط"] = صحيح

متغير json_نتيجة = حول_لـjson(شخص)
اطبع(json_نتيجة + "\n")

// 3. Pretty Print / طباعة منسقة
اطبع("\n3. Pretty Print / طباعة منسقة:\n")

متغير كائن_معقد = {
    "الشركة": "تقنية",
    "الموظفين": [
        {"الاسم": "أحمد", "القسم": "البرمجة"},
        {"الاسم": "فاطمة", "القسم": "التصميم"}
    ],
    "السنة": 2025
}

متغير json_منسق = json_منسق(كائن_معقد, 4)
اطبع(json_منسق)

// 4. JSON Validation / التحقق من JSON
اطبع("\n4. JSON Validation / التحقق من JSON:\n")

متغير json_صحيح = "{\"مفتاح\": \"قيمة\"}"
متغير json_خاطئ = "{خطأ}"

اطبع("صحيح؟ " + نوع_نص(تحقق_من_json(json_صحيح)) + "\n")
اطبع("خاطئ؟ " + نوع_نص(تحقق_من_json(json_خاطئ)) + "\n")

// 5. JSONPath Queries / استعلامات مسار JSON
اطبع("\n5. JSONPath Queries / استعلامات مسار JSON:\n")

متغير بيانات_كبيرة = حلل_json("{
    \"الجامعة\": {
        \"الاسم\": \"جامعة القاهرة\",
        \"الكليات\": [
            {\"الاسم\": \"الهندسة\", \"الطلاب\": 5000},
            {\"الاسم\": \"الطب\", \"الطلاب\": 3000}
        ]
    }
}")

متغير اسم_جامعة = احصل_على_json(بيانات_كبيرة, "الجامعة.الاسم")
اطبع("اسم الجامعة: " + اسم_جامعة + "\n")

// 6. Merge Objects / دمج الكائنات
اطبع("\n6. Merge Objects / دمج الكائنات:\n")

متغير كائن1 = {"أ": 1, "ب": 2}
متغير كائن2 = {"ب": 3, "ج": 4}
متغير مدموج = ادمج_json(كائن1, كائن2)

اطبع("مدموج: " + حول_لـjson(مدموج) + "\n")

// 7. Clone Object / نسخ الكائن
اطبع("\n7. Clone Object / نسخ الكائن:\n")

متغير أصلي = {"البيانات": [1, 2, 3]}
متغير نسخة = انسخ_json(أصلي)

نسخة["البيانات"][0] = 999
اطبع("أصلي: " + حول_لـjson(أصلي) + "\n")
اطبع("نسخة: " + حول_لـjson(نسخة) + "\n")

// 8. Get Keys and Values / الحصول على المفاتيح والقيم
اطبع("\n8. Get Keys and Values / الحصول على المفاتيح والقيم:\n")

متغير بيانات_مفاتيح = {"مفتاح1": "قيمة1", "مفتاح2": "قيمة2", "مفتاح3": "قيمة3"}
متغير المفاتيح = مفاتيح_json(بيانات_مفاتيح)

اطبع("المفاتيح: ")
لكل مفتاح في المفاتيح {
    اطبع(مفتاح + " ")
}
اطبع("\n")

// 9. Array Operations / عمليات المصفوفات
اطبع("\n9. Array Operations / عمليات المصفوفات:\n")

متغير مصفوفة_json = "[10, 20, 30, 40, 50]"
متغير مصفوفة = حلل_json(مصفوفة_json)

اطبع("الطول: " + نوع_نص(طول_json(مصفوفة)) + "\n")
اطبع("النوع: " + نوع_json(مصفوفة) + "\n")

// 10. Escape/Unescape / الهروب وإلغاء الهروب
اطبع("\n10. Escape/Unescape / الهروب وإلغاء الهروب:\n")

متغير نص_خاص = "سطر 1\nسطر 2\tمع تاب"
متغير مهروب = اهرب_json(نص_خاص)
اطبع("مهروب: " + مهروب + "\n")

// 11. Complex Example - API Response / مثال معقد - استجابة API
اطبع("\n11. Complex API Response Example / مثال استجابة API:\n")

متغير api_استجابة = {
    "الحالة": "نجاح",
    "الكود": 200,
    "البيانات": {
        "المستخدمون": [
            {
                "المعرف": 1,
                "الاسم": "أحمد محمد",
                "البريد": "ahmad@example.com",
                "نشط": صحيح,
                "التقييم": 4.5
            },
            {
                "المعرف": 2,
                "الاسم": "فاطمة علي",
                "البريد": "fatima@example.com",
                "نشط": صحيح,
                "التقييم": 4.8
            }
        ],
        "الصفحة": 1,
        "الإجمالي": 2
    }
}

اطبع("API Response:\n")
اطبع(json_منسق(api_استجابة, 2))

// 12. Configuration File / ملف الإعدادات
اطبع("\n12. Configuration File / ملف الإعدادات:\n")

متغير config = {
    "التطبيق": {
        "الاسم": "تطبيق رائع",
        "الإصدار": "1.0.0",
        "المنفذ": 8080
    },
    "قاعدة_البيانات": {
        "المضيف": "localhost",
        "المنفذ": 5432,
        "الاسم": "mydb"
    },
    "التسجيل": {
        "المستوى": "معلومات",
        "الملف": "/var/log/app.log"
    }
}

اطبع("Config JSON:\n")
اطبع(json_منسق(config))

// ============================================================================
// XML Examples - أمثلة XML
// ============================================================================

اطبع("\n\n=== XML Module Examples / أمثلة وحدة XML ===\n")

// 1. Parse XML / تحليل XML
اطبع("\n1. Parse XML / تحليل XML:\n")

متغير نص_xml = "<كتاب><العنوان>البرمجة بلغة C++</العنوان><المؤلف>بيارن ستراوستروب</المؤلف></كتاب>"
متغير كتاب = حلل_xml(نص_xml)

اطبع("العنوان: " + نص_xml(ابناء_xml(كتاب, "العنوان")[0]) + "\n")

// 2. Create XML Element / إنشاء عنصر XML
اطبع("\n2. Create XML Element / إنشاء عنصر XML:\n")

متغير عنصر = عنصر_xml("منتج", "", {"المعرف": "123", "متاح": "صحيح"})
متغير عنصر_ابن1 = عنصر_xml("الاسم", "كتاب برمجة")
متغير عنصر_ابن2 = عنصر_xml("السعر", "50")

متغير عنصر_كامل = اضف_ابن_xml(عنصر, عنصر_ابن1)
عنصر_كامل = اضف_ابن_xml(عنصر_كامل, عنصر_ابن2)

اطبع("XML Element:\n")
اطبع(حول_لـxml(عنصر_كامل) + "\n")

// 3. Pretty Print XML / طباعة XML منسقة
اطبع("\n3. Pretty Print XML / طباعة XML منسقة:\n")

متغير مستند_xml = {
    "name": "المكتبة",
    "children": [
        {
            "name": "كتاب",
            "attributes": {"المعرف": "1"},
            "children": [
                {"name": "العنوان", "text": "البرمجة المتقدمة"},
                {"name": "المؤلف", "text": "أحمد محمد"}
            ]
        },
        {
            "name": "كتاب",
            "attributes": {"المعرف": "2"},
            "children": [
                {"name": "العنوان", "text": "تعلم الذكاء الاصطناعي"},
                {"name": "المؤلف", "text": "فاطمة علي"}
            ]
        }
    ]
}

متغير xml_جميل = xml_منسق(مستند_xml, 2)
اطبع(xml_جميل)

// 4. XML Validation / التحقق من XML
اطبع("\n4. XML Validation / التحقق من XML:\n")

متغير xml_صحيح = "<جذر><عنصر>محتوى</عنصر></جذر>"
متغير xml_خاطئ = "<جذر><عنصر>غير مغلق"

اطبع("XML صحيح؟ " + نوع_نص(تحقق_من_xml(xml_صحيح)) + "\n")
اطبع("XML خاطئ؟ " + نوع_نص(تحقق_من_xml(xml_خاطئ)) + "\n")

// 5. Get/Set Attributes / الحصول على السمات وتعيينها
اطبع("\n5. Get/Set Attributes / الحصول على السمات:\n")

متغير عنصر_مع_سمات = عنصر_xml("div", "محتوى", {"id": "main", "class": "container"})
متغير معرف = احصل_على_سمة_xml(عنصر_مع_سمات, "id")

اطبع("المعرف: " + معرف + "\n")

متغير عنصر_محدث = عين_سمة_xml(عنصر_مع_سمات, "style", "color: blue")
اطبع("مع السمة الجديدة:\n" + حول_لـxml(عنصر_محدث) + "\n")

// 6. Navigate Children / التنقل بين الأبناء
اطبع("\n6. Navigate Children / التنقل بين الأبناء:\n")

متغير قائمة_xml = "<قائمة>
    <عنصر>أول</عنصر>
    <عنصر>ثاني</عنصر>
    <عنصر>ثالث</عنصر>
</قائمة>"

متغير قائمة = حلل_xml(قائمة_xml)
متغير العناصر = ابناء_xml(قائمة, "عنصر")

اطبع("عدد العناصر: " + نوع_نص(طول_json(العناصر)) + "\n")

متغير أول_عنصر = اول_ابن_xml(قائمة, "عنصر")
اطبع("أول عنصر: " + نص_xml(أول_عنصر) + "\n")

// 7. Escape/Unescape XML / الهروب في XML
اطبع("\n7. Escape/Unescape XML:\n")

متغير نص_مع_وسوم = "محتوى مع <وسوم> & \"علامات\""
متغير مهروب_xml = اهرب_xml(نص_مع_وسوم)

اطبع("XML مهروب: " + مهروب_xml + "\n")

// 8. RSS Feed Example / مثال RSS
اطبع("\n8. RSS Feed Example / مثال RSS:\n")

متغير rss = {
    "name": "rss",
    "attributes": {"version": "2.0"},
    "children": [
        {
            "name": "channel",
            "children": [
                {"name": "title", "text": "أخبار التقنية"},
                {"name": "link", "text": "https://tech-news.example.com"},
                {"name": "description", "text": "آخر أخبار التقنية"},
                {
                    "name": "item",
                    "children": [
                        {"name": "title", "text": "إطلاق تقنية جديدة"},
                        {"name": "link", "text": "https://tech-news.example.com/1"},
                        {"name": "pubDate", "text": "2025-12-04"}
                    ]
                }
            ]
        }
    ]
}

اطبع("RSS Feed:\n")
اطبع(xml_منسق(rss, 2))

// 9. SVG Example / مثال SVG
اطبع("\n9. SVG Example / مثال SVG:\n")

متغير svg = {
    "name": "svg",
    "attributes": {
        "width": "100",
        "height": "100",
        "xmlns": "http://www.w3.org/2000/svg"
    },
    "children": [
        {
            "name": "circle",
            "attributes": {
                "cx": "50",
                "cy": "50",
                "r": "40",
                "fill": "blue"
            }
        },
        {
            "name": "text",
            "attributes": {
                "x": "50",
                "y": "55",
                "text-anchor": "middle"
            },
            "text": "SVG"
        }
    ]
}

اطبع("SVG:\n")
اطبع(xml_منسق(svg))

// 10. HTML Example / مثال HTML
اطبع("\n10. HTML Document Example / مثال مستند HTML:\n")

متغير html = {
    "name": "html",
    "children": [
        {
            "name": "head",
            "children": [
                {"name": "title", "text": "صفحة تجريبية"},
                {"name": "meta", "attributes": {"charset": "UTF-8"}}
            ]
        },
        {
            "name": "body",
            "children": [
                {
                    "name": "h1",
                    "attributes": {"id": "title"},
                    "text": "مرحبا بالعالم"
                },
                {
                    "name": "p",
                    "attributes": {"class": "intro"},
                    "text": "هذه صفحة HTML مولدة بلغة الحزينة"
                },
                {
                    "name": "ul",
                    "children": [
                        {"name": "li", "text": "عنصر 1"},
                        {"name": "li", "text": "عنصر 2"},
                        {"name": "li", "text": "عنصر 3"}
                    ]
                }
            ]
        }
    ]
}

اطبع("HTML Document:\n")
اطبع(xml_منسق(html, 2))

// ============================================================================
// Practical Examples - أمثلة عملية
// ============================================================================

اطبع("\n\n=== Practical Examples / أمثلة عملية ===\n")

// 1. Convert JSON to XML (Simple) / تحويل JSON إلى XML
اطبع("\n1. JSON to XML Conversion / تحويل JSON إلى XML:\n")

دالة json_to_xml_simple(json_obj, tag_name) {
    إرجاع عنصر_xml(tag_name, حول_لـjson(json_obj))
}

متغير json_data = {"message": "Hello World"}
متغير xml_من_json = json_to_xml_simple(json_data, "data")
اطبع(حول_لـxml(xml_من_json) + "\n")

// 2. Data Export Function / دالة تصدير البيانات
اطبع("\n2. Data Export Function / دالة تصدير البيانات:\n")

دالة export_data(data, format) {
    إذا format == "json" {
        إرجاع json_منسق(data)
    } وإلا_إذا format == "xml" {
        متغير xml_elem = عنصر_xml("data")
        لكل key في مفاتيح_json(data) {
            متغير child = عنصر_xml(key, نوع_نص(data[key]))
            xml_elem = اضف_ابن_xml(xml_elem, child)
        }
        إرجاع حول_لـxml(xml_elem)
    }
    إرجاع "Format not supported"
}

متغير بيانات_تصدير = {
    "الاسم": "أحمد",
    "العمر": 25,
    "المدينة": "القاهرة"
}

اطبع("JSON Export:\n")
اطبع(export_data(بيانات_تصدير, "json"))

اطبع("\nXML Export:\n")
اطبع(export_data(بيانات_تصدير, "xml"))

// 3. Configuration Manager / مدير الإعدادات
اطبع("\n3. Configuration Manager / مدير الإعدادات:\n")

دالة load_config(config_json) {
    متغير config = حلل_json(config_json)
    اطبع("✓ تم تحميل الإعدادات\n")
    إرجاع config
}

دالة get_setting(config, path, default_value) {
    متغير value = احصل_على_json(config, path, default_value)
    إرجاع value
}

متغير app_config = load_config("{
    \"database\": {
        \"host\": \"localhost\",
        \"port\": 5432
    },
    \"app\": {
        \"debug\": true
    }
}")

متغير db_host = get_setting(app_config, "database.host", "127.0.0.1")
متغير debug_mode = get_setting(app_config, "app.debug", خطأ)

اطبع("DB Host: " + db_host + "\n")
اطبع("Debug: " + نوع_نص(debug_mode) + "\n")

// ============================================================================
// Performance Test - اختبار الأداء
// ============================================================================

اطبع("\n=== Performance Test / اختبار الأداء ===\n")

// Large JSON
متغير large_array = []
لـ متغير i = 0؛ i < 100؛ i = i + 1 {
    large_array = large_array + [{"id": i, "value": "Item " + نوع_نص(i)}]
}

متغير large_json = حول_لـjson(large_array)
اطبع("✓ تم إنشاء JSON كبير (" + نوع_نص(طول_json(large_array)) + " عناصر)\n")

متغير parsed_back = حلل_json(large_json)
اطبع("✓ تم تحليل JSON كبير\n")

// ============================================================================
// Summary - الخلاصة
// ============================================================================

اطبع("\n=== Summary / الخلاصة ===\n")
اطبع("✓ JSON Module: Parse, Stringify, Pretty, Validate, Query\n")
اطبع("✓ XML Module: Parse, Create, Navigate, Transform\n")
اطبع("✓ Utility Functions: Escape, Clone, Merge, Compare\n")
اطبع("✓ Bilingual Support: Arabic & English\n")
اطبع("\nالحمد لله رب العالمين\n")
اطبع("All examples completed successfully! / تم إكمال جميع الأمثلة بنجاح!\n")

// الحمد لله رب العالمين - All praise is due to Allah, Lord of the worlds
