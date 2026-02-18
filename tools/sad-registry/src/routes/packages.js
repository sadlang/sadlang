// بسم الله الرحمن الرحيم
/**
 * @file packages.js
 * @description مسارات إدارة الحزم - Package management routes
 * 
 * متوافق مع عقد API في registry_client.h
 */

const express = require('express');
const router = express.Router();
const multer = require('multer');
const path = require('path');
const fs = require('fs');
const crypto = require('crypto');
const semver = require('semver');
const tar = require('tar');

const { authenticate, optionalAuth } = require('../middleware/auth');
const { publishLimiter } = require('../middleware/rate-limit');
const { validatePackageName, validateVersion } = require('../utils/validators');

// ============================================================================
// إعداد تحميل الملفات / File Upload Configuration
// ============================================================================

const storage = multer.diskStorage({
    destination: (req, file, cb) => {
        const tmpDir = path.join(process.env.STORAGE_PATH || './storage/packages', '_tmp');
        if (!fs.existsSync(tmpDir)) {
            fs.mkdirSync(tmpDir, { recursive: true });
        }
        cb(null, tmpDir);
    },
    filename: (req, file, cb) => {
        const uniqueName = `${Date.now()}-${crypto.randomBytes(8).toString('hex')}${path.extname(file.originalname)}`;
        cb(null, uniqueName);
    }
});

const upload = multer({
    storage,
    limits: {
        fileSize: 50 * 1024 * 1024, // 50MB max
    },
    fileFilter: (req, file, cb) => {
        const allowed = ['.tar.gz', '.tgz', '.zip'];
        const ext = path.extname(file.originalname).toLowerCase();
        // Also check combined extension
        if (file.originalname.endsWith('.tar.gz') || allowed.includes(ext)) {
            cb(null, true);
        } else {
            cb(new Error('نوع ملف غير مدعوم. الأنواع المسموحة: .tar.gz, .tgz, .zip'));
        }
    }
});

// ============================================================================
// GET /api/v1/packages/:name — معلومات الحزمة / Package Info
// ============================================================================

router.get('/:name', (req, res) => {
    const { name } = req.params;
    const db = req.app.locals.db;

    try {
        const pkg = db.prepare(`
            SELECT p.*, u.username as author_name
            FROM packages p
            JOIN users u ON p.owner_id = u.id
            WHERE p.name = ?
        `).get(name);

        if (!pkg) {
            return res.status(404).json({ 
                error: `الحزمة غير موجودة: ${name}`,
                error_en: `Package not found: ${name}`
            });
        }

        // جلب كل الإصدارات / Fetch all versions
        const versions = db.prepare(`
            SELECT version, downloads, created_at, is_yanked
            FROM versions
            WHERE package_id = ?
            ORDER BY major DESC, minor DESC, patch DESC
        `).all(pkg.id);

        // جلب المالكين / Fetch owners
        const owners = db.prepare(`
            SELECT u.username, po.role
            FROM package_owners po
            JOIN users u ON po.user_id = u.id
            WHERE po.package_id = ?
        `).all(pkg.id);

        res.json({
            name: pkg.name,
            description: pkg.description,
            description_ar: pkg.description_ar,
            homepage: pkg.homepage,
            repository: pkg.repository,
            documentation: pkg.documentation,
            license: pkg.license,
            keywords: JSON.parse(pkg.keywords || '[]'),
            categories: JSON.parse(pkg.categories || '[]'),
            latest_version: pkg.latest_version,
            total_downloads: pkg.total_downloads,
            owners: owners,
            versions: versions.map(v => ({
                version: v.version,
                downloads: v.downloads,
                created_at: v.created_at,
                is_yanked: !!v.is_yanked,
            })),
            created_at: pkg.created_at,
            updated_at: pkg.updated_at,
        });
    } catch (error) {
        console.error('خطأ في جلب الحزمة:', error.message);
        res.status(500).json({ error: 'خطأ في الخادم / Server error' });
    }
});

// ============================================================================
// GET /api/v1/packages/:name/versions — إصدارات الحزمة / Package Versions
// ============================================================================

