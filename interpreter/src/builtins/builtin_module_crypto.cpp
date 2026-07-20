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
        }
    } // namespace Interpreter
} // namespace Sad
