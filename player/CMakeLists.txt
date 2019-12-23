
cmake_minimum_required(VERSION 3.4.1)

#导入include路径
include_directories(src/main/cpp/include/ffmpeg)

add_library( # Sets the name of the library.
        native-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        src/main/cpp/native-lib.cpp)



find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

#添加动态库
add_library( avcodec-57 SHARED IMPORTED)
#设置动态库路径
set_target_properties( avcodec-57
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/libavcodec-57.so)



add_library( avformat-57 SHARED IMPORTED)
set_target_properties( avformat-57
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/libavformat-57.so)


add_library( avutil-55 SHARED IMPORTED)
set_target_properties( avutil-55
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/libavutil-55.so)



add_library( swresample-2 SHARED IMPORTED)
set_target_properties( swresample-2
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/libswresample-2.so)


add_library( swscale-4 SHARED IMPORTED)
set_target_properties( swscale-4
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/libswscale-4.so)



target_link_libraries( # Specifies the target library.
        native-lib

        avcodec-57
        avformat-57
        avutil-55
        swresample-2
        swscale-4

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})