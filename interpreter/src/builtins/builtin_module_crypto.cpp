/**
 * @file builtin_module_crypto.cpp
 * @brief (AR) وحدة تشفير — دوال هاش/MAC حديثة (BLAKE3)، ذاتيّة التنفيذ بالكامل
 * @brief (EN) Crypto module — modern hash/MAC functions (BLAKE3), fully self-implemented
 *
 * @details
 * (AR) تنفيذ BLAKE3 مرجعيّ محمول (بلا SIMD/تعدّد خيوط) — مطابق حرفيًّا لنظير
 *      المترجم في tools/compiler/runtime/sad_embedded_runtime.c (دوال
 *      sad_blake3_*). كلاهما تحقّق بنجاح مقابل شعاعات BLAKE3 الرسميّة
 *      (test_vectors.json من مستودع BLAKE3-team) قبل الدمج.
 *
 * @see builtin_registry.cpp — التسجيل المركزي
 */

#include "builtin_common.h"
#include "builtin_registry.h"
namespace Bcr = Sad::Builtins::Names::Crypto;
#include <cstdint>
#include <cstring>
#include <random>
#include <vector>

namespace Sad
{
    namespace Interpreter
    {
        namespace CryptoDetail
        {
            // ════════════════════════════════════════════════════════════
            // (AR) تنفيذ BLAKE3 مرجعيّ — مطابق للنسخة C في وقت تشغيل المترجم
            // (EN) Reference BLAKE3 — must stay byte-identical to the C
            //      version in the compiler's embedded runtime.
            // ════════════════════════════════════════════════════════════
            constexpr uint32_t IV[8] = {
                0x6A09E667u, 0xBB67AE85u, 0x3C6EF372u, 0xA54FF53Au,
                0x510E527Fu, 0x9B05688Cu, 0x1F83D9ABu, 0x5BE0CD19u};
            constexpr int MSG_PERM[16] = {2, 6, 3, 10, 7, 0, 4, 13, 1, 11, 12, 5, 9, 14, 15, 8};

            constexpr uint32_t CHUNK_START = 1u;
            constexpr uint32_t CHUNK_END = 2u;
            constexpr uint32_t PARENT = 4u;
            constexpr uint32_t ROOT = 8u;
            constexpr uint32_t KEYED_HASH = 16u;
            constexpr int BLOCK_LEN = 64;
            constexpr int CHUNK_LEN = 1024;

            inline uint32_t rotr32(uint32_t x, int n) { return (x >> n) | (x << (32 - n)); }

            inline void g(uint32_t *st, int a, int b, int c, int d, uint32_t mx, uint32_t my)
            {
                st[a] = st[a] + st[b] + mx;
                st[d] = rotr32(st[d] ^ st[a], 16);
                st[c] = st[c] + st[d];
                st[b] = rotr32(st[b] ^ st[c], 12);
                st[a] = st[a] + st[b] + my;
                st[d] = rotr32(st[d] ^ st[a], 8);
                st[c] = st[c] + st[d];
                st[b] = rotr32(st[b] ^ st[c], 7);
            }

            inline void round_fn(uint32_t *st, const uint32_t *m)
            {
                g(st, 0, 4, 8, 12, m[0], m[1]);
                g(st, 1, 5, 9, 13, m[2], m[3]);
                g(st, 2, 6, 10, 14, m[4], m[5]);
                g(st, 3, 7, 11, 15, m[6], m[7]);
                g(st, 0, 5, 10, 15, m[8], m[9]);
                g(st, 1, 6, 11, 12, m[10], m[11]);
                g(st, 2, 7, 8, 13, m[12], m[13]);
                g(st, 3, 4, 9, 14, m[14], m[15]);
            }

            inline void permute(uint32_t *m)
            {
                uint32_t t[16];
                for (int i = 0; i < 16; ++i)
                    t[i] = m[MSG_PERM[i]];
                std::memcpy(m, t, sizeof(t));
            }

            inline void compress(const uint32_t cv[8], const uint32_t block_words[16],
                                  uint64_t counter, uint32_t block_len, uint32_t flags,
                                  uint32_t out[16])
            {
                uint32_t st[16];
                uint32_t m[16];
                std::memcpy(st, cv, 8 * sizeof(uint32_t));
                std::memcpy(st + 8, IV, 4 * sizeof(uint32_t));
                st[12] = (uint32_t)(counter & 0xFFFFFFFFu);
                st[13] = (uint32_t)(counter >> 32);
                st[14] = block_len;
                st[15] = flags;
                std::memcpy(m, block_words, 16 * sizeof(uint32_t));
                for (int r = 0; r < 7; ++r)
                {
                    round_fn(st, m);
                    if (r < 6)
                        permute(m);
                }
                for (int i = 0; i < 8; ++i)
                {
                    out[i] = st[i] ^ st[i + 8];
                    out[i + 8] = st[i + 8] ^ cv[i];
                }
            }

            inline void words_from_bytes_padded(const uint8_t *b, size_t len, uint32_t out[16])
            {
                uint8_t buf[64];
                std::memset(buf, 0, 64);
                if (len)
                    std::memcpy(buf, b, len);
                for (int i = 0; i < 16; ++i)
                    out[i] = (uint32_t)buf[i * 4] | ((uint32_t)buf[i * 4 + 1] << 8) |
                             ((uint32_t)buf[i * 4 + 2] << 16) | ((uint32_t)buf[i * 4 + 3] << 24);
            }

            struct ChunkState
            {
                uint32_t cv[8];
                uint64_t chunk_counter;
                uint8_t block[BLOCK_LEN];
                size_t block_len = 0;
                int blocks_compressed = 0;
                uint32_t flags;

                void init(const uint32_t key[8], uint64_t counter, uint32_t f)
                {
                    std::memcpy(cv, key, 8 * sizeof(uint32_t));
                    chunk_counter = counter;
                    block_len = 0;
                    blocks_compressed = 0;
                    flags = f;
                }
                size_t len() const { return (size_t)BLOCK_LEN * blocks_compressed + block_len; }
                uint32_t start_flag() const { return blocks_compressed == 0 ? CHUNK_START : 0; }

                void update(const uint8_t *data, size_t n)
                {
                    while (n > 0)
                    {
                        if (block_len == BLOCK_LEN)
                        {
                            uint32_t block_words[16];
                            uint32_t out[16];
                            words_from_bytes_padded(block, BLOCK_LEN, block_words);
                            compress(cv, block_words, chunk_counter, BLOCK_LEN,
                                     flags | start_flag(), out);
                            std::memcpy(cv, out, 8 * sizeof(uint32_t));
                            blocks_compressed++;
                            block_len = 0;
                        }
                        size_t take = (size_t)BLOCK_LEN - block_len;
                        if (take > n)
                            take = n;
                        std::memcpy(block + block_len, data, take);
                        block_len += take;
                        data += take;
                        n -= take;
                    }
                }
            };

            struct Output
            {
                uint32_t input_cv[8];
                uint32_t block_words[16];
                uint64_t counter;
                uint32_t block_len;
                uint32_t flags;

                void chaining_value(uint32_t cv[8]) const
                {
                    uint32_t out[16];
                    compress(input_cv, block_words, counter, block_len, flags, out);
                    std::memcpy(cv, out, 8 * sizeof(uint32_t));
                }
                void root_bytes(uint8_t *out, size_t out_len) const
                {
                    uint64_t block_counter = 0;
                    size_t written = 0;
                    while (written < out_len)
                    {
                        uint32_t words[16];
                        compress(input_cv, block_words, block_counter, block_len,
                                 flags | ROOT, words);
                        for (int i = 0; i < 16 && written < out_len; ++i)
                        {
                            uint8_t b[4] = {(uint8_t)words[i], (uint8_t)(words[i] >> 8),
                                            (uint8_t)(words[i] >> 16), (uint8_t)(words[i] >> 24)};
                            size_t n = 4;
                            if (n > out_len - written)
                                n = out_len - written;
                            std::memcpy(out + written, b, n);
                            written += n;
                        }
                        block_counter++;
                    }
                }
            };

            inline void chunk_state_output(const ChunkState &cs, Output &out)
            {
                std::memcpy(out.input_cv, cs.cv, 8 * sizeof(uint32_t));
                words_from_bytes_padded(cs.block, cs.block_len, out.block_words);
                out.counter = cs.chunk_counter;
                out.block_len = (uint32_t)cs.block_len;
                out.flags = cs.flags | cs.start_flag() | CHUNK_END;
            }

            inline void parent_output(const uint32_t left_cv[8], const uint32_t right_cv[8],
                                       const uint32_t key[8], uint32_t flags, Output &out)
            {
                std::memcpy(out.input_cv, key, 8 * sizeof(uint32_t));
                std::memcpy(out.block_words, left_cv, 8 * sizeof(uint32_t));
                std::memcpy(out.block_words + 8, right_cv, 8 * sizeof(uint32_t));
                out.counter = 0;
                out.block_len = BLOCK_LEN;
                out.flags = flags | PARENT;
            }

            struct Hasher
            {
                uint32_t key[8];
                ChunkState chunk_state;
                uint32_t cv_stack[54][8];
                int cv_stack_len = 0;
                uint32_t flags;

                void init_internal(const uint32_t k[8], uint32_t f)
                {
                    std::memcpy(key, k, 8 * sizeof(uint32_t));
                    chunk_state.init(k, 0, f);
                    cv_stack_len = 0;
                    flags = f;
                }
                void init() { init_internal(IV, 0); }
                void init_keyed(const uint8_t k[32])
                {
                    uint32_t key_words[8];
                    for (int i = 0; i < 8; ++i)
                        key_words[i] = (uint32_t)k[i * 4] | ((uint32_t)k[i * 4 + 1] << 8) |
                                       ((uint32_t)k[i * 4 + 2] << 16) | ((uint32_t)k[i * 4 + 3] << 24);
                    init_internal(key_words, KEYED_HASH);
                }

                void add_chunk_cv(uint32_t new_cv[8], uint64_t total_chunks)
                {
                    while ((total_chunks & 1) == 0)
                    {
                        uint32_t left[8];
                        cv_stack_len--;
                        std::memcpy(left, cv_stack[cv_stack_len], 8 * sizeof(uint32_t));
                        Output po;
                        parent_output(left, new_cv, key, flags, po);
                        po.chaining_value(new_cv);
                        total_chunks >>= 1;
                    }
                    std::memcpy(cv_stack[cv_stack_len], new_cv, 8 * sizeof(uint32_t));
                    cv_stack_len++;
                }

                void update(const uint8_t *data, size_t len)
                {
                    while (len > 0)
                    {
                        if (chunk_state.len() == (size_t)CHUNK_LEN)
                        {
                            Output co;
                            uint32_t chunk_cv[8];
                            chunk_state_output(chunk_state, co);
                            co.chaining_value(chunk_cv);
                            uint64_t total_chunks = chunk_state.chunk_counter + 1;
                            add_chunk_cv(chunk_cv, total_chunks);
                            chunk_state.init(key, total_chunks, flags);
                        }
                        size_t take = (size_t)CHUNK_LEN - chunk_state.len();
                        if (take > len)
                            take = len;
                        chunk_state.update(data, take);
                        data += take;
                        len -= take;
                    }
                }

