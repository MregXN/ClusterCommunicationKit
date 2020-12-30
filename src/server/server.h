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

class Server
{
 public:
  Server(muduo::net::EventLoop* loop,const muduo::net::InetAddress& listenAddr);
  void start();

private:
  void onConnection(const TcpConnectionPtr& conn);
  void onMessage(const TcpConnectionPtr& conn,Buffer* buf,Timestamp receiveTime);

  EventLoop* loop_;
  TcpServer server_;
  std::map<string, TcpConnectionPtr> message_users_;
  std::map<string, TcpConnectionPtr> file_users_;
};

#endif  // SERVER_H
