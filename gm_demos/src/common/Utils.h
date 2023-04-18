#ifndef UTILS_HEADER_UTILS
#define UTILS_HEADER_UTILS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>



//把字符串数据转为十六进制的字符串
char *ComStrToHex(unsigned char *inData, int inLen, bool bUpper);
//把十六进制的字符串转为字符串数据
int ComHexToStr(const char *inData, unsigned char *outBuf, int inLen);

//按16进制打印内存数据
void dumpMem(unsigned char *data, int len, const char *tag);

//把数据写入到文件（新建文件，或者覆盖文件）成功返回1
int ComWriteFile(char *filename, unsigned char *data, unsigned int dataLen);

//从文件读取所有数据（dataLen需要传data的长度，以保证能正常的读取）成功返回1
//如果返回-2， dataLen会赋值为文件的大小
int ComReadFile(char *filename, unsigned char *data, unsigned int *dataLen);


//获取当前时间 单位：毫秒
long getCurrentTime();
//休眠处理 单位：毫秒
void doSleep(long ms);

#ifdef SMDebug
#define ComPrintf (printf("%s(%d): ",__FILE__, __LINE__), printf)
#else
#define ComPrintf(...)
#endif




#endif