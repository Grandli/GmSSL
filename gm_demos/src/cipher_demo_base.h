#ifndef EMBED_SDK_CIPHER_DEMO_BASE_H
#define EMBED_SDK_CIPHER_DEMO_BASE_H

#include "gmssl_common.h"
#include "common/Utils.h"
#include <stdio.h>

/*
 * 说明：
 * 此C测试用例框架设计比较简单，只是一个雏形，未做深入的分析设计，后续会对功能进一步完善。
 * edit by liguirong
 *
 */

//测试的状态
typedef enum {
    TEST_NONE,  //没有测试
    TEST_TESTING,  //测试中
    TEST_OVER  //测试完成
}DemoTestState;

//测试结果的信息
typedef struct _TestResultInfo{
    char testName[32];//测试的名字
    int totalTimes;//总共需要测试的次数
    int successTimes;//成功测试的次数
    long costTime;//完成测试使用的时间
}TestResultInfo;

TestResultInfo *CreateTestResultInfo(const char *name, int testTimes);

//测试回调：testName返回的测试内容，testTimes返回的是测试次数，successTimes返回的是成功测试的次数， costTime传入的是
typedef void (*TestCB)(TestResultInfo resultInfo);

typedef struct _Cipher_Demo_Base {
    TestResultInfo *resultInfo;
    int haveTestTimes;
    int testState;
    long testBeginTime;

    TestResultInfo * (*doTest)(int testTimes);
    const char *(*testName)(void);
}Cipher_Demo_Base;

//开始测试
void CommonBeginTest(Cipher_Demo_Base *cipherDemo, const char *name, int testTimes);

//结束测试
TestResultInfo *CommonEndTest(Cipher_Demo_Base *cipherDemo);


//初始化一个初始用例
void CommonDemoInit(Cipher_Demo_Base *cipherDemo);
//销毁一个用例
void CommonDemoExit(Cipher_Demo_Base *cipherDemo);


#endif //EMBED_SDK_CIPHER_DEMO_BASE_H