                void finalize(uint8_t *out, size_t out_len)
                {
                    Output output;
                    chunk_state_output(chunk_state, output);
                    int remaining = cv_stack_len;
                    while (remaining > 0)
                    {
                        uint32_t cv[8];
                        remaining--;
                        output.chaining_value(cv);
                        Output po;
                        parent_output(cv_stack[remaining], cv, key, flags, po);
                        output = po;
                    }
                    output.root_bytes(out, out_len);
                }
            };

            inline std::string to_hex(const uint8_t *b, size_t n)
            {
                static const char *hx = "0123456789abcdef";
                std::string s;
                s.resize(n * 2);
                for (size_t i = 0; i < n; ++i)
                {
                    s[i * 2] = hx[b[i] >> 4];
                    s[i * 2 + 1] = hx[b[i] & 0xF];
                }
                return s;
            }

            inline int hex_nibble(char ch)
            {
                if (ch >= '0' && ch <= '9') return ch - '0';
                if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
                if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
                return -1;
            }

            inline std::string blake3_hash_hex(const std::string &data)
            {
                Hasher h;
                h.init();
                h.update(reinterpret_cast<const uint8_t *>(data.data()), data.size());
                uint8_t digest[32];
                h.finalize(digest, 32);
                return to_hex(digest, 32);
            }

            inline void blake3_hash_raw(const std::string &data, uint8_t out[32])
            {
                Hasher h;
                h.init();
                h.update(reinterpret_cast<const uint8_t *>(data.data()), data.size());
                h.finalize(out, 32);
            }

            inline std::string blake3_keyed_hex(const std::string &data, const std::string &key)
            {
                uint8_t key32[32];
                if (key.size() == 32)
                    std::memcpy(key32, key.data(), 32);
                else
                    blake3_hash_raw(key, key32);
                Hasher h;
                h.init_keyed(key32);
                h.update(reinterpret_cast<const uint8_t *>(data.data()), data.size());
                uint8_t digest[32];
                h.finalize(digest, 32);
                return to_hex(digest, 32);
            }

            // ════════════════════════════════════════════════════════════
            // (AR) SHA-256 + HMAC-SHA256 + PBKDF2-HMAC-SHA256 (RFC 8018) +
            //      HKDF-SHA256 (RFC 5869) — مطابقة لنظير المترجم في
            //      tools/compiler/runtime/sad_embedded_runtime.c
            // ════════════════════════════════════════════════════════════
            inline uint32_t sha256_rotr(uint32_t x, int n) { return (x >> n) | (x << (32 - n)); }

            inline void sha256_raw(const uint8_t *data, size_t len, uint8_t out[32])
            {
                static const uint32_t K[64] = {
                    0x428a2f98u, 0x71374491u, 0xb5c0fbcfu, 0xe9b5dba5u, 0x3956c25bu, 0x59f111f1u, 0x923f82a4u, 0xab1c5ed5u,
                    0xd807aa98u, 0x12835b01u, 0x243185beu, 0x550c7dc3u, 0x72be5d74u, 0x80deb1feu, 0x9bdc06a7u, 0xc19bf174u,
                    0xe49b69c1u, 0xefbe4786u, 0x0fc19dc6u, 0x240ca1ccu, 0x2de92c6fu, 0x4a7484aau, 0x5cb0a9dcu, 0x76f988dau,
                    0x983e5152u, 0xa831c66du, 0xb00327c8u, 0xbf597fc7u, 0xc6e00bf3u, 0xd5a79147u, 0x06ca6351u, 0x14292967u,
                    0x27b70a85u, 0x2e1b2138u, 0x4d2c6dfcu, 0x53380d13u, 0x650a7354u, 0x766a0abbu, 0x81c2c92eu, 0x92722c85u,
                    0xa2bfe8a1u, 0xa81a664bu, 0xc24b8b70u, 0xc76c51a3u, 0xd192e819u, 0xd6990624u, 0xf40e3585u, 0x106aa070u,
                    0x19a4c116u, 0x1e376c08u, 0x2748774cu, 0x34b0bcb5u, 0x391c0cb3u, 0x4ed8aa4au, 0x5b9cca4fu, 0x682e6ff3u,
                    0x748f82eeu, 0x78a5636fu, 0x84c87814u, 0x8cc70208u, 0x90befffau, 0xa4506cebu, 0xbef9a3f7u, 0xc67178f2u};

                uint32_t h[8] = {0x6a09e667u, 0xbb67ae85u, 0x3c6ef372u, 0xa54ff53au,
                                  0x510e527fu, 0x9b05688cu, 0x1f83d9abu, 0x5be0cd19u};

                uint64_t bitlen = (uint64_t)len * 8;
                size_t padded_len = ((len + 9 + 63) / 64) * 64;
                std::vector<uint8_t> msg(padded_len, 0);
                std::memcpy(msg.data(), data, len);
                msg[len] = 0x80;
                for (int i = 0; i < 8; ++i)
                    msg[padded_len - 1 - i] = (uint8_t)(bitlen >> (8 * i));

                for (size_t chunk = 0; chunk < padded_len; chunk += 64)
                {
                    uint32_t w[64];
                    for (int i = 0; i < 16; ++i)
                        w[i] = ((uint32_t)msg[chunk + i * 4] << 24) | ((uint32_t)msg[chunk + i * 4 + 1] << 16) |
                               ((uint32_t)msg[chunk + i * 4 + 2] << 8) | (uint32_t)msg[chunk + i * 4 + 3];
                    for (int i = 16; i < 64; ++i)
                    {
                        uint32_t s0 = sha256_rotr(w[i - 15], 7) ^ sha256_rotr(w[i - 15], 18) ^ (w[i - 15] >> 3);
                        uint32_t s1 = sha256_rotr(w[i - 2], 17) ^ sha256_rotr(w[i - 2], 19) ^ (w[i - 2] >> 10);
                        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
                    }
                    uint32_t a = h[0], b = h[1], c = h[2], d = h[3], e = h[4], f = h[5], g_ = h[6], hh = h[7];
                    for (int i = 0; i < 64; ++i)
                    {
                        uint32_t S1 = sha256_rotr(e, 6) ^ sha256_rotr(e, 11) ^ sha256_rotr(e, 25);
                        uint32_t ch = (e & f) ^ (~e & g_);
                        uint32_t temp1 = hh + S1 + ch + K[i] + w[i];
                        uint32_t S0 = sha256_rotr(a, 2) ^ sha256_rotr(a, 13) ^ sha256_rotr(a, 22);
                        uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
                        uint32_t temp2 = S0 + maj;
                        hh = g_;
                        g_ = f;
                        f = e;
                        e = d + temp1;
                        d = c;
                        c = b;
                        b = a;
                        a = temp1 + temp2;
                    }
                    h[0] += a; h[1] += b; h[2] += c; h[3] += d;
                    h[4] += e; h[5] += f; h[6] += g_; h[7] += hh;
                }
                for (int i = 0; i < 8; ++i)
                {
                    out[i * 4] = (uint8_t)(h[i] >> 24);
                    out[i * 4 + 1] = (uint8_t)(h[i] >> 16);
                    out[i * 4 + 2] = (uint8_t)(h[i] >> 8);
                    out[i * 4 + 3] = (uint8_t)h[i];
                }
            }

            inline void hmac_sha256(const uint8_t *key, size_t klen, const uint8_t *msg, size_t mlen, uint8_t out[32])
            {
                uint8_t key_block[64];
                std::memset(key_block, 0, 64);
                if (klen > 64)
                    sha256_raw(key, klen, key_block);
                else
                    std::memcpy(key_block, key, klen);

                uint8_t ipad[64], opad[64];
                for (int i = 0; i < 64; ++i)
                {
                    ipad[i] = key_block[i] ^ 0x36u;
                    opad[i] = key_block[i] ^ 0x5cu;
                }

                std::vector<uint8_t> inner(64 + mlen);
                std::memcpy(inner.data(), ipad, 64);
                std::memcpy(inner.data() + 64, msg, mlen);
                uint8_t inner_hash[32];
                sha256_raw(inner.data(), inner.size(), inner_hash);

                uint8_t outer[64 + 32];
                std::memcpy(outer, opad, 64);
                std::memcpy(outer + 64, inner_hash, 32);
                sha256_raw(outer, sizeof(outer), out);
            }

            inline void pbkdf2_hmac_sha256(const uint8_t *pw, size_t pwlen, const uint8_t *salt, size_t saltlen,
                                            uint64_t iterations, uint8_t *out, size_t dklen)
            {
                uint32_t block_count = (uint32_t)((dklen + 31) / 32);
                std::vector<uint8_t> buf;
                buf.reserve((size_t)saltlen + 4);
                size_t written = 0;
                for (uint32_t i = 1; i <= block_count; ++i)
                {
                    buf.assign(salt, salt + saltlen);
                    buf.push_back((uint8_t)(i >> 24));
                    buf.push_back((uint8_t)(i >> 16));
                    buf.push_back((uint8_t)(i >> 8));
                    buf.push_back((uint8_t)i);

                    uint8_t u[32];
                    hmac_sha256(pw, pwlen, buf.data(), buf.size(), u);
                    uint8_t t[32];
                    std::memcpy(t, u, 32);
                    for (uint64_t iter = 1; iter < iterations; ++iter)
                    {
                        uint8_t u_next[32];
                        hmac_sha256(pw, pwlen, u, 32, u_next);
                        std::memcpy(u, u_next, 32);
                        for (int k = 0; k < 32; ++k)
                            t[k] ^= u[k];
                    }
                    size_t take = 32;
                    if (written + take > dklen)
                        take = dklen - written;
                    std::memcpy(out + written, t, take);
                    written += take;
                }
            }

            inline void hkdf_extract(const uint8_t *salt, size_t saltlen, const uint8_t *ikm, size_t ikmlen, uint8_t prk[32])
            {
                hmac_sha256(salt, saltlen, ikm, ikmlen, prk);
            }

            inline void hkdf_expand(const uint8_t prk[32], const uint8_t *info, size_t infolen, uint8_t *okm, size_t l)
            {
                uint32_t n = (uint32_t)((l + 31) / 32);
                std::vector<uint8_t> t_prev;
                size_t written = 0;
                for (uint32_t i = 1; i <= n; ++i)
                {
                    std::vector<uint8_t> data;
                    data.insert(data.end(), t_prev.begin(), t_prev.end());
                    data.insert(data.end(), info, info + infolen);
                    data.push_back((uint8_t)i);

                    uint8_t t[32];
                    hmac_sha256(prk, 32, data.data(), data.size(), t);
                    t_prev.assign(t, t + 32);

                    size_t take = 32;
                    if (written + take > l)
                        take = l - written;
                    std::memcpy(okm + written, t, take);
                    written += take;
                }
            }

            inline std::string pbkdf2_hex(const std::string &password, const std::string &salt, long long iterations)
            {
                uint8_t out[32];
                pbkdf2_hmac_sha256(reinterpret_cast<const uint8_t *>(password.data()), password.size(),
                                    reinterpret_cast<const uint8_t *>(salt.data()), salt.size(),
                                    (uint64_t)iterations, out, 32);
                return to_hex(out, 32);
            }

