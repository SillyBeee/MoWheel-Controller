<template>
  <div class="motor-control-container">
    <el-row :gutter="20">
      <el-col :span="24">
        <!-- 电机状态概览 -->
        <el-card shadow="hover" header="电机状态概览">
          <el-table :data="motors" stripe style="width: 100%">
            <el-table-column prop="id" label="ID" width="60" />
            <el-table-column prop="name" label="名称" width="100" />
            <el-table-column prop="type" label="型号" width="100" />
            <el-table-column prop="controlMode" label="控制模式" width="120">
              <template #default="scope">
                {{ getControlModeLabel(scope.row.controlMode) }}
              </template>
            </el-table-column>
            <el-table-column prop="speed" label="速度(RPM)" width="120" />
            <el-table-column prop="position" label="位置(°)" width="120" />
            <el-table-column prop="torque" label="扭矩(N·m)" width="120" />
            <el-table-column prop="temperature" label="温度(°C)" width="120" />
            <el-table-column prop="running" label="状态" width="100">
              <template #default="scope">
                <el-tag :type="scope.row.running ? 'success' : 'info'">
                  {{ scope.row.running ? '运行中' : '停止' }}
                </el-tag>
              </template>
            </el-table-column>
            <el-table-column label="操作" width="180">
              <template #default="scope">
                <el-button 
                  :type="scope.row.running ? 'danger' : 'success'" 
                  size="small"
                  @click="toggleMotorStatus(scope.row)"
                  :disabled="!connectionStatus"
                >
                  {{ scope.row.running ? '停止' : '启动' }}
                </el-button>
                <el-button 
                  type="primary" 
                  size="small"
                  @click="editMotorSettings(scope.row)"
                >
                  设置
                </el-button>
              </template>
            </el-table-column>
          </el-table>
        </el-card>
      </el-col>
    </el-row>
    
    <el-row :gutter="20" style="margin-top: 20px;">
      <el-col :span="16">
        <!-- 电机详细控制卡片 -->
        <el-card shadow="hover" :header="`电机控制面板 - ${currentMotor.name} (${currentMotor.type})`">
          <!-- 电机选择 -->
          <el-row style="margin-bottom: 20px;">
            <el-col :span="24">
              <el-radio-group v-model="selectedMotorId" size="large">
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
          
          <!-- 控制模式选择 -->
          <el-row>
            <el-col :span="12">
              <el-select v-model="currentMotor.type" placeholder="选择电机型号" style="width: 90%; margin-bottom: 15px;">
                <el-option
                  v-for="type in motorTypes"
                  :key="type.value"
                  :label="type.label"
                  :value="type.value"
                ></el-option>
              </el-select>
            </el-col>
            <el-col :span="12">
              <el-select v-model="currentMotor.controlMode" placeholder="选择控制模式" style="width: 90%; margin-bottom: 15px;">
                <el-option
                  v-for="mode in controlModes"
                  :key="mode.value"
                  :label="mode.label"
                  :value="mode.value"
                ></el-option>
              </el-select>
            </el-col>
          </el-row>
          
          <el-row style="margin-bottom: 20px;">
            <el-col :span="24">
              <!-- 速度控制模式 -->
              <div v-if="currentMotor.controlMode === 'speed'" class="control-panel">
                <div class="control-item">
                  <span class="label">目标速度 (RPM):</span>
                  <el-slider
                    v-model="currentMotor.targetSpeed"
                    :min="0"
                    :max="getMaxSpeedByType(currentMotor.type)"
                    :step="10"
                    :format-tooltip="value => `${value} RPM`"
                    show-input
                  />
                </div>
                
                <div class="control-item">
                  <span class="label">加速时间 (ms):</span>
                  <el-slider
                    v-model="accelerationTime"
                    :min="0"
                    :max="5000"
                    :step="100"
                    :format-tooltip="value => `${value} ms`"
                    show-input
                  />
                </div>
              </div>
              
              <!-- 扭矩控制模式 -->
              <div v-if="currentMotor.controlMode === 'torque'" class="control-panel">
                <div class="control-item">
                  <span class="label">目标扭矩 (N·m):</span>
                  <el-slider
                    v-model="currentMotor.targetTorque"
                    :min="0"
                    :max="getMaxTorqueByType(currentMotor.type)"
                    :step="0.1"
                    :format-tooltip="value => `${value} N·m`"
                    show-input
                  />
                </div>
                
                <div class="control-item">
                  <span class="label">扭矩增益:</span>
                  <el-slider
                    v-model="torqueGain"
                    :min="0"
                    :max="100"
                    :step="1"
                    :format-tooltip="value => `${value}%`"
                    show-input
                  />
                </div>
              </div>
              
              <!-- 位置控制模式 -->
              <div v-if="currentMotor.controlMode === 'position'" class="control-panel">
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
                
                <div class="control-item">
                  <span class="label">位置增益 (P):</span>
                  <el-slider
                    v-model="positionGain"
                    :min="0"
                    :max="100"
                    :step="1"
                    show-input
                  />
                </div>
              </div>
            </el-col>
          </el-row>
            <el-divider content-position="left">控制操作</el-divider>
          
          <!-- 控制按钮 -->
          <el-row>
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
              
              <el-button 
                type="info" 
                icon="SetUp" 
                @click="applyToAllMotors"
              >
                应用到所有电机
              </el-button>
            </el-col>
          </el-row>
        </el-card>
        
        <!-- 控制历史记录 -->
        <el-card shadow="hover" header="控制历史记录" style="margin-top: 20px;">
          <el-table :data="controlHistory" stripe style="width: 100%">
            <el-table-column prop="time" label="时间" width="180" />
            <el-table-column prop="motor" label="电机" width="100" />
            <el-table-column prop="action" label="操作" width="120" />
            <el-table-column prop="value" label="设定值" />
            <el-table-column prop="result" label="执行结果" width="120">
              <template #default="scope">
                <el-tag 
                  :type="scope.row.result === '成功' ? 'success' : 'danger'"
                >
                  {{ scope.row.result }}
                </el-tag>
              </template>
            </el-table-column>
          </el-table>
        </el-card>
      </el-col>
        <!-- 电机状态和参数 -->
      <el-col :span="8">
        <el-card shadow="hover" :header="`${currentMotor.name} 状态`">
          <div class="status-panel">
            <div class="status-item">
              <el-icon :size="24" color="#409EFF">
                <SemiSelect />
              </el-icon>
              <span class="label">型号:</span>
              <span class="value">{{ currentMotor.type }}</span>
            </div>
            
            <div class="status-item">
              <el-icon :size="24" color="#67C23A">
                <Guide />
              </el-icon>
              <span class="label">当前速度:</span>
              <span class="value">{{ currentMotor.speed }} RPM</span>
            </div>
            
            <div class="status-item">
              <el-icon :size="24" color="#E6A23C">
                <Operation />
              </el-icon>
              <span class="label">当前扭矩:</span>
              <span class="value">{{ currentMotor.torque }} N·m</span>
            </div>
            
            <div class="status-item">
              <el-icon :size="24" color="#F56C6C">
                <Aim />
              </el-icon>
              <span class="label">当前位置:</span>
              <span class="value">{{ currentMotor.position }}°</span>
            </div>
            
            <div class="status-item">
              <el-icon :size="24" color="#409EFF">
                <Monitor />
              </el-icon>
              <span class="label">电压:</span>
              <span class="value">{{ currentMotor.voltage }} V</span>
            </div>
            
            <div class="status-item">
              <el-icon :size="24" color="#67C23A">
                <DataLine />
              </el-icon>
              <span class="label">电流:</span>
              <span class="value">{{ currentMotor.current }} A</span>
            </div>
            
            <div class="status-item">
              <el-icon :size="24" color="#E6A23C">
                <Sunrise />
              </el-icon>
              <span class="label">温度:</span>
              <span class="value">{{ currentMotor.temperature }} °C</span>
            </div>
            
            <div class="status-item">
              <el-icon :size="24" :color="currentMotor.running ? '#67C23A' : '#909399'">
                <component :is="currentMotor.running ? 'VideoPlay' : 'VideoPause'" />
              </el-icon>
              <span class="label">状态:</span>
              <span class="value" :style="{color: currentMotor.running ? '#67C23A' : '#909399'}">
                {{ currentMotor.running ? '运行中' : '停止' }}
              </span>
            </div>
          </div>
          
          <el-divider />
          
          <div class="gauge-container">
            <div ref="speedGaugeRef" class="gauge-chart"></div>
          </div>
        </el-card>
          <!-- 电机参数配置（包含了合并的参数配置功能） -->
        <el-card shadow="hover" :header="`${currentMotor.name} 参数配置`" style="margin-top: 20px;">
          <el-tabs type="border-card">
            <!-- PID参数 -->
            <el-tab-pane label="PID参数">
              <el-form label-position="left" label-width="120px">
                <el-divider content-position="left">速度控制 PID</el-divider>
                
                <el-form-item label="P 系数:">
                  <el-input-number v-model="currentMotor.pid.p" :precision="3" :step="0.001" style="width: 180px;" />
                </el-form-item>
                
                <el-form-item label="I 系数:">
                  <el-input-number v-model="currentMotor.pid.i" :precision="3" :step="0.001" style="width: 180px;" />
                </el-form-item>
                
                <el-form-item label="D 系数:">
                  <el-input-number v-model="currentMotor.pid.d" :precision="3" :step="0.001" style="width: 180px;" />
                </el-form-item>
                
                <el-divider content-position="left">位置控制 PID</el-divider>
                
                <el-form-item label="P 系数:">
                  <el-input-number v-model="positionPID.p" :precision="3" :step="0.001" style="width: 180px;" />
                </el-form-item>
                
                <el-form-item label="I 系数:">
                  <el-input-number v-model="positionPID.i" :precision="3" :step="0.001" style="width: 180px;" />
                </el-form-item>
                
                <el-form-item label="D 系数:">
                  <el-input-number v-model="positionPID.d" :precision="3" :step="0.001" style="width: 180px;" />
                </el-form-item>
                
                <el-divider content-position="left">电流控制 PI</el-divider>
                
                <el-form-item label="P 系数:">
                  <el-input-number v-model="currentPI.p" :precision="3" :step="0.001" style="width: 180px;" />
                </el-form-item>
                
                <el-form-item label="I 系数:">
                  <el-input-number v-model="currentPI.i" :precision="3" :step="0.001" style="width: 180px;" />
                </el-form-item>
              </el-form>
            </el-tab-pane>
            
            <!-- 基本参数 -->
            <el-tab-pane label="基本参数">
              <el-form label-position="left" label-width="150px">
                <el-form-item label="电机型号">
                  <el-select v-model="currentMotor.type" placeholder="请选择电机型号" style="width: 100%;">
                    <el-option 
                      v-for="type in motorTypes" 
                      :key="type.value" 
                      :label="type.label" 
                      :value="type.value" 
                    />
                  </el-select>
                </el-form-item>
                
                <el-form-item label="最大速度 (RPM)">
                  <el-input-number v-model="maxSpeed" :min="0" :max="10000" style="width: 180px;" />
                </el-form-item>
                
                <el-form-item label="最大电流 (A)">
                  <el-input-number v-model="currentLimit" :min="0" :max="50" :precision="1" style="width: 180px;" />
                </el-form-item>
                
                <el-form-item label="极对数">
                  <el-input-number v-model="polePairs" :min="1" :max="50" style="width: 180px;" />
                </el-form-item>
                
                <el-form-item label="编码器分辨率">
                  <el-input-number v-model="encoderResolution" :min="0" :max="100000" style="width: 180px;" />
                </el-form-item>
                
                <el-form-item label="反向运行">
                  <el-switch v-model="reverseDirection" />
                </el-form-item>
              </el-form>
            </el-tab-pane>
            
            <!-- 保护参数 -->
            <el-tab-pane label="保护参数">
              <el-form label-position="left" label-width="220px">
                <el-form-item label="过流保护阈值 (A)">
                  <el-input-number v-model="protectionParams.currentThreshold" :min="0" :precision="1" style="width: 180px;" />
                </el-form-item>
                
                <el-form-item label="过温保护阈值 (°C)">
                  <el-input-number v-model="protectionParams.tempThreshold" :min="0" :max="150" style="width: 180px;" />
                </el-form-item>
                
                <el-form-item label="过压保护阈值 (V)">
                  <el-input-number v-model="protectionParams.voltageHighThreshold" :min="0" :precision="1" style="width: 180px;" />
                </el-form-item>
                
                <el-form-item label="欠压保护阈值 (V)">
                  <el-input-number v-model="protectionParams.voltageLowThreshold" :min="0" :precision="1" style="width: 180px;" />
                </el-form-item>
                
                <el-form-item label="堵转检测时间 (ms)">
                  <el-input-number v-model="protectionParams.stallDetectionTime" :min="0" style="width: 180px;" />
                </el-form-item>
                
                <el-form-item label="堵转检测电流 (A)">
                  <el-input-number v-model="protectionParams.stallDetectionCurrent" :min="0" :precision="1" style="width: 180px;" />
                </el-form-item>
                
                <el-form-item label="发生故障时自动关闭电机">
                  <el-switch v-model="protectionParams.autoShutdown" />
                </el-form-item>
              </el-form>
            </el-tab-pane>
            
            <!-- 参数预设 -->
            <el-tab-pane label="参数预设">
              <div class="preset-header">
                <h3>已保存的预设</h3>
                <el-button type="primary" size="small" @click="createPreset">
                  <el-icon><Plus /></el-icon>
                  新建预设
                </el-button>
              </div>
              
              <el-table :data="presets" stripe style="width: 100%">
                <el-table-column prop="name" label="预设名称" width="180" />
                <el-table-column prop="description" label="描述" />
                <el-table-column prop="date" label="创建日期" width="180" />
                <el-table-column label="操作" width="250">
                  <template #default="scope">
                    <el-button size="small" @click="loadPreset(scope.row)">加载</el-button>
                    <el-button size="small" type="primary" @click="editPreset(scope.row)">编辑</el-button>
                    <el-button size="small" type="danger" @click="deletePreset(scope.row)">删除</el-button>
                  </template>
                </el-table-column>
              </el-table>
            </el-tab-pane>
          </el-tabs>
          
          <div class="form-buttons">
            <el-button type="primary" size="small" @click="saveParameters">保存配置</el-button>
            <el-button type="success" size="small" @click="applyParameters">应用配置</el-button>
            <el-button type="info" size="small" @click="readParameters">读取配置</el-button>
            <el-button size="small" @click="resetParameters">恢复默认</el-button>
          </div>
        </el-card>
        
        <!-- 预设对话框 -->
        <el-dialog v-model="presetDialogVisible" title="参数预设" width="500px">
          <el-form :model="presetForm" label-width="80px">
            <el-form-item label="预设名称">
              <el-input v-model="presetForm.name" placeholder="请输入预设名称" />
            </el-form-item>
            
            <el-form-item label="描述">
              <el-input v-model="presetForm.description" type="textarea" :rows="3" placeholder="请输入预设描述" />
            </el-form-item>
          </el-form>
          <template #footer>
            <span>
              <el-button @click="presetDialogVisible = false">取消</el-button>
              <el-button type="primary" @click="savePreset">保存</el-button>
            </span>
          </template>
        </el-dialog>
      </el-col>
    </el-row>
  </div>
