cmake_minimum_required(VERSION 3.28)
project(untitled C)

set(CMAKE_C_STANDARD 11)
include_directories(D:/ffmpeg-4.0.2-win64-shared/include)
link_directories(D:/ffmpeg-4.0.2-win64-shared/lib)
add_executable(untitled main.c)
target_link_libraries(
        untitled
        avcodec
        avdevice
        avfilter
        avformat
        avutil
        postproc
        swresample
        swscale
)