            inline std::string hkdf_hex(const std::string &secret, const std::string &salt, const std::string &info, long long length)
            {
                uint8_t prk[32];
                hkdf_extract(reinterpret_cast<const uint8_t *>(salt.data()), salt.size(),
                             reinterpret_cast<const uint8_t *>(secret.data()), secret.size(), prk);
                std::vector<uint8_t> okm((size_t)length);
                hkdf_expand(prk, reinterpret_cast<const uint8_t *>(info.data()), info.size(), okm.data(), (size_t)length);
                return to_hex(okm.data(), okm.size());
            }

            // ════════════════════════════════════════════════════════════
            // (AR) sadx_core — SHA-512 (FIPS 180-4) + X25519 (RFC 7748) +
            //      Ed25519 (RFC 8032). ذاتيّ التنفيذ بالكامل، بلا OpenSSL/
            //      libsodium ولا __int128. حساب المجال/المنحنى بتمثيل gf[16]
            //      بجذر 2^16. مطابق حرفيًّا (نفس النصّ) لنظير المترجم في
            //      tools/compiler/runtime/sad_embedded_runtime.c ونسخة رابط
            //      أندرويد. مُتحقَّق مقابل FIPS 180-4 + RFC 7748 §5.2/§6.1 +
            //      RFC 8032 §7.1 (TEST 1/2/3) + أطوال حدود كتلة SHA-512.
            // ════════════════════════════════════════════════════════════
            typedef long long sadx_gf[16];

            static const unsigned long long SADX_K512[80] = {
            0x428a2f98d728ae22ULL,0x7137449123ef65cdULL,0xb5c0fbcfec4d3b2fULL,0xe9b5dba58189dbbcULL,
            0x3956c25bf348b538ULL,0x59f111f1b605d019ULL,0x923f82a4af194f9bULL,0xab1c5ed5da6d8118ULL,
            0xd807aa98a3030242ULL,0x12835b0145706fbeULL,0x243185be4ee4b28cULL,0x550c7dc3d5ffb4e2ULL,
            0x72be5d74f27b896fULL,0x80deb1fe3b1696b1ULL,0x9bdc06a725c71235ULL,0xc19bf174cf692694ULL,
            0xe49b69c19ef14ad2ULL,0xefbe4786384f25e3ULL,0x0fc19dc68b8cd5b5ULL,0x240ca1cc77ac9c65ULL,
            0x2de92c6f592b0275ULL,0x4a7484aa6ea6e483ULL,0x5cb0a9dcbd41fbd4ULL,0x76f988da831153b5ULL,
            0x983e5152ee66dfabULL,0xa831c66d2db43210ULL,0xb00327c898fb213fULL,0xbf597fc7beef0ee4ULL,
            0xc6e00bf33da88fc2ULL,0xd5a79147930aa725ULL,0x06ca6351e003826fULL,0x142929670a0e6e70ULL,
            0x27b70a8546d22ffcULL,0x2e1b21385c26c926ULL,0x4d2c6dfc5ac42aedULL,0x53380d139d95b3dfULL,
            0x650a73548baf63deULL,0x766a0abb3c77b2a8ULL,0x81c2c92e47edaee6ULL,0x92722c851482353bULL,
            0xa2bfe8a14cf10364ULL,0xa81a664bbc423001ULL,0xc24b8b70d0f89791ULL,0xc76c51a30654be30ULL,
            0xd192e819d6ef5218ULL,0xd69906245565a910ULL,0xf40e35855771202aULL,0x106aa07032bbd1b8ULL,
            0x19a4c116b8d2d0c8ULL,0x1e376c085141ab53ULL,0x2748774cdf8eeb99ULL,0x34b0bcb5e19b48a8ULL,
            0x391c0cb3c5c95a63ULL,0x4ed8aa4ae3418acbULL,0x5b9cca4f7763e373ULL,0x682e6ff3d6b2b8a3ULL,
            0x748f82ee5defb2fcULL,0x78a5636f43172f60ULL,0x84c87814a1f0ab72ULL,0x8cc702081a6439ecULL,
            0x90befffa23631e28ULL,0xa4506cebde82bde9ULL,0xbef9a3f7b2c67915ULL,0xc67178f2e372532bULL,
            0xca273eceea26619cULL,0xd186b8c721c0c207ULL,0xeada7dd6cde0eb1eULL,0xf57d4f7fee6ed178ULL,
            0x06f067aa72176fbaULL,0x0a637dc5a2c898a6ULL,0x113f9804bef90daeULL,0x1b710b35131c471bULL,
            0x28db77f523047d84ULL,0x32caab7b40c72493ULL,0x3c9ebe0a15c9bebcULL,0x431d67c49c100d4cULL,
            0x4cc5d4becb3e42b6ULL,0x597f299cfc657e2aULL,0x5fcb6fab3ad6faecULL,0x6c44198c4a475817ULL};

            static unsigned long long sadx_rotr64(unsigned long long x, int n) { return (x >> n) | (x << (64 - n)); }
            static unsigned long long sadx_load64be(const unsigned char *p) {
                return ((unsigned long long)p[0] << 56) | ((unsigned long long)p[1] << 48) |
                       ((unsigned long long)p[2] << 40) | ((unsigned long long)p[3] << 32) |
                       ((unsigned long long)p[4] << 24) | ((unsigned long long)p[5] << 16) |
                       ((unsigned long long)p[6] << 8) | ((unsigned long long)p[7]);
            }
            static void sadx_store64be(unsigned char *p, unsigned long long v) {
                p[0] = (unsigned char)(v >> 56); p[1] = (unsigned char)(v >> 48);
                p[2] = (unsigned char)(v >> 40); p[3] = (unsigned char)(v >> 32);
                p[4] = (unsigned char)(v >> 24); p[5] = (unsigned char)(v >> 16);
                p[6] = (unsigned char)(v >> 8);  p[7] = (unsigned char)v;
            }
            static void sadx_sha512_compress(unsigned long long h[8], const unsigned char block[128]) {
                unsigned long long w[80], a, b, c, d, e, f, g, hh, t1, t2, s0, s1, ch, maj;
                int t;
                for (t = 0; t < 16; ++t) w[t] = sadx_load64be(block + t * 8);
                for (t = 16; t < 80; ++t) {
                    unsigned long long x0 = w[t - 15], x1 = w[t - 2];
                    s0 = sadx_rotr64(x0, 1) ^ sadx_rotr64(x0, 8) ^ (x0 >> 7);
                    s1 = sadx_rotr64(x1, 19) ^ sadx_rotr64(x1, 61) ^ (x1 >> 6);
                    w[t] = w[t - 16] + s0 + w[t - 7] + s1;
                }
                a = h[0]; b = h[1]; c = h[2]; d = h[3]; e = h[4]; f = h[5]; g = h[6]; hh = h[7];
                for (t = 0; t < 80; ++t) {
                    s1 = sadx_rotr64(e, 14) ^ sadx_rotr64(e, 18) ^ sadx_rotr64(e, 41);
                    ch = (e & f) ^ ((~e) & g);
                    t1 = hh + s1 + ch + SADX_K512[t] + w[t];
                    s0 = sadx_rotr64(a, 28) ^ sadx_rotr64(a, 34) ^ sadx_rotr64(a, 39);
                    maj = (a & b) ^ (a & c) ^ (b & c);
                    t2 = s0 + maj;
                    hh = g; g = f; f = e; e = d + t1; d = c; c = b; b = a; a = t1 + t2;
                }
                h[0] += a; h[1] += b; h[2] += c; h[3] += d; h[4] += e; h[5] += f; h[6] += g; h[7] += hh;
            }
            static void sadx_sha512(const unsigned char *msg, size_t len, unsigned char out[64]) {
                unsigned long long h[8] = {
                    0x6a09e667f3bcc908ULL, 0xbb67ae8584caa73bULL, 0x3c6ef372fe94f82bULL, 0xa54ff53a5f1d36f1ULL,
                    0x510e527fade682d1ULL, 0x9b05688c2b3e6c1fULL, 0x1f83d9abfb41bd6bULL, 0x5be0cd19137e2179ULL};
                unsigned char last[256];
                unsigned long long lenbits = (unsigned long long)len;
                unsigned long long bitlenhi = lenbits >> 61;
                unsigned long long bitlenlo = lenbits << 3;
                size_t i, off = 0, rem, padlen;
                while (len - off >= 128) { sadx_sha512_compress(h, msg + off); off += 128; }
                rem = len - off;
                for (i = 0; i < sizeof(last); ++i) last[i] = 0;
                for (i = 0; i < rem; ++i) last[i] = msg[off + i];
                last[rem] = 0x80;
                padlen = (rem < 112) ? 128 : 256;
                sadx_store64be(last + padlen - 16, bitlenhi);
                sadx_store64be(last + padlen - 8, bitlenlo);
                sadx_sha512_compress(h, last);
                if (padlen == 256) sadx_sha512_compress(h, last + 128);
                for (i = 0; i < 8; ++i) sadx_store64be(out + i * 8, h[i]);
            }

            static const sadx_gf
            sadx_gf0 = {0},
            sadx_gf1 = {1},
            sadx_121665 = {0xDB41, 1},
            sadx_D  = {0x78a3, 0x1359, 0x4dca, 0x75eb, 0xd8ab, 0x4141, 0x0a4d, 0x0070, 0xe898, 0x7779, 0x4079, 0x8cc7, 0xfe73, 0x2b6f, 0x6cee, 0x5203},
            sadx_D2 = {0xf159, 0x26b2, 0x9b94, 0xebd6, 0xb156, 0x8283, 0x149a, 0x00e0, 0xd130, 0xeef3, 0x80f2, 0x198e, 0xfce7, 0x56df, 0xd9dc, 0x2406},
            sadx_X  = {0xd51a, 0x8f25, 0x2d60, 0xc956, 0xa7b2, 0x9525, 0xc760, 0x692c, 0xdc5c, 0xfdd6, 0xe231, 0xc0a4, 0x53fe, 0xcd6e, 0x36d3, 0x2169},
            sadx_Y  = {0x6658, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666},
            sadx_I  = {0xa0b0, 0x4a0e, 0x1b27, 0xc4ee, 0xe478, 0xad2f, 0x1806, 0x2f43, 0xd7a7, 0x3dfb, 0x0099, 0x2b4d, 0xdf0b, 0x4fc1, 0x2480, 0x2b83};

