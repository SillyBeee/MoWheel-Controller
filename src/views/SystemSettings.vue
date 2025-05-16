<template>
  <div class="system-settings-container">
    <el-row :gutter="20">
      <!-- 系统设置 -->
      <el-col :span="16">
        <el-card shadow="hover" header="系统设置">
          <el-tabs v-model="activeTab" type="border-card">
            <!-- 通信设置 -->
            <el-tab-pane label="通信设置" name="communication">              <el-form :model="commSettings" label-width="120px" label-position="left">
                <el-form-item label="通信接口">
                  <el-tag type="success" effect="dark" size="large">CAN总线</el-tag>
                </el-form-item>
                
                <el-divider content-position="left">CAN设置</el-divider>
                
                <el-form-item label="设备ID">
                  <el-input-number v-model="commSettings.can.deviceId" :min="1" :max="255" style="width: 100%;" />
                </el-form-item>
                
                <el-form-item label="波特率">
                  <el-select v-model="commSettings.can.baudRate" placeholder="选择波特率" style="width: 100%;">
                    <el-option label="125 Kbps" value="125" />
                    <el-option label="250 Kbps" value="250" />
                    <el-option label="500 Kbps" value="500" />
                    <el-option label="1000 Kbps" value="1000" />
                  </el-select>
                </el-form-item>
                
                <el-form-item label="CAN通道">
                  <el-select v-model="commSettings.can.channel" placeholder="选择CAN通道" style="width: 100%;">
                    <el-option label="CAN 1" value="1" />
                    <el-option label="CAN 2" value="2" />
                  </el-select>
                </el-form-item>
                
                <el-form-item label="终端电阻">
                  <el-switch v-model="commSettings.can.termination" />
                </el-form-item>
              </el-form>
            </el-tab-pane>
            
            <!-- 用户界面 -->
            <el-tab-pane label="用户界面" name="ui">              <el-form :model="uiSettings" label-width="120px" label-position="left">
                <el-form-item label="主题">
                  <div class="theme-selector">
                    <div 
                      class="theme-item" 
                      :class="{ active: uiSettings.theme === 'light' }" 
                      @click="setTheme('light')"
                    >
                      <div class="theme-preview light-theme"></div>
                      <span>浅色</span>
                    </div>
                    <div 
                      class="theme-item" 
                      :class="{ active: uiSettings.theme === 'dark' }" 
                      @click="setTheme('dark')"
                    >
                      <div class="theme-preview dark-theme"></div>
                      <span>深色</span>
                    </div>
                    <div 
                      class="theme-item" 
                      :class="{ active: uiSettings.theme === 'system' }" 
                      @click="setTheme('system')"
                    >
                      <div class="theme-preview system-theme">
                        <div class="split-preview"></div>
                      </div>
                      <span>跟随系统</span>
                    </div>
                  </div>
                </el-form-item>
                
                <el-form-item label="主题颜色">
                  <el-color-picker v-model="uiSettings.primaryColor" show-alpha :predefine="predefineColors" @change="updateThemeColor" />
                </el-form-item>
                
                <el-form-item label="语言">
                  <el-select v-model="uiSettings.language" placeholder="选择语言" style="width: 100%;">
                    <el-option label="中文" value="zh-CN" />
                    <el-option label="English" value="en-US" />
                  </el-select>
                </el-form-item>
                
                <el-form-item label="字体大小">
                  <el-slider v-model="uiSettings.fontSize" :min="12" :max="20" style="width: 100%;" />
                  <span class="font-size-preview" :style="{ fontSize: uiSettings.fontSize + 'px' }">字体预览 Aa</span>
                </el-form-item>
                
                <el-form-item label="数据刷新率">
                  <el-select v-model="uiSettings.refreshRate" placeholder="选择刷新率" style="width: 100%;">
                    <el-option label="高 (100ms)" value="100" />
                    <el-option label="中 (500ms)" value="500" />
                    <el-option label="低 (1000ms)" value="1000" />
                  </el-select>
                </el-form-item>
              </el-form>
            </el-tab-pane>
            
            <!-- 系统配置 -->
            <el-tab-pane label="系统配置" name="system">              <el-form :model="sysSettings" label-width="180px" label-position="left">
                <el-form-item label="自动保存电机参数">
                  <el-switch v-model="sysSettings.autoSaveParams" />
                  <div class="setting-description">
                    <span>自动保存电机参数到本地文件，以便在重新启动时恢复上次的设置</span>
                  </div>
                </el-form-item>
                
                <el-form-item label="数据导出路径">
                  <el-input v-model="sysSettings.exportPath" placeholder="选择导出路径">
                    <template #append>
                      <el-button @click="selectExportPath">
                        <el-icon><Folder /></el-icon>
                        浏览
                      </el-button>
                    </template>
                  </el-input>
                  <div class="setting-description">
                    <span>用于存储导出的数据记录、电机参数配置和日志文件</span>
                  </div>
                </el-form-item>
              </el-form>
            </el-tab-pane>
          </el-tabs>
          
          <div class="button-row">
            <el-button type="primary" @click="saveSettings">保存设置</el-button>
            <el-button @click="resetSettings">恢复默认</el-button>
          </div>
        </el-card>
      </el-col>
      
      <!-- 系统信息 -->
      <el-col :span="8">
        <el-card shadow="hover" header="系统信息">
          <div class="info-item">
            <span class="label">软件版本:</span>
            <span class="value">v1.0.0</span>
          </div>
          
          <div class="info-item">
            <span class="label">系统平台:</span>
            <span class="value">Windows 10</span>
          </div>
          
          <div class="info-item">
            <span class="label">框架版本:</span>
            <span class="value">Tauri 2.0.0 + Vue 3.5</span>
          </div>
          
          <div class="info-item">
            <span class="label">运行时间:</span>
            <span class="value">{{ runtime }}</span>
          </div>
          
          <el-divider />
          
          <div class="check-update">
            <p>当前为最新版本</p>
            <el-button type="primary" size="small" @click="checkUpdate">检查更新</el-button>
          </div>
        </el-card>
        
        <el-card shadow="hover" header="关于软件" style="margin-top: 20px;">
          <div class="about-content">
            <img src="/tauri.svg" alt="Logo" class="about-logo" />
            <h3>MoWheel Controller</h3>
            <p>电机控制上位机软件</p>
            <p class="copyright">© 2025 MoWheel Team. 保留所有权利。</p>
            
            <div class="social-links">
              <a href="#" target="_blank">官方网站</a>
              <span class="divider">|</span>
              <a href="#" target="_blank">技术支持</a>
              <span class="divider">|</span>
              <a href="#" target="_blank">许可协议</a>
            </div>
          </div>
        </el-card>
      </el-col>
    </el-row>
  </div>
