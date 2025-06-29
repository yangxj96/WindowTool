mod util;

use dialoguer::{Select, theme::ColorfulTheme};
use std::io;

use util::registry_util::RegistryUtil;
use util::windows_service_util::ServiceQueryResult;
use util::windows_service_util::WindowsServiceUtil;

const DEFAULT_SERVICES: &[&str] = &["MySQL", "Redis", "PostgreSQL", "INODE_SVR_SERVICE"];

fn main() -> io::Result<()> {
    env_logger::init(); // 可选日志支持

    println!("🔧 欢迎使用小工具");
    let mut services = load_default_services();

    loop {
        let options = &[
            "🔍 查询默认服务状态",
            "🟢 一键启动所有服务",
            "🔴 一键停止所有服务",
            "🔄 单独操作某个服务",
            "🧹 无限试用Navicat",
            "❌ 退出",
        ];

        let selection = Select::with_theme(&ColorfulTheme::default())
            .items(options)
            .default(0)
            .interact()
            .expect("无法读取用户输入");

        match selection {
            0 => display_service_status(&services),
            1 => batch_start_services(&mut services),
            2 => batch_stop_services(&mut services),
            3 => individual_service_control(&mut services),
            4 => RegistryUtil::execute_registry_cleanup(),
            5 => {
                println!("👋 正在退出程序...");
                break;
            }
            _ => unreachable!(),
        }
    }

    Ok(())
}

// 加载默认服务及其初始状态
fn load_default_services() -> Vec<ServiceInfo> {
    DEFAULT_SERVICES
        .iter()
        .map(|&name| ServiceInfo {
            name: name.to_string(),
            status: WindowsServiceUtil::query_service_status(name)
                .unwrap_or(ServiceQueryResult::Unknown),
        })
        .collect()
}

// 显示服务状态
fn display_service_status(services: &[ServiceInfo]) {
    println!("\n📋 当前服务状态：");
    for service in services {
        let status_str = match service.status {
            ServiceQueryResult::Running => "🟢 运行中",
            ServiceQueryResult::Stopped => "🔴 已停止",
            ServiceQueryResult::Starting => "🔄 启动中",
            ServiceQueryResult::Stopping => "🔄 停止中",
            ServiceQueryResult::Paused => "⏸ 暂停",
            ServiceQueryResult::Unknown => "❓ 未知状态",
        };
        println!(" - {}: {}", service.name, status_str);
    }
}

// 一键启动所有服务
fn batch_start_services(services: &mut Vec<ServiceInfo>) {
    println!("⏳ 正在尝试启动所有服务...");

    for service in services {
        if let ServiceQueryResult::Stopped = service.status {
            if WindowsServiceUtil::start_service(&service.name) {
                println!("✅ {} 启动成功", service.name);
                service.status = ServiceQueryResult::Running;
            } else {
                eprintln!("❌ {} 启动失败", service.name);
            }
        } else {
            println!("⏩ {} 已运行，跳过", service.name);
        }
    }
}

// 一键停止所有服务
fn batch_stop_services(services: &mut Vec<ServiceInfo>) {
    println!("⏳ 正在尝试停止所有服务...");

    for service in services {
        if let ServiceQueryResult::Running = service.status {
            if WindowsServiceUtil::stop_service(&service.name) {
                println!("✅ {} 停止成功", service.name);
                service.status = ServiceQueryResult::Stopped;
            } else {
                eprintln!("❌ {} 停止失败", service.name);
            }
        } else {
            println!("⏩ {} 已停止，跳过", service.name);
        }
    }
}

// 单独控制某个服务
fn individual_service_control(services: &mut Vec<ServiceInfo>) {
    let names: Vec<String> = services.iter().map(|s| s.name.clone()).collect();
    let selection = Select::with_theme(&ColorfulTheme::default())
        .items(&names)
        .default(0)
        .interact()
        .expect("无法选择服务");

    let selected = &mut services[selection];
    let actions = &["🟢 启动服务", "🔴 停止服务", "🔄 查询状态"];
    let action = Select::with_theme(&ColorfulTheme::default())
        .items(actions)
        .default(0)
        .interact()
        .expect("无法选择操作");

    match action {
        0 => {
            if WindowsServiceUtil::start_service(&selected.name) {
                selected.status = ServiceQueryResult::Running;
                println!("✅ {} 启动成功", selected.name);
            } else {
                eprintln!("❌ {} 启动失败", selected.name);
            }
        }
        1 => {
            if WindowsServiceUtil::stop_service(&selected.name) {
                selected.status = ServiceQueryResult::Stopped;
                println!("✅ {} 停止成功", selected.name);
            } else {
                eprintln!("❌ {} 停止失败", selected.name);
            }
        }
        2 => match WindowsServiceUtil::query_service_status(&selected.name) {
            Ok(status) => {
                selected.status = status.clone();
                let status_str = match status {
                    ServiceQueryResult::Running => "🟢 运行中",
                    ServiceQueryResult::Stopped => "🔴 已停止",
                    ServiceQueryResult::Starting => "🔄 启动中",
                    ServiceQueryResult::Stopping => "🔄 停止中",
                    ServiceQueryResult::Paused => "⏸ 暂停",
                    ServiceQueryResult::Unknown => "❓ 未知状态",
                };
                println!("🔍 {} 当前状态: {}", selected.name, status_str);
            }
            Err(e) => eprintln!("❌ 查询失败: {}", e),
        },
        _ => unreachable!(),
    }
}

// 内存中的服务信息
#[derive(Clone)]
struct ServiceInfo {
    name: String,
    status: ServiceQueryResult,
}
