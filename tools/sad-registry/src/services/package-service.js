// بسم الله الرحمن الرحيم
// =========================================================================
// خدمة الحزم — العمليات الكاملة لإدارة الحزم
// Package Service — Full package management operations
// =========================================================================

const crypto = require('crypto');
const fs = require('fs');
const path = require('path');
const semver = require('semver');
const { createError } = require('../utils/error-codes');

class PackageService {
    constructor(db, storagePath = './storage/packages') {
        this.db = db;
        this.storagePath = storagePath;

        // إنشاء مجلد التخزين إذا لم يكن موجوداً
        if (!fs.existsSync(this.storagePath)) {
            fs.mkdirSync(this.storagePath, { recursive: true });
        }

        // الاستعلامات المُحضّرة
        this._getPackage = db.prepare(`
            SELECT p.*, u.username as author_name
            FROM packages p
            JOIN users u ON p.owner_id = u.id
            WHERE p.name = ?
        `);

        this._getVersions = db.prepare(`
            SELECT version, downloads, created_at, is_yanked
            FROM versions
            WHERE package_id = ?
            ORDER BY major DESC, minor DESC, patch DESC
        `);

        this._getOwners = db.prepare(`
            SELECT u.username, u.display_name, po.role
            FROM package_owners po
            JOIN users u ON po.user_id = u.id
            WHERE po.package_id = ?
        `);

        this._checkOwnership = db.prepare(
            'SELECT 1 FROM package_owners WHERE package_id = ? AND user_id = ?'
        );

        this._incrementDownloads = db.prepare(
            'UPDATE versions SET downloads = downloads + 1 WHERE id = ?'
        );

        this._incrementPackageDownloads = db.prepare(`
            UPDATE packages SET total_downloads = total_downloads + 1 
            WHERE id = (SELECT package_id FROM versions WHERE id = ?)
        `);

        this._logDownload = db.prepare(
            'INSERT INTO download_log (version_id, ip_address, user_agent) VALUES (?, ?, ?)'
        );
    }

    // ─────────────────────────────────────────────────────────────
    // البحث
    // ─────────────────────────────────────────────────────────────
    search(query = '', options = {}) {
        const { limit = 20, offset = 0, category = null } = options;
        const limitNum = Math.min(parseInt(limit) || 20, 100);
        const offsetNum = parseInt(offset) || 0;
        const searchPattern = `%${query}%`;

        let sql, countSql, params, countParams;

        if (category) {
            const catPattern = `%"${category}"%`;
            sql = `
                SELECT p.name, p.description, p.description_ar, p.latest_version,
                       p.total_downloads, p.license, p.keywords, p.created_at,
                       u.username as author
                FROM packages p
                JOIN users u ON p.owner_id = u.id
                WHERE p.is_yanked = 0
                  AND (p.name LIKE ? OR p.description LIKE ? 
                       OR p.description_ar LIKE ? OR p.keywords LIKE ?)
                  AND p.categories LIKE ?
                ORDER BY p.total_downloads DESC
                LIMIT ? OFFSET ?
            `;
            params = [searchPattern, searchPattern, searchPattern, searchPattern, catPattern, limitNum, offsetNum];

            countSql = `
                SELECT COUNT(*) as total FROM packages 
                WHERE is_yanked = 0
                  AND (name LIKE ? OR description LIKE ? OR description_ar LIKE ? OR keywords LIKE ?)
                  AND categories LIKE ?
            `;
            countParams = [searchPattern, searchPattern, searchPattern, searchPattern, catPattern];
        } else {
            sql = `
                SELECT p.name, p.description, p.description_ar, p.latest_version,
                       p.total_downloads, p.license, p.keywords, p.created_at,
                       u.username as author
                FROM packages p
                JOIN users u ON p.owner_id = u.id
                WHERE p.is_yanked = 0
                  AND (p.name LIKE ? OR p.description LIKE ? 
                       OR p.description_ar LIKE ? OR p.keywords LIKE ?)
                ORDER BY p.total_downloads DESC
                LIMIT ? OFFSET ?
            `;
            params = [searchPattern, searchPattern, searchPattern, searchPattern, limitNum, offsetNum];

            countSql = `
                SELECT COUNT(*) as total FROM packages 
                WHERE is_yanked = 0
                  AND (name LIKE ? OR description LIKE ? OR description_ar LIKE ? OR keywords LIKE ?)
            `;
            countParams = [searchPattern, searchPattern, searchPattern, searchPattern];
        }

        const packages = this.db.prepare(sql).all(...params);
        const { total } = this.db.prepare(countSql).get(...countParams);

        return {
            success: true,
            packages: packages.map(p => ({
                name: p.name,
                description: p.description,
                description_ar: p.description_ar,
                latest_version: p.latest_version,
                downloads: p.total_downloads,
                license: p.license,
                keywords: this._parseJSON(p.keywords, []),
                author: p.author,
                created_at: p.created_at,
            })),
            total,
            limit: limitNum,
            offset: offsetNum,
        };
    }

    // ─────────────────────────────────────────────────────────────
    // معلومات الحزمة
    // ─────────────────────────────────────────────────────────────
    getPackageInfo(name) {
        const pkg = this._getPackage.get(name);
        if (!pkg) {
            return createError('PKG_001', {
                hint: `لم يتم العثور على حزمة باسم "${name}". ابحث عن الحزم المتاحة: sad-pkg search`,
            });
        }

        const versions = this._getVersions.all(pkg.id);
        const owners = this._getOwners.all(pkg.id);

        return {
            success: true,
            package: {
                name: pkg.name,
                description: pkg.description,
                description_ar: pkg.description_ar,
                homepage: pkg.homepage,
                repository: pkg.repository,
                documentation: pkg.documentation,
                license: pkg.license,
                keywords: this._parseJSON(pkg.keywords, []),
                categories: this._parseJSON(pkg.categories, []),
                latest_version: pkg.latest_version,
                total_downloads: pkg.total_downloads,
                owners,
                versions: versions.map(v => ({
                    version: v.version,
                    downloads: v.downloads,
                    created_at: v.created_at,
                    is_yanked: !!v.is_yanked,
                })),
                created_at: pkg.created_at,
                updated_at: pkg.updated_at,
            },
        };
    }

    // ─────────────────────────────────────────────────────────────
    // معلومات إصدار محدد
    // ─────────────────────────────────────────────────────────────
    getVersionInfo(packageName, version) {
        const result = this.db.prepare(`
            SELECT v.*, p.name as package_name, p.repository, p.homepage
            FROM versions v
            JOIN packages p ON v.package_id = p.id
            WHERE p.name = ? AND v.version = ?
        `).get(packageName, version);

        if (!result) {
            // فحص: هل الحزمة موجودة أصلاً؟
            const pkgExists = this.db.prepare('SELECT id FROM packages WHERE name = ?').get(packageName);
            if (!pkgExists) {
                return createError('PKG_001', {
                    hint: `لم يتم العثور على حزمة باسم "${packageName}"`,
                });
            }
            return createError('PKG_002', {
                hint: `الإصدار "${version}" غير موجود للحزمة "${packageName}". اعرض الإصدارات: sad-pkg versions ${packageName}`,
            });
        }

        return {
            success: true,
            version: {
                name: result.package_name,
                version: result.version,
                description: result.description,
                authors: this._parseJSON(result.authors, []),
                license: result.license,
                repository: result.repository,
                homepage: result.homepage,
                dependencies: this._parseJSON(result.dependencies, {}),
                dev_dependencies: this._parseJSON(result.dev_dependencies, {}),
                build_dependencies: this._parseJSON(result.build_dependencies, {}),
                checksum: result.checksum_sha256,
                archive_size: result.archive_size,
                downloads: result.downloads,
                is_yanked: !!result.is_yanked,
                created_at: result.created_at,
            },
        };
    }

    // ─────────────────────────────────────────────────────────────
    // قائمة الإصدارات
    // ─────────────────────────────────────────────────────────────
    getVersionsList(packageName) {
        const pkg = this.db.prepare('SELECT id FROM packages WHERE name = ?').get(packageName);
        if (!pkg) {
            return createError('PKG_001', {
                hint: `لم يتم العثور على حزمة باسم "${packageName}"`,
            });
        }

        const versions = this._getVersions.all(pkg.id);
        return {
            success: true,
            name: packageName,
            versions: versions.map(v => ({
                version: v.version,
                downloads: v.downloads,
                created_at: v.created_at,
                is_yanked: !!v.is_yanked,
            })),
        };
    }

