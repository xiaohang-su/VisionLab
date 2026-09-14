# VisionLab v1.1.0 发布说明

## 版本信息

- **版本**: v1.1.0
- **日期**: 2026-09-14
- **平台**: Windows 10/11 (64-bit)
- **类型**: Demo / 测试版

## 新增功能

### 平台主界面
- 顶部公告栏（时间、天气、公告）
- 左侧导航：AI检测 / 性能监控 / 模型管理 / 设置 / 关于
- 卡密登录页面
- 底部状态栏（官方群、版本、服务器状态）

### 独立悬浮窗
- 登录后自动弹出
- 无边框、可拖拽、纯白简洁风
- 左侧导航：🎯 AI / 📊 监控 / 🧠 模型 / ⚙️ 设置 / ℹ️ 说明
- 右侧实时画面 + 检测框 + 跟踪ID
- 独立 D3D11 渲染，不影响主窗口

### 运行时性能
- 分阶段性能计时（Capture/Vision/Detection/Tracking/Analysis/Render）
- FPS 统计
- 内存流量分析（Frame copy / Texture upload）
- 性能仪表盘（阶段耗时柱状图 + 瓶颈排序）

### 架构层
- AuthService（卡密验证 / 心跳 / 远程公告，预留微验对接）
- PlatformService（时间 / 天气 / 公告 / 弹窗 / 官方群）
- InferenceBackend（推理后端抽象，预留 ONNX/TensorRT/DirectML）
- InferenceDetector（Detector 接口 + 推理后端桥接）

## 系统要求

| 项目 | 最低要求 |
|------|----------|
| 系统 | Windows 10 64-bit 或更高 |
| 内存 | 2GB RAM |
| 显卡 | 支持 DirectX 11 |
| 运行库 | 无需安装（静态链接 MSVC runtime） |

## 使用方法

1. 解压 `VisionLab-1.1.0-Windows.zip`
2. 双击 `启动.bat` 或 `VisionLab.exe`
3. 主窗口打开后，Mock 模式自动登录
4. 悬浮窗自动弹出，显示实时画面和检测框

## 已知限制（Demo 版本）

- 当前为 Mock 数据（模拟画面、模拟检测、模拟跟踪）
- 卡密验证为 Mock 实现，未对接真实服务器
- 天气/时间/公告为 Mock 数据
- 推理后端为 Mock，未接入真实 ONNX 模型
- ScreenCapture 默认关闭（需要 MSVC + Windows SDK 编译）
- 窗口大小调整未处理（已知问题，后续版本修复）
- High DPI 未适配（4K 屏幕可能显示较小）

## 编译方法（开发者）

### 方式一：一键脚本
```
双击 scripts\build_release.bat
```

### 方式二：手动
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DVISIONLAB_UI_IMGUI=ON
cmake --build build --config Release
```

### 打包
```
双击 scripts\package_release.bat
```

## 文件结构

```
VisionLab-1.1.0-Windows/
├── VisionLab.exe      # 主程序
├── 启动.bat            # 一键启动
├── README.md          # 说明文档
└── docs/              # 开发文档
    ├── API_FREEZE.md
    ├── ARCHITECTURE.md
    ├── BUILD_WINDOWS.md
    ├── BUILD_LINUX.md
    └── TROUBLESHOOTING.md
```

## 反馈

测试中发现问题请记录：
- 复现步骤
- 截图
- 系统版本
- 显卡型号
