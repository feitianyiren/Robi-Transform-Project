/*  balance control of "wind fire wheel"
    ----
    Licensed under BSD license.
    ----
    0.1 - 2017.6.22  init version -- Nick Qian
    ----
    output: PWM (<pwm_gen> in pwm.cpp)
    input:  (1) info published from sensors topic
*/


#ifndef _WFW_H_
#define _WFW_H_

#include <stdint.h>            //<cstdint>

#include <ros/ros.h>
#include <actionlib/server/simple_action_server.h>
#include <actprocess/wfwAction.h>

#include <sensor_msgs/Imu.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/Point.h>
#include <geometry_msgs/Quaternion.h>

#include "bc_common.h"
#include "pid.h"

using geometry_msgs::Point;
using geometry_msgs::Quaternion;

/*************************************************
/* main ref: github.com/TKJElectronics/Balanduino
/*************************************************/

typedef enum{
    motor_footL,
    motor_footR
}WfwMotor;



struct wfwGoal{
        std::string wfwCmd;
        geometry_msgs::Pose goal_pose;
        bool keep_balance;
};



/*************************************************************************
/*  wfw action
/*
/*************************************************************************/

class wfwAction{

 public:

    wfwAction(std::string name);

    ~wfwAction(void);


    /* Use executeCB callback method */
    //void executeCB(const actprocess::wfwGoalConstPtr &goal);


    /* Use goal callback method  */
    void goalCB(void);
    void preemptCB(void);
    void analysisCB(const sensor_msgs::ImuConstPtr& msg);

    void executeCB(const actprocess::wfwGoalConstPtr &goal);

    uint32_t wfw_timer;

    typedef enum{
        forward,
        backward,
        turnLeft,
        turnRight,
        stop
    }WfwCommand;




 protected:

    ros::NodeHandle nh_;
    std::string action_name_;
    actionlib::SimpleActionServer<actprocess::wfwAction> as_;
    actprocess::wfwFeedback feedback_;
    actprocess::wfwResult   result_;

    wfwGoal  goal_;

   /************ Sensor data subscriber and PID controller *************/
    ros::Subscriber sub_;               // = nh_.subscribe("tpc_sens_attt", 100, get_imu_data);
    sensor_msgs::ImuPtr imu_;

    //kfilter kfilter_;                 // move kalman filter to attt.cpp

    Pid pid_;

    double Imu_dataAnalyze(const sensor_msgs::ImuConstPtr& msg);

    double lastImuTimStamp_;
    int  countImu_;

    double pitch_ ;
    Point      position_;             //   = {0.1111, 0.1111, 0.1111};
    Quaternion quaternion_;           //   = {0.1111, 0.1111, 0.1111, 0.1111}
    double turningValue_;

    bool layingDown_;                 // = true;
    bool steerStop_;                  // = true;
    bool stopped_;                    // used to set a new tartget position


    void moveMotors(double PIDValue, double turning);
    void moveMotor(WfwMotor motor, WfwCommand direct, double PIDres);
    void stopMotor(WfwMotor motor);
    void stbMotor(WfwMotor motor);

    int32_t pwmCalculate(double PIDsum, uint32_t max_width);
};


#endif