    // ─────────────────────────────────────────────────────────────
    // تنزيل حزمة
    // ─────────────────────────────────────────────────────────────
    download(name, version, ip = '', userAgent = '') {
        const result = this.db.prepare(`
            SELECT v.id, v.archive_path, v.checksum_sha256, v.is_yanked,
                   p.name as package_name
            FROM versions v
            JOIN packages p ON v.package_id = p.id
            WHERE p.name = ? AND v.version = ?
        `).get(name, version);

        if (!result) {
            const pkgExists = this.db.prepare('SELECT id FROM packages WHERE name = ?').get(name);
            if (!pkgExists) {
                return createError('PKG_001');
            }
            return createError('PKG_002', {
                hint: `الإصدار "${version}" غير موجود. اعرض الإصدارات: sad-pkg versions ${name}`,
            });
        }

        if (result.is_yanked) {
            return createError('PKG_007', {
                hint: `الإصدار "${version}" من "${name}" مسحوب. استخدم إصداراً آخر: sad-pkg versions ${name}`,
            });
        }

        if (!result.archive_path) {
            return createError('PKG_008');
        }

        const filePath = path.resolve(result.archive_path);
        if (!fs.existsSync(filePath)) {
            return createError('PKG_008', {
                explain: `ملف الأرشيف مسجل في قاعدة البيانات لكنه مفقود من نظام الملفات: ${result.archive_path}`,
            });
        }

        // تسجيل التنزيل
        this.db.exec('BEGIN');
        try {
            this._incrementDownloads.run(result.id);
            this._incrementPackageDownloads.run(result.id);
            this._logDownload.run(result.id, ip, userAgent);
            this.db.exec('COMMIT');
        } catch (downloadErr) {
            this.db.exec('ROLLBACK');
            // لا نوقف التنزيل بسبب خطأ في التسجيل
            console.error('خطأ في تسجيل التنزيل:', downloadErr.message);
        }

        return {
            success: true,
            filePath,
            checksum: result.checksum_sha256,
        };
    }

    // ─────────────────────────────────────────────────────────────
    // نشر حزمة
    // ─────────────────────────────────────────────────────────────
    publish(metadata, filePath, userId) {
        const {
            name, version: versionStr, description, description_ar,
            authors, license, homepage, repository, documentation,
            keywords, categories, dependencies, dev_dependencies,
            build_dependencies,
        } = metadata;

        // التحقق من الاسم
        if (!name) {
            return createError('PUB_002', {
                hint: 'حقل "name" مفقود في البيانات الوصفية',
                details: { missing: 'name' },
            });
        }

        // التحقق من الإصدار
        if (!versionStr) {
            return createError('PUB_002', {
                hint: 'حقل "version" مفقود في البيانات الوصفية',
                details: { missing: 'version' },
            });
        }

        const parsedVersion = semver.valid(semver.coerce(versionStr));
        if (!parsedVersion) {
            return createError('PKG_006', {
                hint: `الإصدار "${versionStr}" غير صالح. أمثلة صحيحة: 1.0.0, 2.1.3-beta`,
            });
        }

        let pkg = this.db.prepare('SELECT * FROM packages WHERE name = ?').get(name);

        if (pkg) {
            // التحقق من الملكية
            const isOwner = this._checkOwnership.get(pkg.id, userId);
            if (!isOwner && pkg.owner_id !== userId) {
                return createError('PKG_004', {
                    hint: `الحزمة "${name}" مملوكة لمستخدم آخر. إذا كنت تعتقد أن هذا خطأ، تواصل مع المالك`,
                });
            }

            // التحقق من تكرار الإصدار
            const existingVer = this.db.prepare(
                'SELECT 1 FROM versions WHERE package_id = ? AND version = ?'
            ).get(pkg.id, versionStr);

            if (existingVer) {
                return createError('PKG_003', {
                    hint: `الإصدار "${versionStr}" موجود بالفعل للحزمة "${name}". زِد الرقم في sad.json`,
                });
            }
        }

        // حساب checksum
        const fileBuffer = fs.readFileSync(filePath);
        const checksum = crypto.createHash('sha256').update(fileBuffer).digest('hex');

        // حفظ الملف
        const packageDir = path.join(this.storagePath, name, versionStr);
        if (!fs.existsSync(packageDir)) {
            fs.mkdirSync(packageDir, { recursive: true });
        }
        const archivePath = path.join(packageDir, `${name}-${versionStr}.tar.gz`);
        fs.renameSync(filePath, archivePath);

        // تحليل SemVer
        const sv = semver.parse(semver.coerce(versionStr));

        // معاملة قاعدة البيانات
        this.db.exec('BEGIN');
        try {
            if (!pkg) {
                // حزمة جديدة
                const result = this.db.prepare(`
                    INSERT INTO packages (name, description, description_ar, homepage, repository,
                        documentation, license, keywords, categories, owner_id, latest_version)
                    VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
                `).run(
                    name, description || '', description_ar || '',
                    homepage || '', repository || '', documentation || '',
                    license || 'MIT',
                    JSON.stringify(keywords || []),
                    JSON.stringify(categories || []),
                    userId, versionStr
                );

                pkg = { id: result.lastInsertRowid };

                // إضافة المالك
                this.db.prepare(
                    'INSERT INTO package_owners (package_id, user_id, role) VALUES (?, ?, ?)'
                ).run(pkg.id, userId, 'owner');
            }

            // إضافة الإصدار
            this.db.prepare(`
                INSERT INTO versions (package_id, version, major, minor, patch, prerelease,
                    description, dependencies, dev_dependencies, build_dependencies,
                    authors, license, checksum_sha256, archive_path, archive_size, published_by)
                VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
            `).run(
                pkg.id, versionStr,
                sv.major, sv.minor, sv.patch,
                sv.prerelease.join('.') || '',
                description || '',
                JSON.stringify(dependencies || {}),
                JSON.stringify(dev_dependencies || {}),
                JSON.stringify(build_dependencies || {}),
                JSON.stringify(authors || []),
                license || 'MIT',
                checksum, archivePath, fileBuffer.length, userId
            );

            // تحديث أحدث إصدار
            const latest = this.db.prepare(`
                SELECT version FROM versions WHERE package_id = ? AND is_yanked = 0
                ORDER BY major DESC, minor DESC, patch DESC LIMIT 1
            `).get(pkg.id);

            if (latest) {
                this.db.prepare(
                    'UPDATE packages SET latest_version = ?, updated_at = datetime("now") WHERE id = ?'
                ).run(latest.version, pkg.id);
            }

            this.db.exec('COMMIT');
        } catch (publishErr) {
            this.db.exec('ROLLBACK');
            // حذف الملف المحفوظ في حالة الفشل
            try { fs.unlinkSync(archivePath); } catch {}
            throw publishErr;
        }

        return {
            success: true,
            message: `تم نشر الحزمة "${name}" بنجاح (الإصدار ${versionStr})`,
            message_en: `Package "${name}" published successfully (version ${versionStr})`,
            package: {
                name,
                version: versionStr,
                checksum,
                size: fileBuffer.length,
            },
        };
    }

