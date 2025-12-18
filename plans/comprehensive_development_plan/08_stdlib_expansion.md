# 📝 المرحلة 8: توسيع المكتبة القياسية
## Phase 8: Standard Library Expansion

**المدة:** أسبوعان (10 أيام)  
**الأولوية:** 🟡 P2 - متوسطة  
**التبعيات:** المرحلة 1-7

---

## 🎯 الأهداف

### من 44 دالة إلى 120+ دالة
### 15+ وحدات (modules) جديدة

---

## 📋 الوحدات الجديدة

### المهمة 8.1: Network Module (يومان)

```s
استورد شبكة

// HTTP requests
var response = شبكة.get("https://api.example.com")
print(response.status)
print(response.body)

// POST request
var data = {"name": "Ahmed", "age": 25}
شبكة.post("https://api.example.com", data)

// TCP sockets
var socket = شبكة.socket("127.0.0.1", 8080)
socket.send("Hello")
var msg = socket.receive()
socket.close()

// WebSocket
var ws = شبكة.websocket("ws://example.com")
ws.on_message(function(msg) { print(msg) })
```

**Implementation:**
- HTTP client (GET، POST، PUT، DELETE)
- TCP/UDP sockets
- WebSocket support
- SSL/TLS support
- Async networking

### المهمة 8.2: File I/O Advanced (يوم)

```s
استورد ملفات

// Path operations
var path = ملفات.join("مجلد", "ملف.txt")
var exists = ملفات.exists(path)
var absolute = ملفات.absolute(path)

// Directory operations
ملفات.mkdir("مجلد_جديد")
ملفات.rmdir("مجلد_قديم")
var files = ملفات.listdir(".")

// File watching
var watcher = ملفات.watch("*.txt", function(event, file) {
    print("File changed: " + file)
})

// Binary files
var data = ملفات.read_bytes("image.png")
ملفات.write_bytes("copy.png", data)
```

**Implementation:**
- Path manipulation (join، split، extension)
- Directory operations (create، delete، list)
- File watching (inotify/FSEvents)
- Binary file operations
- Temporary files

### المهمة 8.3: Crypto Module (يوم)

```s
استورد تشفير

// Hashing
var hash = تشفير.sha256("password")
var md5 = تشفير.md5("data")

// Encryption (AES)
var key = تشفير.generate_key()
var encrypted = تشفير.encrypt("secret message", key)
var decrypted = تشفير.decrypt(encrypted, key)

// RSA
var keypair = تشفير.generate_keypair()
var signature = تشفير.sign("message", keypair.private)
var valid = تشفير.verify("message", signature, keypair.public)

// Random
var random_bytes = تشفير.random_bytes(32)
var random_int = تشفير.random_int(0, 100)
```

**Implementation:**
- Hash functions (MD5، SHA-1، SHA-256، SHA-512)
- Symmetric encryption (AES، ChaCha20)
- Asymmetric encryption (RSA، Ed25519)
- HMAC
- Secure random

### المهمة 8.4: JSON/XML Module (يوم)

```s
استورد json

// Parse JSON
var data = json.parse('{"name": "Ahmed", "age": 25}')
print(data.name)

// Stringify
var obj = {"key": "value", "number": 42}
var json_str = json.stringify(obj)

// Pretty print
print(json.stringify(obj, indent=2))

استورد xml

// Parse XML
var doc = xml.parse("<root><child>text</child></root>")
var child = doc.find("child")
print(child.text)

// Build XML
var root = xml.Element("root")
var child = xml.SubElement(root, "child")
child.text = "Hello"
print(xml.tostring(root))
```

### المهمة 8.5: DateTime Module (يوم)

