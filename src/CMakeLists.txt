# CMakeLists files in this project can
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

add_subdirectory(Common)

set(ENABLE_DIDEMO ON CACHE INTERNAL "")
set(ENABLE_SEEDARK ON CACHE INTERNAL "")
set(ENABLE_VIDEOSEARCH ON CACHE INTERNAL "")

add_subdirectory(UI)
set(COMMON_SRC_FILES ${COMMON_SRC_FILES} ${UI_SRC_FILES})
set(COMMON_INC_FILES ${COMMON_INC_FILES} ${UI_INC_FILES})

if(WIN32)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif(WIN32)

if(ENABLE_DIDEMO) 
	add_subdirectory(caffeinc)
	add_subdirectory(didemo)
endif (ENABLE_DIDEMO)

if(ENABLE_SEEDARK)
	add_subdirectory(seedark)
endif (ENABLE_SEEDARK)

if(ENABLE_VIDEOSEARCH)
	add_subdirectory(videosearch)
endif(ENABLE_VIDEOSEARCH)