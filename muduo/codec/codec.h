#ifndef MUDUO_EXAMPLES_HUB_CODEC_H
#define MUDUO_EXAMPLES_HUB_CODEC_H

// internal header file

#include "muduo/base/Types.h"
#include "muduo/net/Buffer.h"
#include "muduo/base/Logging.h"
#include "muduo/net/TcpConnection.h"
#include "muduo/net/Endian.h"

namespace pubsub
{
  using muduo::string;

  enum ParseResult
  {
    kError,
    kSuccess,
    kContinue,
  };

  ParseResult parseMessage(muduo::net::Buffer *buf,
                           string *cmd,
                           string *from,
                           string *to,
                           string *content);

  class LengthHeaderCodec : muduo::noncopyable
  {
  public:
    typedef std::function<void(const muduo::net::TcpConnectionPtr &,
                               muduo::net::Buffer *,
                               muduo::Timestamp)>
        StringMessageCallback;

    explicit LengthHeaderCodec(const StringMessageCallback &cb)
        : messageCallback_(cb)
    {
    }

    void onMessage(const muduo::net::TcpConnectionPtr &conn,
                   muduo::net::Buffer *buf,
                   muduo::Timestamp receiveTime)
    {
      // while (buf->readableBytes() >= kHeaderLen) // kHeaderLen == 4
      // {
      //   //FIXME: use Buffer::peekInt32()
      //   LOG_INFO << " you have recevied :" << buf-> toStringPiece();
      //   const void *data = buf->peek();
      //   int32_t be32 = *static_cast<const int32_t *>(data); // SIGBUS
      //   const int32_t len = muduo::net::sockets::networkToHost32(be32);
      //   if (len > 65536 || len < 0)
      //   {
      //     LOG_ERROR << "Invalid length " << len;
      //     LOG_ERROR <<  buf->toStringPiece();
      //     conn->shutdown(); // FIXME: disable reading
      //     break;
      //   }
      //   else if (buf->readableBytes() >= len + kHeaderLen)
      //   {
      //     muduo::string message(buf->peek(), len);
      //     muduo::net::Buffer message;
      //     message.append(buf->peek(), len);
      //     buf->retrieve(kHeaderLen);
      //     LOG_INFO << " you have recevied :" << message.toStringPiece();
      //     messageCallback_(conn, &message, receiveTime);
      //   }
      //   else
      //   {
      //     break;
      //   }
      // }
    }

    // FIXME: TcpConnectionPtr  
    // FIXME: You will send twice  why???
    void send(muduo::net::TcpConnection *conn,
              const muduo::StringPiece &message)
    {
      muduo::net::Buffer buf;
      buf.append(message.data(), message.size());
      // int32_t len = static_cast<int32_t>(message.size());
      // int32_t be32 = muduo::net::sockets::hostToNetwork32(len);
      // buf.prepend(&be32, sizeof be32);
      conn->send(&buf);
          //LOG_INFO << "you send something !!!! :"<< message;
    }

  private:
    StringMessageCallback messageCallback_;
    const static size_t kHeaderLen = sizeof(int32_t);
  };

} // namespace pubsub

#endif // MUDUO_EXAMPLES_HUB_CODEC_H
