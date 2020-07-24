# Copyright (C) 2007 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

LOCAL_PATH := $(call my-dir)

# Needed by build/make/core/Makefile.
RECOVERY_API_VERSION := 3
RECOVERY_FSTAB_VERSION := 2

# libfusesideload (static library)
# ===============================
include $(CLEAR_VARS)
LOCAL_SRC_FILES := fuse_sideload.cpp
LOCAL_CFLAGS := -Wall -Werror
LOCAL_CFLAGS += -D_XOPEN_SOURCE -D_GNU_SOURCE
LOCAL_MODULE := libfusesideload
LOCAL_STATIC_LIBRARIES := \
    libcrypto \
    libbase
include $(BUILD_STATIC_LIBRARY)

# libmounts (static library)
# ===============================
include $(CLEAR_VARS)
LOCAL_SRC_FILES := mounts.cpp
LOCAL_CFLAGS := \
    -Wall \
    -Werror
LOCAL_MODULE := libmounts
LOCAL_STATIC_LIBRARIES := libbase
include $(BUILD_STATIC_LIBRARY)

# librecovery (static library)
# ===============================
include $(CLEAR_VARS)
LOCAL_SRC_FILES := \
    install.cpp
LOCAL_CFLAGS := -Wall -Werror
LOCAL_CFLAGS += -DRECOVERY_API_VERSION=$(RECOVERY_API_VERSION)

ifeq ($(AB_OTA_UPDATER),true)
    LOCAL_CFLAGS += -DAB_OTA_UPDATER=1
endif

LOCAL_MODULE := librecovery
LOCAL_STATIC_LIBRARIES := \
    libminui \
    libotautil \
    libvintf_recovery \
    libcrypto_utils \
    libcrypto \
    libbase \
    libziparchive

include $(BUILD_STATIC_LIBRARY)

# recovery (static executable)
# ===============================
include $(CLEAR_VARS)

commands_recovery_local_path := $(LOCAL_PATH)
 
PTERO_GUI_SRC_PATH := gui/theme

PTERO_GUI_SRC_FILES := \
    $(PTERO_GUI_SRC_PATH)/gui.cpp \
    $(PTERO_GUI_SRC_PATH)/engine.cpp \
    $(PTERO_GUI_SRC_PATH)/FileManager.cpp \
    $(PTERO_GUI_SRC_PATH)/filelist.cpp \
    $(PTERO_GUI_SRC_PATH)/language.cpp \
    $(PTERO_GUI_SRC_PATH)/sidebar.cpp \
    $(PTERO_GUI_SRC_PATH)/statusbar.cpp \
    $(PTERO_GUI_SRC_PATH)/windowpool.cpp
#    $(PTERO_GUI_SRC_PATH)/install_menu.cpp
    

LOCAL_SRC_FILES := \
    bootloader.cpp \
    framework.cpp \
    DataBase.cpp \
    env_service.cpp \
    fs_manager.cpp \
    device_driver.cpp \
    property_manager.cpp \
    global_metadata.cpp \
    pterodon.cpp \
    pterologger.cpp \
    metadata.cpp \
    strdiv.cpp \
    settings.cpp \
    version.cpp \
    $(PTERO_GUI_SRC_FILES)

LOCAL_MODULE := recovery

#LOCAL_STATIC_LIBRARIES :=
#LOCAL_SHARED_LIBRARIES :=

LOCAL_FORCE_STATIC_EXECUTABLE := true

#LOCAL_MODULE_PATH := $(TARGET_RECOVERY_ROOT_OUT)/sbin

LOCAL_REQUIRED_MODULES := mke2fs.conf

LOCAL_CFLAGS += -DRECOVERY_API_VERSION=$(RECOVERY_API_VERSION)
LOCAL_CFLAGS += -Wno-unused-parameter -Wno-unused-variable
LOCAL_CLANG := true

ifneq ($(TARGET_RECOVERY_BACKLIGHT_PATH),)
LOCAL_CFLAGS += -DBACKLIGHT_PATH=\"$(TARGET_RECOVERY_BACKLIGHT_PATH)\"
else
LOCAL_CFLAGS += -DBACKLIGHT_PATH=\"/sys/class/leds/lcd-backlight\"
endif

LOCAL_CFLAGS += -DPTERO_RESOURCES=\"/pterodon\"

