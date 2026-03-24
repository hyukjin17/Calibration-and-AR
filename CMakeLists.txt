cmake_minimum_required(VERSION 3.16)
project(Calibration-and-AR LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenCV REQUIRED)

add_executable(ar src/TargetDetector.cpp src/Calibrator.cpp src/Renderer.cpp src/AppController.cpp src/main.cpp)
target_include_directories(ar PRIVATE ${OpenCV_INCLUDE_DIRS} include)
target_link_libraries(ar PRIVATE ${OpenCV_LIBS})

add_executable(board src/generate_board.cpp src/TargetDetector.cpp)
target_include_directories(board PRIVATE ${OpenCV_INCLUDE_DIRS} include)
target_link_libraries(board PRIVATE ${OpenCV_LIBS})

add_executable(test src/feature_test.cpp)
target_include_directories(test PRIVATE ${OpenCV_INCLUDE_DIRS} include)
target_link_libraries(test PRIVATE ${OpenCV_LIBS})