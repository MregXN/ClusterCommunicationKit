#include "client.h"

using namespace muduo;
using namespace muduo::net;
using namespace pubsub;

Client::Client(EventLoop* loop,
                           const InetAddress& hubAddr,
                           const string& name)
  : client_(loop, hubAddr, name),
    codec_(std::bind(&Client::onMessage, this, _1, _2, _3))
{
  // FIXME: dtor is not thread safe
  client_.setConnectionCallback(
      std::bind(&Client::onConnection, this, _1));
  client_.setMessageCallback(
        std::bind(&LengthHeaderCodec::onMessage, &codec_, _1, _2, _3));
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

bool Client::subscribe(const string& topic, const SubscribeCallback& cb)
{
  string message = "sub " + topic + "\r\n";
  subscribeCallback_ = cb;
  return send(message);
}


void Client::getUser()
{
  string message = "get \r\n";
  send(message);
}

void Client::unsubscribe(const string& topic)
{
  string message = "unsub " + topic + "\r\n";
  send(message);
}


bool Client::publish(const string& topic, const string& content)
{
  string message = "pub " + topic + "\r\n" + content + "\r\n";
  return send(message);
}

void Client::onConnection(const TcpConnectionPtr& conn)
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

void Client::onMessage(const TcpConnectionPtr& conn,
                             Buffer* buf,
                             Timestamp receiveTime)
{
  ParseResult result = kSuccess;
  while (result == kSuccess)
  {
    string cmd;
    string topic;
    string content;
    result = parseMessage(buf, &cmd, &topic, &content);
    if (result == kSuccess)
    {
      if (cmd == "pub" && subscribeCallback_)
      {
        subscribeCallback_(topic, content, receiveTime);
      }
      else
      {
        LOG_INFO << " you have reveive something:   " << cmd << topic << content ;
      }
    }
    else if (result == kError)
    {
      conn->shutdown();
    }
  }
}

bool Client::send(const string& message)
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