            static int sadx_vn(const unsigned char *x, const unsigned char *y, int n) {
                unsigned int i, d = 0;
                for (i = 0; i < (unsigned int)n; ++i) d |= x[i] ^ y[i];
                return (1 & ((d - 1) >> 8)) - 1;
            }
            static int sadx_verify32(const unsigned char *x, const unsigned char *y) { return sadx_vn(x, y, 32); }
            static void sadx_set(sadx_gf r, const sadx_gf a) { int i; for (i = 0; i < 16; ++i) r[i] = a[i]; }
            static void sadx_car(sadx_gf o) {
                int i; long long c;
                for (i = 0; i < 16; ++i) {
                    o[i] += (1LL << 16);
                    c = o[i] >> 16;
                    o[(i + 1) * (i < 15)] += c - 1 + 37 * (c - 1) * (i == 15);
                    o[i] -= c << 16;
                }
            }
            static void sadx_sel(sadx_gf p, sadx_gf q, int b) {
                long long t, i, c = ~(b - 1);
                for (i = 0; i < 16; ++i) { t = c & (p[i] ^ q[i]); p[i] ^= t; q[i] ^= t; }
            }
            static void sadx_pack(unsigned char *o, const sadx_gf n) {
                int i, j, b;
                sadx_gf m, t;
                for (i = 0; i < 16; ++i) t[i] = n[i];
                sadx_car(t); sadx_car(t); sadx_car(t);
                for (j = 0; j < 2; ++j) {
                    m[0] = t[0] - 0xffed;
                    for (i = 1; i < 15; i++) { m[i] = t[i] - 0xffff - ((m[i - 1] >> 16) & 1); m[i - 1] &= 0xffff; }
                    m[15] = t[15] - 0x7fff - ((m[14] >> 16) & 1);
                    b = (m[15] >> 16) & 1;
                    m[14] &= 0xffff;
                    sadx_sel(t, m, 1 - b);
                }
                for (i = 0; i < 16; ++i) { o[2 * i] = t[i] & 0xff; o[2 * i + 1] = t[i] >> 8; }
            }
            static int sadx_neq(const sadx_gf a, const sadx_gf b) {
                unsigned char c[32], d[32];
                sadx_pack(c, a); sadx_pack(d, b);
                return sadx_verify32(c, d);
            }
            static unsigned char sadx_par(const sadx_gf a) { unsigned char d[32]; sadx_pack(d, a); return d[0] & 1; }
            static void sadx_unpack(sadx_gf o, const unsigned char *n) {
                int i;
                for (i = 0; i < 16; ++i) o[i] = n[2 * i] + ((long long)n[2 * i + 1] << 8);
                o[15] &= 0x7fff;
            }
            static void sadx_add_fe(sadx_gf o, const sadx_gf a, const sadx_gf b) { int i; for (i = 0; i < 16; ++i) o[i] = a[i] + b[i]; }
            static void sadx_sub_fe(sadx_gf o, const sadx_gf a, const sadx_gf b) { int i; for (i = 0; i < 16; ++i) o[i] = a[i] - b[i]; }
            static void sadx_mul(sadx_gf o, const sadx_gf a, const sadx_gf b) {
                long long i, j, t[31];
                for (i = 0; i < 31; ++i) t[i] = 0;
                for (i = 0; i < 16; ++i) for (j = 0; j < 16; ++j) t[i + j] += a[i] * b[j];
                for (i = 0; i < 15; ++i) t[i] += 38 * t[i + 16];
                for (i = 0; i < 16; ++i) o[i] = t[i];
                sadx_car(o); sadx_car(o);
            }
            static void sadx_sqr(sadx_gf o, const sadx_gf a) { sadx_mul(o, a, a); }
            static void sadx_inv(sadx_gf o, const sadx_gf i) {
                sadx_gf c; int a;
                for (a = 0; a < 16; ++a) c[a] = i[a];
                for (a = 253; a >= 0; a--) { sadx_sqr(c, c); if (a != 2 && a != 4) sadx_mul(c, c, i); }
                for (a = 0; a < 16; ++a) o[a] = c[a];
            }
            static void sadx_pow2523(sadx_gf o, const sadx_gf i) {
                sadx_gf c; int a;
                for (a = 0; a < 16; ++a) c[a] = i[a];
                for (a = 250; a >= 0; a--) { sadx_sqr(c, c); if (a != 1) sadx_mul(c, c, i); }
                for (a = 0; a < 16; ++a) o[a] = c[a];
            }

            /* X25519 (RFC 7748) — سلّم مونتغومري بزمن ثابت + تقييد سلميّ §5 */
            static int sadx_x25519(unsigned char *q, const unsigned char *n, const unsigned char *p) {
                unsigned char z[32];
                long long x[80], r, i;
                sadx_gf a, b, c, d, e, f;
                for (i = 0; i < 31; ++i) z[i] = n[i];
                z[31] = (n[31] & 127) | 64;
                z[0] &= 248;
                sadx_unpack(x, p);
                for (i = 0; i < 16; ++i) { b[i] = x[i]; d[i] = a[i] = c[i] = 0; }
                a[0] = d[0] = 1;
                for (i = 254; i >= 0; --i) {
                    r = (z[i >> 3] >> (i & 7)) & 1;
                    sadx_sel(a, b, r); sadx_sel(c, d, r);
                    sadx_add_fe(e, a, c); sadx_sub_fe(a, a, c);
                    sadx_add_fe(c, b, d); sadx_sub_fe(b, b, d);
                    sadx_sqr(d, e); sadx_sqr(f, a);
                    sadx_mul(a, c, a); sadx_mul(c, b, e);
                    sadx_add_fe(e, a, c); sadx_sub_fe(a, a, c);
                    sadx_sqr(b, a); sadx_sub_fe(c, d, f);
                    sadx_mul(a, c, sadx_121665); sadx_add_fe(a, a, d);
                    sadx_mul(c, c, a); sadx_mul(a, d, f);
                    sadx_mul(d, b, x); sadx_sqr(b, e);
                    sadx_sel(a, b, r); sadx_sel(c, d, r);
                }
                for (i = 0; i < 16; ++i) { x[i + 16] = a[i]; x[i + 32] = c[i]; x[i + 48] = b[i]; x[i + 64] = d[i]; }
                sadx_inv(x + 32, x + 32);
                sadx_mul(x + 16, x + 16, x + 32);
                sadx_pack(q, x + 16);
                return 0;
            }
            static const unsigned char SADX_BASE9[32] = {9};
            static int sadx_x25519_base(unsigned char *q, const unsigned char *n) { return sadx_x25519(q, n, SADX_BASE9); }

            /* Ed25519 (RFC 8032) */
            static void sadx_ed_add(sadx_gf p[4], sadx_gf q[4]) {
                sadx_gf a, b, c, d, t, e, f, g, h;
                sadx_sub_fe(a, p[1], p[0]); sadx_sub_fe(t, q[1], q[0]); sadx_mul(a, a, t);
                sadx_add_fe(b, p[0], p[1]); sadx_add_fe(t, q[0], q[1]); sadx_mul(b, b, t);
                sadx_mul(c, p[3], q[3]); sadx_mul(c, c, sadx_D2);
                sadx_mul(d, p[2], q[2]); sadx_add_fe(d, d, d);
                sadx_sub_fe(e, b, a); sadx_sub_fe(f, d, c); sadx_add_fe(g, d, c); sadx_add_fe(h, b, a);
                sadx_mul(p[0], e, f); sadx_mul(p[1], h, g); sadx_mul(p[2], g, f); sadx_mul(p[3], e, h);
            }
            static void sadx_ed_cswap(sadx_gf p[4], sadx_gf q[4], unsigned char b) { int i; for (i = 0; i < 4; ++i) sadx_sel(p[i], q[i], b); }
            static void sadx_ed_pack(unsigned char *r, sadx_gf p[4]) {
                sadx_gf tx, ty, zi;
                sadx_inv(zi, p[2]);
                sadx_mul(tx, p[0], zi); sadx_mul(ty, p[1], zi);
                sadx_pack(r, ty);
                r[31] ^= sadx_par(tx) << 7;
            }
            static void sadx_ed_scalarmult(sadx_gf p[4], sadx_gf q[4], const unsigned char *s) {
                int i;
                sadx_set(p[0], sadx_gf0); sadx_set(p[1], sadx_gf1); sadx_set(p[2], sadx_gf1); sadx_set(p[3], sadx_gf0);
                for (i = 255; i >= 0; --i) {
                    unsigned char b = (s[i / 8] >> (i & 7)) & 1;
                    sadx_ed_cswap(p, q, b);
                    sadx_ed_add(q, p);
                    sadx_ed_add(p, p);
                    sadx_ed_cswap(p, q, b);
                }
            }
            static void sadx_ed_scalarbase(sadx_gf p[4], const unsigned char *s) {
                sadx_gf q[4];
                sadx_set(q[0], sadx_X); sadx_set(q[1], sadx_Y); sadx_set(q[2], sadx_gf1); sadx_mul(q[3], sadx_X, sadx_Y);
                sadx_ed_scalarmult(p, q, s);
            }
            static const unsigned long long SADX_L[32] = {
                0xed, 0xd3, 0xf5, 0x5c, 0x1a, 0x63, 0x12, 0x58, 0xd6, 0x9c, 0xf7, 0xa2, 0xde, 0xf9, 0xde, 0x14,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x10};
            static void sadx_modL(unsigned char *r, long long x[64]) {
                long long carry, i, j;
                for (i = 63; i >= 32; --i) {
                    carry = 0;
                    for (j = i - 32; j < i - 12; ++j) {
                        x[j] += carry - 16 * x[i] * (long long)SADX_L[j - (i - 32)];
                        carry = (x[j] + 128) >> 8;
                        x[j] -= carry << 8;
                    }
                    x[j] += carry;
                    x[i] = 0;
                }
                carry = 0;
                for (j = 0; j < 32; ++j) { x[j] += carry - (x[31] >> 4) * (long long)SADX_L[j]; carry = x[j] >> 8; x[j] &= 255; }
                for (j = 0; j < 32; ++j) x[j] -= carry * (long long)SADX_L[j];
                for (i = 0; i < 32; ++i) { x[i + 1] += x[i] >> 8; r[i] = (unsigned char)(x[i] & 255); }
            }
            static void sadx_reduce(unsigned char *r) {
                long long x[64], i;
                for (i = 0; i < 64; ++i) x[i] = (unsigned long long)r[i];
                for (i = 0; i < 64; ++i) r[i] = 0;
                sadx_modL(r, x);
            }
            static void sadx_ed25519_pubkey(unsigned char pk[32], const unsigned char seed[32]) {
                unsigned char d[64];
                sadx_gf p[4];
                sadx_sha512(seed, 32, d);
                d[0] &= 248; d[31] &= 127; d[31] |= 64;
                sadx_ed_scalarbase(p, d);
                sadx_ed_pack(pk, p);
            }
            static void sadx_ed25519_sign(unsigned char sig[64], const unsigned char *m, size_t mlen,
                                          const unsigned char seed[32], unsigned char *scratch) {
                unsigned char d[64], h[64], r[64], pk[32];
                long long i, j, x[64];
                sadx_gf p[4];
                sadx_sha512(seed, 32, d);
                d[0] &= 248; d[31] &= 127; d[31] |= 64;
                sadx_ed_scalarbase(p, d); sadx_ed_pack(pk, p);
                for (i = 0; i < 32; ++i) scratch[i] = d[32 + i];
                for (i = 0; (size_t)i < mlen; ++i) scratch[32 + i] = m[i];
                sadx_sha512(scratch, 32 + mlen, r);
                sadx_reduce(r);
                sadx_ed_scalarbase(p, r); sadx_ed_pack(sig, p);
                for (i = 0; i < 32; ++i) scratch[i] = sig[i];
                for (i = 0; i < 32; ++i) scratch[32 + i] = pk[i];
                for (i = 0; (size_t)i < mlen; ++i) scratch[64 + i] = m[i];
                sadx_sha512(scratch, 64 + mlen, h);
                sadx_reduce(h);
                for (i = 0; i < 64; ++i) x[i] = 0;
                for (i = 0; i < 32; ++i) x[i] = (unsigned long long)r[i];
                for (i = 0; i < 32; ++i) for (j = 0; j < 32; ++j) x[i + j] += (long long)h[i] * (long long)d[j];
                sadx_modL(sig + 32, x);
            }
            static int sadx_ed_unpackneg(sadx_gf r[4], const unsigned char p[32]) {
                sadx_gf t, chk, num, den, den2, den4, den6;
                sadx_set(r[2], sadx_gf1);
                sadx_unpack(r[1], p);
                sadx_sqr(num, r[1]); sadx_mul(den, num, sadx_D);
                sadx_sub_fe(num, num, r[2]); sadx_add_fe(den, r[2], den);
                sadx_sqr(den2, den); sadx_sqr(den4, den2); sadx_mul(den6, den4, den2);
                sadx_mul(t, den6, num); sadx_mul(t, t, den);
                sadx_pow2523(t, t);
                sadx_mul(t, t, num); sadx_mul(t, t, den); sadx_mul(t, t, den); sadx_mul(r[0], t, den);
                sadx_sqr(chk, r[0]); sadx_mul(chk, chk, den);
                if (sadx_neq(chk, num)) sadx_mul(r[0], r[0], sadx_I);
                sadx_sqr(chk, r[0]); sadx_mul(chk, chk, den);
                if (sadx_neq(chk, num)) return -1;
                if (sadx_par(r[0]) == (p[31] >> 7)) sadx_sub_fe(r[0], sadx_gf0, r[0]);
                sadx_mul(r[3], r[0], r[1]);
                return 0;
            }
            static int sadx_ed25519_verify(const unsigned char *m, size_t mlen, const unsigned char sig[64],
                                           const unsigned char pk[32], unsigned char *scratch) {
                unsigned char t[32], h[64];
                sadx_gf p[4], q[4];
                long long i;
                int lt = 0, k;
                for (k = 31; k >= 0; --k) {
                    unsigned char sb = sig[32 + k], lb = (unsigned char)SADX_L[k];
                    if (sb < lb) { lt = 1; break; }
                    if (sb > lb) { break; }
                }
                if (!lt) return 0;
                if (sadx_ed_unpackneg(q, pk)) return 0;
                for (i = 0; i < 32; ++i) scratch[i] = sig[i];
                for (i = 0; i < 32; ++i) scratch[32 + i] = pk[i];
                for (i = 0; (size_t)i < mlen; ++i) scratch[64 + i] = m[i];
                sadx_sha512(scratch, 64 + mlen, h);
                sadx_reduce(h);
                sadx_ed_scalarmult(p, q, h);
                sadx_ed_scalarbase(q, sig + 32);
                sadx_ed_add(p, q);
                sadx_ed_pack(t, p);
                return sadx_verify32(sig, t) == 0 ? 1 : 0;
            }
            static int sadx_is_all_zero(const unsigned char *b, size_t n) {
                unsigned char acc = 0; size_t i;
                for (i = 0; i < n; ++i) acc |= b[i];
                return acc == 0;
            }

