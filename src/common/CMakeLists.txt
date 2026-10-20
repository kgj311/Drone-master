cmake_minimum_required(VERSION 3.0.2)
project(common)

file(GLOB CPP_SOURCES ./src/*.cpp)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  geometry_msgs
  message_generation
  message_runtime
  mavros
  nlink_parser
)

set(Eigen3_DIR "/usr/include/eigen3")
find_package(Eigen3 REQUIRED)
include_directories(${Eigen3_INCLUDE_DIR})

## Generate messages in the 'msg' folder

catkin_package(CATKIN_DEPENDS roscpp rospy std_msgs message_runtime mavros nlink_parser)

include_directories(
  ${catkin_INCLUDE_DIRS}
  ${EIGEN3_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

## 创建可执行文件
#add_executable(uwb_position_pub ${dir_name}/src/uwb_position_pub.cpp)
add_executable(${PROJECT_NAME}uwb_position_pub src/uwb_position_pub.cpp)
## 链接库
#
#target_link_libraries(${PROJECT_NAME}uwb_position_pub 
  #${catkin_LIBRARIES}
#)
target_link_libraries(${PROJECT_NAME}uwb_position_pub ${catkin_LIBRARIES})