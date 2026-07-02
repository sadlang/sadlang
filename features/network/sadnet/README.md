# صNet — مكتبة الشبكات اللامركزية للغة ص

## نظرة عامة

صNet هي مكتبة شبكات لامركزية كاملة مكتوبة بـ C++17، مصممة للعمل مع لغة ص (Sad Programming Language). توفر جميع البنى التحتية اللازمة لبناء تطبيقات لامركزية: من DHT والتشفير وحتى البث المباشر.

**لا تعتمد على أي مكتبة خارجية** — كل شيء مبني من الصفر (SHA-256, AES-256-GCM, X25519, Double Ratchet...).

## المعمارية

```
┌─────────────────────────────────────────────────────────┐
│              تطبيقات لغة ص (استورد شبكة_لامركزية)       │
├─────────────────────────────────────────────────────────┤
│                    ربط المفسر (Bindings)                │
│              builtin_module_sadnet.cpp                   │
├─────────┬──────────┬──────────┬─────────┬───────────────┤
│  بث     │   CDN    │ مراسلة  │ هوية    │  توجيه بصلي  │
│ مباشر  │ لامركزي │  E2EE   │  DID    │  (Onion)      │
├─────────┼──────────┼──────────┼─────────┼───────────────┤
│  أمان   │ تخزين   │  DNS     │ اكتشاف │    DHT        │
│ Sybil   │ موزع    │  .snet  │ LAN+PEX │  Kademlia     │
├─────────┴──────────┴──────────┴─────────┴───────────────┤
│                    NAT (STUN/TURN/ICE)                  │
├─────────────────────────────────────────────────────────┤
│                تشفير (SHA-256, AES-GCM, X25519)         │
├─────────────────────────────────────────────────────────┤
│              نواة (NodeId, Transport, Types)             │
├─────────────────────────────────────────────────────────┤
│     نظام التشغيل (WinSock2/POSIX) أو Freestanding      │
└─────────────────────────────────────────────────────────┘
```

## الوحدات (13 وحدة)

| الوحدة | المسار | الوصف |
|--------|--------|-------|
| **النواة** | `core/` | NodeId (256-bit), Endpoint, Buffer, Result<T>, UDP/TCP |
| **التشفير** | `crypto/` | SHA-256, AES-256-GCM, X25519, HKDF, Double Ratchet, CSPRNG |
| **DHT** | `dht/` | Kademlia: K=20, α=3, 256 K-Bucket, PING/STORE/FIND_NODE/FIND_VALUE |
| **NAT** | `nat/` | STUN (RFC 5389), TURN (RFC 5766), ICE (RFC 8445) |
| **الاكتشاف** | `discovery/` | LAN multicast (239.255.42.99:7655), PEX |
| **المراسلة** | `messaging/` | E2EE عبر Double Ratchet, جلسات جماعية مشفرة |
| **التوجيه** | `routing/` | توجيه بصلي (3+ قفزات), X25519 لكل قفزة, AES-GCM طبقي |
| **الهوية** | `identity/` | DID لامركزي ("did:snet:"), شهادات قابلة للتحقق |
| **DNS** | `dns/` | أسماء .snet, 7 أنواع سجلات, تخزين مؤقت + توقيع |
| **التخزين** | `storage/` | محتوى معنون (IPFS-like), قطع 256KB, جمع القمامة |
| **الأمان** | `security/` | إثبات عمل (PoW), سمعة, شهادات ثقة, قوائم سوداء |
| **البث** | `streaming/` | P2P مباشر, قطع, معدل تكيفي, mesh |
| **CDN** | `cdn/` | تخزين مؤقت LRU مع شعبية, نشر/طلب محتوى |

## الاستخدام في لغة ص

### الاستيراد

```sad
استورد شبكة_لامركزية

# أو مع اسم مستعار
استورد شبكة_لامركزية كـ شبكة

# أو استيراد دوال محددة
من شبكة_لامركزية استورد عقدة_جديدة
```

### الاسماء البديلة للاستيراد

- `شبكة_لامركزية` (الاسم الرسمي)
- `sadnet`
- `p2p`
- `decentralized`
- `شبكة_موزعة`

### مثال: إنشاء عقدة وتخزين ملف

