LOCAL_PATH := $(call my-dir)

ifneq ($(TARGET_SIMULATOR),true)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := system.c popen.c dumwolf.c mincrypt/sha.c mincrypt/sha256.c
LOCAL_MODULE := libcrecovery
LOCAL_MODULE_TAGS := eng
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := system.c popen.c dumwolf.c mincrypt/sha.c mincrypt/sha256.c
LOCAL_MODULE := libcrecovery
LOCAL_MODULE_TAGS := eng
include $(BUILD_SHARED_LIBRARY)

endif