            // ── أغلفة سطح ص (سلسلة تدخل/تخرج) ──────────────────────────────
            inline int sadx_hexnib(char ch) {
                if (ch >= '0' && ch <= '9') return ch - '0';
                if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
                if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
                return -1;
            }
            inline bool sadx_hex_to_bytes(const std::string &hex, unsigned char *out, size_t n) {
                if (hex.size() != n * 2) return false;
                for (size_t i = 0; i < n; ++i) {
                    int hi = sadx_hexnib(hex[2 * i]);
                    int lo = sadx_hexnib(hex[2 * i + 1]);
                    if (hi < 0 || lo < 0) return false;
                    out[i] = (unsigned char)((hi << 4) | lo);
                }
                return true;
            }
            // توليد 32 بايت عشوائيّة عبر std::random_device (نفس نمط شفّر/AEAD في المفسّر)
            inline std::string sadx_keygen32_hex() {
                unsigned char sk[32];
                std::random_device rd;
                for (int i = 0; i < 32; ++i) sk[i] = (unsigned char)(rd() & 0xFF);
                return to_hex(sk, 32);
            }
            inline bool x25519_derive_pub_hex(const std::string &privHex, std::string &out) {
                unsigned char sk[32], pk[32];
                if (!sadx_hex_to_bytes(privHex, sk, 32)) return false;
                sadx_x25519_base(pk, sk);
                out = to_hex(pk, 32);
                return true;
            }
            // 0 = نجاح، 1 = مدخل غير صالح، 2 = سرّ مشترك كلّه أصفار (يُرفض، RFC 7748 §6.1)
            inline int x25519_exchange_hex(const std::string &privHex, const std::string &peerHex, std::string &out) {
                unsigned char sk[32], pp[32], ss[32];
                if (!sadx_hex_to_bytes(privHex, sk, 32) || !sadx_hex_to_bytes(peerHex, pp, 32)) return 1;
                sadx_x25519(ss, sk, pp);
                if (sadx_is_all_zero(ss, 32)) return 2;
                out = to_hex(ss, 32);
                return 0;
            }
            inline bool ed25519_derive_pub_hex(const std::string &seedHex, std::string &out) {
                unsigned char seed[32], pk[32];
                if (!sadx_hex_to_bytes(seedHex, seed, 32)) return false;
                sadx_ed25519_pubkey(pk, seed);
                out = to_hex(pk, 32);
                return true;
            }
            inline bool ed25519_sign_hex(const std::string &msg, const std::string &seedHex, std::string &out) {
                unsigned char seed[32], sig[64];
                if (!sadx_hex_to_bytes(seedHex, seed, 32)) return false;
                std::vector<unsigned char> scratch(msg.size() + 64);
                sadx_ed25519_sign(sig, reinterpret_cast<const unsigned char *>(msg.data()), msg.size(), seed, scratch.data());
                out = to_hex(sig, 64);
                return true;
            }
            inline bool ed25519_verify_hex(const std::string &msg, const std::string &sigHex, const std::string &pubHex) {
                unsigned char sig[64], pk[32];
                if (!sadx_hex_to_bytes(sigHex, sig, 64)) return false;
                if (!sadx_hex_to_bytes(pubHex, pk, 32)) return false;
                std::vector<unsigned char> scratch(msg.size() + 64);
                return sadx_ed25519_verify(reinterpret_cast<const unsigned char *>(msg.data()), msg.size(), sig, pk, scratch.data()) != 0;
            }

            // ════════════════════════════════════════════════════════════
            // (AR) ChaCha20-Poly1305 AEAD (RFC 8439) — ذاتيّ التنفيذ، مطابق
            //      حرفيًّا لنظير المترجم في tools/compiler/runtime/
            //      sad_embedded_runtime.c (sad_chacha20_*/sad_poly1305_*/
            //      sad_security_aead_*). Poly1305 بجذر 2^26 (بلا __int128).
            //      مُتحقَّق مقابل شعاعات RFC 8439 §2.3.2/§2.4.2/§2.5.2/§2.6.2/
            //      §2.8.2 قبل الدمج.
            // (EN) Self-implemented ChaCha20-Poly1305 AEAD, byte-identical in
            //      logic to the compiler runtime. Verified against the RFC
            //      8439 official vectors.
            // ════════════════════════════════════════════════════════════
            inline uint32_t cc_rotl32(uint32_t x, int n) { return (x << n) | (x >> (32 - n)); }
            inline uint32_t cc_load32le(const uint8_t *p)
            {
                return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
                       ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
            }
            inline void cc_store32le(uint8_t *p, uint32_t v)
            {
                p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8);
                p[2] = (uint8_t)(v >> 16); p[3] = (uint8_t)(v >> 24);
            }

#define SAD_CC_QR(a, b, c, d)                    \
    a += b; d ^= a; d = cc_rotl32(d, 16);        \
    c += d; b ^= c; b = cc_rotl32(b, 12);        \
    a += b; d ^= a; d = cc_rotl32(d, 8);         \
    c += d; b ^= c; b = cc_rotl32(b, 7);

            inline void chacha20_block(const uint8_t key[32], uint32_t counter,
                                       const uint8_t nonce[12], uint8_t out[64])
            {
                uint32_t s[16], x[16];
                int i;
                s[0] = 0x61707865u; s[1] = 0x3320646eu; s[2] = 0x79622d32u; s[3] = 0x6b206574u;
                for (i = 0; i < 8; ++i) s[4 + i] = cc_load32le(key + i * 4);
                s[12] = counter;
                s[13] = cc_load32le(nonce + 0);
                s[14] = cc_load32le(nonce + 4);
                s[15] = cc_load32le(nonce + 8);
                for (i = 0; i < 16; ++i) x[i] = s[i];
                for (i = 0; i < 10; ++i)
                {
                    SAD_CC_QR(x[0], x[4], x[8], x[12]);
                    SAD_CC_QR(x[1], x[5], x[9], x[13]);
                    SAD_CC_QR(x[2], x[6], x[10], x[14]);
                    SAD_CC_QR(x[3], x[7], x[11], x[15]);
                    SAD_CC_QR(x[0], x[5], x[10], x[15]);
                    SAD_CC_QR(x[1], x[6], x[11], x[12]);
                    SAD_CC_QR(x[2], x[7], x[8], x[13]);
                    SAD_CC_QR(x[3], x[4], x[9], x[14]);
                }
                for (i = 0; i < 16; ++i) cc_store32le(out + i * 4, x[i] + s[i]);
            }
#undef SAD_CC_QR

            inline void chacha20_xor(const uint8_t key[32], uint32_t counter,
                                     const uint8_t nonce[12], const uint8_t *in,
                                     size_t len, uint8_t *out)
            {
                uint8_t ks[64];
                size_t off = 0;
                while (off < len)
                {
                    size_t i, take = len - off;
                    if (take > 64) take = 64;
                    chacha20_block(key, counter, nonce, ks);
                    for (i = 0; i < take; ++i) out[off + i] = in[off + i] ^ ks[i];
                    off += take;
                    ++counter;
                }
            }