</template>

<script setup>
import { ref, computed, onMounted, onBeforeUnmount, watch } from 'vue';
import { 
  VideoPlay, 
  VideoPause, 
  Position, 
  RefreshRight, 
  WarningFilled,
  SemiSelect,
  Guide,
  Operation,
  Aim,
  Odometer,
  Monitor,
  DataLine,
  SetUp,
  Sunrise
} from '@element-plus/icons-vue';
import * as echarts from 'echarts';
import { useAppStore } from '../store';
import { ElMessage, ElMessageBox } from 'element-plus';

const appStore = useAppStore();
const motors = computed(() => appStore.motors);
const connectionStatus = computed(() => appStore.connectionStatus);
const motorTypes = computed(() => appStore.motorTypes);
const controlModes = computed(() => appStore.controlModes);
const selectedMotorId = ref('1');
const speedGaugeRef = ref(null);
let speedGauge = null;
let statusTimer = null;

// 当前选中的电机
const currentMotor = computed(() => {
  return motors.value.find(motor => motor.id === selectedMotorId.value) || motors.value[0];
});

// 辅助参数
const accelerationTime = ref(1000);
const torqueGain = ref(50);
const positionGain = ref(50);

// 参数配置
const currentLimit = ref(10);
const maxSpeed = ref(5000);
const reverseDirection = ref(false);

