# 2027 协议适配交接

更新时间：2026-10-31

## 目标与配置

本轮工作将 Radar 项目适配到《RoboMaster 2027 机甲大师高校系列赛通信协议 V1.0.0》。当前配置在 `include/config/config.hpp` 中选择 `RefereeRevision::k2027V100`。差异依据和检查清单记录在 `changes.md`，协议 PDF 位于 `rules/`。

## 已完成

- 2027 协议头提供新赛季字段和内存映射；项目侧解析、日志、地图转发和 UI 展示已按 heavyengineer/reserved 语义调整。
- `hero` 对应 `heavyengineer`；`engineer`、`drone/aerial` 对应的保留槽不作为有效机器人状态处理。
- `0x0305` 地图数据保持 48 字节布局，保留槽不再转发旧赛季工程或空中机器人数据。
- `0x0301` 子命令 ID `0x0100`–`0x0104`、`0x0110`、`0x0120`、`0x0121` 沿用原编号；`0x0121 RadarCMD` 扩展为 13 字节，新增 `armor_fortify_cmd`、`fortify_center_x`、`fortify_center_y`，并有长度编译期断言。
- `RadarCMD` 新字段默认零初始化，没有加入自动装甲加固策略。
- 验证命令 `cmake --build build --target radar -j2` 已成功。

## Git 状态

- 根仓库分支为 `2027radar`，提交 `595c645` 已推送到 `https://github.com/XDU-IRobot/Radar2027.git` 的 `main` 分支，远程名为 `radar2027`。
- 根仓库将 `include/radar` 记录为子模块指针 `bef31d0`。该提交在 `include/radar` 仓库中尚未推到 `.gitmodules` 指定的 `https://github.com/XDU-IRobot/radar.git`；此前推送被拒绝，错误为 `403 Write access to repository not granted`。根仓库已发布，但从远程完整初始化该子模块仍需解决提交可访问性。
- `include/radar/libs/librm` 是嵌套仓库，当前仍在 `main` 且有未提交改动，包含 `protocol_2027_v100.hpp`、`protocol_new_v200.hpp` 等文件。父级 `2027radar` 提交只记录了 librm 提交指针，不包含 librm 工作区改动；此前讨论过单独建立 `2027V100` 分支，但尚未创建或推送。
- Git 状态曾显示 `.agent/full_pipeline.md` 和 `.agent/radar_stability_review.md` 被删除。本次只按新要求重新生成了 `.agent/handoff.md`，没有恢复另外两份文档。

## 后续事项

1. 请仓库管理员给当前 GitHub 账号授予 `XDU-IRobot/radar` 写权限。授权后运行 `git -C include/radar push -u origin 2027radar`。
2. 在 `include/radar/libs/librm` 检查变更后创建并提交 `2027V100` 分支，再推送到 librm 远程。注意 librm 工作区包含大量文件差异，提交前核对暂存内容。
3. 按 `changes.md` 复核仍未确认的命令映射和业务消费情况，尤其是 2027 命令表中不再出现的旧命令，以及新增加固字段是否需要纳入比赛策略。
4. 协议和项目侧改动继续完成后，重新构建 `radar`，并检查 2027 保留槽和 `0x0121` 发送帧长度。
