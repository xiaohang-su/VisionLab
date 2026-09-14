# VisionLab 更新日志

## v1.1.2 (2026-09-15) — Stable Release

### 正式发布
- 首个用户可直接下载运行的稳定版本
- GitHub Release 附带预编译 EXE 包
- 静态链接运行时，零依赖开箱即用

### 平台功能
- 平台首页 UI（卡密登录、左侧导航、顶部公告、时间天气）
- 独立悬浮检测窗（无边框、可拖拽、纯白简洁风）
- 运行时性能指标 + Performance Dashboard
- Auth/Platform/Inference 服务抽象层（预留微验/ONNX/TRT/DML）

### 跨平台修复
- MinGW 交叉编译支持（Linux -> Windows）
- 修复 GCC 10 兼容性（std::format -> snprintf）
- 修复 ImGuiContext 命名空间前向声明

### 系统要求
- Windows 10/11 64-bit
- DirectX 11 支持
- 无需安装 VC++ 运行库（已静态链接）

### 下载
- GitHub Release: https://github.com/xiaohang-su/VisionLab/releases/tag/v1.1.2
- 文件名: VisionLab-1.1.2-Windows.zip (1.2 MB)

---

## v1.1.0-demo (2026-09-14)

### 平台架构
- AuthService（卡密验证 / 心跳 / 远程公告，预留微验对接）
- PlatformService（时间 / 天气 / 公告 / 弹窗 / 官方群）
- InferenceBackend（推理后端抽象，预留 ONNX/TensorRT/DirectML）
- InferenceDetector（Detector 接口 + 推理后端桥接）

### UI 重构
- 平台首页布局（顶部公告栏 + 左侧导航 + 登录页 + 底部状态栏）
- 5 个导航页面（AI检测 / 性能监控 / 模型管理 / 设置 / 关于）

---

## v1.0.0-rc1 (2026-09-14)

### API Freeze
- Core 数据结构冻结（Frame / Detection / Track / AnalysisResult）
- Runtime 生命周期冻结（initialize / start / update / shutdown）
- 模块依赖方向固定：Core ← Runtime ← Capture ← Vision ← Detection ← Tracking ← Analysis ← UI
- 文档：docs/API_FREEZE.md, docs/ARCHITECTURE.md

---

## v0.9.6 (2026-09-14) — Stabilization

### Build System 修复
- CMake target_include_directories(src) 修复
- Windows 条件编译从 `_WIN32` 改为 `_WIN32 && _MSC_VER && VISIONLAB_ENABLE_SCREEN_CAPTURE`
- D3D11 链接库补全（d3dcompiler / dwmapi）
- ImGui Handler 前向声明修复
- timer include 路径修复（core/timer.h）
- ImGui initialize 失败路径回滚（window_.destroy()）
- project VERSION 同步到 0.9.6

### 文档
- docs/BUILD_WINDOWS.md
- docs/BUILD_LINUX.md
- docs/TROUBLESHOOTING.md

---

## v0.9.0 ~ v0.9.4 — Performance Observation

### v0.9.0 Runtime Timing
- 分阶段性能计时（capture/vision/detection/tracking/analysis/render）
- FPS 统计
- RuntimeMetrics 数据结构

### v0.9.1 Logger Analysis
- Logger 耗时统计
- 日志调用次数统计

### v0.9.2 Frame Lifetime
- Frame allocation 计数
- Frame copy bytes 统计
- Texture upload bytes 统计

### v0.9.3 Cost Attribution
- Texture upload 耗时测量
- Vision copy bandwidth 分析（MB/s）

### v0.9.4 Performance Dashboard
- Stage Timeline（阶段耗时柱状图）
- Bottleneck Ranking（瓶颈排序）
- Warning Indicator（颜色阈值提示）
- Memory Flow Panel（内存流量面板）

---

## v0.8.0 ~ v0.8.1 — UI Rendering

### v0.8.0 UI Architecture
- UIRenderer 抽象接口
- MockUI 零依赖后端
- UIContext 数据交换桥

### v0.8.1 ImGui DX11 Rendering
- Dear ImGui + DirectX11 + Win32 后端
- Frame Viewer（动态纹理上传）
- Detection Overlay（检测框绘制）
- Tracking Overlay（跟踪框 + ID）
- Analysis Panel
- 纯白简洁圆角 UI 风格

---

## v0.1 ~ v0.7 — Foundation

| 版本 | 内容 |
|------|------|
| v0.1.0 | 工程骨架 |
| v0.2.0 | Core Runtime Foundation |
| v0.3.0 | Capture 架构 |
| v0.3.1 | Windows ScreenCapture |
| v0.4.0 | Vision Pipeline |
| v0.4.1 | Runtime Frame Loop |
| v0.5.0 | Detection 架构 |
| v0.6.0 | Tracking 架构 |
| v0.7.0 | Analysis 架构 |
