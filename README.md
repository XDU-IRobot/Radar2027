# Radar

Radar 是 RoboMaster 雷达站通信项目，负责接收裁判系统数据、维护比赛状态，并按规则生成和发送雷达相关指令。项目主线是“接收、处理、发送”，不包含视觉识别功能。

## 主要功能

- 通过串口接收裁判系统常规协议。
- 通过 TCP 接收信息波和敌方密钥链路数据。
- 支持真实设备输入和二进制文件回放。
- 根据比赛状态维护并发送 `0x0305` 小地图数据，以及 `0x0301/0x0121` 雷达指令。
- 记录协议数据、原始字节流和运行状态，便于调试与赛后复盘。

当前配置使用 RoboMaster 2027 V1.0.0 通信协议。协议版本、输入模式和链路参数统一在 [include/config/config.hpp](include/config/config.hpp) 中配置。

## 构建

```bash
cmake -S . -B build
cmake --build build --target radar -j2
```

## 文档

- [AGENTS.md](AGENTS.md)：项目结构、约定和修改边界
- [changes.md](changes.md)：2027 协议适配记录与待核对项
- [.agent/handoff.md](.agent/handoff.md)：当前进度和后续接手事项
