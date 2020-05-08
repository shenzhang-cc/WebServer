#include "util.h"
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

ssize_t readn(int fd, void *buff, size_t n)
{
    size_t nleft = n;  // 剩余待读取的字节数
    ssize_t nread = 0;  // 一次读取操作读取到的字节数
    ssize_t readSum = 0;  // 读取的总字节数
    char *ptr = (char*)buff;
    while (nleft > 0)
    {
        if ((nread = read(fd, ptr, nleft)) < 0)
        {
            // 异常处理
            if (errno == EINTR)
                nread = 0;
            else if (errno == EAGAIN)
            {
                return readSum;
            }
            else
            {
                return -1;
            }
        }
        else if (nread == 0)
            break;
        readSum += nread;
        nleft -= nread;
        ptr += nread;
    }
    return readSum;
}

ssize_t writen(int fd, void *buff, size_t n)
{
    size_t nleft = n;
    ssize_t nwritten = 0;
    ssize_t writeSum = 0;
    char *ptr = (char*)buff;
    while (nleft > 0)
    {
        if ((nwritten = write(fd, ptr, nleft)) <= 0)
        {
            if (nwritten < 0)
            {
                if (errno == EINTR || errno == EAGAIN)
                {
                    nwritten = 0;
                    continue;
                }
                else
                    return -1;
            }
        }
        writeSum += nwritten;
        nleft -= nwritten;
        ptr += nwritten;
    }
    return writeSum;
}

void handle_for_sigpipe()
{
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = SIG_IGN;  // SIG_DFL：默认信号处理程序,SIG_IGN：忽略信号的处理程序
    sa.sa_flags = 0;
    // sigaction检查或修改与指定信号关联的处理动作
    if(sigaction(SIGPIPE, &sa, NULL))  // SIGPIPE在没有读进程的管道上写
        return;
}

int setSocketNonBlocking(int fd)
{
    int flag = fcntl(fd, F_GETFL, 0);
    if(flag == -1)
        return -1;

    flag |= O_NONBLOCK;
    if(fcntl(fd, F_SETFL, flag) == -1)
        return -1;
    return 0;
}