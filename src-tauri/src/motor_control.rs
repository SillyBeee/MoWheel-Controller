use std::sync::{Arc, Mutex};
use std::time::Duration;
use std::io::{Read, Write, ErrorKind};
use serde::{Serialize, Deserialize};
use serialport::SerialPort;

// 全局存储串口连接
lazy_static::lazy_static! {
    static ref SERIAL_CONNECTION: Arc<Mutex<Option<Box<dyn SerialPort>>>> = Arc::new(Mutex::new(None));
}

#[derive(Debug, Serialize, Deserialize)]
pub struct MotorCommand {
    pub motor_id: u8,
    pub command_type: String,  // "start", "stop", "set_speed", "set_position", "set_torque"
    pub value: f64,
}

#[derive(Debug, Serialize, Deserialize)]
pub struct MotorStatus {
    pub id: String,
    pub speed: f64,
    pub position: f64,
    pub torque: f64,
    pub temperature: f64,
    pub voltage: f64,
    pub current: f64,
    pub connected: bool,
    pub running: bool,
}

// 打开并保存串口连接
pub fn open_serial_port(port_name: &str, baud_rate: u32) -> Result<String, String> {
    match serialport::new(port_name, baud_rate)
        .timeout(Duration::from_millis(50))
        .open()
    {
        Ok(port) => {
            let mut port_mutex = SERIAL_CONNECTION.lock().unwrap();
            *port_mutex = Some(port);
            Ok(format!("成功连接到 {}", port_name))
        }
        Err(e) => Err(format!("无法连接到串口: {}", e)),
    }
}

// 关闭串口连接
pub fn close_serial_port() -> Result<String, String> {
    let mut port_mutex = SERIAL_CONNECTION.lock().unwrap();
    if port_mutex.is_none() {
        return Err("没有活动的串口连接".to_string());
    }
    
    *port_mutex = None;
    Ok("已断开串口连接".to_string())
}

// 检查串口连接状态
pub fn is_port_connected() -> bool {
    let port_mutex = SERIAL_CONNECTION.lock().unwrap();
    port_mutex.is_some()
}

// 发送命令到电机
pub fn send_motor_command(command: MotorCommand) -> Result<String, String> {
    let mut port_mutex = SERIAL_CONNECTION.lock().unwrap();
    
    if port_mutex.is_none() {
        return Err("没有活动的串口连接".to_string());
    }
    
    let port = port_mutex.as_mut().unwrap();
    
    // 实现电机通信协议
    // 这里为简单实现，我们使用基本的格式：ID:命令类型:值\n
    let command_str = format!(
        "{}:{}:{}\n", 
        command.motor_id, 
        command.command_type, 
        command.value
    );
    
    match port.write_all(command_str.as_bytes()) {
        Ok(_) => Ok(format!(
            "成功发送命令: ID={}, 类型={}, 值={}", 
            command.motor_id, 
            command.command_type, 
            command.value
        )),
        Err(e) => Err(format!("发送命令失败: {}", e)),
    }
}

// 读取电机状态
pub fn read_motor_status(motor_id: u8) -> Result<MotorStatus, String> {
    let mut port_mutex = SERIAL_CONNECTION.lock().unwrap();
    
    if port_mutex.is_none() {
        return Err("没有活动的串口连接".to_string());
    }
    
    let port = port_mutex.as_mut().unwrap();
    
    // 发送查询命令
    let query_command = format!("{}:query:0\n", motor_id);
    match port.write_all(query_command.as_bytes()) {
        Ok(_) => {
            // 等待并读取响应
            std::thread::sleep(Duration::from_millis(50));
            
            let mut response = Vec::new();
            let mut buffer = [0; 128];
            
            // 读取可用数据
            loop {                match port.read(&mut buffer) {
                    Ok(0) => break, // 没有更多数据
                    Ok(n) => {
                        response.extend_from_slice(&buffer[..n]);
                        if response.contains(&b'\n') {
                            break;
                        }
                    }
                    Err(e) if e.kind() == ErrorKind::TimedOut => break,
                    Err(e) => return Err(format!("读取响应失败: {}", e)),
                }
            }
            
            // 解析响应
            if response.is_empty() {
                return Err("未收到电机响应".to_string());
            }
            
            // 在实际应用中，这里应该根据您的协议解析响应
            // 这里只是一个简化的示例
            
            // 模拟解析结果
            Ok(MotorStatus {
                id: motor_id.to_string(),
                speed: 1000.0,
                position: 0.0,
                torque: 0.5,
                temperature: 35.0,
                voltage: 24.0,
                current: 1.5,
                connected: true,
                running: true,
            })
        }
        Err(e) => Err(format!("发送查询命令失败: {}", e)),
    }
}
