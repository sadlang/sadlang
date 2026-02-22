// بسم الله الرحمن الرحيم
// =========================================================================
// مسارات الويب — صفحات HTML للمتصفح
// Web Routes — Browser HTML pages
// =========================================================================

const { Router } = require('express');

function createWebRouter(db, services) {
    const router = Router();
    const { packageService, statsService } = services;

    // ═══════════════════════════════════════════════════════════════
    // GET / — الصفحة الرئيسية
    // ═══════════════════════════════════════════════════════════════
    router.get('/', (req, res) => {
        try {
            const rawStats = statsService.getSummary();
            const stats = {
                packages: rawStats.total_packages || 0,
                downloads: rawStats.total_downloads || 0,
                users: rawStats.total_users || 0,
                versions: rawStats.total_versions || 0,
            };
            const popular = statsService.getPopular(6);
            const newest = statsService.getNewest(6);
            const categories = statsService.getCategories();

            res.render('index', {
                title: 'سجل حزم لغة ص',
                stats,
                popular,
                newest,
                categories,
            });
        } catch (err) {
            console.error('خطأ في الصفحة الرئيسية:', err.message);
            res.render('error', {
                title: 'خطأ',
                message: 'حدث خطأ أثناء تحميل الصفحة الرئيسية',
            });
        }
    });

    // ═══════════════════════════════════════════════════════════════
    // GET /search — صفحة البحث
    // ═══════════════════════════════════════════════════════════════
    router.get('/search', (req, res) => {
        try {
            const q = req.query.q || '';
            const result = q ? packageService.search(q, { limit: 50 }) : { packages: [], total: 0 };

            res.render('search', {
                title: `بحث: ${q || 'الحزم'}`,
                query: q,
                packages: result.packages || [],
                total: result.total || 0,
            });
        } catch (err) {
            console.error('خطأ في البحث:', err.message);
            res.render('error', {
                title: 'خطأ',
                message: 'حدث خطأ أثناء البحث',
            });
        }
    });

    // ═══════════════════════════════════════════════════════════════
    // GET /packages/:name — صفحة تفاصيل الحزمة
    // ═══════════════════════════════════════════════════════════════
    router.get('/packages/:name', (req, res) => {
        try {
            const result = packageService.getPackageInfo(req.params.name);

            if (!result.success) {
                return res.status(404).render('404', {
                    title: 'الحزمة غير موجودة',
                    path: req.path,
                });
            }

            res.render('package', {
                title: result.package.name,
                pkg: result.package,
            });
        } catch (err) {
            console.error('خطأ في صفحة الحزمة:', err.message);
            res.render('error', {
                title: 'خطأ',
                message: 'حدث خطأ أثناء تحميل بيانات الحزمة',
            });
        }
    });

    return router;
}

module.exports = createWebRouter;
