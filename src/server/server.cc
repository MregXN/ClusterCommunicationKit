
#include "../../muduo/codec/codec.h"
#include "server.h"

#include "muduo/base/Logging.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/TcpServer.h"

#include <map>
#include <set>
#include <stdio.h>
#include <string>

using namespace pubsub;

Server::Server(muduo::net::EventLoop *loop,
               const muduo::net::InetAddress &listenAddr)
    : loop_(loop),
      server_(loop, listenAddr, "Server")
{
    server_.setConnectionCallback(
        std::bind(&Server::onConnection, this, _1));
    server_.setMessageCallback(
        std::bind(&Server::onMessage, this, _1, _2, _3));
    //loop_->runEvery(1.0, std::bind(&Server::timePublish, this));
}

void Server::start()
{
    server_.start();
}

void Server::onConnection(const TcpConnectionPtr &conn)
{
    if (conn->connected())
    {
    }
    else
    {
        string name = name_maps_[conn->name()];
        users_.erase(name);
        name_maps_.erase(conn->name());
    }
}

void Server::onMessage(const TcpConnectionPtr &conn,
                       Buffer *buf,
                       Timestamp receiveTime)
{

    string cmd;
    string from;
    string to;
    string content;

    ParseResult result = parseMessage(buf, &cmd, &from, &to, &content);
    if (result != kError)
    {
        if (cmd == "get")
        {
            std::map<string, TcpConnectionPtr> *ptr = &users_;

            string users;
            int count = 0;
            for (auto it = ptr->begin(); it != ptr->end(); it++)
            {
                users += std::to_string(++count);
                users += (". " + it->first + "\n");
            }
            string message = "get\r\n" + from + "\r\n" + from + "\r\n" + users + "\r\n";
            conn->send(message);
        }
        else if (cmd == "info")
        {
            name_maps_[conn->name()] = from;
            users_[from] = conn;
        }
        else if (cmd == "message")
        {
            std::map<string, TcpConnectionPtr>::iterator it = users_.find(to);
            if (it != users_.end())
            {
                it->second->send("message\r\n" + from + "\r\n" + to + "\r\n" + content + "\r\n");
            }
            else
                conn->send("info\r\n" + from + "\r\n" + to + "\r\n" + "no user " + to + "\r\n");
        }
        else if (cmd == "file")
        {
            LOG_INFO << "receive file from " << from << " to " << to;
            std::map<string, TcpConnectionPtr>::iterator it = users_.find(to);
            if (it != users_.end())
            {
                it->second->send("file\r\n" + from + "\r\n" + to + "\r\n" + content + "\r\n");
            }
            else
                conn->send("info\r\n" + from + "\r\n" + to + "\r\n" + "no user " + to + "\r\n");
        }
        else
        {
            conn->send("info\r\n" + from + "\r\n" + to + "\r\n" + "no cmd " + cmd + "\r\n");
        }
    }
    else
    {
        LOG_WARN << "received something illegally";
    }
    buf->retrieveAll();
}
