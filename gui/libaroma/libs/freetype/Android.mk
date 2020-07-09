# this is now the default FreeType build for Android
#

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

# compile in ARM mode, since the glyph loader/renderer is a hotspot
# when loading complex pages in the browser
#
LOCAL_ARM_MODE := arm

LOCAL_SRC_FILES:= \
    src/base/ftbbox.c \
    src/base/ftbitmap.c \
	src/base/ftfstype.c \
	src/base/ftglyph.c \
	src/base/ftlcdfil.c \
	src/base/ftstroke.c \
	src/base/fttype1.c \
	src/base/ftbase.c \
	src/base/ftsystem.c \
	src/base/ftinit.c \
	src/base/ftgasp.c \
	src/raster/raster.c \
	src/sfnt/sfnt.c \
	src/smooth/smooth.c \
	src/autofit/autofit.c \
	src/truetype/truetype.c \
	src/cff/cff.c \
	src/cid/type1cid.c \
	src/bdf/bdf.c \
	src/type1/type1.c \
	src/type42/type42.c \
	src/winfonts/winfnt.c \
	src/pcf/pcf.c \
	src/pfr/pfr.c \
	src/psaux/psaux.c \
	src/psnames/psnames.c \
	src/pshinter/pshinter.c \
	src/gzip/ftgzip.c \
	src/lzw/ftlzw.c

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/builds \
    $(LOCAL_PATH)/include \
	$(LOCAL_PATH)/../harfbuzz-ng/src \
	external/libpng \
	external/libpng/include \
	external/zlib

LOCAL_CFLAGS += -W -Wall
LOCAL_CFLAGS += -fPIC -DPIC
LOCAL_CFLAGS += "-DDARWIN_NO_CARBON"
LOCAL_CFLAGS += "-DFT2_BUILD_LIBRARY"

# the following is for testing only, and should not be used in final builds
# of the product
#LOCAL_CFLAGS += "-DTT_CONFIG_OPTION_BYTECODE_INTERPRETER"

LOCAL_CFLAGS += -O2

LOCAL_MODULE := libft2_libaroma

include $(BUILD_STATIC_LIBRARY)
