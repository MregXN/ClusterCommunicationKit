
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
    //std::map<string, TcpConnectionPtr>::iterator it = users_.find(conn->name());
    //LOG_INFO << conn->name() << "connected";
    if (conn->connected())
    {
        // conn->setContext(ConnectionSubscription());
        // if (it == users_.end())
        // {
        //     users_.insert(make_pair(conn->name(), conn));
        // }
    }
    else
    {
        // const ConnectionSubscription &connSub = boost::any_cast<const ConnectionSubscription &>(conn->getContext());
        // // subtle: doUnsubscribe will erase *it, so increase before calling.
        // for (ConnectionSubscription::const_iterator it = connSub.begin();
        //      it != connSub.end();)
        // {
        //     doUnsubscribe(conn, *it++);
        // }
        // if (it != users_.end())
        // {
        //     users_.erase(conn->name());
        // }
    }
}

void Server::onMessage(const TcpConnectionPtr &conn,
                       Buffer *buf,
                       Timestamp receiveTime)
{
    ParseResult result = kSuccess;
    while (result == kSuccess)
    {
        string cmd;
        string from;
        string to;
        string content;
        result = parseMessage(buf, &cmd, &from, &to, &content);
        if (result != kError)
        {
            if (cmd == "pub")
            {
                LOG_INFO << conn->name() << " publics " << content;
            }
            else if (cmd == "get")
            {
                string users;
                int count = 0;
                for (auto it = users_.begin(); it != users_.end(); it++)
                {
                    users += std::to_string(++count);
                    users += (" " + it->first + "\n");
                }
                string message = "get\r\n" + from + "\r\n" + from + "\r\n" + users + "\r\n";
                conn->send(message);
            }
            else if (cmd == "info")
            {
                std::map<string, TcpConnectionPtr>::iterator it = users_.find(from);
                if (it == users_.end())
                {
                    users_.insert(make_pair(from, conn));
                }
            }
            else if (cmd == "message")
            {
                std::map<string, TcpConnectionPtr>::iterator it = users_.find(to);
                if (it != users_.end())
                {
                    it->second->send("message\r\n" + from + "\r\n" + to + "\r\n" + content + "\r\n");
                }
            }
            else
            {
                //conn->shutdown();
                //LOG_INFO << " you have received something illegally";
                //result = kError;
            }
        }
        else
        {
            LOG_INFO << "received something illegally";
            //conn->shutdown();
        }
    }
}

TcpConnectionPtr Server::getUsers(const string &name)
{
    std::map<string, TcpConnectionPtr>::iterator it = users_.find(name);
    if (it == users_.end())
    {
        //it = users.insert(make_pair(name, Topic(topic))).first;
        return NULL;
    }
    return it->second;
}