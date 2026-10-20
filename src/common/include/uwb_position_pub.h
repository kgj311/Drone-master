#ifndef UWB_POSITION_PUB_H
#define UWB_POSITION_PUB_H

#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/State.h>
#include "nlink_parser/LinktrackAnchorframe0.h"
#include "nlink_parser/LinktrackNodeframe2.h"
#include <eigen3/Eigen/Dense>
#include "state_from_mavlink.h"
#include "common_msg/DroneState.h"
#include <mutex> // 用于线程安全

class uwb_position_pub
{
public:
    // 构造函数，初始化成员变量
    uwb_position_pub() : uwb_position_pub_node("~")
    {
        // 初始化 pose_t_drone 的位置和姿态
        pose_t_drone.pose.position.x = 0;
        pose_t_drone.pose.position.y = 0;
        pose_t_drone.pose.position.z = 0;

        pose_t_drone.pose.orientation.x = 0;
        pose_t_drone.pose.orientation.y = 0;
        pose_t_drone.pose.orientation.z = 0;
        pose_t_drone.pose.orientation.w = 1;

        // 订阅 UWB 数据
        sub_from_uwb = uwb_position_pub_node.subscribe<nlink_parser::LinktrackNodeframe2>("/nlink_linktrack_nodeframe2", 10, &uwb_position_pub::position_from_uwb_cb, this);
        // 发布位置信息到 MAVLink
        pub_to_mavlink = uwb_position_pub_node.advertise<geometry_msgs::PoseStamped>("/mavros/vision_pose/pose", 10);
        // 订阅光流传感器数据
        opticalFlowRad_sub = uwb_position_pub_node.subscribe<mavros_msgs::OpticalFlowRad>("/mavros/px4flow/raw/optical_flow_rad", 10, &uwb_position_pub::opticalFlowRad_cb, this);
    }

    geometry_msgs::PoseStamped pose_t_drone;
    float opticalFlowRad_distance;

    // 发布位置信息到无人机
    void pub_position_to_drone();

private:
    ros::NodeHandle uwb_position_pub_node;
    ros::Publisher pub_to_mavlink;
    ros::Subscriber sub_from_uwb;
    ros::Subscriber opticalFlowRad_sub;
    std::mutex mtx; // 互斥锁，用于线程安全

    // 光流传感器数据回调函数
    void opticalFlowRad_cb(const mavros_msgs::OpticalFlowRad::ConstPtr &msg)
    {
        std::lock_guard<std::mutex> lock(mtx); // 加锁
        opticalFlowRad_distance = msg->distance;
        pose_t_drone.pose.position.z = opticalFlowRad_distance;
    }

    // UWB 数据回调函数
    void position_from_uwb_cb(const nlink_parser::LinktrackNodeframe2::ConstPtr &msg)
    {
        std::lock_guard<std::mutex> lock(mtx); // 加锁
        pose_t_drone.header.stamp = ros::Time::now();
        pose_t_drone.header.frame_id = "map";

        // 这里暂时使用固定值，实际应用中可根据需求修改
        pose_t_drone.pose.position.x = 10;
        pose_t_drone.pose.position.y = 30;

        pose_t_drone.pose.orientation.x = msg->quaternion[0];
        pose_t_drone.pose.orientation.y = msg->quaternion[1];
        pose_t_drone.pose.orientation.z = msg->quaternion[2];
        pose_t_drone.pose.orientation.w = msg->quaternion[3];
    }
};

// 实现发布位置信息到无人机的函数
void uwb_position_pub::pub_position_to_drone()
{
    std::lock_guard<std::mutex> lock(mtx); // 加锁
    std::cout << "x_expect: " << pose_t_drone.pose.position.x << "  "
              << "y_expect: " << pose_t_drone.pose.position.y << "  "
              << "z_expect: " << pose_t_drone.pose.position.z << std::endl;
    pub_to_mavlink.publish(pose_t_drone);
}

#endif

