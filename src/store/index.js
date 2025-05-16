import { defineStore } from 'pinia';

export const useAppStore = defineStore('app', {
  state: () => ({
    sidebar: {
      opened: true,
      withoutAnimation: false
    },
    device: 'desktop',
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
    selectedPort: '',
    connectionStatus: false,
    controlModes: [
      { value: 'speed', label: '速度控制' },
      { value: 'position', label: '位置控制' },
      { value: 'torque', label: '扭矩控制' }
    ]
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
    }
  }
});
