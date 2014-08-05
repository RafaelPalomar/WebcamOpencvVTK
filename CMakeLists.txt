project(OpenCVWebcamVTK)

cmake_minimum_required(VERSION 2.6)

find_package(VTK REQUIRED)
find_package(OpenCV REQUIRED)

include(${VTK_USE_FILE})


include_directories(${VTK_INCLUDE_DIRS})

link_directories(${VTK_LIBRARY_DIRS})

add_executable(OpenCVWebcamVTK
  main.cpp
)

target_link_libraries(OpenCVWebcamVTK
  ${OpenCV_LIBS}
  ${VTK_LIBRARIES}
)