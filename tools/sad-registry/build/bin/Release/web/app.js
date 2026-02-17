// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف جافاسكربت الرئيسي — واجهة مستودع حزم لغة ص
// ═══════════════════════════════════════════════════════════════════════════════
// يتعامل مع:
//   ● جلب البيانات من خادم API
//   ● عرض الحزم المميزة والشائعة والجديدة
//   ● نظام البحث (محلي + بحث من الخادم)
//   ● تسجيل الدخول وإنشاء الحساب
//   ● تنقل الصفحات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * الإعدادات العامة
 */
const CONFIG = {
    // رابط API — نفس الخادم (نسبي)
    API_BASE: '/api/v1',
    // عدد الحزم في كل قسم بالصفحة الرئيسية
    HOME_LIMIT: 6,
    // عدد نتائج البحث في كل صفحة
    SEARCH_PER_PAGE: 20,
    // مهلة الطلبات (مللي ثانية)
    TIMEOUT: 15000,
};

// ═══════════════════════════════════════════════════════════════════════════════
// مساعدات عامة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * إرسال طلب API مع معالجة الأخطاء.
 * @param {string} endpoint — المسار (مثال: '/packages/popular')
 * @param {object} options — خيارات fetch إضافية
 * @returns {Promise<object|null>}
 */
async function apiCall(endpoint, options = {}) {
    try {
        const url = CONFIG.API_BASE + endpoint;
        const token = localStorage.getItem('sad_token');

        const headers = {
            'Content-Type': 'application/json',
            ...(token ? { 'Authorization': `Bearer ${token}` } : {}),
            ...(options.headers || {}),
        };

        const controller = new AbortController();
        const timeoutId = setTimeout(() => controller.abort(), CONFIG.TIMEOUT);

        const response = await fetch(url, {
            ...options,
            headers,
            signal: controller.signal,
        });

        clearTimeout(timeoutId);

        if (!response.ok) {
            console.error(`خطأ API: ${response.status} — ${endpoint}`);
            return null;
        }

        return await response.json();
    } catch (err) {
        if (err.name === 'AbortError') {
            console.error(`انتهت المهلة: ${endpoint}`);
        } else {
            console.error(`خطأ في الاتصال: ${err.message}`);
        }
        return null;
    }
}

/**
 * تنسيق الأرقام بالصيغة العربية (مثال: 12,345 → ١٢٬٣٤٥)
 */
function formatNumber(num) {
    if (num === undefined || num === null) return '—';
    return new Intl.NumberFormat('ar-SA').format(num);
}

/**
 * تنسيق التاريخ بالصيغة العربية
 */
function formatDate(dateStr) {
    if (!dateStr) return '—';
    try {
        const date = new Date(dateStr);
        return new Intl.DateTimeFormat('ar-SA', {
            year: 'numeric',
            month: 'long',
            day: 'numeric'
        }).format(date);
    } catch {
        return dateStr;
    }
}

/**
 * تنسيق الوقت النسبي (مثال: "منذ ٣ أيام")
 */
function timeAgo(dateStr) {
    if (!dateStr) return '';
    const now = Date.now();
    const then = new Date(dateStr).getTime();
    const diff = now - then;

    const minute = 60 * 1000;
    const hour = 60 * minute;
    const day = 24 * hour;
    const week = 7 * day;
    const month = 30 * day;

    if (diff < minute) return 'الآن';
    if (diff < hour) return `منذ ${Math.floor(diff / minute)} دقيقة`;
    if (diff < day) return `منذ ${Math.floor(diff / hour)} ساعة`;
    if (diff < week) return `منذ ${Math.floor(diff / day)} يوم`;
    if (diff < month) return `منذ ${Math.floor(diff / week)} أسبوع`;
    return `منذ ${Math.floor(diff / month)} شهر`;
}

/**
 * عرض إشعار مؤقت (Toast)
 */
