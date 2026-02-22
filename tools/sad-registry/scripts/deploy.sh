#!/bin/bash
# بسم الله الرحمن الرحيم
# =========================================================================
# سكريبت نشر خادم سجل حزم لغة ص
# Sad Package Registry — Deployment Script
# =========================================================================

set -e

echo "╔══════════════════════════════════════════════════════════════╗"
echo "║  نشر خادم سجل حزم لغة ص                                   ║"
echo "║  Sad Package Registry Deployment                           ║"
echo "╚══════════════════════════════════════════════════════════════╝"
echo ""

APP_DIR="/opt/sad-registry"
NODE_VERSION="22"  # LTS version

# ─── 1. تحديث النظام ───
echo "▸ تحديث النظام..."
apt-get update -qq
apt-get upgrade -y -qq

# ─── 2. تثبيت Node.js ───
if ! command -v node &> /dev/null; then
    echo "▸ تثبيت Node.js ${NODE_VERSION}..."
    curl -fsSL https://deb.nodesource.com/setup_${NODE_VERSION}.x | bash -
    apt-get install -y -qq nodejs
else
    echo "✓ Node.js موجود: $(node --version)"
fi

# ─── 3. تثبيت PM2 ───
if ! command -v pm2 &> /dev/null; then
    echo "▸ تثبيت PM2..."
    npm install -g pm2
else
    echo "✓ PM2 موجود: $(pm2 --version)"
fi

# ─── 4. إنشاء مجلد التطبيق ───
echo "▸ إعداد مجلد التطبيق ${APP_DIR}..."
mkdir -p ${APP_DIR}
mkdir -p ${APP_DIR}/data
mkdir -p ${APP_DIR}/storage/packages
mkdir -p ${APP_DIR}/logs

# ─── 5. نسخ الملفات ───
echo "▸ نسخ ملفات التطبيق..."
# (يتم تشغيل هذا بعد نسخ الملفات عبر SCP)

# ─── 6. تثبيت التبعيات ───
echo "▸ تثبيت التبعيات..."
cd ${APP_DIR}
npm install --production

# ─── 7. إعداد ملف البيئة ───
if [ ! -f ${APP_DIR}/.env ]; then
    echo "▸ إنشاء ملف .env..."
    cat > ${APP_DIR}/.env << 'EOF'
PORT=3000
HOST=0.0.0.0
DB_PATH=./data/registry.db
STORAGE_PATH=./storage/packages
JWT_SECRET=$(openssl rand -hex 32)
NODE_ENV=production
REGISTRY_URL=http://185.47.174.39:3000
EOF
    # Generate actual JWT secret
    JWT_SECRET=$(openssl rand -hex 32)
    sed -i "s/\$(openssl rand -hex 32)/${JWT_SECRET}/" ${APP_DIR}/.env
    echo "✓ تم إنشاء .env مع مفتاح JWT عشوائي"
else
    echo "✓ ملف .env موجود"
fi

# ─── 8. فتح المنفذ في جدار الحماية ───
echo "▸ فتح المنفذ 3000..."
if command -v ufw &> /dev/null; then
    ufw allow 3000/tcp
    echo "✓ تم فتح المنفذ في UFW"
fi

# ─── 9. تشغيل الخادم عبر PM2 ───
echo "▸ تشغيل الخادم..."
cd ${APP_DIR}
pm2 delete sad-registry 2>/dev/null || true
pm2 start ecosystem.config.js
pm2 save

# ─── 10. إعداد التشغيل التلقائي عند إعادة تشغيل النظام ───
echo "▸ إعداد التشغيل التلقائي..."
pm2 startup systemd -u root --hp /root 2>/dev/null || true
pm2 save

echo ""
echo "╔══════════════════════════════════════════════════════════════╗"
echo "║  ✅ تم النشر بنجاح!                                       ║"
echo "║                                                            ║"
echo "║  العنوان:  http://185.47.174.39:3000                      ║"
echo "║  API:      http://185.47.174.39:3000/api/v1/              ║"
echo "║  الصحة:    http://185.47.174.39:3000/api/v1/health        ║"
echo "║                                                            ║"
echo "║  أوامر مفيدة:                                              ║"
echo "║    pm2 logs sad-registry    ← عرض السجلات                  ║"
echo "║    pm2 restart sad-registry ← إعادة التشغيل               ║"
echo "║    pm2 status               ← حالة الخادم                  ║"
echo "╚══════════════════════════════════════════════════════════════╝"
