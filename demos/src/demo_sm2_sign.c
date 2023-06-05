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
#include <gmssl/sm2.h>
#include <gmssl/error.h>
#include "demo_util.h"

SM2_KEY sm2_key;
SM2_KEY pub_key;
unsigned char dgst[32];

#define OneTimeTestAmount  5*1024
//提前准备sm2测试
void PrePareSM2SignTest()
{
    //明文数据
    unsigned char message[OneTimeTestAmount]= "chinese standard message";
    unsigned int messageLen = OneTimeTestAmount;
    sm3_digest(message, messageLen, dgst);
    sm2_key_generate(&sm2_key);
    memcpy(&pub_key, &sm2_key, sizeof(SM2_POINT));
}


unsigned int DoSm2FullSignTest()
{
    unsigned char sig[SM2_MAX_SIGNATURE_SIZE];
    size_t sigLen;
    int ret;
    ret = sm2_sign(&sm2_key, dgst, sig, &sigLen);
    if(ret!=1)
    {
        printf("sm2_sign fail\n");
    }

    ret = sm2_verify(&pub_key, dgst, sig, sigLen);
    if (ret!= 1) {
        printf("verify failed\n");
        return 0;
    } else {
        //printf("verify success\n");
    }
    return 32;
}

static unsigned char s_testSign[SM2_MAX_SIGNATURE_SIZE];
static size_t s_testSigLen = 0;
unsigned int DoSm2SignTest()
{
    int ret;
    ret = sm2_sign(&sm2_key, dgst, s_testSign, &s_testSigLen);
    if(ret!=1)
    {
        printf("sm2_sign fail\n");
    }

    memcpy(&pub_key, &sm2_key, sizeof(SM2_POINT));
    return 32;
}

unsigned int DoSm2VerifyTest()
{
    int ret = sm2_verify(&pub_key, dgst, s_testSign, s_testSigLen);
    if (ret!= 1) {
        printf("verify failed\n");
        return 0;
    }
    return 32;
}

int main(void)
{
    PrePareSM2SignTest();
    demoDoUtilTest(DoSm2FullSignTest, 1, "sm2FullSignVerify");
    demoDoUtilTest(DoSm2SignTest, 1, "sm2Sign");
    demoDoUtilTest(DoSm2VerifyTest, 1, "sm2Verify");


	return 0;
}
