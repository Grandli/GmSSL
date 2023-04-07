//
// Created by lenovo on 2023/3/17.
//

#ifndef GMSSL_DEMO_UTIL_H
#define GMSSL_DEMO_UTIL_H

#define MAX_TIME_COUNT_NUM 10

typedef unsigned int (*TestFunc)(void);

//获取当前时间 单位：毫秒
long getCurrentTime();

//获取打印的长度
void getPrintSize(char *retStr, unsigned long long size);

//测试用例testFunc testTime：时间（s), testName:测试名称
void demoDoUtilTest(TestFunc testFunc, unsigned int testTime, const char *testName);

//开始测试时间计时
void beginCommonTime(int index);
//结束测试时间计时
float endCommonTime(int index);

#endif //GMSSL_DEMO_UTIL_H
