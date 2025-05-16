<template>
  <div class="data-monitor-container">
    <el-row :gutter="20">
      <!-- 实时曲线面板 -->
      <el-col :span="24">
        <el-card shadow="hover" header="实时数据监测">
          <div class="chart-toolbar">
            <el-space>
              <el-checkbox-group v-model="selectedParams" size="small">
                <el-checkbox-button label="speed">转速</el-checkbox-button>
                <el-checkbox-button label="torque">扭矩</el-checkbox-button>
                <el-checkbox-button label="voltage">电压</el-checkbox-button>
                <el-checkbox-button label="current">电流</el-checkbox-button>
                <el-checkbox-button label="temperature">温度</el-checkbox-button>
              </el-checkbox-group>
              
              <el-select v-model="timeRange" placeholder="时间范围" size="small">
                <el-option label="最近1分钟" value="1m"></el-option>
                <el-option label="最近5分钟" value="5m"></el-option>
                <el-option label="最近10分钟" value="10m"></el-option>
                <el-option label="最近30分钟" value="30m"></el-option>
              </el-select>
              
              <el-button size="small" type="primary" @click="startRecording" :disabled="recording">
                <el-icon><VideoPlay /></el-icon>
                开始记录
              </el-button>
              
              <el-button size="small" type="danger" @click="stopRecording" :disabled="!recording">
                <el-icon><VideoPause /></el-icon>
                停止记录
              </el-button>
              
              <el-button size="small" type="success" @click="exportData">
                <el-icon><Download /></el-icon>
                导出数据
              </el-button>
            </el-space>
          </div>
          
          <div ref="mainChartRef" class="main-chart"></div>
        </el-card>
      </el-col>
    </el-row>
    
    <el-row :gutter="20" style="margin-top: 20px;">
      <!-- 统计仪表盘 -->
      <el-col :xs="24" :sm="12" :md="6">
        <el-card shadow="hover" class="stat-card">
          <template #header>
            <div class="stat-header">
              <span>平均速度</span>
              <el-tag size="small">RPM</el-tag>
            </div>
          </template>
          <div class="stat-value">{{ statistics.avgSpeed }}</div>
          <el-progress 
            :percentage="(statistics.avgSpeed / 5000) * 100" 
            :color="customColors" 
            :show-text="false"
          ></el-progress>
        </el-card>
      </el-col>
      
      <el-col :xs="24" :sm="12" :md="6">
        <el-card shadow="hover" class="stat-card">
          <template #header>
            <div class="stat-header">
              <span>最大速度</span>
              <el-tag size="small">RPM</el-tag>
            </div>
          </template>
          <div class="stat-value">{{ statistics.maxSpeed }}</div>
          <el-progress 
            :percentage="(statistics.maxSpeed / 5000) * 100" 
            :color="customColors" 
            :show-text="false"
          ></el-progress>
        </el-card>
      </el-col>
      
      <el-col :xs="24" :sm="12" :md="6">
        <el-card shadow="hover" class="stat-card">
          <template #header>
            <div class="stat-header">
              <span>平均电流</span>
              <el-tag size="small">A</el-tag>
            </div>
          </template>
          <div class="stat-value">{{ statistics.avgCurrent }}</div>
          <el-progress 
            :percentage="(statistics.avgCurrent / 10) * 100" 
            :color="customColors" 
            :show-text="false"
          ></el-progress>
        </el-card>
      </el-col>
      
      <el-col :xs="24" :sm="12" :md="6">
        <el-card shadow="hover" class="stat-card">
          <template #header>
            <div class="stat-header">
              <span>最高温度</span>
              <el-tag size="small">°C</el-tag>
            </div>
          </template>
          <div class="stat-value">{{ statistics.maxTemp }}</div>
          <el-progress 
            :percentage="(statistics.maxTemp / 100) * 100" 
            :color="customColors" 
            :show-text="false"
          ></el-progress>
        </el-card>
      </el-col>
    </el-row>
    
    <el-row :gutter="20" style="margin-top: 20px;">
      <!-- 历史数据表格 -->
      <el-col :span="24">
        <el-card shadow="hover" header="历史数据记录">
          <div class="table-toolbar">
            <el-space>
              <el-date-picker
                v-model="dateRange"
                type="daterange"
                range-separator="至"
                start-placeholder="开始日期"
                end-placeholder="结束日期"
                size="small"
              />
              
              <el-button type="primary" size="small" @click="searchHistoryData">
                <el-icon><Search /></el-icon>
                查询
              </el-button>
              
              <el-button type="info" size="small" @click="resetSearch">
                <el-icon><RefreshLeft /></el-icon>
                重置
              </el-button>
            </el-space>
          </div>
          
          <el-table :data="historyData" stripe style="width: 100%" height="300">
            <el-table-column prop="timestamp" label="时间" width="180" sortable />
            <el-table-column prop="motor" label="电机" width="100" />
            <el-table-column prop="speed" label="速度(RPM)" width="120" sortable />
            <el-table-column prop="torque" label="扭矩(N·m)" width="120" sortable />
            <el-table-column prop="voltage" label="电压(V)" width="100" sortable />
            <el-table-column prop="current" label="电流(A)" width="100" sortable />
            <el-table-column prop="temperature" label="温度(°C)" width="100" sortable />
            <el-table-column prop="status" label="状态" width="100">
              <template #default="scope">
                <el-tag :type="scope.row.status === '运行' ? 'success' : 'info'">
                  {{ scope.row.status }}
                </el-tag>
              </template>
            </el-table-column>
          </el-table>
          
          <div class="pagination-container">
            <el-pagination
              v-model:current-page="currentPage"
              v-model:page-size="pageSize"
              :page-sizes="[10, 20, 50, 100]"
              layout="total, sizes, prev, pager, next, jumper"
              :total="totalItems"
              @size-change="handleSizeChange"
              @current-change="handleCurrentChange"
            />
          </div>
        </el-card>
      </el-col>
    </el-row>
  </div>
