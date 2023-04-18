cmake_minimum_required(VERSION 3.10)


set(CMAKE_CXX_STANDARD 98)

add_definitions(-DSMDebug)

#设置项目名称
project(gmssl_cipher_demo LANGUAGES C)


#头文件路径
include_directories(${PROJECT_SOURCE_DIR}/include)


#编译文件
add_executable(gmssl_cipher_demo
        src/main.c src/cipher_demo.c
        src/cipher_demo_factory.c
        src/cipher_demo_base.c
        src/cipher_test_report.c
        src/key/key_cipher_demo.c
        src/sm9/sm9_cipher_demo.c
        src/sm4/sm4_cipher_demo.c
        src/sm3/sm3_cipher_demo.c
        src/hmac/hmac_cipher_demo.c
        src/common/Utils.c
        )

#添加动态库
target_link_libraries(gmssl_cipher_demo PUBLIC
        gmssl
        )




