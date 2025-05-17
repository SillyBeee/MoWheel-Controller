// Learn more about Tauri commands at https://tauri.app/develop/calling-rust/
use serde::{Serialize, Deserialize};

mod motor_control;

#[derive(Debug, Serialize)]
struct SerialPortInfo {
    name: String,
    port_type: String,
    port_number: Option<u8>,
    // 可以添加更多字段，如波特率等
}

#[derive(Debug, Serialize, Deserialize)]
struct MotorCommand {
    motor_id: u8,
    command_type: String,  // "start", "stop", "set_speed", "set_position", "set_torque"
    value: f64,
}

#[tauri::command]
fn greet(name: &str) -> String {
    format!("Hello, {}! You've been greeted from Rust!", name)
}

#[tauri::command]
fn get_available_ports() -> Result<Vec<SerialPortInfo>, String> {
    let ports = serialport::available_ports()
        .map_err(|e| format!("无法获取可用串口：{}", e))?;
    
    let mut port_list: Vec<SerialPortInfo> = Vec::new();
    
    for port in ports {
        // 构建端口信息
        let port_type = match &port.port_type {
            serialport::SerialPortType::UsbPort(info) => {
                format!("USB ({:04x}:{:04x})", info.vid, info.pid)
            }
            serialport::SerialPortType::PciPort => String::from("PCI"),
            serialport::SerialPortType::BluetoothPort => String::from("蓝牙"),
            _ => String::from("其他"),
        };
        
        let port_number = if let serialport::SerialPortType::UsbPort(_) = &port.port_type {
            None // USB SerialPortInfo 没有 port_number 字段，我们移除这个属性
        } else {
            None
        };

        port_list.push(SerialPortInfo {
            name: port.port_name,
            port_type,
            port_number,
        });
    }
    
    Ok(port_list)
}

#[tauri::command]
fn connect_to_port(port_name: &str, baud_rate: u32) -> Result<String, String> {
    motor_control::open_serial_port(port_name, baud_rate)
}

#[tauri::command]
fn disconnect_port() -> Result<String, String> {
    motor_control::close_serial_port()
}

#[tauri::command]
fn is_port_connected() -> bool {
    motor_control::is_port_connected()
}

#[tauri::command]
fn send_motor_command(command: MotorCommand) -> Result<String, String> {
    // 将MotorCommand转换为motor_control::MotorCommand
    let motor_command = motor_control::MotorCommand {
        motor_id: command.motor_id,
        command_type: command.command_type,
        value: command.value,
    };
    motor_control::send_motor_command(motor_command)
}

#[tauri::command]
fn read_motor_status(motor_id: u8) -> Result<motor_control::MotorStatus, String> {
    motor_control::read_motor_status(motor_id)
}

#[cfg_attr(mobile, tauri::mobile_entry_point)]
pub fn run() {
    tauri::Builder::default()
        .plugin(tauri_plugin_opener::init())
        .invoke_handler(tauri::generate_handler![
            greet,
            get_available_ports,
            connect_to_port,
            disconnect_port,
            is_port_connected,
            send_motor_command,
            read_motor_status
        ])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}
