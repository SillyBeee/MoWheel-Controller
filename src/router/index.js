import { createRouter, createWebHistory } from 'vue-router';

const routes = [  {
    path: '/',
    name: 'Home',
    component: () => import('../views/Home.vue'),
    meta: {
      title: '首页',
      icon: 'HomeFilled',
      keepAlive: true
    }
  },{
    path: '/motor-control',
    name: 'MotorControl',
    component: () => import('../views/SimpleMotorControl.vue'),
    meta: {
      title: '电机控制',
      icon: 'Monitor',
      keepAlive: true
    }
  },{
    path: '/system-settings',
    name: 'SystemSettings',
    component: () => import('../views/SystemSettings.vue'),
    meta: {
      title: '系统设置',
      icon: 'Tools',
      keepAlive: true
    }
  }
];

const router = createRouter({
  history: createWebHistory(),
  routes
});

// 全局前置守卫
router.beforeEach((to, from, next) => {
  // 更新文档标题
  document.title = to.meta.title ? `${to.meta.title} - MoWheel Controller` : 'MoWheel Controller';
  
  // 尝试清理可能的内存泄漏
  if (from.name === 'MotorControl') {
    console.log('从电机控制页面离开，执行清理操作');
    
    // 触发全局事件，让组件有机会清理资源
    window.dispatchEvent(new CustomEvent('cleanupMotorControl'));
    
    // 清理DOM中可能存在的图表实例
    const charts = document.querySelectorAll('.echarts-container');
    if (charts && charts.length > 0) {
      console.log(`清理 ${charts.length} 个图表实例`);
    }
  }
  
  next();
});

export default router;