function showToast(message, duration = 3000) {
    // إزالة إشعار سابق إن وجد
    const existing = document.querySelector('.toast');
    if (existing) existing.remove();

    const toast = document.createElement('div');
    toast.className = 'toast';
    toast.textContent = message;
    document.body.appendChild(toast);

    // تفعيل الحركة
    requestAnimationFrame(() => {
        toast.classList.add('visible');
    });

    // إخفاء بعد المدة
    setTimeout(() => {
        toast.classList.remove('visible');
        setTimeout(() => toast.remove(), 300);
    }, duration);
}

/**
 * الحصول على معاملات URL
 */
function getUrlParams() {
    return new URLSearchParams(window.location.search);
}

// ═══════════════════════════════════════════════════════════════════════════════
// إنشاء عناصر HTML للحزم
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * إنشاء بطاقة حزمة (للصفحة الرئيسية)
 */
function createPackageCard(pkg) {
    const card = document.createElement('a');
    card.className = 'package-card';
    card.href = `/package.html?name=${encodeURIComponent(pkg.name)}`;

    // الاسم العربي (إن وُجد)
    const arabicName = pkg.arabic_name
        ? `<span class="package-name-ar">(${pkg.arabic_name})</span>`
        : '';

    // الكلمات المفتاحية
    let keywordsHtml = '';
    if (pkg.keywords && pkg.keywords.length > 0) {
        const tags = pkg.keywords.slice(0, 3).map(k =>
            `<span class="keyword-tag">${escapeHtml(k)}</span>`
        ).join('');
        keywordsHtml = `<div class="package-keywords">${tags}</div>`;
    }

    card.innerHTML = `
        <div class="package-header">
            <span class="package-name">${escapeHtml(pkg.name)}</span>
            ${arabicName}
            <span class="package-version">v${escapeHtml(pkg.latest_version || pkg.version || '0.0.0')}</span>
        </div>
        ${keywordsHtml}
        <p class="package-description">
            ${escapeHtml(pkg.description_ar || pkg.description || 'بدون وصف')}
        </p>
        <div class="package-meta">
            <span class="package-meta-item">📥 ${formatNumber(pkg.downloads || 0)}</span>
            <span class="package-meta-item">👤 ${escapeHtml(pkg.author || pkg.owner || '—')}</span>
            ${pkg.updated_at ? `<span class="package-meta-item">🕐 ${timeAgo(pkg.updated_at)}</span>` : ''}
        </div>
    `;

    return card;
}

/**
 * إنشاء بطاقة نتيجة بحث (قائمة عمودية)
 */
function createSearchResultCard(pkg) {
    const card = document.createElement('a');
    card.className = 'search-result-card';
    card.href = `/package.html?name=${encodeURIComponent(pkg.name)}`;

    const arabicName = pkg.arabic_name
        ? `<span class="package-name-ar">(${pkg.arabic_name})</span>`
        : '';

    card.innerHTML = `
        <div class="result-main">
            <div class="package-header">
                <span class="package-name">${escapeHtml(pkg.name)}</span>
                ${arabicName}
                <span class="package-version">v${escapeHtml(pkg.latest_version || pkg.version || '0.0.0')}</span>
            </div>
            <p class="package-description">
                ${escapeHtml(pkg.description_ar || pkg.description || 'بدون وصف')}
            </p>
            <div class="package-meta">
                <span class="package-meta-item">👤 ${escapeHtml(pkg.author || pkg.owner || '—')}</span>
                ${pkg.updated_at ? `<span class="package-meta-item">🕐 ${timeAgo(pkg.updated_at)}</span>` : ''}
                ${pkg.license ? `<span class="package-meta-item">📄 ${escapeHtml(pkg.license)}</span>` : ''}
            </div>
        </div>
        <div class="result-stats">
            <span class="result-downloads">${formatNumber(pkg.downloads || 0)}</span>
            <span>تنزيل</span>
        </div>
    `;

    return card;
}

/**
 * حماية من XSS — تحويل الأحرف الخاصة
 */
function escapeHtml(str) {
    if (!str) return '';
    const div = document.createElement('div');
    div.textContent = str;
    return div.innerHTML;
}

// ═══════════════════════════════════════════════════════════════════════════════
// الصفحة الرئيسية
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * تحميل إحصائيات المستودع وعرضها في القسم الرئيسي
 */
