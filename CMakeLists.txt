cmake_minimum_required(VERSION 3.12)
project(tophat_recode_reorganize)

set(CMAKE_CXX_STANDARD 14)

set(SOURCES test.cpp src/dilate_erode_binary.cpp src/dilate_erode_gray_nonflat.cpp src/dilate_erode_packed.cpp src/morph.cpp src/neighborhood.cpp dsm_handle.cpp)

set(GDAL_DIR /Library/Frameworks/GDAL.framework/unix)

include_directories(include ${GDAL_DIR}/include)

link_directories(${GDAL_DIR}/lib)

add_executable(tophat_recode_reorganize ${SOURCES})
target_link_libraries(tophat_recode_reorganize ${GDAL_DIR}/lib/libgdal.dylib)