LOCAL_LDFLAGS += -Wl,--no-fatal-warnings

##
## libs module name
##
LIBAROMA_MK_ZLIB_LIB := libz
LIBAROMA_MK_PNG_LIB := libpng
LIBAROMA_MK_JPEG_LIB := libjpeg_static
LIBAROMA_MK_FREETYPE_LIB := libft2_libaroma
LIBAROMA_MK_HARFBUZZNG_LIB := libharfbuzz_ng_libaroma
LIBAROMA_MK_MINZIP_LIB := libminzip_libaroma

LOCAL_C_INCLUDES += \
    system/vold \
    system/extras \
    system/core/libcutils \
    system/core/include \
    system/core/adb \
    system/core/healthd \
    system/core/libsparse \
    external/zlib \
    $(LOCAL_PATH)/bootloader_pterodon/include \
    external/e2fsprogs/lib \
    external/libpng \
    $(commands_recovery_local_path)/gui/theme \
    $(commands_recovery_local_path)/gui/libaroma/src \
    $(commands_recovery_local_path)/gui/libaroma/src/aroma/ui \
    $(commands_recovery_local_path)/gui/libaroma/libs/jpeg \
    $(commands_recovery_local_path)/gui/libaroma/src/contrib/platform/linux/include \
    $(commands_recovery_local_path)/gui/libaroma/include \
    $(commands_recovery_local_path)/gui/libaroma/libs/freetype/include \
    $(commands_recovery_local_path)/gui/libaroma/libs/harfbuzz-ng/include \
    $(commands_recovery_local_path)/gui/libaroma/libs/harfbuzz-ng/src/hb-ucdn \
    $(commands_recovery_local_path)/gui/libaroma/libs/harfbuzz-ng/src \
    $(commands_recovery_local_path)/gui/libaroma/libs/minzip
    
LOCAL_STATIC_LIBRARIES += \
   libaroma \
   libft2_libaroma \
   libharfbuzz_ng_libaroma \
   libminzip_libaroma

# Health HAL dependency
LOCAL_STATIC_LIBRARIES += \
    android.hardware.health@2.0-impl \
    android.hardware.health@2.0 \
    android.hardware.health@1.0 \
    android.hardware.health@1.0-convert \
    libhealthstoragedefault \
    libhidltransport \
    libhidlbase \
    libhwbinder_noltopgo \
    libvndksupport \
    libbatterymonitor

LOCAL_STATIC_LIBRARIES += \
    librecovery \
    libbootloader_message_pterodon \
    libverifier \
    libfs_mgr \
    libext4_utils \
    libext2_blkid \
    libext2_uuid \
    libsparse \
    libreboot \
    libziparchive \
    libotautil \
    libmounts \
    libminadbd \
    libasyncio \
    libfusesideload \
    libminui \
    libpng \
    libcrypto_utils \
    libcrypto \
    libvintf_recovery \
    libvintf \
    libhidl-gen-utils \
    libtinyxml2 \
    libbase \
    libutils \
    libcutils \
    liblog \
    libselinux \
    libz \
    libm \
    libc

# libarchive
#LOCAL_STATIC_LIBRARIES += \
#    libarchive

#LOCAL_C_INCLUDES += \
#    external/libarchive/libarchive \
#    external/libarchive/contrib/android/include
    
LOCAL_C_INCLUDES += external/libselinux/include
LOCAL_STATIC_LIBRARIES += libselinux

LOCAL_HAL_STATIC_LIBRARIES := libhealthd

ifeq ($(AB_OTA_UPDATER),true)
    LOCAL_CFLAGS += -DAB_OTA_UPDATER=1
endif

ifeq ($(BOARD_HAS_DOWNLOAD_MODE), true)
    LOCAL_CFLAGS += -DDOWNLOAD_MODE
endif

ifeq ($(TARGET_BUILD_VARIANT),eng)
    LOCAL_CFLAGS += -DSHOW_TESTS
endif

LOCAL_MODULE_PATH := $(TARGET_RECOVERY_ROOT_OUT)/sbin

ifeq ($(TARGET_RECOVERY_UI_LIB),)
  LOCAL_SRC_FILES += default_device.cpp
else
  LOCAL_STATIC_LIBRARIES += $(TARGET_RECOVERY_UI_LIB)
