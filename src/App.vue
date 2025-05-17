<script setup>
import { ref, onMounted } from 'vue';
import Layout from './components/Layout.vue';
import { useAppStore } from './store';

const appStore = useAppStore();
const isLoading = ref(true);

// 页面加载时触发
onMounted(() => {
  // 初始化主题
  appStore.initTheme();
  
  // 初始化一些演示数据
  setTimeout(() => {
    // 模拟数据加载完成
    isLoading.value = false;
    console.log('App组件已经挂载，数据已加载完成');
  }, 500);
});
</script>

<template>
  <div v-if="isLoading" class="app-loading">
    <div class="loading-spinner"></div>
    <div class="loading-text">加载中...</div>
  </div>
  <Layout v-else />
</template>

<style>
/* 全局样式 */
html, body {
  margin: 0;
  padding: 0;
  height: 100%;
  font-family: 'Helvetica Neue', Helvetica, 'PingFang SC', 'Hiragino Sans GB', 'Microsoft YaHei', '微软雅黑', Arial, sans-serif;
}

#app {
  height: 100%;
}

.app-loading {
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;
  background-color: #f5f7fa;
  z-index: 9999;
}

.loading-spinner {
  width: 40px;
  height: 40px;
  border: 4px solid #e4e7ed;
  border-top: 4px solid #409EFF;
  border-radius: 50%;
  animation: spin 1s linear infinite;
}

.loading-text {
  margin-top: 15px;
  font-size: 16px;
  color: #606266;
}

@keyframes spin {
  0% { transform: rotate(0deg); }
  100% { transform: rotate(360deg); }
}
</style>


