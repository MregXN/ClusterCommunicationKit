#ifndef CLIENT_H
#define CLIENT_H

#include "muduo/net/TcpClient.h"
#include "../../muduo/codec/codec.h"
#include "muduo/base/Mutex.h"
#include "muduo/base/Logging.h"

using muduo::string;
using namespace pubsub;
using namespace muduo;
using namespace muduo::net;

// FIXME: dtor is not thread safe
class Client : muduo::noncopyable
{
public:
  typedef std::function<void(Client *)> ConnectionCallback;
  typedef std::function<void(const string &cmd,
                             const string &from,
                             const string &to,
                             const string &content,
                             muduo::Timestamp)>
      SubscribeCallback;

  Client(muduo::net::EventLoop *loop,
         const muduo::net::InetAddress &hubAddr,
         const string &name);
  void start();
  void stop();

  string name()
  {
    //return name_;
    return client_.name();
  };

  string getUsersUpdate()
  {
    while (!nowOnlineUpDate)
      ;
    return nowOnline;
  };

  bool connected() const;

  void setConnectionCallback(const ConnectionCallback &cb)
  {
    connectionCallback_ = cb;
  }

  bool sendMessage(const string &from, const string &to, const string &content);
  void getUser(string func);
  void sendInfo(string func, string content);

private:
  void onConnection(const muduo::net::TcpConnectionPtr &conn);
  void onMessage(const muduo::net::TcpConnectionPtr &conn,
                 muduo::net::Buffer *buf,
                 muduo::Timestamp receiveTime);
  bool send(const string &message);

  muduo::net::TcpClient client_;
  MutexLock mutex_;
  muduo::net::TcpConnectionPtr conn_ GUARDED_BY(mutex_);
  ConnectionCallback connectionCallback_;
  SubscribeCallback subscribeCallback_;
  LengthHeaderCodec codec_;
  string nowOnline;
  bool nowOnlineUpDate;
};

#endif // CLIENT_H