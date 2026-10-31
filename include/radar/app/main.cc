#include <librm.hpp>

#include "can.h"
#include "stm32f4xx_hal_gpio.h"
#include "usart.h"

#include "timer_task.hpp"

#include "main.hpp"
#include "Gimbal.hpp"
#include "subReferee/TaskScheduler.hpp"
#include "UI/UIuser1.hpp"
#include "UI/UIDrone.hpp"

using namespace rm;

void MainLoop() {
  globals->time_++;
  globals->SubLoop500Hz();
  globals->SubLoop250Hz();
  globals->SubLoop100Hz();
  globals->SubLoop50Hz();
  globals->SubLoop10Hz();
}

extern "C" [[noreturn]] void AppMain(void) {
  globals = new GlobalWarehouse;
  gimbal = new Gimbal;
  HAL_GPIO_WritePin(GPIOH, GPIO_PIN_2, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOH, GPIO_PIN_3, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOH, GPIO_PIN_4, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOH, GPIO_PIN_5, GPIO_PIN_SET);
  rm::Sleep(std::chrono::milliseconds(2000));
  globals->Init();

  // 创建主循环定时任务，定频1khz
  TimerTask mainloop_1000hz{
      &htim14, //
      etl::delegate<void()>::create<MainLoop>() //
  };
  mainloop_1000hz.SetPrescalerAndPeriod(84 - 1, 1000 - 1);
  mainloop_1000hz.Start();

  for (;;) {
    // __WFI();
  }
}

void static_UI_add();

void GlobalWarehouse::Init() {
  buzzer = new Buzzer;

  can1 = new rm::hal::Can{hcan1};
  // can_communicator = new rm::device::AimbotCanCommunicator(*can2);
  dbus = new rm::hal::Serial{huart1, 18, rm::hal::stm32::UartMode::kNormal, rm::hal::stm32::UartMode::kDma};
  refereeUart = new rm::hal::Serial{huart3, 127, rm::hal::stm32::UartMode::kDma, rm::hal::stm32::UartMode::kDma};

  rc = new rm::device::DR16{*dbus};
  yaw_motor = new rm::device::DmMotor<rm::device::DmMotorControlMode::kMit> //
      {*can1, {0x04, 0x03, 3.141593f, 420.0f, 5.0f, {0.f, 500.f}, {0.f, 5.f}}};
  pitch_motor = new rm::device::DmMotor<rm::device::DmMotorControlMode::kMit> //
      {*can1, {0x02, 0x01, 3.141593f, 420.0f, 5.0f, {0.f, 500.f}, {0.f, 5.f}}};
  yaw_encoder = new rm::device::JyMe02Can{*can1, 0x60, 0.002f};
  pitch_encoder = new rm::device::JyMe02Can{*can1, 0x55, 0.002f};
  referee = new rm::device::Referee<device::RefereeRevision::kNewV120>;
  subReferee = new rm::device::RefereeUser(*referee);
  referee->AttachCallback(std::bind(&rm::device::RefereeUser<rm::device::RefereeRevision::kNewV120>::AttachCallback,
                                    subReferee, std::placeholders::_1,
                                    std::placeholders::_2));
  refereeUART = new rm::device::RefereeUART{*refereeUart, *referee};

  can1->SetFilter(0, 0);
  can1->Begin();
  rc->Begin();
  buzzer->Init();
  refereeUART->Begin();

  device_rc << rc; // 遥控器
  device_gimbal << yaw_motor << pitch_motor; // 云台电机
  // device_nuc << can_communicator;

  buzzer_controller.Play<modules::buzzer_melody::Startup>();

  gimbal->GimbalInit();
}

void GlobalWarehouse::RCStateUpdate() {
  if (globals->device_rc.all_device_ok() && globals->device_gimbal.all_device_ok()) {
    switch (globals->rc->switch_r()) {
      case rm::device::DR16::SwitchPosition::kUp:
      case rm::device::DR16::SwitchPosition::kMid:
        globals->StateMachine_ = kTest;
        break;
      case rm::device::DR16::SwitchPosition::kDown:
      default:
        globals->StateMachine_ = kNoForce;
    }
    switch (globals->rc->switch_l()) {
      case rm::device::DR16::SwitchPosition::kUp:
      case rm::device::DR16::SwitchPosition::kMid:
        gimbal->GimbalMove_ = kGbAimbot;
        break;
      case rm::device::DR16::SwitchPosition::kDown:
      default:
        gimbal->GimbalMove_ = kGbRemote;
    }
  } else {
    globals->StateMachine_ = kNoForce;
  }
}

void Euler_To_Quat() {
  float cy = cos(gimbal->yaw_angle_encoder_ * 0.5f);
  float sy = sin(gimbal->yaw_angle_encoder_ * 0.5f);
  float cp = cos(gimbal->pitch_angle_encoder_ * 0.5f);
  float sp = sin(gimbal->pitch_angle_encoder_ * 0.5f);
  float cr = cos(0 * 0.5f);
  float sr = sin(0 * 0.5f);

  globals->GimbalData.q0 = cr * cp * cy + sr * sp * sy; // w
  globals->GimbalData.q1 = sr * cp * cy - cr * sp * sy; // x
  globals->GimbalData.q2 = cr * sp * cy + sr * cp * sy; // y
  globals->GimbalData.q3 = cr * cp * sy - sr * sp * cy; // z
}

