#include "client.h"
#include <string>
#include "getinfo.h"

using namespace muduo;
using namespace muduo::net;
using namespace pubsub;
using namespace std;

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
      std::bind(&Client::onMessage, this, _1, _2, _3));

  client_.getLoop()->runEvery(1, std::bind(&Client::tiktok, this));

  get_cpuoccupy(&client_info.cpu_stat);
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

void Client::sendFile(string &to, string &content)
{
  string message = "file\r\n" + name() + "\r\n" + to + "\r\n" + content + "\r\n";
  send(message);
}

void Client::getUser()
{
  nowOnlineUpDate = false;
  string message = "get\r\n\r\n\r\n\r\n";
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
  //  sendInfo("online", "");
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
          printf("\033[31m%s: \033[0m", from.c_str());
          printf("%s \r\n", content.c_str());
        }
        else if (cmd == "file")
        {
          string fileName = string(content.begin(), content.begin() + content.find("\n"));
          string fileContent = string(content.begin() + content.find("\n") + 1, content.end());
          printf("\033[31m\r\nreceiving files %s from %s ... \r\n \033[0m", fileName.c_str(), from.c_str());

          FILE *fp = ::fopen(fileName.c_str(), "w");
          if (fp)
          {
            fwrite(fileContent.c_str(), fileContent.size(), 1, fp);
            ::fclose(fp);
          }
          printf("\033[31m\r\nreceive complete \r\n \033[0m");
          printf("\r\n");
        }
        else if (cmd == "info")
        {
          printf("%s \r\n", content.c_str());
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

void Client::tiktok()
{
  CPU_OCCUPY cpu_stat_temp;
  get_memoccupy(&(client_info.mem_stat));
  // double mem_usage = client_info.mem_stat.MemFree * 1.0 / (client_info.mem_stat.MemTotal * 1.0);
  string mem_usage = to_string(static_cast<int>(client_info.mem_stat.MemFree / 1024)) + "MB / " + to_string(static_cast<int>(client_info.mem_stat.MemTotal / 1024)) + "MB";

  get_cpuoccupy(&cpu_stat_temp);
  string cpu_usage = to_string(cal_cpuoccupy(&client_info.cpu_stat, &cpu_stat_temp) * 100) + "%";
  string online_time = to_string(++client_info.online_time);
  string s = online_time + "\n" + cpu_usage + "\n" + mem_usage;

  client_info.cpu_stat = cpu_stat_temp;
  sendInfo("tiktok", s);
}