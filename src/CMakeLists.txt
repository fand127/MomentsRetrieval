# CMakeLists files in this project can
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

## Find all source files to be compiled into object files
file(GLOB SRC_FILES *.cpp)
file(GLOB INC_FILES *.hpp *.h)

add_definitions(-lstdc++ -ldl)
add_definitions(-DCPU_ONLY)

set(DEPENDENCY_LINKER_RELEASE_LIBS "")
set(DEPENDENCY_LINKER_DEBUG_LIBS "")
set(DEPENDENCY_INCLUDE_DIRS "")


set(CAFFE_LIB C:/Work/Project/caffe-windows-ms/Build/x64/Debug/libcaffe.lib)
set(CAFFE_INCLUDE_DIR C:/Work/Project/caffe-windows-ms/include)
include_directories (${CAFFE_INCLUDE_DIR})
include_directories (${CAFFE_INCLUDE_DIR}/caffe)

###HDF5 
set(HDF5L C:/Work/Project/caffe-windows-ms/windows/thirdparty/HDF5/lib)
#set(HDF5_INCLUDE_DIRS C:/Work/Project/caffe-windows-ms/windows/thirdparty/HDF5/include)
#list(APPEND DEPENDENCY_INCLUDE_DIRS PUBLIC ${HDF5_INCLUDE_DIRS})
#list(APPEND DEPENDENCY_LINKER_DEBUG_LIBS ${HDF5L}/hdf5_tools.lib)
#list(APPEND DEPENDENCY_LINKER_DEBUG_LIBS ${HDF5L}/hdf5_hl_fortran.lib )
#list(APPEND DEPENDENCY_LINKER_DEBUG_LIBS ${HDF5L}/hdf5_fortran.lib)
#list(APPEND DEPENDENCY_LINKER_DEBUG_LIBS ${HDF5L}/hdf5_hl_f90cstub.lib)
#list(APPEND DEPENDENCY_LINKER_DEBUG_LIBS ${HDF5L}/hdf5_f90cstub.lib)
#list(APPEND DEPENDENCY_LINKER_DEBUG_LIBS ${HDF5L}/hdf5_hl_cpp.lib)
#list(APPEND DEPENDENCY_LINKER_DEBUG_LIBS ${HDF5L}/hdf5_hl.lib)
#list(APPEND DEPENDENCY_LINKER_RELEASE_LIBS ${HDF5L}/hdf5.lib)


###OPENBLAS
set(BLAS_LIBRARIES_DIR C:/Work/Project/caffe-windows-ms/windows/thirdparty/OpenBLAS/lib)
set(BLAS_INCLUDE_DIRS C:/Work/Project/caffe-windows-ms/windows/thirdparty/OpenBLAS/include)
list(APPEND DEPENDENCY_INCLUDE_DIRS PUBLIC ${BLAS_INCLUDE_DIRS})
list(APPEND DEPENDENCY_LINKER_DEBUG_LIBS ${BLAS_LIBRARIES_DIR}/libopenblas.dll.a)
list(APPEND DEPENDENCY_LINKER_RELEASE_LIBS ${BLAS_LIBRARIES_DIR}/libopenblas.dll.a)


###OPENCV
set(CV_LIBRARIES_DIR C:/Work/Project/caffe-windows-ms/windows/thirdparty/OpenCV/x64/vc14/lib)
set(CV_INCLUDE_DIRS C:/Work/Project/caffe-windows-ms/windows/thirdparty/OpenCV/include)
list(APPEND DEPENDENCY_INCLUDE_DIRS PUBLIC ${CV_INCLUDE_DIRS})
list(APPEND DEPENDENCY_LINKER_DEBUG_LIBS ${CV_LIBRARIES_DIR}/opencv_world310d.lib)
list(APPEND DEPENDENCY_LINKER_RELEASE_LIBS ${CV_LIBRARIES_DIR}/opencv_world310.lib)

