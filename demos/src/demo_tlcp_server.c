/*
 *  Copyright 2014-2022 The GmSSL Project. All Rights Reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the License); you may
 *  not use this file except in compliance with the License.
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <netinet/in.h>
#include <gmssl/tls.h>
#include <gmssl/error.h>

#define MAX_EVENTS 10
#define BUFFER_SIZE 1024

TLS_CTX ctx;
//处理网络连接
void *doTlcpServerTest(void *arg);

//初始化服务器fd
int initServerFd(uint16_t port)
{
    int server_sockfd, client_sockfd, epollfd, nready, i;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addrlen;
    struct epoll_event ev, events[MAX_EVENTS];


    fprintf(stderr, "begin socket init\n");

    // 创建socket，监听端口
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sockfd < 0) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);
    if (bind(server_sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("bind error");
        exit(EXIT_FAILURE);
    }
    if (listen(server_sockfd, 5) < 0) {
        perror("listen error");
        exit(EXIT_FAILURE);
    }

    return server_sockfd;
}

void *epoll_process(void *arg)
{
    int client_sockfd, nready, i;
    struct sockaddr_in client_addr;
    socklen_t client_addrlen;
    char buf[BUFFER_SIZE];
    struct epoll_event ev, events[MAX_EVENTS];
    int epollfd;
    int server_sockfd = *((int *)arg);
    // 创建epoll句柄，并添加监听事件
    epollfd = epoll_create(1024);
    if (epollfd < 0) {
        perror("epoll_create error");
        exit(EXIT_FAILURE);
    }
    ev.events = EPOLLIN;
    ev.data.fd = server_sockfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, server_sockfd, &ev) < 0) {
        perror("epoll_ctl error");
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "begin epool work\n");
    // 循环等待IO事件
    while (1) {
        nready = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (nready < 0) {
            fprintf(stderr, "epoll_wait error");
            exit(EXIT_FAILURE);
        }
        for (i = 0; i < nready; i++) {
            if (events[i].data.fd == server_sockfd) { // 监听端口有新连接
                client_addrlen = sizeof(client_addr);
                client_sockfd = accept(server_sockfd, (struct sockaddr *) &client_addr, &client_addrlen);
                if (client_sockfd < 0) {
                    fprintf(stderr, "accept error");
                    continue;
                }
                fprintf(stderr, "client connected, ip: %s, port: %d\n", inet_ntoa(client_addr.sin_addr),
                       ntohs(client_addr.sin_port));

                pthread_t tid;
                pthread_create(&tid, NULL, doTlcpServerTest, &client_sockfd);

                // 将新连接的客户端添加到epoll事件中
//                ev.events = EPOLLIN;
//                ev.data.fd = client_sockfd;
//                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, client_sockfd, &ev) < 0) {
//                    perror("epoll_ctl error");
//                    exit(EXIT_FAILURE);
//                }
            } else { // 客户端有数据可读
                memset(buf, 0, sizeof(buf));
                int len = recv(events[i].data.fd, buf, BUFFER_SIZE, 0);
                if (len <= 0) {
                    fprintf(stderr, "client disconnected\n");
                    close(events[i].data.fd);
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                } else {
                    fprintf(stderr, "receive %s %d bytes\n", buf, len);
                    //send(events[i].data.fd, buf, len, 0);
                }
            }
        }
    }

}

//处理网络连接
void *doTlcpServerTest(void *arg)
{
    TLS_CONNECT conn;
    int clientSock = *((int *)arg);
    memset(&conn, 0, sizeof(conn));
    //初始化tls的网络连接信息
    tls_init(&conn, &ctx);
    //设置tls的网络连接的socket
    tls_set_socket(&conn, clientSock);
    fprintf(stdout, "before tls_do_handshake\n");
    //握手
    if (tls_do_handshake(&conn) != 1) {
        fprintf(stderr, "tls_do_handshake: error\n");
        tls_socket_close(conn.sock);
        tls_cleanup(&conn);
        return NULL;
    }

    fprintf(stderr, "tls_do_handshake: success!\n");

    char buf[1600] = {0};
    size_t len = sizeof(buf);
    for (;;) {

        int rv;
        size_t sentLen;

        do {
            len = sizeof(buf);
            if ((rv = tls_recv(&conn, (uint8_t *)buf, sizeof(buf), &len)) != 1) {
                if (rv < 0) fprintf(stderr, "tls_recv: recv failure\n");
                else fprintf(stderr, "tls_recv: Disconnected by remote\n");

                tls_socket_close(conn.sock);
                tls_cleanup(&conn);
                return NULL;
            }
        } while (!len);

        if (tls_send(&conn, (uint8_t *)buf, len, &sentLen) != 1) {
            fprintf(stderr, "tls_send: send failure, close connection\n");
            tls_socket_close(conn.sock);
            tls_cleanup(&conn);
            return NULL;
        }
    }
}

int main(int argc, char *argv[])
{
	const int cipher = TLS_cipher_ecc_sm4_cbc_sm3;
	int port = 9090;
	int sock;
    char *certfile = "encandsign.crt";
    char *signkeyfile = "s_signkey.pem";
    char *signpass = "123456";
    char *enckeyfile = "s_enckey.pem";
    char *encpass = "123456";


    //tls的socket网络库初始化
    if (tls_socket_lib_init() != 1) {
        error_print();
        return -1;
    }

    //初始化socket
    sock = initServerFd(port);

	memset(&ctx, 0, sizeof(ctx));

    //初始化tls上下文（设置tls协议和客户端模式）
	tls_ctx_init(&ctx, TLS_protocol_tlcp, TLS_server_mode);
    //设置密码套件
    tls_ctx_set_cipher_suites(&ctx, &cipher, 1);
    //设置证书和密钥
    if(tls_ctx_set_tlcp_server_certificate_and_keys(&ctx, certfile, signkeyfile, signpass, enckeyfile, encpass) != 1)
    {
        fprintf(stderr, "tls_ctx_set_tlcp_server_certificate_and_keys fail\n");
        return -1;
    }
    fprintf(stderr, "tls_ctx_set_tlcp_server_certificate_and_keys success!\n");

    pthread_t tid;
    pthread_create(&tid, NULL, epoll_process, &sock);

    while(1)
    {
        sleep(1);
    }

    printf("server quit\n");
	close(sock);
	tls_ctx_cleanup(&ctx);
	return 0;
}