router.get('/:name/versions', (req, res) => {
    const { name } = req.params;
    const db = req.app.locals.db;

    try {
        const pkg = db.prepare('SELECT id FROM packages WHERE name = ?').get(name);
        if (!pkg) {
            return res.status(404).json({ error: `الحزمة غير موجودة: ${name}` });
        }

        const versions = db.prepare(`
            SELECT version FROM versions
            WHERE package_id = ? AND is_yanked = 0
            ORDER BY major DESC, minor DESC, patch DESC
        `).all(pkg.id);

        res.json({
            versions: versions.map(v => v.version)
        });
    } catch (error) {
        console.error('خطأ في جلب الإصدارات:', error.message);
        res.status(500).json({ error: 'خطأ في الخادم / Server error' });
    }
});

// ============================================================================
// GET /api/v1/packages/:name/:version — معلومات إصدار محدد / Version Info
// ============================================================================

router.get('/:name/:version', (req, res) => {
    const { name, version } = req.params;
    const db = req.app.locals.db;

    try {
        const result = db.prepare(`
            SELECT v.*, p.name as package_name, p.repository, p.homepage
            FROM versions v
            JOIN packages p ON v.package_id = p.id
            WHERE p.name = ? AND v.version = ?
        `).get(name, version);

        if (!result) {
            return res.status(404).json({
                error: `الإصدار غير موجود: ${name}@${version}`,
                error_en: `Version not found: ${name}@${version}`
            });
        }

        res.json({
            name: result.package_name,
            version: result.version,
            description: result.description,
            authors: JSON.parse(result.authors || '[]'),
            license: result.license,
            repository: result.repository,
            homepage: result.homepage,
            dependencies: JSON.parse(result.dependencies || '{}'),
            dev_dependencies: JSON.parse(result.dev_dependencies || '{}'),
            build_dependencies: JSON.parse(result.build_dependencies || '{}'),
            checksum: result.checksum_sha256,
            archive_size: result.archive_size,
            downloads: result.downloads,
            is_yanked: !!result.is_yanked,
            created_at: result.created_at,
        });
    } catch (error) {
        console.error('خطأ في جلب الإصدار:', error.message);
        res.status(500).json({ error: 'خطأ في الخادم / Server error' });
    }
});

// ============================================================================
// GET /api/v1/packages/:name/:version/download — تنزيل الحزمة / Download
// ============================================================================

router.get('/:name/:version/download', (req, res) => {
    const { name, version } = req.params;
    const db = req.app.locals.db;

    try {
        const result = db.prepare(`
            SELECT v.id, v.archive_path, v.checksum_sha256
            FROM versions v
            JOIN packages p ON v.package_id = p.id
            WHERE p.name = ? AND v.version = ? AND v.is_yanked = 0
        `).get(name, version);

        if (!result || !result.archive_path) {
            return res.status(404).json({
                error: `الحزمة غير موجودة للتنزيل: ${name}@${version}`,
                error_en: `Package not available for download: ${name}@${version}`
            });
        }

        const filePath = path.resolve(result.archive_path);
        if (!fs.existsSync(filePath)) {
            return res.status(404).json({ error: 'ملف الأرشيف غير موجود / Archive file not found' });
        }

        // تحديث عداد التنزيلات / Update download counter
        db.prepare('UPDATE versions SET downloads = downloads + 1 WHERE id = ?').run(result.id);
        db.prepare(`
            UPDATE packages SET total_downloads = total_downloads + 1 
            WHERE id = (SELECT package_id FROM versions WHERE id = ?)
        `).run(result.id);

        // تسجيل التنزيل / Log download
        db.prepare(`
            INSERT INTO download_log (version_id, ip_address, user_agent)
            VALUES (?, ?, ?)
        `).run(result.id, req.ip, req.get('user-agent') || '');

        // إرسال checksum في الرأس / Send checksum in header
        if (result.checksum_sha256) {
            res.set('X-Checksum-SHA256', result.checksum_sha256);
        }

        res.set('Content-Type', 'application/gzip');
        res.set('Content-Disposition', `attachment; filename="${name}-${version}.tar.gz"`);
        res.sendFile(filePath);

    } catch (error) {
        console.error('خطأ في التنزيل:', error.message);
        res.status(500).json({ error: 'خطأ في التنزيل / Download error' });
    }
});

