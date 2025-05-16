import { createRouter, createWebHistory } from 'vue-router';

const routes = [
  {
    path: '/',
    name: 'Home',
    component: () => import('../views/Home.vue'),
    meta: {
      title: '首页',
      icon: 'HomeFilled'
    }
  },  {
    path: '/motor-control',
    name: 'MotorControl',
    component: () => import('../views/MotorControl.vue'),
    meta: {
      title: '电机控制',
      icon: 'Monitor'
    }
  },
  {
    path: '/system-settings',
    name: 'SystemSettings',
    component: () => import('../views/SystemSettings.vue'),
    meta: {
      title: '系统设置',
      icon: 'Tools'
    }
  }
];

const router = createRouter({
  history: createWebHistory(),
  routes
});

export default router;
