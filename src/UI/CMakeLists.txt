# CMakeLists files in this project can
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

## Find all source files to be compiled into object files
file(GLOB SRC_FILES *.cpp)
file(GLOB INC_FILES *.hpp *.h)

set(UI_SRC_FILES ${SRC_FILES} PARENT_SCOPE)
set(UI_INC_FILES ${INC_FILES} PARENT_SCOPE)


