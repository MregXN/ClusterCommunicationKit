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
  typedef std::function<void (Client*)> ConnectionCallback;
  typedef std::function<void (const string& topic,
                              const string& content,
                              muduo::Timestamp)> SubscribeCallback;

  Client(muduo::net::EventLoop* loop,
               const muduo::net::InetAddress& hubAddr,
               const string& name);
  void start();
  void stop();
  bool connected() const;

  void setConnectionCallback(const ConnectionCallback& cb)
  { connectionCallback_ = cb; }

  bool subscribe(const string& topic, const SubscribeCallback& cb);
  void unsubscribe(const string& topic);
  bool publish(const string& topic, const string& content);
  void getUser();

 private:
  void onConnection(const muduo::net::TcpConnectionPtr& conn);
  void onMessage(const muduo::net::TcpConnectionPtr& conn,
                 muduo::net::Buffer* buf,
                 muduo::Timestamp receiveTime);
  bool send(const string& message);

  muduo::net::TcpClient client_;
  MutexLock mutex_;
  muduo::net::TcpConnectionPtr conn_ GUARDED_BY(mutex_);
  ConnectionCallback connectionCallback_;
  SubscribeCallback subscribeCallback_;
  LengthHeaderCodec codec_;
};

#endif  // CLIENT_H