// ═══════════════════════════════════════════════════════════════
// محمّل WASM لمفسر لغة ص — Sad WASM Loader
// ═══════════════════════════════════════════════════════════════
import { ref, readonly } from 'vue'

interface WasmResult {
  success: boolean
  output: string
  error: string
}

interface SadWasmModule {
  cwrap: any
  UTF8ToString: (ptr: number) => string
  stringToUTF8: (str: string, ptr: number, maxBytes: number) => void
  lengthBytesUTF8: (str: string) => number
  _malloc: (size: number) => number
  _free: (ptr: number) => void
}

const wasmModule = ref<SadWasmModule | null>(null)
const loading = ref(false)
const loaded = ref(false)
const error = ref<string | null>(null)

let sadExecute: ((src: string, stdin: string) => string) | null = null

async function loadWasm(): Promise<boolean> {
  if (loaded.value) return true
  if (loading.value) {
    // Wait for existing load to complete
    return new Promise((resolve) => {
      const check = setInterval(() => {
        if (!loading.value) { clearInterval(check); resolve(loaded.value) }
      }, 100)
    })
  }

  loading.value = true
  error.value = null

  try {
    // Dynamic import of the WASM JS module from public directory
    const script = document.createElement('script')
    script.src = '/sad.js'

    await new Promise<void>((resolve, reject) => {
      script.onload = () => resolve()
      script.onerror = () => reject(new Error('فشل تحميل ملف sad.js'))
      document.head.appendChild(script)
    })

    // SadWasm is now a global factory function
    const factory = (window as any).SadWasm
    if (!factory) throw new Error('SadWasm factory not found')

    const mod = await factory({
      locateFile: (path: string) => {
        if (path.endsWith('.wasm')) return '/sad.wasm'
        return path
      }
    }) as SadWasmModule

    wasmModule.value = mod

    // Create the wrapped execute function
    const _execute = mod.cwrap('sad_execute', 'number', ['string', 'string'])

    sadExecute = (source: string, stdin: string = ''): string => {
      const ptr = _execute(source, stdin)
      const result = mod.UTF8ToString(ptr)
      mod._free(ptr)
      return result
    }

    loaded.value = true

    // Expose on window for SadCodeRunner compatibility
    ;(window as any).__sadWasm = {
      runCode: (code: string, stdin?: string): { output: string; error: string } => {
        try {
          const json = sadExecute!(code, stdin || '')
          const parsed: WasmResult = JSON.parse(json)
          return {
            output: parsed.output || '',
            error: parsed.error || '',
          }
        } catch (e: any) {
          return { output: '', error: `خطأ WASM: ${e?.message}` }
        }
      }
    }

    return true
  } catch (e: any) {
    error.value = e?.message || 'فشل تحميل WASM'
    loaded.value = false
    return false
  } finally {
    loading.value = false
  }
}

export function useSadWasm() {
  return {
    loading: readonly(loading),
    loaded: readonly(loaded),
    error: readonly(error),
    loadWasm,

    runCode(code: string, stdin: string = ''): { output: string; error: string } {
      if (!sadExecute) return { output: '', error: 'WASM غير محمّل بعد' }
      try {
        const json = sadExecute(code, stdin)
        const parsed: WasmResult = JSON.parse(json)
        return { output: parsed.output || '', error: parsed.error || '' }
      } catch (e: any) {
        return { output: '', error: `خطأ: ${e?.message}` }
      }
    }
  }
}
