<template>
  <div class="simple-motor-control">
    <h2>简化版电机控制</h2>
    
    <el-card shadow="hover" header="电机状态">
      <el-table :data="motors" stripe>
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
            >
              {{ scope.row.running ? '停止' : '启动' }}
            </el-button>
          </template>
        </el-table-column>
      </el-table>
    </el-card>
    
    <el-card shadow="hover" header="控制面板" style="margin-top: 20px;">
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
            />
          </div>
        </el-col>
      </el-row>
      
      <el-row style="margin-top: 20px;">
        <el-col :span="24" class="button-group">
          <el-button 
            type="success" 
            @click="applySpeed"
          >
            应用速度
          </el-button>
          
          <el-button 
            type="warning" 
            @click="resetMotor"
          >
            重置电机
          </el-button>
        </el-col>
      </el-row>
    </el-card>
  </div>
</template>

<script setup>
import { ref, computed, onMounted, onBeforeUnmount } from 'vue';
import { useAppStore } from '../store';

const appStore = useAppStore();
const motors = computed(() => appStore.motors);
const selectedMotorId = ref('1');
let statusTimer = null;

// 当前选中的电机
const currentMotor = computed(() => {
  return motors.value.find(motor => motor.id === selectedMotorId.value) || motors.value[0];
});

// 切换电机运行状态
const toggleMotor = (motor) => {
  const motorIndex = motors.value.findIndex(m => m.id === motor.id);
  if (motorIndex !== -1) {
    motors.value[motorIndex].running = !motors.value[motorIndex].running;
  }
};

// 应用速度设置
const applySpeed = () => {
  const motorIndex = motors.value.findIndex(motor => motor.id === selectedMotorId.value);
  if (motorIndex !== -1 && motors.value[motorIndex].running) {
    motors.value[motorIndex].speed = motors.value[motorIndex].targetSpeed;
  }
};

// 重置电机
const resetMotor = () => {
  const motorIndex = motors.value.findIndex(motor => motor.id === selectedMotorId.value);
  if (motorIndex !== -1) {
    motors.value[motorIndex].speed = 0;
    motors.value[motorIndex].running = false;
    motors.value[motorIndex].targetSpeed = 0;
  }
};

// 生命周期钩子
onMounted(() => {
  console.log('SimpleMotorControl组件已挂载');
  
  // 模拟数据更新
  statusTimer = setInterval(() => {
    motors.value.forEach((motor, index) => {
      if (motor.running) {
        // 如果电机正在运行，逐渐接近目标速度
        const diff = motor.targetSpeed - motor.speed;
        if (Math.abs(diff) > 50) {
          motor.speed += diff > 0 ? 50 : -50;
        } else {
          motor.speed = motor.targetSpeed;
        }
      }
    });
  }, 500);
});

onBeforeUnmount(() => {
  console.log('SimpleMotorControl组件即将卸载');
  
  if (statusTimer) {
    clearInterval(statusTimer);
    statusTimer = null;
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
}

.button-group {
  display: flex;
  justify-content: center;
  gap: 15px;
}
</style>