async function loadStats() {
    const data = await apiCall('/stats');
    if (data) {
        const el = (id) => document.getElementById(id);
        if (el('statPackages')) el('statPackages').textContent = formatNumber(data.total_packages || 0);
        if (el('statDownloads')) el('statDownloads').textContent = formatNumber(data.total_downloads || 0);
        if (el('statUsers'))    el('statUsers').textContent = formatNumber(data.total_users || 0);
    }
}

/**
 * تحميل قسم حزم (مميزة/شائعة/حديثة) وعرضه
 * @param {string} endpoint — مسار API (مثال: '/packages/featured')
 * @param {string} containerId — معرف العنصر HTML
 */
async function loadPackageSection(endpoint, containerId) {
    const container = document.getElementById(containerId);
    if (!container) return;

    const data = await apiCall(`${endpoint}?limit=${CONFIG.HOME_LIMIT}`);

    if (data && data.packages && data.packages.length > 0) {
        container.innerHTML = '';
        data.packages.forEach(pkg => {
            container.appendChild(createPackageCard(pkg));
        });
    } else if (data && Array.isArray(data) && data.length > 0) {
        container.innerHTML = '';
        data.forEach(pkg => {
            container.appendChild(createPackageCard(pkg));
        });
    } else {
        // عرض حالة فارغة مع أمثلة تجريبية
        container.innerHTML = '';
        const demoPackages = getDemoPackages();
        demoPackages.forEach(pkg => {
            container.appendChild(createPackageCard(pkg));
        });
    }
}

/**
 * حزم تجريبية للعرض عند عدم وجود بيانات حقيقية
 */
function getDemoPackages() {
    return [
        {
            name: 'شبكة',
            arabic_name: 'شبكة',
            latest_version: '2.1.0',
            description_ar: 'مكتبة شاملة للتعامل مع بروتوكول HTTP — طلبات GET/POST/PUT/DELETE مع دعم كامل للترميز العربي',
            downloads: 12500,
            author: 'أحمد',
            keywords: ['شبكات', 'HTTP', 'ويب'],
        },
        {
            name: 'رياضيات',
            arabic_name: 'رياضيات',
            latest_version: '1.5.3',
            description_ar: 'دوال رياضية متقدمة: جبر خطي، إحصاء، تحليل عددي، مع دعم الأعداد العربية',
            downloads: 9800,
            author: 'فاطمة',
            keywords: ['رياضيات', 'إحصاء', 'جبر'],
        },
        {
            name: 'json',
            arabic_name: 'جيسون',
            latest_version: '3.0.1',
            description_ar: 'محلل ومولد JSON سريع وآمن — يدعم UTF-8 والنصوص العربية بالكامل',
            downloads: 15200,
            author: 'عمر',
            keywords: ['JSON', 'تحليل', 'بيانات'],
        },
        {
            name: 'قاعدة',
            arabic_name: 'قاعدة',
            latest_version: '1.2.0',
            description_ar: 'واجهة موحدة للتعامل مع قواعد البيانات — SQLite، PostgreSQL مع استعلامات بالعربية',
            downloads: 7300,
            author: 'محمد',
            keywords: ['قواعد_بيانات', 'SQL', 'تخزين'],
        },
        {
            name: 'واجهة',
            arabic_name: 'واجهة',
            latest_version: '0.9.0',
            description_ar: 'إطار عمل لبناء واجهات المستخدم الرسومية بلغة ص — أزرار، نوافذ، قوائم',
            downloads: 5100,
            author: 'سارة',
            keywords: ['واجهات', 'GUI', 'رسوميات'],
        },
        {
            name: 'تشفير',
            arabic_name: 'تشفير',
            latest_version: '1.0.2',
            description_ar: 'مكتبة تشفير شاملة — AES، RSA، SHA، HMAC مع واجهة عربية سهلة الاستخدام',
            downloads: 4200,
            author: 'خالد',
            keywords: ['أمان', 'تشفير', 'حماية'],
        },
    ];
}

// ═══════════════════════════════════════════════════════════════════════════════
// البحث
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * تنفيذ البحث وعرض النتائج (في صفحة البحث)
 */