// ============================================================================
// POST /api/v1/packages/publish — نشر حزمة / Publish Package
// ============================================================================

router.post('/publish', authenticate, publishLimiter, upload.single('package'), async (req, res) => {
    const db = req.app.locals.db;
    const userId = req.user.id;

    try {
        // التحقق من وجود الملف / Verify file exists
        if (!req.file) {
            return res.status(400).json({ error: 'لم يتم رفع ملف الحزمة / No package file uploaded' });
        }

        // تحليل البيانات الوصفية / Parse metadata
        let metadata;
        try {
            metadata = JSON.parse(req.body.metadata || '{}');
        } catch (e) {
            // محاولة قراءة sad.toml من الأرشيف
            return res.status(400).json({ error: 'بيانات وصفية غير صالحة / Invalid metadata' });
        }

        const { name, version: versionStr, description, description_ar, authors,
                license, homepage, repository, documentation, keywords, categories,
                dependencies, dev_dependencies, build_dependencies } = metadata;

        // التحقق من الصحة / Validation
        if (!name || !versionStr) {
            cleanupFile(req.file.path);
            return res.status(400).json({ error: 'الاسم والإصدار مطلوبان / Name and version required' });
        }

        if (!validatePackageName(name)) {
            cleanupFile(req.file.path);
            return res.status(400).json({ 
                error: 'اسم الحزمة غير صالح. يجب أن يحتوي على حروف عربية/إنجليزية وأرقام وشرطات فقط',
                error_en: 'Invalid package name. Must contain Arabic/English letters, numbers, and hyphens only'
            });
        }

        const parsedVersion = semver.valid(semver.coerce(versionStr));
        if (!parsedVersion) {
            cleanupFile(req.file.path);
            return res.status(400).json({ error: `إصدار غير صالح: ${versionStr} / Invalid version` });
        }

        // التحقق من الحزمة الموجودة أو إنشاء جديدة
        let pkg = db.prepare('SELECT * FROM packages WHERE name = ?').get(name);

        if (pkg) {
            // التحقق من الملكية / Check ownership
            const isOwner = db.prepare(`
                SELECT 1 FROM package_owners WHERE package_id = ? AND user_id = ?
            `).get(pkg.id, userId);

            if (!isOwner && pkg.owner_id !== userId) {
                cleanupFile(req.file.path);
                return res.status(403).json({ 
                    error: 'ليس لديك صلاحية النشر لهذه الحزمة',
                    error_en: 'You do not have permission to publish this package'
                });
            }

            // التحقق من عدم وجود الإصدار / Check version doesn't exist
            const existingVersion = db.prepare(`
                SELECT 1 FROM versions WHERE package_id = ? AND version = ?
            `).get(pkg.id, versionStr);

            if (existingVersion) {
                cleanupFile(req.file.path);
                return res.status(409).json({
                    error: `الإصدار ${versionStr} موجود بالفعل للحزمة ${name}`,
                    error_en: `Version ${versionStr} already exists for package ${name}`
                });
            }
        } else {
            // إنشاء حزمة جديدة / Create new package
            const result = db.prepare(`
                INSERT INTO packages (name, description, description_ar, homepage, repository,
                    documentation, license, keywords, categories, owner_id, latest_version)
                VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
            `).run(
                name,
                description || '',
                description_ar || '',
                homepage || '',
                repository || '',
                documentation || '',
                license || 'MIT',
                JSON.stringify(keywords || []),
                JSON.stringify(categories || []),
                userId,
                versionStr
            );

            pkg = { id: result.lastInsertRowid };

            // إضافة المالك / Add owner
            db.prepare(`
                INSERT INTO package_owners (package_id, user_id, role) VALUES (?, ?, 'owner')
            `).run(pkg.id, userId);
        }

        // حساب checksum / Calculate checksum
        const fileBuffer = fs.readFileSync(req.file.path);
        const checksum = crypto.createHash('sha256').update(fileBuffer).digest('hex');

        // نقل الملف إلى التخزين الدائم / Move file to permanent storage
        const storagePath = process.env.STORAGE_PATH || './storage/packages';
        const packageDir = path.join(storagePath, name, versionStr);
        if (!fs.existsSync(packageDir)) {
            fs.mkdirSync(packageDir, { recursive: true });
        }
        const archivePath = path.join(packageDir, `${name}-${versionStr}.tar.gz`);
        fs.renameSync(req.file.path, archivePath);

        // تحليل الإصدار / Parse version
        const sv = semver.parse(semver.coerce(versionStr));

        // إدخال الإصدار / Insert version
        db.prepare(`
            INSERT INTO versions (package_id, version, major, minor, patch, prerelease,
                description, dependencies, dev_dependencies, build_dependencies,
                authors, license, checksum_sha256, archive_path, archive_size, published_by)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
        `).run(
            pkg.id,
            versionStr,
            sv.major,
            sv.minor,
            sv.patch,
            sv.prerelease.join('.') || '',
            description || '',
            JSON.stringify(dependencies || {}),
            JSON.stringify(dev_dependencies || {}),
            JSON.stringify(build_dependencies || {}),
            JSON.stringify(authors || []),
            license || 'MIT',
            checksum,
            archivePath,
            fileBuffer.length,
            userId
        );

        // تحديث أحدث إصدار / Update latest version
        const allVersions = db.prepare(`
            SELECT version FROM versions WHERE package_id = ? AND is_yanked = 0
            ORDER BY major DESC, minor DESC, patch DESC LIMIT 1
        `).get(pkg.id);

        if (allVersions) {
            db.prepare('UPDATE packages SET latest_version = ?, updated_at = datetime("now") WHERE id = ?')
                .run(allVersions.version, pkg.id);
        }

        res.status(201).json({
            success: true,
            message: `✅ تم نشر ${name}@${versionStr} بنجاح`,
            message_en: `Successfully published ${name}@${versionStr}`,
            package: {
                name,
                version: versionStr,
                checksum,
                size: fileBuffer.length,
            }
        });

    } catch (error) {
        if (req.file) cleanupFile(req.file.path);
        console.error('خطأ في النشر:', error.message);
        res.status(500).json({ error: 'فشل النشر / Publish failed: ' + error.message });
    }
});

