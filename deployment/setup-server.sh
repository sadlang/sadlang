#!/bin/bash
# =============================================================================
# سكريبت إعداد السيرفر لاستضافة موقعي لغة ص
# يُشغَّل مرة واحدة على السيرفر لتجهيز البنية التحتية
# =============================================================================
set -euo pipefail

echo "╔══════════════════════════════════════════════════╗"
echo "║   إعداد سيرفر لغة ص — sad-lang.org + sila-hub.dev  ║"
echo "╚══════════════════════════════════════════════════╝"

# === 1. تحديث النظام وتثبيت الحزم ===
echo ""
echo "▶ [1/7] تحديث النظام وتثبيت الحزم المطلوبة..."
apt-get update -qq
apt-get install -y -qq nginx certbot python3-certbot-nginx

# === 2. إنشاء المجلدات ===
echo "▶ [2/7] إنشاء مجلدات المواقع..."
mkdir -p /opt/sad-lang-website
mkdir -p /opt/sad-registry
mkdir -p /var/www/certbot
mkdir -p /var/log/nginx

# === 3. نسخ إعدادات Nginx ===
echo "▶ [3/7] إعداد Nginx Virtual Hosts..."

# حذف الإعداد الافتراضي إذا وجد
rm -f /etc/nginx/sites-enabled/default

# نسخ إعدادات المواقع
cp /tmp/deploy/sila-hub.dev.conf /etc/nginx/sites-available/sila-hub.dev
cp /tmp/deploy/sad-lang.org.conf /etc/nginx/sites-available/sad-lang.org

# تفعيل المواقع
ln -sf /etc/nginx/sites-available/sila-hub.dev /etc/nginx/sites-enabled/
ln -sf /etc/nginx/sites-available/sad-lang.org /etc/nginx/sites-enabled/

# === 4. إعداد Nginx مؤقت بدون SSL (للحصول على الشهادات) ===
echo "▶ [4/7] إعداد Nginx مؤقت (HTTP فقط) للحصول على شهادات SSL..."

