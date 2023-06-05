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


#define OneTimeTestAmount  8*1024

unsigned int DoSm4Test()
{
    SM4_KEY sm4_key;
    unsigned char key[16];
    unsigned char iv[16];
    unsigned char plain[10] =  "goodluck";
    //明文数据
    unsigned char *message = (unsigned char *)malloc(OneTimeTestAmount);
    unsigned int messageLen = OneTimeTestAmount;
    //密文数据
    unsigned char *cipherData = (unsigned char *)malloc(OneTimeTestAmount+16);
    unsigned int cipherLen = OneTimeTestAmount+16;
    //解密后的数据
    unsigned char *outData = (unsigned char *)malloc(OneTimeTestAmount+16);
    unsigned int outLen = OneTimeTestAmount+16;
    int i;

    memcpy(message, plain, 8);
    rand_bytes(key, sizeof(key));
    rand_bytes(iv, sizeof(iv));


    sm4_set_encrypt_key(&sm4_key, key);
    sm4_cbc_encrypt(&sm4_key, iv, message, messageLen/SM4_BLOCK_SIZE, cipherData);


    sm4_set_decrypt_key(&sm4_key, key);
    sm4_cbc_decrypt(&sm4_key, iv, cipherData, messageLen/SM4_BLOCK_SIZE, outData);

    return OneTimeTestAmount;
}

int main(void)
{
//    unsigned long long haveTestAmount = 0;
//    unsigned int tmpAmount = 0;
//    unsigned int testTimes = 0;
//    char amountStr[100]="";
//
//    unsigned long currentTime = getCurrentTime();
//
//    while(1){
//        if(getCurrentTime()>=currentTime+10000)
//            break;
//        tmpAmount = DoSm4Test();
//        haveTestAmount += tmpAmount;
//        testTimes++;
//    }
//    getPrintSize(amountStr, haveTestAmount);
//    printf("haveCostTime = %ld ms, testTimes = %d,  testAmount = %s\n",getCurrentTime()-currentTime, testTimes, amountStr);
    demoDoUtilTest(DoSm4Test, 3, "sm4_cbc");

	return 0;
}
