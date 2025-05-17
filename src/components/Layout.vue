<template>
  <div class="app-wrapper">
    <!-- 侧边栏 -->
    <div class="sidebar-container" :class="{'is-collapse': !sidebarOpen}">
      <div class="sidebar-logo">
        <img src="/tauri.svg" alt="logo">
        <span class="title" v-if="sidebarOpen">MoWheel Controller</span>
      </div>
      
      <el-menu
        :default-active="activeRoute"
        :collapse="!sidebarOpen"
        :unique-opened="true"
        text-color="#fff"
        active-text-color="#409EFF"
        background-color="#304156"
        :collapse-transition="false"
        mode="vertical"
        router
      >
        <el-menu-item v-for="route in routes" :key="route.path" :index="route.path">
          <el-icon>
            <component :is="route.meta.icon"></component>
          </el-icon>
          <span>{{ route.meta.title }}</span>
        </el-menu-item>
      </el-menu>
    </div>
    
    <!-- 主容器 -->
    <div class="main-container" :class="{'is-collapse': !sidebarOpen}">
      <div class="header-container">
        <div class="left-area">
          <el-button type="text" @click="toggleSidebar">
            <el-icon :size="20">
              <Fold v-if="sidebarOpen" />
              <Expand v-else />
            </el-icon>
          </el-button>
          <el-breadcrumb separator="/" class="breadcrumb">
            <el-breadcrumb-item :to="{ path: '/' }">首页</el-breadcrumb-item>
            <el-breadcrumb-item>{{ currentTitle }}</el-breadcrumb-item>
          </el-breadcrumb>
        </div>
        
        <div class="right-area">
          <el-space>
            <el-select 
              v-model="selectedPort" 
              placeholder="选择串口" 
              size="small" 
              @change="handlePortChange"
              :disabled="connectionStatus"
            >
              <el-option
                v-for="port in serialPorts"
                :key="port"
                :label="port"
                :value="port"
              ></el-option>
            </el-select>
            
            <el-button 
              :type="connectionStatus ? 'danger' : 'primary'" 
              size="small" 
              @click="toggleConnection"
            >
              {{ connectionStatus ? '断开连接' : '连接' }}
            </el-button>
            
            <el-tooltip content="帮助" placement="bottom">
              <el-button type="info" icon="QuestionFilled" circle size="small"></el-button>
            </el-tooltip>
            
            <el-dropdown trigger="click">
              <el-avatar :size="32" src="https://cube.elemecdn.com/0/88/03b0d39583f48206768a7534e55bcpng.png"></el-avatar>
              <template #dropdown>
                <el-dropdown-menu>
                  <el-dropdown-item>个人中心</el-dropdown-item>
                  <el-dropdown-item>设置</el-dropdown-item>
                  <el-dropdown-item divided>退出登录</el-dropdown-item>
                </el-dropdown-menu>
              </template>
            </el-dropdown>
          </el-space>        </div>      </div>      <div class="content-container">
        <!-- 路由视图 -->
        <router-view v-slot="{ Component }">
          <keep-alive>
            <component 
              :is="Component" 
              :key="$route.fullPath"
            />
          </keep-alive>
        </router-view>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, computed, onMounted, onBeforeUnmount } from 'vue';
import { useRouter, useRoute } from 'vue-router';
import { 
  HomeFilled, 
  Monitor, 
  DataLine, 
  Setting, 
  Tools, 
  Fold, 
  Expand, 
  QuestionFilled 
} from '@element-plus/icons-vue';
import { useAppStore } from '../store';

const router = useRouter();
const route = useRoute();
const appStore = useAppStore();

// 从 store 获取状态
const sidebarOpen = computed(() => appStore.sidebar.opened);
const selectedPort = computed({
  get: () => appStore.selectedPort,
  set: (val) => appStore.setSelectedPort(val)
});
const connectionStatus = computed(() => appStore.connectionStatus);
const serialPorts = computed(() => appStore.serialPorts);

// 导航菜单
const routes = router.getRoutes().filter(route => route.meta && route.meta.title && route.meta.icon);

// 当前活动路由
const activeRoute = computed(() => route.path);
const currentTitle = computed(() => 
  routes.find(r => r.path === route.path)?.meta?.title || '未知页面'
);

// 切换侧边栏
const toggleSidebar = () => {
  appStore.toggleSidebar();
};

// 处理串口选择
const handlePortChange = (port) => {
  appStore.setSelectedPort(port);
};

// 切换连接状态
const toggleConnection = () => {
  if (connectionStatus.value) {
    // 断开连接
    appStore.setConnectionStatus(false);
    // 这里应该调用 Tauri API 来断开串口连接
  } else {
    // 建立连接
    if (selectedPort.value) {
      appStore.setConnectionStatus(true);
      // 这里应该调用 Tauri API 来建立串口连接
    } else {
      ElMessage.warning('请先选择串口');
    }
  }
};

onMounted(() => {
  // 模拟获取串口列表
  // 实际中应该调用 Tauri API 获取可用串口
  appStore.setSerialPorts(['COM1', 'COM2', 'COM3', 'COM4']);
  
  // 添加事件监听器以确保侧边栏响应性
  window.addEventListener('resize', handleResize);
});

onBeforeUnmount(() => {
  // 移除事件监听器，避免内存泄漏
  window.removeEventListener('resize', handleResize);
});

// 处理窗口大小变化
const handleResize = () => {
  if (document.body.clientWidth < 768) {
    appStore.closeSidebar({ withoutAnimation: true });
  }
};
</script>

<style scoped>
.app-wrapper {
  height: 100%;
  width: 100%;
  position: relative;
}

.sidebar-container {
  transition: width 0.28s;
  width: 210px;
  height: 100%;
  position: fixed;
  top: 0;
  bottom: 0;
  left: 0;
  z-index: 1001;
  overflow: hidden;
  background-color: #304156;
}

.sidebar-container.is-collapse {
  width: 64px;
}

.sidebar-logo {
  height: 50px;
  line-height: 50px;
  text-align: center;
  overflow: hidden;
  background-color: #2b3649;
  padding: 0 10px;
}

.sidebar-logo img {
  display: inline-block;
  height: 32px;
  width: 32px;
  vertical-align: middle;
}

.sidebar-logo .title {
  display: inline-block;
  margin-left: 10px;
  color: #fff;
  font-weight: 600;
  line-height: 50px;
  font-size: 14px;
  vertical-align: middle;
}

.main-container {
  min-height: 100%;
  transition: margin-left 0.28s;
  margin-left: 210px;
  position: relative;
}

.main-container.is-collapse {
  margin-left: 64px;
}

.header-container {
  height: 50px;
  background-color: #fff;
  box-shadow: 0 1px 4px rgba(0, 21, 41, 0.08);
  position: relative;
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 0 15px;
}

.left-area, .right-area {
  display: flex;
  align-items: center;
}

.breadcrumb {
  margin-left: 15px;
  font-weight: bold;
}

.content-container {
  padding: 15px;
  min-height: calc(100vh - 50px);
  background-color: #f0f2f5;
}

/* Transition动画 */
.fade-transform-enter-active,
.fade-transform-leave-active {
  transition: all 0.5s;
}

.fade-transform-enter-from {
  opacity: 0;
  transform: translateX(-30px);
}

.fade-transform-leave-to {
  opacity: 0;
  transform: translateX(30px);
}
</style>
