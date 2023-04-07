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

//提前准备sm2测试
void PrePareSM2Test()
{
    sm2_key_generate(&sm2_key);
    memcpy(&pub_key, &sm2_key, sizeof(SM2_POINT));
}

#define OneTimeTestAmount  5*1024
unsigned int DoSm2Test() {
    //明文数据
    unsigned char message[OneTimeTestAmount]= "chinese standard message";
    unsigned long messageLen = strlen(message);
    //密文数据
    unsigned char cipherData[OneTimeTestAmount+1024];
    unsigned long cipherLen = OneTimeTestAmount+1024;
    //解密后的数据
    unsigned char outData[OneTimeTestAmount+1024]="";
    unsigned long outLen = OneTimeTestAmount+1024;
    int ret;

    beginCommonTime(0);
    ret = sm2_encrypt(&pub_key, message, messageLen, cipherData, &cipherLen);
    if(ret==-1)
    {
        fprintf(stderr, "sm2_encrypt error\n");
        return 0;
    }
    printf("do sm2_encrypt %f ms\n", endCommonTime(0));

    beginCommonTime(0);
    if (sm2_decrypt(&sm2_key, cipherData, cipherLen, outData, &outLen) != 1) {
        fprintf(stderr, "sm2_decrypt error\n");
        return 0;
    }
    printf("do sm2_decrypt %f ms\n", endCommonTime(0));

    //如果数据不一致
    if(memcmp(outData, message, outLen)!=0)
    {
        printf("outData[%ld] = %s\n", outLen, outData);
        return 0;
    }
    return messageLen;
}


int main(void)
{
    PrePareSM2Test();
    int i;
    for(i=0;i<6;i++)
    {
        demoDoUtilTest(DoSm2Test, 1, "sm2");
    }

	return 0;
}