f32 d_aimbot_yaw = 0, d_aimbot_pitch = 0, d_aim_state = 0;
f32 d_aimbot_yaw_vel = 0, d_aimbot_pitch_vel = 0;
f32 d_encoder_yaw = 0, d_encoder_pitch = 0;
f32 d_vel_yaw = 0, d_vel_pitch = 0;
f32 d_yaw_motor_t = 0, d_pitch_motor_t = 0;
f32 d_yaw_target = 0, d_pitch_target = 0;
f32 d_referee_pos_X = 0.0f, d_referee_pos_y = 0.0f;
f32 d_referee_ammo_direction = 0.0f;

device::Drone2Hero d2h;
device::Hero2Drone h2d;

void GlobalWarehouse::SubLoop500Hz() {
  imu_time = HAL_GetTick();

  globals->RCStateUpdate();
  gimbal->GimbalTask();

  globals->yaw_motor->SetMitCommand(0, 0, gimbal->yaw_speed_pid_.out(), 0, 0);
  globals->pitch_motor->SetMitCommand(0, 0, gimbal->pitch_speed_pid_.out(), 0, 0);

  // globals->yaw_motor->SetMitCommand(0, 0, 0, 0, 0);
  // globals->pitch_motor->SetMitCommand(0, 0, 0, 0, 0);

  // 调试信息
  d_aimbot_yaw = globals->Aimbot.Yaw;
  d_aimbot_pitch = globals->Aimbot.Pitch;
  d_aimbot_yaw_vel = globals->Aimbot.YawVelocity;
  d_aimbot_pitch_vel = globals->Aimbot.PitchVelocity;
  d_aim_state = globals->Aimbot.AimbotState;
  d_encoder_pitch = gimbal->pitch_angle_encoder_;
  d_encoder_yaw = gimbal->yaw_angle_encoder_;
  d_vel_pitch = gimbal->pitch_vel_;
  d_vel_yaw = gimbal->yaw_vel_;
  d_yaw_motor_t = gimbal->yaw_speed_pid_.out();
  d_pitch_motor_t = gimbal->pitch_speed_pid_.out();
  d_yaw_target = gimbal->yaw_target_encoder_;
  d_pitch_target = gimbal->pitch_target_encoder_;
  d_referee_pos_X = globals->referee->data().robot_pos.x;
  d_referee_pos_y = globals->referee->data().robot_pos.x;
  d_referee_ammo_direction = globals->referee->data().robot_pos.angle;
  memcpy(&d2h, &globals->subReferee->data().drone_2_hero, sizeof(d2h));
  memcpy(&h2d, &globals->subReferee->data().hero_2_drone, sizeof(h2d));
}

void GlobalWarehouse::SubLoop250Hz() {
  if (globals->time_ % 2 == 0) {
  }
}

void GlobalWarehouse::SubLoop100Hz() {
  if (globals->time_ % 5 == 0) {
    // 在线检测
    globals->device_rc.Update();
    globals->device_gimbal.Update();
    globals->device_nuc.Update();
    if (globals->rc->switch_l() != rm::device::DR16::SwitchPosition::kUnknown &&
        globals->rc->switch_r() != rm::device::DR16::SwitchPosition::kUnknown) {
      if (globals->rc->switch_l() != globals->last_switch_l || globals->rc->switch_r() != globals->last_switch_r) {
        globals->buzzer_controller.Play<modules::buzzer_melody::Beeps<1>>();
        globals->last_switch_l = globals->rc->switch_l();
        globals->last_switch_r = globals->rc->switch_r();
        static_UI_add();
      }
    }
    Euler_To_Quat();
    GimbalDataSend();
  }
}

void GlobalWarehouse::SubLoop50Hz() {
  if (globals->time_ % 10 == 0) {
    buzzer->SetFrequency(globals->buzzer_controller.Update().frequency);
  }
}

static auto schedule = device::UITaskScheduler(30);

void GlobalWarehouse::SubLoop10Hz() {
  if (globals->time_ % 34 == 0) {
    schedule.schedule();
  }
}

static auto UIRobotHeaderBlueADD = device::UITask(UITextHeaderRobotBlue_add);

static auto UIhpBlueADD = device::UITask(UITextHeaderHPBlue_add);
static auto UIhpBlueEDIT = device::UITask(UITextHeaderHPBlue_edit, 2);

static auto UIalBlueADD = device::UITask(UITextHeaderAllowBlue_add);
static auto UIalBlueEDIT = device::UITask(UITextHeaderAllowBlue_edit, 2);

static auto UIDroneHeroADD = device::UITask(UIDroneHero_add);
static auto UIDroneHeroEDIT = device::UITask(UIDroneHero_edit, 5);

static auto UId2h = device::UITask(D2H_func, 5);

void static_UI_add() {
  schedule.addTaskStatic(&UIDroneHeroADD);
  schedule.addTaskStatic(&UIRobotHeaderBlueADD);
  schedule.addTaskStatic(&UIalBlueADD);
  schedule.addTaskStatic(&UIhpBlueADD);

  schedule.addTask(&UIDroneHeroEDIT);
  schedule.addTask(&UIhpBlueEDIT);
  schedule.addTask(&UIalBlueEDIT);
}