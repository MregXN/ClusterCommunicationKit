#include "codec.h"
#include "muduo/base/Logging.h"

using namespace muduo;
using namespace muduo::net;
using namespace pubsub;

ParseResult pubsub::parseMessage(Buffer *buf,
                                 string *cmd,
                                 string *from,
                                 string *to,
                                 string *content)
{
  ParseResult result = kError;
  const char *crlf = buf->findCRLF();
  if (crlf)
  {
    cmd->assign(buf->peek(), crlf);
    //LOG_INFO << "cmd : " << *cmd;
    const char *crlf1 = buf->findCRLF(crlf+2);
    if(crlf1)
    {
      from->assign(crlf+2, crlf1);
      //LOG_INFO << "from : " << *from;
      const char *crlf2 = buf->findCRLF(crlf1+2);
      if(crlf2)
      {
        to->assign(crlf1+2, crlf2);
        //LOG_INFO << "to : " << *to;
        const char *crlf3 = buf->findCRLF(crlf2+2);
        if(crlf3)
        {
          content->assign(crlf2+2, crlf3);
          //LOG_INFO << "content : " << *content;
          buf->retrieveUntil(crlf3+2);
          result = kSuccess;
        }
        else
        {
          result = kContinue;
        }
        
      }
      else{
        result = kContinue;
      }
    }
    else
    {
      result = kContinue;
    }
    
    // const char *space = std::find(buf->peek(), crlf, ' ');
    // if (space != crlf)
    // {
    //   cmd->assign(buf->peek(), space);
    //   topic->assign(space + 1, crlf);
    //   if (*cmd == "pub")
    //   {
    //     const char *start = crlf + 2;
    //     crlf = buf->findCRLF(start);
    //     if (crlf)
    //     {
    //       content->assign(start, crlf);
    //       buf->retrieveUntil(crlf + 2);
    //       result = kSuccess;
    //     }
    //     else
    //     {
    //       result = kContinue;
    //     }
    //   }
    //   else
    //   {
    //     buf->retrieveUntil(crlf + 2);
    //     result = kSuccess;
    //   }
    // }
    // else
    // {
    //   result = kError;
    // }

  }
  else
  {
    result = kError;
  }
  return result;
}