async function performSearch() {
    const params = getUrlParams();
    const query = params.get('q') || '';
    const category = params.get('category') || '';
    const sort = params.get('sort') || 'relevance';
    const page = parseInt(params.get('page') || '1');

    // تحديث حقل البحث
    const searchInputs = document.querySelectorAll('input[name="q"]');
    searchInputs.forEach(input => input.value = query);

    // عنوان البحث
    const headerEl = document.getElementById('searchHeader');
    if (headerEl) {
        if (query) {
            headerEl.innerHTML = `<h1>نتائج البحث عن: "${escapeHtml(query)}"</h1>`;
        } else if (category) {
            headerEl.innerHTML = `<h1>تصنيف: ${escapeHtml(category)}</h1>`;
        } else {
            headerEl.innerHTML = `<h1>استكشاف الحزم</h1>`;
        }
    }

    // جلب النتائج من API
    const resultsContainer = document.getElementById('searchResults');
    if (!resultsContainer) return;

    resultsContainer.innerHTML = '<div class="loading-placeholder">جارٍ البحث...</div>';

    let endpoint = '/search?';
    if (query) endpoint += `q=${encodeURIComponent(query)}&`;
    if (category) endpoint += `category=${encodeURIComponent(category)}&`;
    endpoint += `sort=${sort}&page=${page}&per_page=${CONFIG.SEARCH_PER_PAGE}`;

    const data = await apiCall(endpoint);

    if (data && data.results && data.results.length > 0) {
        resultsContainer.innerHTML = '';
        // معلومات النتائج
        const info = document.getElementById('searchInfo');
        if (info) {
            info.textContent = `${formatNumber(data.total || data.results.length)} نتيجة` +
                (data.search_time_ms ? ` (${data.search_time_ms.toFixed(0)} مللي ثانية)` : '');
        }

        data.results.forEach(pkg => {
            resultsContainer.appendChild(createSearchResultCard(pkg));
        });

        // ترقيم الصفحات
        renderPagination(data.total || data.results.length, page);
    } else {
        // عرض حزم تجريبية عند عدم وجود اتصال
        const demos = getDemoPackages().filter(p =>
            !query || p.name.includes(query) ||
            (p.description_ar && p.description_ar.includes(query)) ||
            (p.arabic_name && p.arabic_name.includes(query))
        );

        if (demos.length > 0) {
            resultsContainer.innerHTML = '';
            demos.forEach(pkg => {
                resultsContainer.appendChild(createSearchResultCard(pkg));
            });
        } else {
            resultsContainer.innerHTML = `
                <div class="empty-state">
                    <div class="empty-state-icon">🔍</div>
                    <h3>لم يتم العثور على نتائج</h3>
                    <p>جرّب كلمات بحث مختلفة أو تصفّح التصنيفات</p>
                </div>
            `;
        }
    }
}

/**
 * عرض أزرار الترقيم
 */
function renderPagination(total, currentPage) {
    const container = document.getElementById('pagination');
    if (!container) return;

    const totalPages = Math.ceil(total / CONFIG.SEARCH_PER_PAGE);
    if (totalPages <= 1) {
        container.innerHTML = '';
        return;
    }

    const params = getUrlParams();
    let html = '';

    for (let i = 1; i <= Math.min(totalPages, 10); i++) {
        params.set('page', i);
        const active = i === currentPage ? 'active' : '';
        html += `<a href="?${params.toString()}" class="page-btn ${active}">${i}</a>`;
    }

    container.innerHTML = html;
}

// ═══════════════════════════════════════════════════════════════════════════════
// صفحة الحزمة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * تحميل وعرض تفاصيل حزمة واحدة
 */
