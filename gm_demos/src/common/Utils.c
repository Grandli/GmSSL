#include "Utils.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <sys/time.h>
#include <unistd.h>

//把字符串数据转为十六进制的字符串
char *ComStrToHex(unsigned char *inData, int inLen, bool bUpper)
{
    int outLen = inLen * 2 +2;
	int i = 0;
    char *outData = (char*) malloc(outLen);
    if (!outData)
        return "";
    memset(outData, 0, outLen);

    for(i=0; i<inLen; i++){
        sprintf(outData+i*2, bUpper?"%02X":"%02x", inData[i]);
    }
    return outData;
}
//把十六进制的字符串转为字符串数据
int ComHexToStr(const char *inData, unsigned char *outBuf, int inLen)
{
    unsigned char high, low;
    int idx, retLen=0;
    for (idx=0; idx<inLen; idx+=2)
    {
        high = inData[idx];
        low = inData[idx+1];

        if(high>='0' && high<='9')
            high = high-'0';
        else if(high>='A' && high<='F')
            high = high - 'A' + 10;
        else if(high>='a' && high<='f')
            high = high - 'a' + 10;
        else
            return -1;

        if(low>='0' && low<='9')
            low = low-'0';
        else if(low>='A' && low<='F')
            low = low - 'A' + 10;
        else if(low>='a' && low<='f')
            low = low - 'a' + 10;
        else
            return -1;

        outBuf[retLen++] = high<<4 | low;
    }
    return retLen;
}

//把数据写入到文件（新建文件，或者覆盖文件）
int ComWriteFile(char *filename, unsigned char *data, unsigned int dataLen)
{
    int ret = 1;
    FILE *fp = fopen(filename, "wb");
    if(!fp) {
        return -1;
    }
    ret = fwrite(data, 1, dataLen, fp);
    if(ret != dataLen) {
        return -2;
    }
    fclose(fp);
    return 1;
}

//从文件读取所有数据（dataLen需要传data的长度，以保证能正常的读取）
int ComReadFile(char *filename, unsigned char *data, unsigned int *dataLen)
{
    int ret = 1;
    int fLen = 0;
    FILE *fp = fopen(filename, "rb");
    if(!fp) {
        return -1;
    }
    //先将指针偏移到文件尾
    fseek(fp, 0, SEEK_END);
    fLen = ftell(fp);
    if(fLen>*dataLen) {
        *dataLen = fLen;
        fclose(fp);
        return -2;
    }
    //再将指针偏移到文件头
    fseek(fp, 0, SEEK_SET);
    //读取文件
    ret = fread(data, 1, fLen, fp);
    if(ret != fLen) {
        fclose(fp);
        return -3;
    }
    *dataLen = fLen;

    fclose(fp);
    return 1;
}

//按16进制打印内存数据
void dumpMem(unsigned char *data, int len, const char *tag)
{
    int i;
    //数据为空返回
    if(data==NULL)
    {
        return;
    }
    printf("----begin dump %s len: %d ---\n", tag, len);
    for(i=0; i<len; i++)
        printf("0x%02x,",(unsigned char)(data[i]));
    printf("\n----dump %s end---\n", tag);
}

//获取当前时间 单位：毫秒
long getCurrentTime() {
#ifdef WIN32
    return clock();
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long total_time = tv.tv_sec*1000+tv.tv_usec/1000;
    return total_time;
#endif
}
//休眠处理 单位：毫秒
void doSleep(long ms) {
#ifdef WIN32
    Sleep(ms);
#else
    usleep(1000*ms);
#endif
}


