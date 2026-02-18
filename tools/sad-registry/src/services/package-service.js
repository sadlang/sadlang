// بسم الله الرحمن الرحيم
// =========================================================================
// خدمة الحزم — الطبقة المنطقية لعمليات إدارة الحزم
// Package Service — Business logic layer for package operations
// =========================================================================
//
// هذه الخدمة تفصل المنطق البرمجي عن مسارات Express (routes).
// الهدف: أن تكون المسارات رقيقة (thin controllers) وأن يكون المنطق
// كله هنا قابلاً لإعادة الاستخدام والاختبار المستقل.
//
// البنية المعمارية:
//   Route (مسار) → Service (خدمة) → Database (قاعدة بيانات)
//                                  → Storage (تخزين)
//
// الأسباب:
//   1. إعادة استخدام المنطق في مسارات API وواجهة الويب
//   2. اختبار المنطق بدون HTTP
//   3. تبديل قاعدة البيانات بسهولة مستقبلاً
// =========================================================================

const crypto = require('crypto');
const fs = require('fs');
const path = require('path');
const semver = require('semver');

// =========================================================================
// فئة خدمة الحزم / PackageService Class
// =========================================================================

class PackageService {
    /**
     * إنشاء خدمة الحزم
     * 
     * @param {import('better-sqlite3').Database} db — كائن قاعدة بيانات SQLite
     * @param {string} storagePath — مسار تخزين ملفات الحزم المضغوطة
     * 
     * @example
     *   const service = new PackageService(db, './storage/packages');
     */
    constructor(db, storagePath = './storage/packages') {
        this.db = db;
        this.storagePath = storagePath;

        // ---------------------------------------------------------------
        // تحضير الاستعلامات المتكررة (Prepared Statements)
        // هذا أسرع بكثير من إعادة التحضير في كل طلب
        // ---------------------------------------------------------------

        /** جلب حزمة بالاسم مع اسم المالك */
        this._getPackage = db.prepare(`
            SELECT p.*, u.username as author_name
            FROM packages p
            JOIN users u ON p.owner_id = u.id
            WHERE p.name = ?
        `);

        /** جلب إصدارات حزمة مرتبة تنازلياً */
        this._getVersions = db.prepare(`
            SELECT version, downloads, created_at, is_yanked
            FROM versions
            WHERE package_id = ?
            ORDER BY major DESC, minor DESC, patch DESC
        `);

        /** جلب مالكي حزمة */
        this._getOwners = db.prepare(`
            SELECT u.username, u.display_name, po.role
            FROM package_owners po
            JOIN users u ON po.user_id = u.id
            WHERE po.package_id = ?
        `);

        /** التحقق من ملكية حزمة */
        this._checkOwnership = db.prepare(`
            SELECT 1 FROM package_owners WHERE package_id = ? AND user_id = ?
        `);

        /** زيادة عداد التنزيلات */
        this._incrementDownloads = db.prepare(`
            UPDATE versions SET downloads = downloads + 1 WHERE id = ?
        `);

        this._incrementPackageDownloads = db.prepare(`
            UPDATE packages SET total_downloads = total_downloads + 1 
            WHERE id = (SELECT package_id FROM versions WHERE id = ?)
        `);

        /** تسجيل التنزيل في السجل */
        this._logDownload = db.prepare(`
            INSERT INTO download_log (version_id, ip_address, user_agent)
            VALUES (?, ?, ?)
        `);
    }

    // =====================================================================
    // البحث عن الحزم / Search Packages
    // =====================================================================
    //
    // يبحث في:
    //   - اسم الحزمة (name)
    //   - الوصف الإنجليزي (description)
    //   - الوصف العربي (description_ar)
    //   - الكلمات المفتاحية (keywords)
    //
    // يدعم تصفية اختيارية حسب التصنيف (category)
    //
    // @param {string} query — نص البحث
    // @param {Object} options — خيارات البحث
    // @param {number} options.limit — الحد الأقصى للنتائج (افتراضي: 20)
    // @param {number} options.offset — الإزاحة للتصفح (افتراضي: 0)
    // @param {string} options.category — تصنيف اختياري للتصفية
    // @returns {{ packages: Array, total: number }}
    // =====================================================================

