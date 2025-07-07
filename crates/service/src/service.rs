use std::time::Duration;

use windows_service::service::{ServiceAccess, ServiceState};
use windows_service::service_manager::{ServiceManager, ServiceManagerAccess};

#[derive(Debug, Clone)]
pub enum ServiceQueryResult {
    Running,
    Stopped,
    Starting,
    Stopping,
    Paused,
    Unknown,
}

/// 查询服务状态，返回枚举类型
pub fn query_service_status(service_name: &str) -> Result<ServiceQueryResult, String> {
    let manager = ServiceManager::local_computer(None::<&str>, ServiceManagerAccess::CONNECT)
        .map_err(|e| format!("无法连接服务管理器: {:?}", e))?;

    let service = manager
        .open_service(service_name, ServiceAccess::QUERY_STATUS)
        .map_err(|e| format!("无法打开服务 {}: {:?}", service_name, e))?;

    let status = service.query_status().map_err(|e| format!("查询服务 {} 状态失败: {:?}", service_name, e))?;

    Ok(match status.current_state {
        ServiceState::Running => ServiceQueryResult::Running,
        ServiceState::Stopped => ServiceQueryResult::Stopped,
        ServiceState::StartPending => ServiceQueryResult::Starting,
        ServiceState::StopPending => ServiceQueryResult::Stopping,
        ServiceState::Paused => ServiceQueryResult::Paused,
        _ => ServiceQueryResult::Unknown,
    })
}

/// 启动服务，返回 bool 表示是否成功
pub fn start_service(service_name: &str) -> bool {
    match with_service_handle(service_name, ServiceAccess::START) {
        Ok(service) => {
            // 先检查当前状态
            if let Ok(ServiceQueryResult::Running) = query_service_status(service_name) {
                println!("服务 '{}' 已经在运行", service_name);
                return true;
            }

            // 尝试启动服务（无参数）
            if let Err(e) = service.start::<&str>(&[]) {
                eprintln!("启动服务 {} 失败: {:?}", service_name, e);
                false
            } else {
                wait_for_service_state(service_name, ServiceState::Running, Duration::from_secs(10)).is_ok()
            }
        }
        Err(e) => {
            eprintln!("{}", e);
            false
        }
    }
}

/// 停止服务，返回 bool 表示是否成功
pub fn stop_service(service_name: &str) -> bool {
    match with_service_handle(service_name, ServiceAccess::STOP) {
        Ok(service) => {
            // 先检查当前状态
            if let Ok(ServiceQueryResult::Stopped) = query_service_status(service_name) {
                println!("服务 '{}' 已经停止", service_name);
                return true;
            }

            // 发送停止指令
            if let Err(e) = service.stop() {
                eprintln!("停止服务 {} 失败: {:?}", service_name, e);
                return false;
            }

            // 等待服务完全停止
            wait_for_service_state(service_name, ServiceState::Stopped, Duration::from_secs(10)).is_ok()
        }
        Err(e) => {
            eprintln!("{}", e);
            false
        }
    }
}

/// 获取服务句柄
fn with_service_handle(service_name: &str, access: ServiceAccess) -> Result<windows_service::service::Service, String> {
    let manager = ServiceManager::local_computer(None::<&str>, ServiceManagerAccess::CONNECT)
        .map_err(|e| format!("无法连接服务管理器:{:?}", e))?;

    manager.open_service(service_name, access).map_err(|e| format!("无法打开服务{}:{:?}", service_name, e))
}

/// 等待服务进入句柄
fn wait_for_service_state(service_name: &str, target_state: ServiceState, timeout: Duration) -> Result<(), String> {
    let start_time = std::time::Instant::now();
    while start_time.elapsed() < timeout {
        match query_service_status(service_name) {
            Ok(ServiceQueryResult::Running) if target_state == ServiceState::Running => {
                return Ok(());
            }
            Ok(ServiceQueryResult::Stopped) if target_state == ServiceState::Stopped => {
                return Ok(());
            }
            _ => std::thread::sleep(Duration::from_millis(500)),
        }
    }

    Err(format!("等待服务 '{}' 进入状态 {:?} 超时", service_name, target_state))
}