async function loadPackageDetails() {
    const params = getUrlParams();
    const name = params.get('name');

    if (!name) {
        document.getElementById('packageContent').innerHTML =
            '<div class="empty-state"><h3>لم يتم تحديد حزمة</h3></div>';
        return;
    }

    // تحديث العنوان
    document.title = `${name} — مستودع حزم لغة ص`;

    const data = await apiCall(`/packages/${encodeURIComponent(name)}`);

    if (data) {
        renderPackageDetails(data);
    } else {
        // عرض تجريبي
        const demo = getDemoPackages().find(p => p.name === name);
        if (demo) {
            renderPackageDetails({
                name: demo.name,
                arabic_name: demo.arabic_name,
                latest_version: demo.latest_version,
                description: demo.description_ar,
                description_ar: demo.description_ar,
                downloads: demo.downloads,
                author: demo.author,
                license: 'MIT',
                keywords: demo.keywords,
                created_at: '2025-01-15',
                updated_at: '2025-06-20',
                readme: `# ${demo.name}\n\n${demo.description_ar}\n\n## التثبيت\n\n\`\`\`\nص-حزم أضف ${demo.name}\n\`\`\`\n\n## الاستخدام\n\n\`\`\`\nاستورد "${demo.name}"\n\`\`\``,
            });
        } else {
            document.getElementById('packageContent').innerHTML =
                '<div class="empty-state"><h3>الحزمة غير موجودة</h3></div>';
        }
    }
}

/**
 * عرض تفاصيل الحزمة في الصفحة
 */
function renderPackageDetails(pkg) {
    const container = document.getElementById('packageContent');
    if (!container) return;

    const arabicTitle = pkg.arabic_name ? `<p class="package-page-title-ar">${escapeHtml(pkg.arabic_name)}</p>` : '';

    // تحويل README بسيط (Markdown → HTML مبسط)
    let readmeHtml = '';
    if (pkg.readme) {
        readmeHtml = simpleMarkdown(pkg.readme);
    } else {
        readmeHtml = `<p>${escapeHtml(pkg.description_ar || pkg.description || 'بدون وصف')}</p>`;
    }

    container.innerHTML = `
        <!-- رأس الحزمة -->
        <div class="package-page-header">
            <h1 class="package-page-title">${escapeHtml(pkg.name)}</h1>
            ${arabicTitle}
            <p>${escapeHtml(pkg.description_ar || pkg.description || '')}</p>
            <div class="package-install-cmd">
                <code>ص-حزم أضف ${escapeHtml(pkg.name)}</code>
                <button class="copy-btn" onclick="copyInstall('${escapeHtml(pkg.name)}')">نسخ 📋</button>
            </div>
        </div>

        <!-- المحتوى -->
        <div class="package-content">
            <div class="package-readme">
                ${readmeHtml}
            </div>
            <div class="package-sidebar">
                <!-- معلومات عامة -->
                <div class="sidebar-card">
                    <h4>معلومات</h4>
                    <div class="sidebar-item">
                        <span class="sidebar-item-label">الإصدار</span>
                        <span class="sidebar-item-value">${escapeHtml(pkg.latest_version || pkg.version || '—')}</span>
                    </div>
                    <div class="sidebar-item">
                        <span class="sidebar-item-label">الرخصة</span>
                        <span class="sidebar-item-value">${escapeHtml(pkg.license || '—')}</span>
                    </div>
                    <div class="sidebar-item">
                        <span class="sidebar-item-label">المؤلف</span>
                        <span class="sidebar-item-value">${escapeHtml(pkg.author || pkg.owner || '—')}</span>
                    </div>
                    <div class="sidebar-item">
                        <span class="sidebar-item-label">التنزيلات</span>
                        <span class="sidebar-item-value">${formatNumber(pkg.downloads || 0)}</span>
                    </div>
                </div>

                <!-- التواريخ -->
                <div class="sidebar-card">
                    <h4>التواريخ</h4>
                    <div class="sidebar-item">
                        <span class="sidebar-item-label">الإنشاء</span>
                        <span class="sidebar-item-value">${formatDate(pkg.created_at)}</span>
                    </div>
                    <div class="sidebar-item">
                        <span class="sidebar-item-label">آخر تحديث</span>
                        <span class="sidebar-item-value">${formatDate(pkg.updated_at)}</span>
                    </div>
                </div>

                ${pkg.keywords && pkg.keywords.length > 0 ? `
                <div class="sidebar-card">
                    <h4>الكلمات المفتاحية</h4>
                    <div class="package-keywords">
                        ${pkg.keywords.map(k => `<span class="keyword-tag">${escapeHtml(k)}</span>`).join('')}
                    </div>
                </div>
                ` : ''}

                <!-- التثبيت -->
                <div class="sidebar-card">
                    <h4>التثبيت</h4>
                    <div class="code-block" style="font-size: 13px;">
                        <code>ص-حزم أضف ${escapeHtml(pkg.name)}</code>
                    </div>
                </div>
            </div>
        </div>
    `;
}