endif

ifeq ($(BOARD_CACHEIMAGE_PARTITION_SIZE),)
LOCAL_REQUIRED_MODULES += recovery-persist recovery-refresh
endif

LOCAL_REQUIRED_MODULES += \
    toybox_static \
    recovery_mkshrc \
    pterodon

# Symlinks
RECOVERY_TOOLS := \
    reboot \
    sh \
    gunzip \
    gzip \
    unzip \
    zip \
    awk \
    $(FILESYSTEM_TOOLS)
LOCAL_POST_INSTALL_CMD := $(hide) $(foreach t,$(RECOVERY_TOOLS),ln -sf ${LOCAL_MODULE} $(LOCAL_MODULE_PATH)/$(t);)

ifneq ($(TARGET_RECOVERY_DEVICE_MODULES),)
    LOCAL_REQUIRED_MODULES += $(TARGET_RECOVERY_DEVICE_MODULES)
endif

include $(BUILD_EXECUTABLE)

# mkshrc
# ===============================
include $(CLEAR_VARS)
LOCAL_MODULE := recovery_mkshrc
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_RECOVERY_ROOT_OUT)/etc
LOCAL_SRC_FILES := etc/mkshrc
LOCAL_MODULE_STEM := mkshrc
include $(BUILD_PREBUILT)

# Reboot static library
# ===============================
include $(CLEAR_VARS)
LOCAL_MODULE := libreboot
LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS := -Dmain=reboot_main
LOCAL_SRC_FILES := ../../system/core/reboot/reboot.c
include $(BUILD_STATIC_LIBRARY)

# recovery-persist (system partition dynamic executable run after /data mounts)
# ===============================
include $(CLEAR_VARS)
LOCAL_SRC_FILES := \
    recovery-persist.cpp \
    rotate_logs.cpp
LOCAL_MODULE := recovery-persist
LOCAL_SHARED_LIBRARIES := liblog libbase
LOCAL_CFLAGS := -Wall -Werror
LOCAL_INIT_RC := recovery-persist.rc
include $(BUILD_EXECUTABLE)

# recovery-refresh (system partition dynamic executable run at init)
# ===============================
include $(CLEAR_VARS)
LOCAL_SRC_FILES := \
    recovery-refresh.cpp \
    rotate_logs.cpp
LOCAL_MODULE := recovery-refresh
LOCAL_SHARED_LIBRARIES := liblog libbase
LOCAL_CFLAGS := -Wall -Werror
LOCAL_INIT_RC := recovery-refresh.rc
include $(BUILD_EXECUTABLE)

# libverifier (static library)
# ===============================
include $(CLEAR_VARS)
LOCAL_MODULE := libverifier
LOCAL_SRC_FILES := \
    asn1_decoder.cpp \
    verifier.cpp
LOCAL_STATIC_LIBRARIES := \
    libotautil \
    libcrypto_utils \
    libcrypto \
    libbase
LOCAL_CFLAGS := -Wall -Werror
include $(BUILD_STATIC_LIBRARY)

include \
    $(commands_recovery_local_path)/boot_control/Android.mk \
    $(commands_recovery_local_path)/bootloader_pterodon/Android.mk \
    $(commands_recovery_local_path)/flashutils/Android.mk \
    $(commands_recovery_local_path)/minadbd/Android.mk \
    $(commands_recovery_local_path)/minui/Android.mk \
    $(commands_recovery_local_path)/mtdutils/Android.mk \
    $(commands_recovery_local_path)/mmcutils/Android.mk \
    $(commands_recovery_local_path)/libcrecovery/Android.mk \
    $(commands_recovery_local_path)/tests/Android.mk \
    $(commands_recovery_local_path)/tools/Android.mk \
    $(commands_recovery_local_path)/updater/Android.mk \
    $(commands_recovery_local_path)/update_verifier/Android.mk \
    $(commands_recovery_local_path)/gui/libaroma/Android.mk \
    $(commands_recovery_local_path)/gui/theme/Android.mk \
    $(commands_recovery_local_path)/gui/libaroma/libs/minzip/Android.mk \
    $(commands_recovery_local_path)/gui/libaroma/libs/harfbuzz-ng/Android.mk \
    $(commands_recovery_local_path)/gui/libaroma/libs/freetype/Android.mk
    
    