#include "Gimbal.hpp"

void Gimbal::GimbalInit() {
  // PID 传参
  gimbal->yaw_position_pid_.SetKp(32000.0f).SetKi(0.0f).SetKd(0.0f).SetMaxOut(150.0f).SetMaxIout(0.0f);
  gimbal->yaw_speed_pid_.SetKp(0.0125f).SetKi(0.0f).SetKd(0.0f).SetMaxOut(1.8f).SetMaxIout(0.0f);
  gimbal->pitch_position_pid_.SetKp(18000.0f).SetKi(0.0f).SetKd(0.0f).SetMaxOut(150.0f).SetMaxIout(0.0f);
  gimbal->pitch_speed_pid_.SetKp(0.0125f).SetKi(0.0f).SetKd(0.0f).SetMaxOut(1.8f).SetMaxIout(0.0f);

  yaw_feedforward_ = new YawSpeedFeedforward(0.002, 0);
  pitch_feedforward_ = new YawSpeedFeedforward(0.002, 0);

  // gimbal->yaw_encoder_.Reset(0, globals->yaw_motor->pos());
  // gimbal->pitch_encoder_.Reset(0, globals->pitch_motor->pos());
  gimbal->gimbal_yaw_target_ = 0;
  gimbal->gimbal_pitch_target_ = 0;
}

void Gimbal::GimbalTask() {
  // gimbal->yaw_encoder_.Update(globals->yaw_motor->pos());
  // gimbal->pitch_encoder_.Update(globals->pitch_motor->pos());

  yaw_angle_encoder_ = globals->yaw_encoder->angle_deg() / 180.f * static_cast<f32>(M_PI);
  pitch_angle_encoder_ = globals->pitch_encoder->angle_deg() / 180.f * static_cast<f32>(M_PI);
  yaw_vel_ = globals->yaw_motor->vel();
  pitch_vel_ = globals->pitch_motor->vel();

  gimbal->GimbalStateUpdate();
}

void Gimbal::GimbalStateUpdate() {
  switch (globals->StateMachine_) {
    case kTest:
      gimbal->GimbalEnableUpdate();
      break;
    default:
      gimbal->GimbalDisableUpdate();
  }
}

void Gimbal::GimbalRCTargetUpdate() {
  // if (first_reade_flag_ == false) {
  //   gimbal->yaw_target_encoder_ = globals->yaw_encoder->angle_deg() / 180.f * static_cast<f32>(M_PI);
  //   gimbal->pitch_target_encoder_ = -globals->pitch_encoder->angle_deg() / 180.f * static_cast<f32>(M_PI);
  //   first_reade_flag_ = true;
  // }

  gimbal->yaw_target_encoder_ += rm::modules::Map(globals->rc->right_x(), -660.0f, 660.0f,
                                                  -gimbal->sensitivity_, gimbal->sensitivity_);
  gimbal->pitch_target_encoder_ += rm::modules::Map(globals->rc->right_y(), -660.0f, 660.0f,
                                                    -gimbal->sensitivity_, gimbal->sensitivity_);

  gimbal->yaw_target_encoder_ = rm::modules::Clamp(gimbal->yaw_target_encoder_,
                                                   gimbal->min_yaw_encoder_angle_, gimbal->max_yaw_encoder_angle_);
  gimbal->pitch_target_encoder_ = rm::modules::Clamp(gimbal->pitch_target_encoder_,
                                                     gimbal->lowest_pitch_encoder_angle_,
                                                     gimbal->highest_pitch_encoder_angle_);

  globals->Aimbot.Yaw = gimbal->yaw_target_encoder_;
  globals->Aimbot.Pitch = gimbal->pitch_target_encoder_;
}

void Gimbal::GimbalAimbotTargetUpdate() {
  if (globals->Aimbot.AimbotState == 0x01) {
    gimbal->yaw_target_encoder_ = globals->Aimbot.Yaw;
    gimbal->pitch_target_encoder_ = globals->Aimbot.Pitch;
  } else {
    gimbal->yaw_target_encoder_ = gimbal->yaw_target_encoder_;
    gimbal->pitch_target_encoder_ = gimbal->pitch_target_encoder_;
  }

  gimbal->yaw_target_encoder_ = rm::modules::Clamp(gimbal->yaw_target_encoder_,
                                                   gimbal->min_yaw_encoder_angle_, gimbal->max_yaw_encoder_angle_);
  gimbal->pitch_target_encoder_ = rm::modules::Clamp(gimbal->pitch_target_encoder_,
                                                     gimbal->lowest_pitch_encoder_angle_,
                                                     gimbal->highest_pitch_encoder_angle_);
}

