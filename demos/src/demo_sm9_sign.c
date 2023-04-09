/*
 *  Copyright 2014-2022 The GmSSL Project. All Rights Reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the License); you may
 *  not use this file except in compliance with the License.
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gmssl/sm9.h>
#include <gmssl/error.h>
#include "demo_util.h"

static SM9_SIGN_MASTER_KEY sign_master;
static SM9_SIGN_MASTER_KEY sign_master_public;
static SM9_SIGN_KEY sign_key;

//提前准备sm9测试
void PrePareSM9SignTest()
{
    uint8_t buf[512];
    uint8_t *p = buf;
    const uint8_t *cp = buf;
    size_t len;
    const char *id = "Alice";
    sm9_sign_master_key_generate(&sign_master);
    sm9_sign_master_key_extract_key(&sign_master, id, strlen(id), &sign_key);

    sm9_sign_master_public_key_to_der(&sign_master, &p, &len);
    sm9_sign_master_public_key_from_der(&sign_master_public, &cp, &len);

}

#define OneTimeTestAmount  10*1024
unsigned int DoSm9SignTest() {
    SM9_SIGN_CTX sign_ctx;
    const char *id = "Alice";
    uint8_t sig[SM9_SIGNATURE_SIZE];
    //明文数据
    unsigned char message[OneTimeTestAmount]= "chinese standard message";
    unsigned int messageLen = OneTimeTestAmount;
    size_t siglen;
    int ret;

    sm9_sign_init(&sign_ctx);
    sm9_sign_update(&sign_ctx, (uint8_t *) message, messageLen);
    sm9_sign_finish(&sign_ctx, &sign_key, sig, &siglen);

    //format_bytes(stdout, 0, 0, "signature", sig, siglen);


    sm9_verify_init(&sign_ctx);
    sm9_verify_update(&sign_ctx, (uint8_t *) message, messageLen);
    ret = sm9_verify_finish(&sign_ctx, sig, siglen, &sign_master_public, id, strlen(id));
    //printf("verify %s\n", ret == 1 ? "success" : "failure");

    return OneTimeTestAmount;
}

int main(void)
{
    PrePareSM9SignTest();
    demoDoUtilTest(DoSm9SignTest, 1, "sm9Sign");


	return 0;
}
