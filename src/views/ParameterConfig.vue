<template>
  <div class="parameter-config-container">
    <el-row :gutter="20">
      <el-col :span="24">
        <el-card shadow="hover" header="电机参数配置">
          <el-tabs v-model="activeTab" type="border-card">
            <!-- 基本参数 -->
            <el-tab-pane label="基本参数" name="basic">
              <el-form :model="basicParams" label-width="150px" label-position="left">
                <el-form-item label="电机型号">
                  <el-select v-model="basicParams.model" placeholder="请选择电机型号" style="width: 100%;">
                    <el-option label="MoWheel-100" value="MW-100" />
                    <el-option label="MoWheel-200" value="MW-200" />
                    <el-option label="MoWheel-300" value="MW-300" />
                    <el-option label="自定义" value="custom" />
                  </el-select>
                </el-form-item>
                
                <el-form-item label="最大速度 (RPM)">
                  <el-input-number v-model="basicParams.maxSpeed" :min="0" :max="10000" style="width: 100%;" />
                </el-form-item>
                
                <el-form-item label="最大电流 (A)">
                  <el-input-number v-model="basicParams.maxCurrent" :min="0" :max="50" :precision="1" style="width: 100%;" />
                </el-form-item>
                
                <el-form-item label="额定电压 (V)">
                  <el-input-number v-model="basicParams.ratedVoltage" :min="0" :max="500" :precision="1" style="width: 100%;" />
                </el-form-item>
                
                <el-form-item label="极对数">
                  <el-input-number v-model="basicParams.polePairs" :min="1" :max="50" style="width: 100%;" />
                </el-form-item>
                
                <el-form-item label="编码器分辨率">
                  <el-input-number v-model="basicParams.encoderResolution" :min="0" :max="100000" style="width: 100%;" />
                </el-form-item>
                
                <el-form-item label="反向运行">
                  <el-switch v-model="basicParams.reverseDirection" />
                </el-form-item>
              </el-form>
            </el-tab-pane>
            
            <!-- 控制参数 -->
            <el-tab-pane label="控制参数" name="control">
              <el-form :model="controlParams" label-width="150px" label-position="left">
                <el-divider content-position="left">速度控制 PID</el-divider>
                
                <el-form-item label="P 系数">
                  <el-input-number v-model="controlParams.speedPID.p" :precision="3" :step="0.001" style="width: 100%;" />
                </el-form-item>
                
                <el-form-item label="I 系数">
                  <el-input-number v-model="controlParams.speedPID.i" :precision="3" :step="0.001" style="width: 100%;" />
                </el-form-item>
                
                <el-form-item label="D 系数">
                  <el-input-number v-model="controlParams.speedPID.d" :precision="3" :step="0.001" style="width: 100%;" />
                </el-form-item>
                
                <el-divider content-position="left">位置控制 PID</el-divider>
                
                <el-form-item label="P 系数">
                  <el-input-number v-model="controlParams.positionPID.p" :precision="3" :step="0.001" style="width: 100%;" />
                </el-form-item>
                
                <el-form-item label="I 系数">
                  <el-input-number v-model="controlParams.positionPID.i" :precision="3" :step="0.001" style="width: 100%;" />
                </el-form-item>
                
                <el-form-item label="D 系数">
                  <el-input-number v-model="controlParams.positionPID.d" :precision="3" :step="0.001" style="width: 100%;" />
                </el-form-item>
                
                <el-divider content-position="left">电流控制 PI</el-divider>
                
                <el-form-item label="P 系数">
                  <el-input-number v-model="controlParams.currentPI.p" :precision="3" :step="0.001" style="width: 100%;" />
                </el-form-item>
                
                <el-form-item label="I 系数">
                  <el-input-number v-model="controlParams.currentPI.i" :precision="3" :step="0.001" style="width: 100%;" />
                </el-form-item>
              </el-form>
            </el-tab-pane>
            
            <!-- 保护参数 -->
            <el-tab-pane label="保护参数" name="protection">
              <el-form :model="protectionParams" label-width="220px" label-position="left">
                <el-form-item label="过流保护阈值 (A)">
                  <el-input-number v-model="protectionParams.currentThreshold" :min="0" :precision="1" style="width: 100%;" />
                </el-form-item>
                
                <el-form-item label="过温保护阈值 (°C)">
                  <el-input-number v-model="protectionParams.tempThreshold" :min="0" :max="150" style="width: 100%;" />
                </el-form-item>
                
                <el-form-item label="过压保护阈值 (V)">
                  <el-input-number v-model="protectionParams.voltageHighThreshold" :min="0" :precision="1" style="width: 100%;" />
                </el-form-item>
                
                <el-form-item label="欠压保护阈值 (V)">
                  <el-input-number v-model="protectionParams.voltageLowThreshold" :min="0" :precision="1" style="width: 100%;" />
                </el-form-item>
                
                <el-form-item label="堵转检测时间 (ms)">
                  <el-input-number v-model="protectionParams.stallDetectionTime" :min="0" style="width: 100%;" />
                </el-form-item>
                
                <el-form-item label="堵转检测电流 (A)">
                  <el-input-number v-model="protectionParams.stallDetectionCurrent" :min="0" :precision="1" style="width: 100%;" />
                </el-form-item>
                
                <el-form-item label="发生故障时自动关闭电机">
                  <el-switch v-model="protectionParams.autoShutdown" />
                </el-form-item>
              </el-form>
            </el-tab-pane>
          </el-tabs>
          
          <div class="button-row">
            <el-button type="primary" @click="saveParams">保存配置</el-button>
            <el-button type="success" @click="applyParams">应用配置</el-button>
            <el-button type="info" @click="readParams">读取配置</el-button>
            <el-button @click="resetParams">恢复默认</el-button>
          </div>
        </el-card>
      </el-col>
    </el-row>
    
    <el-row :gutter="20" style="margin-top: 20px;">
      <el-col :span="24">
        <el-card shadow="hover" header="参数预设">
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
        </el-card>
      </el-col>
    </el-row>
    
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
  </div>
