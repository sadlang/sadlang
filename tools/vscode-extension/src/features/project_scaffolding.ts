/**
 * @file project_scaffolding.ts
 * @brief سقالات المشروع — Project Scaffolding
 * 
 * إنشاء مشاريع جديدة من قوالب مع بنية مجلدات وملفات جاهزة
 */

import * as vscode from 'vscode';
import * as path from 'path';
import * as fs from 'fs';

interface ProjectTemplate {
    id: string;
    name: string;
    description: string;
    icon: string;
    files: { [path: string]: string };
}

export function registerProjectScaffolding(context: vscode.ExtensionContext): void {
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.newProject', () => createNewProject()),
        vscode.commands.registerCommand('sad.addModule', () => addModule()),
        vscode.commands.registerCommand('sad.addTest', () => addTestFile()),
    );
}

const TEMPLATES: ProjectTemplate[] = [
    {
        id: 'basic',
        name: 'مشروع أساسي',
        description: 'مشروع بسيط بملف رئيسي',
        icon: '📄',
        files: {
            'رئيسي.ص': `# المشروع الرئيسي
# تم إنشاؤه بواسطة أداة سقالات لغة ص

دالة رئيسية()
    اطبع_سطر("مرحباً بالعالم! 🌍")
نهاية

رئيسية()
`,
            'اقرأني.md': `# {{name}}

## الوصف
مشروع بلغة ص

## التشغيل
\`\`\`
sad رئيسي.ص
\`\`\`
`,
        }
    },
    {
        id: 'oop',
        name: 'مشروع كائني',
        description: 'مشروع مع أصناف وبنية OOP',
        icon: '🏗️',
        files: {
            'رئيسي.ص': `# مشروع كائني التوجه
استورد "نماذج/شخص" من "نماذج"

دالة رئيسية()
    متغير شخص1 = جديد شخص("أحمد"، 25)
    شخص1.تحية()
نهاية

رئيسية()
`,
            'نماذج/شخص.ص': `# صنف الشخص
صدّر صنف شخص
    عام الاسم
    عام العمر

    باني(اسم، عمر)
        هذا.الاسم = اسم
        هذا.العمر = عمر
    نهاية

    عام دالة تحية()
        اطبع_سطر("مرحباً، أنا " + هذا.الاسم + " وعمري " + هذا.العمر)
    نهاية
نهاية
`,
            'اختبارات/اختبار_شخص.ص': `# اختبارات صنف الشخص
استورد "نماذج/شخص" من "نماذج"

اختبر "إنشاء شخص"
    متغير ش = جديد شخص("سارة"، 30)
    # تحقق من القيم
    اطبع_سطر("الاسم: " + ش.الاسم)
    اطبع_سطر("العمر: " + ش.العمر)
نهاية
`,
            'اقرأني.md': `# {{name}}

## البنية
\`\`\`
├── رئيسي.ص
├── نماذج/
│   └── شخص.ص
└── اختبارات/
    └── اختبار_شخص.ص
\`\`\`
`,
        }
    },
    {
        id: 'web',
        name: 'تطبيق ويب',
        description: 'خادم HTTP بسيط مع توجيه',
        icon: '🌐',
        files: {
            'خادم.ص': `# خادم ويب بسيط
استورد "شبكة/http" من "مكتبة_قياسية"

دالة رئيسية()
    متغير خادم = جديد خادم_http(8080)
    
    خادم.عند_طلب("/", دالة(طلب، استجابة)
        استجابة.اكتب("<h1>مرحباً بالعالم!</h1>")
    نهاية)
    
    اطبع_سطر("الخادم يعمل على المنفذ 8080")
    خادم.ابدأ()
نهاية

رئيسية()
`,
            'اقرأني.md': `# {{name}} — تطبيق ويب

## التشغيل
\`\`\`
sad خادم.ص
\`\`\`

ثم افتح المتصفح على http://localhost:8080
`,
        }
    },
    {
        id: 'cli',
        name: 'أداة سطر أوامر',
        description: 'أداة CLI مع معالجة الوسائط',
        icon: '⌨️',
        files: {
            'أداة.ص': `# أداة سطر أوامر
استورد "نظام" من "مكتبة_قياسية"

دالة رئيسية()
    متغير وسائط = نظام.وسائط()
    
    إذا (وسائط.الطول() < 2)
        اطبع_سطر("الاستخدام: أداة <أمر> [خيارات]")
        اطبع_سطر("الأوامر:")
        اطبع_سطر("  مساعدة  — عرض المساعدة")
        اطبع_سطر("  نسخة   — عرض النسخة")
        ارجع
    نهاية
    
    متغير أمر = وسائط[1]
    
    طابق أمر
        عندما "مساعدة"
            اطبع_سطر("مساعدة الأداة...")
        عندما "نسخة"
            اطبع_سطر("النسخة 1.0.0")
        افتراضي
            اطبع_سطر("أمر غير معروف: " + أمر)
    نهاية
نهاية

رئيسية()
`,
            'اقرأني.md': `# {{name}} — أداة CLI

## الاستخدام
\`\`\`
sad أداة.ص مساعدة
sad أداة.ص نسخة
\`\`\`
`,
        }
    },
    {
        id: 'graphics',
        name: 'تطبيق رسومي',
        description: 'تطبيق مع نافذة رسوميات',
        icon: '🎨',
        files: {
            'رسم.ص': `# تطبيق رسومي
استورد "رسوميات" من "مكتبة_قياسية"

دالة رئيسية()
    متغير نافذة = جديد نافذة("تطبيقي الرسومي"، 800، 600)
    
    بينما (نافذة.مفتوحة())
        نافذة.مسح(لون(30، 30، 50))
        
        # رسم دائرة
        نافذة.ارسم_دائرة(400، 300، 100، لون(255، 100، 50))
        
        # رسم نص
        نافذة.ارسم_نص("مرحباً! 🎨"، 300، 200، لون(255، 255، 255))
        
        نافذة.حدّث()
    نهاية
نهاية

رئيسية()
`,
            'اقرأني.md': `# {{name}} — تطبيق رسومي

يتطلب مكتبة الرسوميات (SDL2/OpenGL)

## التشغيل
\`\`\`
sad رسم.ص
\`\`\`
`,
        }
    },
    {
        id: 'library',
        name: 'مكتبة',
        description: 'مكتبة قابلة لإعادة الاستخدام مع اختبارات',
        icon: '📚',
        files: {
            'المصدر/المكتبة.ص': `# مكتبة {{name}}
# وحدة قابلة لإعادة الاستخدام

صدّر دالة جمع(أ، ب)
    ارجع أ + ب
نهاية

صدّر دالة طرح(أ، ب)
    ارجع أ - ب
نهاية

صدّر دالة ضرب(أ، ب)
    ارجع أ * ب
نهاية

صدّر دالة قسمة(أ، ب)
    إذا (ب == 0)
        ارمي "لا يمكن القسمة على صفر"
    نهاية
    ارجع أ / ب
نهاية
`,
            'اختبارات/اختبار_المكتبة.ص': `# اختبارات المكتبة
استورد "المصدر/المكتبة"

اختبر "اختبار الجمع"
    متغير نتيجة = جمع(2، 3)
    اطبع_سطر("2 + 3 = " + نتيجة)
نهاية

اختبر "اختبار القسمة على صفر"
    حاول
        قسمة(10، 0)
        اطبع_سطر("خطأ: لم يتم رمي استثناء")
    امسك خطأ
        اطبع_سطر("✅ تم اكتشاف القسمة على صفر: " + خطأ)
    نهاية
نهاية
`,
            'اقرأني.md': `# {{name}} — مكتبة

## الاستخدام
\`\`\`sad
استورد "المصدر/المكتبة"

متغير نتيجة = جمع(5، 3)
\`\`\`
`,
        }
    },
];

