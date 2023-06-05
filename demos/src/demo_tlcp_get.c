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
#include <netinet/in.h>
#include <gmssl/tls.h>
#include <gmssl/error.h>

int main(int argc, char *argv[])
{
	int ret = -1;
	char *prog = argv[0];
	const int cipher = TLS_cipher_ecc_sm4_cbc_sm3;
	struct hostent *hp;
	int port = 443;
	struct sockaddr_in server;
	int sock;
	TLS_CTX ctx;
	TLS_CONNECT conn;
	char request[1024];
	uint8_t buf[16800];
	char *p;
	size_t len;

	if (!(hp = gethostbyname("sm2only.ovssl.cn"))) {
        fprintf(stderr, "tlcp_client: '-host' invalid");
		goto end;
	}
    port = 443;

	server.sin_addr = *((struct in_addr *)hp->h_addr_list[0]);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

    //创建socket
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		goto end;
	}
    //连接服务器
	if (connect(sock, (struct sockaddr *)&server , sizeof(server)) < 0) {
		perror("connect");
		goto end;
	}

	memset(&ctx, 0, sizeof(ctx));
	memset(&conn, 0, sizeof(conn));
    //初始化tls上下文
	tls_ctx_init(&ctx, TLS_protocol_tlcp, TLS_client_mode);
    //设置密码套件
	tls_ctx_set_cipher_suites(&ctx, &cipher, 1);
    //初始化tls的网络连接信息
	tls_init(&conn, &ctx);
    //设置tls的网络连接的socket
	tls_set_socket(&conn, sock);
    //握手
	if (tls_do_handshake(&conn) != 1) {
		fprintf(stderr, "%s: error\n", prog);
		goto end;
	}
    //设置get信息
	snprintf(request, sizeof(request)-1, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n",
		"/",
             "sm2only.ovssl.cn");
    //发送请求
	tls_send(&conn, (uint8_t *)request, strlen(request), &len);
    //接收发挥数据
	if (tls_recv(&conn, buf, sizeof(buf), &len) != 1) {
		fprintf(stderr, "recv failure\n");
		goto end;
	}
	buf[len] = 0;

	p = strstr((char *)buf, "\r\n\r\n");
	if (p) {
		printf("%s", p + 4);
		fflush(stdout);
	}

end:
	close(sock);
	tls_ctx_cleanup(&ctx);
	tls_cleanup(&conn);
	return 0;
}
