import { defineStore } from 'pinia';

export const useAppStore = defineStore('app', {
  state: () => ({
    sidebar: {
      opened: true,
      withoutAnimation: false
    },
    device: 'desktop',
    theme: {
      mode: 'light', // 'light', 'dark', 'system'
      primaryColor: '#409EFF'
    },
    motors: [
      {
        id: '1',
        name: '电机1',
        type: 'DM4310',
        connected: false,
        running: false,
        speed: 0,
        position: 0,
        torque: 0,
        temperature: 0,
        voltage: 0,
        current: 0,
        controlMode: 'speed', // 'speed', 'position', 'torque'
        pid: {
          p: 0.8,
          i: 0.05,
          d: 0.1
        },
        targetSpeed: 0,
        targetPosition: 0,
        targetTorque: 0
      },
      {
        id: '2',
        name: '电机2',
        type: 'GM6020',
        connected: false,
        running: false,
        speed: 0,
        position: 0,
        torque: 0,
        temperature: 0,
        voltage: 0,
        current: 0,
        controlMode: 'speed',
        pid: {
          p: 0.8,
          i: 0.05,
          d: 0.1
        },
        targetSpeed: 0,
        targetPosition: 0,
        targetTorque: 0
      },
      {
        id: '3',
        name: '电机3',
        type: 'M3508',
        connected: false,
        running: false,
        speed: 0,
        position: 0,
        torque: 0,
        temperature: 0,
        voltage: 0,
        current: 0,
        controlMode: 'speed',
        pid: {
          p: 0.8,
          i: 0.05,
          d: 0.1
        },
        targetSpeed: 0,
        targetPosition: 0,
        targetTorque: 0
      },
      {
        id: '4',
        name: '电机4',
        type: 'DM4310',
        connected: false,
        running: false,
        speed: 0,
        position: 0,
        torque: 0,
        temperature: 0,
        voltage: 0,
        current: 0,
        controlMode: 'speed',
        pid: {
          p: 0.8,
          i: 0.05,
          d: 0.1
        },
        targetSpeed: 0,
        targetPosition: 0,
        targetTorque: 0
      },
      {
        id: '5',
        name: '电机5',
        type: 'GM6020',
        connected: false,
        running: false,
        speed: 0,
        position: 0,
        torque: 0,
        temperature: 0,
        voltage: 0,
        current: 0,
        controlMode: 'speed',
        pid: {
          p: 0.8,
          i: 0.05,
          d: 0.1
        },
        targetSpeed: 0,
        targetPosition: 0,
        targetTorque: 0
      },
      {
        id: '6',
        name: '电机6',
        type: 'M3508',
        connected: false,
        running: false,
        speed: 0,
        position: 0,
        torque: 0,
        temperature: 0,
        voltage: 0,
        current: 0,
        controlMode: 'speed',
        pid: {
          p: 0.8,
          i: 0.05,
          d: 0.1
        },
        targetSpeed: 0,
        targetPosition: 0,
        targetTorque: 0
      },
      {
        id: '7',
        name: '电机7',
        type: 'DM4310',
        connected: false,
        running: false,
        speed: 0,
        position: 0,
        torque: 0,
        temperature: 0,
        voltage: 0,
        current: 0,
        controlMode: 'speed',
        pid: {
          p: 0.8,
          i: 0.05,
          d: 0.1
        },
        targetSpeed: 0,
        targetPosition: 0,
        targetTorque: 0
      }
    ],
    motorTypes: [
      { value: 'DM4310', label: 'DM4310' },
      { value: 'GM6020', label: 'GM6020' },
      { value: 'M3508', label: 'M3508' }
    ],
    selectedMotor: '1',    
    serialPorts: [],
    availablePorts: [], // 存储从系统检测到的可用串口详细信息
    selectedPort: '',
    connectionStatus: false,
    connectionError: null,
    baudRate: 9600, // 默认波特率
    controlModes: [
      { value: 'speed', label: '速度控制' },
      { value: 'position', label: '位置控制' },
      { value: 'torque', label: '扭矩控制' }
    ],
    motorStatusPolling: null, // 用于存储轮询定时器
    isPolling: false // 是否正在轮询电机状态
  }),
  
  actions: {
    toggleSidebar() {
      this.sidebar.opened = !this.sidebar.opened;
      this.sidebar.withoutAnimation = false;
    },
    closeSidebar({ withoutAnimation }) {
      this.sidebar.opened = false;
      this.sidebar.withoutAnimation = withoutAnimation;
    },
    setDevice(device) {
      this.device = device;
    },
    updateMotorStatus(status) {
      this.motorStatus = { ...this.motorStatus, ...status };
    },
    setSelectedMotor(motorId) {
      this.selectedMotor = motorId;
    },
    setSerialPorts(ports) {
      this.serialPorts = ports;
    },
    setSelectedPort(port) {
      this.selectedPort = port;
    },
    setConnectionStatus(status) {
      this.connectionStatus = status;
      
      // 当连接状态改变时，开始或停止轮询
      if (status) {
        this.startMotorStatusPolling();
      } else {
        this.stopMotorStatusPolling();
      }
    },
    setConnectionError(error) {
      this.connectionError = error;
    },
    setAvailablePorts(ports) {
      this.availablePorts = ports;
      // 从详细的端口信息中提取端口名称，给UI使用
      this.serialPorts = ports.map(port => port.name);
    },
    setBaudRate(rate) {
      this.baudRate = rate;
    },
    async refreshSerialPorts() {
      try {
        const { invoke } = await import('@tauri-apps/api');
        const ports = await invoke('get_available_ports');
        this.setAvailablePorts(ports);
        return ports;
      } catch (error) {
        console.error('获取串口列表失败:', error);
        this.setConnectionError('获取串口列表失败: ' + error.toString());
        return [];
      }
    },
    async connectToPort(portName = this.selectedPort, baudRate = this.baudRate) {
      if (!portName) {
        this.setConnectionError('未选择串口');
        return false;
      }

      try {
        const { invoke } = await import('@tauri-apps/api');
        await invoke('connect_to_port', { portName, baudRate });
        this.setConnectionStatus(true);
        this.setConnectionError(null);
        return true;
      } catch (error) {
        console.error('连接串口失败:', error);
        this.setConnectionError('连接串口失败: ' + error.toString());
        this.setConnectionStatus(false);
        return false;
      }
    },
    async disconnectPort() {
      try {
        const { invoke } = await import('@tauri-apps/api');
        await invoke('disconnect_port');
        this.setConnectionStatus(false);
        this.setConnectionError(null);
        return true;
      } catch (error) {
        console.error('断开串口连接失败:', error);
        this.setConnectionError('断开串口连接失败: ' + error.toString());
        return false;
      }
    },
    async checkPortConnection() {
      try {
        const { invoke } = await import('@tauri-apps/api');
        const isConnected = await invoke('is_port_connected');
        this.setConnectionStatus(isConnected);
        return isConnected;
      } catch (error) {
        console.error('检查串口连接状态失败:', error);
        this.setConnectionStatus(false);
        return false;
      }
    },
    async sendMotorCommand(motorId, commandType, value) {
      if (!this.connectionStatus) {
        this.setConnectionError('未连接到串口');
        return false;
      }

      try {
        const { invoke } = await import('@tauri-apps/api');
        const result = await invoke('send_motor_command', {
          command: {
            motor_id: parseInt(motorId),
            command_type: commandType,
            value: parseFloat(value)
          }
        });
        
        console.log('电机命令发送结果:', result);
        return true;
      } catch (error) {
        console.error('发送电机命令失败:', error);
        this.setConnectionError('发送电机命令失败: ' + error.toString());
        return false;
      }
    },
    async readMotorStatus(motorId) {
      if (!this.connectionStatus) {
        return null;
      }

      try {
        const { invoke } = await import('@tauri-apps/api');
        const status = await invoke('read_motor_status', {
          motorId: parseInt(motorId)
        });
        
        // 更新电机状态
        const motorIndex = this.motors.findIndex(motor => motor.id === status.id);
        if (motorIndex !== -1) {
          this.motors[motorIndex] = {
            ...this.motors[motorIndex],
            speed: status.speed,
            position: status.position,
            torque: status.torque,
            temperature: status.temperature,
            voltage: status.voltage,
            current: status.current,
            connected: status.connected,
            running: status.running
          };
        }
        
        return status;
      } catch (error) {
        console.error('读取电机状态失败:', error);
        return null;
      }
    },
    startMotorStatusPolling() {
      // 如果已经在轮询，不重复启动
      if (this.isPolling) return;
      
      this.isPolling = true;
      
      // 每500ms轮询一次所有电机状态
      this.motorStatusPolling = setInterval(async () => {
        if (!this.connectionStatus) {
          this.stopMotorStatusPolling();
          return;
        }
        
        // 轮询所有电机状态
        for (const motor of this.motors) {
          await this.readMotorStatus(motor.id);
        }
      }, 500);
    },
    stopMotorStatusPolling() {
      if (this.motorStatusPolling) {
        clearInterval(this.motorStatusPolling);
        this.motorStatusPolling = null;
      }
      this.isPolling = false;
    },
    async startMotor(motorId) {
      const result = await this.sendMotorCommand(motorId, 'start', 0);
      if (result) {
        // 更新本地电机状态
        const motorIndex = this.motors.findIndex(motor => motor.id === motorId);
        if (motorIndex !== -1) {
          this.motors[motorIndex].running = true;
        }
      }
      return result;
    },
    async stopMotor(motorId) {
      const result = await this.sendMotorCommand(motorId, 'stop', 0);
      if (result) {
        // 更新本地电机状态
        const motorIndex = this.motors.findIndex(motor => motor.id === motorId);
        if (motorIndex !== -1) {
          this.motors[motorIndex].running = false;
          this.motors[motorIndex].speed = 0;
        }
      }
      return result;
    },
    async setMotorSpeed(motorId, speed) {
      return await this.sendMotorCommand(motorId, 'set_speed', speed);
    },
    async setMotorPosition(motorId, position) {
      return await this.sendMotorCommand(motorId, 'set_position', position);
    },
    async setMotorTorque(motorId, torque) {
      return await this.sendMotorCommand(motorId, 'set_torque', torque);
    },
    setTheme(mode) {
      this.theme.mode = mode;
      localStorage.setItem('theme-mode', mode);
      this.applyTheme();
    },
    setPrimaryColor(color) {
      this.theme.primaryColor = color;
      localStorage.setItem('theme-primary-color', color);
      document.documentElement.style.setProperty('--el-color-primary', color);
    },    
    applyTheme() {
      const mode = this.theme.mode;
      if (mode === 'system') {
        const prefersDark = window.matchMedia('(prefers-color-scheme: dark)').matches;
        document.documentElement.setAttribute('data-theme', prefersDark ? 'dark' : 'light');
        document.documentElement.classList.toggle('dark-mode', prefersDark);
        // 设置Element Plus的主题
        document.documentElement.style.setProperty('--el-bg-color', prefersDark ? '#1d1e1f' : '#ffffff');
        document.documentElement.style.setProperty('--el-text-color-primary', prefersDark ? '#ffffff' : '#303133');
      } else {
        document.documentElement.setAttribute('data-theme', mode);
        document.documentElement.classList.toggle('dark-mode', mode === 'dark');
        // 设置Element Plus的主题
        document.documentElement.style.setProperty('--el-bg-color', mode === 'dark' ? '#1d1e1f' : '#ffffff');
        document.documentElement.style.setProperty('--el-text-color-primary', mode === 'dark' ? '#ffffff' : '#303133');
      }
    },
    initTheme() {
      const savedMode = localStorage.getItem('theme-mode') || 'light';
      const savedColor = localStorage.getItem('theme-primary-color') || '#409EFF';
      this.theme.mode = savedMode;
      this.theme.primaryColor = savedColor;
      
      // 应用保存的主题
      this.applyTheme();
      document.documentElement.style.setProperty('--el-color-primary', savedColor);
      
      // 如果是系统主题，检查系统当前偏好
      if (savedMode === 'system') {
        const prefersDark = window.matchMedia('(prefers-color-scheme: dark)').matches;
        document.documentElement.classList.toggle('dark-mode', prefersDark);
        document.documentElement.setAttribute('data-theme', prefersDark ? 'dark' : 'light');
      }
      
      // 监听系统主题变化
      const mql = window.matchMedia('(prefers-color-scheme: dark)');
      mql.addEventListener('change', (e) => {
        if (this.theme.mode === 'system') {
          document.documentElement.classList.toggle('dark-mode', e.matches);
          document.documentElement.setAttribute('data-theme', e.matches ? 'dark' : 'light');
        }
      });
      
      console.log(`应用主题: ${savedMode}, 主题色: ${savedColor}`);
    }
  }
});
