//
// Created by lenovo on 2023/3/17.
//

#ifndef GMSSL_DEMO_UTIL_H
#define GMSSL_DEMO_UTIL_H

typedef unsigned int (*TestFunc)(void);

//获取当前时间 单位：毫秒
long getCurrentTime();

//获取打印的长度
void getPrintSize(char *retStr, unsigned long long size);

//测试用例testFunc testTime：时间（s), testName:测试名称
void demoDoUtilTest(TestFunc testFunc, unsigned int testTime, const char *testName);

#endif //GMSSL_DEMO_UTIL_H
