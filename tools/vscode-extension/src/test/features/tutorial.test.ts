/**
 * @file tutorial.test.ts
 * @brief اختبارات وحدة للدروس التفاعلية — 40 اختبار
 *
 * يختبر هيكل الدروس، التحقق من التمارين،
 * تتبع التقدم، ومعالجة المحتوى التعليمي.
 */

import * as assert from 'assert';

// ─── محاكاة نظام الدروس ───

interface Exercise {
    instruction: string;
    initialCode: string;
    mustContain?: string[];
    mustNotContain?: string[];
    mustMatch?: RegExp;
}

interface Lesson {
    id: number;
    title: string;
    description: string;
    exercises: Exercise[];
}

interface Progress {
    completed: number[];
    current: number;
    scores: { [key: number]: number };
}

/** التحقق من إجابة تمرين */
function validateExercise(exercise: Exercise, userCode: string): { passed: boolean; message: string } {
    const trimmed = userCode.trim();
    if (trimmed === '') {
        return { passed: false, message: 'الكود فارغ' };
    }
    if (exercise.mustContain) {
        for (const keyword of exercise.mustContain) {
            if (!trimmed.includes(keyword)) {
                return { passed: false, message: `يجب أن يحتوي الكود على: ${keyword}` };
            }
        }
    }
    if (exercise.mustNotContain) {
        for (const keyword of exercise.mustNotContain) {
            if (trimmed.includes(keyword)) {
                return { passed: false, message: `يجب ألا يحتوي الكود على: ${keyword}` };
            }
        }
    }
    if (exercise.mustMatch) {
        if (!exercise.mustMatch.test(trimmed)) {
            return { passed: false, message: 'الكود لا يطابق النمط المطلوب' };
        }
    }
    return { passed: true, message: 'أحسنت! ✅' };
}

/** حساب نسبة الإكمال */
function completionPercentage(progress: Progress, totalLessons: number): number {
    if (totalLessons === 0) return 0;
    return Math.round((progress.completed.length / totalLessons) * 100);
}

/** هل الدرس مكتمل */
function isCompleted(progress: Progress, lessonId: number): boolean {
    return progress.completed.includes(lessonId);
}

/** الدرس التالي */
function nextLesson(progress: Progress, totalLessons: number): number | null {
    for (let i = 1; i <= totalLessons; i++) {
        if (!progress.completed.includes(i)) return i;
    }
    return null;
}

/** تسجيل إكمال درس */
function completeLesson(progress: Progress, lessonId: number, score: number): Progress {
    const newCompleted = progress.completed.includes(lessonId)
        ? progress.completed
        : [...progress.completed, lessonId];
    return {
        completed: newCompleted,
        current: lessonId,
        scores: { ...progress.scores, [lessonId]: score },
    };
}

/** متوسط النقاط */
function averageScore(progress: Progress): number {
    const scores = Object.values(progress.scores);
    if (scores.length === 0) return 0;
    return Math.round(scores.reduce((a: number, b: number) => a + b, 0) / scores.length);
}

// ─── بيانات الدروس ───

const LESSONS: Lesson[] = [
    {
        id: 1, title: 'مرحبا بالعالم', description: 'أول برنامج',
        exercises: [{ instruction: 'اكتب أمر طباعة', initialCode: '', mustContain: ['اطبع'] }],
    },
    {
        id: 2, title: 'المتغيرات', description: 'تعريف المتغيرات',
        exercises: [{ instruction: 'عرّف متغيرا', initialCode: '', mustContain: ['متغير'], mustMatch: /متغير\s+\S+\s*=/ }],
    },
    {
        id: 3, title: 'الشروط', description: 'عبارات إذا',
        exercises: [{ instruction: 'اكتب شرطا', initialCode: '', mustContain: ['إذا', 'نهاية'] }],
    },
    {
        id: 4, title: 'الحلقات', description: 'التكرار',
        exercises: [{ instruction: 'اكتب حلقة', initialCode: '', mustContain: ['بينما', 'نهاية'] }],
    },
    {
        id: 5, title: 'الدوال', description: 'تعريف الدوال',
        exercises: [{ instruction: 'عرّف دالة', initialCode: '', mustContain: ['دالة', 'نهاية'] }],
    },
    {
        id: 6, title: 'المصفوفات', description: 'هياكل بيانات',
        exercises: [{ instruction: 'أنشئ مصفوفة', initialCode: '', mustContain: ['[', ']'] }],
    },
    {
        id: 7, title: 'الأصناف', description: 'OOP',
        exercises: [{ instruction: 'عرّف صنفا', initialCode: '', mustContain: ['صنف', 'نهاية'] }],
    },
];

