#include "pubsub.h"
#include "codec.h"

#include "muduo/base/Logging.h"
#include "muduo/base/Mutex.h"
#include "muduo/net/EventLoopThread.h"
#include "muduo/net/TcpClient.h"
#include "muduo/net/EventLoop.h"

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

using namespace muduo;
using namespace muduo::net;
using namespace pubsub;

EventLoop* g_loop = NULL;

void connection(PubSubClient* client)
{
  if (client->connected())
  {
    printf("... connected successfully!");
  }
  else
  {
    g_loop->quit();
  }
}

void subscription(const string topic, const string content, Timestamp)
{
  printf("%s: %s\n", topic.c_str(), content.c_str());
}


void message (string name, string hostip,uint16_t port)
{
  
  PubSubClient client(g_loop, InetAddress(hostip, port), name);
  client.setConnectionCallback(connection);
  client.subscribe(name, subscription);
  client.start();
  g_loop->loop();

  string line;
  while (getline(std::cin, line))
  {
    client.publish("slave02", line);
  }

}

void fileTransfer()
{}


int main(int argc, char* argv[])
{
  if (argc != 2)
  {
    printf("Usage: ./client [server_ip]:[port] \n");
    return 0;
  }

  string hostport = argv[1];
  size_t colon = hostport.find(':');
  if (colon == string::npos)
  {
    printf("Usage: ./client [server_ip]:[port] \n");
    return 0;
  }
  string hostip = hostport.substr(0, colon);
  uint16_t port = static_cast<uint16_t>(atoi(hostport.c_str()+colon+1));


  printf("please input your hostname:  \n");
  string name;
  getline(std::cin,name);

  printf("\033[2J");
  printf("welcome %s \n",name.c_str());
  printf("\n");
  printf("please input number to choose funtions: \n");
  printf("-------------------------------------- \n");
  printf("1. message  \n");
  printf("2. file transfer \n");
  printf("0. quit \n");
  printf("-------------------------------------- \n");

  string cmd;
  getline(std::cin,cmd);

  EventLoop loop;
  g_loop = &loop;

  switch(cmd[0])
  {
    case '1': 
      message(name,hostip,port);
    break;

    case '2': 
      fileTransfer();
    break;

    case '0': 
      return 0;
    break;

  }



}
