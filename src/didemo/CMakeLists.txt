# CMakeLists files in this project can
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

## Find all source files to be compiled into object files
file(GLOB SRC_FILES *.cpp)
file(GLOB INC_FILES *.hpp *.h)

set(DEPENDENCY_LINKER_RELEASE_LIBS "")
set(DEPENDENCY_LINKER_DEBUG_LIBS "")
set(DEPENDENCY_INCLUDE_DIRS "")

if(WIN32)
	include("windows.cmake")
else()
	find_package(HDF5 COMPONENTS C CXX HL REQUIRED)
	list(APPEND DEPENDENCY_INCLUDE_DIRS ${HDF5_INCLUDE_DIRS})
	list(APPEND DEPENDENCY_LINKER_DEBUG_LIBS ${HDF5_LIBRARIES} ${HDF5_HL_LIBRARIES} ${HDF5_CXX_LIBRARIES})


	find_package(Caffe)
	include_directories (${Caffe_DIR}/include)
	include_directories(${Caffe_INCLUDE_DIRS})
	add_definitions(${Caffe_DEFINITIONS})
endif(WIN32)
include_directories (${DEPENDENCY_INCLUDE_DIRS})

############################################
set(RAPIDJSON_INC ${PROJECT_SOURCE_DIR}/thirdparty/rapidjson)
include_directories (${RAPIDJSON_INC})
set(SOURCE ${SRC_FILES} ${INC_FILES})

##Excutable
add_executable(didemo_test ${COMMON_INC_FILES} ${COMMON_SRC_FILES} ${SOURCE} )

target_link_libraries(didemo_test debug ${Caffe_LIBRARIES} ${DEPENDENCY_LINKER_DEBUG_LIBS})	
target_link_libraries(didemo_test optimized ${Caffe_LIBRARIES} ${DEPENDENCY_LINKER_RELEASE_LIBS})