// 控制历史
const controlHistory = ref([
  { time: '2025-05-16 01:20:15', motor: '电机 1', action: '启动', value: '1500 RPM', result: '成功' },
  { time: '2025-05-16 01:18:32', motor: '电机 1', action: '停止', value: '0 RPM', result: '成功' },
  { time: '2025-05-16 01:15:45', motor: '电机 1', action: '校准', value: '-', result: '成功' }
]);

// 获取控制模式的显示标签
const getControlModeLabel = (mode) => {
  const found = controlModes.value.find(item => item.value === mode);
  return found ? found.label : mode;
};

// 根据电机类型获取最大速度
const getMaxSpeedByType = (type) => {
  switch (type) {
    case 'DM4310': return 10000;
    case 'GM6020': return 7000;
    case 'M3508': return 15000;
    default: return 5000;
  }
};

// 根据电机类型获取最大扭矩
const getMaxTorqueByType = (type) => {
  switch (type) {
    case 'DM4310': return 18;
    case 'GM6020': return 12;
    case 'M3508': return 20;
    default: return 10;
  }
};

// 切换电机运行状态
const toggleMotorRunning = () => {
  if (!connectionStatus.value) {
    ElMessage.warning('请先连接串口');
    return;
  }
  
  const motorIndex = motors.value.findIndex(motor => motor.id === selectedMotorId.value);
  if (motorIndex !== -1) {
    const newStatus = !motors.value[motorIndex].running;
    motors.value[motorIndex].running = newStatus;
    
    // 记录控制历史
    const now = new Date();
    const timeStr = `${now.getFullYear()}-${String(now.getMonth() + 1).padStart(2, '0')}-${String(now.getDate()).padStart(2, '0')} ${String(now.getHours()).padStart(2, '0')}:${String(now.getMinutes()).padStart(2, '0')}:${String(now.getSeconds()).padStart(2, '0')}`;
    
    controlHistory.value.unshift({
      time: timeStr,
      motor: motors.value[motorIndex].name,
      action: newStatus ? '启动' : '停止',
      value: newStatus ? `${motors.value[motorIndex].targetSpeed} RPM` : '0 RPM',
      result: '成功'
    });
    
    ElMessage({
      message: newStatus ? `${motors.value[motorIndex].name} 启动成功` : `${motors.value[motorIndex].name} 已停止`,
      type: newStatus ? 'success' : 'info'
    });
  }
};

