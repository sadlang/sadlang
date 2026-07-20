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
                if (key.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
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
        }
    } // namespace Interpreter
} // namespace Sad