            inline void poly1305_mac(const uint8_t *msg, size_t len,
                                     const uint8_t key[32], uint8_t tag[16])
            {
                uint32_t r0, r1, r2, r3, r4;
                uint32_t s1, s2, s3, s4;
                uint32_t h0 = 0, h1 = 0, h2 = 0, h3 = 0, h4 = 0;
                uint32_t t0, t1, t2, t3;
                uint64_t d0, d1, d2, d3, d4;
                uint32_t c;
                uint64_t f;
                uint32_t g0, g1, g2, g3, g4;
                uint32_t mask;

                t0 = cc_load32le(key + 0);
                t1 = cc_load32le(key + 4);
                t2 = cc_load32le(key + 8);
                t3 = cc_load32le(key + 12);
                r0 = t0 & 0x3ffffff; t0 = (t0 >> 26) | (t1 << 6);
                r1 = t0 & 0x3ffff03; t1 = (t1 >> 20) | (t2 << 12);
                r2 = t1 & 0x3ffc0ff; t2 = (t2 >> 14) | (t3 << 18);
                r3 = t2 & 0x3f03fff; t3 = (t3 >> 8);
                r4 = t3 & 0x00fffff;

                s1 = r1 * 5; s2 = r2 * 5; s3 = r3 * 5; s4 = r4 * 5;

                while (len > 0)
                {
                    uint8_t block[16];
                    size_t i, n = len < 16 ? len : 16;
                    uint32_t hibit;
                    for (i = 0; i < n; ++i) block[i] = msg[i];
                    if (n < 16)
                    {
                        block[n] = 1;
                        for (i = n + 1; i < 16; ++i) block[i] = 0;
                        hibit = 0;
                    }
                    else
                    {
                        hibit = (1u << 24);
                    }
                    t0 = cc_load32le(block + 0);
                    t1 = cc_load32le(block + 4);
                    t2 = cc_load32le(block + 8);
                    t3 = cc_load32le(block + 12);
                    h0 += t0 & 0x3ffffff;
                    h1 += ((t0 >> 26) | (t1 << 6)) & 0x3ffffff;
                    h2 += ((t1 >> 20) | (t2 << 12)) & 0x3ffffff;
                    h3 += ((t2 >> 14) | (t3 << 18)) & 0x3ffffff;
                    h4 += (t3 >> 8) | hibit;

                    d0 = (uint64_t)h0 * r0 + (uint64_t)h1 * s4 + (uint64_t)h2 * s3 + (uint64_t)h3 * s2 + (uint64_t)h4 * s1;
                    d1 = (uint64_t)h0 * r1 + (uint64_t)h1 * r0 + (uint64_t)h2 * s4 + (uint64_t)h3 * s3 + (uint64_t)h4 * s2;
                    d2 = (uint64_t)h0 * r2 + (uint64_t)h1 * r1 + (uint64_t)h2 * r0 + (uint64_t)h3 * s4 + (uint64_t)h4 * s3;
                    d3 = (uint64_t)h0 * r3 + (uint64_t)h1 * r2 + (uint64_t)h2 * r1 + (uint64_t)h3 * r0 + (uint64_t)h4 * s4;
                    d4 = (uint64_t)h0 * r4 + (uint64_t)h1 * r3 + (uint64_t)h2 * r2 + (uint64_t)h3 * r1 + (uint64_t)h4 * r0;

                    c = (uint32_t)(d0 >> 26); h0 = (uint32_t)d0 & 0x3ffffff;
                    d1 += c; c = (uint32_t)(d1 >> 26); h1 = (uint32_t)d1 & 0x3ffffff;
                    d2 += c; c = (uint32_t)(d2 >> 26); h2 = (uint32_t)d2 & 0x3ffffff;
                    d3 += c; c = (uint32_t)(d3 >> 26); h3 = (uint32_t)d3 & 0x3ffffff;
                    d4 += c; c = (uint32_t)(d4 >> 26); h4 = (uint32_t)d4 & 0x3ffffff;
                    h0 += c * 5; c = h0 >> 26; h0 &= 0x3ffffff;
                    h1 += c;

                    msg += n;
                    len -= n;
                }

                c = h1 >> 26; h1 &= 0x3ffffff;
                h2 += c; c = h2 >> 26; h2 &= 0x3ffffff;
                h3 += c; c = h3 >> 26; h3 &= 0x3ffffff;
                h4 += c; c = h4 >> 26; h4 &= 0x3ffffff;
                h0 += c * 5; c = h0 >> 26; h0 &= 0x3ffffff;
                h1 += c;

                g0 = h0 + 5; c = g0 >> 26; g0 &= 0x3ffffff;
                g1 = h1 + c; c = g1 >> 26; g1 &= 0x3ffffff;
                g2 = h2 + c; c = g2 >> 26; g2 &= 0x3ffffff;
                g3 = h3 + c; c = g3 >> 26; g3 &= 0x3ffffff;
                g4 = h4 + c - (1u << 26);

                mask = (g4 >> 31) - 1;
                g0 &= mask; g1 &= mask; g2 &= mask; g3 &= mask; g4 &= mask;
                mask = ~mask;
                h0 = (h0 & mask) | g0;
                h1 = (h1 & mask) | g1;
                h2 = (h2 & mask) | g2;
                h3 = (h3 & mask) | g3;
                h4 = (h4 & mask) | g4;

                h0 = (h0) | (h1 << 26);
                h1 = (h1 >> 6) | (h2 << 20);
                h2 = (h2 >> 12) | (h3 << 14);
                h3 = (h3 >> 18) | (h4 << 8);

                f = (uint64_t)h0 + cc_load32le(key + 16); h0 = (uint32_t)f;
                f = (uint64_t)h1 + cc_load32le(key + 20) + (f >> 32); h1 = (uint32_t)f;
                f = (uint64_t)h2 + cc_load32le(key + 24) + (f >> 32); h2 = (uint32_t)f;
                f = (uint64_t)h3 + cc_load32le(key + 28) + (f >> 32); h3 = (uint32_t)f;

                cc_store32le(tag + 0, h0);
                cc_store32le(tag + 4, h1);
                cc_store32le(tag + 8, h2);
                cc_store32le(tag + 12, h3);
            }

            inline void poly1305_keygen(const uint8_t key[32], const uint8_t nonce[12], uint8_t otk[32])
            {
                uint8_t blk[64];
                chacha20_block(key, 0, nonce, blk);
                std::memcpy(otk, blk, 32);
            }

            // (AR) وسم AEAD (RFC 8439 §2.8.1): mac على
            //      pad16(AAD)||pad16(ct)||le64(|AAD|)||le64(|ct|). سطح لغة ص
            //      بلا AAD ⇒ aadlen==0 دومًا (حالة AAD فارغة موثَّقة).
            inline void aead_compute_tag(const uint8_t otk[32], const uint8_t *aad, size_t aadlen,
                                         const uint8_t *ct, size_t ctlen, uint8_t tag[16])
            {
                std::vector<uint8_t> buf;
                size_t apad = (16 - (aadlen % 16)) % 16;
                size_t cpad = (16 - (ctlen % 16)) % 16;
                buf.reserve(aadlen + apad + ctlen + cpad + 16);
                buf.insert(buf.end(), aad, aad + aadlen);
                buf.insert(buf.end(), apad, (uint8_t)0);
                buf.insert(buf.end(), ct, ct + ctlen);
                buf.insert(buf.end(), cpad, (uint8_t)0);
                uint64_t a = (uint64_t)aadlen, cl = (uint64_t)ctlen;
                for (int i = 0; i < 8; ++i) buf.push_back((uint8_t)(a >> (8 * i)));
                for (int i = 0; i < 8; ++i) buf.push_back((uint8_t)(cl >> (8 * i)));
                poly1305_mac(buf.data(), buf.size(), otk, tag);
            }

            inline bool ct_equal(const uint8_t *a, const uint8_t *b, size_t n)
            {
                uint8_t d = 0;
                for (size_t i = 0; i < n; ++i) d |= (uint8_t)(a[i] ^ b[i]);
                return d == 0;
            }

            // (AR) اشتقاق مفتاح 32 بايت: مباشر إن كان الطول 32، وإلّا SHA-256(المفتاح).
            inline void aead_key32(const std::string &key, uint8_t out[32])
            {
                if (key.size() == 32)
                    std::memcpy(out, key.data(), 32);
                else
                    sha256_raw(reinterpret_cast<const uint8_t *>(key.data()), key.size(), out);
            }

            // (AR) شفّر_موثّق — المغلّف الست عشريّ: [nonce 12][ct][tag 16]. nonce
            //      عشوائيّ جديد لكل استدعاء عبر std::random_device (نفس نمط شفّر).
            inline std::string aead_encrypt_hex(const std::string &text, const std::string &key)
            {
                uint8_t key32[32], nonce[12], tag[16];
                aead_key32(key, key32);
                std::random_device rd;
                for (int i = 0; i < 12; ++i) nonce[i] = (uint8_t)(rd() & 0xFF);

                std::vector<uint8_t> ct(text.size());
                chacha20_xor(key32, 1, nonce, reinterpret_cast<const uint8_t *>(text.data()), text.size(), ct.data());
                uint8_t otk[32];
                uint8_t aad_empty = 0; // AAD طوله صفر (سطح ص بلا AAD)
                poly1305_keygen(key32, nonce, otk);
                aead_compute_tag(otk, &aad_empty, 0, ct.data(), ct.size(), tag);

                std::vector<uint8_t> env;
                env.reserve(12 + ct.size() + 16);
                env.insert(env.end(), nonce, nonce + 12);
                env.insert(env.end(), ct.begin(), ct.end());
                env.insert(env.end(), tag, tag + 16);
                return to_hex(env.data(), env.size());
            }

            // (AR) فك_تشفير_موثّق — يتحقّق من الوسم ثمّ يفكّ. يُرجع true ويملأ out
            //      عند النجاح؛ false عند مغلّف مُشوَّه أو فشل مصادقة (المتّصل يرمي
            //      استثناءً قابلًا للالتقاط بالمفسّر عبر ctx.error).
            inline bool aead_decrypt(const std::string &hex, const std::string &key, std::string &out)
            {
                size_t hlen = hex.size();
                if (hlen % 2 != 0 || (hlen / 2) < (12 + 16))
                    return false;
                size_t rlen = hlen / 2;
                std::vector<uint8_t> raw(rlen);
                for (size_t i = 0; i < rlen; ++i)
                {
                    int hi = hex_nibble(hex[i * 2]);
                    int lo = hex_nibble(hex[i * 2 + 1]);
                    if (hi < 0 || lo < 0)
                        return false;
                    raw[i] = (uint8_t)((hi << 4) | lo);
                }
                uint8_t nonce[12], tag[16], tag2[16], key32[32];
                std::memcpy(nonce, raw.data(), 12);
                std::memcpy(tag, raw.data() + rlen - 16, 16);
                const uint8_t *ct = raw.data() + 12;
                size_t ctlen = rlen - 12 - 16;

                aead_key32(key, key32);
                uint8_t otk[32];
                uint8_t aad_empty = 0; // AAD طوله صفر (سطح ص بلا AAD)
                poly1305_keygen(key32, nonce, otk);
                aead_compute_tag(otk, &aad_empty, 0, ct, ctlen, tag2);
                if (!ct_equal(tag, tag2, 16))
                    return false;

                std::vector<uint8_t> pt(ctlen);
                chacha20_xor(key32, 1, nonce, ct, ctlen, pt.data());
                out.assign(reinterpret_cast<const char *>(pt.data()), ctlen);
                return true;
            }

