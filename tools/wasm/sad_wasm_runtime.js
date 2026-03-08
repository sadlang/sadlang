/**
 * ════════════════════════════════════════════════════════════════════════════════
 *  sad_wasm_runtime.js — وقت التشغيل JavaScript لـ WebAssembly
 *  Sad Language WASM Runtime — provides JS functions imported by WASM
 * ════════════════════════════════════════════════════════════════════════════════
 *
 *  هذا الملف يوفر جميع الدوال التي يستوردها WASM من JavaScript:
 *    - الطباعة والـ console
 *    - DOM manipulation
 *    - Fetch API
 *    - LocalStorage
 *    - الذاكرة
 *
 *  @author فريق لغة ص
 *  @date 2026-03-06
 * ════════════════════════════════════════════════════════════════════════════════
 */

/**
 * إنشاء وقت التشغيل
 * @param {WebAssembly.Memory} memory - ذاكرة WASM
 * @param {object} options - خيارات إضافية
 */
export function createSadRuntime(memory, options = {}) {
    const encoder = new TextEncoder();
    const decoder = new TextDecoder('utf-8');
    
    // مؤشر الكومة
    let heapBase = options.heapBase || 65536;
    
    // ════════════════════════════════════════════════════════════════════════
    //  دوال الذاكرة المساعدة
    // ════════════════════════════════════════════════════════════════════════
    
    /**
     * قراءة نص من الذاكرة (null-terminated)
     */
    function readString(ptr) {
        const mem = new Uint8Array(memory.buffer);
        let end = ptr;
        while (mem[end] !== 0 && end < mem.length) end++;
        return decoder.decode(mem.slice(ptr, end));
    }
    
    /**
     * كتابة نص في الذاكرة
     * @returns {number} مؤشر النص
     */
    function writeString(str) {
        const bytes = encoder.encode(str + '\0');
        const ptr = heapBase;
        new Uint8Array(memory.buffer).set(bytes, ptr);
        heapBase += bytes.length + ((8 - bytes.length % 8) % 8); // محاذاة 8
        return ptr;
    }
    
    /**
     * قراءة UTF-8 مع طول محدد
     */
    function readStringLen(ptr, len) {
        const mem = new Uint8Array(memory.buffer);
        return decoder.decode(mem.slice(ptr, ptr + len));
    }
    
    // ════════════════════════════════════════════════════════════════════════
    //  DOM Cache — لتخزين مراجع العناصر
    // ════════════════════════════════════════════════════════════════════════
    
    const domCache = new Map();
    let nextDomHandle = 1;
    
    function storeDomElement(element) {
        if (!element) return 0;
        const handle = nextDomHandle++;
        domCache.set(handle, element);
        return handle;
    }
    
    function getDomElement(handle) {
        return domCache.get(handle) || null;
    }
    
    // ════════════════════════════════════════════════════════════════════════
    //  Callback Cache — لتخزين مراجع الدوال
    // ════════════════════════════════════════════════════════════════════════
    
    const callbackCache = new Map();
    let wasmInstance = null;
    
    function setWasmInstance(instance) {
        wasmInstance = instance;
    }
    
    function invokeCallback(funcPtr, ...args) {
        if (wasmInstance && wasmInstance.exports.table) {
            const fn = wasmInstance.exports.table.get(funcPtr);
            if (fn) return fn(...args);
        }
        return 0;
    }
    
    // ════════════════════════════════════════════════════════════════════════
    //  عنصر الإخراج (للـ اطبع)
    // ════════════════════════════════════════════════════════════════════════
    
    let outputElement = options.outputElement || null;
    let outputCallback = options.onOutput || console.log;
    
    function appendOutput(text) {
        if (outputElement) {
            outputElement.textContent += text;
        }
        if (outputCallback) {
            outputCallback(text);
        }
    }
    
    // ════════════════════════════════════════════════════════════════════════
    //  الدوال المُصدَّرة لـ WASM
    // ════════════════════════════════════════════════════════════════════════
    
    return {
        // ─── إعداد ───
        _setWasmInstance: setWasmInstance,
        _setOutputElement: (el) => { outputElement = el; },
        _setOutputCallback: (cb) => { outputCallback = cb; },
        
        // ════════════════════════════════════════════════════════════════════
        //  الطباعة / Print
        // ════════════════════════════════════════════════════════════════════
        
        __sad_print: (ptr) => {
            const text = readString(ptr);
            appendOutput(text);
        },
        
        __sad_print_int: (n) => {
            appendOutput(String(n));
        },
        
        __sad_print_float: (n) => {
            appendOutput(String(n));
        },
        
        __sad_println: (ptr) => {
            const text = readString(ptr);
            appendOutput(text + '\n');
        },
        
        // ════════════════════════════════════════════════════════════════════
        //  الرياضيات / Math
        // ════════════════════════════════════════════════════════════════════
        
        __sad_sqrt: (x) => Math.sqrt(x),
        __sad_pow: (base, exp) => Math.pow(base, exp),
        __sad_sin: (x) => Math.sin(x),
        __sad_cos: (x) => Math.cos(x),
        __sad_random: () => Math.random(),
        
        // ════════════════════════════════════════════════════════════════════
        //  الذاكرة / Memory
        // ════════════════════════════════════════════════════════════════════
        
        __sad_malloc: (size) => {
            const ptr = heapBase;
            heapBase += size + ((8 - size % 8) % 8);
            return ptr;
        },
        
        __sad_free: (ptr) => {
            // GC will handle — no-op in browser
        },
        
        // ════════════════════════════════════════════════════════════════════
        //  النصوص / Strings
        // ════════════════════════════════════════════════════════════════════
        
        __sad_str_len: (ptr) => {
            const mem = new Uint8Array(memory.buffer);
            let len = 0;
            while (mem[ptr + len] !== 0) len++;
            return len;
        },
        
        __sad_str_concat: (ptr1, ptr2) => {
            const str1 = readString(ptr1);
            const str2 = readString(ptr2);
            return writeString(str1 + str2);
        },
        
        // ════════════════════════════════════════════════════════════════════
        //  DOM
        // ════════════════════════════════════════════════════════════════════
        
        __sad_dom_query: (selectorPtr) => {
            const selector = readString(selectorPtr);
            const element = document.querySelector(selector);
            return storeDomElement(element);
        },
        
        __sad_dom_create: (tagPtr) => {
            const tag = readString(tagPtr);
            const element = document.createElement(tag);
            return storeDomElement(element);
        },
        
        __sad_dom_set_text: (handle, textPtr) => {
            const element = getDomElement(handle);
            if (element) {
                element.textContent = readString(textPtr);
            }
        },
        
        __sad_dom_set_attr: (handle, namePtr, valuePtr) => {
            const element = getDomElement(handle);
            if (element) {
                const name = readString(namePtr);
                const value = readString(valuePtr);
                element.setAttribute(name, value);
            }
        },
        
        __sad_dom_append: (parentHandle, childHandle) => {
            const parent = getDomElement(parentHandle);
            const child = getDomElement(childHandle);
            if (parent && child) {
                parent.appendChild(child);
            }
        },
        
        __sad_dom_remove: (handle) => {
            const element = getDomElement(handle);
            if (element && element.parentNode) {
                element.parentNode.removeChild(element);
            }
            domCache.delete(handle);
        },
        
        __sad_dom_on_click: (handle, callbackPtr) => {
            const element = getDomElement(handle);
            if (element) {
                element.addEventListener('click', () => {
                    invokeCallback(callbackPtr, handle);
                });
            }
        },
        
        __sad_dom_on_input: (handle, callbackPtr) => {
            const element = getDomElement(handle);
            if (element) {
                element.addEventListener('input', (e) => {
                    const valuePtr = writeString(e.target.value);
                    invokeCallback(callbackPtr, handle, valuePtr);
                });
            }
        },
        
        // ════════════════════════════════════════════════════════════════════
        //  Console
        // ════════════════════════════════════════════════════════════════════
        
        __sad_console_log: (ptr) => {
            console.log(readString(ptr));
        },
        
        __sad_console_error: (ptr) => {
            console.error(readString(ptr));
        },
        
        __sad_console_time: (ptr) => {
            console.time(readString(ptr));
        },
        
        __sad_console_timeEnd: (ptr) => {
            console.timeEnd(readString(ptr));
        },
        
        // ════════════════════════════════════════════════════════════════════
        //  الشبكة / Fetch
        // ════════════════════════════════════════════════════════════════════
        
        __sad_fetch_start: async (urlPtr, optionsPtr) => {
            const url = readString(urlPtr);
            try {
                const response = await fetch(url);
                const text = await response.text();
                return writeString(text);
            } catch (e) {
                console.error('Fetch error:', e);
                return 0;
            }
        },
        
        __sad_fetch_result: (handle) => {
            // TODO: استدعاء غير متزامن
            return 0;
        },
        
        // ════════════════════════════════════════════════════════════════════
        //  التخزين المحلي / LocalStorage
        // ════════════════════════════════════════════════════════════════════
        
        __sad_storage_get: (keyPtr) => {
            const key = readString(keyPtr);
            const value = localStorage.getItem(key);
            return value ? writeString(value) : 0;
        },
        
        __sad_storage_set: (keyPtr, valuePtr) => {
            const key = readString(keyPtr);
            const value = readString(valuePtr);
            localStorage.setItem(key, value);
        },
    };
}

