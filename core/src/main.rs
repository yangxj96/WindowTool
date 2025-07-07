use dialoguer::{Select, theme::ColorfulTheme};
use std::{io, ops::Deref};
// 自己写的crate模块
use windows_tool_config::config;
use windows_tool_service::service;
use windows_tool_shell::probation;

/// 服务信息,扩展config::Service中的内容
#[derive(Clone, Debug)]
struct ServiceInfo {
    inner: config::Service,
    status: service::ServiceQueryResult,
}

/// 扩展
impl Deref for ServiceInfo {
    type Target = config::Service;

    fn deref(&self) -> &Self::Target {
        &self.inner
    }
}

fn main() -> io::Result<()> {
    // 可选日志支持
    env_logger::init();

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
            4 => probation::navicat_registry_cleanup(),
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
    let services = config::get_services();
    services
        .into_iter()
        .map(|s| ServiceInfo {
            // name: s.name.clone(),
            inner: s.clone(),
            status: service::query_service_status(&s.name).unwrap_or(service::ServiceQueryResult::Unknown),
        })
        .collect()
}

// 显示服务状态
fn display_service_status(services: &[ServiceInfo]) {
    println!("\n📋 当前服务状态：");
    for service in services {
        let status_str = match service.status {
            service::ServiceQueryResult::Running => "🟢 运行中",
            service::ServiceQueryResult::Stopped => "🔴 已停止",
            service::ServiceQueryResult::Starting => "🔄 启动中",
            service::ServiceQueryResult::Stopping => "🔄 停止中",
            service::ServiceQueryResult::Paused => "⏸ 暂停",
            service::ServiceQueryResult::Unknown => "❓ 未知状态",
        };
        println!(" - {}: {}", service.name, status_str);
    }
}

// 一键启动所有服务
fn batch_start_services(services: &mut Vec<ServiceInfo>) {
    println!("⏳ 正在尝试启动所有服务...");
    for service in services {
        if service.unify && matches!(service.status, service::ServiceQueryResult::Stopped) {
            if service::start_service(&service.name) {
                println!("✅ {} 启动成功", service.name);
                service.status = service::ServiceQueryResult::Running;
            } else {
                eprintln!("❌ {} 启动失败", service.name);
            }
        } else if !service.unify {
            println!("⏩ {} 被标记为不统一管理，跳过", service.name);
        } else {
            println!("⏩ {} 已运行或启动中，跳过", service.name);
        }
    }
}

// 一键停止所有服务
fn batch_stop_services(services: &mut Vec<ServiceInfo>) {
    println!("⏳ 正在尝试停止所有服务...");
    for service in services {
        if service.unify && matches!(service.status, service::ServiceQueryResult::Running) {
            if service::stop_service(&service.name) {
                println!("✅ {} 停止成功", service.name);
                service.status = service::ServiceQueryResult::Stopped;
            } else {
                eprintln!("❌ {} 停止失败", service.name);
            }
        } else if !service.unify {
            println!("⏩ {} 被标记为不统一管理，跳过", service.name);
        } else {
            println!("⏩ {} 已停止，跳过", service.name);
        }
    }
}

// 单独控制某个服务
fn individual_service_control(services: &mut Vec<ServiceInfo>) {
    let names: Vec<String> = services.iter().map(|s| s.name.clone()).collect();
    let selection =
        Select::with_theme(&ColorfulTheme::default()).items(&names).default(0).interact().expect("无法选择服务");

    let selected = &mut services[selection];
    let actions = &["🟢 启动服务", "🔴 停止服务", "🔄 查询状态"];
    let action =
        Select::with_theme(&ColorfulTheme::default()).items(actions).default(0).interact().expect("无法选择操作");

    match action {
        0 => {
            if service::start_service(&selected.name) {
                selected.status = service::ServiceQueryResult::Running;
                println!("✅ {} 启动成功", selected.name);
            } else {
                eprintln!("❌ {} 启动失败", selected.name);
            }
        }
        1 => {
            if service::stop_service(&selected.name) {
                selected.status = service::ServiceQueryResult::Stopped;
                println!("✅ {} 停止成功", selected.name);
            } else {
                eprintln!("❌ {} 停止失败", selected.name);
            }
        }
        2 => match service::query_service_status(&selected.name) {
            Ok(status) => {
                selected.status = status.clone();
                let status_str = match status {
                    service::ServiceQueryResult::Running => "🟢 运行中",
                    service::ServiceQueryResult::Stopped => "🔴 已停止",
                    service::ServiceQueryResult::Starting => "🔄 启动中",
                    service::ServiceQueryResult::Stopping => "🔄 停止中",
                    service::ServiceQueryResult::Paused => "⏸ 暂停",
                    service::ServiceQueryResult::Unknown => "❓ 未知状态",
                };
                println!("🔍 {} 当前状态: {}", selected.name, status_str);
            }
            Err(e) => eprintln!("❌ 查询失败: {}", e),
        },
        _ => unreachable!(),
    }
}
