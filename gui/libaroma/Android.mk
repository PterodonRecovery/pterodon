#####################################################################[libaroma]#
## Copyright (C) 2011-2015 Ahmad Amarullah (http://amarullz.com/)
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
##      http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.
##______________________________________________________________________________
##
## Filename    : Android.mk
## Description : Android make file
##
## + This is part of libaroma, an embedded ui toolkit.
## + 27/01/15 - Author(s): Ahmad Amarullah
##
#####################################################################[libaroma]#

##
## local path
##
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LIBAROMA_PATH := $(LOCAL_PATH)

##
## features configurations
##
LIBAROMA_MK_WITH_JPEG := false
LIBAROMA_MK_SHMEMFB := 1
LIBAROMA_ALLOW_HB := 1
LIBAROMA_MK_DEBUG_LEVEL := 0
LIBAROMA_MK_DEBUG_WITH_FILE := 0
LIBAROMA_MK_DEBUG_MEMORY_LEAK := 1
LIBAROMA_MK_DEBUG_COMPILER := 1

##
## libs path
##
LIBAROMA_MK_ZLIB_PATH := external/zlib
LIBAROMA_MK_PNG_PATH := external/libpng
LIBAROMA_MK_JPEG_PATH := external/jpeg
LIBAROMA_MK_FREETYPE_PATH := $(commands_recovery_local_path)/gui/libaroma/libs/freetype
LIBAROMA_MK_HARFBUZZNG_PATH := $(commands_recovery_local_path)/gui/libaroma/libs/harfbuzz-ng
LIBAROMA_MK_MINZIP_PATH := $(commands_recovery_local_path)/gui/libaroma/libs/minzip

##
## libs module name
##
LIBAROMA_MK_ZLIB_LIB := libz
LIBAROMA_MK_PNG_LIB := libpng
LIBAROMA_MK_JPEG_LIB := libjpeg_static
LIBAROMA_MK_FREETYPE_LIB := libft2_libaroma
LIBAROMA_MK_HARFBUZZNG_LIB := libharfbuzz_ng_libaroma
LIBAROMA_MK_MINZIP_LIB := libminzip_libaroma

################################################################################
## build libaroma shared library
##
  include $(CLEAR_VARS)
  LOCAL_MODULE := libaroma
  LOCAL_MODULE_TAGS := eng
  
  ##
  ## sources
  ##
  LOCAL_SRC_FILES := \
    src/aroma/aroma.c \
    src/aroma/version.c \
    src/aroma/utils/array.c \
    src/aroma/utils/json.c \
    src/aroma/utils/minutf8.c \
    src/aroma/utils/motions.c \
    src/aroma/utils/runtime.c \
    src/aroma/utils/stream.c \
    src/aroma/utils/strings.c \
    src/aroma/utils/system.c \
    src/aroma/utils/time.c \
    src/aroma/utils/zip.c \
    src/aroma/debug/compiler_message.c \
    src/aroma/debug/memory_tracking.c \
    src/aroma/graph/artworker.c \
    src/aroma/graph/canvas.c \
    src/aroma/graph/fb.c \
    src/aroma/graph/image.c \
    src/aroma/graph/jpeg.c \
    src/aroma/graph/png.c \
    src/aroma/graph/svg.c \
    src/aroma/graph/text.c \
    src/aroma/graph/draw/blur.c \
    src/aroma/graph/draw/commondraw.c \
    src/aroma/graph/draw/filters.c \
    src/aroma/graph/draw/gradient.c \
    src/aroma/graph/draw/path.c \
    src/aroma/graph/draw/scale.c \
    src/aroma/graph/engine/alignblt.c \
    src/aroma/graph/engine/alpha.c \
    src/aroma/graph/engine/blt.c \
    src/aroma/graph/engine/color.c \
    src/aroma/graph/engine/dither.c \
    src/aroma/hid/hid.c \
    src/aroma/hid/messages.c \
    src/aroma/ui/color_manager.c \
    src/aroma/ui/control.c \
    src/aroma/ui/dialog.c \
    src/aroma/ui/window.c \
    src/aroma/ui/window_layer.c \
    src/aroma/ui/window_manager.c \
    src/aroma/controls/ctl_bar.c \
    src/aroma/controls/ctl_button.c \
    src/aroma/controls/ctl_fragment.c \
    src/aroma/controls/ctl_image.c \
    src/aroma/controls/ctl_label.c \
    src/aroma/controls/ctl_list.c \
    src/aroma/controls/ctl_pager.c \
    src/aroma/controls/ctl_progress.c \
    src/aroma/controls/ctl_scroll.c \
    src/aroma/controls/ctl_tabs.c \
    src/aroma/controls/listitem/listitem_caption.c \
    src/aroma/controls/listitem/listitem_check.c \
    src/aroma/controls/listitem/listitem_divider.c \
    src/aroma/controls/listitem/listitem_image.c \
    src/aroma/controls/listitem/listitem_menu.c \
    src/aroma/controls/listitem/listitem_option.c \
    src/aroma/controls/listitem/listitem_text.c \
    src/contrib/platform/linux/fb_driver.c \
    src/contrib/platform/linux/hid_driver.c \
    src/contrib/platform/linux/platform.c
    
