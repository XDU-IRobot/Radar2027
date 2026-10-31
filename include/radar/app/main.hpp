#ifndef MAIN_HPP
#define MAIN_HPP

#include <librm.hpp>

#include "buzzer.hpp"
#include "aimbot_comm_can.hpp"
#include "RefereeUart.hpp"
#include "subReferee/referee_user.hpp"
#include "USB.hpp"

// 状态机
typedef enum {
  kNoForce, // 无力模式
  kInit, // 初始化状态
  kTest, // 调试模式

  kGbRemote, // 云台遥控模式
  kGbAimbot, // 云台自瞄模式
  kGbTest, // 云台调试模式
} StateMachineType;

inline struct GlobalWarehouse {
public:
  Buzzer *buzzer{nullptr}; ///< 蜂鸣器
  rm::modules::BuzzerController<
    rm::modules::buzzer_melody::Silent, rm::modules::buzzer_melody::Startup, rm::modules::buzzer_melody::Success,
    rm::modules::buzzer_melody::Error, rm::modules::buzzer_melody::SuperMario, rm::modules::buzzer_melody::SeeUAgain
    ,
    rm::modules::buzzer_melody::TheLick, rm::modules::buzzer_melody::Beeps<1>, rm::modules::buzzer_melody::Beeps<2>,
    rm::modules::buzzer_melody::Beeps<3>, rm::modules::buzzer_melody::Beeps<4>, rm::modules::buzzer_melody::Beeps<
      5>>
  buzzer_controller;

  // 硬件接口 //
  rm::hal::Can *can1{nullptr}; ///< CAN 总线接口
  rm::hal::Serial *dbus{nullptr}; ///< 遥控器串口接口
  rm::hal::Serial *refereeUart{nullptr};
  rm::device::RefereeUART *refereeUART{nullptr};
  rm::device::AimbotCanCommunicator *can_communicator{nullptr}; ///< CAN 通信器

  // 设备 //
  rm::device::DeviceManager<1> device_rc; ///< 设备管理器，维护所有设备在线状态
  rm::device::DeviceManager<2> device_gimbal;
  rm::device::DeviceManager<1> device_nuc;
  // 云台
  rm::device::DR16 *rc{nullptr}; ///< 遥控器
  rm::device::DmMotor<rm::device::DmMotorControlMode::kMit> *yaw_motor{nullptr}; ///< 云台 Yaw 电机
  rm::device::DmMotor<rm::device::DmMotorControlMode::kMit> *pitch_motor{nullptr}; ///< 云台 Pitch 电机
  rm::device::JyMe02Can *yaw_encoder{nullptr}; ///< 云台 Yaw 轴角度编码器
  rm::device::JyMe02Can *pitch_encoder{nullptr}; ///< 云台 Pitch 轴角度编码器
  // 裁判系统串口
  rm::device::Referee<rm::device::RefereeRevision::kNewV120> *referee{nullptr};
  rm::device::RefereeUser<rm::device::RefereeRevision::kNewV120> *subReferee{nullptr};
  u_int8_t dataBox[128];
  // USB //
  GimbalDataFrame_SCM_t GimbalData{0, 0, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0}; ///< IMU数据
  AimbotFrame_SCM_t Aimbot{0, 0, 0, 0, 0.0f, 0.0f, 0.0f, 0}; ///< 自瞄数据

  StateMachineType StateMachine_ = {kNoForce}; // 当前状态
  uint16_t time_ = 0; // 主程序计数器
  u_int16_t time_offline[3]{}; // 掉线计数器
  u_int8_t aim_mode = 0; // 自瞄模式
  u_int8_t time_camera = 0; // 摄像头计数器
  u_int16_t imu_count = 0; // IMU计数器
  u_int32_t imu_time = 0; // INU解算时的时间戳
  const float yaw_gyro_bias_ = 0.0015f; // 偏航角（角度值）的陀螺仪偏移量

  rm::device::DR16::SwitchPosition last_switch_l = rm::device::DR16::SwitchPosition::kDown; // 左拨杆上一次状态
  rm::device::DR16::SwitchPosition last_switch_r = rm::device::DR16::SwitchPosition::kDown; // 右拨杆上一次状态
public:
  void Init();

  void SubLoop500Hz();

  void SubLoop250Hz();

  void SubLoop100Hz();

  void SubLoop50Hz();

  void SubLoop10Hz();

private:
  void RCStateUpdate();
} *globals;

#endif  // MAIN_HPP