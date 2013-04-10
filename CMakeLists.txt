PROJECT(TransDemo)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

IF(NOT WIN32)
	SET(CMAKE_C_FLAGS "-W -Wall -Wno-unused -fPIC")
	SET(CMAKE_CXX_FLAGS "-W -Wall -Wno-unused -fPIC")
ELSE(NOT WIN32)
	SET(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "Add a postfix, usually d on windows")
ENDIF(NOT WIN32)

SET(OPENSCENEGRAPH_INCLUDE_DIR "" CACHE PATH "Location of the OSG include headers.")
SET(OPENSCENEGRAPH_LIB_DIR "" CACHE PATH "Location of the the OSG library objects.")

SET(SRC_FILES
	"src/EBCNode.cpp"
	"src/EBCReader.cpp"
	"src/TransDemo.cpp"
)

SET(HEADER_FILES
	"src/EBCNode.h"
	"src/EBCReader.h"
)

SOURCE_GROUP("Header Files" FILES ${HEADER_FILES})
SOURCE_GROUP("Source Files" FILES ${SRC_FILES})

INCLUDE_DIRECTORIES("${OPENSCENEGRAPH_INCLUDE_DIR}")
LINK_DIRECTORIES(${OPENSCENEGRAPH_LIB_DIR})

SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/examples")

ADD_EXECUTABLE(TransDemo ${HEADER_FILES} ${SRC_FILES})

TARGET_LINK_LIBRARIES(TransDemo
	debug     OpenThreads${CMAKE_DEBUG_POSTFIX}
	optimized OpenThreads
	debug     osg${CMAKE_DEBUG_POSTFIX}
	optimized osg
	debug     osgGA${CMAKE_DEBUG_POSTFIX}
	optimized osgGA
	debug     osgViewer${CMAKE_DEBUG_POSTFIX}
	optimized osgViewer
)

