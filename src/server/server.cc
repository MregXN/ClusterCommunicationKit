
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
    //std::map<string, TcpConnectionPtr>::iterator it = message_users_.find(conn->name());
    //LOG_INFO << conn->name() << "connected";
    if (conn->connected())
    {
        // conn->setContext(ConnectionSubscription());
        // if (it == message_users_.end())
        // {
        //     message_users_.insert(make_pair(conn->name(), conn));
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
        // if (it != message_users_.end())
        // {
        //     message_users_.erase(conn->name());
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
                std::map<string, TcpConnectionPtr> *ptr;
                if (to == "message")
                    ptr = &message_users_;
                else if (to == "file")
                    ptr = &file_users_;

                string users;
                int count = 0;
                for (auto it = ptr->begin(); it != ptr->end(); it++)
                {
                    users += std::to_string(++count);
                    users += (" " + it->first + "\n");
                }
                string message = "get\r\n" + from + "\r\n" + from + "\r\n" + users + "\r\n";
                conn->send(message);
            }
            else if (cmd == "info")
            {

                std::map<string, TcpConnectionPtr> *ptr;

                if (to == "message")
                    ptr = &message_users_;
                else if (to == "file")
                    ptr = &file_users_;

                if (content == "in")
                {
                    std::map<string, TcpConnectionPtr>::iterator it = ptr->find(from);
                    if (it == ptr->end())
                        ptr->insert(make_pair(from, conn));
                }
                else if (content == "out")
                {
                    ptr->erase(from);
                }
            }
            else if (cmd == "message")
            {
                std::map<string, TcpConnectionPtr>::iterator it = message_users_.find(to);
                if (it != message_users_.end())
                {
                    it->second->send("message\r\n" + from + "\r\n" + to + "\r\n" + content + "\r\n");
                }
            }
            else if (cmd == "file")
            {
                LOG_INFO << "receive file from " <<  from << " to " << to;
                std::map<string, TcpConnectionPtr>::iterator it = file_users_.find(to);
                if (it != file_users_.end())
                {
                    it->second->send("file\r\n" + from + "\r\n" + to + "\r\n" + content + "\r\n");
                }
                else
                    LOG_INFO << "no user " << to;
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
        //buf->retrieveAll();
    }
    buf->retrieveAll();
}
