APP_BUIDL_SCRIPT := ./jni/  #默认情况先在项目的jni子目录下查找Android.mk
APP_PLATFORM := android-28
APP_CPPFLAGS := -std=c++11
#APP_CPPFLAGS += -fexceptions
#APP_CPPFLAGS += -frtti
#APP_STL := stlport_static
#APP_STL :=gnustl_static
#APP_STL :=gnustl_shared

APP_ABI :=arm64-v8a
#APP_ABI +=armeabi
#APP_ABI +=armeabi-v7a
APP_CPPFLAGS += -Wno-error=format-security