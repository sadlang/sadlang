/**
 * خادم API محلي لتنفيذ كود لغة ص في Playground
 * Local API server to execute Sad code in the Playground
 *
 * الاستخدام / Usage:
 *   node website/api/server.js
 *   # or with custom sad.exe path:
 *   SAD_EXE=./build/bin/Debug/sad.exe node website/api/server.js
 *
 * الـ Endpoint الوحيد / Single endpoint:
 *   POST http://localhost:3731/execute
 *   Body: { "source": "اطبع_سطر(...)", "stdin": "" }
 *   Response: { "success": true, "output": "...", "error": "" }
 */

const http   = require('http')
const path   = require('path')
const fs     = require('fs')
const os     = require('os')
const cp     = require('child_process')
const crypto = require('crypto')

// ── إعداد مسار sad.exe ──────────────────────────────────────────────────────
const WORKSPACE = path.resolve(__dirname, '../..')  // project root

function findSadExe() {
  const candidates = [
    process.env.SAD_EXE,
    path.join(WORKSPACE, 'build/bin/Debug/sad.exe'),
    path.join(WORKSPACE, 'build/bin/Release/sad.exe'),
    path.join(WORKSPACE, 'build/bin/Debug/sad'),
    path.join(WORKSPACE, 'build/bin/Release/sad'),
    // Linux / macOS install
    '/usr/local/bin/sad',
    '/usr/bin/sad',
  ].filter(Boolean)

  for (const p of candidates) {
    try { fs.accessSync(p, fs.constants.X_OK); return p } catch {}
  }
  return null
}

const SAD_EXE = findSadExe()
const PORT    = parseInt(process.env.PORT || '3731', 10)
const TIMEOUT = parseInt(process.env.TIMEOUT || '8000', 10)  // ms
const MAX_OUTPUT = 64 * 1024  // 64 KB max output

// ── CORS headers ─────────────────────────────────────────────────────────────
const CORS = {
  'Access-Control-Allow-Origin' : 'http://localhost:5173,http://localhost:5174,http://localhost:4173',
  'Access-Control-Allow-Methods': 'POST, OPTIONS',
  'Access-Control-Allow-Headers': 'Content-Type',
  'Content-Type'                : 'application/json; charset=utf-8',
}

function writeCors(res, code = 200) {
  res.writeHead(code, CORS)
}

// ── قراءة body كـ JSON ────────────────────────────────────────────────────────
function readBody(req) {
  return new Promise((resolve, reject) => {
    let data = ''
    req.on('data', chunk => { data += chunk })
    req.on('end', () => {
      try { resolve(JSON.parse(data)) }
      catch { reject(new Error('Invalid JSON')) }
    })
    req.on('error', reject)
  })
}

// ── تنفيذ الكود عبر sad.exe ───────────────────────────────────────────────────
function executeSad(source, stdinData = '') {
  return new Promise((resolve) => {
    if (!SAD_EXE) {
      return resolve({ success: false, output: '', error: 'sad.exe not found — install Sad or set SAD_EXE env var' })
    }

    // كتابة ملف مؤقت
    const tmpFile = path.join(os.tmpdir(), `sad_${crypto.randomBytes(6).toString('hex')}.ص`)
    fs.writeFileSync(tmpFile, source, 'utf8')

    let output = ''
    let error  = ''
    let timedOut = false

    const proc = cp.spawn(SAD_EXE, [tmpFile], {
      timeout: TIMEOUT,
      killSignal: 'SIGKILL',
    })

    if (stdinData) {
      proc.stdin.write(stdinData)
      proc.stdin.end()
    }

    proc.stdout.on('data', chunk => {
      output += chunk.toString('utf8')
      if (output.length > MAX_OUTPUT) {
        timedOut = true
        proc.kill('SIGKILL')
      }
    })

    proc.stderr.on('data', chunk => { error += chunk.toString('utf8') })

    proc.on('close', code => {
      try { fs.unlinkSync(tmpFile) } catch {}
      if (timedOut) {
        resolve({ success: false, output, error: '⏱ تجاوز الوقت المحدد / Execution timeout exceeded' })
      } else {
        resolve({ success: code === 0, output, error: error || '' })
      }
    })

    proc.on('error', err => {
      try { fs.unlinkSync(tmpFile) } catch {}
      resolve({ success: false, output: '', error: err.message })
    })
  })
}

// ── الخادم ───────────────────────────────────────────────────────────────────
const server = http.createServer(async (req, res) => {
  // CORS preflight
  if (req.method === 'OPTIONS') {
    writeCors(res, 204)
    res.end()
    return
  }

  // Health check
  if (req.method === 'GET' && req.url === '/health') {
    writeCors(res)
    res.end(JSON.stringify({
      status : 'ok',
      sadExe : SAD_EXE || null,
      version: '1.0.0',
    }))
    return
  }

  // POST /execute
  if (req.method === 'POST' && req.url === '/execute') {
    try {
      const { source = '', stdin: stdinData = '' } = await readBody(req)

      if (!source || typeof source !== 'string') {
        writeCors(res, 400)
        res.end(JSON.stringify({ success: false, output: '', error: 'source field is required' }))
        return
      }

      const result = await executeSad(source, stdinData)
      writeCors(res)
      res.end(JSON.stringify(result))
    } catch (err) {
      writeCors(res, 500)
      res.end(JSON.stringify({ success: false, output: '', error: err.message }))
    }
    return
  }

  writeCors(res, 404)
  res.end(JSON.stringify({ error: 'Not found' }))
})

server.listen(PORT, '127.0.0.1', () => {
  console.log(`\n🚀 خادم لغة ص / Sad API server`)
  console.log(`   الرابط   : http://localhost:${PORT}`)
  console.log(`   sad.exe  : ${SAD_EXE || '❌ not found'}`)
  console.log(`   الحد الزمني: ${TIMEOUT}ms\n`)
  if (!SAD_EXE) {
    console.warn('⚠️  تعذّر إيجاد sad.exe — ابنِ المشروع أولاً أو ضع مساره في متغير SAD_EXE')
    console.warn('   cmake --build build --config Debug --target sad')
  }
})

process.on('SIGTERM', () => server.close())
process.on('SIGINT',  () => { console.log('\nإيقاف الخادم...'); server.close(() => process.exit(0)) })
