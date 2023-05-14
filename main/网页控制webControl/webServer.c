//
// Created by Tony Li on 2023/4/24.
//
// 引入一些必要的头文件
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <time.h>

#include <wiringPi.h>
#include <softPwm.h>

// 定义一些常量
#define PORT 8080 // 服务器监听的端口号
#define BUFFER_SIZE 1024 // 缓冲区大小
#define CRLF "\r\n" // 回车换行符

// 定义一些辅助函数

// 从套接字中读取一行数据，以CRLF结尾，返回读取的字节数，出错返回-1
int read_line(int sock, char *buf, int size) {
    int i = 0;
    char c = '\0';
    int n;

    while ((i < size - 1) && (c != '\n')) {
        n = recv(sock, &c, 1, 0);
        if (n > 0) {
            if (c == '\r') {
                n = recv(sock, &c, 1, MSG_PEEK);
                if ((n > 0) && (c == '\n')) {
                    recv(sock, &c, 1, 0);
                } else {
                    c = '\n';
                }
            }
            buf[i] = c;
            i++;
        } else {
            c = '\n';
        }
    }
    buf[i] = '\0';

    return i;
}

// 向套接字中写入一行数据，以CRLF结尾，返回写入的字节数，出错返回-1
int write_line(int sock, const char *buf) {
    int n;
    n = send(sock, buf, strlen(buf), 0);
    if (n == -1) {
        return -1;
    }
    n = send(sock, CRLF, strlen(CRLF), 0);
    if (n == -1) {
        return -1;
    }
    return n + strlen(CRLF);
}

// 向套接字中写入一个HTTP响应头，包括状态码，内容类型和内容长度，返回写入的字节数，出错返回-1
int write_header(int sock, int status_code, const char *content_type, int content_length) {
    char buf[BUFFER_SIZE];
    int n;

    // 写入状态行
    sprintf(buf, "HTTP/1.1 %d OK", status_code);
    n = write_line(sock, buf);
    if (n == -1) {
        return -1;
    }

    // 写入内容类型
    sprintf(buf, "Content-Type: %s", content_type);
    n = write_line(sock, buf);
    if (n == -1) {
        return -1;
    }

    // 写入内容长度
    sprintf(buf, "Content-Length: %d", content_length);
    n = write_line(sock, buf);
    if (n == -1) {
        return -1;
    }

    // 写入空行
    n = write_line(sock, "");
    if (n == -1) {
        return -1;
    }

    return n;
}

// 向套接字中写入一个HTTP响应体，包括内容数据，返回写入的字节数，出错返回-1
int write_body(int sock, const char *content) {
    int n;
    n = send(sock, content, strlen(content), 0);
    if (n == -1) {
        return -1;
    }
    return n;
}

// 定义一些GPIO控制小车移动的函数，这里只写函数头，具体内容省略

int PWMA = 1;
int AIN2 = 2;
int AIN1 = 3;

int PWMB = 4;
int BIN2 = 5;
int BIN1 = 6;


unsigned int speed = 30;
unsigned int t_time = 500;

void forward() {
    digitalWrite(AIN2,0);
    digitalWrite(AIN1,1);
    softPwmWrite(PWMA,speed);

    digitalWrite(BIN2,0);
    digitalWrite(BIN1,1);
    softPwmWrite(PWMB,speed);
    delay(t_time);
}

void backward() {
    digitalWrite(AIN2,1);
    digitalWrite(AIN1,0);
    softPwmWrite(PWMA,speed);

    digitalWrite(BIN2,1);
    digitalWrite(BIN1,0);
    softPwmWrite(PWMB,speed);
    delay(t_time);
}

void left() {
    digitalWrite(AIN2,1);
    digitalWrite(AIN1,0);
    softPwmWrite(PWMA,speed);

    digitalWrite(BIN2,0);
    digitalWrite(BIN1,1);
    softPwmWrite(PWMB,speed);
    delay(t_time);
}

void right() {
    digitalWrite(AIN2,0);
    digitalWrite(AIN1,1);
    softPwmWrite(PWMA,speed);

    digitalWrite(BIN2,1);
    digitalWrite(BIN1,0);
    softPwmWrite(PWMB,speed);
    delay(t_time);
}

void stop() {
    digitalWrite(AIN2,0);
    digitalWrite(AIN1,0);
    softPwmWrite(PWMA,0);

    digitalWrite(BIN2,0);
    digitalWrite(BIN1,0);
    softPwmWrite(PWMB,0);
    delay(t_time);
}

