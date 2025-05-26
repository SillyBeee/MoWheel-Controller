# MoWheel Controller - 电机控制上位机

MoWheel Controller 是一款基于 USB-CAN 的电机控制上位机软件，旨在提供直观、高效的多种电机控制解决方案。该项目采用现代化技术栈，实现跨平台部署，支持多种电机型号的参数配置与运行控制。

## 技术栈

- **前端**：Tauri + Vue 3 + Vite
  - 响应式界面设计
  - 实时数据可视化
  - 主题定制支持

- **后端**：Rust
  - 高性能、安全的系统级编程
  - 与前端无缝集成
  - 跨平台兼容性

- **硬件通信**：C++ 库
  - PCAN-Basic API 集成
  - USB-CAN 接口通信
  - 实时电机控制和数据采集

## 核心功能

- 🔌 USB-CAN 设备自动识别与连接
- 🛠️ 多种电机参数实时配置
- 📊 运行数据实时监控与图表显示
- 🎮 简易电机控制面板
- 💾 配置导入/导出功能
- 🔄 固件更新支持

## 系统架构

```
┌─────────────────┐      ┌─────────────┐      ┌──────────────┐
│     Vue 前端     │ ←──→ │  Rust 后端   │ ←──→ │   C++ 库     │
│  (用户界面/交互)  │      │ (业务逻辑)   │      │ (硬件通信)   │
└─────────────────┘      └─────────────┘      └──────────────┘
                                                     ↓
                                              ┌──────────────┐
                                              │  USB-CAN设备  │
                                              └──────────────┘
                                                     ↓
                                              ┌──────────────┐
                                              │    电机设备    │
                                              └──────────────┘
```

## 快速开始

### 开发环境要求

- Node.js 16+
- Rust 1.65+
- MSVC (用于 C++ 构建)
- CMake 3.15+
- Git

### 安装依赖

1. 克隆仓库
   ```powershell
   git clone https://github.com/yourusername/mowheel-controller.git
   cd mowheel-controller
   ```

2. 安装前端依赖
   ```powershell
   npm install
   ```

3. 编译 C++ 库
   ```powershell
   cd lib_PCAN
   mkdir build
   cd build
   cmake -B . -S ..
   cmake --build . --config Release
   cd ../..
   ```

4. 开发模式启动
   ```powershell
   npm run tauri dev
   ```

5. 构建应用
   ```powershell
   npm run tauri build
   ```

### 硬件要求

- 支持的 USB-CAN 适配器:
  - PCAN-USB (PEAK-System)
  - 其他兼容 PCAN-Basic API 的设备

- 支持的电机类型:
  - 伺服电机
  - 步进电机
  - 直流电机
  - 自定义电机 (通过配置文件)

## 用户指南

### 连接设备

1. 将 USB-CAN 适配器连接到计算机
2. 打开软件，进入"系统设置"页面
3. 选择设备并点击"连接"

### 电机控制

1. 进入"电机控制"页面
2. 选择目标电机
3. 根据需要调整参数
4. 使用控制面板操作电机

### 数据监控

1. 进入"数据监控"页面
2. 选择要监控的参数
3. 数据将以实时图表形式显示

## 项目结构

- `src/` - 前端 Vue 代码
- `src-tauri/` - Rust 后端代码
- `lib_PCAN/` - C++ 硬件通信库
  - `include/` - 头文件
  - `src/` - 源文件
  - `lib/` - 编译后的库文件

## 贡献指南

欢迎对 MoWheel Controller 做出贡献！

## 许可证

本项目采用 MIT 许可证 - 查看 [LICENSE](LICENSE) 文件了解详情

## 联系方式

项目维护者 - [@SillyBeee](https://github.com/SillyBeee)

项目链接: [https://github.com/SillyBeee/mowheel-controller](https://github.com/yourusername/mowheel-controller)
