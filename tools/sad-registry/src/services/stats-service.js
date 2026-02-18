// بسم الله الرحمن الرحيم
// =========================================================================
// خدمة الإحصائيات — تجميع وعرض إحصائيات المستودع
// Stats Service — Aggregate and display registry statistics
// =========================================================================
//
// هذه الخدمة توفر:
//   1. إحصائيات عامة: عدد الحزم، الإصدارات، المستخدمين، التنزيلات
//   2. الحزم الأكثر شعبية (الأكثر تنزيلاً)
//   3. أحدث الحزم المضافة
//   4. أحدث الحزم المحدّثة
//   5. قائمة التصنيفات
//   6. إحصائيات تنزيلات يومية (لوحة معلومات)
//
// تُستخدم في:
//   - نقطة نهاية GET /api/v1/stats
//   - الصفحة الرئيسية للويب (الإحصائيات في قسم البطل)
// =========================================================================

class StatsService {
    /**
     * إنشاء خدمة الإحصائيات
     * 
     * @param {import('better-sqlite3').Database} db — كائن قاعدة بيانات SQLite
     */
    constructor(db) {
        this.db = db;
    }

    // =====================================================================
    // الإحصائيات الملخصة / Summary Statistics
    // =====================================================================
    //
    // ترجع الأرقام الأساسية الأربعة:
    //   - total_packages: عدد الحزم الفعالة (غير المسحوبة)
    //   - total_versions: عدد الإصدارات الفعالة
    //   - total_users: عدد المستخدمين النشطين
    //   - total_downloads: مجموع التنزيلات لكل الحزم
    //
    // هذه الأرقام تظهر في:
    //   - قسم البطل في الصفحة الرئيسية
    //   - نقطة نهاية /api/v1/stats
    // =====================================================================

    getSummary() {
        return {
            total_packages: this.db.prepare(
                'SELECT COUNT(*) as c FROM packages WHERE is_yanked = 0'
            ).get().c,

            total_versions: this.db.prepare(
                'SELECT COUNT(*) as c FROM versions WHERE is_yanked = 0'
            ).get().c,

            total_users: this.db.prepare(
                'SELECT COUNT(*) as c FROM users WHERE is_active = 1'
            ).get().c,

            total_downloads: this.db.prepare(
                'SELECT COALESCE(SUM(total_downloads), 0) as c FROM packages'
            ).get().c,
        };
    }

    // =====================================================================
    // الحزم الأكثر شعبية / Most Popular Packages
    // =====================================================================
    //
    // مرتبة تنازلياً حسب عدد التنزيلات الكلي.
    //
    // @param {number} limit — عدد النتائج (افتراضي: 10)
    // @returns {Array} مصفوفة الحزم الأكثر تنزيلاً
    // =====================================================================

    getPopular(limit = 10) {
        return this.db.prepare(`
            SELECT p.name, p.description_ar, p.description, 
                   p.total_downloads, p.latest_version
            FROM packages p
            WHERE p.is_yanked = 0
            ORDER BY p.total_downloads DESC
            LIMIT ?
        `).all(limit);
    }

    // =====================================================================
    // أحدث الحزم / Newest Packages
    // =====================================================================
    //
    // مرتبة حسب تاريخ الإنشاء (الأحدث أولاً).
    // مفيدة لاكتشاف الحزم الجديدة.
    //
    // @param {number} limit — عدد النتائج (افتراضي: 10)
    // @returns {Array} مصفوفة أحدث الحزم
    // =====================================================================

    getNewest(limit = 10) {
        return this.db.prepare(`
            SELECT p.name, p.description_ar, p.description, 
                   p.latest_version, p.created_at
            FROM packages p
            WHERE p.is_yanked = 0
            ORDER BY p.created_at DESC
            LIMIT ?
        `).all(limit);
    }

    // =====================================================================
    // أحدث التحديثات / Recently Updated
    // =====================================================================
    //
    // مرتبة حسب تاريخ آخر تحديث (نشر إصدار جديد).
    // مفيدة لمعرفة الحزم النشطة التي تتلقى تحديثات.
    //
    // @param {number} limit — عدد النتائج (افتراضي: 10)
    // @returns {Array}
    // =====================================================================

    getRecentlyUpdated(limit = 10) {
        return this.db.prepare(`
            SELECT p.name, p.latest_version, p.updated_at
            FROM packages p
            WHERE p.is_yanked = 0
            ORDER BY p.updated_at DESC
            LIMIT ?
        `).all(limit);
    }

    // =====================================================================
    // التصنيفات / Categories
    // =====================================================================
    //
    // جلب كل التصنيفات مع عدد حزم كل تصنيف.
    // مرتبة أبجدياً بالعربية.
    //
    // @returns {Array} مصفوفة التصنيفات
    // =====================================================================

    getCategories() {
        return this.db.prepare(
            'SELECT * FROM categories ORDER BY name_ar'
        ).all();
    }

    // =====================================================================
    // إحصائيات التنزيلات اليومية / Daily Download Stats
    // =====================================================================
    //
    // تُحسب من جدول download_log لآخر N يوم.
    // مفيدة لرسم مخطط التنزيلات في لوحة المعلومات.
    //
    // @param {number} days — عدد الأيام (افتراضي: 30)
    // @returns {Array<{ date: string, count: number }>}
    // =====================================================================

    getDailyDownloads(days = 30) {
        return this.db.prepare(`
            SELECT DATE(downloaded_at) as date, COUNT(*) as count
            FROM download_log
            WHERE downloaded_at >= datetime('now', '-' || ? || ' days')
            GROUP BY DATE(downloaded_at)
            ORDER BY date ASC
        `).all(days);
    }

    // =====================================================================
    // الإحصائيات الكاملة / Full Stats
    // =====================================================================
    //
    // تجمع كل الإحصائيات في كائن واحد.
    // تُستخدم في نقطة نهاية GET /api/v1/stats
    //
    // @returns {Object} كل الإحصائيات مجمعة
    // =====================================================================

    getFullStats() {
        return {
            summary: this.getSummary(),
            popular_packages: this.getPopular(),
            newest_packages: this.getNewest(),
            recently_updated: this.getRecentlyUpdated(),
            categories: this.getCategories(),
        };
    }
}

module.exports = StatsService;
