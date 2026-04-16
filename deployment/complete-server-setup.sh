#!/bin/bash
# complete-server-setup.sh
# تشغيل: bash /tmp/complete-server-setup.sh
set -u

SERVER_IP="185.47.174.39"
echo "=START="

echo "=STEP1= SSH key already handled by PowerShell"

echo "=STEP2= Updating registry env..."
ENV="/opt/sad-registry/.env"
if [ -f "$ENV" ]; then
    sed -i 's|REGISTRY_URL=.*|REGISTRY_URL=https://sila-hub.dev|' "$ENV"
    sed -i 's|NODE_ENV=.*|NODE_ENV=production|' "$ENV"
    grep -q "REGISTRY_URL" "$ENV" || echo "REGISTRY_URL=https://sila-hub.dev" >> "$ENV"
    grep -q "NODE_ENV" "$ENV" || echo "NODE_ENV=production" >> "$ENV"
else
    cat > "$ENV" << 'ENVEOF'
PORT=3000
HOST=0.0.0.0
DB_PATH=./data/registry.db
STORAGE_PATH=./storage/packages
JWT_EXPIRY=30d
BCRYPT_ROUNDS=12
NODE_ENV=production
REGISTRY_URL=https://sila-hub.dev
MAX_PACKAGE_SIZE=50mb
RATE_LIMIT_WINDOW_MS=900000
RATE_LIMIT_MAX_REQUESTS=100
LOG_LEVEL=info
ENVEOF
fi
pm2 restart sad-registry 2>/dev/null || true
echo "=STEP2_DONE="

echo "=STEP3= Checking DNS..."
SILA_IP=$(dig +short sila-hub.dev A 2>/dev/null | head -1 || echo "NXDOMAIN")
SAD_IP=$(dig +short sad-lang.org A 2>/dev/null | head -1 || echo "NXDOMAIN")
echo "SILA_IP=${SILA_IP:-UNSET}"
echo "SAD_IP=${SAD_IP:-UNSET}"

DNS_READY=0
if [ "${SILA_IP}" = "${SERVER_IP}" ] && [ "${SAD_IP}" = "${SERVER_IP}" ]; then
    DNS_READY=1
    echo "DNS_READY=YES"
else
    echo "DNS_READY=NO"
fi

echo "=STEP4= SSL setup..."
if [ "$DNS_READY" = "1" ]; then
    certbot certonly --webroot -w /var/www/certbot \
        -d sila-hub.dev -d www.sila-hub.dev \
        --non-interactive --agree-tos \
        --email admin@sila-hub.dev --no-eff-email 2>&1 || true

    certbot certonly --webroot -w /var/www/certbot \
        -d sad-lang.org -d www.sad-lang.org \
        --non-interactive --agree-tos \
        --email admin@sad-lang.org --no-eff-email 2>&1 || true

    SILA_CERT="/etc/letsencrypt/live/sila-hub.dev/fullchain.pem"
    SAD_CERT="/etc/letsencrypt/live/sad-lang.org/fullchain.pem"

    if [ -f "$SILA_CERT" ] && [ -f "$SAD_CERT" ]; then
        ln -sf /etc/nginx/sites-available/sila-hub.dev /etc/nginx/sites-enabled/sila-hub.dev
        ln -sf /etc/nginx/sites-available/sad-lang.org /etc/nginx/sites-enabled/sad-lang.org
        rm -f /etc/nginx/sites-available/sila-hub.dev.tmp 2>/dev/null || true
        rm -f /etc/nginx/sites-available/sad-lang.org.tmp 2>/dev/null || true
        nginx -t && systemctl reload nginx
        systemctl enable certbot.timer 2>/dev/null || true
        echo "SSL_STATUS=COMPLETE"
    else
        echo "SSL_STATUS=PARTIAL"
    fi
else
    echo "SSL_STATUS=SKIPPED_DNS_NOT_READY"
fi

echo "=STEP5= Final status..."
echo "NGINX_STATUS=$(systemctl is-active nginx)"
echo "WEBSITE_FILES=$(ls /opt/sad-lang-website/ 2>/dev/null | wc -l)"
echo "PM2_ONLINE=$(pm2 list 2>/dev/null | grep -c online || echo 0)"
HTTP_WEB=$(curl -s -o /dev/null -w '%{http_code}' http://127.0.0.1/ 2>/dev/null || echo "000")
echo "HTTP_WEB=${HTTP_WEB}"
HTTP_API=$(curl -s -o /dev/null -w '%{http_code}' http://127.0.0.1:3000/api/v1 2>/dev/null || echo "000")
echo "HTTP_API=${HTTP_API}"
SILA_SSL="NO"
if [ -f /etc/letsencrypt/live/sila-hub.dev/fullchain.pem ]; then SILA_SSL="YES"; fi
echo "SSL_SILA=${SILA_SSL}"
SAD_SSL="NO"
if [ -f /etc/letsencrypt/live/sad-lang.org/fullchain.pem ]; then SAD_SSL="YES"; fi
echo "SSL_SAD=${SAD_SSL}"

echo "=DONE="
