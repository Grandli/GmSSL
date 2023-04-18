#include "sm4_cipher_demo.h"

const char *testName() {
    return "sm4";
}

//测试用例1
static int doTestSuit1() {
    //明文数据
    unsigned char message[]= "I Love China!";
    unsigned int messageLen = strlen(message);
    //密文数据
    unsigned char cipherData[128];
    size_t cipherLen = 128;
    size_t cipherLen2 = 128;
    //解密后的数据
    unsigned char outData[128];
    size_t outLen = 128;
    size_t outLen2 = 128;
    //密钥
    unsigned char key[17]="1234567812345678";
    //初始向量
    unsigned char iv[17]="1234567812345678";
    int iRet = 0;
    SM4_CBC_CTX sm4Ctx;

    //rand_bytes(key, sizeof(key));
    //rand_bytes(iv, sizeof(iv));


    sm4_cbc_encrypt_init(&sm4Ctx, key, iv);
    sm4_cbc_encrypt_update(&sm4Ctx, message, messageLen, cipherData, &cipherLen);
    sm4_cbc_encrypt_finish(&sm4Ctx, cipherData+cipherLen, &cipherLen2);
    cipherLen += cipherLen2;

    dumpMem(cipherData, cipherLen, "sm4 cipherdata");


    sm4_cbc_decrypt_init(&sm4Ctx, key, iv);
    sm4_cbc_decrypt_update(&sm4Ctx, cipherData, cipherLen, outData, &outLen);
    sm4_cbc_decrypt_finish(&sm4Ctx, outData+outLen, &outLen2);
    outLen += outLen2;

    if(memcmp(message, outData, messageLen)!=0)
    {
        ComPrintf("sm4 test fail\n");
        return -1;
    }

    ComPrintf("sm4 test success\n");
    return GM_SUCCESS;
}

static TestResultInfo *doTest(int testTimes) {
    //开始测试
    CommonBeginTest(&sm4CipherDemo, testName(), testTimes);

    while(1)
    {
        sm4CipherDemo.haveTestTimes++;
        if(doTestSuit1()==GM_SUCCESS)
        {
            sm4CipherDemo.resultInfo->successTimes++;
        }
        if(sm4CipherDemo.haveTestTimes>=sm4CipherDemo.resultInfo->totalTimes)
        {
            break;
        }
    }

    //结束测试
    CommonEndTest(&sm4CipherDemo);

    return sm4CipherDemo.resultInfo;
}



Cipher_Demo_Base sm4CipherDemo = {
        NULL,
        0,
        0,
        0,
        doTest,
        testName
};
