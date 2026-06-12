// بسم الله الرحمن الرحيم
// test_network_error.cpp - Unit tests for NetworkException and error handling

#include <gtest/gtest.h>
#include "network/network_error.h"

using namespace sad::network;

// Test NetworkException construction
TEST(NetworkException, BasicConstruction) {
    NetworkException ex(
        NetworkErrorCode::SOCKET_CREATE_FAILED,
        "فشل إنشاء المقبس",
        "Failed to create socket"
    );
    
    EXPECT_EQ(ex.get_code(), NetworkErrorCode::SOCKET_CREATE_FAILED);
    EXPECT_EQ(ex.what_arabic(), "فشل إنشاء المقبس");
    EXPECT_STREQ(ex.what(), "Failed to create socket");
}

// Test error code to Arabic conversion
TEST(NetworkError, ErrorCodeToArabic) {
    std::string msg = error_code_to_arabic(NetworkErrorCode::SOCKET_CONNECT_FAILED);
    EXPECT_EQ(msg, "فشل الاتصال بالخادم");
    
    msg = error_code_to_arabic(NetworkErrorCode::TIMEOUT);
    EXPECT_EQ(msg, "انتهت مهلة العملية");
    
    msg = error_code_to_arabic(NetworkErrorCode::INVALID_ADDRESS);
    EXPECT_EQ(msg, "عنوان IP غير صالح");
}

// Test error code to English conversion
TEST(NetworkError, ErrorCodeToEnglish) {
    std::string msg = error_code_to_english(NetworkErrorCode::SOCKET_BIND_FAILED);
    EXPECT_EQ(msg, "Failed to bind socket");
    
    msg = error_code_to_english(NetworkErrorCode::CONNECTION_CLOSED);
    EXPECT_EQ(msg, "Connection closed by peer");
    
    msg = error_code_to_english(NetworkErrorCode::BUFFER_TOO_SMALL);
    EXPECT_EQ(msg, "Buffer is too small");
}

// Test all error codes have translations
TEST(NetworkError, AllErrorCodesHaveTranslations) {
    for (int i = 0; i <= static_cast<int>(NetworkErrorCode::SSL_ERROR); ++i) {
        auto code = static_cast<NetworkErrorCode>(i);
        
        std::string arabic = error_code_to_arabic(code);
        std::string english = error_code_to_english(code);
        
        EXPECT_FALSE(arabic.empty()) << "Missing Arabic for code " << i;
        EXPECT_FALSE(english.empty()) << "Missing English for code " << i;
    }
}

#ifdef _WIN32
// Test Windows system error conversion
TEST(NetworkError, WindowsSystemErrorConversion) {
    NetworkErrorCode code = system_error_to_network_error(WSAECONNREFUSED);
    EXPECT_EQ(code, NetworkErrorCode::SOCKET_CONNECT_FAILED);
    
    code = system_error_to_network_error(WSAEADDRINUSE);
    EXPECT_EQ(code, NetworkErrorCode::SOCKET_BIND_FAILED);
    
    code = system_error_to_network_error(WSAETIMEDOUT);
    EXPECT_EQ(code, NetworkErrorCode::TIMEOUT);
    
    code = system_error_to_network_error(WSAEWOULDBLOCK);
    EXPECT_EQ(code, NetworkErrorCode::WOULD_BLOCK);
    
    code = system_error_to_network_error(WSAECONNRESET);
    EXPECT_EQ(code, NetworkErrorCode::CONNECTION_RESET);
}
#else
// Test Linux/POSIX system error conversion
TEST(NetworkError, PosixSystemErrorConversion) {
    NetworkErrorCode code = system_error_to_network_error(ECONNREFUSED);
    EXPECT_EQ(code, NetworkErrorCode::SOCKET_CONNECT_FAILED);
    
    code = system_error_to_network_error(EADDRINUSE);
    EXPECT_EQ(code, NetworkErrorCode::SOCKET_BIND_FAILED);
    
    code = system_error_to_network_error(ETIMEDOUT);
    EXPECT_EQ(code, NetworkErrorCode::TIMEOUT);
    
    code = system_error_to_network_error(EWOULDBLOCK);
    EXPECT_EQ(code, NetworkErrorCode::WOULD_BLOCK);
    
    code = system_error_to_network_error(ECONNRESET);
    EXPECT_EQ(code, NetworkErrorCode::CONNECTION_RESET);
}
#endif

// Test exception with system error
TEST(NetworkException, WithSystemError) {
    try {
        throw_network_exception(NetworkErrorCode::SOCKET_CREATE_FAILED);
        FAIL() << "Exception should have been thrown";
    } catch (const NetworkException& ex) {
        EXPECT_EQ(ex.get_code(), NetworkErrorCode::SOCKET_CREATE_FAILED);
        EXPECT_FALSE(ex.what_arabic().empty());
        EXPECT_FALSE(std::string(ex.what()).empty());
    }
}

// Test exception copy and move
TEST(NetworkException, CopyAndMove) {
    NetworkException ex1(
        NetworkErrorCode::INVALID_PORT,
        "منفذ غير صالح",
        "Invalid port"
    );
    
    // Copy constructor
    NetworkException ex2 = ex1;
    EXPECT_EQ(ex2.get_code(), ex1.get_code());
    EXPECT_EQ(ex2.what_arabic(), ex1.what_arabic());
    
    // Move constructor
    NetworkException ex3 = std::move(ex2);
    EXPECT_EQ(ex3.get_code(), ex1.get_code());
}

// Test nested exception context
TEST(NetworkException, NestedContext) {
    try {
        try {
            throw std::runtime_error("Inner error");
        } catch (...) {
            std::throw_with_nested(
                NetworkException(
                    NetworkErrorCode::SOCKET_CREATE_FAILED,
                    "فشل مع خطأ داخلي",
                    "Failed with inner error"
                )
            );
        }
    } catch (const NetworkException& ex) {
        EXPECT_EQ(ex.get_code(), NetworkErrorCode::SOCKET_CREATE_FAILED);
        
        try {
            std::rethrow_if_nested(ex);
        } catch (const std::runtime_error& inner) {
            EXPECT_STREQ(inner.what(), "Inner error");
        }
    }
}

// Test error message formatting
TEST(NetworkError, MessageFormatting) {
    std::string msg = format_network_error(
        NetworkErrorCode::SOCKET_CONNECT_FAILED,
        "127.0.0.1:8080"
    );
    
    EXPECT_NE(msg.find("127.0.0.1:8080"), std::string::npos);
}

// Test is_critical_error helper
TEST(NetworkError, CriticalErrors) {
    EXPECT_TRUE(is_critical_error(NetworkErrorCode::OUT_OF_MEMORY));
    EXPECT_FALSE(is_critical_error(NetworkErrorCode::WOULD_BLOCK));
    EXPECT_FALSE(is_critical_error(NetworkErrorCode::TIMEOUT));
}

// Test is_recoverable_error helper
TEST(NetworkError, RecoverableErrors) {
    EXPECT_TRUE(is_recoverable_error(NetworkErrorCode::TIMEOUT));
    EXPECT_TRUE(is_recoverable_error(NetworkErrorCode::WOULD_BLOCK));
    EXPECT_FALSE(is_recoverable_error(NetworkErrorCode::OUT_OF_MEMORY));
    EXPECT_FALSE(is_recoverable_error(NetworkErrorCode::INVALID_ADDRESS));
}

// Main test entry point
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