</template>

<script setup>
import { ref, onMounted, onBeforeUnmount } from 'vue';
import * as echarts from 'echarts';
import { 
  VideoPlay, 
  VideoPause, 
  Download, 
  Search, 
  RefreshLeft 
} from '@element-plus/icons-vue';
import { ElMessage, ElMessageBox } from 'element-plus';

// 实时图表相关
const mainChartRef = ref(null);
let mainChart = null;
const selectedParams = ref(['speed', 'current']);
const timeRange = ref('5m');
const recording = ref(false);
let chartTimer = null;

// 数据统计
const statistics = ref({
  avgSpeed: 3456,
  maxSpeed: 4780,
  avgCurrent: 6.5,
  maxTemp: 65
});

// 历史数据查询
const dateRange = ref([new Date(Date.now() - 7 * 24 * 60 * 60 * 1000), new Date()]);
const currentPage = ref(1);
const pageSize = ref(10);
const totalItems = ref(85);

// 自定义进度条颜色
const customColors = [
  { color: '#67C23A', percentage: 50 },
  { color: '#E6A23C', percentage: 75 },
  { color: '#F56C6C', percentage: 100 }
];

// 模拟历史数据
const historyData = ref([
  { timestamp: '2025-05-16 01:00:00', motor: '电机 1', speed: 3500, torque: 12.5, voltage: 220.5, current: 6.7, temperature: 58, status: '运行' },
  { timestamp: '2025-05-16 00:55:00', motor: '电机 1', speed: 3520, torque: 12.8, voltage: 220.2, current: 6.8, temperature: 57, status: '运行' },
  { timestamp: '2025-05-16 00:50:00', motor: '电机 1', speed: 3490, torque: 12.3, voltage: 220.4, current: 6.7, temperature: 56, status: '运行' },
  { timestamp: '2025-05-16 00:45:00', motor: '电机 1', speed: 3510, torque: 12.4, voltage: 221.0, current: 6.7, temperature: 56, status: '运行' },
  { timestamp: '2025-05-16 00:40:00', motor: '电机 1', speed: 3500, torque: 12.5, voltage: 220.8, current: 6.6, temperature: 55, status: '运行' },
  { timestamp: '2025-05-16 00:35:00', motor: '电机 1', speed: 3480, torque: 12.2, voltage: 220.5, current: 6.5, temperature: 55, status: '运行' },
  { timestamp: '2025-05-16 00:30:00', motor: '电机 1', speed: 3470, torque: 12.0, voltage: 220.7, current: 6.5, temperature: 54, status: '运行' },
  { timestamp: '2025-05-16 00:25:00', motor: '电机 1', speed: 3460, torque: 11.9, voltage: 220.6, current: 6.4, temperature: 54, status: '运行' },
  { timestamp: '2025-05-16 00:20:00', motor: '电机 1', speed: 0, torque: 0, voltage: 220.0, current: 0.2, temperature: 45, status: '停止' },
  { timestamp: '2025-05-16 00:15:00', motor: '电机 1', speed: 0, torque: 0, voltage: 220.0, current: 0.2, temperature: 40, status: '停止' }
]);

