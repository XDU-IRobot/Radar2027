/*
Copyright (c) 2026 XDU-IRobot

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

/**
 * @file  librm/device/referee/protocol_new_v200.hpp
 * @brief 裁判系统通信协议V2.0.0（2026-6-26）
 */

#ifndef LIBRM_DEVICE_REFEREE_PROTOCOL_NEW_V200_HPP
#define LIBRM_DEVICE_REFEREE_PROTOCOL_NEW_V200_HPP

#include "protocol.hpp"

#include <cstddef>

#include <mapbox/eternal.hpp>

namespace rm::device {

template <>
struct RefereeCmdId<RefereeRevision::kNewV200> {
  constexpr static u16 kGameStatus = 0x1;              // 比赛状态
  constexpr static u16 kGameResult = 0x2;              // 比赛结果
  constexpr static u16 kGameRobotHp = 0x3;             // 机器人血量
  constexpr static u16 kEventData = 0x101;             // 场地事件
  constexpr static u16 kRefereeWarning = 0x104;        // 裁判系统警告
  constexpr static u16 kDartInformation = 0x105;       // 飞镖发射
  constexpr static u16 kRobotStatus = 0x201;           // 机器人性能数据
  constexpr static u16 kPowerHeatData = 0x202;         // 底盘/射击热量
  constexpr static u16 kRobotPos = 0x203;              // 机器人位置
  constexpr static u16 kBuff = 0x204;                  // 增益
  constexpr static u16 kHurtData = 0x206;              // 伤害数据
  constexpr static u16 kShootData = 0x207;             // 射击数据
  constexpr static u16 kProjectileAllowance = 0x208;   // 允许发弹量;
  constexpr static u16 kRfidStatus = 0x209;            // RFID模块
  constexpr static u16 kDartClientCmd = 0x20a;         // 飞镖选手端
  constexpr static u16 kGroundRobotPosition = 0x20b;   // 地面机器人位置数据（哨兵）
  constexpr static u16 kRadarMarkData = 0x20c;         // 雷达标记
  constexpr static u16 kSentryInfo = 0x20d;            // 哨兵自主决策
  constexpr static u16 kRadarInfo = 0x20e;             // 雷达自主决策
  constexpr static u16 kRobotInteractionData = 0x301;  // 机器人交互数据
  constexpr static u16 kCustomRobotData = 0x302;       // 自定义控制器与机器人交互
  constexpr static u16 kMapCommand = 0x303;            // 选手端小地图
  constexpr static u16 kMapRobotData = 0x305;          // 选手端小地图收雷达数据
  constexpr static u16 kCustomClientData = 0x306;      // 自定义控制器与选手交互
  constexpr static u16 kMapData = 0x307;               // 选手端小地图（哨兵）
  constexpr static u16 kCustomInfo = 0x308;            // 选手端小地图接收己方机器人数据
  constexpr static u16 kRobotCustomData = 0x309;       // 自定义控制器接收
  constexpr static u16 kRobotCustomData2 = 0x310;      // 自定义客户端
  constexpr static u16 kRobotCustomData3 = 0x311;      // 自定义客户端与机器人交互自定义命令
  constexpr static u16 kRadar0 = 0xa01;                // 雷达标记对方位置
  constexpr static u16 kRadar1 = 0xa02;                // 雷达标记对方血量
  constexpr static u16 kRadar2 = 0xa03;                // 雷达标记对方发弹
  constexpr static u16 kRadar3 = 0xa04;                // 雷达标记对方宏观状态
  constexpr static u16 kRadar4 = 0xa05;                // 雷达标记对方增益
  constexpr static u16 kRadar5 = 0xa06;                // 雷达标记对方干扰波密钥
};

#pragma pack(push, 1)

template <>
struct RefereeProtocol<RefereeRevision::kNewV200> {
  struct {
    u8 game_type : 4;
    u8 game_progress : 4;
    u16 stage_remain_time;
    u64 SyncTimeStamp;
  } game_status;
  struct {
    u8 winner;
  } game_result;
  struct {
    u16 ally_1_robot_HP;
    u16 ally_2_robot_HP;
    u16 ally_3_robot_HP;
    u16 ally_4_robot_HP;
    int16_t damage_difference;
    u16 ally_7_robot_HP;
    u16 ally_outpost_HP;
    u16 ally_base_HP;
    u16 enemy_outpost_HP;
    u16 enemy_base_HP;
  } game_robot_HP;
  struct {
    u32 event_data;
  } event_data;
  struct {
    u8 level;
    u8 offending_robot_id;
    u8 count;
  } referee_warning;
  struct {
    u8 dart_remaining_time;
    u16 dart_info;
  } dart_info;
  struct {
    u8 robot_id;
    u8 robot_level;
    u16 current_HP;
    u16 maximum_HP;
    u16 shooter_barrel_cooling_value;
    u16 shooter_barrel_heat_limit;
    u16 chassis_power_limit;
    float bullet_speed_limit;
    u8 power_management_gimbal_output : 1;
    u8 power_management_chassis_output : 1;
    u8 power_management_shooter_output : 1;
  } robot_status;
  struct {
    u16 reserved;
    u16 reserved_2;
    f32 reserved_3;
    u16 buffer_energy;
    u16 shooter_17mm_1_barrel_heat;
    u16 shooter_42mm_barrel_heat;
  } power_heat_data;
  struct {
    f32 x;
    f32 y;
    f32 angle;
  } robot_pos;
  struct {
    u8 recovery_buff;
    u16 cooling_buff;
    u8 defence_buff;
    u8 vulnerability_buff;
    u16 attack_buff;
    u8 remaining_energy;
  } buff;
  struct {
    u8 armor_id : 4;
    u8 HP_deduction_reason : 4;
  } hurt_data;
  struct {
    u8 bullet_type;
    u8 shooter_number;
    u8 launching_frequency;
    f32 initial_speed;
  } shoot_data;
  struct {
    u16 projectile_allowance_17mm;
    u16 projectile_allowance_42mm;
    u16 remaining_gold_coin;
    u16 projectile_allowance_fortress;
  } projectile_allowance;
  struct {
    u32 rfid_status;
    u8 rfid_status_2;
  } rfid_status;
  struct {
    u8 dart_launch_opening_status;
    u8 reserved;
    u16 target_change_time;
    u16 latest_launch_cmd_time;
  } dart_client_cmd;
  struct {
    f32 hero_x;
    f32 hero_y;
    f32 engineer_x;
    f32 engineer_y;
    f32 standard_3_x;
    f32 standard_3_y;
    f32 standard_4_x;
    f32 standard_4_y;
    f32 reserved;
    f32 reserved_2;
  } ground_robot_position;
  struct {
    u16 mark_progress;
  } radar_mark_data;
  struct {
    u32 sentry_info;
    u16 sentry_info_2;
    u64 sentry_info_3;
  } sentry_info;
  struct {
    u8 radar_info;
  } radar_info;
  struct {
    u16 data_cmd_id;
    u16 sender_id;
    u16 receiver_id;
    u8 user_data[112];
  } robot_interaction_data;
  struct {
    f32 target_position_x;
    f32 target_position_y;
    u8 cmd_keyboard;
    u8 target_robot_id;
    u16 cmd_source;
  } map_command;
  struct {
    u16 opponent_hero_position_x;
    u16 opponent_hero_position_y;
    u16 opponent_engineer_position_x;
    u16 opponent_engineer_position_y;
    u16 opponent_infantry_3_position_x;
    u16 opponent_infantry_3_position_y;
    u16 opponent_infantry_4_position_x;
    u16 opponent_infantry_4_position_y;
    u16 opponent_aerial_position_x;
    u16 opponent_aerial_position_y;
    u16 opponent_sentry_position_x;
    u16 opponent_sentry_position_y;
    u16 ally_hero_position_x;
    u16 ally_hero_position_y;
    u16 ally_engineer_position_x;
    u16 ally_engineer_position_y;
    u16 ally_infantry_3_position_x;
    u16 ally_infantry_3_position_y;
    u16 ally_infantry_4_position_x;
    u16 ally_infantry_4_position_y;
    u16 ally_aerial_position_x;
    u16 ally_aerial_position_y;
    u16 ally_sentry_position_x;
    u16 ally_sentry_position_y;
  } map_robot_data;
  struct {
    u8 intention;
    u16 start_position_x;
    u16 start_position_y;
    i8 delta_x[49];
    i8 delta_y[49];
    u16 sender_id;
  } map_data;
  struct {
    u16 sender_id;
    u16 receiver_id;
    u8 user_data[30];
  } custom_info;
  struct {
    u8 data[30];
  } custom_robot_data;
  struct {
    u8 data[30];
  } robot_custom_data;
  struct {
    u8 data[300];
  } robot_custom_data_2;
  struct {
    u8 data[30];
  } robot_custom_data_3;
  struct {
    u16 key_value;
    u16 x_position : 12;
    u16 mouse_left : 4;
    u16 y_position : 12;
    u16 mouse_right : 4;
    u16 reserved;
  } custom_client_data;
  struct {
    u16 opponent_hero_position_x;
    u16 opponent_hero_position_y;
    u16 opponent_engineer_position_x;
    u16 opponent_engineer_position_y;
    u16 opponent_infantry_3_position_x;
    u16 opponent_infantry_3_position_y;
    u16 opponent_infantry_4_position_x;
    u16 opponent_infantry_4_position_y;
    u16 opponent_aerial_position_x;
    u16 opponent_aerial_position_y;
    u16 opponent_sentry_position_x;
    u16 opponent_sentry_position_y;
  } radar0;
  struct {
    u16 opponent_hero_HP;
    u16 opponent_engineer_HP;
    u16 opponent_infantry_3_HP;
    u16 opponent_infantry_4_HP;
    u16 reserved;
    u16 opponent_sentry_HP;
  } radar1;
  struct {
    u16 opponent_hero_allowance_ammo;
    u16 opponent_infantry_3_allowance_ammo;
    u16 opponent_infantry_4_allowance_ammo;
    u16 opponent_aerial_allowance_ammo;
    u16 opponent_sentry_allowance_ammo;
  } radar2;
  struct {
    u16 enemy_remaining_gold_coin;
    u16 enemy_total_gold_coin;
    u32 enemy_status;
  } radar3;
  struct {
    u8 enemy_hero_hp_buff;
    u16 enemy_hero_cooling_buff;
    u8 enemy_hero_defence_buff;
    u8 enemy_hero_vulnerability_buff;
    u16 enemy_hero_attack_buff;
    u8 enemy_engineer_hp_buff;
    u16 enemy_engineer_cooling_buff;
    u8 enemy_engineer_defence_buff;
    u8 enemy_engineer_vulnerability_buff;
    u16 enemy_engineer_attack_buff;
    u8 enemy_infantry_3_hp_buff;
    u16 enemy_infantry_3_cooling_buff;
    u8 enemy_infantry_3_defence_buff;
    u8 enemy_infantry_3_vulnerability_buff;
    u16 enemy_infantry_3_attack_buff;
    u8 enemy_infantry_4_hp_buff;
    u16 enemy_infantry_4_cooling_buff;
    u8 enemy_infantry_4_defence_buff;
    u8 enemy_infantry_4_vulnerability_buff;
    u16 enemy_infantry_4_attack_buff;
    u8 enemy_sentry_hp_buff;
    u16 enemy_sentry_cooling_buff;
    u8 enemy_sentry_defence_buff;
    u8 enemy_sentry_vulnerability_buff;
    u16 enemy_sentry_attack_buff;
    u8 enemy_sentry_posture;
    u8 enemy_hero_status;
    u8 enemy_engineer_status;
    u8 enemy_infantry_3_status;
    u8 enemy_infantry_4_status;
    u8 enemy_sentry_status;
  } radar4;
  /*
  enemy_sentry_posture: (1 为进攻姿态， 2 为防御姿态， 3 为移动姿态， 4 为强化进攻姿态， 5 为强化防御姿态，
  6 为强化移动姿态)
  enemy_hero_status: (0 为存活， 1 为战亡， 2 为无敌但不虚弱， 3
  为无敌且虚弱，异常离线不会影响此处的状态判断，下同)
  */
  struct {
    u8 key[6];
  } radar5;
};

#pragma pack(pop)

template <>
struct RefereeProtocolMemoryMap<RefereeRevision::kNewV200> {
  // clang-format off
  static MAPBOX_ETERNAL_CONSTEXPR const auto map = mapbox::eternal::map<u16, usize>({
      {RefereeCmdId<RefereeRevision::kNewV200>::kGameStatus, offsetof(RefereeProtocol<RefereeRevision::kNewV200>, game_status)},
      {RefereeCmdId<RefereeRevision::kNewV200>::kGameResult, offsetof(RefereeProtocol<RefereeRevision::kNewV200>, game_result)},
      {RefereeCmdId<RefereeRevision::kNewV200>::kGameRobotHp, offsetof(RefereeProtocol<RefereeRevision::kNewV200>, game_robot_HP)},
      {RefereeCmdId<RefereeRevision::kNewV200>::kEventData, offsetof(RefereeProtocol<RefereeRevision::kNewV200>, event_data)},
      {RefereeCmdId<RefereeRevision::kNewV200>::kRefereeWarning, offsetof(RefereeProtocol<RefereeRevision::kNewV200>, referee_warning)},
      {RefereeCmdId<RefereeRevision::kNewV200>::kDartInformation, offsetof(RefereeProtocol<RefereeRevision::kNewV200>, dart_info)},
      {RefereeCmdId<RefereeRevision::kNewV200>::kRobotStatus, offsetof(RefereeProtocol<RefereeRevision::kNewV200>, robot_status)},
      {RefereeCmdId<RefereeRevision::kNewV200>::kPowerHeatData, offsetof(RefereeProtocol<RefereeRevision::kNewV200>, power_heat_data)},
      {RefereeCmdId<RefereeRevision::kNewV200>::kRobotPos, offsetof(RefereeProtocol<RefereeRevision::kNewV200>, robot_pos)},
      {RefereeCmdId<RefereeRevision::kNewV200>::kBuff, offsetof(RefereeProtocol<RefereeRevision::kNewV200>, buff)},
      {RefereeCmdId<RefereeRevision::kNewV200>::kHurtData, offsetof(RefereeProtocol<RefereeRevision::kNewV200>, hurt_data)},
      {RefereeCmdId<RefereeRevision::kNewV200>::kShootData, offsetof(RefereeProtocol<RefereeRevision::kNewV200>, shoot_data)},
      {RefereeCmdId<RefereeRevision::kNewV200>::kProjectileAllowance, offsetof(RefereeProtocol<RefereeRevision::kNewV200>, projectile_allowance)},
      {RefereeCmdId<RefereeRevision::kNewV200>::kRfidStatus, offsetof(RefereeProtocol<RefereeRevision::kNewV200>, rfid_status)},
      {RefereeCmdId<RefereeRevision::kNewV200>::kDartClientCmd, offsetof(RefereeProtocol<RefereeRevision::kNewV200>, dart_client_cmd)},
      {RefereeCmdId<RefereeRevision::kNewV200>::kGroundRobotPosition, offsetof(RefereeProtocol<RefereeRevision::kNewV200>, ground_robot_position)},
      {RefereeCmdId<RefereeRevision::kNewV200>::kRadarMarkData, offsetof(RefereeProtocol<RefereeRevision::kNewV200>, radar_mark_data)},
      {RefereeCmdId<RefereeRevision::kNewV200>::kSentryInfo, offsetof(RefereeProtocol<RefereeRevision::kNewV200>, sentry_info)},
      {RefereeCmdId<RefereeRevision::kNewV200>::kRadarInfo, offsetof(RefereeProtocol<RefereeRevision::kNewV200>, radar_info)},
      {RefereeCmdId<RefereeRevision::kNewV200>::kRobotInteractionData, offsetof(RefereeProtocol<RefereeRevision::kNewV200>, robot_interaction_data)},
      {RefereeCmdId<RefereeRevision::kNewV200>::kCustomRobotData, offsetof(RefereeProtocol<RefereeRevision::kNewV200>, custom_robot_data)},
      {RefereeCmdId<RefereeRevision::kNewV200>::kMapCommand, offsetof(RefereeProtocol<RefereeRevision::kNewV200>, map_command)},
      {RefereeCmdId<RefereeRevision::kNewV200>::kMapRobotData, offsetof(RefereeProtocol<RefereeRevision::kNewV200>, map_robot_data)},
      {RefereeCmdId<RefereeRevision::kNewV200>::kCustomClientData, offsetof(RefereeProtocol<RefereeRevision::kNewV200>, custom_client_data)},
      {RefereeCmdId<RefereeRevision::kNewV200>::kMapData, offsetof(RefereeProtocol<RefereeRevision::kNewV200>, map_data)},
      {RefereeCmdId<RefereeRevision::kNewV200>::kCustomInfo, offsetof(RefereeProtocol<RefereeRevision::kNewV200>, custom_info)},
      {RefereeCmdId<RefereeRevision::kNewV200>::kRobotCustomData, offsetof(RefereeProtocol<RefereeRevision::kNewV200>, robot_custom_data)},
      {RefereeCmdId<RefereeRevision::kNewV200>::kRobotCustomData2, offsetof(RefereeProtocol<RefereeRevision::kNewV200>, robot_custom_data_2)},
      {RefereeCmdId<RefereeRevision::kNewV200>::kRobotCustomData3, offsetof(RefereeProtocol<RefereeRevision::kNewV200>, robot_custom_data_3)},
      {RefereeCmdId<RefereeRevision::kNewV200>::kRadar0, offsetof(RefereeProtocol<RefereeRevision::kNewV200>, radar0)},
      {RefereeCmdId<RefereeRevision::kNewV200>::kRadar1, offsetof(RefereeProtocol<RefereeRevision::kNewV200>, radar1)},
      {RefereeCmdId<RefereeRevision::kNewV200>::kRadar2, offsetof(RefereeProtocol<RefereeRevision::kNewV200>, radar2)},
      {RefereeCmdId<RefereeRevision::kNewV200>::kRadar3, offsetof(RefereeProtocol<RefereeRevision::kNewV200>, radar3)},
      {RefereeCmdId<RefereeRevision::kNewV200>::kRadar4, offsetof(RefereeProtocol<RefereeRevision::kNewV200>, radar4)},
      {RefereeCmdId<RefereeRevision::kNewV200>::kRadar5, offsetof(RefereeProtocol<RefereeRevision::kNewV200>, radar5)},
  });
  // clang-format on
};

}  // namespace rm::device

#endif  // LIBRM_DEVICE_REFEREE_PROTOCOL_NEW_V200_HPP