// ═════════════════════════════════════════════
suite('الدروس التفاعلية — Interactive Tutorial', () => {

    // ─── مجموعة 1: التحقق من التمارين (10) ───
    test('1. كود يحتوي الكلمة المطلوبة', () => {
        const ex: Exercise = { instruction: '', initialCode: '', mustContain: ['اطبع'] };
        assert.strictEqual(validateExercise(ex, 'اطبع("مرحبا")').passed, true);
    });
    test('2. كود لا يحتوي الكلمة المطلوبة', () => {
        const ex: Exercise = { instruction: '', initialCode: '', mustContain: ['اطبع'] };
        assert.strictEqual(validateExercise(ex, 'متغير س = 1').passed, false);
    });
    test('3. كود فارغ', () => {
        const ex: Exercise = { instruction: '', initialCode: '' };
        assert.strictEqual(validateExercise(ex, '').passed, false);
    });
    test('4. mustNotContain — ناجح', () => {
        const ex: Exercise = { instruction: '', initialCode: '', mustNotContain: ['while'] };
        assert.strictEqual(validateExercise(ex, 'بينما (صحيح)\nنهاية').passed, true);
    });
    test('5. mustNotContain — فاشل', () => {
        const ex: Exercise = { instruction: '', initialCode: '', mustNotContain: ['print'] };
        assert.strictEqual(validateExercise(ex, 'print("hello")').passed, false);
    });
    test('6. mustMatch — ناجح', () => {
        const ex: Exercise = { instruction: '', initialCode: '', mustMatch: /متغير\s+\S+\s*=/ };
        assert.strictEqual(validateExercise(ex, 'متغير س = 5').passed, true);
    });
    test('7. mustMatch — فاشل', () => {
        const ex: Exercise = { instruction: '', initialCode: '', mustMatch: /دالة\s+\S+/ };
        assert.strictEqual(validateExercise(ex, 'متغير س = 5').passed, false);
    });
    test('8. عدة mustContain — كلها موجودة', () => {
        const ex: Exercise = { instruction: '', initialCode: '', mustContain: ['إذا', 'نهاية'] };
        assert.strictEqual(validateExercise(ex, 'إذا (صحيح)\nنهاية').passed, true);
    });
    test('9. عدة mustContain — واحدة ناقصة', () => {
        const ex: Exercise = { instruction: '', initialCode: '', mustContain: ['إذا', 'نهاية'] };
        assert.strictEqual(validateExercise(ex, 'إذا (صحيح)').passed, false);
    });
    test('10. رسالة النجاح', () => {
        const ex: Exercise = { instruction: '', initialCode: '' };
        assert.ok(validateExercise(ex, 'أي كود').message.includes('✅'));
    });

    // ─── مجموعة 2: تتبع التقدم (10) ───
    test('11. بداية فارغة', () => {
        const p: Progress = { completed: [], current: 0, scores: {} };
        assert.strictEqual(completionPercentage(p, 7), 0);
    });
    test('12. كل الدروس مكتملة', () => {
        const p: Progress = { completed: [1, 2, 3, 4, 5, 6, 7], current: 7, scores: {} };
        assert.strictEqual(completionPercentage(p, 7), 100);
    });
    test('13. نصف الدروس', () => {
        const p: Progress = { completed: [1, 2, 3], current: 3, scores: {} };
        assert.strictEqual(completionPercentage(p, 6), 50);
    });
    test('14. isCompleted — نعم', () => {
        const p: Progress = { completed: [1, 2], current: 2, scores: {} };
        assert.strictEqual(isCompleted(p, 1), true);
    });
    test('15. isCompleted — لا', () => {
        const p: Progress = { completed: [1], current: 1, scores: {} };
        assert.strictEqual(isCompleted(p, 3), false);
    });
    test('16. nextLesson — التالي المتاح', () => {
        const p: Progress = { completed: [1, 2], current: 2, scores: {} };
        assert.strictEqual(nextLesson(p, 7), 3);
    });
    test('17. nextLesson — الكل مكتمل', () => {
        const p: Progress = { completed: [1, 2, 3, 4, 5, 6, 7], current: 7, scores: {} };
        assert.strictEqual(nextLesson(p, 7), null);
    });
    test('18. completeLesson — إضافة', () => {
        const p: Progress = { completed: [1], current: 1, scores: {} };
        const updated = completeLesson(p, 2, 90);
        assert.ok(updated.completed.includes(2));
    });
    test('19. completeLesson — لا تكرار', () => {
        const p: Progress = { completed: [1, 2], current: 2, scores: {} };
        const updated = completeLesson(p, 2, 100);
        assert.strictEqual(updated.completed.filter((x: number) => x === 2).length, 1);
    });
    test('20. completeLesson — النقاط', () => {
        const p: Progress = { completed: [], current: 0, scores: {} };
        const updated = completeLesson(p, 1, 85);
        assert.strictEqual(updated.scores[1], 85);
    });

    // ─── مجموعة 3: هيكل الدروس (10) ───
    test('21. عدد الدروس 7', () => {
        assert.strictEqual(LESSONS.length, 7);
    });
    test('22. الدرس الأول — مرحبا', () => {
        assert.strictEqual(LESSONS[0].title, 'مرحبا بالعالم');
    });
    test('23. كل درس له تمارين', () => {
        assert.ok(LESSONS.every((l: Lesson) => l.exercises.length > 0));
    });
    test('24. كل درس له معرف فريد', () => {
        const ids = LESSONS.map((l: Lesson) => l.id);
        assert.strictEqual(new Set(ids).size, ids.length);
    });
    test('25. الدرس الأخير — OOP', () => {
        assert.strictEqual(LESSONS[6].title, 'الأصناف');
    });
    test('26. درس المتغيرات — mustMatch', () => {
        assert.ok(LESSONS[1].exercises[0].mustMatch);
    });
    test('27. درس الشروط — يتطلب إذا', () => {
        assert.ok(LESSONS[2].exercises[0].mustContain?.includes('إذا'));
    });
    test('28. درس الحلقات — يتطلب بينما', () => {
        assert.ok(LESSONS[3].exercises[0].mustContain?.includes('بينما'));
    });
    test('29. درس الدوال — يتطلب دالة', () => {
        assert.ok(LESSONS[4].exercises[0].mustContain?.includes('دالة'));
    });
    test('30. كل درس له وصف', () => {
        assert.ok(LESSONS.every((l: Lesson) => l.description.length > 0));
    });

    // ─── مجموعة 4: نقاط ومتوسطات (10) ───
    test('31. متوسط فارغ = 0', () => {
        assert.strictEqual(averageScore({ completed: [], current: 0, scores: {} }), 0);
    });
    test('32. متوسط درس واحد', () => {
        assert.strictEqual(averageScore({ completed: [1], current: 1, scores: { 1: 80 } }), 80);
    });
    test('33. متوسط درسين', () => {
        assert.strictEqual(averageScore({ completed: [1, 2], current: 2, scores: { 1: 80, 2: 100 } }), 90);
    });
    test('34. متوسط مقرَّب', () => {
        // (80 + 70 + 85) / 3 = 78.33 → 78
        assert.strictEqual(averageScore({ completed: [1, 2, 3], current: 3, scores: { 1: 80, 2: 70, 3: 85 } }), 78);
    });
    test('35. نسبة مع 0 دروس', () => {
        assert.strictEqual(completionPercentage({ completed: [], current: 0, scores: {} }, 0), 0);
    });
    test('36. nextLesson — من البداية', () => {
        assert.strictEqual(nextLesson({ completed: [], current: 0, scores: {} }, 7), 1);
    });
    test('37. إكمال كل الدروس بنقاط', () => {
        let p: Progress = { completed: [], current: 0, scores: {} };
        for (let i = 1; i <= 7; i++) p = completeLesson(p, i, 100);
        assert.strictEqual(averageScore(p), 100);
        assert.strictEqual(completionPercentage(p, 7), 100);
    });
    test('38. إكمال درس وسط بدون ترتيب', () => {
        const p: Progress = { completed: [3, 1, 5], current: 5, scores: {} };
        assert.strictEqual(nextLesson(p, 7), 2);
    });
    test('39. تحديث نقاط درس مكتمل', () => {
        let p: Progress = { completed: [1], current: 1, scores: { 1: 70 } };
        p = completeLesson(p, 1, 95);
        assert.strictEqual(p.scores[1], 95);
    });
    test('40. completeLesson يحفظ الدرس الحالي', () => {
        const p = completeLesson({ completed: [], current: 0, scores: {} }, 4, 88);
        assert.strictEqual(p.current, 4);
    });
});
