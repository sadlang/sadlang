// بسم الله الرحمن الرحيم
// websocket_frame.h - WebSocket Frame Parser and Builder
// محلل ومنشئ إطارات WebSocket

#ifndef SAD_WEBSOCKET_FRAME_H
#define SAD_WEBSOCKET_FRAME_H

#include "websocket_base.h"
#include <cstring>
#include <random>
#include <stdexcept>

namespace sad {
namespace network {
namespace websocket {

/**
 * @brief WebSocket Frame Builder and Parser / منشئ ومحلل إطارات WebSocket
 * 
 * يتولى بناء وتحليل إطارات WebSocket وفق RFC 6455
 * Handles building and parsing WebSocket frames according to RFC 6455
 */
class FrameBuilder {
public:
    /**
     * @brief Build a WebSocket frame / بناء إطار WebSocket
     * @param payload البيانات
     * @param opcode رمز العملية
     * @param fin هل هي نهاية الرسالة
     * @param mask هل يجب تطبيق القناع
     * @return الإطار المبني
     */
    static std::vector<uint8_t> build(
        const std::vector<uint8_t>& payload,
        WebSocketOpcode opcode,
        bool fin = true,
        bool mask = true
    ) {
        std::vector<uint8_t> frame;
        
        // Byte 1: FIN + RSV + Opcode
        uint8_t byte1 = static_cast<uint8_t>(opcode);
        if (fin) byte1 |= 0x80;
        frame.push_back(byte1);
        
        // Byte 2: MASK + Payload length
        uint64_t payload_len = payload.size();
        uint8_t byte2 = mask ? 0x80 : 0x00;
        
        if (payload_len < 126) {
            byte2 |= static_cast<uint8_t>(payload_len);
            frame.push_back(byte2);
        } else if (payload_len < 65536) {
            byte2 |= 126;
            frame.push_back(byte2);
            frame.push_back((payload_len >> 8) & 0xFF);
            frame.push_back(payload_len & 0xFF);
        } else {
            byte2 |= 127;
            frame.push_back(byte2);
            for (int i = 7; i >= 0; --i) {
                frame.push_back((payload_len >> (i * 8)) & 0xFF);
            }
        }
        
        // Masking key (if masked)
        uint8_t masking_key[4] = {0};
        if (mask) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, 255);
            for (int i = 0; i < 4; ++i) {
                masking_key[i] = static_cast<uint8_t>(dis(gen));
                frame.push_back(masking_key[i]);
            }
        }
        
        // Payload (masked if needed)
        for (size_t i = 0; i < payload.size(); ++i) {
            if (mask) {
                frame.push_back(payload[i] ^ masking_key[i % 4]);
            } else {
                frame.push_back(payload[i]);
            }
        }
        
        return frame;
    }
    
    /**
     * @brief Build a text frame / بناء إطار نصي
     */
    static std::vector<uint8_t> build_text(const std::string& text, bool mask = true) {
        std::vector<uint8_t> payload(text.begin(), text.end());
        return build(payload, WebSocketOpcode::Text, true, mask);
    }
    
    /**
     * @brief Build a binary frame / بناء إطار ثنائي
     */
    static std::vector<uint8_t> build_binary(const std::vector<uint8_t>& data, bool mask = true) {
        return build(data, WebSocketOpcode::Binary, true, mask);
    }
    
    /**
     * @brief Build a ping frame / بناء إطار نبض
     */
    static std::vector<uint8_t> build_ping(const std::vector<uint8_t>& data = {}, bool mask = true) {
        return build(data, WebSocketOpcode::Ping, true, mask);
    }
    
    /**
     * @brief Build a pong frame / بناء إطار استجابة
     */
    static std::vector<uint8_t> build_pong(const std::vector<uint8_t>& data = {}, bool mask = true) {
        return build(data, WebSocketOpcode::Pong, true, mask);
    }
    
    /**
     * @brief Build a close frame / بناء إطار إغلاق
     */
    static std::vector<uint8_t> build_close(CloseCode code = CloseCode::Normal, 
                                            const std::string& reason = "",
                                            bool mask = true) {
        std::vector<uint8_t> payload;
        
        // Add close code (2 bytes, network byte order)
        uint16_t code_val = static_cast<uint16_t>(code);
        payload.push_back((code_val >> 8) & 0xFF);
        payload.push_back(code_val & 0xFF);
        
        // Add reason
        payload.insert(payload.end(), reason.begin(), reason.end());
        
        return build(payload, WebSocketOpcode::Close, true, mask);
    }
};

/**
 * @brief WebSocket Frame Parser / محلل إطارات WebSocket
 */
class FrameParser {
public:
    enum class ParseResult {
        Complete,      // إطار كامل
        Incomplete,    // بيانات غير كافية
        Error          // خطأ
    };
    