###PROTOBUF
set(PROTO_LIBRARIES_DIR C:/Work/Project/caffe-windows-ms/windows/thirdparty/Protobuf/lib)
set(PROTO_INCLUDE_DIRS C:/Work/Project/caffe-windows-ms/windows/thirdparty/Protobuf/include)
list(APPEND DEPENDENCY_INCLUDE_DIRS PUBLIC ${PROTO_INCLUDE_DIRS})
list(APPEND DEPENDENCY_LINKER_DEBUG_LIBS ${PROTO_LIBRARIES_DIR}/libprotobufd.lib)
list(APPEND DEPENDENCY_LINKER_RELEASE_LIBS ${PROTO_LIBRARIES_DIR}/libprotobuf.lib)


###LEVELDB
set(LEVELDB_LIBRARIES_DIR C:/Work/Project/caffe-windows-ms/windows/thirdparty/LEVELDB/lib)
set(LEVELDB_INCLUDE_DIRS C:/Work/Project/caffe-windows-ms/windows/thirdparty/LEVELDB/include)
list(APPEND DEPENDENCY_INCLUDE_DIRS PUBLIC ${LEVELDB_INCLUDE_DIRS})
list(APPEND DEPENDENCY_LINKER_DEBUG_LIBS ${LEVELDB_LIBRARIES_DIR}/leveldbd.lib)
list(APPEND DEPENDENCY_LINKER_RELEASE_LIBS ${LEVELDB_LIBRARIES_DIR}/leveldb.lib)

###LMDB
set(LMDB_LIBRARIES_DIR C:/Work/Project/caffe-windows-ms/windows/thirdparty/LMDB/lib)
set(LMDB_INCLUDE_DIRS C:/Work/Project/caffe-windows-ms/windows/thirdparty/LMDB/include)
list(APPEND DEPENDENCY_INCLUDE_DIRS PUBLIC ${LMDB_INCLUDE_DIRS})
list(APPEND DEPENDENCY_LINKER_DEBUG_LIBS ${LMDB_LIBRARIES_DIR}/lmdbd.lib)
list(APPEND DEPENDENCY_LINKER_RELEASE_LIBS ${LMDB_LIBRARIES_DIR}/lmdb.lib)

###GFLAG
set(GFLAG_LIBRARIES_DIR C:/Work/Project/caffe-windows-ms/windows/thirdparty/GFlags/Lib)
set(GFLAG_INCLUDE_DIRS C:/Work/Project/caffe-windows-ms/windows/thirdparty/GFlags/Include)
list(APPEND DEPENDENCY_INCLUDE_DIRS PUBLIC ${GFLAG_INCLUDE_DIRS})
list(APPEND DEPENDENCY_LINKER_DEBUG_LIBS ${GFLAG_LIBRARIES_DIR}/gflagsd.lib)
list(APPEND DEPENDENCY_LINKER_RELEASE_LIBS ${GFLAG_LIBRARIES_DIR}/gflags.lib)


###GLOG
set(GLOG_LIBRARIES_DIR C:/Work/Project/caffe-windows-ms/windows/thirdparty/GLog/lib)
set(GLOG_INCLUDE_DIRS C:/Work/Project/caffe-windows-ms/windows/thirdparty/GLog/include)
list(APPEND DEPENDENCY_INCLUDE_DIRS PUBLIC ${GLOG_INCLUDE_DIRS})
list(APPEND DEPENDENCY_LINKER_DEBUG_LIBS ${GLOG_LIBRARIES_DIR}/glogd.lib)
list(APPEND DEPENDENCY_LINKER_RELEASE_LIBS ${GLOG_LIBRARIES_DIR}/glog.lib)

###Boost
add_definitions(-DBOOST_REGEX_NO_LIB -DBOOST_DATE_TIME_NO_LIB)
IF(MSVC)
    ADD_DEFINITIONS("/EHsc")