    search(query = '', options = {}) {
        const { limit = 20, offset = 0, category = null } = options;

        // تحديد الحد الأقصى بـ 100 لمنع الإفراط في التحميل
        const limitNum = Math.min(parseInt(limit) || 20, 100);
        const offsetNum = parseInt(offset) || 0;
        const searchPattern = `%${query}%`;

        let sql, countSql, params, countParams;

        if (category) {
            // ---------------------------------------------------------
            // بحث مع تصفية تصنيف
            // البحث في التصنيفات يستخدم LIKE لأن التصنيفات مخزنة
            // كـ JSON array (مثلاً: '["math","data"]')
            // ---------------------------------------------------------
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
            const catPattern = `%"${category}"%`;
            params = [searchPattern, searchPattern, searchPattern, searchPattern, catPattern, limitNum, offsetNum];

            countSql = `
                SELECT COUNT(*) as total FROM packages 
                WHERE is_yanked = 0
                  AND (name LIKE ? OR description LIKE ? OR description_ar LIKE ? OR keywords LIKE ?)
                  AND categories LIKE ?
            `;
            countParams = [searchPattern, searchPattern, searchPattern, searchPattern, catPattern];
        } else {
            // ---------------------------------------------------------
            // بحث بدون تصنيف — في كل الحزم
            // ---------------------------------------------------------
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

        // تنسيق النتائج: تحويل JSON المخزن كنص إلى مصفوفة حقيقية
        return {
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

    // =====================================================================
    // جلب معلومات حزمة واحدة / Get Package Info
    // =====================================================================
    //
    // يرجع كل المعلومات عن حزمة بما في ذلك:
    //   - البيانات الوصفية (الاسم، الوصف، الرخصة...)
    //   - قائمة كل الإصدارات
    //   - قائمة المالكين
    //
    // @param {string} name — اسم الحزمة
    // @returns {Object|null} — كائن الحزمة أو null إذا غير موجودة
    // =====================================================================

    getPackageInfo(name) {
        const pkg = this._getPackage.get(name);
        if (!pkg) return null;

        const versions = this._getVersions.all(pkg.id);
        const owners = this._getOwners.all(pkg.id);

        return {
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
            owners: owners,
            versions: versions.map(v => ({
                version: v.version,
                downloads: v.downloads,
                created_at: v.created_at,
                is_yanked: !!v.is_yanked,
            })),
            created_at: pkg.created_at,
            updated_at: pkg.updated_at,
        };
    }

    // =====================================================================
    // جلب معلومات إصدار محدد / Get Version Info
    // =====================================================================
    //
    // @param {string} packageName — اسم الحزمة
    // @param {string} version — رقم الإصدار (مثلاً "1.2.0")
    // @returns {Object|null}
    // =====================================================================

    getVersionInfo(packageName, version) {
        const result = this.db.prepare(`
            SELECT v.*, p.name as package_name, p.repository, p.homepage
            FROM versions v
            JOIN packages p ON v.package_id = p.id
            WHERE p.name = ? AND v.version = ?
        `).get(packageName, version);

        if (!result) return null;

        return {
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
        };
    }

    // =====================================================================
    // تنزيل حزمة / Download Package
    // =====================================================================
    //
    // يتحقق من وجود الإصدار والملف، ثم يزيد عدادات التنزيل.
    //
    // لماذا نسجل كل تنزيل في download_log؟
    //   1. إحصائيات دقيقة (تنزيلات/يوم، /أسبوع)
    //   2. كشف الاستخدام المشبوه (bot scraping)
    //   3. تحليلات الاستخدام للمطورين
    //
    // @param {string} name — اسم الحزمة
    // @param {string} version — الإصدار
    // @param {string} ip — عنوان IP للتنزيل
    // @param {string} userAgent — متصفح/عميل التنزيل
    // @returns {{ filePath: string, checksum: string }|null}
    // =====================================================================

    recordDownload(name, version, ip = '', userAgent = '') {
        const result = this.db.prepare(`
            SELECT v.id, v.archive_path, v.checksum_sha256
            FROM versions v
            JOIN packages p ON v.package_id = p.id
            WHERE p.name = ? AND v.version = ? AND v.is_yanked = 0
        `).get(name, version);

        if (!result || !result.archive_path) return null;

        const filePath = path.resolve(result.archive_path);
        if (!fs.existsSync(filePath)) return null;

        // تحديث العدادات وتسجيل التنزيل (كعملية ذرية)
        const downloadTransaction = this.db.transaction(() => {
            this._incrementDownloads.run(result.id);
            this._incrementPackageDownloads.run(result.id);
            this._logDownload.run(result.id, ip, userAgent);
        });
        downloadTransaction();

        return {
            filePath,
            checksum: result.checksum_sha256,
        };
    }

    // =====================================================================
    // نشر حزمة / Publish Package
    // =====================================================================
    //
    // خطوات النشر:
    //   1. التحقق من صحة البيانات الوصفية (الاسم، الإصدار)
    //   2. التحقق من الملكية (إذا الحزمة موجودة)
    //   3. التحقق من عدم تكرار الإصدار
    //   4. حساب checksum SHA-256
    //   5. نقل الملف من المؤقت إلى التخزين الدائم
    //   6. إدخال السجلات في قاعدة البيانات
    //   7. تحديث أحدث إصدار
    //
    // @param {Object} metadata — البيانات الوصفية من sad.toml
    // @param {string} filePath — مسار الملف المرفوع (مؤقت)
    // @param {number} userId — معرف المستخدم الناشر
    // @returns {{ success: boolean, error?: string, package?: Object }}
    // =====================================================================

    publish(metadata, filePath, userId) {
        const {
            name, version: versionStr, description, description_ar,
            authors, license, homepage, repository, documentation,
            keywords, categories, dependencies, dev_dependencies,
            build_dependencies
        } = metadata;

        // ----- التحقق من الصحة -----
        if (!name || !versionStr) {
            return { success: false, error: 'الاسم والإصدار مطلوبان' };
        }

        // التحقق من الإصدار بنظام Semantic Versioning
        const parsedVersion = semver.valid(semver.coerce(versionStr));
        if (!parsedVersion) {
            return { success: false, error: `إصدار غير صالح: ${versionStr}` };
        }

        // ----- التحقق من الملكية -----
        let pkg = this.db.prepare('SELECT * FROM packages WHERE name = ?').get(name);

        if (pkg) {
            const isOwner = this._checkOwnership.get(pkg.id, userId);
            if (!isOwner && pkg.owner_id !== userId) {
                return { success: false, error: 'ليس لديك صلاحية النشر لهذه الحزمة', code: 403 };
            }

            // التحقق من عدم تكرار الإصدار
            const existingVer = this.db.prepare(
                'SELECT 1 FROM versions WHERE package_id = ? AND version = ?'
            ).get(pkg.id, versionStr);

            if (existingVer) {
                return { success: false, error: `الإصدار ${versionStr} موجود بالفعل`, code: 409 };
            }
        }

        // ----- حساب checksum وحجم الملف -----
        const fileBuffer = fs.readFileSync(filePath);
        const checksum = crypto.createHash('sha256').update(fileBuffer).digest('hex');

        // ----- نقل الملف إلى التخزين الدائم -----
        const packageDir = path.join(this.storagePath, name, versionStr);
        if (!fs.existsSync(packageDir)) {
            fs.mkdirSync(packageDir, { recursive: true });
        }
        const archivePath = path.join(packageDir, `${name}-${versionStr}.tar.gz`);
        fs.renameSync(filePath, archivePath);

        // ----- إدخال البيانات (عملية ذرية) -----
        const sv = semver.parse(semver.coerce(versionStr));

        const publishTransaction = this.db.transaction(() => {
            // إنشاء حزمة جديدة إذا لم تكن موجودة
            if (!pkg) {
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

                // تسجيل الملكية
                this.db.prepare(
                    'INSERT INTO package_owners (package_id, user_id, role) VALUES (?, ?, ?)'
                ).run(pkg.id, userId, 'owner');
            }

            // إدخال الإصدار
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
        });

        publishTransaction();

        return {
            success: true,
            package: {
                name,
                version: versionStr,
                checksum,
                size: fileBuffer.length,
            }
        };
    }

    // =====================================================================
    // أدوات مساعدة خاصة / Private Helpers
    // =====================================================================

    /**
     * تحليل JSON بأمان — يرجع القيمة الافتراضية عند الفشل
     * Safe JSON parse — returns default value on failure
     * 
     * لماذا نحتاج هذا؟ لأن بعض الحقول في قاعدة البيانات مخزنة كنص JSON
     * وقد تكون فارغة أو تالفة. هذا يمنع انهيار الخادم.
     *
     * @param {string} str — النص المراد تحليله
     * @param {*} defaultValue — القيمة عند الفشل
     * @returns {*} الكائن المحلل أو القيمة الافتراضية
     */
    _parseJSON(str, defaultValue) {
        try {
            return JSON.parse(str || JSON.stringify(defaultValue));
        } catch {
            return defaultValue;
        }
    }
}

module.exports = PackageService;
