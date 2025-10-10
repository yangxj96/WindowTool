需要在项目根目录下新建一个 config.local.cmake文件,用于定义一些本地路径的相关问题

本地内容示例:

```cmake
# config.local.cmake
set(CMAKE_PREFIX_PATH "D:/Develop/Platform/Qt/6.9.2/mingw_64")
# 设置默认安装路径（可被命令行覆盖） 再CLION中添加了FORCE才生效了
set(CMAKE_INSTALL_PREFIX "D:/Software/WinTool" CACHE PATH "Installation prefix" FORCE)
```