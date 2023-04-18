#include "sm3_cipher_demo.h"

static const char *testName() {
    return "sm3";
}

//测试用例1
static int doTestSuit1() {
    //需要计算哈希值的消息
    char message[]= "I Love China!";
    //返回的哈希值的比特数据
    unsigned char hash[32] = "";
    unsigned char hash2[32] = "";
    unsigned int hashLen = 32;

    ComPrintf("sm3 test success\n");
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
