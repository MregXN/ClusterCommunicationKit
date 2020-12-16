#ifndef CLIENT_H
#define CLIENT_H

#include "muduo/net/TcpClient.h"

using muduo::string;

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
  muduo::net::TcpConnectionPtr conn_;
  ConnectionCallback connectionCallback_;
  SubscribeCallback subscribeCallback_;
};

#endif  // CLIENT_H