</template>

<script setup>
import { ref, onMounted, onBeforeUnmount } from 'vue';
import { ElMessage } from 'element-plus';
import { Folder } from '@element-plus/icons-vue';

// 活动标签页
const activeTab = ref('communication');

// 可用串口
const availablePorts = ref(['COM1', 'COM2', 'COM3', 'COM4']);

// 通信设置
const commSettings = ref({
  can: {
    deviceId: 1,
    baudRate: '500',
    channel: '1',
    termination: true
  }
});

// 预定义颜色
const predefineColors = ref([
  '#409EFF',
  '#67C23A',
  '#E6A23C',
  '#F56C6C',
  '#909399',
  '#7B54EA',
  '#26A69A',
  '#FF9800'
]);

// UI 设置
const uiSettings = ref({
  theme: 'light',
  primaryColor: '#409EFF',
  language: 'zh-CN',
  fontSize: 14,
  refreshRate: '500'
});

// 系统设置
const sysSettings = ref({
  autoSaveParams: false,
  exportPath: 'C:/Users/Documents/MoWheel'
});

// 设置主题
const setTheme = (theme) => {
  uiSettings.value.theme = theme;
  applyTheme(theme);
};

// 应用主题
const applyTheme = (theme) => {
  document.documentElement.setAttribute('data-theme', theme);
  if (theme === 'system') {
    const prefersDark = window.matchMedia('(prefers-color-scheme: dark)').matches;
    document.documentElement.classList.toggle('dark', prefersDark);
  } else {
    document.documentElement.classList.toggle('dark', theme === 'dark');
  }
};

// 更新主题色
const updateThemeColor = (color) => {
  document.documentElement.style.setProperty('--el-color-primary', color);
};

// 运行时间
const runtime = ref('00:00:00');
let runtimeTimer = null;
let seconds = 0;

// 选择导出路径
const selectExportPath = () => {
  // 这里应该调用 Tauri API 打开文件选择对话框
  ElMessage.info('选择导出路径功能将在未来版本实现');
};