ENDIF(MSVC)
set(Boost_LIBRARIES_DIR C:/Work/Project/caffe-windows-ms/windows/thirdparty/Boost/lib64-msvc-14.0)
set(Boost_INCLUDE_DIRS C:/Work/Project/caffe-windows-ms/windows/thirdparty/Boost)
list(APPEND DEPENDENCY_INCLUDE_DIRS PUBLIC ${Boost_INCLUDE_DIRS})
list(APPEND DEPENDENCY_LINKER_DEBUG_LIBS ${Boost_LIBRARIES_DIR}/libboost_date_time-vc140-mt-gd-1_64.lib)
list(APPEND DEPENDENCY_LINKER_DEBUG_LIBS ${Boost_LIBRARIES_DIR}/libboost_filesystem-vc140-mt-gd-1_64.lib)
list(APPEND DEPENDENCY_LINKER_DEBUG_LIBS ${Boost_LIBRARIES_DIR}/libboost_system-vc140-mt-gd-1_64.lib)
list(APPEND DEPENDENCY_LINKER_DEBUG_LIBS ${Boost_LIBRARIES_DIR}/libboost_thread-vc140-mt-gd-1_64.lib)
list(APPEND DEPENDENCY_LINKER_DEBUG_LIBS ${Boost_LIBRARIES_DIR}/libboost_date_time-vc140-mt-gd-1_64.lib)
list(APPEND DEPENDENCY_LINKER_DEBUG_LIBS ${Boost_LIBRARIES_DIR}/libboost_python-vc140-mt-gd-1_64.lib)
list(APPEND DEPENDENCY_LINKER_DEBUG_LIBS ${Boost_LIBRARIES_DIR}/libboost_chrono-vc140-mt-gd-1_64.lib)
list(APPEND DEPENDENCY_LINKER_DEBUG_LIBS ${Boost_LIBRARIES_DIR}/libboost_python3-vc140-mt-gd-1_64.lib)

list(APPEND DEPENDENCY_LINKER_RELEASE_LIBS ${Boost_LIBRARIES_DIR}/libboost_date_time-vc140-mt-1_64.lib)



###HDF5
if(WIN32)
	FIND_PACKAGE(HDF5 COMPONENTS C CXX HL REQUIRED)
	set(HDF5_INCLUDE_DIRS ${HDF5_DIR}/../../include)	
	set(HDF5_LIB_DIR ${HDF5_DIR}/../../lib)
else()
	find_package(HDF5 COMPONENTS C CXX HL REQUIRED)
endif(WIN32)

#include_directories (${HDF5_INCLUDE_DIRS})
list(APPEND DEPENDENCY_INCLUDE_DIRS PUBLIC ${HDF5_INCLUDE_DIRS})

include_directories (${DEPENDENCY_INCLUDE_DIRS})

############################################
set(RAPIDJSON_INC ${PROJECT_SOURCE_DIR}/thirdparty/rapidjson)
include_directories (${RAPIDJSON_INC})
set(SOURCE ${SRC_FILES} ${INC_FILES})

##Excutable
add_executable(lib_didemo ${SOURCE})

if(WIN32)
	target_link_libraries(lib_didemo debug ${HDF5_LIB_DIR}/libszip_D.lib ${HDF5_LIB_DIR}/libzlib_D.lib ${HDF5_LIB_DIR}/libhdf5_D.lib ${HDF5_LIB_DIR}/libhdf5_cpp_D.lib ${HDF5_LIB_DIR}/libhdf5_hl_D.lib ${HDF5_LIB_DIR}/libhdf5_hl_cpp_D.lib ${CAFFE_LIB} ${DEPENDENCY_LINKER_DEBUG_LIBS})	
	target_link_libraries(lib_didemo optimized ${HDF5_LIB_DIR}/libszip_D.lib ${HDF5_LIB_DIR}/libzlib_D.lib ${HDF5_LIB_DIR}/libhdf5_D.lib ${HDF5_LIB_DIR}/libhdf5_cpp_D.lib ${CAFFE_LIB} ${DEPENDENCY_LINKER_RELEASE_LIBS})
	#target_link_libraries(lib_didemo ${HDF5_LIB_DIR}/libszip.lib ${HDF5_LIB_DIR}/libzlib.lib ${HDF5_LIB_DIR}/libhdf5.lib ${HDF5_LIB_DIR}/libhdf5_cpp.lib)
else(WIN32)
	target_link_libraries(lib_didemo ${HDF5_LIBRARIES} ${HDF5_HL_LIBRARIES} ${HDF5_CXX_LIBRARIES})

endif(WIN32)

