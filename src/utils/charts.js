// charts.js - 图表工具函数
import * as echarts from 'echarts';

// 全局图表实例缓存
const chartInstances = new Map();

/**
 * 创建或重新初始化图表
 * @param {HTMLElement} el - 图表容器元素
 * @param {String} id - 图表唯一标识
 * @param {Object} options - 图表配置选项
 * @returns {Object} 图表实例
 */
export function initChart(el, id, options) {
  // 如果已存在实例，先销毁它
  disposeChart(id);
  
  // 创建新实例
  try {
    const chart = echarts.init(el);
    chart.setOption(options);
    chartInstances.set(id, chart);
    return chart;
  } catch (error) {
    console.error('初始化图表失败:', error);
    return null;
  }
}

/**
 * 更新图表数据
 * @param {String} id - 图表唯一标识
 * @param {Object} options - 新的图表选项或数据
 * @returns {Boolean} 是否更新成功
 */
export function updateChart(id, options) {
  if (!chartInstances.has(id)) return false;
  
  try {
    const chart = chartInstances.get(id);
    chart.setOption(options);
    return true;
  } catch (error) {
    console.error('更新图表失败:', error);
    return false;
  }
}

/**
 * 调整图表大小
 * @param {String} id - 图表唯一标识
 */
export function resizeChart(id) {
  if (!chartInstances.has(id)) return;
  
  try {
    const chart = chartInstances.get(id);
    chart.resize();
  } catch (error) {
    console.error('调整图表大小失败:', error);
  }
}

/**
 * 销毁图表实例
 * @param {String} id - 图表唯一标识
 */
export function disposeChart(id) {
  if (!chartInstances.has(id)) return;
  
  try {
    const chart = chartInstances.get(id);
    chart.dispose();
    chartInstances.delete(id);
  } catch (error) {
    console.error('销毁图表失败:', error);
  }
}

/**
 * 销毁所有图表实例
 */
export function disposeAllCharts() {
  chartInstances.forEach((chart, id) => {
    try {
      chart.dispose();
    } catch (error) {
      console.error(`销毁图表 ${id} 失败:`, error);
    }
  });
  
  chartInstances.clear();
}

/**
 * 窗口大小变化时调整所有图表大小
 */
export function resizeAllCharts() {
  chartInstances.forEach((chart, id) => {
    try {
      chart.resize();
    } catch (error) {
      console.error(`调整图表 ${id} 大小失败:`, error);
    }
  });
}

// 添加窗口大小变化监听
window.addEventListener('resize', resizeAllCharts);