/**
 * تحميل وتشغيل ملف WASM
 * @param {string} wasmPath - مسار ملف .wasm
 * @param {object} options - خيارات إضافية
 */
export async function loadSadWasm(wasmPath, options = {}) {
    // إنشاء الذاكرة
    const memory = new WebAssembly.Memory({
        initial: options.initialMemory || 16,
        maximum: options.maxMemory || 256,
    });
    
    // إنشاء وقت التشغيل
    const runtime = createSadRuntime(memory, options);
    
    // كائن الاستيراد
    const importObject = {
        env: {
            memory,
            ...runtime,
        },
        wasi_snapshot_preview1: {
            fd_write: () => {},
            fd_close: () => {},
            fd_seek: () => {},
            proc_exit: (code) => {
                console.log('Program exited with code:', code);
            },
        },
    };
    
    // تحميل WASM
    const response = await fetch(wasmPath);
    const bytes = await response.arrayBuffer();
    const { instance } = await WebAssembly.instantiate(bytes, importObject);
    
    // تمرير المثيل لوقت التشغيل
    runtime._setWasmInstance(instance);
    
    return {
        instance,
        memory,
        runtime,
        
        /**
         * استدعاء الدالة الرئيسية
         */
        run: () => {
            if (instance.exports._start) {
                return instance.exports._start();
            } else if (instance.exports.رئيسية) {
                return instance.exports.رئيسية();
            } else if (instance.exports.main) {
                return instance.exports.main();
            }
            throw new Error('No entry point found (main, رئيسية, or _start)');
        },
        
        /**
         * استدعاء دالة بالاسم
         */
        call: (name, ...args) => {
            const fn = instance.exports[name];
            if (fn) return fn(...args);
            throw new Error(`Function not found: ${name}`);
        },
    };
}

