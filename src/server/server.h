#ifndef SERVER_H
#define SERVER_H

#include "../../muduo/codec/codec.h"

#include "muduo/base/Logging.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/TcpServer.h"

#include <map>
#include <set>
#include <stdio.h>
#include <vector>
#include <string>

using namespace muduo;
using namespace muduo::net;
using namespace std;

class Server
{
public:
  Server(muduo::net::EventLoop *loop, const muduo::net::InetAddress &listenAddr);
  void start();

private:
  void onConnection(const TcpConnectionPtr &conn);
  void onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp receiveTime);
  void httpHandle(const TcpConnectionPtr &conn, Buffer *buf, Timestamp receiveTime);
  void clientHandle(const TcpConnectionPtr &conn, Buffer *buf, Timestamp receiveTime);

  EventLoop *loop_;
  TcpServer server_;
  std::map<string, TcpConnectionPtr> users_;
  std::map<string, string> name_maps_;
  std::map<string, vector<string>> user_info;
};

#endif // SERVER_H
