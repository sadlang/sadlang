/**
 * @file tutorial.ts
 * @brief المساعد التعليمي التفاعلي — Interactive Arabic Tutorial Engine
 * 
 * هذا الملف يوفر نظام تعليمي تفاعلي مدمج في VS Code
 * يُرشد المبرمج المبتدئ خطوة بخطوة لتعلم لغة ص.
 * 
 * === البنية ===
 * 
 * النظام التعليمي يتكون من:
 *   - دروس (Lessons): كل درس يشرح مفهوماً واحداً
 *   - تمارين (Exercises): أسئلة عملية بعد كل درس
 *   - تحقق (Validation): يتحقق من صحة إجابة المتعلم
 *   - شريط تقدم: يعرض نسبة الإنجاز
 * 
 * === الأقسام التعليمية ===
 * 
 * 1. أساسيات   — المتغيرات، الثوابت، الطباعة
 * 2. التحكم    — الشروط، الحلقات
 * 3. الدوال    — تعريف واستدعاء الدوال
 * 4. المصفوفات — إنشاء والتعامل مع المصفوفات
 * 5. النصوص    — عمليات على النصوص
 * 6. الكائنات  — الأصناف والوراثة
 * 7. المتقدم   — الاستثناءات، التعدادات، السمات
 * 
 * === كيف يعمل ===
 * 
 * 1. المستخدم يفتح المساعد التعليمي
 * 2. يختار القسم أو ينقر "التالي"
 * 3. يقرأ الشرح ويطبق التمرين في المحرر
 * 4. ينقر "تحقق" لفحص إجابته
 * 5. يحصل على ملاحظات فورية (✅ أو ❌)
 * 6. يتقدم إلى الدرس التالي
 * 
 * @author فريق لغة ص
 * @date 2025
 */

import * as vscode from 'vscode';

// ─────────────────────────────────────────────
// تعريف هيكل الدرس والتمرين
// ─────────────────────────────────────────────

/**
 * @brief تمرين واحد
 * 
 * يحتوي على تعليمات وكود مبدئي ودالة تحقق.
 */
interface Exercise {
    /** العنوان */
    title: string;
    /** التعليمات */
    instructions: string;
    /** الكود المبدئي (يُوضع في المحرر) */
    starterCode: string;
    /** شروط التحقق — regex أو كلمات مفتاحية يجب وجودها */
    validation: {
        mustContain?: string[];
        mustNotContain?: string[];
        mustMatch?: RegExp;
    };
    /** تلميح يُعرض عند الخطأ */
    hint: string;
}

/**
 * @brief درس واحد
 * 
 * يحتوي على شرح نظري + تمرين عملي.
 */
interface Lesson {
    /** عنوان الدرس */
    title: string;
    /** رمز القسم */
    icon: string;
    /** الشرح النظري (HTML) */
    explanation: string;
    /** أمثلة كود */
    codeExample: string;
    /** التمرين المطلوب */
    exercise: Exercise;
}

// ─────────────────────────────────────────────
// محتوى الدروس التعليمية
// ─────────────────────────────────────────────

/**
 * @brief المنهج التعليمي الكامل
 * 
 * كل درس يتضمن:
 *   - شرح نظري بالعربية
 *   - مثال كود عملي
 *   - تمرين مع تحقق آلي
 *   - تلميح عند الخطأ
 */