            // ════════════════════════════════════════════════════════════
            // (AR) أرجون2id (RFC 9106)، التوازي ثابت على 1 — مبنيّ فوق
            // BLAKE2b (RFC 7693) مستقلّ عن SHA-256 أعلاه. مطابق حرفيًّا
            // لنظير المترجم في sad_embedded_runtime.c، ومُتحقَّق بايتًا
            // بايت مقابل libargon2 (عبر argon2-cffi) قبل الدمج.
            // ════════════════════════════════════════════════════════════
            constexpr uint64_t B2B_IV[8] = {
                0x6A09E667F3BCC908ULL, 0xBB67AE8584CAA73BULL, 0x3C6EF372FE94F82BULL, 0xA54FF53A5F1D36F1ULL,
                0x510E527FADE682D1ULL, 0x9B05688C2B3E6C1FULL, 0x1F83D9ABFB41BD6BULL, 0x5BE0CD19137E2179ULL};

            constexpr uint8_t B2B_SIGMA[12][16] = {
                {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                {14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3},
                {11, 8, 12, 0, 5, 2, 15, 13, 10, 14, 3, 6, 7, 1, 9, 4},
                {7, 9, 3, 1, 13, 12, 11, 14, 2, 6, 5, 10, 4, 0, 15, 8},
                {9, 0, 5, 7, 2, 4, 10, 15, 14, 1, 11, 12, 6, 8, 3, 13},
                {2, 12, 6, 10, 0, 11, 8, 3, 4, 13, 7, 5, 15, 14, 1, 9},
                {12, 5, 1, 15, 14, 13, 4, 10, 0, 7, 6, 3, 9, 2, 8, 11},
                {13, 11, 7, 14, 12, 1, 3, 9, 5, 0, 15, 4, 8, 6, 2, 10},
                {6, 15, 14, 9, 11, 3, 0, 8, 12, 2, 13, 7, 1, 4, 10, 5},
                {10, 2, 8, 4, 7, 6, 1, 5, 15, 11, 9, 14, 3, 12, 13, 0},
                {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                {14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3}};

            inline uint64_t rotr64(uint64_t x, int n) { return (x >> n) | (x << (64 - n)); }

            struct Blake2bState
            {
                uint64_t h[8];
                uint64_t t[2] = {0, 0};
                uint8_t buf[128];
                size_t buflen = 0;
                size_t outlen;
            };

            inline void b2b_g(uint64_t *v, int a, int b, int c, int d, uint64_t x, uint64_t y)
            {
                v[a] = v[a] + v[b] + x;
                v[d] = rotr64(v[d] ^ v[a], 32);
                v[c] = v[c] + v[d];
                v[b] = rotr64(v[b] ^ v[c], 24);
                v[a] = v[a] + v[b] + y;
                v[d] = rotr64(v[d] ^ v[a], 16);
                v[c] = v[c] + v[d];
                v[b] = rotr64(v[b] ^ v[c], 63);
            }

            inline void b2b_compress(Blake2bState &s, const uint8_t block[128], bool last)
            {
                uint64_t m[16], v[16];
                for (int i = 0; i < 16; ++i)
                {
                    m[i] = 0;
                    for (int j = 0; j < 8; ++j)
                        m[i] |= ((uint64_t)block[i * 8 + j]) << (8 * j);
                }
                for (int i = 0; i < 8; ++i)
                    v[i] = s.h[i];
                for (int i = 0; i < 8; ++i)
                    v[8 + i] = B2B_IV[i];
                v[12] ^= s.t[0];
                v[13] ^= s.t[1];
                if (last)
                    v[14] = ~v[14];
                for (int i = 0; i < 12; ++i)
                {
                    const uint8_t *sg = B2B_SIGMA[i];
                    b2b_g(v, 0, 4, 8, 12, m[sg[0]], m[sg[1]]);
                    b2b_g(v, 1, 5, 9, 13, m[sg[2]], m[sg[3]]);
                    b2b_g(v, 2, 6, 10, 14, m[sg[4]], m[sg[5]]);
                    b2b_g(v, 3, 7, 11, 15, m[sg[6]], m[sg[7]]);
                    b2b_g(v, 0, 5, 10, 15, m[sg[8]], m[sg[9]]);
                    b2b_g(v, 1, 6, 11, 12, m[sg[10]], m[sg[11]]);
                    b2b_g(v, 2, 7, 8, 13, m[sg[12]], m[sg[13]]);
                    b2b_g(v, 3, 4, 9, 14, m[sg[14]], m[sg[15]]);
                }
                for (int i = 0; i < 8; ++i)
                    s.h[i] ^= v[i] ^ v[i + 8];
            }

            inline void b2b_init(Blake2bState &s, size_t outlen)
            {
                std::memcpy(s.h, B2B_IV, sizeof(s.h));
                s.h[0] ^= 0x01010000ULL ^ (uint64_t)outlen;
                s.t[0] = s.t[1] = 0;
                s.buflen = 0;
                s.outlen = outlen;
            }

            inline void b2b_update(Blake2bState &s, const uint8_t *data, size_t len)
            {
                for (size_t i = 0; i < len; ++i)
                {
                    if (s.buflen == 128)
                    {
                        s.t[0] += 128;
                        if (s.t[0] < 128)
                            s.t[1]++;
                        b2b_compress(s, s.buf, false);
                        s.buflen = 0;
                    }
                    s.buf[s.buflen++] = data[i];
                }
            }

            inline void b2b_final(Blake2bState &s, uint8_t *out)
            {
                s.t[0] += s.buflen;
                if (s.t[0] < s.buflen)
                    s.t[1]++;
                while (s.buflen < 128)
                    s.buf[s.buflen++] = 0;
                b2b_compress(s, s.buf, true);
                for (size_t i = 0; i < s.outlen; ++i)
                    out[i] = (uint8_t)(s.h[i / 8] >> (8 * (i % 8)));
            }

            inline void blake2b(uint8_t *out, size_t outlen, const uint8_t *in, size_t inlen)
            {
                Blake2bState s;
                b2b_init(s, outlen);
                b2b_update(s, in, inlen);
                b2b_final(s, out);
            }

            inline void argon2_hprime(uint8_t *out, size_t outlen, const uint8_t *in, size_t inlen)
            {
                uint8_t le_len[4] = {(uint8_t)outlen, (uint8_t)(outlen >> 8), (uint8_t)(outlen >> 16), (uint8_t)(outlen >> 24)};
                if (outlen <= 64)
                {
                    Blake2bState s;
                    b2b_init(s, outlen);
                    b2b_update(s, le_len, 4);
                    b2b_update(s, in, inlen);
                    b2b_final(s, out);
                    return;
                }
                uint8_t v[64];
                Blake2bState s;
                b2b_init(s, 64);
                b2b_update(s, le_len, 4);
                b2b_update(s, in, inlen);
                b2b_final(s, v);
                std::memcpy(out, v, 32);
                size_t produced = 32;
                while (outlen - produced > 64)
                {
                    blake2b(v, 64, v, 64);
                    std::memcpy(out + produced, v, 32);
                    produced += 32;
                }
                blake2b(v, outlen - produced, v, 64);
                std::memcpy(out + produced, v, outlen - produced);
            }

            constexpr int ARGON2_QWORDS_IN_BLOCK = 128;
            constexpr int ARGON2_BLOCK_SIZE = ARGON2_QWORDS_IN_BLOCK * 8;
            constexpr int ARGON2_SYNC_POINTS = 4;

            struct Argon2Block
            {
                uint64_t v[ARGON2_QWORDS_IN_BLOCK];
            };

            inline void argon2_block_xor(Argon2Block &dst, const Argon2Block &a, const Argon2Block &b)
            {
                for (int i = 0; i < ARGON2_QWORDS_IN_BLOCK; ++i)
                    dst.v[i] = a.v[i] ^ b.v[i];
            }

            inline void argon2_p(uint64_t *v)
            {
                static constexpr int perm[8][4] = {
                    {0, 4, 8, 12}, {1, 5, 9, 13}, {2, 6, 10, 14}, {3, 7, 11, 15}, {0, 5, 10, 15}, {1, 6, 11, 12}, {2, 7, 8, 13}, {3, 4, 9, 14}};
                for (int i = 0; i < 8; ++i)
                {
                    int a = perm[i][0], b = perm[i][1], c = perm[i][2], d = perm[i][3];
                    v[a] = v[a] + v[b] + 2ULL * (uint64_t)(uint32_t)v[a] * (uint64_t)(uint32_t)v[b];
                    v[d] = rotr64(v[d] ^ v[a], 32);
                    v[c] = v[c] + v[d] + 2ULL * (uint64_t)(uint32_t)v[c] * (uint64_t)(uint32_t)v[d];
                    v[b] = rotr64(v[b] ^ v[c], 24);
                    v[a] = v[a] + v[b] + 2ULL * (uint64_t)(uint32_t)v[a] * (uint64_t)(uint32_t)v[b];
                    v[d] = rotr64(v[d] ^ v[a], 16);
                    v[c] = v[c] + v[d] + 2ULL * (uint64_t)(uint32_t)v[c] * (uint64_t)(uint32_t)v[d];
                    v[b] = rotr64(v[b] ^ v[c], 63);
                }
            }

            inline void argon2_fill_block(const Argon2Block &x, const Argon2Block &y, Argon2Block &out,
                                           bool with_xor, const Argon2Block &prev_out)
            {
                Argon2Block r, q, newval;
                argon2_block_xor(r, x, y);
                q = r;
                for (int i = 0; i < 8; ++i)
                    argon2_p(&q.v[16 * i]);
                for (int i = 0; i < 8; ++i)
                {
                    uint64_t col[16];
                    for (int m = 0; m < 8; ++m)
                    {
                        col[2 * m] = q.v[2 * i + 16 * m];
                        col[2 * m + 1] = q.v[2 * i + 16 * m + 1];
                    }
                    argon2_p(col);
                    for (int m = 0; m < 8; ++m)
                    {
                        q.v[2 * i + 16 * m] = col[2 * m];
                        q.v[2 * i + 16 * m + 1] = col[2 * m + 1];
                    }
                }
                for (int i = 0; i < ARGON2_QWORDS_IN_BLOCK; ++i)
                    newval.v[i] = q.v[i] ^ r.v[i];
                if (with_xor)
                    for (int i = 0; i < ARGON2_QWORDS_IN_BLOCK; ++i)
                        newval.v[i] ^= prev_out.v[i];
                out = newval;
            }

            struct Argon2AddrInput
            {
                uint64_t pass, lane, slice, m_prime, t_prime, type, counter;
            };

            inline void argon2_gen_addr_block(Argon2Block &addr, const Argon2AddrInput &ai)
            {
                Argon2Block zero{}, in{}, tmp;
                in.v[0] = ai.pass;
                in.v[1] = ai.lane;
                in.v[2] = ai.slice;
                in.v[3] = ai.m_prime;
                in.v[4] = ai.t_prime;
                in.v[5] = ai.type;
                in.v[6] = ai.counter;
                argon2_fill_block(zero, in, tmp, false, zero);
                argon2_fill_block(zero, tmp, addr, false, zero);
            }

            // RFC 9106 §3.4 indexing, restricted to parallelism = 1 (always same-lane).
            inline uint32_t argon2_index_alpha(uint32_t pass, uint32_t slice, uint32_t seg_len, uint32_t index,
                                                uint64_t rand64, uint32_t lane_len)
            {
                uint64_t reference_area_size;
                if (pass == 0)
                    reference_area_size = (slice == 0) ? (index - 1) : ((uint64_t)slice * seg_len + index - 1);
                else
                    reference_area_size = (uint64_t)lane_len - seg_len + index - 1;

                uint64_t rel = rand64 & 0xFFFFFFFFULL;
                rel = (rel * rel) >> 32;
                rel = reference_area_size - 1 - ((reference_area_size * rel) >> 32);

                uint32_t start_position = 0;
                if (pass != 0)
                    start_position = (slice == ARGON2_SYNC_POINTS - 1) ? 0 : (slice + 1) * seg_len;

                return (uint32_t)((start_position + rel) % lane_len);
            }

            // أرجون2 / Argon2id (RFC 9106), التوازي ثابت على 1 عمدًا (يبسّط
            // التنفيذ، يتجنّب تعقيد التزامن متعدّد الخيوط).
            inline std::string argon2id_hex(const std::string &password, const std::string &salt,
                                             long long memory_cost_kib, long long iterations)
            {
                const uint32_t lanes = 1;
                uint32_t m_cost = (uint32_t)memory_cost_kib;
                uint32_t t_cost = (uint32_t)iterations;
                uint32_t m_prime = (m_cost / 4) * 4;
                if (m_prime < 2 * ARGON2_SYNC_POINTS * lanes)
                    m_prime = 2 * ARGON2_SYNC_POINTS * lanes;
                uint32_t lane_len = m_prime / lanes;
                uint32_t seg_len = lane_len / ARGON2_SYNC_POINTS;

                uint8_t h0[64];
                {
                    uint8_t buf[4];
                    Blake2bState s;
                    b2b_init(s, 64);
                    auto put32 = [&](uint32_t val)
                    {
                        buf[0] = (uint8_t)val;
                        buf[1] = (uint8_t)(val >> 8);
                        buf[2] = (uint8_t)(val >> 16);
                        buf[3] = (uint8_t)(val >> 24);
                        b2b_update(s, buf, 4);
                    };
                    put32(lanes);
                    put32(32u);
                    put32(m_cost); // RFC 9106 §3.2 H0 uses the raw memory cost, not the rounded-down m'
                    put32(t_cost);
                    put32(0x13u);
                    put32(2u);
                    put32((uint32_t)password.size());
                    b2b_update(s, reinterpret_cast<const uint8_t *>(password.data()), password.size());
                    put32((uint32_t)salt.size());
                    b2b_update(s, reinterpret_cast<const uint8_t *>(salt.data()), salt.size());
                    put32(0u);
                    put32(0u);
                    b2b_final(s, h0);
                }

                std::vector<Argon2Block> B(m_prime);

                {
                    uint8_t seed[72];
                    std::memcpy(seed, h0, 64);
                    std::memset(seed + 64, 0, 8);
                    argon2_hprime((uint8_t *)&B[0], ARGON2_BLOCK_SIZE, seed, 72);
                    seed[64] = 1;
                    argon2_hprime((uint8_t *)&B[1], ARGON2_BLOCK_SIZE, seed, 72);
                }

                for (uint32_t pass = 0; pass < t_cost; ++pass)
                {
                    for (uint32_t slice = 0; slice < ARGON2_SYNC_POINTS; ++slice)
                    {
                        bool data_independent = (pass == 0 && slice < ARGON2_SYNC_POINTS / 2);
                        Argon2Block addr_block{};
                        Argon2AddrInput ai{pass, 0, slice, m_prime, t_cost, 2, 0};
                        uint32_t start_index = (pass == 0 && slice == 0) ? 2 : 0;

                        for (uint32_t j = 0; j < seg_len; ++j)
                        {
                            if (data_independent && j % ARGON2_QWORDS_IN_BLOCK == 0)
                            {
                                ai.counter++;
                                argon2_gen_addr_block(addr_block, ai);
                            }
                            if (j < start_index)
                                continue;

                            uint32_t pos_in_lane = slice * seg_len + j;
                            uint32_t curr_index = pos_in_lane;
                            uint32_t prev_index = (pos_in_lane == 0) ? (lane_len - 1) : (curr_index - 1);

                            uint64_t rand64 = data_independent ? addr_block.v[j % ARGON2_QWORDS_IN_BLOCK] : B[prev_index].v[0];
                            uint32_t ref_index = argon2_index_alpha(pass, slice, seg_len, j, rand64, lane_len);
                            bool with_xor = (pass != 0);
                            argon2_fill_block(B[prev_index], B[ref_index], B[curr_index], with_xor, B[curr_index]);
                        }
                    }
                }

                uint8_t tag[32];
                argon2_hprime(tag, 32, (uint8_t *)&B[lane_len - 1], ARGON2_BLOCK_SIZE);
                return to_hex(tag, 32);
            }
        } // namespace CryptoDetail

        void registerBuiltinsCrypto(Interpreter &interpreter)
        {
            // بلايك3 / blake3 — هاش BLAKE3 (256 بت، سلسلة ست عشريّة 64 حرفًا)
            auto blake3_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args();
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string input = args[0]->toString();
                return std::make_shared<Data::Value>(CryptoDetail::blake3_hash_hex(input));
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bcr::BLAKE3_HASH), blake3_func);

