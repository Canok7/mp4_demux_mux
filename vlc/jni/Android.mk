LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := vlc_mp4
VLC_MP4_DIR = $(LOCAL_PATH)/demux
LOCAL_C_INCLUDES :=$(VLC_MP4_DIR) \
  $(LOCAL_PATH)/include \
  $(LOCAL_PATH)/asf
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)
LOCAL_SRC_FILES :=  \
  $(VLC_MP4_DIR)/meta.c \
  $(VLC_MP4_DIR)/fragments.c \
  $(VLC_MP4_DIR)/libmp4.c \
  $(VLC_MP4_DIR)/mp4.c \
  $(VLC_MP4_DIR)/essetup.c \
  $(LOCAL_PATH)/input/stream.c \
  $(LOCAL_PATH)/input/stream_memory.c \
  $(LOCAL_PATH)/input/my_file_stream.c \
  $(LOCAL_PATH)/input/meta.c \
  $(LOCAL_PATH)/input/es_out.c \
  $(LOCAL_PATH)/text/unicode.c \
  $(LOCAL_PATH)/misc/messages.c \
  $(LOCAL_PATH)/misc/es_format.c \
  $(LOCAL_PATH)/misc/block.c \
  $(LOCAL_PATH)/misc/libc.c  \
  $(LOCAL_PATH)/asf/asfpacket.c 
LOCAL_LDLIBS :=  -llog
LOCAL_CFLAGS += -Wall -Werror -Wno-unused-variable -Wno-reorder -Wno-implicit-function-declaration -Wno-int-conversion -Wno-unused-function -Wno-pragma-pack
#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := test_mp4demuxer
LOCAL_STATIC_LIBRARIES := vlc_mp4
LOCAL_SRC_FILES := test_demux.c
LOCAL_LDFLAGS += -pie -fPIE 
include $(BUILD_EXECUTABLE)