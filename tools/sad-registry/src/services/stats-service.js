// بسم الله الرحمن الرحيم
// =========================================================================
// خدمة الإحصائيات — تجميع وعرض إحصائيات المستودع
// Stats Service — Registry statistics aggregation
// =========================================================================

class StatsService {
    constructor(db) {
        this.db = db;
    }

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

    getPopular(limit = 10) {
        return this.db.prepare(`
            SELECT p.name, p.description_ar, p.description, 
                   p.total_downloads, p.latest_version,
                   u.username as author
            FROM packages p
            JOIN users u ON p.owner_id = u.id
            WHERE p.is_yanked = 0
            ORDER BY p.total_downloads DESC
            LIMIT ?
        `).all(limit);
    }

    getNewest(limit = 10) {
        return this.db.prepare(`
            SELECT p.name, p.description_ar, p.description, 
                   p.latest_version, p.created_at,
                   u.username as author
            FROM packages p
            JOIN users u ON p.owner_id = u.id
            WHERE p.is_yanked = 0
            ORDER BY p.created_at DESC
            LIMIT ?
        `).all(limit);
    }

    getRecentlyUpdated(limit = 10) {
        return this.db.prepare(`
            SELECT p.name, p.latest_version, p.updated_at,
                   u.username as author
            FROM packages p
            JOIN users u ON p.owner_id = u.id
            WHERE p.is_yanked = 0
            ORDER BY p.updated_at DESC
            LIMIT ?
        `).all(limit);
    }

    getCategories() {
        return this.db.prepare(
            'SELECT * FROM categories ORDER BY name_ar'
        ).all();
    }

    getDailyDownloads(days = 30) {
        return this.db.prepare(`
            SELECT DATE(downloaded_at) as date, COUNT(*) as count
            FROM download_log
            WHERE downloaded_at >= datetime('now', '-' || ? || ' days')
            GROUP BY DATE(downloaded_at)
            ORDER BY date ASC
        `).all(days);
    }

    getFullStats() {
        return {
            success: true,
            summary: this.getSummary(),
            popular_packages: this.getPopular(),
            newest_packages: this.getNewest(),
            recently_updated: this.getRecentlyUpdated(),
            categories: this.getCategories(),
        };
    }
}

module.exports = StatsService;
