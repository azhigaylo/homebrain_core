include defaults.mk

TARGET_NAME 	:= HomeBrainVx01

#ROOT := src
SOURCE_SUBDIRS	:= main pal/common pal/os_wraper framework/interface framework/devctrl \
                   framework/staticpool framework/eventpool events framework/parser startup \
                   framework/debug project/provider driver/serial project/interfaces project/uso 
                   
#INC_SUBDIRS     :=  /home/azhigaylo/Qt5.4.0/5.4/gcc/include /home/azhigaylo/Qt5.4.0/5.4/gcc/include/QtNetwork /home/azhigaylo/Qt5.4.0/5.4/gcc/include/QtXml /home/azhigaylo/Qt5.4.0/5.4/gcc/include/QtCore
#COMPILER_PREFIX := arm-none-eabi-
LINK = $(CXX)

include rules.mk