void Gimbal::GimbalTargetEncoderUpdate() {
  gimbal->yaw_target_encoder_ =
      (500.0f * std::sin(gimbal_yaw_target_ - 0.175f) + 200.0f) / 3.0f * static_cast<f32>(M_PI);
  gimbal->pitch_target_encoder_ =
      (500.0f - std::cos(gimbal_pitch_target_) * 300.0f -
       static_cast<f32>(std::sqrt(40000.0f - std::pow(std::sin(gimbal_pitch_target_) * 300.0f, 2)))) /
      3.0f * static_cast<f32>(M_PI);
}

f32 d_yaw_v_error = 0.0f, d_pitch_v_error = 0.0f;
f32 d_yaw_v_forward = 0.f, d_pitch_v_forward = 0.f;

void Gimbal::GimbalMovePIDUpdate() {
  yaw_feedforward_->Update(gimbal->yaw_target_encoder_);
  pitch_feedforward_->Update(gimbal->pitch_target_encoder_);

  gimbal->yaw_position_pid_.Update(gimbal->yaw_target_encoder_, yaw_angle_encoder_);
  gimbal->yaw_speed_pid_.Update(gimbal->yaw_position_pid_.out() + yaw_feedforward_->GetYawSpeedFeedforward(),
                                globals->yaw_motor->vel());
  gimbal->pitch_position_pid_.Update(gimbal->pitch_target_encoder_, pitch_angle_encoder_);
  gimbal->pitch_speed_pid_.Update(gimbal->pitch_position_pid_.out() + pitch_feedforward_->GetYawSpeedFeedforward(),
                                  globals->pitch_motor->vel());

  // 调试信息
  d_yaw_v_error = gimbal->yaw_position_pid_.out();
  d_yaw_v_forward = yaw_feedforward_->GetYawSpeedFeedforward();
}

void Gimbal::GimbalEnableUpdate() {
  gimbal->DaMiaoMotorEnable();
  if (gimbal->GimbalMove_ == kGbRemote) {
    globals->aim_mode = 0x00;
    gimbal->GimbalRCTargetUpdate();
    gimbal->GimbalMovePIDUpdate();
  } else if (gimbal->GimbalMove_ == kGbAimbot) {
    globals->aim_mode = 0x01;
    gimbal->GimbalAimbotTargetUpdate();
    gimbal->GimbalMovePIDUpdate();
  } else if (gimbal->GimbalMove_ == kGbTest) {
    gimbal->GimbalMovePIDUpdate();
  } else {
    globals->aim_mode = 0x00;
  }
}

void Gimbal::GimbalDisableUpdate() {
  gimbal->DaMiaoMotorDisable();
  gimbal->yaw_speed_pid_.Update(0, 0);
  gimbal->pitch_speed_pid_.Update(0, 0);
  gimbal->yaw_target_encoder_ = yaw_angle_encoder_;
  gimbal->pitch_target_encoder_ = pitch_angle_encoder_;
}

void Gimbal::DaMiaoMotorEnable() {
  if (gimbal->yaw_enable_flag_ == false && gimbal->pitch_enable_flag_ == true) {
    globals->yaw_motor->SendInstruction(rm::device::DmMotorInstructions::kEnable);
    gimbal->yaw_enable_flag_ = true;
  }
  if (gimbal->pitch_enable_flag_ == false) {
    globals->pitch_motor->SendInstruction(rm::device::DmMotorInstructions::kEnable);
    gimbal->pitch_enable_flag_ = true;
  }
}

void Gimbal::DaMiaoMotorDisable() {
  if (gimbal->yaw_enable_flag_ == true && gimbal->pitch_enable_flag_ == false) {
    globals->yaw_motor->SendInstruction(rm::device::DmMotorInstructions::kDisable);
    gimbal->yaw_enable_flag_ = false;
  }
  if (gimbal->pitch_enable_flag_ == true) {
    globals->pitch_motor->SendInstruction(rm::device::DmMotorInstructions::kDisable);
    gimbal->pitch_enable_flag_ = false;
  }
}