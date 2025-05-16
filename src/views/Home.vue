<template>
  <div class="home-container">
    <el-row :gutter="20">
      <!-- 连接状态卡片 -->
      <el-col :xs="24" :sm="12" :md="6">
        <div class="card-panel">
          <div class="card-panel-icon-wrapper" style="background: #ecf8ff;">
            <el-icon style="color: #409EFF" class="card-panel-icon">
              <Connection />
            </el-icon>
          </div>
          <div class="card-panel-description">
            <div class="card-panel-text">连接状态</div>
            <div class="card-panel-num">{{ connectionStatus ? '已连接' : '未连接' }}</div>
          </div>
        </div>
      </el-col>
      
      <!-- 串口选择 -->
      <el-col :xs="24" :sm="12" :md="6">
        <el-card class="port-card">
          <template #header>
            <div class="port-header">
              <span>串口设置</span>
              <el-button 
                :type="connectionStatus ? 'danger' : 'primary'" 
                size="small" 
                @click="toggleConnection"
              >
                {{ connectionStatus ? '断开' : '连接' }}
              </el-button>
            </div>
          </template>
          <el-select 
            v-model="selectedPort" 
            placeholder="选择串口" 
            size="large" 
            :disabled="connectionStatus"
            style="width: 100%;"
          >
            <el-option
              v-for="port in serialPorts"
              :key="port"
              :label="port"
              :value="port"
            ></el-option>
          </el-select>
        </el-card>
      </el-col>
      
      <!-- 电机数量指示 -->
      <el-col :xs="24" :sm="12" :md="6">
        <div class="card-panel">
          <div class="card-panel-icon-wrapper" style="background: #f0f9eb;">
            <el-icon style="color: #67C23A" class="card-panel-icon">
              <DCaret />
            </el-icon>
          </div>
          <div class="card-panel-description">
            <div class="card-panel-text">电机数量</div>
            <div class="card-panel-num">{{ motors.length }}</div>
          </div>
        </div>
      </el-col>
      
      <!-- 运行状态指示 -->
      <el-col :xs="24" :sm="12" :md="6">
        <div class="card-panel">
          <div class="card-panel-icon-wrapper" style="background: #fdf6ec;">
            <el-icon style="color: #E6A23C" class="card-panel-icon">
              <Warning />
            </el-icon>
          </div>
          <div class="card-panel-description">
            <div class="card-panel-text">运行电机</div>
            <div class="card-panel-num">{{ runningMotorsCount }}</div>
          </div>
        </div>
      </el-col>
    </el-row>

    <!-- 电机控制部分 -->
    <el-card shadow="hover" header="电机控制" style="margin-top: 20px;">
      <el-row :gutter="20">
        <!-- 电机选择 -->
        <el-col :span="24">
          <el-radio-group v-model="selectedMotorId" size="large" class="motor-selector">
            <el-radio-button 
              v-for="motor in motors" 
              :key="motor.id" 
              :label="motor.id"
            >
              {{ motor.name }} ({{ motor.type }})
            </el-radio-button>
          </el-radio-group>
        </el-col>
      </el-row>
      
      <el-divider content-position="left">控制模式</el-divider>
      
      <!-- 电机类型和控制模式 -->
      <el-row :gutter="20">
        <el-col :span="12">
          <el-form-item label="电机类型">
            <el-select v-model="currentMotor.type" placeholder="选择电机类型">
              <el-option
                v-for="type in motorTypes"
                :key="type.value"
                :label="type.label"
                :value="type.value"
              ></el-option>
            </el-select>
          </el-form-item>
        </el-col>
        <el-col :span="12">
          <el-form-item label="控制模式">
            <el-select v-model="currentMotor.controlMode" placeholder="选择控制模式">
              <el-option
                v-for="mode in controlModes"
                :key="mode.value"
                :label="mode.label"
                :value="mode.value"
              ></el-option>
            </el-select>
          </el-form-item>
        </el-col>
      </el-row>
      
      <!-- 控制参数设置 -->
      <el-row :gutter="20" style="margin-top: 20px;">
        <el-col :span="24">
          <!-- 速度控制模式 -->
          <div v-if="currentMotor.controlMode === 'speed'">
            <div class="control-item">
              <span class="label">目标速度 (RPM):</span>
              <el-slider
                v-model="currentMotor.targetSpeed"
                :min="0"
                :max="5000"
                :step="10"
                :format-tooltip="value => `${value} RPM`"
                show-input
              />
            </div>
          </div>
          
          <!-- 位置控制模式 -->
          <div v-if="currentMotor.controlMode === 'position'">
            <div class="control-item">
              <span class="label">目标位置 (度):</span>
              <el-slider
                v-model="currentMotor.targetPosition"
                :min="0"
                :max="360"
                :step="1"
                :format-tooltip="value => `${value}°`"
                show-input
              />
            </div>
          </div>
          
          <!-- 扭矩控制模式 -->
          <div v-if="currentMotor.controlMode === 'torque'">
            <div class="control-item">
              <span class="label">目标扭矩 (N·m):</span>
              <el-slider
                v-model="currentMotor.targetTorque"
                :min="0"
                :max="20"
                :step="0.1"
                :format-tooltip="value => `${value} N·m`"
                show-input
              />
            </div>
          </div>
        </el-col>
      </el-row>      <!-- 控制按钮 -->
      <el-row style="margin-top: 20px;">
        <el-col :span="24" class="button-group">
          <el-button 
            type="success" 
            :icon="currentMotor.running ? 'VideoPause' : 'VideoPlay'"
            :disabled="!connectionStatus"
            @click="toggleMotorRunning"
          >
            {{ currentMotor.running ? '停止' : '运行' }}
          </el-button>
          
          <el-button 
            type="primary" 
            icon="Position" 
            :disabled="!connectionStatus"
            @click="resetPosition"
          >
            回零
          </el-button>
          
          <el-button 
            type="warning" 
            icon="RefreshRight" 
            :disabled="!connectionStatus"
            @click="calibrateMotor"
          >
            校准
          </el-button>
          
          <el-button 
            type="danger" 
            icon="WarningFilled"
            :disabled="!connectionStatus" 
            @click="emergencyStop"
          >
            紧急停止
          </el-button>
        </el-col>
      </el-row>
    </el-card>
    
    <!-- 状态参数展示 -->
    <el-row :gutter="20" style="margin-top: 20px;">
      <el-col :span="12">
        <el-card shadow="hover" :header="`${currentMotor.name} 状态信息`">
          <el-row :gutter="15">
            <el-col :span="12">
              <el-statistic title="当前速度" :value="currentMotor.speed">
                <template #suffix>
                  <span>RPM</span>
                </template>
              </el-statistic>
            </el-col>
            <el-col :span="12">
              <el-statistic title="当前扭矩" :value="currentMotor.torque">
                <template #suffix>
                  <span>N·m</span>
                </template>
              </el-statistic>
            </el-col>
          </el-row>
          
          <el-divider />
          
          <el-row :gutter="15">
            <el-col :span="12">
              <el-statistic title="当前位置" :value="currentMotor.position">
                <template #suffix>
                  <span>°</span>
                </template>
              </el-statistic>
            </el-col>
            <el-col :span="12">
              <el-statistic title="电机温度" :value="currentMotor.temperature">
                <template #suffix>
                  <span>°C</span>
                </template>
              </el-statistic>
            </el-col>
          </el-row>
          
          <el-divider />
          
          <el-row :gutter="15">
            <el-col :span="12">
              <el-statistic title="电机电压" :value="currentMotor.voltage">
                <template #suffix>
                  <span>V</span>
                </template>
              </el-statistic>
            </el-col>
            <el-col :span="12">
              <el-statistic title="电机电流" :value="currentMotor.current">
                <template #suffix>
                  <span>A</span>
                </template>
              </el-statistic>
            </el-col>
          </el-row>
        </el-card>
      </el-col>
      
      <!-- 电机电气参数图表 -->
      <el-col :span="12">
        <el-card shadow="hover" header="电气参数">
          <div ref="voltageChartRef" style="height: 300px;"></div>
        </el-card>
      </el-col>
    </el-row>
    
    <!-- 系统日志 -->
    <el-row :gutter="20" style="margin-top: 20px;">
      <el-col :span="24">
        <el-card shadow="hover" header="系统日志">
          <el-table :data="logData" stripe style="width: 100%">
            <el-table-column prop="time" label="时间" width="180" />
            <el-table-column prop="level" label="级别" width="100">
              <template #default="scope">
                <el-tag :type="getTagType(scope.row.level)">{{ scope.row.level }}</el-tag>
              </template>
            </el-table-column>
            <el-table-column prop="message" label="消息" />
          </el-table>
        </el-card>
      </el-col>
    </el-row>
  </div>
