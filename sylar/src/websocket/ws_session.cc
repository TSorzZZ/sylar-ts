
#include "endiann.h"
#include "http.h"
#include "http_session.h"
#include "log.h"
#include "config.h"
#include <cstdint>
#include <cstring>
#include <string>
#include "ws_session.h"

namespace sylar {

namespace http {

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

sylar::ConfigVar<uint32_t>::ptr g_websocket_message_max_size
    = sylar::Config::Lookup("websocket.message.max_size"
            ,(uint32_t) 1024 * 1024 * 32, "websocket message max size");


std::string WSFrameHead::toString() const {
    std::stringstream ss;
    ss << "[WSFrameHead fin=" << fin
       << " rsv1=" << rsv1
       << " rsv2=" << rsv2
       << " rsv3=" << rsv3
       << " opcode=" << opcode
       << " mask=" << mask
       << " payload=" << payload
       << "]";
    return ss.str();
}

WSFrameMessage::WSFrameMessage(int opcode, const std::string& data):m_opcode(opcode)
    ,m_data(data) {
}


WSSession::WSSession(Socket::ptr sock, bool owner):HttpSession(sock, owner){

}

/// server client
HttpRequest::ptr WSSession::handleShake(){
    HttpRequest::ptr req;
    do{
        req = recvRequest();
        if(!req){
            SYLAR_LOG_INFO(g_logger) << "invalid http request";
        }

        //websocket版本
        if(req->getHeaderAs<int>("Sec-webSocket-Version") != 13) {
            SYLAR_LOG_INFO(g_logger) << "http header Sec-webSocket-Version != 13";
            break;
        }
        //私钥？
        std::string key = req->getHeader("Sec-WebSocket-Key");
        if(key.empty()) {
            SYLAR_LOG_INFO(g_logger) << "http header Sec-WebSocket-Key = null";
            break;
        }

        std::string v = key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
        v = sylar::base64encode(sylar::sha1sum(v));

        auto rsp = req->createResponse();
        rsp->setStatus(HttpStatus::SWITCHING_PROTOCOLS);
        rsp->setWebsocket(true);
        rsp->setReason("WebSocket Protocol Handshake");
        rsp->setHeader("Upgrade", "websocket");
        rsp->setHeader("Connection", "Upgrade");
        rsp->setHeader("Sec-WebSocket-Accept", v);

        //发回给客户端
        sendResponse(rsp);
        SYLAR_LOG_INFO(g_logger) << *req;
        SYLAR_LOG_INFO(g_logger) << *rsp;
        return req;

    }while(false);
    if(req) {
        SYLAR_LOG_INFO(g_logger) << *req;
    }
    return nullptr;
}

WSFrameMessage::ptr WSSession::recvMessage(){
    return WSRecvMessage(this, false);
}

int32_t WSSession::sendMessage(WSFrameMessage::ptr msg, bool fin){
    return WSSendMessage(this, msg, false, fin);
}

int32_t WSSession::sendMessage(const std::string& msg, int32_t opcode, bool fin){
    return WSSendMessage(this, std::make_shared<WSFrameMessage>(opcode, msg), false, fin);
}

int32_t WSSession::ping(){
    return WSPing(this);
}

int32_t WSSession::pong(){
    return WSPong(this);
}



//接收一个ws消息帧
WSFrameMessage::ptr WSRecvMessage(Stream* stream, bool client){
    int opcode = 0;
    std::string data;
    int cur_len = 0;
    //一直读 读够为止
    do{
        WSFrameHead ws_head;
        if(stream->readFixSize(&ws_head, sizeof(ws_head)) <= 0) break;
        SYLAR_LOG_DEBUG(g_logger) << "WSFrameHead " << ws_head.toString();
        if(ws_head.opcode == WSFrameHead::PING){
            SYLAR_LOG_INFO(g_logger) << "PING";
            if(WSPong(stream) <= 0) {
                break;
            }
        }else if(ws_head.opcode == WSFrameHead::PONG) {
        }else if(ws_head.opcode == WSFrameHead::CONTINUE ||
                  ws_head.opcode == WSFrameHead::TEXT_FRAME || 
                  ws_head.opcode == WSFrameHead::BIN_FRAME) {
            //表示该帧的数据是否被掩码处理。
            //如果是客户端发送给服务器的数据，必须为1。
            //服务器发送给客户端的数据可以为0。
            if(!client && !ws_head.mask){
                SYLAR_LOG_INFO(g_logger) << "WSFrameHead mask != 1";
                break;
            }
            uint64_t length = 0;
            if(ws_head.payload == 126){  //126说明要借两位
                uint16_t len = 0;
                if(stream->readFixSize(&len, sizeof(len)) <= 0) {
                    break;
                }
                length = sylar::byteswapOnLittleEndian(len);
            }else if(ws_head.payload == 127) {  //127说明要借八位
                uint64_t len = 0;
                if(stream->readFixSize(&len, sizeof(len)) <= 0) {
                    break;
                }
                length = sylar::byteswapOnLittleEndian(len);
            } else {    //不用借位 直接取长度
                length = ws_head.payload;
            }
            //超出长度
            if((cur_len + length) >= g_websocket_message_max_size->getValue()) {
                SYLAR_LOG_WARN(g_logger) << "WSFrameMessage length > "
                    << g_websocket_message_max_size->getValue()
                    << " (" << (cur_len + length) << ")";
                break;
            }

            char mask[4] = {0};
            if(ws_head.mask) {
                if(stream->readFixSize(mask, sizeof(mask)) <= 0) {
                    break;
                }
            }

            data.resize(cur_len + length);
            //从当前位置读出length长度的数据
            if(stream->readFixSize(&data[cur_len], length) <= 0) {
                break;
            }

            //将data的每一位与mask进行异或
            if(ws_head.mask){
                for(int i = 0; i < length; i++){
                    data[cur_len + i] ^= mask[i % 4];
                }
            }
            cur_len += length;

            if(!opcode && ws_head.opcode != WSFrameHead::CONTINUE) {
                opcode = ws_head.opcode;
            }

            //这一段数据读完整了，才返回
            if(ws_head.fin) {
                SYLAR_LOG_DEBUG(g_logger) << data;
                return WSFrameMessage::ptr(new WSFrameMessage(opcode, std::move(data)));
            }
        } else //非法操作符
        {
            SYLAR_LOG_DEBUG(g_logger) << "invalid opcode=" << ws_head.opcode;
        }
        
    }while(true);
    stream->close();
    return nullptr;

}

//发送消息
int32_t WSSendMessage(Stream* stream, WSFrameMessage::ptr msg, bool client, bool fin){
    do{
        WSFrameHead ws_head;
        memset(&ws_head, 0, sizeof(ws_head));
        ws_head.fin = fin;
        ws_head.opcode = msg->getOpcode();
        ws_head.mask = client;
        uint64_t size = msg->getData().size();
        if(size < 126){
            ws_head.payload = size;
        }else if(size < 65536) {
            ws_head.payload = 126;
        } else {
            ws_head.payload = 127;
        }

        if(stream->writeFixSize(&ws_head, sizeof(ws_head)) <= 0){
            break;
        }

        //如果是126或127 需要多写入
        if(ws_head.payload == 126){
            uint16_t len = size;
            len = sylar::byteswapOnLittleEndian(len);
            if(stream->writeFixSize(&len, sizeof(len)) <= 0){
                break;
            }
        }
        else if(ws_head.payload == 127){
            uint64_t len = size;
            len = sylar::byteswapOnLittleEndian(len);
            if(stream->writeFixSize(&len, sizeof(len)) <= 0){
                break;
            }
        }

        //加密， 构造mask
        if(client){
            char mask[4];
            uint32_t rand_value = rand();
            memcpy(mask, &rand_value, sizeof(mask));
            std::string& data = msg->getData();
            for(size_t i = 0; i < data.size(); ++i) {
                data[i] ^= mask[i % 4];
            }

            if(stream->writeFixSize(mask, sizeof(mask)) <= 0) {
                break;
            }
        }

        //数据
        if(stream->writeFixSize(msg->getData().c_str(), size) <= 0) {
            break;
        }
        return size + sizeof(ws_head);


    }while(0);
    stream->close();
    return -1;
}

int32_t WSPing(Stream* stream){
    WSFrameHead ws_head;
    memset(&ws_head, 0, sizeof(ws_head));
    ws_head.fin = 1;
    ws_head.opcode = WSFrameHead::PING;
    int32_t v = stream->writeFixSize(&ws_head, sizeof(ws_head));
    if(v <= 0) {
        stream->close();
    }
    return v;
}

int32_t WSPong(Stream* stream){
    WSFrameHead ws_head;
    memset(&ws_head, 0, sizeof(ws_head));
    ws_head.fin = 1;
    ws_head.opcode = WSFrameHead::PONG;
    int32_t v = stream->writeFixSize(&ws_head, sizeof(ws_head));
    if(v <= 0) {
        stream->close();
    }
    return v;
}


}

}