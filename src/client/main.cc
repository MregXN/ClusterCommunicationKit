#include "client.h"

#include "muduo/base/Logging.h"
#include "muduo/base/Mutex.h"
#include "muduo/net/EventLoopThread.h"
#include "muduo/net/TcpClient.h"
//#include "muduo/net/EventLoop.h"
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

  }
  else
  {
    printf("... disconnected!\r\n");
  }
}

void getUsers(Client *client )
{
 // client->sendInfo("online", "");
  client->getUser();
  printf("\033[2J");
  printf("users online: \r\n");
  printf("%s", client->getUsersUpdate().c_str());
  printf("\r\n\r\n");
}

void message(Client *client)
{
  string user;
  printf("input the user you want to chat with: ");
  getline(std::cin, user);

  printf("start chatting with %s :\r\n", user.c_str());
  printf("(use [:q] to quit)\r\n");
  printf("-------------------------------------- \n");

  string line;
  while (getline(std::cin, line))
  {
    if (line == ":q")
      break;
    client->sendMessage(hostname, user, line);
  }
}

string readFile(const char *filename)
{
  string content;
  FILE *fp = ::fopen(filename, "rb");
  if (fp)
  {
    // inefficient!!!
    const int kBufSize = 1024 * 1024;
    char iobuf[kBufSize];
    ::setbuffer(fp, iobuf, sizeof iobuf);

    char buf[kBufSize];
    size_t nread = 0;
    while ((nread = ::fread(buf, 1, sizeof buf, fp)) > 0)
    {
      content.append(buf, nread);
    }
    ::fclose(fp);
  }
  else
  {
    printf("\r\nNo local files \r\n ");
    return string();
  }
  return content;
}

void fileTransfer(Client *client)
{
  string user;
  printf("input the user you want to transfer file with: ");
  getline(std::cin, user);

  string file_path;
  printf("input the file path: ");
  getline(std::cin, file_path);

  string fileContent = readFile(file_path.c_str());
  string fileName = string(file_path.begin() + file_path.rfind('/') + 1, file_path.end());

  string file = fileName + '\n' + fileContent;
  client->sendFile(user, file);
  printf("transfer file successfully !");
  printf("\r\n");
  printf("\r\n");
}

int main(int argc, char *argv[])
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

  // string hostip = "127.0.0.1";
  // uint16_t port = 3000;

  printf("\033[2J");
  printf("please input your hostname:  \n");
  getline(std::cin, hostname);

  EventLoopThread loop;
  Client client(loop.startLoop(), InetAddress(hostip, port), hostname);
  client.start();
  client.setConnectionCallback(connection);

  printf("\033[2J");
  printf("welcome %s \n", hostname.c_str());
  printf("\n");

  sleep(2);
  client.sendInfo("online", "");

  while (1)
  {
    printf("please input number to choose funtions: \n");
    printf("-------------------------------------- \n");
    printf("1. online users  \n");
    printf("2. message  \n");
    printf("3. file transfer \n");
    printf("0. quit \n");
    printf("-------------------------------------- \n");

    string cmd;
    getline(std::cin, cmd);

    switch (cmd[0])
    {
    case '1':
      getUsers(&client);
      break;

    case '2':
      message(&client);
      break;

    case '3':
      fileTransfer(&client);
      break;

    case '0':
      printf("quitting... \n");
      client.stop();
      return 0;
      break;
    }
  }
}