</template>

<script setup>
import { ref, onMounted, computed, onBeforeUnmount } from 'vue';
import { useAppStore } from '../store';
import * as echarts from 'echarts';
import { ElMessage } from 'element-plus';
import { 
  Connection, 
  Monitor, 
  DataLine, 
  Warning, 
  VideoPlay, 
  VideoPause,
  Position,
  RefreshRight,
  WarningFilled,
  DCaret
} from '@element-plus/icons-vue';

const appStore = useAppStore();
const voltageChartRef = ref(null);
let voltageChart = null;
let chartTimer = null;

// 从store获取状态
const motors = computed(() => appStore.motors);
const connectionStatus = computed(() => appStore.connectionStatus);
const serialPorts = computed(() => appStore.serialPorts);
const selectedPort = computed({
  get: () => appStore.selectedPort,
  set: (val) => appStore.setSelectedPort(val)
});
const motorTypes = computed(() => appStore.motorTypes);
const controlModes = computed(() => appStore.controlModes);

// 选中的电机ID和当前电机对象
const selectedMotorId = ref('1');
const currentMotor = computed(() => {
  return motors.value.find(motor => motor.id === selectedMotorId.value) || motors.value[0];
});

// 计算运行中的电机数量
const runningMotorsCount = computed(() => {
  return motors.value.filter(motor => motor.running).length;
});

