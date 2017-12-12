cmake_minimum_required(VERSION 3.6)

PROJECT("Mesh Maker" LANGUAGES CXX)

if(WIN32)	
	set(BOOST_ROOT "D:/boost_1_61_0")
	find_package(Boost REQUIRED COMPONENTS)
endif(WIN32)

if (UNIX)
	#set(VTK_DIR "/home/uqocairn/usr/local/lib/cmake/vtk-7.1" CACHE PATH "VTK directory override" FORCE)
	set(VTK_DIR "/home/uqocairn/usr/local-old-vtk/lib/cmake/vtk-7.1" CACHE PATH "VTK directory override" FORCE)
	find_package(Boost REQUIRED COMPONENTS thread filesystem system date_time chrono regex timer program_options)
endif(UNIX)


find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "include/*.h")

#ncurses libs
#if (WIN32)
#{
#	file(GLOB CURSES_LIBRARIES "")
#}

#fmt includes and libs
if(WIN32)
	file(GLOB FMT_LIBS "c:/Program\ Files/FMT/lib/*.lib")
	set(FMT_INC_DIR "c:/Program\ Files/FMT/include")
endif(WIN32)

if (UNIX)
	file(GLOB FMT_LIBS "/home/uqocairn/usr/local/lib64/*.a")
	set(FMT_INC_DIR "/home/uqocairn/usr/local/include/")
endif(UNIX)

#Postgres includes and libs
if(UNIX)
	file(GLOB POST_LIBS "/home/uqocairn/usr/local/pgsql/lib/*.a")
	set(INC_DIR "/home/uqocairn/usr/local/pgsql/include")
endif(UNIX)

if(WIN32)
	file(GLOB POST_LIBS "C:/Program\ Files/PostgreSQL/9.6/lib/*.lib")
	set(INC_DIR "C:/Program\ Files/PostgreSQL/9.6/include")
endif(WIN32)

include_directories(include ${INC_DIR})
include_directories(${Boost_INCLUDE_DIRS}) 
include_directories(${FMT_INC_DIR})
add_executable(MeshMaker ${SOURCES})

#BOOST windows libs
if (WIN32)

	target_compile_definitions(MeshMaker PRIVATE WINDOWS=1)
	
	target_link_libraries(MeshMaker "D:/boost_1_61_0/stage/lib/libboost_filesystem-vc140-mt-1_61.lib")
	target_link_libraries(MeshMaker "D:/boost_1_61_0/stage/lib/libboost_system-vc140-mt-1_61.lib")
	target_link_libraries(MeshMaker "D:/boost_1_61_0/stage/lib/libboost_thread-vc140-mt-1_61.lib")
	target_link_libraries(MeshMaker "D:/boost_1_61_0/stage/lib/libboost_date_time-vc140-mt-1_61.lib")
	target_link_libraries(MeshMaker "D:/boost_1_61_0/stage/lib/libboost_chrono-vc140-mt-1_61.lib")
	target_link_libraries(MeshMaker "D:/boost_1_61_0/stage/lib/libboost_regex-vc140-mt-1_61.lib")
	target_link_libraries(MeshMaker "D:/boost_1_61_0/stage/lib/libboost_timer-vc140-mt-1_61.lib")
	target_link_libraries(MeshMaker "D:/boost_1_61_0/stage/lib/libboost_program_options-vc140-mt-1_61.lib")	
		
	#[[
	target_link_libraries(MeshMaker "D:/boost_1_61_0/stage/lib/libboost_filesystem-vc140-mt-gd-1_61.lib")
	target_link_libraries(MeshMaker "D:/boost_1_61_0/stage/lib/libboost_system-vc140-mt-gd-1_61.lib")
	target_link_libraries(MeshMaker "D:/boost_1_61_0/stage/lib/libboost_thread-vc140-mt-gd-1_61.lib")
	target_link_libraries(MeshMaker "D:/boost_1_61_0/stage/lib/libboost_date_time-vc140-mt-gd-1_61.lib")
	target_link_libraries(MeshMaker "D:/boost_1_61_0/stage/lib/libboost_chrono-vc140-mt-gd-1_61.lib")
	target_link_libraries(MeshMaker "D:/boost_1_61_0/stage/lib/libboost_regex-vc140-mt-gd-1_61.lib")
	target_link_libraries(MeshMaker "D:/boost_1_61_0/stage/lib/libboost_timer-vc140-mt-gd-1_61.lib")
	target_link_libraries(MeshMaker "D:/boost_1_61_0/stage/lib/libboost_program_options-vc140-mt-gd-1_61.lib")	
	]]
	
endif(WIN32)
if(UNIX)
	target_link_libraries(MeshMaker ${Boost_LIBRARIES})
	file(GLOB VTK_LIBRARIES "/home/uqocairn/usr/local-old-vtk/lib/*.so")
	set (CMAKE_CXX_FLAGS "-Wno-deprecated-declarations")
endif(UNIX)

target_link_libraries(MeshMaker ${VTK_LIBRARIES}) 
target_link_libraries(MeshMaker ${POST_LIBS})
target_link_libraries(MeshMaker ${FMT_LIBS})

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
