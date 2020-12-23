
#include "../../muduo/codec/codec.h"
#include "server.h"

#include "muduo/base/Logging.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/TcpServer.h"

#include <map>
#include <set>
#include <stdio.h>

using namespace pubsub;

Topic::Topic(const string &topic)
    : topic_(topic)
{
}

void Topic::add(const TcpConnectionPtr &conn)
{
    audiences_.insert(conn);
    if (lastPubTime_.valid())
    {
        conn->send(makeMessage());
    }
}

void Topic::remove(const TcpConnectionPtr &conn)
{
    audiences_.erase(conn);
}

void Topic::publish(const string &content, Timestamp time)
{
    content_ = content;
    lastPubTime_ = time;
    string message = makeMessage();
    for (std::set<TcpConnectionPtr>::iterator it = audiences_.begin();
         it != audiences_.end();
         ++it)
    {
        (*it)->send(message);
    }
}

string Topic::makeMessage()
{
    return "pub " + topic_ + "\r\n" + content_ + "\r\n";
}


Server::Server(muduo::net::EventLoop *loop,
               const muduo::net::InetAddress &listenAddr)
    : loop_(loop),
      server_(loop, listenAddr, "Server")
{
    server_.setConnectionCallback(
        std::bind(&Server::onConnection, this, _1));
    server_.setMessageCallback(
        std::bind(&Server::onMessage, this, _1, _2, _3));
    loop_->runEvery(1.0, std::bind(&Server::timePublish, this));
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
        conn->setContext(ConnectionSubscription());
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
        result = parseMessage(buf, &cmd, &from,&to, &content);
        if (result != kError)
        {
            if (cmd == "pub")
            {
                LOG_INFO << conn->name() << " publics " << content;
            }
            else if ( cmd == "get")
            {
                LOG_INFO << " want to get user";
                string users;
                for(auto it = users_.begin(); it != users_.end();it++) 
                {
                    //LOG_INFO << "user is " << it->first;
                    users += it->first;
                } 
                LOG_INFO << "user is " << users;
                string message = "get\r\n"+from+"\r\n"+from+"\r\n"+users+"\r\n";
                conn->send(message);
            }
            else if (cmd == "info")
            {
                //LOG_INFO << " you received some info from";
                std::map<string, TcpConnectionPtr>::iterator it = users_.find(from);
                if (it == users_.end())
                {
                    users_.insert(make_pair(from, conn));
                    LOG_INFO << "user" << from << " has added ";
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
            LOG_INFO << " you have received something illegally";
           //conn->shutdown();
        }
    }
}

void Server::timePublish()
{
    Timestamp now = Timestamp::now();
    doPublish("internal", "utc_time", now.toFormattedString(), now);
}

void Server::doSubscribe(const TcpConnectionPtr &conn,
                         const string &topic)
{
    ConnectionSubscription *connSub = boost::any_cast<ConnectionSubscription>(conn->getMutableContext());

    connSub->insert(topic);
    getTopic(topic).add(conn);
}

void Server::doUnsubscribe(const TcpConnectionPtr &conn,
                           const string &topic)
{
    LOG_INFO << conn->name() << " unsubscribes " << topic;
    getTopic(topic).remove(conn);
    // topic could be the one to be destroyed, so don't use it after erasing.
    ConnectionSubscription *connSub = boost::any_cast<ConnectionSubscription>(conn->getMutableContext());
    connSub->erase(topic);
}

void Server::doPublish(const string &source,
                       const string &topic,
                       const string &content,
                       Timestamp time)
{
    getTopic(topic).publish(content, time);
}

Topic &Server::getTopic(const string &topic)
{
    std::map<string, Topic>::iterator it = topics_.find(topic);
    if (it == topics_.end())
    {
        it = topics_.insert(make_pair(topic, Topic(topic))).first;
    }
    return it->second;
}


TcpConnectionPtr Server::getUsers(const string& name)
{
    std::map<string, TcpConnectionPtr>::iterator it = users_.find(name);
    if (it == users_.end())
    {
        //it = users.insert(make_pair(name, Topic(topic))).first;
        return NULL;
    }
    return it->second;
}