// 初始化主图表
const initMainChart = () => {
  if (mainChartRef.value) {
    mainChart = echarts.init(mainChartRef.value);
    
    const option = {
      tooltip: {
        trigger: 'axis',
        axisPointer: {
          type: 'cross'
        }
      },
      legend: {
        data: ['转速', '扭矩', '电压', '电流', '温度']
      },
      grid: {
        left: '3%',
        right: '4%',
        bottom: '3%',
        containLabel: true
      },
      xAxis: {
        type: 'category',
        boundaryGap: false,
        data: []
      },
      yAxis: [
        {
          type: 'value',
          name: '转速/电压',
          position: 'left',
          axisLine: {
            show: true,
            lineStyle: {
              color: '#5470C6'
            }
          },
          axisLabel: {
            formatter: '{value}'
          }
        },
        {
          type: 'value',
          name: '电流/扭矩/温度',
          position: 'right',
          axisLine: {
            show: true,
            lineStyle: {
              color: '#91CC75'
            }
          },
          axisLabel: {
            formatter: '{value}'
          }
        }
      ],
      series: [
        {
          name: '转速',
          type: 'line',
          smooth: true,
          yAxisIndex: 0,
          data: [],
          symbol: 'none',
          lineStyle: {
            width: 2
          },
          emphasis: {
            focus: 'series'
          }
        },
        {
          name: '扭矩',
          type: 'line',
          smooth: true,
          yAxisIndex: 1,
          data: [],
          symbol: 'none',
          lineStyle: {
            width: 2
          },
          emphasis: {
            focus: 'series'
          }
        },
        {
          name: '电压',
          type: 'line',
          smooth: true,
          yAxisIndex: 0,
          data: [],
          symbol: 'none',
          lineStyle: {
            width: 2
          },
          emphasis: {
            focus: 'series'
          }
        },
        {
          name: '电流',
          type: 'line',
          smooth: true,
          yAxisIndex: 1,
          data: [],
          symbol: 'none',
          lineStyle: {
            width: 2
          },
          emphasis: {
            focus: 'series'
          }
        },
        {
          name: '温度',
          type: 'line',
          smooth: true,
          yAxisIndex: 1,
          data: [],
          symbol: 'none',
          lineStyle: {
            width: 2
          },
          emphasis: {
            focus: 'series'
          }
        }
      ],
      dataZoom: [
        {
          type: 'inside',
          start: 0,
          end: 100
        },
        {
          start: 0,
          end: 100
        }
      ]
    };
    
    mainChart.setOption(option);
    
    // 更新图表数据
    updateChartData();
  }
};

// 更新图表数据
const updateChartData = () => {
  // 生成时间数据点
  const timePoints = [];
  const now = new Date();
  for (let i = 0; i < 60; i++) {
    const time = new Date(now - (59 - i) * 1000);
    timePoints.push(`${time.getHours()}:${String(time.getMinutes()).padStart(2, '0')}:${String(time.getSeconds()).padStart(2, '0')}`);
  }
  
  // 模拟数据序列
  const speedData = Array.from({ length: 60 }, () => Math.floor(Math.random() * 500 + 3300));
  const torqueData = Array.from({ length: 60 }, () => parseFloat((Math.random() * 2 + 11).toFixed(1)));
  const voltageData = Array.from({ length: 60 }, () => parseFloat((Math.random() * 2 + 219).toFixed(1)));
  const currentData = Array.from({ length: 60 }, () => parseFloat((Math.random() * 1 + 6).toFixed(2)));
  const tempData = Array.from({ length: 60 }, () => Math.floor(Math.random() * 5 + 55));
  
  mainChart.setOption({
    xAxis: {
      data: timePoints
    },
    series: [
      {
        name: '转速',
        data: speedData
      },
      {
        name: '扭矩',
        data: torqueData
      },
      {
        name: '电压',
        data: voltageData
      },
      {
        name: '电流',
        data: currentData
      },
      {
        name: '温度',
        data: tempData
      }
    ]
  });
};