const LESSONS: Lesson[] = [
    // ═══ الدرس 1: أهلاً بالعالم ═══
    {
        title: 'أهلاً بالعالم',
        icon: '👋',
        explanation: `
            <h3>مرحباً بك في لغة ص!</h3>
            <p>لغة ص هي أول لغة برمجة عربية كاملة. كل الكلمات المفتاحية بالعربية.</p>
            <p>لنبدأ بأبسط برنامج — طباعة نص على الشاشة:</p>
            <ul>
                <li><code>اطبع("نص")</code> — يطبع نصاً بدون سطر جديد</li>
                <li><code>اطبع_سطر("نص")</code> — يطبع نصاً مع سطر جديد</li>
            </ul>
        `,
        codeExample: `# أول برنامج بلغة ص\nاطبع_سطر("أهلاً بالعالم!")`,
        exercise: {
            title: 'اطبع اسمك',
            instructions: 'اكتب برنامجاً يطبع اسمك باستخدام <code>اطبع_سطر</code>',
            starterCode: '# اكتب كودك هنا\n',
            validation: { mustContain: ['اطبع_سطر'] },
            hint: 'استخدم: اطبع_سطر("اسمك")',
        },
    },

    // ═══ الدرس 2: المتغيرات ═══
    {
        title: 'المتغيرات والثوابت',
        icon: '📦',
        explanation: `
            <h3>المتغيرات</h3>
            <p>المتغيرات تُخزّن القيم. أنشئها بكلمة <code>متغير</code>:</p>
            <ul>
                <li><code>متغير</code> — قيمة قابلة للتغيير</li>
                <li><code>ثابت</code> — قيمة لا يمكن تغييرها</li>
            </ul>
            <p>الأنواع: <code>رقم</code>، <code>عشري</code>، <code>نص</code>، <code>منطقي</code></p>
        `,
        codeExample: `متغير العمر = 25\nثابت الاسم = "أحمد"\nمتغير الطول = 1.75\n\nاطبع_سطر("الاسم: " + الاسم)\nاطبع_سطر("العمر: " + العمر)`,
        exercise: {
            title: 'أنشئ متغيراتك',
            instructions: 'أنشئ متغيراً اسمه <code>العدد</code> بقيمة رقمية، وثابتاً اسمه <code>اللقب</code> بقيمة نصية',
            starterCode: '# أنشئ متغير وثابت\n\n\n# اطبع القيم\n',
            validation: { mustContain: ['متغير', 'ثابت', 'العدد', 'اللقب'] },
            hint: 'استخدم: متغير العدد = 10 ثم ثابت اللقب = "..."',
        },
    },

    // ═══ الدرس 3: الشروط ═══
    {
        title: 'الشروط',
        icon: '🔀',
        explanation: `
            <h3>جملة الشرط</h3>
            <p>استخدم <code>إذا</code> و<code>وإلا</code> للتحكم في مسار البرنامج:</p>
            <ul>
                <li>الشرط يُوضع بين قوسين <code>( )</code></li>
                <li>الكتلة تنتهي بـ <code>نهاية</code></li>
                <li>عوامل المقارنة: <code>==</code>، <code>!=</code>، <code>&lt;</code>، <code>&gt;</code>، <code>&lt;=</code>، <code>&gt;=</code></li>
            </ul>
        `,
        codeExample: `متغير الدرجة = 85\n\nإذا (الدرجة >= 90)\n    اطبع_سطر("ممتاز!")\nوإلا\n    إذا (الدرجة >= 70)\n        اطبع_سطر("جيد")\n    وإلا\n        اطبع_سطر("يحتاج تحسين")\n    نهاية\nنهاية`,
        exercise: {
            title: 'فحص العمر',
            instructions: 'اكتب برنامجاً يفحص متغير <code>العمر</code>: إذا >= 18 يطبع "بالغ"، وإلا يطبع "قاصر"',
            starterCode: 'متغير العمر = 20\n\n# اكتب شرطك هنا\n',
            validation: { mustContain: ['إذا', 'وإلا', 'نهاية', 'العمر'] },
            hint: 'استخدم: إذا (العمر >= 18) ... وإلا ... نهاية',
        },
    },

    // ═══ الدرس 4: الحلقات ═══
    {
        title: 'الحلقات',
        icon: '🔄',
        explanation: `
            <h3>الحلقات التكرارية</h3>
            <p>لغة ص توفر نوعين من الحلقات:</p>
            <ul>
                <li><code>بينما (شرط)</code> — تكرار طالما الشرط صحيح</li>
                <li><code>لكل عنصر في مجموعة</code> — تكرار على كل عنصر</li>
            </ul>
            <p>استخدم <code>توقف</code> للخروج و<code>استمر</code> لتخطي التكرار الحالي</p>
        `,
        codeExample: `# حلقة بينما\nمتغير ع = 1\nبينما (ع <= 5)\n    اطبع_سطر(ع)\n    ع = ع + 1\nنهاية\n\n# حلقة لكل\nلكل فاكهة في ["تفاح"، "موز"، "برتقال"]\n    اطبع_سطر(فاكهة)\nنهاية`,
        exercise: {
            title: 'طباعة الأعداد',
            instructions: 'اكتب حلقة <code>بينما</code> تطبع الأعداد من 1 إلى 10',
            starterCode: '# اطبع الأعداد 1-10\n\n',
            validation: { mustContain: ['بينما', 'نهاية'] },
            hint: 'أنشئ متغيراً عدّاداً واستخدم بينما (العداد <= 10)',
        },
    },

    // ═══ الدرس 5: الدوال ═══
    {
        title: 'الدوال',
        icon: '⚡',
        explanation: `
            <h3>تعريف الدوال</h3>
            <p>الدوال تنظم الكود في وحدات قابلة لإعادة الاستخدام:</p>
            <ul>
                <li><code>دالة اسم(معامل1، معامل2)</code> — تعريف الدالة</li>
                <li><code>ارجع قيمة</code> — إرجاع قيمة</li>
                <li>استدعاء: <code>اسم(وسيط1، وسيط2)</code></li>
            </ul>
        `,
        codeExample: `دالة مرحباً(اسم)\n    ارجع "أهلاً يا " + اسم + "!"\nنهاية\n\nدالة مضاعفة(عدد)\n    ارجع عدد * 2\nنهاية\n\nاطبع_سطر(مرحباً("سارة"))\nاطبع_سطر(مضاعفة(21))`,
        exercise: {
            title: 'دالة الجمع',
            instructions: 'اكتب دالة <code>جمع</code> تأخذ عددين وترجع مجموعهما',
            starterCode: '# اكتب دالة الجمع\n\n\n# اختبر الدالة\n# اطبع_سطر(جمع(3، 5))\n',
            validation: { mustContain: ['دالة', 'جمع', 'ارجع', 'نهاية'] },
            hint: 'استخدم: دالة جمع(أ، ب) ثم ارجع أ + ب ثم نهاية',
        },
    },

    // ═══ الدرس 6: المصفوفات ═══
    {
        title: 'المصفوفات',
        icon: '📋',
        explanation: `
            <h3>المصفوفات (القوائم)</h3>
            <p>المصفوفات تُخزّن عدة قيم في متغير واحد:</p>
            <ul>
                <li><code>[قيمة1، قيمة2]</code> — إنشاء مصفوفة</li>
                <li><code>المصفوفة[0]</code> — الوصول بالفهرس (يبدأ من 0)</li>
                <li><code>لكل عنصر في المصفوفة</code> — التكرار</li>
            </ul>
        `,
        codeExample: `متغير الأعداد = [10، 20، 30، 40]\n\n# الوصول بالفهرس\nاطبع_سطر(الأعداد[0])  # يطبع 10\n\n# التكرار\nلكل عدد في الأعداد\n    اطبع_سطر(عدد)\nنهاية`,
        exercise: {
            title: 'مجموع المصفوفة',
            instructions: 'أنشئ مصفوفة أعداد واحسب مجموعها باستخدام <code>لكل</code>',
            starterCode: '# أنشئ مصفوفة واحسب المجموع\n\n',
            validation: { mustContain: ['لكل', 'في', 'نهاية'] },
            hint: 'أنشئ متغير مجموع = 0 ثم استخدم لكل لجمع الأعداد',
        },
    },

    // ═══ الدرس 7: الأصناف ═══
    {
        title: 'البرمجة الكائنية',
        icon: '🏗️',
        explanation: `
            <h3>الأصناف والكائنات</h3>
            <p>الأصناف تُجمّع البيانات والسلوك معاً:</p>
            <ul>
                <li><code>صنف اسم</code> — تعريف صنف</li>
                <li><code>باني(معاملات)</code> — المُنشئ</li>
                <li><code>هذا.خاصية</code> — خصائص الكائن</li>
                <li><code>جديد صنف()</code> — إنشاء كائن</li>
                <li><code>يرث</code> — الوراثة</li>
            </ul>
        `,
        codeExample: `صنف حيوان\n    باني(الاسم، الصوت)\n        هذا.الاسم = الاسم\n        هذا.الصوت = الصوت\n    نهاية\n\n    دالة تكلم()\n        اطبع_سطر(هذا.الاسم + " يقول: " + هذا.الصوت)\n    نهاية\nنهاية\n\nمتغير قطة = جديد حيوان("قطة"، "مياو")\nقطة.تكلم()`,
        exercise: {
            title: 'صنف شخص',
            instructions: 'أنشئ صنف <code>شخص</code> مع خاصيتي <code>الاسم</code> و<code>العمر</code> ودالة <code>قدّم</code>',
            starterCode: '# أنشئ صنف شخص\n\n',
            validation: { mustContain: ['صنف', 'شخص', 'باني', 'هذا', 'نهاية'] },
            hint: 'استخدم: صنف شخص ثم باني(الاسم، العمر) ثم هذا.الاسم = الاسم',
        },
    },
];

