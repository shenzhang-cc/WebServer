// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#include <getopt.h>
#include <string>
#include "EventLoop.h"
#include "Server.h"
#include "base/Logging.h"


int main(int argc, char *argv[]) {
  int threadNum = 4;   //线程数
  int port = 80;  // http服务器端口号为80
  std::string logPath = "./WebServer.log";  //日志文件的保存位置

  // parse args
  int opt;  // 选项的处理
  const char *str = "t:l:p:";
  // getopt()用于分析命令行参数
  while ((opt = getopt(argc, argv, str)) != -1) {
    switch (opt) {
      case 't': { //选项t用于设定线程的数目
        threadNum = atoi(optarg);  // extern char* optarg, 指向当前选项的参数的指针。全局变量。
        break;  // atoi 将字符串转换为整数
      }
      case 'l': {
        logPath = optarg;
        if (logPath.size() < 2 || optarg[0] != '/') {
          printf("logPath should start with \"/\"\n");
          abort();  //终止不正常的进程
        }
        break;
      }
      case 'p': {
        port = atoi(optarg);
        break;
      }
      default:
        break;
    }
  }
  Logger::setLogFileName(logPath);
// STL库在多线程上应用
#ifndef _PTHREADS  // 如果没有引入这个头文件就
  LOG << "_PTHREADS is not defined !";
#endif
  EventLoop mainLoop;
  Server myHTTPServer(&mainLoop, threadNum, port);
  myHTTPServer.start();
  mainLoop.loop();
  return 0;
}