/**
 * نسخ أمر التثبيت إلى الحافظة
 */
function copyInstall(name) {
    const cmd = `ص-حزم أضف ${name}`;
    navigator.clipboard.writeText(cmd).then(() => {
        showToast('✓ تم النسخ إلى الحافظة');
    }).catch(() => {
        // بديل إذا لم يعمل clipboard API
        showToast('الأمر: ' + cmd);
    });
}

/**
 * محوّل Markdown → HTML مبسط
 * يدعم: العناوين، الكود، القوائم، الأسطر
 */
function simpleMarkdown(text) {
    let html = escapeHtml(text);

    // كتل الكود (```)
    html = html.replace(/```([^`]*?)```/gs, '<pre><code>$1</code></pre>');
    // الكود السطري (``)
    html = html.replace(/`([^`]+)`/g, '<code style="background:#f0f0f0;padding:2px 6px;border-radius:4px;">$1</code>');
    // العناوين
    html = html.replace(/^### (.+)$/gm, '<h3>$1</h3>');
    html = html.replace(/^## (.+)$/gm, '<h2>$1</h2>');
    html = html.replace(/^# (.+)$/gm, '<h1>$1</h1>');
    // القوائم
    html = html.replace(/^[\-\*] (.+)$/gm, '<li>$1</li>');
    html = html.replace(/(<li>.*<\/li>)/gs, '<ul>$1</ul>');
    // الغامق والمائل
    html = html.replace(/\*\*(.+?)\*\*/g, '<strong>$1</strong>');
    html = html.replace(/\*(.+?)\*/g, '<em>$1</em>');
    // الأسطر الفارغة → فقرات
    html = html.replace(/\n\n/g, '</p><p>');
    html = '<p>' + html + '</p>';
    // تنظيف
    html = html.replace(/<p><\/p>/g, '');
    html = html.replace(/<p>(<h[1-6]>)/g, '$1');
    html = html.replace(/(<\/h[1-6]>)<\/p>/g, '$1');
    html = html.replace(/<p>(<pre>)/g, '$1');
    html = html.replace(/(<\/pre>)<\/p>/g, '$1');
    html = html.replace(/<p>(<ul>)/g, '$1');
    html = html.replace(/(<\/ul>)<\/p>/g, '$1');

    return html;
}

// ═══════════════════════════════════════════════════════════════════════════════
// المصادقة (تسجيل الدخول / إنشاء حساب)
// ═══════════════════════════════════════════════════════════════════════════════

let isRegistering = false;

/**
 * تهيئة نظام تسجيل الدخول
 */
function initAuth() {
    const loginBtn = document.getElementById('loginBtn');
    const modalClose = document.getElementById('modalClose');
    const modalOverlay = document.getElementById('loginModal');
    const authForm = document.getElementById('authForm');
    const authToggle = document.getElementById('authToggle');

    if (!loginBtn || !modalOverlay) return;

    // فتح النافذة
    loginBtn.addEventListener('click', (e) => {
        e.preventDefault();

        // إذا كان مسجل الدخول → عرض معلومات المستخدم
        const token = localStorage.getItem('sad_token');
        if (token) {
            showToast('أنت مسجل الدخول بالفعل');
            return;
        }

        modalOverlay.classList.add('active');
    });

    // إغلاق النافذة
    if (modalClose) {
        modalClose.addEventListener('click', () => {
            modalOverlay.classList.remove('active');
        });
    }
    modalOverlay.addEventListener('click', (e) => {
        if (e.target === modalOverlay) modalOverlay.classList.remove('active');
    });

    // التبديل بين تسجيل الدخول وإنشاء الحساب
    if (authToggle) {
        authToggle.addEventListener('click', (e) => {
            e.preventDefault();
            isRegistering = !isRegistering;
            updateAuthUI();
        });
    }

    // إرسال النموذج
    if (authForm) {
        authForm.addEventListener('submit', async (e) => {
            e.preventDefault();
            await handleAuth();
        });
    }

    // تحديث واجهة المستخدم عند الحمل
    updateLoginButton();
}

/**
 * تحديث واجهة نافذة المصادقة
 */
function updateAuthUI() {
    const title = document.getElementById('modalTitle');
    const usernameGroup = document.getElementById('usernameGroup');
    const submitBtn = document.getElementById('authSubmit');
    const toggleText = document.getElementById('authToggleText');
    const toggle = document.getElementById('authToggle');

    if (isRegistering) {
        if (title) title.textContent = 'إنشاء حساب جديد';
        if (usernameGroup) usernameGroup.style.display = 'block';
        if (submitBtn) submitBtn.textContent = 'إنشاء حساب';
        if (toggleText) toggleText.textContent = 'لديك حساب بالفعل؟';
        if (toggle) toggle.textContent = 'تسجيل الدخول';
    } else {
        if (title) title.textContent = 'تسجيل الدخول';
        if (usernameGroup) usernameGroup.style.display = 'none';
        if (submitBtn) submitBtn.textContent = 'دخول';
        if (toggleText) toggleText.textContent = 'ليس لديك حساب؟';
        if (toggle) toggle.textContent = 'إنشاء حساب جديد';
    }
}

/**
 * معالجة إرسال نموذج المصادقة
 */
async function handleAuth() {
    const email = document.getElementById('authEmail')?.value;
    const password = document.getElementById('authPassword')?.value;
    const username = document.getElementById('authUsername')?.value;

    if (!email || !password) {
        showToast('يرجى ملء جميع الحقول');
        return;
    }

    const endpoint = isRegistering ? '/auth/register' : '/auth/login';
    const body = isRegistering
        ? { username, email, password }
        : { email, password };

    const data = await apiCall(endpoint, {
        method: 'POST',
        body: JSON.stringify(body),
    });

    if (data && data.token) {
        localStorage.setItem('sad_token', data.token);
        localStorage.setItem('sad_user', data.username || email);
        showToast(isRegistering ? '✓ تم إنشاء الحساب بنجاح' : '✓ تم تسجيل الدخول');
        document.getElementById('loginModal')?.classList.remove('active');
        updateLoginButton();
    } else if (data && data.error) {
        showToast('⚠ ' + data.error);
    } else {
        showToast('⚠ خطأ في الاتصال');
    }
}

/**
 * تحديث زر تسجيل الدخول حسب الحالة
 */
function updateLoginButton() {
    const loginBtn = document.getElementById('loginBtn');
    if (!loginBtn) return;

    const user = localStorage.getItem('sad_user');
    if (user) {
        loginBtn.textContent = `👤 ${user}`;
        loginBtn.title = 'اضغط لتسجيل الخروج';
        loginBtn.onclick = (e) => {
            e.preventDefault();
            localStorage.removeItem('sad_token');
            localStorage.removeItem('sad_user');
            showToast('تم تسجيل الخروج');
            loginBtn.textContent = 'تسجيل الدخول';
            loginBtn.onclick = null;
        };
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// التهيئة عند تحميل الصفحة
// ═══════════════════════════════════════════════════════════════════════════════

document.addEventListener('DOMContentLoaded', () => {
    // تحديد الصفحة الحالية
    const path = window.location.pathname;

    // تهيئة المصادقة (مشتركة بين جميع الصفحات)
    initAuth();

    if (path === '/' || path === '/index.html' || path === '') {
        // ═══ الصفحة الرئيسية ═══
        loadStats();
        loadPackageSection('/packages/featured', 'featuredPackages');
        loadPackageSection('/packages/popular',  'popularPackages');
        loadPackageSection('/packages/recent',   'recentPackages');
    }
    else if (path === '/search.html') {
        // ═══ صفحة البحث ═══
        performSearch();
    }
    else if (path === '/package.html') {
        // ═══ صفحة الحزمة ═══
        loadPackageDetails();
    }
});