ifneq ($(TARGET_ARCH),)
ifeq ($(TARGET_ARCH),arm)
LOCAL_SRC_FILES += src/aroma/arch/arm/memset32.S
else ifeq ($(TARGET_ARCH),arm64)
LOCAL_SRC_FILES += src/aroma/arch/arm64/android_memset.S
else ifeq ($(TARGET_ARCH),mips)
LOCAL_SRC_FILES += src/aroma/arch/mips/android_memset.c
else ifeq ($(TARGET_ARCH),x86)
LOCAL_SRC_FILES += src/aroma/arch/x86/android_memset16.S
LOCAL_SRC_FILES += src/aroma/arch/x86/android_memset32.S
else ifeq ($(TARGET_ARCH),x86_64)
LOCAL_SRC_FILES += src/aroma/arch/x86_64/android_memset16.S
LOCAL_SRC_FILES += src/aroma/arch/x86_64/android_memset32.S
endif
endif
  
LOCAL_C_INCLUDES := \
    $(commands_recovery_local_path)/gui/libaroma/src \
    $(commands_recovery_local_path)/gui/libaroma/libs/jpeg \
    $(commands_recovery_local_path)/gui/libaroma/src/contrib/platform/linux/include \
    $(commands_recovery_local_path)/gui/libaroma/src/contrib/platform/linux \
    $(commands_recovery_local_path)/gui/libaroma/include \
    $(LIBAROMA_MK_ZLIB_PATH) \
    $(LIBAROMA_MK_PNG_PATH) \
    $(LIBAROMA_MK_FREETYPE_PATH)/include \
    $(LIBAROMA_MK_FREETYPE_PATH)/builds \
    $(LIBAROMA_MK_HARFBUZZNG_PATH)/src \
    $(LIBAROMA_MK_HARFBUZZNG_PATH)/src/hb-ucdn \
    $(LIBAROMA_MK_MINZIP_PATH)
  
  ##
  ## cflags
  ##
  LOCAL_CFLAGS := \
    -O3 \
    -ffunction-sections -Wl,--gc-sections \
    -fPIC -DPIC -Wl,-s -Wall -Wextra -Wshadow -Werror -Wno-unused-parameter
  
  ##
  ## arm neon
  ##
  ifeq ($(ARCH_ARM_HAVE_NEON),true)
    AROMA_ARM_NEON    := true

    LOCAL_CFLAGS += \
      -D__ARM_HAVE_NEON \
      -mfloat-abi=softfp \
      -mfpu=neon
  endif
  
  ##
  ## with jpeg support
  ##
  ifeq ($(LIBAROMA_MK_WITH_JPEG),true)
    LOCAL_C_INCLUDES += $(LIBAROMA_MK_JPEG_PATH)
  else
    LOCAL_CFLAGS += -DLIBAROMA_CONFIG_NOJPEG=1
  endif
  
  ##
  ## with shared memory framebuffer
  ##
  LOCAL_CFLAGS += \
    -DLIBAROMA_CONFIG_SHMEMFB=$(LIBAROMA_MK_SHMEMFB)
  
  ##
  ## debugging flags
  ##
  LOCAL_CFLAGS += \
    -DLIBAROMA_CONFIG_DEBUG=$(LIBAROMA_MK_DEBUG_LEVEL) \
    -DLIBAROMA_CONFIG_DEBUG_FILE=$(LIBAROMA_MK_DEBUG_WITH_FILE) \
    -DLIBAROMA_CONFIG_DEBUG_MEMORY=$(LIBAROMA_MK_DEBUG_MEMORY_LEAK) \
    -DLIBAROMA_CONFIG_COMPILER_MESSAGE=$(LIBAROMA_MK_DEBUG_COMPILER) \
    -DLIBAROMA_CONFIG_TEXT_NOHARFBUZZ=$(LIBAROMA_ALLOW_HB)
    
  ## build as static library
  include $(BUILD_STATIC_LIBRARY)

################################################################################

##
## include minzip, harfbuzz-ng, freetype
##

##[EOF]