# Lightcyber - 64k Intro by Team210 at Evoke 2k19
# Copyright (C) 2019 Alexander Kraus <nr4@z10.info>
# Copyright (C) 2019 DaDummy <c.anselm@paindevs.com>
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.12)

project(lightcyber)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Tell CMake to run qt tools when necessary (For Qt5 GUIs among other things)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find the right python interpreter version
find_package(Python3 REQUIRED COMPONENTS Interpreter)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(Qt5 COMPONENTS Widgets)

set(COMMON_HEADERS config_types.h config.h glext.h engine/platform.h engine/renderer.h engine/shader.h engine/loader.h engine/orchestrator.h)
set(LOAD_SHADER gfx/load.frag)
set(SFX_SHADER sfx.frag)
set(STRINGS_FILE font/strings.txt)
set(FRAGMENT_SHADER_FILES voronoidesign.frag groundboxes.frag graffiti.frag greet.frag evoke.frag canal.frag text.frag post.frag logo210.frag transbubbles.frag volclouds.frag chart.frag)

set(FRAGMENT_SHADERS "")
foreach(shader ${FRAGMENT_SHADER_FILES})
	list(APPEND FRAGMENT_SHADERS "gfx/${shader}")
endforeach(shader)


#################################
# Demo executable
#################################

set(DEMO_HEADERS common.h pal_linux.h pal_win32.h sequence.h gfx/logo210.h gfx/post.h gfx/text.h gfx/ui.h)

set(LOAD_HEADER gfx/load.h)
set(SFX_HEADER sfx.h)
add_custom_target(minification
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/minification/
	COMMAND "${Python3_EXECUTABLE}" shader-compressor.py -o "${PROJECT_SOURCE_DIR}/${SFX_HEADER}" "${PROJECT_SOURCE_DIR}/${SFX_SHADER}" --no-minification
	COMMAND "${Python3_EXECUTABLE}" shader-compressor.py -o "${PROJECT_SOURCE_DIR}/${LOAD_HEADER}" "${PROJECT_SOURCE_DIR}/${LOAD_SHADER}" --no-minification
	SOURCES minification/shader-compressor.py ${SFX_SHADER} ${LOAD_SHADER}
	BYPRODUCTS ${SFX_HEADER} ${LOAD_HEADER}
)

set(FONT_HEADER font/font.h)
add_custom_target(texture_packing
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/font/
    COMMAND "${Python3_EXECUTABLE}" pack.py
	SOURCES font/pack.py ${STRINGS_FILE}
	BYPRODUCTS ${FONT_HEADER}
)

set(SHADER_HEADER gfx/symbols.h)
add_custom_target(symbolization
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/gfx/
	COMMAND "${Python3_EXECUTABLE}" symbolize.py ${FRAGMENT_SHADER_FILES} -o symbols.h
	SOURCES gfx/symbolize.py minification/Rule.py minification/Token.py minification/GLSLLexer130.py minification/Compressor.py ${FRAGMENT_SHADERS}
	BYPRODUCTS ${SHADER_HEADER}
)

add_executable(lightcyber
	ls.c
	${COMMON_HEADERS}
	${DEMO_HEADERS}
	${LOAD_SHADER}
	${SFX_SHADER}
	${STRINGS_FILE}
	${FRAGMENT_SHADERS}
)
add_dependencies(lightcyber minification)
add_dependencies(lightcyber texture_packing)
add_dependencies(lightcyber symbolization)

target_include_directories(lightcyber SYSTEM PRIVATE "${OPENGL_INCLUDE_DIR}")

target_link_libraries(lightcyber OpenGL::GL)

if(MSVC)
	set_target_properties(lightcyber PROPERTIES LINK_FLAGS "/ENTRY:demo /SUBSYSTEM:Windows /OPT:REF /OPT:ICF=10 /VERBOSE /NODEFAULTLIB")
	target_compile_options(lightcyber PRIVATE /Gy /O1 /fp:fast /GR- /GS- /MT )

    if (MSVC_VERSION GREATER_EQUAL 1910) # VS2017
        target_link_libraries(lightcyber Winmm.lib User32.lib ucrt.lib msvcrt.lib vcruntime.lib ucrt.lib)
    else()
        # for debug output: User32.lib ucrt.lib msvcrt.lib
        target_link_libraries(lightcyber User32.lib Winmm.lib ucrt.lib msvcrt.lib)
    endif()

	if (CMAKE_SIZEOF_VOID_P EQUAL 32)
		find_program(KKRUNCHY_K7 kkrunchy_k7.exe HINTS "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}")
		add_custom_command(TARGET lightcyber COMMAND "${KKRUNCHY_K7}" --best "$<TARGET_FILE:lightcyber>" --out "$<TARGET_FILE_DIR:lightcyber>/lightcyberc.exe" POST_BUILD)
	endif()
else()
    find_package(SDL2 REQUIRED)
    target_include_directories(lightcyber SYSTEM PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(lightcyber ${SDL2_LIBRARIES})
endif()


#################################
# Studio executable
#################################

add_executable(studio
	studio/studio.cpp
	studio/mainwindow.cpp
	studio/mainwindow.ui
	studio/demoapi.cpp
	studio/renderviewwidget.cpp
	${COMMON_HEADERS}
)

target_link_libraries(studio Qt5::Widgets)
target_link_libraries(studio OpenGL::GL)