</template>

<script setup>
import { ref } from 'vue';
import { ElMessage } from 'element-plus';
import { Plus } from '@element-plus/icons-vue';

// 活动标签页
const activeTab = ref('basic');

// 基本参数
const basicParams = ref({
  model: 'MW-200',
  maxSpeed: 5000,
  maxCurrent: 20.0,
  ratedVoltage: 220.0,
  polePairs: 4,
  encoderResolution: 2048,
  reverseDirection: false
});

// 控制参数
const controlParams = ref({
  speedPID: {
    p: 0.500,
    i: 0.050,
    d: 0.002
  },
  positionPID: {
    p: 5.000,
    i: 0.010,
    d: 0.100
  },
  currentPI: {
    p: 0.020,
    i: 0.002
  }
});

// 保护参数
const protectionParams = ref({
  currentThreshold: 25.0,
  tempThreshold: 85,
  voltageHighThreshold: 250.0,
  voltageLowThreshold: 180.0,
  stallDetectionTime: 2000,
  stallDetectionCurrent: 15.0,
  autoShutdown: true
});

// 预设数据
const presets = ref([
  { name: '高速模式', description: '适用于高速运行场景，弱化电流控制', date: '2025-05-01' },
  { name: '高扭矩模式', description: '适用于大负载场景，增强电流控制', date: '2025-05-05' },
  { name: '位置控制模式', description: '适用于精确位置控制场景', date: '2025-05-10' }
]);

// 预设对话框
const presetDialogVisible = ref(false);
const presetForm = ref({
  name: '',
  description: '',
  isEdit: false,
  editIndex: -1
});

// 保存配置
const saveParams = () => {
  ElMessage.success('参数配置已保存');
};

// 应用配置
const applyParams = () => {
  ElMessage.success('参数配置已应用到电机');
};

// 读取配置
const readParams = () => {
  ElMessage.success('已从电机读取参数配置');
};

// 恢复默认
const resetParams = () => {
  ElMessage({
    message: '是否确定恢复默认参数？',
    type: 'warning',
    showClose: true,
    showCancelButton: true,
    confirmButtonText: '确定',
    cancelButtonText: '取消',
    beforeClose: (action, instance, done) => {
      if (action === 'confirm') {
        // 恢复默认值
        basicParams.value = {
          model: 'MW-200',
          maxSpeed: 5000,
          maxCurrent: 20.0,
          ratedVoltage: 220.0,
          polePairs: 4,
          encoderResolution: 2048,
          reverseDirection: false
        };
        
        controlParams.value = {
          speedPID: {
            p: 0.500,
            i: 0.050,
            d: 0.002
          },
          positionPID: {
            p: 5.000,
            i: 0.010,
            d: 0.100
          },
          currentPI: {
            p: 0.020,
            i: 0.002
          }
        };
        
        protectionParams.value = {
          currentThreshold: 25.0,
          tempThreshold: 85,
          voltageHighThreshold: 250.0,
          voltageLowThreshold: 180.0,
          stallDetectionTime: 2000,
          stallDetectionCurrent: 15.0,
          autoShutdown: true
        };
        
        ElMessage.success('已恢复默认参数');
      }
      done();
    }
  });
};

// 创建预设
const createPreset = () => {
  presetForm.value = {
    name: '',
    description: '',
    isEdit: false,
    editIndex: -1
  };
  presetDialogVisible.value = true;
};

// 加载预设
const loadPreset = (preset) => {
  ElMessage.success(`已加载预设: ${preset.name}`);
};

// 编辑预设
const editPreset = (preset) => {
  const idx = presets.value.findIndex(p => p.name === preset.name);
  presetForm.value = {
    name: preset.name,
    description: preset.description,
    isEdit: true,
    editIndex: idx
  };
  presetDialogVisible.value = true;
};

// 删除预设
const deletePreset = (preset) => {
  ElMessage({
    message: `是否确定删除预设: ${preset.name}？`,
    type: 'warning',
    showClose: true,
    showCancelButton: true,
    confirmButtonText: '确定',
    cancelButtonText: '取消',
    beforeClose: (action, instance, done) => {
      if (action === 'confirm') {
        const idx = presets.value.findIndex(p => p.name === preset.name);
        presets.value.splice(idx, 1);
        ElMessage.success('预设已删除');
      }
      done();
    }
  });
};

// 保存预设
const savePreset = () => {
  if (!presetForm.value.name) {
    ElMessage.warning('请输入预设名称');
    return;
  }
  
  if (presetForm.value.isEdit) {
    // 更新已有预设
    presets.value[presetForm.value.editIndex] = {
      name: presetForm.value.name,
      description: presetForm.value.description,
      date: presets.value[presetForm.value.editIndex].date
    };
    ElMessage.success('预设已更新');
  } else {
    // 创建新预设
    const today = new Date();
    const dateStr = `${today.getFullYear()}-${String(today.getMonth() + 1).padStart(2, '0')}-${String(today.getDate()).padStart(2, '0')}`;
    
    presets.value.push({
      name: presetForm.value.name,
      description: presetForm.value.description,
      date: dateStr
    });
    ElMessage.success('预设已创建');
  }
  
  presetDialogVisible.value = false;
};
</script>

<style scoped>
.parameter-config-container {
  padding: 10px;
}

.button-row {
  margin-top: 20px;
  display: flex;
  justify-content: flex-end;
  gap: 10px;
}

.preset-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 15px;
}
</style>