```sad
استورد شبكة_لامركزية

# إنشاء عقدة
متغير عقدة = عقدة_جديدة()
اطبع_سطر("معرّف العقدة: " + معرّف_العقدة(عقدة))

# تخزين ملف في الشبكة الموزعة
متغير محتوى = "مرحبا بالعالم اللامركزي!"
متغير معرّف = تخزين_ملف(عقدة، "مرحبا.نص"، محتوى)
اطبع_سطر("تم التخزين: " + معرّف)

# استرجاع الملف
متغير بيانات = استرجاع_ملف(عقدة، معرّف)
اطبع_سطر("المحتوى: " + بيانات)

# تنظيف
تدمير_عقدة(عقدة)
```

### مثال: DNS لامركزي

```sad
استورد شبكة_لامركزية

متغير عقدة = عقدة_جديدة()

# تسجيل اسم
dns_تسجيل(عقدة، "موقعي.snet"، "192.168.1.100")

# حل الاسم
متغير عنوان = dns_حل(عقدة، "موقعي.snet")
اطبع_سطر("العنوان: " + عنوان)  # 192.168.1.100
```

### مثال: التشفير

```sad
استورد شبكة_لامركزية

# تجزئة SHA-256
متغير هاش = تشفير_sha256("رسالة سرية")
اطبع_سطر("SHA-256: " + هاش)

# بايتات عشوائية آمنة
متغير عشوائي = بايتات_عشوائية(32)
اطبع_سطر("عشوائي: " + عشوائي)
```

## قائمة الدوال المتاحة

| الدالة العربية | البديل الإنجليزي | الوصف |
|----------------|------------------|-------|
| `عقدة_جديدة()` | `snet_new_node()` | إنشاء عقدة شبكية جديدة |
| `معرّف_العقدة(ع)` | `snet_node_id(n)` | معرّف العقدة (hex 64 حرف) |
| `تدمير_عقدة(ع)` | `snet_destroy_node(n)` | تدمير العقدة وتحرير مواردها |
| `تشفير_sha256(نص)` | `snet_sha256(text)` | تجزئة SHA-256 |
| `بايتات_عشوائية(عدد)` | `snet_random_bytes(n)` | بايتات عشوائية آمنة |
| `dht_تخزين(ع، مفتاح، قيمة)` | `snet_dht_store(...)` | تخزين في الجدول الموزع |
| `dht_بحث(ع، مفتاح)` | `snet_dht_find(...)` | بحث في الجدول الموزع |
| `تخزين_ملف(ع، اسم، محتوى)` | `snet_store_file(...)` | تخزين ملف موزع |
| `استرجاع_ملف(ع، معرّف)` | `snet_retrieve_file(...)` | استرجاع ملف موزع |
| `dns_تسجيل(ع، اسم، عنوان)` | `snet_dns_register(...)` | تسجيل اسم DNS |
| `dns_حل(ع، اسم)` | `snet_dns_resolve(...)` | حل اسم DNS |
| `رسالة_مشفرة(ع، مستقبل، نص)` | `snet_send_encrypted(...)` | إرسال رسالة E2EE |
| `إثبات_عمل(ع)` | `snet_proof_of_work(n)` | إنشاء إثبات عمل |
| `سمعة_قرين(ع، قرين)` | `snet_peer_reputation(...)` | سمعة قرين |
| `حظر_قرين(ع، قرين)` | `snet_ban_peer(...)` | حظر قرين |
| `قرين_موثوق(ع، قرين)` | `snet_is_trusted(...)` | هل القرين موثوق؟ |
| `هوية_جديدة(ع)` | `snet_create_identity(n)` | إنشاء هوية DID |
| `بث_جديد(ع، عنوان)` | `snet_create_stream(...)` | بث مباشر جديد |
| `بث_إيقاف(ع، معرّف)` | `snet_stop_stream(...)` | إيقاف بث |
| `cdn_نشر(ع، اسم، نوع، محتوى)` | `snet_cdn_publish(...)` | نشر محتوى CDN |
| `cdn_محتوى(ع، معرّف)` | `snet_cdn_get(...)` | استرجاع محتوى CDN |
| `snet_إصدار()` | `snet_version()` | إصدار المكتبة |

## البناء