```s
استورد تاريخ

// Current time
var now = تاريخ.now()
print(now.year, now.month, now.day)
print(now.hour, now.minute, now.second)

// Formatting
print(now.format("%Y-%m-%d %H:%M:%S"))
print(now.format_arabic("ت:د:س ي/ش/س"))

// Parsing
var date = تاريخ.parse("2025-12-18", "%Y-%m-%d")

// Arithmetic
var tomorrow = now.add_days(1)
var next_week = now.add_weeks(1)
var diff = tomorrow - now

// Timezone
var utc = now.to_utc()
var cairo = now.to_timezone("Africa/Cairo")
```

### المهمة 8.6: Regex Module (يوم)

```s
استورد regex

// Match
var pattern = regex.compile(r"\d+")
var match = pattern.match("123abc")
if (match) {
    print(match.group(0))  // "123"
}

// Find all
var matches = pattern.findall("123 456 789")
// ["123", "456", "789"]

// Replace
var result = pattern.replace("123abc456", "XXX")
// "XXXabcXXX"

// Split
var parts = regex.split(r"\s+", "one  two   three")
// ["one", "two", "three"]
```

### المهمة 8.7: Threading Module (يوم)

```s
استورد خيوط

// Create thread
function worker(id) {
    print("Thread " + id + " running")
}

var thread1 = خيوط.Thread(worker, [1])
var thread2 = خيوط.Thread(worker, [2])

thread1.start()
thread2.start()

thread1.join()
thread2.join()

// Lock
var lock = خيوط.Lock()

function critical_section() {
    lock.acquire()
    // Critical code
    lock.release()
}

// Thread pool
var pool = خيوط.ThreadPool(4)
pool.submit(worker, [1])
pool.submit(worker, [2])
pool.shutdown()
```

### المهمة 8.8: Collections Module (يوم)

```s
استورد مجموعات

// Deque
var deque = مجموعات.Deque([1, 2, 3])
deque.append(4)
deque.appendleft(0)
deque.pop()
deque.popleft()

// OrderedDict
var ordered = مجموعات.OrderedDict()
ordered["b"] = 2
ordered["a"] = 1
// Maintains insertion order

// Counter
var counter = مجموعات.Counter([1, 2, 2, 3, 3, 3])
// {1: 1, 2: 2, 3: 3}

// DefaultDict
var dd = مجموعات.DefaultDict(function() { return [] })
dd["key"].append(1)  // Creates list automatically
```

### المهمة 8.9: Random Module (نصف يوم)

```s
استورد عشوائي

// Random numbers
var r = عشوائي.random()  // 0.0 to 1.0
var i = عشوائي.randint(1, 100)
var f = عشوائي.uniform(0.0, 10.0)

// Choice
var choice = عشوائي.choice([1, 2, 3, 4, 5])

// Shuffle
var list = [1, 2, 3, 4, 5]
عشوائي.shuffle(list)

// Sample
var sample = عشوائي.sample([1, 2, 3, 4, 5], 3)
```

### المهمة 8.10: OS Module (نصف يوم)

```s
استورد os

// Environment
var path = os.getenv("PATH")
os.setenv("MY_VAR", "value")

// Process
var pid = os.getpid()
os.system("ls -la")

// Platform
print(os.platform())  // "windows", "linux", "macos"
print(os.arch())      // "x86_64", "aarch64"
```

---

## 📅 الجدول الزمني

| اليوم | المهمة | الساعات |
|------|---------|----------|
| 1-2 | Network module | 16 |
| 3 | File I/O advanced | 8 |
| 4 | Crypto module | 8 |
| 5 | JSON/XML | 8 |
| 6 | DateTime | 8 |
| 7 | Regex | 8 |
| 8 | Threading | 8 |
| 9 | Collections | 8 |
| 10 | Random + OS + testing | 8 |

**المجموع:** 80 ساعة

---

## 🎯 النتائج المتوقعة

- ✅ 120+ دالة في المكتبة القياسية
- ✅ 15+ وحدات جديدة
- ✅ 100% موثقة بالعربية والإنجليزية
- ✅ اختبارات شاملة لكل وحدة
- ✅ أمثلة عملية لكل ميزة

---

**الحالة:** 📝 جاهز للتنفيذ