// ════════════════════════════════════════════════════════════════════════════════
//  DOM API العربي
// ════════════════════════════════════════════════════════════════════════════════

export const dom = {
    // استعلام
    استعلم: (selector) => document.querySelector(selector),
    استعلم_الكل: (selector) => [...document.querySelectorAll(selector)],
    بالمعرف: (id) => document.getElementById(id),
    
    // إنشاء
    أنشئ: (tag) => document.createElement(tag),
    أنشئ_نص: (text) => document.createTextNode(text),
    
    // تعديل
    أضف_ابن: (parent, child) => parent.appendChild(child),
    أزل_ابن: (parent, child) => parent.removeChild(child),
    عيّن_سمة: (el, name, value) => el.setAttribute(name, value),
    اقرأ_سمة: (el, name) => el.getAttribute(name),
    
    // محتوى
    نص: (el, text) => { if (text !== undefined) el.textContent = text; return el.textContent; },
    html: (el, html) => { if (html !== undefined) el.innerHTML = html; return el.innerHTML; },
    قيمة: (el, val) => { if (val !== undefined) el.value = val; return el.value; },
    
    // أصناف CSS
    أضف_صنف: (el, cls) => el.classList.add(cls),
    أزل_صنف: (el, cls) => el.classList.remove(cls),
    بدّل_صنف: (el, cls) => el.classList.toggle(cls),
    
    // أحداث
    عند: (el, event, handler) => el.addEventListener(event, handler),
    أزل_عند: (el, event, handler) => el.removeEventListener(event, handler),
};

// أسماء الأحداث بالعربية
export const أحداث = {
    نقر: 'click',
    نقر_مزدوج: 'dblclick',
    ضغط_مفتاح: 'keydown',
    رفع_مفتاح: 'keyup',
    إدخال: 'input',
    تغيير: 'change',
    إرسال: 'submit',
    تحميل: 'load',
};
