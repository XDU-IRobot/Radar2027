#ifndef GIMBAL_HPP
#define GIMBAL_HPP

#include <librm.hpp>

#include "main.hpp"
#include "encoder_counter.hpp"
#include "yaw_speed_feedforward.hpp"

using namespace rm;

inline class Gimbal {
public:
  StateMachineType GimbalMove_ = {kNoForce}; // 云台运动状态

  rm::modules::PID yaw_speed_pid_;
  rm::modules::PID pitch_speed_pid_;

  f32 yaw_angle_encoder_ = 0.0f; // 云台yaw轴当前角度 （编码器控制，弧度制，左大右小）
  f32 yaw_vel_ = 0.0f;
  f32 pitch_angle_encoder_ = 0.0f; // 云台pitch轴当前角度 （编码器控制，弧度制，下大上小）
  f32 pitch_vel_ = 0.0f;
  f32 gimbal_yaw_target_ = 0.0f; // 云台yaw轴目标数据（编码器控制，弧度制，左正右负）
  f32 gimbal_pitch_target_ = 0.0f; // 云台pitch轴目标数据（编码器控制，弧度制，下正上负）
  f32 yaw_target_encoder_ = 0.0f; // 云台yaw轴目标数据（编码器控制，弧度制，左正右负）
  f32 pitch_target_encoder_ = 0.0f; // 云台pitch轴目标数据（编码器控制，弧度制，下正上负）

  bool yaw_enable_flag_ = false; // yaw 轴电机使能标志
  bool pitch_enable_flag_ = false; // pitch 轴电机使能标志
  bool first_reade_flag_ = false;

private:
  rm::modules::PID yaw_position_pid_;
  rm::modules::PID pitch_position_pid_;
  YawSpeedFeedforward *yaw_feedforward_;
  YawSpeedFeedforward *pitch_feedforward_;

  EncoderCounter yaw_encoder_;
  EncoderCounter pitch_encoder_;

  const f32 sensitivity_ = 0.0005f; // 云台灵敏度
  const f32 max_yaw_angle_ = 0.615f; // 云台yaw轴最大角度（弧度制）
  const f32 min_yaw_angle_ = -0.615f; // 云台yaw轴最小角度（弧度制）
  const f32 highest_pitch_angle_ = 0.615f; // 云台pitch轴最高（弧度制）
  const f32 lowest_pitch_angle_ = -0.675f; // 云台pitch轴最低（弧度制）
  const f32 max_yaw_encoder_angle_ = 1.486f; // 云台yaw轴最大角度（弧度制）
  const f32 min_yaw_encoder_angle_ = 1.058f; // 云台yaw轴最小角度（弧度制）
  const f32 highest_pitch_encoder_angle_ = 1.580f; // 云台yaw轴最大角度（弧度制）
  const f32 lowest_pitch_encoder_angle_ = 1.310f; // 云台yaw轴最小角度（弧度制）

public:
  void GimbalInit();

  void GimbalTask();

private:
  void GimbalStateUpdate();

  void GimbalRCTargetUpdate();

  void GimbalAimbotTargetUpdate();

  void GimbalTargetEncoderUpdate();

  void GimbalMovePIDUpdate();

  void GimbalMatchUpdate();

  void GimbalEnableUpdate();

  void GimbalDisableUpdate();

  void DaMiaoMotorEnable();

  void DaMiaoMotorDisable();
} *gimbal;

#endif  // GIMBAL_HPP