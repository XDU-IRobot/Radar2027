//
// Created by Jason on 26-5-27.
//

#include "UI/UIDrone.hpp"
#include "librm.hpp"
#include "../main.hpp"

using namespace rm;
using namespace rm::device;

extern u16 robotID;

static float cmd_yaw = 0.0f;
static i16 cmd_ammo = 0;
static u8 cmd_fire = 0;

// 无人机显示英雄UI
void UIDroneHero_add() {
  UIFigure5 UIGroup1;
  UIGroup1.figure1.fillFloat(u8"yaw", UIFigure::Operation::Add, 0,
                             UIFigure::Color::Yellow, 5, 1460, 470, 20,
                             globals->subReferee->data().hero_2_drone.hero_yaw_angle * 1000);
  UIGroup1.figure2.fillFloat(u8"pit", UIFigure::Operation::Add, 0,
                             UIFigure::Color::Black, 5, 1620, 470, 20,
                             globals->subReferee->data().hero_2_drone.hero_pitch_angle * 1000);
  UIGroup1.figure3.fillIntegrate(u8"adj", UIFigure::Operation::Add, 0,
                                 UIFigure::Color::Green, 5, 1650, 580, 22,
                                 globals->subReferee->data().hero_2_drone.hero_ammo_adjust);
  UIGroup1.figure4.fillFloat(u8"isp", UIFigure::Operation::Add, 0,
                             UIFigure::Color::Cyan, 5, 1460, 580, 22,
                             globals->subReferee->data().hero_2_drone.hero_initial_speed * 1000);
  UIGroup1.figure5.fillFloat(u8"cya", UIFigure::Operation::Add, 0,
                             UIFigure::Color::RedBlue, 5, 1460, 520, 24,
                             cmd_yaw * 1000);

  const auto dataLen = Referee0x301Prepare(globals->dataBox, 0, UIGroup1, robotID,
                                           robotID + 256);
  globals->refereeUart->Write(globals->dataBox, dataLen);
}

void UIDroneHero_edit() {
  UIFigure5 UIGroup1;
  UIGroup1.figure1.fillFloat(u8"yaw", UIFigure::Operation::Edit, 0,
                             UIFigure::Color::Yellow, 5, 1460, 470, 20,
                             globals->subReferee->data().hero_2_drone.hero_yaw_angle * 1000);
  UIGroup1.figure2.fillFloat(u8"pit", UIFigure::Operation::Edit, 0,
                             UIFigure::Color::Black, 5, 1620, 470, 20,
                             globals->subReferee->data().hero_2_drone.hero_pitch_angle * 1000);
  UIGroup1.figure3.fillIntegrate(u8"adj", UIFigure::Operation::Edit, 0,
                                 UIFigure::Color::Green, 5, 1650, 580, 22,
                                 globals->subReferee->data().hero_2_drone.hero_ammo_adjust);
  UIGroup1.figure4.fillFloat(u8"isp", UIFigure::Operation::Edit, 0,
                             UIFigure::Color::Cyan, 5, 1460, 580, 22,
                             globals->subReferee->data().hero_2_drone.hero_initial_speed * 1000);
  UIGroup1.figure5.fillFloat(u8"cya", UIFigure::Operation::Edit, 0,
                             UIFigure::Color::RedBlue, 5, 1460, 520, 24,
                             cmd_yaw * 1000);
  const auto dataLen = Referee0x301Prepare(globals->dataBox, 0, UIGroup1, robotID,
                                           robotID + 256);
  globals->refereeUart->Write(globals->dataBox, dataLen);
}

void D2H_func() {
  static auto last_rc_key_x = false;
  static auto last_rc_key_c = false;
  if (globals->rc->switch_l() == DR16::SwitchPosition::kDown) {
    cmd_yaw = globals->subReferee->data().hero_2_drone.hero_yaw_angle;
    cmd_ammo = globals->subReferee->data().hero_2_drone.hero_ammo_adjust;
  } else {
    cmd_yaw += static_cast<float>(globals->rc->left_x()) * 0.000001f;
    if (last_rc_key_x == false && globals->rc->key(DR16::Key::kX)) {
      cmd_ammo += 10;
    } else if (last_rc_key_c == false && globals->rc->key(DR16::Key::kC)) {
      cmd_ammo -= 10;
    }
  }
  last_rc_key_x = globals->rc->key(DR16::Key::kX);
  last_rc_key_c = globals->rc->key(DR16::Key::kC);

  if (globals->rc->dial() > 600) { cmd_fire = 1; } else { cmd_fire = 0; }
  Drone2Hero d2h{cmd_yaw, cmd_ammo, cmd_fire};
  const auto size = Referee0x301Prepare(globals->dataBox, 0, d2h,
                                                    robotID,
                                                    robotID - 5);
  globals->refereeUart->Write(globals->dataBox, size);
}