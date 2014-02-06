#
#  (C) Copyright 2014 ZeXx86 (tomasj@spirit-system.com)
#  (C) Copyright 2013 ZeXx86 (tomasj@spirit-system.com)
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include \
		    $(LOCAL_PATH)/../SDL_image

# Add your application source files here...
LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.cpp \
	main.cpp gl.cpp event.cpp mouse.cpp kbd.cpp camera.cpp terrain.cpp logic.cpp octree.cpp tex.cpp frustum.cpp polygonise.cpp font.cpp water.cpp 

LOCAL_SHARED_LIBRARIES := SDL2_image SDL2

LOCAL_LDLIBS := -llog -lGLESv1_CM

include $(BUILD_SHARED_LIBRARY)