// 系统日志数据
const logData = ref([
  { time: '2025-05-16 01:15:23', level: 'info', message: '系统启动成功' },
  { time: '2025-05-16 01:15:25', level: 'info', message: '扫描可用串口：COM1, COM2, COM3, COM4' },
  { time: '2025-05-16 01:15:30', level: 'warning', message: '未检测到连接的设备' }
]);

// 获取日志级别对应的标签类型
const getTagType = (level) => {
  switch (level.toLowerCase()) {
    case 'info': return '';
    case 'warning': return 'warning';
    case 'error': return 'danger';
    case 'success': return 'success';
    default: return 'info';
  }
};

// 切换连接状态
const toggleConnection = () => {
  if (connectionStatus.value) {
    // 断开连接
    appStore.setConnectionStatus(false);
    addLog('info', '已断开串口连接');
    // 这里应该调用 Tauri API 来断开串口连接
  } else {
    // 建立连接
    if (selectedPort.value) {
      appStore.setConnectionStatus(true);
      addLog('success', `已连接到串口 ${selectedPort.value}`);
      // 这里应该调用 Tauri API 来建立串口连接
    } else {
      ElMessage.warning('请先选择串口');
      addLog('warning', '未选择串口，无法建立连接');
    }
  }
};

// 添加日志
const addLog = (level, message) => {
  const now = new Date();
  const timeStr = `${now.getFullYear()}-${String(now.getMonth() + 1).padStart(2, '0')}-${String(now.getDate()).padStart(2, '0')} ${String(now.getHours()).padStart(2, '0')}:${String(now.getMinutes()).padStart(2, '0')}:${String(now.getSeconds()).padStart(2, '0')}`;
  
  logData.value.unshift({
    time: timeStr,
    level,
    message
  });
  
  // 保持日志数量合理
  if (logData.value.length > 100) {
    logData.value = logData.value.slice(0, 100);
  }
};

// 切换电机运行状态
const toggleMotorRunning = () => {
  if (!connectionStatus.value) {
    ElMessage.warning('请先连接串口');
    return;
  }
  
  const currentMotorIndex = motors.value.findIndex(motor => motor.id === selectedMotorId.value);
  if (currentMotorIndex !== -1) {
    // 更新电机状态
    const newStatus = !motors.value[currentMotorIndex].running;
    motors.value[currentMotorIndex].running = newStatus;
    
    ElMessage({
      message: newStatus ? `${motors.value[currentMotorIndex].name} 启动成功` : `${motors.value[currentMotorIndex].name} 已停止`,
      type: newStatus ? 'success' : 'info'
    });
    
    addLog(newStatus ? 'success' : 'info', 
      `${motors.value[currentMotorIndex].name} ${newStatus ? '启动' : '停止'}`);
  }
};

// 回零操作
const resetPosition = () => {
  if (!connectionStatus.value) {
    ElMessage.warning('请先连接串口');
    return;
  }
  
  const currentMotorIndex = motors.value.findIndex(motor => motor.id === selectedMotorId.value);
  if (currentMotorIndex !== -1) {
    motors.value[currentMotorIndex].position = 0;
    
    ElMessage({
      message: `${motors.value[currentMotorIndex].name} 位置已重置为0`,
      type: 'success'
    });
    
    addLog('success', `${motors.value[currentMotorIndex].name} 位置已重置为0`);
  }
};

// 校准电机
const calibrateMotor = () => {
  if (!connectionStatus.value) {
    ElMessage.warning('请先连接串口');
    return;
  }
  
  ElMessage({
    message: `${currentMotor.value.name} 校准成功`,
    type: 'success'
  });
  
  addLog('success', `${currentMotor.value.name} 校准成功`);
};

// 紧急停止
const emergencyStop = () => {
  if (!connectionStatus.value) {
    ElMessage.warning('请先连接串口');
    return;
  }
  
  // 停止所有电机
  motors.value.forEach(motor => {
    motor.running = false;
    motor.speed = 0;
    motor.torque = 0;
  });
  
  ElMessage({
    message: '紧急停止已执行，所有电机已停止',
    type: 'warning'
  });
  
  addLog('warning', '紧急停止已执行，所有电机已停止');
};