async function createNewProject(): Promise<void> {
    // اختيار القالب
    const items = TEMPLATES.map(t => ({
        label: `${t.icon} ${t.name}`,
        description: t.description,
        template: t,
    }));

    const selected = await vscode.window.showQuickPick(items, {
        placeHolder: 'اختر نوع المشروع — Choose project type',
    });

    if (!selected) { return; }

    // اسم المشروع
    const projectName = await vscode.window.showInputBox({
        prompt: 'اسم المشروع',
        placeHolder: 'مشروعي_الجديد',
        validateInput: (v) => v.trim() ? null : 'يجب إدخال اسم المشروع',
    });

    if (!projectName) { return; }

    // اختيار مجلد
    const folders = await vscode.window.showOpenDialog({
        canSelectFolders: true,
        canSelectFiles: false,
        openLabel: 'اختر مجلد المشروع',
    });

    if (!folders || folders.length === 0) { return; }

    const projectDir = path.join(folders[0].fsPath, projectName);

    // إنشاء الملفات
    const template = selected.template;
    for (const [filePath, content] of Object.entries(template.files)) {
        const fullPath = path.join(projectDir, filePath);
        const dir = path.dirname(fullPath);

        if (!fs.existsSync(dir)) {
            fs.mkdirSync(dir, { recursive: true });
        }

        const processedContent = content.replace(/\{\{name\}\}/g, projectName);
        fs.writeFileSync(fullPath, processedContent, 'utf-8');
    }

    // فتح المشروع
    const openChoice = await vscode.window.showInformationMessage(
        `✅ تم إنشاء المشروع "${projectName}" بنجاح!`,
        'فتح في نافذة جديدة',
        'فتح في النافذة الحالية',
        'لا شكراً'
    );

    if (openChoice === 'فتح في نافذة جديدة') {
        await vscode.commands.executeCommand('vscode.openFolder', vscode.Uri.file(projectDir), true);
    } else if (openChoice === 'فتح في النافذة الحالية') {
        await vscode.commands.executeCommand('vscode.openFolder', vscode.Uri.file(projectDir), false);
    }
}

