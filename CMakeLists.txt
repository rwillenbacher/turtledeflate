cmake_minimum_required(VERSION 3.19)
project( TURTLEDEFLATE )

set( TURTLEDEFLATE_ENABLE_GPROF "false" )

if( ${TURTLEDEFLATE_ENABLE_GPROF} STREQUAL true )
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
	SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")
endif()


set ( _lib_sources
	inc/turtledeflate_api.h
	lib/turtledeflate.c
	lib/turtledeflate.h
	lib/turtledeflate_tree.c
	lib/turtledeflate_tree.h
	lib/turtledeflate_block.c
	lib/turtledeflate_block.h
	lib/turtledeflate_bitstream.c
	lib/turtledeflate_bitstream.h
)

set ( _app_sources
	src/turtledeflate_app.c
)

# the lib
add_library(libturtledeflate STATIC ${_lib_sources} )
target_include_directories(libturtledeflate PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc" )

# the app
add_executable(turtledeflate_app ${_app_sources} )


if(WIN32)
elseif(APPLE)
	target_link_libraries(yevce_app PUBLIC m )
else()
	target_link_libraries(yevce_app PUBLIC m )
endif()

target_link_libraries(turtledeflate_app libturtledeflate )

