# Copyright (C) 2016 Nigel Williams
#
# Vulkan Free Surface Modeling Engine (VFSME) is free software:
# you can redistribute it and/or modify it under the terms of the
# GNU Lesser General Public License as published by
# the Free Software Foundation, version 3.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

VULKAN_PATH = /c/Dev/VulkanSDK/1.0.30.0
GLFW_PATH = /c/Dev/glfw/glfw-3.2.1.bin.WIN32
GLM_PATH = /C/Dev/glm

CFLAGS = -std=c++14 -Wall -g
INCLUDE = -I$(VULKAN_PATH)/include -I$(GLFW_PATH)/include -I$(GLM_PATH)
LDFLAGS = -L$(VULKAN_PATH)/Bin32 -L$(GLFW_PATH)/lib-mingw
LDLIBS = -lvulkan-1 -lglfw3 -lgdi32
DEFINES = -DVK_USE_PLATFORM_WIN32_KHR
OBJS = commands.o renderer.o system.o controller.o compositor.o compute.o

.PHONY: clean shaders test 

vulkan: main.cpp $(OBJS) shaders
	g++ $(CFLAGS) $(DEFINES) $(INCLUDE) $(LDFLAGS) -o vulkan main.cpp $(OBJS) $(LDLIBS)

system.o: system.h system.cpp
	g++ $(CFLAGS) $(DEFINES) $(INCLUDE) -c system.cpp -o $@	

commands.o: commands.h commands.cpp shared.h
	g++ $(CFLAGS) $(DEFINES) $(INCLUDE) -c commands.cpp -o $@
	
renderer.o: renderer.h renderer.cpp commands.h
	g++ $(CFLAGS) $(DEFINES) $(INCLUDE) -c renderer.cpp -o $@
	
compute.o: compute.h compute.cpp commands.h
	g++ $(CFLAGS) $(DEFINES) $(INCLUDE) -c compute.cpp -o $@

controller.o: controller.h controller.cpp shared.h
	g++ $(CFLAGS) $(DEFINES) $(INCLUDE) -c controller.cpp -o $@
	
compositor.o: compositor.h compositor.cpp renderer.h compute.h
	g++ $(CFLAGS) $(DEFINES) $(INCLUDE) -c compositor.cpp -o $@
	
test: vulkan
	LD_LIBRARY_PATH=$(VULKAN_PATH)/lib VK_LAYER_PATH=$(VULKAN_PATH)/etc/explicit_layer.d ./vulkan

shaders:
	$(VULKAN_PATH)/Bin32/glslangValidator.exe -V shader.vert
	$(VULKAN_PATH)/Bin32/glslangValidator.exe -V shader.frag
	$(VULKAN_PATH)/Bin32/glslangValidator.exe -V shader.comp

clean:
	rm *.exe *.o *.spv