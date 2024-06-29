#ifndef __SYLAR_HTTP_WS_SESSION_H__
#define __SYLAR_HTTP_WS_SESSION_H__


#include "config.h"
#include "http.h"
#include "http_session.h"
#include "sockett.h"
#include <cstdint>
#include <memory>
#include <string>
namespace sylar {

namespace http {

// 紧凑排列  头部总共两字节
#pragma pack(push,1)
struct WSFrameHead{
    enum OPCODE {
        
        CONTINUE = 0,   /// 数据分片帧
        TEXT_FRAME = 1, /// 文本帧
        BIN_FRAME = 2,  /// 二进制帧
        CLOSE = 8,  /// 断开连接
        PING = 0x9, /// PING
        PONG = 0xA  /// PONG
    };
    uint32_t opcode: 4;
    bool rsv3: 1;
    bool rsv2: 1;
    bool rsv1: 1;
    bool fin: 1;
    uint32_t payload: 7;
    bool mask: 1;

    std::string toString() const;
};
#pragma pack(pop)




class WSFrameMessage{
public:
    using ptr = std::shared_ptr<WSFrameMessage>;

    WSFrameMessage(int opcode = 0, const std::string& data = "");

    int getOpcode() const { return m_opcode;}
    void setOpcode(int v) { m_opcode = v;}

    const std::string& getData() const { return m_data;}
    std::string& getData() { return m_data;}
    void setData(const std::string& v) { m_data = v;}
private:
    int m_opcode;
    std::string m_data;
};


class WSSession: public HttpSession{
public:
    using ptr = std::shared_ptr<WSSession>;
    WSSession(Socket::ptr sock, bool owner = true);

    HttpRequest::ptr handleShake(); //websocket握手


    WSFrameMessage::ptr recvMessage();
    int32_t sendMessage(WSFrameMessage::ptr msg, bool fin = true);
    int32_t sendMessage(const std::string& msg, 
                        int32_t opcode = WSFrameHead::TEXT_FRAME,
                        bool fin = true);
    int32_t ping();
    int32_t pong();
private:
    bool handleServerShake();
    bool handleClientShake();

};


extern sylar::ConfigVar<uint32_t>::ptr g_websocket_message_max_size;
WSFrameMessage::ptr WSRecvMessage(Stream* stream, bool client);
int32_t WSSendMessage(Stream* stream, WSFrameMessage::ptr msg, bool client, bool fin);
int32_t WSPing(Stream* stream);
int32_t WSPong(Stream* stream);


}

}



#endif