// 控制表格中的电机状态切换
const toggleMotorStatus = (motor) => {
  if (!connectionStatus.value) {
    ElMessage.warning('请先连接串口');
    return;
  }
  
  const motorIndex = motors.value.findIndex(m => m.id === motor.id);
  if (motorIndex !== -1) {
    const newStatus = !motors.value[motorIndex].running;
    motors.value[motorIndex].running = newStatus;
    
    // 记录控制历史
    const now = new Date();
    const timeStr = `${now.getFullYear()}-${String(now.getMonth() + 1).padStart(2, '0')}-${String(now.getDate()).padStart(2, '0')} ${String(now.getHours()).padStart(2, '0')}:${String(now.getMinutes()).padStart(2, '0')}:${String(now.getSeconds()).padStart(2, '0')}`;
    
    controlHistory.value.unshift({
      time: timeStr,
      motor: motors.value[motorIndex].name,
      action: newStatus ? '启动' : '停止',
      value: newStatus ? `${motors.value[motorIndex].targetSpeed} RPM` : '0 RPM',
      result: '成功'
    });
  }
};

// 编辑电机设置
const editMotorSettings = (motor) => {
  selectedMotorId.value = motor.id;
  ElMessage.info(`正在编辑 ${motor.name} 设置`);
};

// 回零操作
const resetPosition = () => {
  if (!connectionStatus.value) {
    ElMessage.warning('请先连接串口');
    return;
  }
  
  const motorIndex = motors.value.findIndex(motor => motor.id === selectedMotorId.value);
  if (motorIndex !== -1) {
    motors.value[motorIndex].position = 0;
    
    ElMessage({
      message: `${motors.value[motorIndex].name} 位置已重置为0`,
      type: 'success'
    });
    
    // 记录控制历史
    const now = new Date();
    const timeStr = `${now.getFullYear()}-${String(now.getMonth() + 1).padStart(2, '0')}-${String(now.getDate()).padStart(2, '0')} ${String(now.getHours()).padStart(2, '0')}:${String(now.getMinutes()).padStart(2, '0')}:${String(now.getSeconds()).padStart(2, '0')}`;
    
    controlHistory.value.unshift({
      time: timeStr,
      motor: motors.value[motorIndex].name,
      action: '回零',
      value: '0°',
      result: '成功'
    });
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
  
  // 记录控制历史
  const now = new Date();
  const timeStr = `${now.getFullYear()}-${String(now.getMonth() + 1).padStart(2, '0')}-${String(now.getDate()).padStart(2, '0')} ${String(now.getHours()).padStart(2, '0')}:${String(now.getMinutes()).padStart(2, '0')}:${String(now.getSeconds()).padStart(2, '0')}`;
  
  controlHistory.value.unshift({
    time: timeStr,
    motor: currentMotor.value.name,
    action: '校准',
    value: '-',
    result: '成功'
  });
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
  
  // 记录控制历史
  const now = new Date();
  const timeStr = `${now.getFullYear()}-${String(now.getMonth() + 1).padStart(2, '0')}-${String(now.getDate()).padStart(2, '0')} ${String(now.getHours()).padStart(2, '0')}:${String(now.getMinutes()).padStart(2, '0')}:${String(now.getSeconds()).padStart(2, '0')}`;
  
  controlHistory.value.unshift({
    time: timeStr,
    motor: '全部电机',
    action: '紧急停止',
    value: '-',
    result: '成功'
  });
};

// 应用设置到所有电机
const applyToAllMotors = () => {
  ElMessageBox.confirm(
    `是否将当前电机 ${currentMotor.value.name} 的设置应用到所有电机？`,
    '应用确认',
    {
      confirmButtonText: '确定',
      cancelButtonText: '取消',
      type: 'warning'
    }
  ).then(() => {
    // 应用配置到所有电机
    const { type, controlMode, targetSpeed, targetPosition, targetTorque, pid } = currentMotor.value;
    motors.value.forEach(motor => {
      if (motor.id !== currentMotor.value.id) {
        motor.controlMode = controlMode;
        motor.targetSpeed = targetSpeed;
        motor.targetPosition = targetPosition;
        motor.targetTorque = targetTorque;
        motor.pid = { ...pid };
      }
    });
    
    ElMessage({
      message: '设置已应用到所有电机',
      type: 'success'
    });
  }).catch(() => {
    // 取消操作
  });
};

// 保存参数
const saveParameters = () => {
  // 更新当前电机的PID参数
  ElMessage({
    message: `${currentMotor.value.name} 参数已保存`,
    type: 'success'
  });
};

// 重置参数
const resetParameters = () => {
  // 重置当前电机的PID参数为默认值
  const motorIndex = motors.value.findIndex(motor => motor.id === selectedMotorId.value);
  if (motorIndex !== -1) {
    motors.value[motorIndex].pid = {
      p: 0.8,
      i: 0.05,
      d: 0.1
    };
    currentLimit.value = 10;
    maxSpeed.value = getMaxSpeedByType(motors.value[motorIndex].type);
    reverseDirection.value = false;
    
    ElMessage({
      message: `${motors.value[motorIndex].name} 参数已重置为默认值`,
      type: 'info'
    });
  }
};

// 初始化速度仪表盘
const initSpeedGauge = () => {
  if (speedGaugeRef.value) {
    speedGauge = echarts.init(speedGaugeRef.value);
    
    const option = {
      series: [
        {
          type: 'gauge',
          progress: {
            show: true,
            width: 18
          },
          axisLine: {
            lineStyle: {
              width: 18
            }
          },
          axisTick: {
            show: false
          },
          splitLine: {
            length: 15,
            lineStyle: {
              width: 2,
              color: '#999'
            }
          },
          axisLabel: {
            distance: 25,
            color: '#999',
            fontSize: 12
          },
          anchor: {
            show: true,
            showAbove: true,
            size: 25,
            itemStyle: {
              borderWidth: 10
            }
          },
          title: {
            show: true,
            fontSize: 14
          },
          detail: {
            valueAnimation: true,
            fontSize: 30,
            offsetCenter: [0, '70%'],
            formatter: '{value} RPM',
            color: 'inherit'
          },
          data: [
            {
              value: currentMotor.value ? currentMotor.value.speed : 0,
              name: '转速',
              title: {
                offsetCenter: [0, '95%']
              }
            }
          ],
          max: currentMotor.value ? getMaxSpeedByType(currentMotor.value.type) : 5000
        }
      ]
    };
    
    speedGauge.setOption(option);
  }
};

// 更新仪表盘数据
const updateGauge = () => {
  if (!speedGauge) return;
  
  speedGauge.setOption({
    series: [
      {
        data: [
          {
            value: currentMotor.value ? currentMotor.value.speed : 0
          }
        ],
        max: currentMotor.value ? getMaxSpeedByType(currentMotor.value.type) : 5000
      }
    ]
  });
};

// 当选中的电机变化时更新图表
watch(selectedMotorId, () => {
  updateGauge();
});

// 生命周期钩子
onMounted(() => {
  initSpeedGauge();
  window.addEventListener('resize', handleResize);
  
  // 更新电机状态
  statusTimer = setInterval(() => {
    updateGauge();
    
    // 模拟更新电机状态
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
  }, 1000);
  
  // 初始化页面所需数据
  currentLimit.value = 10;
  maxSpeed.value = getMaxSpeedByType(currentMotor.value.type);
  reverseDirection.value = false;
});

onBeforeUnmount(() => {
  window.removeEventListener('resize', handleResize);
  if (statusTimer) clearInterval(statusTimer);
  if (speedGauge) speedGauge.dispose();
});

const handleResize = () => {
  if (speedGauge) speedGauge.resize();
};
</script>

<style scoped>
.motor-control-container {
  padding: 10px;
}

.control-panel {
  padding: 20px 0;
}

.control-item {
  margin-bottom: 20px;
}

.control-item .label {
  display: block;
  margin-bottom: 10px;
  font-weight: bold;
}

.button-group {
  display: flex;
  justify-content: center;
  gap: 15px;
}

.status-panel {
  display: flex;
  flex-direction: column;
  gap: 15px;
}

.status-item {
  display: flex;
  align-items: center;
}

.status-item .label {
  margin-left: 10px;
  margin-right: 5px;
  color: #666;
  width: 80px;
}

.status-item .value {
  font-weight: bold;
}

.gauge-container {
  margin-top: 20px;
}

.gauge-chart {
  height: 240px;
}

.unit {
  margin-left: 5px;
  color: #666;
}

.form-buttons {
  margin-top: 20px;
  display: flex;
  justify-content: flex-end;
  gap: 10px;
}
</style>