// ─────────────────────────────────────────────
// بناء WebView التعليمي
// ─────────────────────────────────────────────

/**
 * @brief بناء HTML للمساعد التعليمي
 * 
 * يُنشئ واجهة تعليمية كاملة مع:
 *   - قائمة الدروس (يسار)
 *   - محتوى الدرس (وسط)
 *   - تمرين مع تحقق (أسفل)
 *   - شريط تقدم
 * 
 * @param currentLesson رقم الدرس الحالي (0-based)
 * @param completedLessons أرقام الدروس المكتملة
 * @returns نص HTML
 */
function getTutorialHTML(currentLesson: number, completedLessons: Set<number>): string {
    const lesson = LESSONS[currentLesson];
    const progress = Math.round((completedLessons.size / LESSONS.length) * 100);
    
    // بناء قائمة الدروس
    const lessonListHTML = LESSONS.map((l, i) => {
        const done = completedLessons.has(i);
        const active = i === currentLesson;
        const cls = active ? 'active' : (done ? 'done' : '');
        return `<li class="${cls}" onclick="navigateLesson(${i})">
            <span class="icon">${done ? '✅' : l.icon}</span>
            <span class="title">${l.title}</span>
        </li>`;
    }).join('\n');
    
    return `<!DOCTYPE html>
<html lang="ar" dir="rtl">
<head>
    <meta charset="UTF-8">
    <title>المساعد التعليمي — لغة ص</title>
    <style>
        * { box-sizing: border-box; }
        body {
            font-family: 'Segoe UI', Tahoma, sans-serif;
            background: var(--vscode-editor-background);
            color: var(--vscode-editor-foreground);
            margin: 0; display: flex; height: 100vh;
        }
        
        /* === الشريط الجانبي === */
        .sidebar {
            width: 220px; flex-shrink: 0;
            background: var(--vscode-sideBar-background);
            border-left: 1px solid var(--vscode-panel-border);
            display: flex; flex-direction: column;
            overflow-y: auto;
        }
        .sidebar h2 {
            font-size: 14px; padding: 12px; margin: 0;
            background: var(--vscode-titleBar-activeBackground);
        }
        .sidebar ul { list-style: none; padding: 0; margin: 0; }
        .sidebar li {
            padding: 10px 12px; cursor: pointer; display: flex;
            align-items: center; gap: 8px; font-size: 13px;
            border-bottom: 1px solid var(--vscode-panel-border);
        }
        .sidebar li:hover { background: var(--vscode-list-hoverBackground); }
        .sidebar li.active {
            background: var(--vscode-list-activeSelectionBackground);
            color: var(--vscode-list-activeSelectionForeground);
        }
        .sidebar li.done { opacity: 0.7; }
        
        /* === المحتوى === */
        .main { flex: 1; display: flex; flex-direction: column; overflow-y: auto; }
        
        .progress-bar {
            height: 4px; background: var(--vscode-progressBar-background, #333);
        }
        .progress-fill {
            height: 100%; background: #0078d4;
            width: ${progress}%;
            transition: width 0.3s;
        }
        
        .lesson-content { padding: 20px 24px; flex: 1; }
        .lesson-content h2 { margin-top: 0; font-size: 22px; }
        .lesson-content code {
            background: var(--vscode-textCodeBlock-background);
            padding: 1px 6px; border-radius: 3px; font-size: 13px;
            font-family: 'Cascadia Code', 'Consolas', monospace;
            direction: ltr; unicode-bidi: embed;
        }
        .lesson-content pre {
            background: var(--vscode-textCodeBlock-background);
            padding: 12px 16px; border-radius: 6px; font-size: 13px;
            font-family: 'Cascadia Code', 'Consolas', monospace;
            direction: ltr; text-align: left; overflow-x: auto;
            border: 1px solid var(--vscode-panel-border);
        }
        
        .exercise-box {
            background: var(--vscode-textCodeBlock-background);
            border: 1px solid var(--vscode-panel-border);
            border-radius: 8px; padding: 16px; margin-top: 20px;
        }
        .exercise-box h3 { margin-top: 0; color: #0078d4; }
        
        .btn {
            padding: 8px 20px; border: none; border-radius: 4px;
            cursor: pointer; font-size: 13px; color: #fff;
            font-family: inherit;
        }
        .btn-primary { background: #0078d4; }
        .btn-success { background: #388e3c; }
        .btn-secondary { background: #555; }
        .btn:hover { opacity: 0.9; }
        
        .actions { display: flex; gap: 8px; margin-top: 12px; }
        
        .feedback {
            margin-top: 12px; padding: 10px; border-radius: 6px;
            display: none;
        }
        .feedback.success { display: block; background: #1b5e20; color: #a5d6a7; }
        .feedback.error { display: block; background: #b71c1c; color: #ef9a9a; }
        
        .progress-text {
            padding: 8px 16px; font-size: 12px; opacity: 0.6;
            text-align: center;
        }
        
        .nav-btns {
            display: flex; justify-content: space-between;
            padding: 12px 24px; border-top: 1px solid var(--vscode-panel-border);
        }
    </style>
</head>
<body>
    <div class="sidebar">
        <h2>📚 الدروس</h2>
        <ul>${lessonListHTML}</ul>
        <div class="progress-text">${completedLessons.size} / ${LESSONS.length} مكتمل (${progress}%)</div>
    </div>
    
    <div class="main">
        <div class="progress-bar"><div class="progress-fill"></div></div>
        
        <div class="lesson-content">
            <h2>${lesson.icon} ${lesson.title}</h2>
            ${lesson.explanation}
            
            <h3>📝 مثال:</h3>
            <pre>${escapeHTML(lesson.codeExample)}</pre>
            
            <div class="exercise-box">
                <h3>🎯 تمرين: ${lesson.exercise.title}</h3>
                <p>${lesson.exercise.instructions}</p>
                
                <div class="actions">
                    <button class="btn btn-primary" onclick="openExercise()">📝 فتح في المحرر</button>
                    <button class="btn btn-success" onclick="checkAnswer()">✅ تحقق من الإجابة</button>
                </div>
                
                <div id="feedback" class="feedback"></div>
            </div>
        </div>
        
        <div class="nav-btns">
            <button class="btn btn-secondary" onclick="navigateLesson(${currentLesson - 1})"
                ${currentLesson === 0 ? 'disabled style="opacity:0.3"' : ''}>
                ← السابق
            </button>
            <button class="btn btn-primary" onclick="navigateLesson(${currentLesson + 1})"
                ${currentLesson >= LESSONS.length - 1 ? 'disabled style="opacity:0.3"' : ''}>
                التالي →
            </button>
        </div>
    </div>
    
    <script>
        const vscode = acquireVsCodeApi();
        
        function navigateLesson(index) {
            if (index < 0 || index >= ${LESSONS.length}) return;
            vscode.postMessage({ command: 'navigate', lesson: index });
        }
        
        function openExercise() {
            vscode.postMessage({ command: 'openExercise', lesson: ${currentLesson} });
        }
        
        function checkAnswer() {
            vscode.postMessage({ command: 'checkAnswer', lesson: ${currentLesson} });
        }
    </script>
</body>
</html>`;
}