async function addModule(): Promise<void> {
    const moduleName = await vscode.window.showInputBox({
        prompt: 'اسم الوحدة الجديدة',
        placeHolder: 'اسم_الوحدة',
    });

    if (!moduleName) { return; }

    const workspaceFolder = vscode.workspace.workspaceFolders?.[0];
    if (!workspaceFolder) { return; }

    const content = `# وحدة ${moduleName}
# تم إنشاؤها بواسطة أداة سقالات لغة ص

## الوظائف المصدّرة:

صدّر دالة ${moduleName}_ابدأ()
    اطبع_سطر("وحدة ${moduleName} جاهزة")
نهاية
`;

    const filePath = path.join(workspaceFolder.uri.fsPath, `${moduleName}.ص`);
    fs.writeFileSync(filePath, content, 'utf-8');

    const doc = await vscode.workspace.openTextDocument(filePath);
    await vscode.window.showTextDocument(doc);

    vscode.window.showInformationMessage(`✅ تم إنشاء الوحدة: ${moduleName}.ص`);
}

async function addTestFile(): Promise<void> {
    const editor = vscode.window.activeTextEditor;
    const baseName = editor ? path.basename(editor.document.uri.fsPath, '.ص') : '';

    const testName = await vscode.window.showInputBox({
        prompt: 'اسم ملف الاختبار',
        value: baseName ? `اختبار_${baseName}` : 'اختبار_جديد',
    });

    if (!testName) { return; }

    const workspaceFolder = vscode.workspace.workspaceFolders?.[0];
    if (!workspaceFolder) { return; }

    const importLine = baseName ? `استورد "${baseName}"\n\n` : '';

    const content = `# ملف اختبار: ${testName}
${importLine}اختبر "الاختبار الأول"
    # ضع اختباراتك هنا
    اطبع_سطر("✅ نجح الاختبار")
نهاية

اختبر "الاختبار الثاني"
    # اختبار آخر
    اطبع_سطر("✅ نجح الاختبار")
نهاية
`;

    const testDir = path.join(workspaceFolder.uri.fsPath, 'اختبارات');
    if (!fs.existsSync(testDir)) {
        fs.mkdirSync(testDir, { recursive: true });
    }

    const filePath = path.join(testDir, `${testName}.ص`);
    fs.writeFileSync(filePath, content, 'utf-8');

    const doc = await vscode.workspace.openTextDocument(filePath);
    await vscode.window.showTextDocument(doc);

    vscode.window.showInformationMessage(`✅ تم إنشاء ملف الاختبار: ${testName}.ص`);
}
