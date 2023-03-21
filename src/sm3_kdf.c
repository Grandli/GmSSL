/*
 *  Copyright 2014-2022 The GmSSL Project. All Rights Reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the License); you may
 *  not use this file except in compliance with the License.
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 */



#include <string.h>
#include <gmssl/sm3.h>
#include <gmssl/endian.h>
#include <gmssl/error.h>

//初始化上下文和设置要输出的长度
void sm3_kdf_init(SM3_KDF_CTX *ctx, size_t outlen)
{
	sm3_init(&ctx->sm3_ctx);
	ctx->outlen = outlen;
}
//派生函数数据处理：内部是哈希运算输入数据
void sm3_kdf_update(SM3_KDF_CTX *ctx, const uint8_t *data, size_t datalen)
{
	sm3_update(&ctx->sm3_ctx, data, datalen);
}
//输出派生密钥结果
void sm3_kdf_finish(SM3_KDF_CTX *ctx, uint8_t *out)
{
	SM3_CTX sm3_ctx;
	size_t outlen = ctx->outlen;
	uint8_t counter_be[4];
	uint8_t dgst[SM3_DIGEST_SIZE];
	uint32_t counter = 1;
	size_t len;

	while (outlen) {
        //把uint32的couter转为uint8的counter_be的字符串
		PUTU32(counter_be, counter);
		counter++;

		sm3_ctx = ctx->sm3_ctx;
        //把counter_be作为数据继续输入
		sm3_update(&sm3_ctx, counter_be, sizeof(counter_be));
        //得到sm3的哈希值dgst
		sm3_finish(&sm3_ctx, dgst);

		len = outlen < SM3_DIGEST_SIZE ? outlen : SM3_DIGEST_SIZE;
		memcpy(out, dgst, len);
		out += len;
		outlen -= len;
	}
    //清除数据，以防被重复使用
	memset(&sm3_ctx, 0, sizeof(SM3_CTX));
	memset(dgst, 0, sizeof(dgst));
}