/**
 * @brief تهريب HTML
 * 
 * يحوّل الأحرف الخاصة إلى entities لمنع XSS.
 */
function escapeHTML(text: string): string {
    return text.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
}

/**
 * @brief تسجيل المساعد التعليمي
 * 
 * يسجل أمر sad.openTutorial الذي:
 *   1. يفتح WebView مع قائمة الدروس
 *   2. يسمح بالتنقل بين الدروس
 *   3. يفتح تمارين في المحرر
 *   4. يتحقق من الإجابات
 *   5. يتتبع التقدم
 * 
 * @param context سياق الإضافة
 */
export function registerTutorial(context: vscode.ExtensionContext): void {
    let panel: vscode.WebviewPanel | undefined;
    let currentLesson = 0;
    
    // تحميل التقدم المحفوظ
    const savedProgress = context.globalState.get<number[]>('tutorialProgress', []);
    const completedLessons = new Set<number>(savedProgress);
    
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.openTutorial', () => {
            if (panel) {
                panel.reveal();
                return;
            }
            
            panel = vscode.window.createWebviewPanel(
                'sadTutorial',
                '📚 تعلّم لغة ص',
                vscode.ViewColumn.One,
                { enableScripts: true, retainContextWhenHidden: true }
            );
            
            updatePanel();
            
            panel.webview.onDidReceiveMessage(async (msg: any) => {
                switch (msg.command) {
                    case 'navigate':
                        currentLesson = msg.lesson;
                        updatePanel();
                        break;
                        
                    case 'openExercise': {
                        // فتح التمرين في ملف جديد
                        const lesson = LESSONS[msg.lesson];
                        const doc = await vscode.workspace.openTextDocument({
                            language: 'sad',
                            content: lesson.exercise.starterCode,
                        });
                        await vscode.window.showTextDocument(doc, vscode.ViewColumn.Beside);
                        break;
                    }
                    
                    case 'checkAnswer': {
                        // التحقق من الإجابة
                        const editor = vscode.window.activeTextEditor;
                        if (!editor || editor.document.languageId !== 'sad') {
                            vscode.window.showWarningMessage('افتح ملف .ص يحتوي إجابتك');
                            return;
                        }
                        
                        const code = editor.document.getText();
                        const lesson = LESSONS[msg.lesson];
                        const v = lesson.exercise.validation;
                        let passed = true;
                        let errorMsg = '';
                        
                        // فحص الكلمات المطلوبة
                        if (v.mustContain) {
                            for (const word of v.mustContain) {
                                if (!code.includes(word)) {
                                    passed = false;
                                    errorMsg = `❌ يجب أن يحتوي الكود على: ${word}`;
                                    break;
                                }
                            }
                        }
                        
                        // فحص الكلمات المحظورة
                        if (passed && v.mustNotContain) {
                            for (const word of v.mustNotContain) {
                                if (code.includes(word)) {
                                    passed = false;
                                    errorMsg = `❌ يجب ألا يحتوي الكود على: ${word}`;
                                    break;
                                }
                            }
                        }
                        
                        // فحص النمط
                        if (passed && v.mustMatch && !v.mustMatch.test(code)) {
                            passed = false;
                            errorMsg = '❌ الكود لا يطابق النمط المطلوب';
                        }
                        
                        if (passed) {
                            completedLessons.add(msg.lesson);
                            await context.globalState.update(
                                'tutorialProgress',
                                Array.from(completedLessons)
                            );
                            vscode.window.showInformationMessage('✅ أحسنت! التمرين صحيح.');
                            updatePanel();
                        } else {
                            const hint = lesson.exercise.hint;
                            vscode.window.showWarningMessage(
                                `${errorMsg}\n💡 تلميح: ${hint}`
                            );
                        }
                        break;
                    }
                }
            });
            
            panel.onDidDispose(() => { panel = undefined; });
        })
    );
    
    /** تحديث محتوى الويب */
    function updatePanel(): void {
        if (panel) {
            panel.webview.html = getTutorialHTML(currentLesson, completedLessons);
        }
    }
}