    struct ParseState {
        ParseResult result = ParseResult::Incomplete;
        size_t bytes_needed = 2;  // الحد الأدنى للإطار
        WebSocketFrame frame;
        std::string error_message;
    };
    
    /**
     * @brief Parse a WebSocket frame / تحليل إطار WebSocket
     * @param data البيانات الواردة
     * @param offset موقع البداية
     * @return حالة التحليل
     */
    static ParseState parse(const std::vector<uint8_t>& data, size_t offset = 0) {
        ParseState state;
        size_t pos = offset;
        size_t available = data.size() - offset;
        
        // Need at least 2 bytes for header
        if (available < 2) {
            state.bytes_needed = 2 - available;
            return state;
        }
        
        // Byte 1: FIN + RSV + Opcode
        uint8_t byte1 = data[pos++];
        state.frame.fin = (byte1 & 0x80) != 0;
        state.frame.rsv1 = (byte1 & 0x40) != 0;
        state.frame.rsv2 = (byte1 & 0x20) != 0;
        state.frame.rsv3 = (byte1 & 0x10) != 0;
        state.frame.opcode = static_cast<WebSocketOpcode>(byte1 & 0x0F);
        
        // Check RSV bits (must be 0 without extensions)
        if (state.frame.rsv1 || state.frame.rsv2 || state.frame.rsv3) {
            state.result = ParseResult::Error;
            state.error_message = "Reserved bits must be 0 / بتات محجوزة يجب أن تكون 0";
            return state;
        }
        
        // Byte 2: MASK + Payload length
        uint8_t byte2 = data[pos++];
        state.frame.masked = (byte2 & 0x80) != 0;
        uint64_t payload_len = byte2 & 0x7F;
        
        // Extended payload length
        size_t header_size = 2;
        if (payload_len == 126) {
            header_size += 2;
            if (available < header_size) {
                state.bytes_needed = header_size - available;
                return state;
            }
            payload_len = (static_cast<uint64_t>(data[pos]) << 8) | data[pos + 1];
            pos += 2;
        } else if (payload_len == 127) {
            header_size += 8;
            if (available < header_size) {
                state.bytes_needed = header_size - available;
                return state;
            }
            payload_len = 0;
            for (int i = 0; i < 8; ++i) {
                payload_len = (payload_len << 8) | data[pos + i];
            }
            pos += 8;
            
            // Check for invalid large payload
            if (payload_len > MAX_FRAME_PAYLOAD) {
                state.result = ParseResult::Error;
                state.error_message = "Payload too large / البيانات كبيرة جداً";
                return state;
            }
        }
        
        state.frame.payload_length = payload_len;
        
        // Masking key (4 bytes if masked)
        if (state.frame.masked) {
            header_size += 4;
            if (available < header_size) {
                state.bytes_needed = header_size - available;
                return state;
            }
            std::memcpy(state.frame.masking_key, &data[pos], 4);
            pos += 4;
        }
        
        // Check if we have the full payload
        size_t total_needed = header_size + payload_len;
        if (available < total_needed) {
            state.bytes_needed = total_needed - available;
            return state;
        }
        
        // Read payload
        state.frame.payload.resize(payload_len);
        std::memcpy(state.frame.payload.data(), &data[pos], payload_len);
        
        // Unmask if needed
        if (state.frame.masked) {
            for (size_t i = 0; i < payload_len; ++i) {
                state.frame.payload[i] ^= state.frame.masking_key[i % 4];
            }
        }
        
        state.result = ParseResult::Complete;
        state.bytes_needed = 0;
        return state;
    }
    
    /**
     * @brief Parse close frame payload / تحليل بيانات إطار الإغلاق
     */
    static CloseInfo parse_close_payload(const std::vector<uint8_t>& payload) {
        CloseInfo info;
        
        if (payload.size() >= 2) {
            uint16_t code = (static_cast<uint16_t>(payload[0]) << 8) | payload[1];
            info.code = static_cast<CloseCode>(code);
            
            if (payload.size() > 2) {
                info.reason = std::string(payload.begin() + 2, payload.end());
            }
        }
        
        return info;
    }
};

} // namespace websocket
} // namespace network
} // namespace sad

#endif // SAD_WEBSOCKET_FRAME_H
