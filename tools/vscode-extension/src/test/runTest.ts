/**
 * @file runTest.ts
 * @brief نقطة دخول تشغيل الاختبارات
 * 
 * يُشغّل جميع اختبارات الوحدة لإضافة لغة ص.
 * يستخدم Mocha كإطار اختبار.
 */

import * as path from 'path';
import Mocha from 'mocha';
import glob from 'glob';

export function run(): Promise<void> {
    const mocha = new Mocha({
        ui: 'tdd',
        color: true,
        timeout: 10000,
    });

    const testsRoot = path.resolve(__dirname, '.');

    return new Promise((resolve, reject) => {
        glob('**/**.test.js', { cwd: testsRoot }, (err: Error | null, files: string[]) => {
            if (err) {
                reject(err);
                return;
            }
            files.forEach((f: string) => mocha.addFile(path.resolve(testsRoot, f)));

            try {
                mocha.run((failures: number) => {
                    if (failures > 0) {
                        reject(new Error(`${failures} tests failed.`));
                    } else {
                        resolve();
                    }
                });
            } catch (e) {
                reject(e);
            }
        });
    });
}
