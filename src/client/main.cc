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

void message(Client *client)
{
  client->sendInfo("message","in"); // why just here?
  client->getUser("message");

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
  client->sendInfo("message","out");
  printf("\033[2J");
}



string readFile(const char* filename)
{
  string content;
  FILE* fp = ::fopen(filename, "rb");
  if (fp)
  {
    // inefficient!!!
    const int kBufSize = 1024*1024;
    char iobuf[kBufSize];
    ::setbuffer(fp, iobuf, sizeof iobuf);

    char buf[kBufSize];
    size_t nread = 0;
    while ( (nread = ::fread(buf, 1, sizeof buf, fp)) > 0)
    {
      content.append(buf, nread);
    }
    ::fclose(fp);
  }
  return content;
}


void fileTransfer(Client *client)
{
  client->sendInfo("file","in"); // why just here?
  client->getUser("file");

  printf("users online: \r\n");
  printf("%s", client->getUsersUpdate().c_str());

  string user;
  printf("input the user you want to transfer file: ");
  getline(std::cin, user);

  string file_path;
  printf("input the file path: ");
  getline(std::cin, file_path);

  string fileContent = readFile(file_path.c_str());

  client->sendFile(user,fileContent);




  client->sendInfo("file","out");
  //printf("\033[2J");

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

  printf("\033[2J");
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
      message(&client);
      break;

    case '2':
      fileTransfer(&client);
      break;

    case '0':
      return 0;
      break;
    }
  }
}
