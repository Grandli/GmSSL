#include "sm3_cipher_demo.h"

static const char *testName() {
    return "sm3";
}

//测试用例1
static int doTestSuit1() {
    //需要计算哈希值的消息
    char message[]= "I Love China!";
    //返回的哈希值
    char *hash = NULL;
    //期待返回的hex编码hash的值
    unsigned char hash2[65] = "C661494FB7E8F3C7FE9C1926D69961FB1A3CCDC2A1C8CDD817FE0B7F777CEA5A";
    unsigned int hash2Len = 64;
    SM3_CTX sm3_ctx;
    size_t len = strlen(message);
    //返回的哈希值
    uint8_t dgst[32];

    sm3_init(&sm3_ctx);
    sm3_update(&sm3_ctx, message, len);
    sm3_finish(&sm3_ctx, dgst);

    hash = ComStrToHex(dgst, 32, true);
    if(memcmp(hash, hash2, hash2Len)!=0)
    {
        ComPrintf("hash data is error\n");
        return -1;
    }

    ComPrintf("sm3 test success hash = %s\n", hash);
    return GM_SUCCESS;
}


static TestResultInfo *doTest(int testTimes) {
    //开始测试
    CommonBeginTest(&sm3CipherDemo, testName(), testTimes);

    while(1)
    {
        sm3CipherDemo.haveTestTimes++;
        if(doTestSuit1()==GM_SUCCESS)
        {
            sm3CipherDemo.resultInfo->successTimes++;
        }
        if(sm3CipherDemo.haveTestTimes>=sm3CipherDemo.resultInfo->totalTimes)
        {
            break;
        }
    }

    //结束测试
    CommonEndTest(&sm3CipherDemo);

    return sm3CipherDemo.resultInfo;
}


Cipher_Demo_Base sm3CipherDemo = {
        NULL,
        0,
        0,
        0,
        doTest,
        testName
};
