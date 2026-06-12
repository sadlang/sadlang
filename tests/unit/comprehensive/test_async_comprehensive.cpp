// بسم الله الرحمن الرحيم
/**
 * @file test_async_comprehensive.cpp
 * @brief Comprehensive Tests for Async Module - اختبارات شاملة لوحدة التزامن
 *
 * Tests: BidirectionalChannel, ChannelResult, thread safety
 *
 * @author ريم — مهندسة المكتبات والأدوات
 * @date March 2026
 */

#include "sad_test_framework.h"
#include <thread>
#include <chrono>
#include <atomic>
#include <string>

// Channel is header-only style — include the source directly
#include "async/src/channels.cpp"

using namespace sad::stdlib::async;

int main() {
    SAD_TEST_INIT();

    // ================================================================
    // 1. ChannelResult
    // ================================================================
    SAD_GROUP("نتائج القنوات / Channel Results");

    SAD_TEST("ok result has value", {
        auto result = ChannelResult<int>::ok(42);
        SAD_ASSERT_TRUE(result.success);
        SAD_ASSERT_TRUE(result.value.has_value());
        SAD_ASSERT_EQ(result.value.value(), 42);
    });

    SAD_TEST("empty result", {
        auto result = ChannelResult<int>::empty();
        SAD_ASSERT_TRUE(result.success);
        SAD_ASSERT_FALSE(result.value.has_value());
    });

    SAD_TEST("error result", {
        auto result = ChannelResult<int>::error("القناة مغلقة");
        SAD_ASSERT_FALSE(result.success);
        SAD_ASSERT_EQ(result.error_ar, std::string("القناة مغلقة"));
    });

    SAD_TEST("string result", {
        auto result = ChannelResult<std::string>::ok("مرحبا");
        SAD_ASSERT_TRUE(result.success);
        SAD_ASSERT_EQ(result.value.value(), std::string("مرحبا"));
    });

    // ================================================================
    // 2. BidirectionalChannel Basic Operations
    // ================================================================
    SAD_GROUP("القنوات ثنائية الاتجاه — أساسيات / Bidirectional Channel — Basics");

    SAD_TEST("send A to B and receive B", {
        BidirectionalChannel<int> chan;
        chan.أرسل_أ(100);
        auto result = chan.استقبل_ب();
        SAD_ASSERT_TRUE(result.success);
        SAD_ASSERT_EQ(result.value.value(), 100);
    });

    SAD_TEST("send B to A and receive A", {
        BidirectionalChannel<int> chan;
        chan.أرسل_ب(200);
        auto result = chan.استقبل_أ();
        SAD_ASSERT_TRUE(result.success);
        SAD_ASSERT_EQ(result.value.value(), 200);
    });

    SAD_TEST("FIFO order A to B", {
        BidirectionalChannel<int> chan;
        chan.أرسل_أ(1);
        chan.أرسل_أ(2);
        chan.أرسل_أ(3);
        SAD_ASSERT_EQ(chan.استقبل_ب().value.value(), 1);
        SAD_ASSERT_EQ(chan.استقبل_ب().value.value(), 2);
        SAD_ASSERT_EQ(chan.استقبل_ب().value.value(), 3);
    });

    SAD_TEST("string messages", {
        BidirectionalChannel<std::string> chan;
        chan.أرسل_أ("أهلاً");
        chan.أرسل_ب("مرحباً");
        SAD_ASSERT_EQ(chan.استقبل_ب().value.value(), std::string("أهلاً"));
        SAD_ASSERT_EQ(chan.استقبل_أ().value.value(), std::string("مرحباً"));
    });

    SAD_TEST("multiple messages both directions", {
        BidirectionalChannel<int> chan;
        for (int i = 0; i < 10; ++i) {
            chan.أرسل_أ(i);
            chan.أرسل_ب(i * 10);
        }
        for (int i = 0; i < 10; ++i) {
            SAD_ASSERT_EQ(chan.استقبل_ب().value.value(), i);
            SAD_ASSERT_EQ(chan.استقبل_أ().value.value(), i * 10);
        }
    });

    // ================================================================
    // 3. Close Behavior
    // ================================================================
    SAD_GROUP("سلوك الإغلاق / Close Behavior");

    SAD_TEST("close prevents new sends", {
        BidirectionalChannel<int> chan;
        chan.أغلق();
        SAD_ASSERT_FALSE(chan.مفتوحة());
    });

    SAD_TEST("send after close fails", {
        BidirectionalChannel<int> chan;
        chan.أغلق();
        bool sent = chan.أرسل_أ(42);
        SAD_ASSERT_FALSE(sent);
    });

    SAD_TEST("drain remaining after close", {
        BidirectionalChannel<int> chan;
        chan.أرسل_أ(42);
        chan.أغلق();
        // Data sent before close should still be retrievable
        auto result = chan.استقبل_ب();
        SAD_ASSERT_TRUE(result.success);
        SAD_ASSERT_EQ(result.value.value(), 42);
    });

    // ================================================================
    // 4. Timeout Operations
    // ================================================================
    SAD_GROUP("المهلة الزمنية / Timeout");

    SAD_TEST("receive with timeout on empty channel", {
        BidirectionalChannel<int> chan;
        auto result = chan.استقبل_ب_مهلة(std::chrono::milliseconds(10));
        // Should return error after timeout
        SAD_ASSERT_FALSE(result.success);
    });

    SAD_TEST("receive with timeout succeeds when data available", {
        BidirectionalChannel<int> chan;
        chan.أرسل_أ(99);
        auto result = chan.استقبل_ب_مهلة(std::chrono::milliseconds(100));
        SAD_ASSERT_TRUE(result.success);
        SAD_ASSERT_EQ(result.value.value(), 99);
    });

    // ================================================================
    // 5. Size Tracking
    // ================================================================
    SAD_GROUP("تتبع الحجم / Size Tracking");

    SAD_TEST("initial size is zero", {
        BidirectionalChannel<int> chan;
        SAD_ASSERT_EQ(chan.حجم_أ(), (size_t)0);
        SAD_ASSERT_EQ(chan.حجم_ب(), (size_t)0);
    });

    SAD_TEST("size increases after send", {
        BidirectionalChannel<int> chan;
        chan.أرسل_أ(1);
        chan.أرسل_أ(2);
        SAD_ASSERT_EQ(chan.حجم_أ(), (size_t)2);
    });

    // ================================================================
    // 6. Thread Safety
    // ================================================================
    SAD_GROUP("أمان الخيوط / Thread Safety");

    SAD_TEST("concurrent send/receive A to B", {
        BidirectionalChannel<int> chan;
        const int N = 100;
        std::atomic<int> sum{0};

        std::thread sender([&]() {
            for (int i = 1; i <= N; ++i) {
                chan.أرسل_أ(i);
            }
        });

        std::thread receiver([&]() {
            for (int i = 0; i < N; ++i) {
                auto result = chan.استقبل_ب();
                if (result.success && result.value.has_value()) {
                    sum += result.value.value();
                }
            }
        });

        sender.join();
        receiver.join();
        SAD_ASSERT_EQ(sum.load(), N * (N + 1) / 2);
    });

    SAD_TEST("bidirectional concurrent exchange", {
        BidirectionalChannel<int> chan;
        const int N = 50;
        std::atomic<int> sum_a{0}, sum_b{0};

        std::thread t1([&]() {
            for (int i = 1; i <= N; ++i) {
                chan.أرسل_أ(i);
            }
            for (int i = 0; i < N; ++i) {
                auto r = chan.استقبل_أ();
                if (r.success && r.value.has_value()) sum_a += r.value.value();
            }
        });

        std::thread t2([&]() {
            for (int i = 0; i < N; ++i) {
                auto r = chan.استقبل_ب();
                if (r.success && r.value.has_value()) sum_b += r.value.value();
            }
            for (int i = 1; i <= N; ++i) {
                chan.أرسل_ب(i * 10);
            }
        });

        t1.join();
        t2.join();
        SAD_ASSERT_EQ(sum_b.load(), N * (N + 1) / 2);
        SAD_ASSERT_EQ(sum_a.load(), 10 * N * (N + 1) / 2);
    });

    SAD_TEST("multiple senders to channel", {
        BidirectionalChannel<int> chan;
        std::atomic<int> total{0};
        const int threads = 4;
        const int per_thread = 25;

        std::vector<std::thread> senders;
        for (int t = 0; t < threads; ++t) {
            senders.emplace_back([&]() {
                for (int i = 0; i < per_thread; ++i) {
                    chan.أرسل_أ(1);
                }
            });
        }

        std::thread receiver([&]() {
            for (int i = 0; i < threads * per_thread; ++i) {
                auto r = chan.استقبل_ب();
                if (r.success && r.value.has_value()) total += r.value.value();
            }
        });

        for (auto& s : senders) s.join();
        receiver.join();
        SAD_ASSERT_EQ(total.load(), threads * per_thread);
    });

    // ================================================================
    // 7. Edge Cases
    // ================================================================
    SAD_GROUP("حالات الحافة / Edge Cases");

    SAD_TEST("channel with large messages", {
        BidirectionalChannel<std::string> chan;
        std::string large(10000, 'X');
        chan.أرسل_أ(large);
        auto r = chan.استقبل_ب();
        SAD_ASSERT_TRUE(r.success);
        SAD_ASSERT_EQ(r.value.value().size(), (size_t)10000);
    });

    SAD_TEST("rapid open-close cycles", {
        for (int i = 0; i < 10; ++i) {
            BidirectionalChannel<int> chan;
            chan.أرسل_أ(i);
            auto r = chan.استقبل_ب();
            SAD_ASSERT_EQ(r.value.value(), i);
            chan.أغلق();
        }
    });

    return _runner.printSummary();
}