// 初始化电压电流图表
const initVoltageChart = () => {
  if (voltageChartRef.value) {
    voltageChart = echarts.init(voltageChartRef.value);
    
    const option = {
      tooltip: {
        trigger: 'axis',
        axisPointer: {
          type: 'cross',
          label: {
            backgroundColor: '#6a7985'
          }
        }
      },
      legend: {
        data: ['电压', '电流']
      },
      grid: {
        left: '3%',
        right: '4%',
        bottom: '3%',
        containLabel: true
      },
      xAxis: [
        {
          type: 'category',
          boundaryGap: false,
          data: []
        }
      ],
      yAxis: [
        {
          type: 'value',
          name: '电压 (V)',
          position: 'left'
        },
        {
          type: 'value',
          name: '电流 (A)',
          position: 'right'
        }
      ],
      series: [
        {
          name: '电压',
          type: 'line',
          smooth: true,
          emphasis: {
            focus: 'series'
          },
          yAxisIndex: 0,
          data: []
        },
        {
          name: '电流',
          type: 'line',
          smooth: true,
          emphasis: {
            focus: 'series'
          },
          yAxisIndex: 1,
          data: []
        }
      ]
    };
    
    voltageChart.setOption(option);
    
    // 模拟数据更新
    updateChartData();
    chartTimer = setInterval(updateChartData, 3000);
  }
};

// 更新图表数据
const updateChartData = () => {
  if (!voltageChart) return;
  
  const now = new Date();
  const timeStr = `${now.getHours()}:${now.getMinutes()}:${now.getSeconds()}`;
  
  voltageChart.setOption({
    xAxis: {
      data: function(old) {
        if (old.length >= 10) {
          old.shift();
        }
        old.push(timeStr);
        return old;
      }
    },
    series: [
      {
        name: '电压',
        data: function(old) {
          if (old.length >= 10) {
            old.shift();
          }
          // 使用当前选中电机的电压数据
          old.push(currentMotor.value.voltage);
          return old;
        }
      },
      {
        name: '电流',
        data: function(old) {
          if (old.length >= 10) {
            old.shift();
          }
          // 使用当前选中电机的电流数据
          old.push(currentMotor.value.current);
          return old;
        }
      }
    ]
  });
};

onMounted(() => {
  initVoltageChart();
  window.addEventListener('resize', handleResize);
  
  // 模拟获取串口列表
  appStore.setSerialPorts(['COM1', 'COM2', 'COM3', 'COM4']);
  
  // 模拟更新电机状态
  setInterval(() => {
    // 更新所有电机的状态
    motors.value.forEach((motor, index) => {
      if (motor.running) {
        motor.speed = parseInt(Math.random() * 1000 + 2000);
        motor.torque = parseFloat((Math.random() * 5 + 5).toFixed(2));
        motor.temperature = parseInt(Math.random() * 20 + 40);
        motor.voltage = parseFloat((Math.random() * 10 + 220).toFixed(1));
        motor.current = parseFloat((Math.random() * 2 + 1).toFixed(2));
        motor.position = (motor.position + 10) % 360;
      }
    });
    
    updateChartData();
  }, 1000);
});

onBeforeUnmount(() => {
  window.removeEventListener('resize', handleResize);
  if (chartTimer) clearInterval(chartTimer);
  if (voltageChart) voltageChart.dispose();
});

const handleResize = () => {
  if (voltageChart) voltageChart.resize();
};
</script>

<style scoped>
.home-container {
  padding: 10px;
}

.card-panel {
  height: 108px;
  cursor: pointer;
  position: relative;
  overflow: hidden;
  color: #666;
  background: #fff;
  box-shadow: 0 2px 12px 0 rgba(0, 0, 0, 0.1);
  border-radius: 4px;
  margin-bottom: 20px;
  transition: all 0.3s;
}

.card-panel:hover {
  box-shadow: 0 4px 16px 0 rgba(0, 0, 0, 0.2);
  transform: translateY(-5px);
}

.card-panel-icon-wrapper {
  float: left;
  margin: 14px 0 0 14px;
  padding: 16px;
  transition: all 0.3s;
  border-radius: 6px;
}

.card-panel-icon {
  float: left;
  font-size: 48px;
}

.card-panel-description {
  float: right;
  font-weight: bold;
  margin: 26px 15px 0 0;
  text-align: right;
}

.card-panel-text {
  line-height: 18px;
  color: rgba(0, 0, 0, 0.45);
  font-size: 16px;
  margin-bottom: 12px;
}

.card-panel-num {
  font-size: 20px;
}
</style>
