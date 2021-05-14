# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)

# Declares and names the project.

project("cclplayer")
include_directories(${CMAKE_SOURCE_DIR}/include)
# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             ccl-player

             # Sets the library as a shared library.
             SHARED
             # Provides a relative path to your source file(s).
            CCLjni.cpp
            calljava/CCLJavaCall.cpp
            CCLBasePlayer.cpp
            CCLFFmpeg.cpp
            CCLAudio.cpp
            CCLVideo.cpp
            CCLPlayStatus.cpp
            CCLAudioChannel.cpp
            CCLQueue.cpp
        )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.


add_library(
        avcodec
        SHARED
        IMPORTED
)

add_library(
        avfilter
        SHARED
        IMPORTED
)


add_library(
        avformat
        SHARED
        IMPORTED
)


add_library(
        avutil
        SHARED
        IMPORTED
)

add_library(
        swresample
        SHARED
        IMPORTED
)

add_library(
        swscale
        SHARED
        IMPORTED
)


add_library(
        fdk-aac
        SHARED
        IMPORTED
)

message(WARNING "chencl = ------------- ${CMAKE_SOURCE_DIR}/include" )
if(${ANDROID_ABI}  STREQUAL "x86_64")
    set_target_properties(
            avcodec
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/../jniLibs/x86_64/libavcodec.so
    )

    set_target_properties(
            avfilter
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/../jniLibs/x86_64/libavfilter.so
    )

    set_target_properties(
            avformat
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/../jniLibs/x86_64/libavformat.so
    )

    set_target_properties(
            avutil
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/../jniLibs/x86_64/libavutil.so
    )

    set_target_properties(
            swresample
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/../jniLibs/x86_64/libswresample.so
    )

    set_target_properties(
            swscale
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/../jniLibs/x86_64/libswscale.so
    )

    set_target_properties(
            fdk-aac
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/../jniLibs/x86_64/libfdk-aac.so
    )
endif(${ANDROID_ABI}  STREQUAL "x86_64")


if(${ANDROID_ABI}  STREQUAL "armeabi-v7a")
    set_target_properties(
            avcodec
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/../jniLibs/armeabi-v7a/libavcodec.so
    )

    set_target_properties(
            avfilter
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/../jniLibs/armeabi-v7a/libavfilter.so
    )

    set_target_properties(
            avformat
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/../jniLibs/armeabi-v7a/libavformat.so
    )

    set_target_properties(
            avutil
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/../jniLibs/armeabi-v7a/libavutil.so
    )

    set_target_properties(
            swresample
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/../jniLibs/armeabi-v7a/libswresample.so
    )

    set_target_properties(
            swscale
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/../jniLibs/armeabi-v7a/libswscale.so
    )

    set_target_properties(
            fdk-aac
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/../jniLibs/armeabi-v7a/libfdk-aac.so
    )
endif(${ANDROID_ABI}  STREQUAL "armeabi-v7a")


find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       ccl-player
                       fdk-aac
                       avcodec
                       avfilter
                       avformat
                       avutil
                       swresample
                       swscale
                        OpenSLES
                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )