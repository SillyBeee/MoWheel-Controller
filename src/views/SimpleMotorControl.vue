<template>
  <div class="simple-motor-control">
    <h2>简化版电机控制</h2>
    
    <el-alert
      v-if="!connectionStatus"
      title="未连接到串口，请先在顶部导航栏连接串口"
      type="warning"
      :closable="false"
      show-icon
      style="margin-bottom: 20px;"
    />
    
    <el-card shadow="hover" header="电机状态" class="themed-card">
      <el-table :data="motors" stripe class="themed-table">
        <el-table-column prop="id" label="ID" width="60" />
        <el-table-column prop="name" label="名称" width="100" />
        <el-table-column prop="speed" label="速度(RPM)" width="120" />
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
              @click="toggleMotor(scope.row)"
              :disabled="!connectionStatus"
            >
              {{ scope.row.running ? '停止' : '启动' }}
            </el-button>
          </template>
        </el-table-column>
      </el-table>
    </el-card>
    
    <el-card shadow="hover" header="控制面板" style="margin-top: 20px;" class="themed-card">
      <el-row>
        <el-col :span="12">
          <div class="control-item">
            <span class="label">选择电机:</span>
            <el-select v-model="selectedMotorId" placeholder="选择电机">
              <el-option 
                v-for="motor in motors" 
                :key="motor.id" 
                :label="motor.name" 
                :value="motor.id" 
              />
            </el-select>
          </div>
        </el-col>
        <el-col :span="12">
          <div class="control-item">
            <span class="label">目标速度 (RPM):</span>
            <el-slider
              v-model="currentMotor.targetSpeed"
              :min="0"
              :max="5000"
              :step="100"
              show-input
              :disabled="!connectionStatus || !currentMotor.running"
            />
          </div>
        </el-col>
      </el-row>
      
      <el-row style="margin-top: 20px;">
        <el-col :span="24" class="button-group">
          <el-button 
            type="success" 
            @click="applySpeed"
            :disabled="!connectionStatus || !currentMotor.running"
            :loading="sendingCommand"
          >
            应用速度
          </el-button>
          
          <el-button 
            type="warning" 
            @click="resetMotor"
            :disabled="!connectionStatus"
            :loading="sendingCommand"
          >
            重置电机
          </el-button>
        </el-col>
      </el-row>
    </el-card>
    
    <el-card shadow="hover" header="电机详细信息" style="margin-top: 20px;" class="themed-card" v-if="connectionStatus">
      <el-descriptions :column="3" border>
        <el-descriptions-item label="电机型号">{{ currentMotor.type }}</el-descriptions-item>
        <el-descriptions-item label="当前速度">{{ currentMotor.speed }} RPM</el-descriptions-item>
        <el-descriptions-item label="目标速度">{{ currentMotor.targetSpeed }} RPM</el-descriptions-item>
        <el-descriptions-item label="位置">{{ currentMotor.position.toFixed(2) }}</el-descriptions-item>
        <el-descriptions-item label="扭矩">{{ currentMotor.torque.toFixed(2) }} N·m</el-descriptions-item>
        <el-descriptions-item label="温度">{{ currentMotor.temperature.toFixed(1) }} °C</el-descriptions-item>
        <el-descriptions-item label="电压">{{ currentMotor.voltage.toFixed(1) }} V</el-descriptions-item>
        <el-descriptions-item label="电流">{{ currentMotor.current.toFixed(2) }} A</el-descriptions-item>
        <el-descriptions-item label="功率">{{ (currentMotor.voltage * currentMotor.current).toFixed(2) }} W</el-descriptions-item>
      </el-descriptions>
      
      <div style="margin-top: 20px;">
        <h4>实时监控</h4>
        <div ref="chartContainer" style="height: 300px;"></div>
      </div>
    </el-card>
  </div>
</template>

<script setup>
import { ref, computed, onMounted, onBeforeUnmount, watch } from 'vue';
import { useAppStore } from '../store';
import { ElMessage } from 'element-plus';
import * as echarts from 'echarts';

const appStore = useAppStore();
const motors = computed(() => appStore.motors);
const selectedMotorId = ref('1');
const connectionStatus = computed(() => appStore.connectionStatus);
const chartContainer = ref(null);
const myChart = ref(null);
const sendingCommand = ref(false);
const speedHistory = ref([]);
const timeLabels = ref([]);

// 当前选中的电机
const currentMotor = computed(() => {
  return motors.value.find(motor => motor.id === selectedMotorId.value) || motors.value[0];
});

// 切换电机运行状态
const toggleMotor = async (motor) => {
  sendingCommand.value = true;
  try {
    if (motor.running) {
      await appStore.stopMotor(motor.id);
      ElMessage.success(`电机 ${motor.name} 已停止`);
    } else {
      await appStore.startMotor(motor.id);
      ElMessage.success(`电机 ${motor.name} 已启动`);
    }
  } catch (error) {
    ElMessage.error(`操作失败: ${error}`);
  } finally {
    sendingCommand.value = false;
  }
};

// 应用速度设置
const applySpeed = async () => {
  sendingCommand.value = true;
  try {
    await appStore.setMotorSpeed(selectedMotorId.value, currentMotor.value.targetSpeed);
    ElMessage.success(`已将电机 ${currentMotor.value.name} 速度设置为 ${currentMotor.value.targetSpeed} RPM`);
  } catch (error) {
    ElMessage.error(`设置速度失败: ${error}`);
  } finally {
    sendingCommand.value = false;
  }
};

// 重置电机
const resetMotor = async () => {
  sendingCommand.value = true;
  try {
    if (currentMotor.value.running) {
      await appStore.stopMotor(selectedMotorId.value);
    }
    currentMotor.value.targetSpeed = 0;
    ElMessage.success(`电机 ${currentMotor.value.name} 已重置`);
  } catch (error) {
    ElMessage.error(`重置电机失败: ${error}`);
  } finally {
    sendingCommand.value = false;
  }
};

// 初始化图表
const initChart = () => {
  if (chartContainer.value) {
    myChart.value = echarts.init(chartContainer.value);
    
    // 设置图表主题
    const isDarkMode = document.documentElement.classList.contains('dark-mode');
    const textColor = isDarkMode ? '#ffffff' : '#303133';
    const axisLineColor = isDarkMode ? 'rgba(255, 255, 255, 0.2)' : 'rgba(0, 0, 0, 0.2)';
    
    const option = {
      backgroundColor: 'transparent',
      title: {
        text: '电机速度历史',
        textStyle: {
          color: textColor
        }
      },
      tooltip: {
        trigger: 'axis'
      },
      xAxis: {
        type: 'category',
        data: timeLabels.value,
        axisLine: {
          lineStyle: {
            color: axisLineColor
          }
        },
        axisLabel: {
          color: textColor
        }
      },
      yAxis: {
        type: 'value',
        name: '速度 (RPM)',
        nameTextStyle: {
          color: textColor
        },
        axisLine: {
          lineStyle: {
            color: axisLineColor
          }
        },
        axisLabel: {
          color: textColor
        },
        splitLine: {
          lineStyle: {
            color: axisLineColor
          }
        }
      },
      series: [{
        name: '实际速度',
        type: 'line',
        data: speedHistory.value,
        smooth: true,
        showSymbol: false,
        emphasis: {
          focus: 'series'
        },
        lineStyle: {
          width: 3
        }
      }, {
        name: '目标速度',
        type: 'line',
        data: new Array(speedHistory.value.length).fill(currentMotor.value.targetSpeed),
        smooth: true,
        showSymbol: false,
        lineStyle: {
          type: 'dashed',
          width: 2
        },
        emphasis: {
          focus: 'series'
        }
      }]
    };
    
    myChart.value.setOption(option);
  }
};

// 更新图表数据
const updateChart = () => {
  if (myChart.value && currentMotor.value) {
    // 添加新数据点
    const now = new Date();
    const timeStr = `${now.getHours()}:${now.getMinutes()}:${now.getSeconds()}`;
    
    speedHistory.value.push(currentMotor.value.speed);
    timeLabels.value.push(timeStr);
    
    // 最多保留30个点
    if (speedHistory.value.length > 30) {
      speedHistory.value.shift();
      timeLabels.value.shift();
    }
    
    // 更新图表
    myChart.value.setOption({
      xAxis: {
        data: timeLabels.value
      },
      series: [{
        name: '实际速度',
        data: speedHistory.value
      }, {
        name: '目标速度',
        data: new Array(speedHistory.value.length).fill(currentMotor.value.targetSpeed)
      }]
    });
  }
};

// 监听主题变化，重新初始化图表
watch(() => appStore.theme.mode, () => {
  if (myChart.value) {
    myChart.value.dispose();
    setTimeout(initChart, 100); // 延迟初始化，确保DOM已更新
  }
});

// 监听选中的电机变化，重置图表数据
watch(selectedMotorId, () => {
  speedHistory.value = [];
  timeLabels.value = [];
  if (myChart.value) {
    updateChart();
  }
});

// 生命周期钩子
onMounted(() => {
  console.log('SimpleMotorControl组件已挂载');
  
  // 初始化图表
  initChart();
  
  // 定时更新图表数据
  const chartUpdateTimer = setInterval(updateChart, 1000);
  
  // 检查端口连接状态
  appStore.checkPortConnection();
  
  // 组件卸载时清除定时器
  onBeforeUnmount(() => {
    console.log('SimpleMotorControl组件即将卸载');
    
    if (chartUpdateTimer) {
      clearInterval(chartUpdateTimer);
    }
    
    if (myChart.value) {
      myChart.value.dispose();
      myChart.value = null;
    }
  });
});

// 监听窗口大小变化，调整图表大小
window.addEventListener('resize', () => {
  if (myChart.value) {
    myChart.value.resize();
  }
});
</script>

<style scoped>
.simple-motor-control {
  padding: 20px;
}

.control-item {
  margin-bottom: 20px;
}

.control-item .label {
  display: block;
  margin-bottom: 10px;
  font-weight: bold;
  color: var(--text-color-regular);
}

.button-group {
  display: flex;
  justify-content: center;
  gap: 15px;
}

/* 主题化卡片 */
.themed-card {
  background-color: var(--card-bg-color) !important;
  color: var(--text-color-primary) !important;
  border-color: var(--border-color) !important;
}

.themed-card :deep(.el-card__header) {
  background-color: var(--bg-color-mute);
  color: var(--text-color-primary);
  border-color: var(--border-color);
}

/* 主题化表格 */
.themed-table :deep(.el-table__header-wrapper),
.themed-table :deep(.el-table__body-wrapper) {
  color: var(--text-color-primary);
}

.themed-table :deep(.el-table) {
  --el-table-border-color: var(--border-color);
  --el-table-header-bg-color: var(--bg-color-mute);
  --el-table-tr-bg-color: var(--card-bg-color);
  --el-table-row-hover-bg-color: var(--bg-color-soft);
}
</style>