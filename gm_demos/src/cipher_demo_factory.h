#ifndef EMBED_SDK_CIPHER_DEMO_FACTORY_H
#define EMBED_SDK_CIPHER_DEMO_FACTORY_H

#include "cipher_demo_base.h"

//密码运算相关类型
typedef enum {
    KEY_DEMO,//密钥下载demo
    SM4_DEMO,//SM4运算demo
    SM9_DEMO,//SM9运算demo
    SM3_DEMO,//SM3运算demo
    HMAC_DEMO,//HMAC运算demo
    MAX_DEMO_NUM
}CipherDemoType;

//创建相应的密码运算实例
Cipher_Demo_Base *FactoryCreateCipherDemo(int type);


#endif //EMBED_SDK_CIPHER_DEMO_FACTORY_H
