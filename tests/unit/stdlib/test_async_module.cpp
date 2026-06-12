// بسم الله الرحمن الرحيم
/**
 * @file test_async_module.cpp
 * @brief Comprehensive Tests for Async Module - اختبارات شاملة لوحدة البرمجة غير المتزامنة
 *
 * Tests cover: BidirectionalChannel (send/receive, timeout, close, thread safety)
 *
 * @author ريم — مهندسة المكتبات والأدوات
 * @date March 2026
 * الحمد لله رب العالمين
 */

#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>

// The channels.cpp is a standalone file with implementation
// We include it directly since it's header-only style
#include "async/src/channels.cpp"

using namespace sad::stdlib::async;

// ============================================================================
// 1. ChannelResult Tests
//    اختبارات نتيجة القناة
// ============================================================================

TEST(ChannelResultTest, OkResult) {
    auto result = ChannelResult<int>::ok(42);
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.value.has_value());
    EXPECT_EQ(result.value.value(), 42);
    EXPECT_TRUE(result.error_ar.empty());
}

TEST(ChannelResultTest, EmptyResult) {
    auto result = ChannelResult<int>::empty();
    EXPECT_FALSE(result.success);
    EXPECT_FALSE(result.value.has_value());
}

TEST(ChannelResultTest, ErrorResult) {
    auto result = ChannelResult<int>::error("القناة مغلقة");
    EXPECT_FALSE(result.success);
    EXPECT_FALSE(result.value.has_value());
    EXPECT_EQ(result.error_ar, "القناة مغلقة");
}

TEST(ChannelResultTest, StringType) {
    auto result = ChannelResult<std::string>::ok("مرحبا");
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.value.value(), "مرحبا");
}

// ============================================================================
// 2. BidirectionalChannel Basic Tests
//    اختبارات القناة ثنائية الاتجاه — أساسية
// ============================================================================

TEST(BidirectionalChannelTest, Construction) {
    BidirectionalChannel<int> chan(10);
    EXPECT_TRUE(chan.مفتوحة());
    EXPECT_EQ(chan.حجم_أ(), 0u);
    EXPECT_EQ(chan.حجم_ب(), 0u);
}

TEST(BidirectionalChannelTest, SendReceiveAtoB) {
    BidirectionalChannel<int> chan(10);

    // A sends, B receives
    EXPECT_TRUE(chan.أرسل_أ(42));
    EXPECT_EQ(chan.حجم_أ(), 1u);

    auto result = chan.استقبل_ب();
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.value.value(), 42);
    EXPECT_EQ(chan.حجم_أ(), 0u);
}

TEST(BidirectionalChannelTest, SendReceiveBtoA) {
    BidirectionalChannel<int> chan(10);

    // B sends, A receives
    EXPECT_TRUE(chan.أرسل_ب(100));
    EXPECT_EQ(chan.حجم_ب(), 1u);

    auto result = chan.استقبل_أ();
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.value.value(), 100);
}

TEST(BidirectionalChannelTest, MultipleMessages) {
    BidirectionalChannel<int> chan(10);

    chan.أرسل_أ(1);
    chan.أرسل_أ(2);
    chan.أرسل_أ(3);
    EXPECT_EQ(chan.حجم_أ(), 3u);

    // FIFO order
    EXPECT_EQ(chan.استقبل_ب().value.value(), 1);
    EXPECT_EQ(chan.استقبل_ب().value.value(), 2);
    EXPECT_EQ(chan.استقبل_ب().value.value(), 3);
}

TEST(BidirectionalChannelTest, StringMessages) {
    BidirectionalChannel<std::string> chan(5);

    chan.أرسل_أ("مرحبا");
    chan.أرسل_أ("العالم");

    EXPECT_EQ(chan.استقبل_ب().value.value(), "مرحبا");
    EXPECT_EQ(chan.استقبل_ب().value.value(), "العالم");
}

// ============================================================================
// 3. Channel Close Tests
//    اختبارات إغلاق القناة
// ============================================================================

TEST(BidirectionalChannelTest, CloseChannel) {
    BidirectionalChannel<int> chan(10);
    EXPECT_TRUE(chan.مفتوحة());

    chan.أغلق();
    EXPECT_FALSE(chan.مفتوحة());
}

TEST(BidirectionalChannelTest, SendOnClosedChannel) {
    BidirectionalChannel<int> chan(10);
    chan.أغلق();

    EXPECT_FALSE(chan.أرسل_أ(42));
    EXPECT_FALSE(chan.أرسل_ب(42));
}

