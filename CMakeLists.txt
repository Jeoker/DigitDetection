project(digit_recognition)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package( OpenCV REQUIRED  )
if( OpenCV_FOUND  )
    list( APPEND ThirdParty_LIBS ${OpenCV_LIBS}  )
    include_directories( ${OpenCV_INCLUDE_DIRS}  )
endif( OpenCV_FOUND  )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


add_executable(${PROJECT_NAME}_trainer train_main.cpp trainer.cpp)
target_link_libraries(${PROJECT_NAME}_trainer ${OpenCV_LIBS})

add_executable(${PROJECT_NAME}_rotate_sample rotate_main.cpp)
target_link_libraries(${PROJECT_NAME}_rotate_sample ${OpenCV_LIBS})

add_executable(${PROJECT_NAME}_recognition find_main.cpp digit_finder.cpp)
target_link_libraries(${PROJECT_NAME}_recognition ${OpenCV_LIBS})