// 定义一个处理HTTP请求的函数，根据请求的URL和方法，调用相应的GPIO控制函数或返回相应的内容，返回处理结果，0为成功，-1为失败
// 定义一个处理HTTP请求的函数，根据请求的URL和方法，调用相应的GPIO控制函数或返回相应的内容，返回处理结果，0为成功，-1为失败
int handle_request(int sock) {
    char buf[BUFFER_SIZE];
    char method[BUFFER_SIZE];
    char url[BUFFER_SIZE];
    int n;

    // 读取请求行
    n = read_line(sock, buf, BUFFER_SIZE);
    if (n == -1) {
        return -1;
    }

    // 解析请求方法和URL
    sscanf(buf, "%s %s", method, url);

    // 如果请求方法不是GET，返回405 Method Not Allowed
    if (strcmp(method, "GET") != 0) {
        write_header(sock, 405, "text/plain", 18);
        write_body(sock, "Method Not Allowed");
        return -1;
    }

    // 根据URL判断要执行的操作
    if (strcmp(url, "/") == 0) {
        // 从同级文件目录中读取网页的HTML文件
        // 定义文件名，可以根据需要修改
        const char *filename = "index.html";
        // 定义一个文件指针
        FILE *fp;
        // 打开文件，如果失败，返回404 Not Found
        fp = fopen(filename, "r");
        if (fp == NULL) {
            write_header(sock, 404, "text/plain", 9);
            write_body(sock, "Not Found");
            return -1;
        }
        // 获取文件大小，如果失败，关闭文件并返回500 Internal Server Error
        fseek(fp, 0L, SEEK_END);
        int file_size = ftell(fp);
        if (file_size == -1) {
            fclose(fp);
            write_header(sock, 500, "text/plain", 21);
            write_body(sock, "Internal Server Error");
            return -1;
        }
        // 回到文件开头，准备读取内容
        rewind(fp);
        // 定义一个缓冲区，大小为文件大小加一
        char *content = malloc(file_size + 1);
        // 读取文件内容到缓冲区，如果失败，关闭文件并释放缓冲区，返回500 Internal Server Error
        if (fread(content, file_size, 1, fp) != 1) {
            fclose(fp);
            free(content);
            write_header(sock, 500, "text/plain", 21);
            write_body(sock, "Internal Server Error");
            return -1;
        }
        // 关闭文件
        fclose(fp);
        // 在缓冲区末尾添加空字符，作为字符串结束标志
        content[file_size] = '\0';
        // 返回网页的内容，并设置正确的内容类型和内容长度
        write_header(sock, 200, "text/html", file_size);
        write_body(sock, content);
        // 释放缓冲区
        free(content);
    } else if (strcmp(url, "/forward") == 0) {
        // 调用前进函数
        forward();
        // 返回200 OK和一些文本内容
        write_header(sock, 200, "text/plain", 7);
        write_body(sock, "Forward");
    } else if (strcmp(url, "/backward") == 0) {
        // 调用后退函数
        backward();
        // 返回200 OK和一些文本内容
        write_header(sock, 200, "text/plain", 8);
        write_body(sock, "Backward");
    } else if (strcmp(url, "/left") == 0) {
        // 调用左转函数
        left();
        // 返回200 OK和一些文本内容
        write_header(sock, 200, "text/plain", 4);
        write_body(sock, "Left");
    } else if (strcmp(url, "/right") == 0) {
        // 调用右转函数
        right();
        // 返回200 OK和一些文本内容
        write_header(sock, 200, "text/plain", 5);
        write_body(sock, "Right");
    } else if (strcmp(url, "/stop") == 0) {
        // 调用停止函数
        stop();
        // 返回200 OK和一些文本内容
        write_header(sock, 200, "text/plain", 4);
        write_body(sock, "Stop");
    } else {
        // 其他URL返回404 Not Found
        write_header(sock, 404, "text/plain", 9);
        write_body(sock, "Not Found");
        return -1;
    }

    return 0;
}


// 定义主函数，创建套接字，绑定端口，监听连接，接受请求，处理请求，关闭连接，循环执行
int main(int argc, char *argv[]) {
    int server_sock;
    int client_sock;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_len;

    /*RPI*/
    wiringPiSetup();
    /*WiringPi GPIO*/
    pinMode (1, OUTPUT);	//PWMA
    pinMode (2, OUTPUT);	//AIN2
    pinMode (3, OUTPUT);	//AIN1

    pinMode (4, OUTPUT);	//PWMB
    pinMode (5, OUTPUT);	//BIN2
    pinMode (6, OUTPUT);    //BIN1

    /*PWM output*/
    softPwmCreate(PWMA,0,100);//
    softPwmCreate(PWMB,0,100);


    // 创建套接字
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("socket");
        exit(1);
    }

    // 设置套接字选项，允许重用地址和端口
    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
               &opt, sizeof(opt));

    // 绑定端口
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock,
             (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0) {
        perror("bind");
        exit(1);
    }

    // 监听连接
    if (listen(server_sock, 10) < 0) {
        perror("listen");
        exit(1);
    }

    printf("Listening on port %d...\n", PORT);

    while (1) {
        // 接受请求
        client_len = sizeof(client_addr);
        client_sock = accept(server_sock,
                             (struct sockaddr *)&client_addr,
                             &client_len);
        if (client_sock < 0) {
            perror("accept");
            continue;
        }

        printf("Accepted connection from %s:%d\n",
               inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port));

        // 处理请求
        handle_request(client_sock);

        // 关闭连接
        close(client_sock);
    }

    // 关闭套接字
    close(server_sock);

    return 0;
}

