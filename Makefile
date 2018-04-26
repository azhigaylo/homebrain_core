include defaults.mk

TARGET_NAME 	:= home-brain-vx01

#ROOT := src
SOURCE_SUBDIRS	:= main pal/common pal/os_wraper framework/interface framework/devctrl \
                   framework/staticpool framework/eventpool events framework/parser startup \
                   framework/debug project/provider driver/serial project/interfaces project/uso \
                   project/processor \

LINK = $(CXX)

include rules.mk