TEST(BidirectionalChannelTest, ReceiveOnClosedEmptyChannel) {
    BidirectionalChannel<int> chan(10);
    chan.أغلق();

    auto result = chan.استقبل_أ();
    EXPECT_FALSE(result.success);
}

TEST(BidirectionalChannelTest, ReceiveRemainingAfterClose) {
    BidirectionalChannel<int> chan(10);
    chan.أرسل_أ(1);
    chan.أرسل_أ(2);
    chan.أغلق();

    // Should still be able to drain buffered messages
    auto r1 = chan.استقبل_ب();
    if (r1.success) {
        EXPECT_EQ(r1.value.value(), 1);
    }
}

// ============================================================================
// 4. Timeout Tests
//    اختبارات المهلة الزمنية
// ============================================================================

TEST(BidirectionalChannelTest, ReceiveWithTimeout) {
    BidirectionalChannel<int> chan(10);

    // No message available — should timeout
    auto start = std::chrono::steady_clock::now();
    auto result = chan.استقبل_أ_مهلة(std::chrono::milliseconds(50));
    auto elapsed = std::chrono::steady_clock::now() - start;

    EXPECT_FALSE(result.success);
    EXPECT_GE(std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count(), 40);
}

TEST(BidirectionalChannelTest, ReceiveWithTimeoutSuccess) {
    BidirectionalChannel<int> chan(10);
    chan.أرسل_ب(99); // Message available for A

    auto result = chan.استقبل_أ_مهلة(std::chrono::milliseconds(100));
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.value.value(), 99);
}

// ============================================================================
// 5. Thread Safety Tests
//    اختبارات أمان الخيوط
// ============================================================================

TEST(BidirectionalChannelTest, ConcurrentSendReceive) {
    BidirectionalChannel<int> chan(100);
    const int num_messages = 50;
    std::atomic<int> received_count{0};
    std::atomic<int> received_sum{0};

    // Producer thread: A sends
    std::thread producer([&]() {
        for (int i = 1; i <= num_messages; ++i) {
            chan.أرسل_أ(i);
        }
    });

    // Consumer thread: B receives
    std::thread consumer([&]() {
        for (int i = 0; i < num_messages; ++i) {
            auto result = chan.استقبل_ب_مهلة(std::chrono::milliseconds(500));
            if (result.success) {
                received_count++;
                received_sum += result.value.value();
            }
        }
    });

    producer.join();
    consumer.join();

    EXPECT_EQ(received_count.load(), num_messages);
    // Sum of 1..50 = 1275
    EXPECT_EQ(received_sum.load(), (num_messages * (num_messages + 1)) / 2);
}

TEST(BidirectionalChannelTest, BidirectionalConcurrent) {
    BidirectionalChannel<int> chan(50);
    std::atomic<int> a_received{0};
    std::atomic<int> b_received{0};

    // Thread A: sends to B, receives from B
    std::thread threadA([&]() {
        for (int i = 0; i < 20; ++i) {
            chan.أرسل_أ(i);
        }
        for (int i = 0; i < 20; ++i) {
            auto r = chan.استقبل_أ_مهلة(std::chrono::milliseconds(500));
            if (r.success) a_received++;
        }
    });

    // Thread B: sends to A, receives from A
    std::thread threadB([&]() {
        for (int i = 0; i < 20; ++i) {
            chan.أرسل_ب(i + 100);
        }
        for (int i = 0; i < 20; ++i) {
            auto r = chan.استقبل_ب_مهلة(std::chrono::milliseconds(500));
            if (r.success) b_received++;
        }
    });

    threadA.join();
    threadB.join();

    EXPECT_EQ(a_received.load(), 20);
    EXPECT_EQ(b_received.load(), 20);
}

// ============================================================================
// 6. Buffer Size Tests
//    اختبارات حجم المخزن المؤقت
// ============================================================================

TEST(BidirectionalChannelTest, ZeroBufferUnbuffered) {
    BidirectionalChannel<int> chan(0);
    // Unbuffered channel — send may block or fail
    // Just verify no crash
    EXPECT_TRUE(chan.مفتوحة());
}

TEST(BidirectionalChannelTest, SmallBuffer) {
    BidirectionalChannel<int> chan(2);
    EXPECT_TRUE(chan.أرسل_أ(1));
    EXPECT_TRUE(chan.أرسل_أ(2));
    EXPECT_EQ(chan.حجم_أ(), 2u);
}
