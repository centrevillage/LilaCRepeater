# Project Name
TARGET = dooper

# Sources
SRC_DIR=./src/
CPP_SOURCES = $(wildcard $(SRC_DIR)*.cpp) $(wildcard $(SRC_DIR)image/*.cpp)
CPP_SOURCES += igb_sdk/igb_sdk/font/bitmap/cvfont_5x8.cpp
CPP_SOURCES += igb_sdk/igb_sdk/font/bitmap/cvfont_8x16.cpp
C_SOURCES = $(wildcard $(SRC_DIR)*.c)
C_SOURCES += igb_sdk/igb_sdk/util/text.c

#C_INCLUDES = -Isrc -Iigb_sdk -DUSE_DAISY -DUSE_FULL_LL_DRIVER -DTESTMODE
C_INCLUDES = -Isrc -Iigb_sdk -DUSE_DAISY -DUSE_FULL_LL_DRIVER

CPP_STANDARD = -std=c++17 -fconcepts

# Library Locations
LIBDAISY_DIR = libdaisy
DAISYSP_DIR = DaisySP

# FatFs
FATFS_DIR = $(LIBDAISY_DIR)/Middlewares/Third_Party/FatFs/src
C_SOURCES += \
	$(FATFS_DIR)/diskio.c \
	$(FATFS_DIR)/ff.c \
	$(FATFS_DIR)/ff_gen_drv.c \
	$(FATFS_DIR)/option/ccsbcs.c \

# FATFS dir contains fatfs itself, libdaisy sys folder contains ffconf 
C_INCLUDES += -I$(FATFS_DIR) -I$(LIBDAISY_DIR)/src/sys


# Core location, and generic makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

