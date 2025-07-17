#![allow(unsafe_code)]
use dialoguer::{Select, theme::ColorfulTheme};
use std::env;
use std::ffi::CString;
use std::io;
use std::path::PathBuf;
use std::process::Command;
use walkdir::WalkDir;
use winreg::RegKey;
use winreg::enums::*;

/// 获取当前系统的 JAVA_HOME 环境变量值
pub fn get_current_java_home() -> Option<String> {
    env::var("JAVA_HOME").ok()
}

/// 获取当前 Java 的版本信息（通过 java -version）
pub fn get_java_version(java_home: Option<&str>) -> Option<String> {
    let java_bin = if let Some(path) = java_home {
        PathBuf::from(path).join("bin").join("java.exe")
    } else {
        // 尝试从 PATH 中找 java
        PathBuf::from("java")
    };

    let output = Command::new(java_bin).arg("-version").output().ok()?;

    let stderr = String::from_utf8_lossy(&output.stderr).to_string();

    // 提取版本号部分，例如："openjdk version "17.0.1"..."
    if let Some(start) = stderr.find("\"") {
        if let Some(end) = stderr[start + 1..].find("\"") {
            return Some(stderr[start + 1..start + 1 + end].to_string());
        }
    }

    Some(stderr.lines().next()?.to_string())
}

/// 遍历目录，列出所有 JDK 子目录
pub fn list_jdks(base_dir: &str) -> io::Result<Vec<PathBuf>> {
    let mut paths = Vec::new();
    for entry in WalkDir::new(base_dir).max_depth(1) {
        let entry = entry.map_err(|e| io::Error::new(io::ErrorKind::Other, e))?;
        if entry.file_type().is_dir()
            && entry.path().parent() == Some(PathBuf::from(base_dir).as_path())
        {
            paths.push(entry.into_path());
        }
    }
    Ok(paths)
}

/// 设置 JAVA_HOME 环境变量
pub fn set_java_home(path: &str) -> io::Result<()> {
    let hkcu = RegKey::predef(HKEY_CURRENT_USER);
    let environment = hkcu.open_subkey_with_flags("Environment", KEY_READ | KEY_WRITE)?;
    environment.set_value("JAVA_HOME", &path)?;
    println!("✅ JAVA_HOME 已设置为: {}", path);

    // 广播环境变量变更
    unsafe {
        use winapi::um::winuser::{HWND_BROADCAST, SendMessageTimeoutA, WM_SETTINGCHANGE};
        let env = CString::new("Environment").unwrap();
        SendMessageTimeoutA(
            HWND_BROADCAST,
            WM_SETTINGCHANGE,
            0,
            env.as_ptr() as isize,
            0,
            1000,
            std::ptr::null_mut(),
        );
    }

    Ok(())
}

/// 交互式切换 JDK 版本
pub fn switch_java_home_interactive(base_dir: &str) -> io::Result<()> {
    if !PathBuf::from(base_dir).exists() {
        return Err(io::Error::new(io::ErrorKind::NotFound, "JDK 目录不存在"));
    }
    let jdks = list_jdks(base_dir)?;
    if jdks.is_empty() {
        return Err(io::Error::new(io::ErrorKind::NotFound, "未找到任何 JDK"));
    }

    let jdk_names: Vec<String> = jdks
        .iter()
        .map(|p| p.file_name().unwrap().to_string_lossy().into_owned())
        .collect();

    let selection = Select::with_theme(&ColorfulTheme::default())
        .items(&jdk_names)
        .default(0)
        .interact()
        .map_err(|e| io::Error::new(io::ErrorKind::Other, e))?;

    let selected_path = jdks[selection].to_str().unwrap().to_string();
    set_java_home(&selected_path)?;

    println!("📌 当前使用 JDK: {}", selected_path);
    Ok(())
}
