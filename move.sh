#!/bin/sh

# 将一个项目(库或可执行文件)复制到特定的目录
if [ ! -d bin/module ]                       # -d 检查 bin/module 是否存在, ! 即不存在 
then 
    mkdir bin/module
else 
    unlink bin/chat_room                 # 删除当前目录下 bin/chat_room 的符号链接
    unlink bin/module/libchat_room.so    # 删除 bin/module/libchat_room.so 符号链接
fi

cp sylar/bin/sylar bin/chat_room         # 将 sylar/bin/sylar 复制为 bin/chat_room
cp lib/libchat_room.so bin/module/       # 将 lib/libchat_room.so 复制到 bin/module/