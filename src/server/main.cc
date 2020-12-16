#include "server.h"

#include "muduo/base/Logging.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/TcpServer.h"

#include <stdio.h>

using namespace muduo;
using namespace muduo::net;


int main(int argc, char* argv[])
{
  // if (argc > 1)
  // {
  // uint16_t port = static_cast<uint16_t>(atoi(argv[1]));
    uint16_t port = 3000;
    EventLoop loop;
    if (argc > 2)
    {
      //int inspectPort = atoi(argv[2]);
    }
    Server server(&loop, InetAddress(port));
    server.start();
    loop.loop();
  // }
  // else
  // {
  //   printf("Usage: %s pubsub_port [inspect_port]\n", argv[0]);
  // }
}

