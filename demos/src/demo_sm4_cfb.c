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
#include <stdlib.h>
#include <sys/time.h>
#include <gmssl/sm4.h>
#include <gmssl/rand.h>
#include "demo_util.h"


#define OneTimeTestAmount  32

unsigned int DoSm4Test()
{
    SM4_KEY sm4_key;
    unsigned char key[16];
    char *originKey="0123456789abcdeffedcba9876543210";
    unsigned char iv[16];
    char *originIV="0123456789abcdeffedcba9876543210";
    char *plain =  "0123456789abcdeffedcba98765432100123456789abcdeffedcba98765432100123456789abcdeffedcba98765432100123456789abcdef";
    unsigned int plainLen = strlen(plain);
    //明文数据
    unsigned char *message = (unsigned char *)malloc(plainLen);
    unsigned int messageLen = plainLen;
    //密文数据
    unsigned char *cipherData = (unsigned char *)malloc(plainLen+16);
    unsigned int cipherLen = plainLen+16;
    //解密后的数据
    unsigned char *outData = (unsigned char *)malloc(plainLen+16);
    unsigned int outLen = plainLen+16;
    int i;

    messageLen = ComHexToStr(plain, message, plainLen);
    //rand_bytes(key, sizeof(key));
    //rand_bytes(originIV, sizeof(originIV));
    ComHexToStr(originIV, iv, 32);
    ComHexToStr(originKey, key, 32);
//    memcpy(iv, originIV, 16);
//    memcpy(key, originKey, 16);



    sm4_set_encrypt_key(&sm4_key, key);
    sm4_cfb_encrypt(&sm4_key, iv, message, messageLen, cipherData);

    dumpMem(cipherData, messageLen);

    ComHexToStr(originIV, iv, 32);
    ComHexToStr(originKey, key, 32);
//    memcpy(iv, originIV, 16);
//    memcpy(key, originKey, 16);

    sm4_set_encrypt_key(&sm4_key, key);
    sm4_cfb_decrypt(&sm4_key, iv, cipherData, messageLen, outData);

    dumpMem(outData, messageLen);

    return messageLen;
}

unsigned int DoSm4Test2()
{
    SM4_KEY sm4_key;
    unsigned char key[16];
    char *originKey="0123456789abcdeffedcba9876543210";
    unsigned char iv[16];
    char *originIV="0123456789abcdeffedcba9876543210";
    char *plain =  "0123456789abcdeffedcba98765432100123456789abcdeffedcba98765432100123456789abcdeffedcba98765432100123456789abcdef";
    unsigned int plainLen = strlen(plain);
    //明文数据
    unsigned char *message = (unsigned char *)malloc(plainLen);
    unsigned int messageLen = plainLen;
    //密文数据
    unsigned char *cipherData = (unsigned char *)malloc(plainLen+16);
    unsigned int cipherLen = 0;
    unsigned int cipherLen2 = 0;
    //解密后的数据
    unsigned char *outData = (unsigned char *)malloc(plainLen+16);
    unsigned int outLen = 0;
    unsigned int outLen2 = 0;
    int i;

    memcpy(message, plain, plainLen);
    //rand_bytes(key, sizeof(key));
    //rand_bytes(originIV, sizeof(originIV));
    ComHexToStr(originIV, iv, 32);
    ComHexToStr(originKey, key, 32);
//    memcpy(iv, originIV, 16);
//    memcpy(key, originKey, 16);


    SM4_CFB_CTX cfbCtx;
    sm4_cfb_encrypt_init(&cfbCtx, key, iv);
    sm4_cfb_encrypt_update(&cfbCtx, message, messageLen/2, cipherData, &cipherLen2);
    cipherLen += cipherLen2;
    sm4_cfb_encrypt_update(&cfbCtx, message+messageLen/2, messageLen-messageLen/2, cipherData+cipherLen, &cipherLen2);
    cipherLen += cipherLen2;
    sm4_cfb_encrypt_finish(&cfbCtx, cipherData+cipherLen, &cipherLen2);
    cipherLen += cipherLen2;

    dumpMem(cipherData, cipherLen);

    ComHexToStr(originIV, iv, 32);
    ComHexToStr(originKey, key, 32);
//    memcpy(iv, originIV, 16);
//    memcpy(key, originKey, 16);

    sm4_cfb_decrypt_init(&cfbCtx, key, iv);
    sm4_cfb_decrypt_update(&cfbCtx, cipherData, cipherLen/2, outData, &outLen2);
    outLen += outLen2;
    sm4_cfb_decrypt_update(&cfbCtx, cipherData+cipherLen/2, cipherLen - cipherLen/2, outData+outLen, &outLen2);
    outLen += outLen2;
    sm4_cfb_decrypt_finish(&cfbCtx, outData+outLen, &outLen2);
    outLen += outLen2;

    dumpMem(outData, outLen);

    return messageLen;
}

int main(void)
{
    DoSm4Test();
    DoSm4Test2();
    //demoDoUtilTest(DoSm4Test, 3, "sm4_ofb");

	return 0;
}
