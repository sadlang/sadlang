// بسم الله الرحمن الرحيم
// =========================================================================
// إعداد PM2 — تشغيل خادم سجل الحزم كخدمة دائمة
// PM2 Ecosystem Config — Run registry server as a persistent service
// =========================================================================

module.exports = {
    apps: [{
        name: 'sad-registry',
        script: 'src/server.js',
        cwd: __dirname,
        node_args: '--experimental-sqlite',
        instances: 1,
        autorestart: true,
        watch: false,
        max_memory_restart: '512M',
        env: {
            NODE_ENV: 'production',
            PORT: 3000,
            HOST: '0.0.0.0',
        },
        // إعادة التشغيل عند التوقف المفاجئ
        exp_backoff_restart_delay: 100,
        // تسجيل السجلات
        error_file: './logs/error.log',
        out_file: './logs/output.log',
        log_date_format: 'YYYY-MM-DD HH:mm:ss Z',
        merge_logs: true,
    }],
};
