import { createApp } from "vue";
import ElementPlus from 'element-plus';
import 'element-plus/dist/index.css';
import * as ElementPlusIconsVue from '@element-plus/icons-vue';
import App from "./App.vue";
import router from "./router";
import { createPinia } from 'pinia';
import './styles/index.css';
import './styles/theme.css';

const app = createApp(App);
const pinia = createPinia();

// 注册所有Element Plus图标组件
for (const [key, component] of Object.entries(ElementPlusIconsVue)) {
  app.component(key, component);
}

app.use(ElementPlus)
   .use(router)
   .use(pinia)
   .mount("#app");
