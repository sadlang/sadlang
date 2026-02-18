#!/usr/bin/env node
// بسم الله الرحمن الرحيم
/**
 * @file seed.js
 * @description بذر قاعدة البيانات ببيانات اختبارية
 * Seeds the database with test data
 * 
 * Usage: node scripts/seed.js
 */

require('dotenv').config();
const bcrypt = require('bcryptjs');
const crypto = require('crypto');
const path = require('path');
const fs = require('fs');
const { initDatabase } = require('../src/models/database');

async function seed() {
    console.log('🌱 بذر قاعدة البيانات...\n');

    const db = initDatabase(process.env.DB_PATH || './data/registry.db');

    // ================================================================
    // إنشاء المستخدمين / Create Users
    // ================================================================
    console.log('👥 إنشاء المستخدمين...');

    const users = [
        { username: 'صالح', email: 'saleh@sadlang.org', display_name: 'صالح - مؤسس لغة ص', is_admin: 1 },
        { username: 'أحمد', email: 'ahmad@sadlang.org', display_name: 'أحمد المطور' },
        { username: 'فاطمة', email: 'fatima@sadlang.org', display_name: 'فاطمة المهندسة' },
        { username: 'sad-team', email: 'team@sadlang.org', display_name: 'فريق لغة ص', is_admin: 1 },
    ];

    const password = await bcrypt.hash('password123', 10);
    const insertUser = db.prepare(`
        INSERT OR IGNORE INTO users (username, email, password_hash, display_name, api_token, is_admin)
        VALUES (?, ?, ?, ?, ?, ?)
    `);

    const userIds = {};
    for (const u of users) {
        const token = `sad_${crypto.randomBytes(32).toString('hex')}`;
        const result = insertUser.run(u.username, u.email, password, u.display_name, token, u.is_admin || 0);
        if (result.changes > 0) {
            userIds[u.username] = result.lastInsertRowid;
            console.log(`  ✅ ${u.username} (token: ${token.substring(0, 15)}...)`);
        } else {
            const existing = db.prepare('SELECT id FROM users WHERE username = ?').get(u.username);
            userIds[u.username] = existing.id;
            console.log(`  ⏭️  ${u.username} (موجود مسبقاً)`);
        }
    }

    // ================================================================
    // إنشاء الحزم / Create Packages
    // ================================================================
    console.log('\n📦 إنشاء الحزم...');

    const packages = [
        {
            name: 'sad-math',
            description: 'Advanced mathematics library for Sad Language',
            description_ar: 'مكتبة رياضيات متقدمة للغة ص',
            owner: 'sad-team',
            license: 'MIT',
            keywords: ['رياضيات', 'math', 'حساب', 'جبر'],
            categories: ['math'],
            versions: [
                { version: '1.0.0', deps: {} },
                { version: '1.1.0', deps: {} },
                { version: '1.2.0', deps: {} },
            ]
        },
        {
            name: 'sad-crypto',
            description: 'Cryptography and hashing library',
            description_ar: 'مكتبة تشفير وتجزئة',
            owner: 'sad-team',
            license: 'MIT',
            keywords: ['تشفير', 'crypto', 'أمان', 'هاش'],
            categories: ['crypto'],
            versions: [
                { version: '1.0.0', deps: {} },
            ]
        },
        {
            name: 'sad-collections',
            description: 'Advanced data structures and collections',
            description_ar: 'هياكل بيانات ومجموعات متقدمة',
            owner: 'sad-team',
            license: 'MIT',
            keywords: ['هياكل', 'بيانات', 'مجموعات', 'قائمة', 'شجرة'],
            categories: ['data'],
            versions: [
                { version: '1.0.0', deps: {} },
            ]
        },
        {
            name: 'هجري',
            description: 'Hijri calendar conversion and utilities',
            description_ar: 'تحويل التاريخ الهجري وأدوات التقاويم الإسلامية',
            owner: 'أحمد',
            license: 'MIT',
            keywords: ['هجري', 'تقويم', 'تاريخ', 'إسلامي', 'hijri'],
            categories: ['date', 'islamic'],
            versions: [
                { version: '1.0.0', deps: {} },
                { version: '1.1.0', deps: {} },
            ]
        },
        {
            name: 'نص-عربي',
            description: 'Arabic text processing and normalization',
            description_ar: 'معالجة وتطبيع النصوص العربية (تشكيل، بحث، تنسيق)',
            owner: 'فاطمة',
            license: 'MIT',
            keywords: ['عربي', 'نص', 'تشكيل', 'arabic', 'text'],
            categories: ['text', 'i18n'],
            versions: [
                { version: '0.1.0', deps: {} },
                { version: '0.2.0', deps: {} },
                { version: '1.0.0', deps: {} },
            ]
        },
        {
            name: 'sad-json',
            description: 'JSON parser and serializer for Sad Language',
            description_ar: 'محلل ومنسق JSON للغة ص',
            owner: 'sad-team',
            license: 'MIT',
            keywords: ['json', 'تحليل', 'بيانات'],
            categories: ['data', 'io'],
            versions: [
                { version: '1.0.0', deps: {} },
            ]
        },
        {
            name: 'sad-http',
            description: 'HTTP client and server library',
            description_ar: 'مكتبة عميل وخادم HTTP',
            owner: 'صالح',
            license: 'MIT',
            keywords: ['http', 'شبكة', 'خادم', 'عميل', 'web'],
            categories: ['web', 'network'],
            versions: [
                { version: '0.1.0', deps: { 'sad-json': '^1.0.0' } },
                { version: '1.0.0', deps: { 'sad-json': '^1.0.0' } },
            ]
        },
        {
            name: 'sad-test',
            description: 'Testing framework for Sad Language',
            description_ar: 'إطار اختبارات للغة ص',
            owner: 'فاطمة',
            license: 'MIT',
            keywords: ['اختبار', 'test', 'testing', 'تأكد'],
            categories: ['testing'],
            versions: [
                { version: '1.0.0', deps: {} },
            ]
        },
        {
            name: 'عملات',
            description: 'Currency conversion for Arabic currencies (SAR, AED, EGP, etc.)',
            description_ar: 'تحويل العملات العربية (ريال، درهم، جنيه مصري، وأكثر)',
            owner: 'أحمد',
            license: 'MIT',
            keywords: ['عملات', 'مال', 'تحويل', 'ريال', 'currency'],
            categories: ['finance'],
            versions: [
                { version: '1.0.0', deps: { 'sad-http': '^1.0.0' } },
            ]
        },
        {
            name: 'sad-cli',
            description: 'CLI framework with argument parsing and colored output',
            description_ar: 'إطار سطر أوامر مع تحليل الأوامر وإخراج ملون',
            owner: 'صالح',
            license: 'MIT',
            keywords: ['cli', 'سطر أوامر', 'terminal', 'أوامر'],
            categories: ['cli'],
            versions: [
                { version: '1.0.0', deps: {} },
            ]
        },
    ];

    const insertPackage = db.prepare(`
        INSERT OR IGNORE INTO packages (name, description, description_ar, license, keywords, categories, owner_id, latest_version, total_downloads)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
    `);

    const insertVersion = db.prepare(`
        INSERT OR IGNORE INTO versions (package_id, version, major, minor, patch, description, dependencies, authors, license, published_by, downloads)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    `);

    const insertOwner = db.prepare(`
        INSERT OR IGNORE INTO package_owners (package_id, user_id, role) VALUES (?, ?, 'owner')
    `);

    const seedTransaction = db.transaction(() => {
        for (const pkg of packages) {
            const ownerId = userIds[pkg.owner];
            const latestVersion = pkg.versions[pkg.versions.length - 1].version;
            const downloads = Math.floor(Math.random() * 5000);

            const result = insertPackage.run(
                pkg.name, pkg.description, pkg.description_ar,
                pkg.license, JSON.stringify(pkg.keywords), JSON.stringify(pkg.categories),
                ownerId, latestVersion, downloads
            );

            let pkgId;
            if (result.changes > 0) {
                pkgId = result.lastInsertRowid;
                console.log(`  ✅ ${pkg.name} (${pkg.versions.length} إصدارات)`);
            } else {
                const existing = db.prepare('SELECT id FROM packages WHERE name = ?').get(pkg.name);
                pkgId = existing.id;
                console.log(`  ⏭️  ${pkg.name} (موجودة مسبقاً)`);
            }

            // إضافة المالك
            insertOwner.run(pkgId, ownerId);

            // إضافة الإصدارات
            for (const ver of pkg.versions) {
                const [major, minor, patch] = ver.version.split('.').map(Number);
                const verDownloads = Math.floor(Math.random() * 1000);

                insertVersion.run(
                    pkgId, ver.version, major, minor, patch,
                    pkg.description, JSON.stringify(ver.deps),
                    JSON.stringify([pkg.owner]),
                    pkg.license, ownerId, verDownloads
                );
            }
        }
    });

    seedTransaction();

    // ================================================================
    // إحصائيات / Statistics
    // ================================================================
    const stats = {
        users: db.prepare('SELECT COUNT(*) as c FROM users').get().c,
        packages: db.prepare('SELECT COUNT(*) as c FROM packages').get().c,
        versions: db.prepare('SELECT COUNT(*) as c FROM versions').get().c,
        categories: db.prepare('SELECT COUNT(*) as c FROM categories').get().c,
    };

    console.log('\n📊 الإحصائيات:');
    console.log(`  👥 ${stats.users} مستخدمين`);
    console.log(`  📦 ${stats.packages} حزم`);
    console.log(`  🏷️  ${stats.versions} إصدارات`);
    console.log(`  📂 ${stats.categories} تصنيفات`);
    console.log('\n✅ تم بذر قاعدة البيانات بنجاح!\n');

    // طباعة رمز API للمشرف
    const adminToken = db.prepare('SELECT api_token FROM users WHERE username = ?').get('صالح');
    if (adminToken) {
        console.log('🔑 رمز API للمشرف (صالح):');
        console.log(`   ${adminToken.api_token}`);
        console.log('');
    }
}

seed().catch(err => {
    console.error('❌ فشل البذر:', err.message);
    process.exit(1);
});
