#include "client.h"

using namespace muduo;
using namespace muduo::net;
using namespace pubsub;

Client::Client(EventLoop *loop,
               const InetAddress &hubAddr,
               const string &name)
    : client_(loop, hubAddr, name),
      codec_(std::bind(&Client::onMessage, this, _1, _2, _3))
//name_(name)
{
  // FIXME: dtor is not thread safe
  client_.setConnectionCallback(
      std::bind(&Client::onConnection, this, _1));
  client_.setMessageCallback(
      //std::bind(&LengthHeaderCodec::onMessage, &codec_, _1, _2, _3));
      std::bind(&Client::onMessage, this, _1, _2, _3));
}

void Client::start()
{
  client_.connect();
}

void Client::stop()
{
  client_.disconnect();
}

bool Client::connected() const
{
  return conn_ && conn_->connected();
}

void Client::sendInfo(string func, string content)
{
  string message = "info\r\n" + name() + "\r\n" + func + "\r\n" + content + "\r\n";
  send(message);
}

void Client::sendFile(string& to, string& content)
{
  string message = "file\r\n" + name() + "\r\n" + to + "\r\n" + content + "\r\n";
  send(message);
}

void Client::getUser(string func)
{
  nowOnlineUpDate = false;
  string message = "get\r\n" + name() + "\r\n" + func + "\r\n \r\n";
  send(message);
}

bool Client::sendMessage(const string &from, const string &to, const string &content)
{
  string message = "message\r\n" + from + "\r\n" + to + "\r\n" + content + "\r\n";
  return send(message);
}

void Client::onConnection(const TcpConnectionPtr &conn)
{
  MutexLockGuard lock(mutex_);
  if (conn->connected())
  {
    conn_ = conn;
    // FIXME: re-sub
  }
  else
  {
    conn_.reset();
  }
  if (connectionCallback_)
  {
    connectionCallback_(this);
  }
}

void Client::onMessage(const TcpConnectionPtr &conn,
                       Buffer *buf,
                       Timestamp receiveTime)
{
  while (buf->readableBytes() >= 4)
  {
    ParseResult result = kSuccess;
    while (result == kSuccess)
    {
      string cmd;
      string from;
      string to;
      string content;
      result = parseMessage(buf, &cmd, &from, &to, &content);
      if (result == kSuccess)
      {
        if (cmd == "get")
        {
          nowOnline = content;
          nowOnlineUpDate = true;
        }
        else if (cmd == "message")
        {
          printf("%s :", from.c_str());
          printf("%s \r\n", content.c_str());
        }
        else if( cmd == "file" )
        {
          printf("\r\nreceiving files\r\n");



          
        }
        else
        {
        }
        buf->retrieveAll();
      }
      else if (result == kError)
      {
        //conn->shutdown();
      }
    }
  }
}

bool Client::send(const string &message)
{
  MutexLockGuard lock(mutex_);
  bool succeed = false;
  if (conn_ && conn_->connected())
  {
    codec_.send(get_pointer(conn_), message);
    succeed = true;
  }
  return succeed;
}