            // هاش_مفتاح / keyed_hash — مصادقة رسالة عبر نمط BLAKE3 المُفتاح
            auto keyed_hash_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args();
                if (args.size() < 2)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string data = args[0]->toString();
                std::string key = args[1]->toString();
                return std::make_shared<Data::Value>(CryptoDetail::blake3_keyed_hex(data, key));
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bcr::BLAKE3_KEYED_HASH), keyed_hash_func);

            // اشتق_مفتاح_مرور / derive_password_key — PBKDF2-HMAC-SHA256 (RFC 8018)
            auto pbkdf2_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args();
                if (args.size() < 3)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string password = args[0]->toString();
                std::string salt = args[1]->toString();
                long long iterations = args[2]->toInt();
                if (iterations <= 0)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                return std::make_shared<Data::Value>(CryptoDetail::pbkdf2_hex(password, salt, iterations));
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bcr::KDF_PBKDF2), pbkdf2_func);

            // اشتق_مفتاح / derive_key — HKDF-SHA256 (RFC 5869)
            auto hkdf_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args();
                if (args.size() < 4)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string secret = args[0]->toString();
                std::string salt = args[1]->toString();
                std::string info = args[2]->toString();
                long long length = args[3]->toInt();
                // RFC 5869 §2.3: max output length L <= 255 * HashLen (32 bytes) = 8160 bytes
                if (length <= 0 || length > 8160)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                return std::make_shared<Data::Value>(CryptoDetail::hkdf_hex(secret, salt, info, length));
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bcr::KDF_HKDF), hkdf_func);

            // شفّر_موثّق / aead_encrypt — ChaCha20-Poly1305 AEAD (RFC 8439)
            auto aead_encrypt_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args();
                if (args.size() < 2)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string text = args[0]->toString();
                std::string key = args[1]->toString();
                if (key.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                return std::make_shared<Data::Value>(CryptoDetail::aead_encrypt_hex(text, key));
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bcr::AEAD_ENCRYPT), aead_encrypt_func);

            // فك_تشفير_موثّق / aead_decrypt — يفشل بخطأ قابل للالتقاط (حاول/امسك)
            // على فشل المصادقة أو مغلّف مُشوَّه (كشف العبث هو غرض الدالّة).
            auto aead_decrypt_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args();
                if (args.size() < 2)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string envelope = args[0]->toString();
                std::string key = args[1]->toString();
                if (key.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string plain;
                if (!CryptoDetail::aead_decrypt(envelope, key, plain))
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                return std::make_shared<Data::Value>(plain);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bcr::AEAD_DECRYPT), aead_decrypt_func);

            // أرجون2 / argon2id — اشتقاق مفتاح صعب الحساب ذاكرةً وزمنًا (RFC 9106)
            auto argon2id_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args();
                if (args.size() < 4)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string password = args[0]->toString();
                std::string salt = args[1]->toString();
                long long memory_cost_kib = args[2]->toInt();
                long long iterations = args[3]->toInt();
                if (memory_cost_kib < 8)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                if (iterations <= 0)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                if (salt.size() < 8)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                return std::make_shared<Data::Value>(CryptoDetail::argon2id_hex(password, salt, memory_cost_kib, iterations));
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bcr::KDF_ARGON2ID), argon2id_func);

            // ولّد_مفتاح_خاص_x25519 / X25519 private key gen (0 args)
            auto x25519_keygen_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                (void)ctx;
                return std::make_shared<Data::Value>(CryptoDetail::sadx_keygen32_hex());
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bcr::X25519_KEYGEN_PRIV), x25519_keygen_func);

            // اشتق_مفتاح_عام_x25519 / X25519 derive public (1 arg)
            auto x25519_pub_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args();
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string priv = args[0]->toString();
                std::string out;
                if (!CryptoDetail::x25519_derive_pub_hex(priv, out))
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                return std::make_shared<Data::Value>(out);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bcr::X25519_DERIVE_PUB), x25519_pub_func);

            // تبادل_مفتاح / X25519 exchange (2 args) — يرفض السرّ المشترك الصفريّ (RFC 7748 §6.1)
            auto x25519_exchange_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args();
                if (args.size() < 2)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string priv = args[0]->toString();
                std::string peer = args[1]->toString();
                std::string out;
                if (CryptoDetail::x25519_exchange_hex(priv, peer, out) != 0)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                return std::make_shared<Data::Value>(out);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bcr::X25519_EXCHANGE), x25519_exchange_func);

            // ولّد_مفتاح_خاص_توقيع / Ed25519 seed gen (0 args)
            auto ed25519_keygen_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                (void)ctx;
                return std::make_shared<Data::Value>(CryptoDetail::sadx_keygen32_hex());
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bcr::ED25519_KEYGEN_PRIV), ed25519_keygen_func);

            // اشتق_مفتاح_عام_توقيع / Ed25519 derive public (1 arg)
            auto ed25519_pub_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args();
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string seed = args[0]->toString();
                std::string out;
                if (!CryptoDetail::ed25519_derive_pub_hex(seed, out))
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                return std::make_shared<Data::Value>(out);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bcr::ED25519_DERIVE_PUB), ed25519_pub_func);

            // وقّع / Ed25519 sign (2 args)
            auto ed25519_sign_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args();
                if (args.size() < 2)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string msg = args[0]->toString();
                std::string seed = args[1]->toString();
                std::string out;
                if (!CryptoDetail::ed25519_sign_hex(msg, seed, out))
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                return std::make_shared<Data::Value>(out);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bcr::ED25519_SIGN), ed25519_sign_func);

            // تحقق_توقيع / Ed25519 verify (3 args) — دالّة استعلام: تُرجع منطقيًّا،
            // لا ترمي أبدًا على توقيع فاسد أو مدخل مشوَّه (كلاهما يُرجع خطأ/false).
            auto ed25519_verify_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args();
                if (args.size() < 3)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string msg = args[0]->toString();
                std::string sig = args[1]->toString();
                std::string pub = args[2]->toString();
                bool ok = CryptoDetail::ed25519_verify_hex(msg, sig, pub);
                return std::make_shared<Data::Value>(ok);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bcr::ED25519_VERIFY), ed25519_verify_func);
        }
    } // namespace Interpreter
} // namespace Sad
