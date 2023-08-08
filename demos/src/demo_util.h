//
// Created by lenovo on 2023/3/17.
//

#ifndef GMSSL_DEMO_UTIL_H
#define GMSSL_DEMO_UTIL_H

#include <stdbool.h>

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

//把字符串数据转为十六进制的字符串
char *ComStrToHex(unsigned char *inData, int inLen, bool bUpper);
//把十六进制的字符串转为字符串数据
int ComHexToStr(const char *inData, unsigned char *outBuf, int inLen);

//按16进制打印内存数据
void dumpMem(unsigned char *data, int len);

//把数据写入到文件（新建文件，或者覆盖文件）
int ComWriteFile(char *filename, unsigned char *data, unsigned int dataLen);

//从文件读取所有数据（dataLen需要传data的长度，以保证能正常的读取）
int ComReadFile(char *filename, unsigned char *data, unsigned int *dataLen);

#endif //GMSSL_DEMO_UTIL_H
