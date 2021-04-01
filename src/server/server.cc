
#include "../../muduo/codec/codec.h"
#include "server.h"

#include "muduo/base/Logging.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/TcpServer.h"
#include "muduo/net/http/HttpContext.h"
#include "muduo/net/http/HttpRequest.h"
#include "muduo/net/http/HttpResponse.h"

#include <map>
#include <set>
#include <stdio.h>
#include <string>

using namespace muduo;
using namespace muduo::net;
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
        conn->setContext(HttpContext());
    }
    else
    {
        string name = name_maps_[conn->name()];
        users_.erase(name);
        name_maps_.erase(conn->name());
        user_info.erase(name);
    }
}

void Server::onMessage(const TcpConnectionPtr &conn,
                       Buffer *buf,
                       Timestamp receiveTime)
{

    const char *crlf = buf->findCRLF();
    if (crlf)
    {
        const char *colon = std::find(buf->peek(), crlf, ' ');
        if (colon == crlf) //客户端消息
        {
            clientHandle(conn, buf, receiveTime);
        }
        else //HTTP请求
        {
            httpHandle(conn, buf, receiveTime);
        }
    }
    else
    {
        conn->shutdown();
    }
}

void Server::clientHandle(const TcpConnectionPtr &conn, Buffer *buf, Timestamp receiveTime)
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
            if (to == "online")
            {
                name_maps_[conn->name()] = from;
                users_[from] = conn;
            }
            else if (to == "tiktok")
            {
                //LOG_INFO << content;

                size_t pos1 = content.find('\n', 0);
                size_t pos2 = content.find('\n', pos1 + 1);
 
                string info1 = content.substr(0, pos1);
                string info2 = content.substr(pos1 + 1, pos2 - pos1 - 1);
                string info3 = content.substr(pos2 + 1, content.size() - pos2- 1);

                // LOG_INFO << info1;
                // LOG_INFO << info2;
                // LOG_INFO << info3;
                user_info[from] = {from,info1, info2, info3};
            }
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

void Server::httpHandle(const TcpConnectionPtr &conn, Buffer *buf, Timestamp receiveTime)
{
    HttpContext *context = boost::any_cast<HttpContext>(conn->getMutableContext());

    // LOG_INFO <<  buf->retrieveAllAsString() << "\r\n";

    //LOG_INFO << context->parseRequest(buf, receiveTime) << "\r\n";

    if (!context->parseRequest(buf, receiveTime))
    {
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }
    if (context->gotAll())
    {
        const string &connection = context->request().getHeader("Connection");
        bool close = connection == "close" ||
                     (context->request().getVersion() == HttpRequest::kHttp10 && connection != "Keep-Alive");
        HttpResponse response(close);

        //  httpCallback_(req, &response);

        LOG_INFO << "Headers " << context->request().methodString() << " " << context->request().path();

        if (context->request().path() == "/")
        {
            string now = Timestamp::now().toFormattedString();

            response.setStatusCode(HttpResponse::k200Ok);
            response.setStatusMessage("OK");
            response.setContentType("text/html");


            string info ;

            for(auto& p : user_info )
            {
                info+= "<h2>" + p.second[0] +  "</h2> <ul>   Online Time:  " + p.second[1] + "s  </ul><ul>   CPU Usage:   "
                +p.second[2] + "  </ul><ul>   MEM Usage:  " +p.second[3] + "  </ul>";
            }

            response.setBody("<html><head><title> ClusterCommunicationKit </title></head>"
                             "<body><h1>Cluster Dashboard</h1>" +
                             info +
                             "</body></html>");
        }
        else
        {
            response.setStatusCode(HttpResponse::k404NotFound);
            response.setStatusMessage("Not Found");
            response.setCloseConnection(true);
        }

        Buffer buffer;
        response.appendToBuffer(&buffer);
        conn->send(&buffer);
        if (response.closeConnection())
        {
            conn->shutdown();
        }

        context->reset();
    }
}