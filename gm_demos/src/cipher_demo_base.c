#include "cipher_demo_base.h"

TestResultInfo *CreateTestResultInfo(const char *name, int testTimes)
{
    TestResultInfo *info  = (TestResultInfo *)malloc(sizeof(TestResultInfo));
    strcpy(info->testName, name);
    info->successTimes = 0;
    info->totalTimes = testTimes;
    info->costTime = 0;
    return info;
}

//开始测试
void CommonBeginTest(Cipher_Demo_Base *cipherDemo, const char *name, int testTimes)
{
    if(cipherDemo->resultInfo==NULL)
    {
        cipherDemo->resultInfo = CreateTestResultInfo(name, testTimes);
    }
    else
    {
        strcpy(cipherDemo->resultInfo->testName, name);
        cipherDemo->resultInfo->totalTimes = testTimes;
    }
    cipherDemo->haveTestTimes = 0;
    cipherDemo->testState = TEST_TESTING;
    cipherDemo->testBeginTime = getCurrentTime();
}

//结束测试
TestResultInfo *CommonEndTest(Cipher_Demo_Base *cipherDemo)
{
    cipherDemo->testState = TEST_OVER;
    if(cipherDemo->resultInfo)
    {
        cipherDemo->resultInfo->costTime = getCurrentTime()-cipherDemo->testBeginTime;
    }
    return cipherDemo->resultInfo;
}

//初始化测试实例
void CommonDemoInit(Cipher_Demo_Base *cipherDemo)
{
    if(cipherDemo->resultInfo)
    {
        free(cipherDemo->resultInfo);
        cipherDemo->resultInfo = NULL;
    }
    cipherDemo->haveTestTimes = 0;
    cipherDemo->testState = 0;
    cipherDemo->testBeginTime = 0;
}

//结束测试实例
void CommonDemoExit(Cipher_Demo_Base *cipherDemo)
{
    if(cipherDemo->resultInfo)
    {
        free(cipherDemo->resultInfo);
        cipherDemo->resultInfo = NULL;
    }
}
