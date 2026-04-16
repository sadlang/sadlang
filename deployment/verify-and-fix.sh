#!/bin/bash
# =============================================================================
# سكريبت فحص وإصلاح — يُشغَّل على السيرفر عبر SSH واحد
# ssh root@185.47.174.39 "bash /tmp/deploy/verify-and-fix.sh"
# =============================================================================
set -euo pipefail

echo "╔═══════════════════════════════════════╗"
echo "║     فحص وإصلاح سيرفر لغة ص           ║"
echo "╚═══════════════════════════════════════╝"
echo ""

# === 1. فحص Nginx ===
echo "▶ [1] Nginx..."
if systemctl is-active --quiet nginx; then
    echo "   ✓ Nginx يعمل"
else
    echo "   ⚠ Nginx لا يعمل — جارٍ التشغيل..."
    systemctl start nginx
fi

echo "   المواقع المفعّلة:"
ls -1 /etc/nginx/sites-enabled/

# === 2. فحص PM2 وخادم الوحدات ===
echo ""
echo "▶ [2] PM2 / خادم الوحدات..."
pm2 list 2>/dev/null | grep -E "name|sad"

# === 3. تحديث .env بالنطاق الجديد ===
echo ""
echo "▶ [3] تحديث إعدادات خادم الوحدات..."
ENV_FILE="/opt/sad-registry/.env"
if [ -f "$ENV_FILE" ]; then
    # تحديث REGISTRY_URL
    if grep -q "REGISTRY_URL" "$ENV_FILE"; then
        sed -i 's|REGISTRY_URL=.*|REGISTRY_URL=https://sila-hub.dev|' "$ENV_FILE"
    else
        echo "REGISTRY_URL=https://sila-hub.dev" >> "$ENV_FILE"
    fi
    
    # التأكد من NODE_ENV=production
    sed -i 's|NODE_ENV=.*|NODE_ENV=production|' "$ENV_FILE"
    
    echo "   ✓ REGISTRY_URL = https://sila-hub.dev"
    echo "   ✓ NODE_ENV = production"
    cat "$ENV_FILE"
else
    echo "   ⚠ ملف .env غير موجود — جارٍ الإنشاء..."
    cat > "$ENV_FILE" << 'EOF'
PORT=3000
HOST=0.0.0.0
DB_PATH=./data/registry.db
STORAGE_PATH=./storage/packages
JWT_SECRET=sad-registry-prod-$(openssl rand -hex 32)
JWT_EXPIRY=30d
BCRYPT_ROUNDS=12
NODE_ENV=production
REGISTRY_URL=https://sila-hub.dev
MAX_PACKAGE_SIZE=50mb
RATE_LIMIT_WINDOW_MS=900000
RATE_LIMIT_MAX_REQUESTS=100
LOG_LEVEL=info
EOF
    echo "   ✓ ملف .env مُنشأ"
fi

# إعادة تشغيل الخادم بالإعدادات الجديدة
pm2 restart sad-registry 2>/dev/null && echo "   ✓ خادم الوحدات أُعيد تشغيله"

# === 4. فحص ملفات الموقع ===
echo ""
echo "▶ [4] موقع اللغة..."
WEBSITE_DIR="/opt/sad-lang-website"
if [ -f "$WEBSITE_DIR/index.html" ]; then
    FILE_COUNT=$(find "$WEBSITE_DIR" -type f | wc -l)
    DIR_SIZE=$(du -sh "$WEBSITE_DIR" | cut -f1)
    echo "   ✓ $FILE_COUNT ملف ($DIR_SIZE)"
else
    echo "   ✗ موقع اللغة غير موجود!"
fi

# === 5. اختبار الاتصال ===
echo ""
echo "▶ [5] اختبار الوصول..."
REGISTRY_CODE=$(curl -s -o /dev/null -w '%{http_code}' http://127.0.0.1:3000/api/v1 2>/dev/null || echo "000")
WEBSITE_CODE=$(curl -s -o /dev/null -w '%{http_code}' http://127.0.0.1/ 2>/dev/null || echo "000")

echo "   خادم الوحدات (3000): HTTP $REGISTRY_CODE"
echo "   موقع اللغة (nginx):  HTTP $WEBSITE_CODE"

# === 6. فحص DNS ===
echo ""
echo "▶ [6] فحص DNS..."
SILA_IP=$(dig +short sila-hub.dev A 2>/dev/null || echo "لم يُحل")
SAD_IP=$(dig +short sad-lang.org A 2>/dev/null || echo "لم يُحل")
echo "   sila-hub.dev  → $SILA_IP"
echo "   sad-lang.org  → $SAD_IP"

if [ "$SILA_IP" = "185.47.174.39" ] && [ "$SAD_IP" = "185.47.174.39" ]; then
    echo ""
    echo "   ✓ DNS صحيح! جارٍ الحصول على شهادات SSL..."
    
    certbot certonly --webroot -w /var/www/certbot \
        -d sila-hub.dev -d www.sila-hub.dev \
        --non-interactive --agree-tos --email admin@sila-hub.dev --no-eff-email \
        2>&1 && echo "   ✓ شهادة sila-hub.dev" || echo "   ⚠ فشل شهادة sila-hub.dev"
    
    certbot certonly --webroot -w /var/www/certbot \
        -d sad-lang.org -d www.sad-lang.org \
        --non-interactive --agree-tos --email admin@sad-lang.org --no-eff-email \
        2>&1 && echo "   ✓ شهادة sad-lang.org" || echo "   ⚠ فشل شهادة sad-lang.org"
    
    # التبديل إلى إعدادات SSL الكاملة
    if [ -f /etc/letsencrypt/live/sila-hub.dev/fullchain.pem ] && \
       [ -f /etc/letsencrypt/live/sad-lang.org/fullchain.pem ]; then
        ln -sf /etc/nginx/sites-available/sila-hub.dev /etc/nginx/sites-enabled/sila-hub.dev
        ln -sf /etc/nginx/sites-available/sad-lang.org /etc/nginx/sites-enabled/sad-lang.org
        rm -f /etc/nginx/sites-available/*.tmp
        nginx -t && systemctl reload nginx
        echo "   ✓ HTTPS مُفعّل!"
    fi
else
    echo ""
    echo "   ⚠ DNS لم يُضبط بعد — الموقعان يعملان على HTTP"
    echo "   عند ضبط DNS، شغّل هذا السكريبت مرة أخرى"
fi

# === النتيجة ===
echo ""
echo "╔═══════════════════════════════════════╗"
echo "║           الحالة النهائية              ║"
echo "╠═══════════════════════════════════════╣"
echo "║  IP: 185.47.174.39                    ║"
echo "║  sila-hub.dev → Express:3000          ║"
echo "║  sad-lang.org → /opt/sad-lang-website ║"
echo "╚═══════════════════════════════════════╝"