# إنشاء إعداد مؤقت بدون SSL
cat > /etc/nginx/sites-available/sila-hub.dev.tmp << 'TMPEOF'
server {
    listen 80;
    listen [::]:80;
    server_name sila-hub.dev www.sila-hub.dev;
    
    location /.well-known/acme-challenge/ {
        root /var/www/certbot;
        allow all;
    }
    
    location / {
        proxy_pass http://127.0.0.1:3000;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    }
}
TMPEOF

cat > /etc/nginx/sites-available/sad-lang.org.tmp << 'TMPEOF'
server {
    listen 80;
    listen [::]:80;
    server_name sad-lang.org www.sad-lang.org;
    
    location /.well-known/acme-challenge/ {
        root /var/www/certbot;
        allow all;
    }
    
    root /opt/sad-lang-website;
    index index.html;
    
    location / {
        try_files $uri $uri.html $uri/ /index.html;
    }
}
TMPEOF

# استخدام الإعدادات المؤقتة أولاً
ln -sf /etc/nginx/sites-available/sila-hub.dev.tmp /etc/nginx/sites-enabled/sila-hub.dev
ln -sf /etc/nginx/sites-available/sad-lang.org.tmp /etc/nginx/sites-enabled/sad-lang.org

# اختبار وإعادة تشغيل nginx
nginx -t && systemctl restart nginx
echo "   ✓ Nginx يعمل بنجاح (HTTP مؤقت)"

# === 5. استخراج موقع اللغة ===
echo "▶ [5/7] نشر موقع لغة ص..."
if [ -f /tmp/deploy/sad-lang-website.tar.gz ]; then
    rm -rf /opt/sad-lang-website/*
    tar -xzf /tmp/deploy/sad-lang-website.tar.gz -C /opt/sad-lang-website/
    chown -R www-data:www-data /opt/sad-lang-website/
    echo "   ✓ موقع اللغة منشور في /opt/sad-lang-website/"
else
    echo "   ⚠ ملف موقع اللغة غير موجود — سيُرفع لاحقاً"
fi

# === 6. التأكد من عمل خادم الوحدات ===
echo "▶ [6/7] التحقق من خادم الوحدات..."
if pm2 list 2>/dev/null | grep -q "sad-registry"; then
    echo "   ✓ خادم الوحدات يعمل عبر PM2"
else
    echo "   ⚠ خادم الوحدات غير مُشغّل — شغّله: cd /opt/sad-registry && pm2 start ecosystem.config.js"
fi

# === 7. الحصول على شهادات SSL ===
echo "▶ [7/7] الحصول على شهادات SSL عبر Let's Encrypt..."
echo ""
echo "   سيتم طلب شهادات لـ:"
echo "   - sila-hub.dev + www.sila-hub.dev"
echo "   - sad-lang.org + www.sad-lang.org"
echo ""

# الحصول على شهادات (بدون تفاعل)
CERT_SUCCESS=true

certbot certonly --webroot \
    -w /var/www/certbot \
    -d sila-hub.dev -d www.sila-hub.dev \
    --non-interactive --agree-tos \
    --email admin@sila-hub.dev \
    --no-eff-email \
    2>&1 || { echo "   ⚠ فشل الحصول على شهادة sila-hub.dev — تأكد من DNS"; CERT_SUCCESS=false; }

certbot certonly --webroot \
    -w /var/www/certbot \
    -d sad-lang.org -d www.sad-lang.org \
    --non-interactive --agree-tos \
    --email admin@sad-lang.org \
    --no-eff-email \
    2>&1 || { echo "   ⚠ فشل الحصول على شهادة sad-lang.org — تأكد من DNS"; CERT_SUCCESS=false; }

if [ "$CERT_SUCCESS" = true ]; then
    echo "   ✓ تم الحصول على شهادات SSL بنجاح"
    
    # التبديل إلى الإعدادات الكاملة مع SSL
    ln -sf /etc/nginx/sites-available/sila-hub.dev /etc/nginx/sites-enabled/sila-hub.dev
    ln -sf /etc/nginx/sites-available/sad-lang.org /etc/nginx/sites-enabled/sad-lang.org
    
    # حذف الإعدادات المؤقتة
    rm -f /etc/nginx/sites-available/sila-hub.dev.tmp
    rm -f /etc/nginx/sites-available/sad-lang.org.tmp
    
    # إعادة تشغيل nginx مع SSL
    nginx -t && systemctl reload nginx
    echo "   ✓ Nginx يعمل مع HTTPS"
    
    # تفعيل التجديد التلقائي
    systemctl enable certbot.timer 2>/dev/null || true
    echo "   ✓ التجديد التلقائي للشهادات مُفعّل"
else
    echo ""
    echo "   ⚠ بعض الشهادات لم تُصدر — الموقعان يعملان على HTTP"
    echo "   عند ضبط DNS بشكل صحيح، شغّل:"
    echo "   certbot --nginx -d sila-hub.dev -d www.sila-hub.dev"
    echo "   certbot --nginx -d sad-lang.org -d www.sad-lang.org"
    echo "   ثم: ln -sf /etc/nginx/sites-available/sila-hub.dev /etc/nginx/sites-enabled/"
    echo "   و: ln -sf /etc/nginx/sites-available/sad-lang.org /etc/nginx/sites-enabled/"
    echo "   و: nginx -t && systemctl reload nginx"
fi

# === النتيجة ===
echo ""
echo "╔══════════════════════════════════════════════════╗"
echo "║              ✓ الإعداد مكتمل!                    ║"
echo "╠══════════════════════════════════════════════════╣"
echo "║                                                  ║"
echo "║  🌐 sad-lang.org     → /opt/sad-lang-website/   ║"
echo "║  📦 sila-hub.dev     → Express:3000 (PM2)       ║"
echo "║                                                  ║"
echo "║  سجلات Nginx:                                    ║"
echo "║  /var/log/nginx/sad-lang.org.*.log               ║"
echo "║  /var/log/nginx/sila-hub.dev.*.log               ║"
echo "║                                                  ║"
echo "╚══════════════════════════════════════════════════╝"