### كمكتبة مستقلة

```bash
# إضافة في CMakeLists.txt الرئيسي:
include(cmake/sadnet.cmake)
```

### ربط مع المفسر

```bash
# في cmake/executables.cmake أو CMakeLists.txt:
target_sources(sad_core PRIVATE
    interpreter_new/src/builtins/builtin_module_sadnet.cpp
)
target_link_libraries(sad_core PRIVATE sadnet)
```

### وضع Freestanding (بدون نظام تشغيل)

```bash
cmake -DSADNET_FREESTANDING=ON ..
```

في هذا الوضع:
- لا يتم ربط `ws2_32` أو `pthread`
- يجب توفير `NetworkCallbacks` من النواة
- تُستخدم `sadnet/freestanding/freestanding.h`

```cpp
// في كود النواة (kernel):
#include <sadnet/freestanding/freestanding.h>

sad::net::freestanding::NetworkCallbacks cb;
cb.udp_send = my_kernel_udp_send;
cb.udp_recv = my_kernel_udp_recv;
cb.get_time_ms = my_kernel_get_time;
cb.get_random = my_kernel_get_random;
sad::net::freestanding::init(cb);
```

## البروتوكولات والمعايير

| البروتوكول | المعيار | الملف |
|-----------|---------|-------|
| SHA-256 | FIPS 180-4 | `crypto/sha256.cpp` |
| AES-256-GCM | NIST SP 800-38D | `crypto/aes_gcm.cpp` |
| X25519 | RFC 7748 | `crypto/x25519.cpp` |
| HKDF-SHA256 | RFC 5869 | `crypto/sha256.cpp` |
| Double Ratchet | Signal Protocol | `crypto/x25519.cpp` |
| Kademlia DHT | Maymounkov & Mazières 2002 | `dht/` |
| STUN | RFC 5389 / RFC 3489 | `nat/stun.cpp` |
| TURN | RFC 5766 | `nat/turn.cpp` |
| ICE | RFC 8445 | `nat/ice.cpp` |

## هيكل الملفات

```
features/network/sadnet/
├── include/sadnet/
│   ├── core/types.h            # NodeId, Buffer, Result, NetError
│   ├── core/transport.h        # UdpSocket, TcpSocket
│   ├── crypto/crypto.h         # SHA-256, AES-GCM, X25519, HKDF
│   ├── dht/dht.h               # KBucket, RoutingTable, DHTEngine
│   ├── nat/nat.h               # STUN, TURN, ICE
│   ├── discovery/discovery.h   # LANDiscovery, PEX
│   ├── messaging/messaging.h   # E2EE, ChatSession, GroupSession
│   ├── routing/routing.h       # OnionRouter, Circuit
│   ├── identity/identity.h     # DIDDocument, Credentials
│   ├── dns/dns.h               # DNSManager, DNSRecord
│   ├── storage/storage.h       # Chunk, FileManifest, StorageManager
│   ├── security/security.h     # PoW, Reputation, TrustCertificate
│   ├── streaming/streaming.h   # StreamPublisher, StreamViewer
│   ├── cdn/cdn.h               # CDNManager, ContentEntry
│   └── freestanding/           # طبقة التوافق بدون نظام تشغيل
│       └── freestanding.h
├── src/
│   ├── core/types.cpp
│   ├── core/transport.cpp
│   ├── crypto/sha256.cpp
│   ├── crypto/aes_gcm.cpp
│   ├── crypto/x25519.cpp
│   ├── dht/routing_table.cpp
│   ├── dht/dht_engine.cpp
│   ├── nat/stun.cpp
│   ├── nat/turn.cpp
│   ├── nat/ice.cpp
│   ├── discovery/discovery.cpp
│   ├── messaging/messaging.cpp
│   ├── routing/onion_routing.cpp
│   ├── identity/identity.cpp
│   ├── dns/dns.cpp
│   ├── storage/storage.cpp
│   ├── security/security.cpp
│   ├── streaming/streaming.cpp
│   ├── cdn/cdn.cpp
│   └── freestanding/freestanding.cpp
└── tests/
    └── test_sadnet.ص           # اختبارات بلغة ص
```

## الترخيص

جزء من مشروع لغة ص — نفس الترخيص.
