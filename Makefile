include defaults.mk

TARGET_NAME 	:= HomeBrainVx01

#ROOT := src
SOURCE_SUBDIRS	:= main pal/common pal/os_wraper framework/interface framework/devctrl \
                   framework/staticpool framework/eventpool events
#COMPILER_PREFIX := arm-none-eabi-
LINK = $(CXX)

include rules.mk