    // ─────────────────────────────────────────────────────────────
    // سحب إصدار (yank)
    // ─────────────────────────────────────────────────────────────
    yank(packageName, version, userId) {
        const pkg = this.db.prepare('SELECT * FROM packages WHERE name = ?').get(packageName);
        if (!pkg) {
            return createError('PKG_001');
        }

        // التحقق من الملكية
        const isOwner = this._checkOwnership.get(pkg.id, userId);
        if (!isOwner && pkg.owner_id !== userId) {
            return createError('PKG_004');
        }

        const ver = this.db.prepare(
            'SELECT id, is_yanked FROM versions WHERE package_id = ? AND version = ?'
        ).get(pkg.id, version);

        if (!ver) {
            return createError('PKG_002', {
                hint: `الإصدار "${version}" غير موجود للحزمة "${packageName}"`,
            });
        }

        if (ver.is_yanked) {
            return {
                success: true,
                message: 'هذا الإصدار مسحوب بالفعل',
                message_en: 'This version is already yanked',
            };
        }

        this.db.prepare('UPDATE versions SET is_yanked = 1 WHERE id = ?').run(ver.id);

        // تحديث أحدث إصدار
        const latest = this.db.prepare(`
            SELECT version FROM versions WHERE package_id = ? AND is_yanked = 0
            ORDER BY major DESC, minor DESC, patch DESC LIMIT 1
        `).get(pkg.id);

        if (latest) {
            this.db.prepare(
                'UPDATE packages SET latest_version = ?, updated_at = datetime("now") WHERE id = ?'
            ).run(latest.version, pkg.id);
        } else {
            this.db.prepare(
                'UPDATE packages SET latest_version = "", is_yanked = 1, updated_at = datetime("now") WHERE id = ?'
            ).run(pkg.id);
        }

        return {
            success: true,
            message: `تم سحب الإصدار ${version} من الحزمة "${packageName}"`,
            message_en: `Version ${version} of "${packageName}" has been yanked`,
        };
    }

    // ─────────────────────────────────────────────────────────────
    // أدوات مساعدة
    // ─────────────────────────────────────────────────────────────
    _parseJSON(str, defaultValue) {
        try {
            return JSON.parse(str || JSON.stringify(defaultValue));
        } catch {
            return defaultValue;
        }
    }
}

module.exports = PackageService;