// ============================================================================
// DELETE /api/v1/packages/:name/:version/yank — سحب إصدار / Yank Version
// ============================================================================

router.delete('/:name/:version/yank', authenticate, (req, res) => {
    const { name, version } = req.params;
    const db = req.app.locals.db;
    const userId = req.user.id;

    try {
        const pkg = db.prepare('SELECT id FROM packages WHERE name = ?').get(name);
        if (!pkg) {
            return res.status(404).json({ error: 'الحزمة غير موجودة' });
        }

        // التحقق من الملكية
        const isOwner = db.prepare(`
            SELECT 1 FROM package_owners WHERE package_id = ? AND user_id = ?
        `).get(pkg.id, userId);
        const isAdmin = req.user.is_admin;

        if (!isOwner && !isAdmin) {
            return res.status(403).json({ error: 'ليس لديك صلاحية سحب هذا الإصدار' });
        }

        const result = db.prepare(`
            UPDATE versions SET is_yanked = 1 WHERE package_id = ? AND version = ?
        `).run(pkg.id, version);

        if (result.changes === 0) {
            return res.status(404).json({ error: 'الإصدار غير موجود' });
        }

        // تحديث أحدث إصدار
        const latest = db.prepare(`
            SELECT version FROM versions WHERE package_id = ? AND is_yanked = 0
            ORDER BY major DESC, minor DESC, patch DESC LIMIT 1
        `).get(pkg.id);

        db.prepare('UPDATE packages SET latest_version = ? WHERE id = ?')
            .run(latest ? latest.version : null, pkg.id);

        res.json({ 
            success: true, 
            message: `تم سحب ${name}@${version}`,
            message_en: `Yanked ${name}@${version}`
        });
    } catch (error) {
        console.error('خطأ في السحب:', error.message);
        res.status(500).json({ error: 'خطأ في الخادم' });
    }
});

// ============================================================================
// أدوات مساعدة / Utilities
// ============================================================================

function cleanupFile(filePath) {
    try {
        if (filePath && fs.existsSync(filePath)) {
            fs.unlinkSync(filePath);
        }
    } catch (e) {
        console.error('خطأ في حذف الملف المؤقت:', e.message);
    }
}

module.exports = router;
