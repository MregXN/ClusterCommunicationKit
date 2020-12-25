#include "client.h"

#include "muduo/base/Logging.h"
#include "muduo/base/Mutex.h"
#include "muduo/net/EventLoopThread.h"
#include "muduo/net/TcpClient.h"
#include "muduo/net/EventLoop.h"
#include "muduo/base/ProcessInfo.h"

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

using namespace muduo;
using namespace muduo::net;

string hostname;

void connection(Client *client)
{
  if (client->connected())
  {
    //printf("... connected successfully!\r\n");
    // client->publish("slave01", "132456");
  }
  else
  {
    printf("... connected unsuccessfully!\r\n");
  }
}

void message(Client *client, string hostip, uint16_t port)
{
  client->sendInfo(); // why just here?
  client->getUser();

  printf("users online: \r\n");
  printf("%s", client->getUsersUpdate().c_str());

  string user;
  printf("input the user you want to chat with: ");
  getline(std::cin, user);

  printf("start chatting with %s :\r\n", user.c_str());
  printf("(use [:q] to quit)\r\n");
  printf("-------------------------------------- \n");

  string line;
  while (getline(std::cin, line))
  {
    if(line == ":q") break;
    client->sendMessage(hostname, user, line);
  }
  printf("\033[2J");
}

void fileTransfer()
{
}

int main(int argc, char *argv[])
{
  // if (argc != 2)
  // {
  //   printf("Usage: ./client [server_ip]:[port] \n");
  //   return 0;
  // }

  // string hostport = argv[1];
  // size_t colon = hostport.find(':');
  // if (colon == string::npos)
  // {
  //   printf("Usage: ./client [server_ip]:[port] \n");
  //   return 0;
  // }
  // string hostip = hostport.substr(0, colon);
  // uint16_t port = static_cast<uint16_t>(atoi(hostport.c_str()+colon+1));

  string hostip = "127.0.0.1";
  uint16_t port = 3000;

  printf("please input your hostname:  \n");
  getline(std::cin, hostname);
  //hostname = "slave01";

  EventLoopThread loop;
  Client client(loop.startLoop(), InetAddress(hostip, port), hostname);
  client.start();
  client.setConnectionCallback(connection);

  printf("\033[2J");
  printf("welcome %s \n", hostname.c_str());
  printf("\n");

  while (1)
  {
    printf("please input number to choose funtions: \n");
    printf("-------------------------------------- \n");
    printf("1. message  \n");
    printf("2. file transfer \n");
    printf("0. quit \n");
    printf("-------------------------------------- \n");

    string cmd;
    getline(std::cin, cmd);

    switch (cmd[0])
    {
    case '1':
      message(&client, hostip, port);
      break;

    case '2':
      fileTransfer();
      break;

    case '0':
      return 0;
      break;
    }
  }
}