// 更新运行时间
const updateRuntime = () => {
  seconds++;
  const hours = Math.floor(seconds / 3600);
  const minutes = Math.floor((seconds % 3600) / 60);
  const secs = seconds % 60;
  
  runtime.value = `${String(hours).padStart(2, '0')}:${String(minutes).padStart(2, '0')}:${String(secs).padStart(2, '0')}`;
};

// 保存设置
const saveSettings = () => {
  ElMessage.success('设置已保存');
};

// 恢复默认设置
const resetSettings = () => {
  ElMessage({
    message: '是否确定恢复默认设置？',
    type: 'warning',
    showClose: true,
    showCancelButton: true,
    confirmButtonText: '确定',
    cancelButtonText: '取消',
    beforeClose: (action, instance, done) => {
      if (action === 'confirm') {        // 恢复默认设置
        commSettings.value = {
          can: {
            deviceId: 1,
            baudRate: '500',
            channel: '1',
            termination: true
          }
        };
        
        uiSettings.value = {
          theme: 'light',
          primaryColor: '#409EFF',
          language: 'zh-CN',
          fontSize: 14,
          refreshRate: '500'
        };
        
        sysSettings.value = {
          autoSaveParams: false,
          exportPath: 'C:/Users/Documents/MoWheel'
        };
        
        // 应用默认主题
        applyTheme('light');
        updateThemeColor('#409EFF');
        
        ElMessage.success('已恢复默认设置');
      }
      done();
    }
  });
};

// 检查更新
const checkUpdate = () => {
  ElMessage.info('当前已是最新版本');
};

// 生命周期钩子
onMounted(() => {
  runtimeTimer = setInterval(updateRuntime, 1000);
  
  // 初始化主题
  applyTheme(uiSettings.value.theme);
  updateThemeColor(uiSettings.value.primaryColor);
  
  // 监听系统主题变化
  window.matchMedia('(prefers-color-scheme: dark)').addEventListener('change', (e) => {
    if (uiSettings.value.theme === 'system') {
      document.documentElement.classList.toggle('dark', e.matches);
    }
  });
});

onBeforeUnmount(() => {
  if (runtimeTimer) clearInterval(runtimeTimer);
  
  // 移除系统主题变化监听
  window.matchMedia('(prefers-color-scheme: dark)').removeEventListener('change', () => {});
});
</script>

<style scoped>
.system-settings-container {
  padding: 10px;
}

.button-row {
  margin-top: 20px;
  display: flex;
  justify-content: flex-end;
  gap: 10px;
}

.info-item {
  margin-bottom: 10px;
  display: flex;
  justify-content: space-between;
  border-bottom: 1px dashed #eee;
  padding-bottom: 5px;
}

.info-item .label {
  font-weight: bold;
  color: #606266;
}

.info-item .value {
  color: #409EFF;
}

.check-update {
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.about-content {
  text-align: center;
}

.about-logo {
  width: 80px;
  height: 80px;
  margin-bottom: 10px;
}

.copyright {
  font-size: 12px;
  color: #909399;
  margin-top: 20px;
}

.social-links {
  margin-top: 15px;
  font-size: 14px;
}

.social-links a {
  color: #409EFF;
  text-decoration: none;
}

.social-links .divider {
  margin: 0 10px;
  color: #dcdfe6;
}

.theme-selector {
  display: flex;
  gap: 20px;
}

.theme-item {
  display: flex;
  flex-direction: column;
  align-items: center;
  cursor: pointer;
  opacity: 0.6;
  transition: all 0.3s;
}

.theme-item.active {
  opacity: 1;
}

.theme-preview {
  width: 60px;
  height: 60px;
  border-radius: 8px;
  border: 2px solid transparent;
  margin-bottom: 8px;
  overflow: hidden;
  box-shadow: 0 2px 12px 0 rgba(0, 0, 0, 0.1);
}

.theme-item.active .theme-preview {
  border-color: var(--el-color-primary);
  transform: scale(1.05);
}

.light-theme {
  background-color: #ffffff;
}

.dark-theme {
  background-color: #303133;
}

.system-theme {
  position: relative;
  background-color: #ffffff;
}

.split-preview {
  position: absolute;
  right: 0;
  top: 0;
  width: 50%;
  height: 100%;
  background-color: #303133;
}

.font-size-preview {
  display: inline-block;
  margin-left: 15px;
  transition: font-size 0.3s;
}

.setting-description {
  font-size: 12px;
  color: #909399;
  margin-top: 5px;
  margin-left: 2px;
  line-height: 1.4;
}
</style>
