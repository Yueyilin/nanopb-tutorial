假设已经有protobuf的基础
# 介绍
Nanopb 用于以 Google 的 Protobuf 格式**编码**和**解码**消息，对RAM和代码空间的要求很低。它主要适用于 32 位微控制器。
总的来说就是编码（压缩）-传递-解码（解压）

因此，你手里的某个项目的项目可能包括以下文件：
Nanopb 运行时库：

- pb.h
- pb_common.h 和 pb_common.c（始终需要）
- pb_decode.h 和 pb_decode.c（解码消息时需要）
- pb_encode.h 和 pb_encode.c（编码消息时需要）

协议描述（您可以有很多）：

- person.proto（只是一个例子）
- person.pb.c（自动生成，包含消息描述符）
- person.pb.h（自动生成，包含类型声明和宏）

其他介绍参阅：[官方介绍](https://jpa.kapsi.fi/nanopb/docs/index.html)
# 下载
GitHub源码：[https://github.com/nanopb/nanopb](https://github.com/nanopb/nanopb)
稳定版下载：[https ://jpa.kapsi.fi/nanopb/download/](https://jpa.kapsi.fi/nanopb/download/)
![image.png](https://cdn.nlark.com/yuque/0/2022/png/1595228/1659927086373-c4a9951a-8388-408c-aeb4-9acb7f7413d0.png#clientId=u976b8d59-fd3e-4&crop=0&crop=0&crop=1&crop=1&from=paste&height=598&id=u8973bf5b&margin=%5Bobject%20Object%5D&name=image.png&originHeight=598&originWidth=922&originalType=binary&ratio=1&rotation=0&showTitle=false&size=73794&status=done&style=none&taskId=u26a70f58-9280-46e9-906c-cd7f6051d89&title=&width=922)
下载新版的比如`nanopb-0.4.6-linux-x86.tar.gz`，解压
将解压后的`generator-bin`文件夹添加到环境变量
`gedit ~/.bashrc`，末尾加上
`export PATH=$PATH:/home/aillian/nanopb/nanopb-0.4.6-linux-x86/generator-bin/`
这样我们就可以在终端使用
```cmake
nanopb_generator -D ${OUT_DIR} xxx.proto
protoc --nanopb_out=${OUT_DIR} xxx.proto
```
生成对应的`xxx.pb.c`和`xxx.pb.h`文件
ps：我系统找不到`--nanopb_out`这个选项，有好心人找到原因了请留言告诉我，不过用`nanopb_generator`效果是一样的，生成的文件没有区别
# 快速开始
有关简单情况的完整示例，请参阅[示例/简单/simple.c](https://github.com/nanopb/nanopb/blob/master/examples/simple/simple.c)。有关具有网络接口的更复杂的示例，请参阅[示例/network_server](https://github.com/nanopb/nanopb/tree/master/examples/network_server)子目录。

简单的编码和解码示例
```c
#include <stdio.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include "simple.pb.h"

int main()
{
    /* This is the buffer where we will store our message. */
    uint8_t buffer[128];
    size_t message_length;
    bool status;
    
    /* Encode our message */
    {
        /* Allocate space on the stack to store the message data.
         *
         * Nanopb generates simple struct definitions for all the messages.
         * - check out the contents of simple.pb.h!
         * It is a good idea to always initialize your structures
         * so that you do not have garbage data from RAM in there.
         */
        SimpleMessage message = SimpleMessage_init_zero;
        
        /* Create a stream that will write to our buffer. */
        pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
        
        /* Fill in the lucky number */
        message.lucky_number = 13;
        
        /* Now we are ready to encode the message! */
        status = pb_encode(&stream, SimpleMessage_fields, &message);
        message_length = stream.bytes_written;
        
        /* Then just check for any errors.. */
        if (!status)
        {
            printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
            return 1;
        }
    }
    
    /* Now we could transmit the message over network, store it in a file or
     * wrap it to a pigeon's leg.
     */

    /* But because we are lazy, we will just decode it immediately. */
    
    {
        /* Allocate space for the decoded message. */
        SimpleMessage message = SimpleMessage_init_zero;
        
        /* Create a stream that reads from the buffer. */
        pb_istream_t stream = pb_istream_from_buffer(buffer, message_length);
        
        /* Now we are ready to decode the message. */
        status = pb_decode(&stream, SimpleMessage_fields, &message);
        
        /* Check for errors... */
        if (!status)
        {
            printf("Decoding failed: %s\n", PB_GET_ERROR(&stream));
            return 1;
        }
        
        /* Print the data contained in the message. */
        printf("Your lucky number was %d!\n", (int)message.lucky_number);
    }
    
    return 0;
}
```
# 基本概念
官方文档：[https://jpa.kapsi.fi/nanopb/docs/concepts.html](https://jpa.kapsi.fi/nanopb/docs/concepts.html)
# 快速开始
基础用法例子
有关简单情况的完整示例，请参阅[示例/简单/simple.c](https://github.com/nanopb/nanopb/blob/master/examples/simple/simple.c)。有关具有网络接口的更复杂的示例，请参阅[示例/network_server](https://github.com/nanopb/nanopb/tree/master/examples/network_server)子目录。
```cmake
.
├── build
├── CMakeLists.txt
└── modules
    ├── app
    │   └── simple.c
    └── common
        └── data_struct
            └── proto
                └── data_struct.proto

```
```protobuf
syntax = "proto2";

message MySimpleMessage {
    optional int32 my_number = 1;
}
```
```c
#include <stdio.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include "data_struct.pb.h"

int main()
{
    uint8_t mysimplemessagebuf[MySimpleMessage_size];
    memset(mysimplemessagebuf, 0, MySimpleMessage_size);
    size_t message_length;
    // Encode our message 
    {
        MySimpleMessage message;
        message.my_number = 15;
        pb_ostream_t enc_stream = pb_ostream_from_buffer(mysimplemessagebuf, MySimpleMessage_size);
        if (!pb_encode(&enc_stream, MySimpleMessage_fields, &message)) {
            printf("Encoding failed: %s\n", PB_GET_ERROR(&enc_stream));
        } else {
            message_length = enc_stream.bytes_written;
        }
    }
    // Decode our message 
    {
        MySimpleMessage message;
        pb_istream_t dec_stream = pb_istream_from_buffer(mysimplemessagebuf, message_length);

        if (!pb_decode(&dec_stream, MySimpleMessage_fields, &message)) {
            printf("Decoding failed: %s\n", PB_GET_ERROR(&dec_stream));
        }
        printf("Your lucky number was %d!\n", (int)message.my_number);
    }
    
    return 0;
}
```
```cmake
cmake_minimum_required(VERSION 2.8)
project(NANOPB_CMAKE_SIMPLE C)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
# change to your nanopb dir
set(CMAKE_MODULE_PATH /home/aillian/nanopb/nanopb-0.4.6-linux-x86/extra)
set(SOURCES ${CMAKE_SOURCE_DIR}/modules/app/simple.c)
find_package(Nanopb REQUIRED)
include_directories(${NANOPB_INCLUDE_DIRS})

nanopb_generate_cpp(PROTO_SRCS PROTO_HDRS
    ${CMAKE_SOURCE_DIR}/modules/common/data_struct/proto/data_struct.proto
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -g -O0")

add_executable(exe ${SOURCES} ${PROTO_SRCS} ${PROTO_HDRS})

```
配合API参考就很容易理解了
# API参考
官方文档：[https://jpa.kapsi.fi/nanopb/docs/reference.html](https://jpa.kapsi.fi/nanopb/docs/reference.html)



# 参考

- 主页： [https ://jpa.kapsi.fi/nanopb/](https://jpa.kapsi.fi/nanopb/)
- Git 仓库： [https ://github.com/nanopb/nanopb/](https://github.com/nanopb/nanopb/)
- 文档： [https ://jpa.kapsi.fi/nanopb/docs/](https://jpa.kapsi.fi/nanopb/docs/)
- 论坛： [https ://groups.google.com/forum/#!forum/nanopb](https://groups.google.com/forum/#!forum/nanopb)
- 稳定版下载： [https ://jpa.kapsi.fi/nanopb/download/](https://jpa.kapsi.fi/nanopb/download/)
- 预发布二进制包： [https ://github.com/nanopb/nanopb/actions/workflows/binary_packages.yml](https://github.com/nanopb/nanopb/actions/workflows/binary_packages.yml)

