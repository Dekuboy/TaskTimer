cmake_minimum_required(VERSION 2.6)
set(CMAKE_GENERATOR_PLATFORM Win32)
project(WORKTIMER)

include_directories(
	src
	include
)

link_directories(
	C:\Users\gjene\Desktop\Projects\Timer_To_Text_File\lib
)

add_library(worktimer
	src/wtimer.h
	src/wtimer.cpp
)

target_link_libraries(worktimer SDL2 SDL2_ttf)

if(MSVC)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
else()

endif()

add_executable(timer WIN32
	src/main.cpp
)

target_link_libraries(timer
	worktimer
)