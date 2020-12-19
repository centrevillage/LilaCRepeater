# Project Name
TARGET = dooper

# Sources
CPP_SOURCES = src/main.cpp
CPP_SOURCES += igb_sdk/igb_sdk/font/bitmap/cvfont_5x8.cpp
CPP_SOURCES += igb_sdk/igb_sdk/font/bitmap/cvfont_8x16.cpp
C_SOURCES = igb_sdk/igb_sdk/util/text.c

C_INCLUDES = -Isrc -Iigb_sdk -DUSE_DAISY -DUSE_FULL_LL_DRIVER

CPP_STANDARD = -std=c++17 -fconcepts

# Library Locations
LIBDAISY_DIR = libdaisy
DAISYSP_DIR = DaisySP

# Core location, and generic makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

