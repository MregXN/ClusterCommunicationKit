#ifndef SERVER_H
#define SERVER_H

#include "../../muduo/codec/codec.h"

#include "muduo/base/Logging.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/TcpServer.h"

#include <map>
#include <set>
#include <stdio.h>

using namespace muduo;
using namespace muduo::net;
typedef std::set<string> ConnectionSubscription;

class Topic
{
 public:
  Topic(const string& topic);
  void add(const TcpConnectionPtr& conn);
  void remove(const TcpConnectionPtr& conn);
  void publish(const string& content, Timestamp time);
  string getAllAudiencesName();

 private:
  string makeMessage();
  string topic_;
  string content_;
  Timestamp lastPubTime_;
  std::set<TcpConnectionPtr> audiences_;
};

class Server
{
 public:
  Server(muduo::net::EventLoop* loop,const muduo::net::InetAddress& listenAddr);
  void start();

private:
  void onConnection(const TcpConnectionPtr& conn);
  void onMessage(const TcpConnectionPtr& conn,Buffer* buf,Timestamp receiveTime);
  void timePublish();
  void doSubscribe(const TcpConnectionPtr& conn,const string& topic);
  void doUnsubscribe(const TcpConnectionPtr& conn,const string& topic);
  void doPublish(const string& source,const string& topic,const string& content, Timestamp time);

  Topic& getTopic(const string& topic);
  EventLoop* loop_;
  TcpServer server_;
  std::map<string, Topic> topics_;
};

#endif  // SERVER_H