// 开始记录数据
const startRecording = () => {
  recording.value = true;
  ElMessage.success('开始记录数据');
  
  // 实时更新图表
  chartTimer = setInterval(() => {
    if (mainChart) {
      mainChart.setOption({
        series: [
          {
            name: '转速',
            data: function(old) {
              old.shift();
              old.push(Math.floor(Math.random() * 500 + 3300));
              return old;
            }
          },
          {
            name: '扭矩',
            data: function(old) {
              old.shift();
              old.push(parseFloat((Math.random() * 2 + 11).toFixed(1)));
              return old;
            }
          },
          {
            name: '电压',
            data: function(old) {
              old.shift();
              old.push(parseFloat((Math.random() * 2 + 219).toFixed(1)));
              return old;
            }
          },
          {
            name: '电流',
            data: function(old) {
              old.shift();
              old.push(parseFloat((Math.random() * 1 + 6).toFixed(2)));
              return old;
            }
          },
          {
            name: '温度',
            data: function(old) {
              old.shift();
              old.push(Math.floor(Math.random() * 5 + 55));
              return old;
            }
          }
        ],
        xAxis: {
          data: function(old) {
            const now = new Date();
            old.shift();
            old.push(`${now.getHours()}:${String(now.getMinutes()).padStart(2, '0')}:${String(now.getSeconds()).padStart(2, '0')}`);
            return old;
          }
        }
      });
    }
  }, 1000);
};

// 停止记录数据
const stopRecording = () => {
  recording.value = false;
  clearInterval(chartTimer);
  ElMessage.info('已停止记录数据');
};

// 导出数据
const exportData = () => {
  ElMessageBox.confirm(
    '确定要将当前显示的数据导出为CSV文件吗？',
    '导出数据',
    {
      confirmButtonText: '确定',
      cancelButtonText: '取消',
      type: 'info'
    }
  ).then(() => {
    ElMessage({
      type: 'success',
      message: '数据已导出为CSV文件'
    });
  }).catch(() => {});
};

// 查询历史数据
const searchHistoryData = () => {
  // 这里应调用 API 获取实际数据
  ElMessage.success('查询成功');
};

// 重置搜索条件
const resetSearch = () => {
  dateRange.value = [new Date(Date.now() - 7 * 24 * 60 * 60 * 1000), new Date()];
  currentPage.value = 1;
  searchHistoryData();
};

// 分页处理
const handleSizeChange = (newSize) => {
  pageSize.value = newSize;
  searchHistoryData();
};

const handleCurrentChange = (newPage) => {
  currentPage.value = newPage;
  searchHistoryData();
};

// 生命周期钩子
onMounted(() => {
  initMainChart();
  window.addEventListener('resize', handleResize);
});

onBeforeUnmount(() => {
  window.removeEventListener('resize', handleResize);
  if (chartTimer) clearInterval(chartTimer);
  if (mainChart) mainChart.dispose();
});

const handleResize = () => {
  if (mainChart) mainChart.resize();
};
</script>

<style scoped>
.data-monitor-container {
  padding: 10px;
}

.chart-toolbar {
  margin-bottom: 15px;
  display: flex;
  justify-content: space-between;
}

.main-chart {
  height: 400px;
  width: 100%;
}

.stat-card {
  margin-bottom: 20px;
}

.stat-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.stat-value {
  font-size: 28px;
  font-weight: bold;
  color: #409EFF;
  margin: 15px 0;
  text-align: center;
}

.table-toolbar {
  margin-bottom: 15px;
}

.pagination-container {
  margin-top: 15px;
  display: flex;
  justify-content: center;
}
</style>
