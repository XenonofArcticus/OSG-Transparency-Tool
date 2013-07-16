PROJECT(osgTT)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

IF(NOT WIN32)
	SET(CMAKE_C_FLAGS "-W -Wall -Wno-unused -fPIC")
	SET(CMAKE_CXX_FLAGS "-W -Wall -Wno-unused -fPIC")
ENDIF()

SET(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "Add a postfix, usually d on windows")
SET(OPENSCENEGRAPH_INCLUDE_DIR "" CACHE PATH "Location of the OSG include headers.")
SET(OPENSCENEGRAPH_LIB_DIR "" CACHE PATH "Location of the the OSG library objects.")

SET(SRC_FILES
	src/DepthPeeling.cpp
	src/EBCNode.cpp
	src/EBCReader.cpp
	src/TransparencyGroup.cpp
)

SET(HEADER_FILES
	src/DepthPeeling.h
	src/EBCNode.h
	src/EBCReader.h
	src/TransparencyGroup.h
)

SOURCE_GROUP("Header Files" FILES ${HEADER_FILES})
SOURCE_GROUP("Source Files" FILES ${SRC_FILES})

INCLUDE_DIRECTORIES(
	${OPENSCENEGRAPH_INCLUDE_DIR}
	src
)

LINK_DIRECTORIES(${OPENSCENEGRAPH_LIB_DIR})

ADD_LIBRARY(osgtt STATIC ${HEADER_FILES} ${SRC_FILES})

TARGET_LINK_LIBRARIES(osgtt
	debug     OpenThreads${CMAKE_DEBUG_POSTFIX}
	optimized OpenThreads
	debug     osg${CMAKE_DEBUG_POSTFIX}
	optimized osg
	debug     osgGA${CMAKE_DEBUG_POSTFIX}
	optimized osgGA
	debug     osgDB${CMAKE_DEBUG_POSTFIX}
	optimized osgDB
	debug     osgUtil${CMAKE_DEBUG_POSTFIX}
	optimized osgUtil
	debug     osgViewer${CMAKE_DEBUG_POSTFIX}
	optimized osgViewer
)

ADD_EXECUTABLE(osgtt-demo src/osgtt-demo.cpp)
ADD_EXECUTABLE(osgtt-shape src/osgtt-shape.cpp)

TARGET_LINK_LIBRARIES(osgtt-demo osgtt)
TARGET_LINK_LIBRARIES(osgtt-shape osgtt)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/demo)

