use std::io::Read;
use std::process::{Command, Stdio};

/// 注册表操作工具类
pub struct RegistryUtil;

impl RegistryUtil {
    /// 执行命令并返回 UTF-8 编码的输出（支持 lossy 转换）
    fn run_command_and_capture_output(command: &str, args: &[&str]) -> Result<String, String> {
        let mut child = Command::new(command)
            .args(args)
            .stdout(Stdio::piped())
            .stderr(Stdio::null())
            .spawn()
            .map_err(|e| format!("无法执行命令: {} {:?}", e, args))?;

        let mut output_bytes = Vec::new();
        child
            .stdout
            .as_mut()
            .ok_or("子进程输出为空")?
            .read_to_end(&mut output_bytes)
            .map_err(|e| format!("读取输出失败: {}", e))?;

        // 使用 from_utf8_lossy 避免非法 UTF-8 导致 panic
        let output = String::from_utf8_lossy(&output_bytes);
        Ok(output.into_owned())
    }

    /// 静默执行命令（不显示输出）
    pub fn run_silent_command(command: &str, args: &[&str]) {
        if let Err(e) = Command::new(command)
            .args(args)
            .stdout(Stdio::null())
            .stderr(Stdio::null())
            .spawn()
        {
            eprintln!("静默执行命令失败: {}: {:?}", command, e);
        }
    }

    /// 列出指定注册表路径下的所有子项
    pub fn list_registry_keys(path: &str) -> Result<Vec<String>, String> {
        let output = Self::run_command_and_capture_output("reg", &["query", path])?;

        Ok(output
            .lines()
            .filter(|line| line.starts_with("HKEY_"))
            .map(|s| s.to_string())
            .collect())
    }

    /// 检查注册表项是否包含某个字符串
    pub fn registry_key_contains(key: &str, term: &str) -> Result<bool, String> {
        let output =
            Self::run_command_and_capture_output("reg", &["query", key, "/s", "/f", term])?;

        Ok(output.contains(term))
    }

    /// 执行完整的注册表清理逻辑
    pub fn execute_registry_cleanup() {
        println!("⏳ 正在执行注册表清理...");

        let reg_delete = |path: &str| {
            Self::run_silent_command("reg", &["delete", path, "/f"]);
        };

        reg_delete("HKEY_CURRENT_USER\\Software\\PremiumSoft\\NavicatPremium\\Registration17XCS");
        reg_delete("HKEY_CURRENT_USER\\Software\\PremiumSoft\\NavicatPremium\\Update");

        let reg_path = "HKEY_CURRENT_USER\\Software\\Classes\\CLSID";
        let search_terms = ["Info", "ShellFolder"];

        match Self::list_registry_keys(reg_path) {
            Ok(keys) => {
                for key in keys {
                    for &term in &search_terms {
                        if let Ok(true) = Self::registry_key_contains(&key, term) {
                            Self::run_silent_command("reg", &["delete", &key, "/f"]);
                        }
                    }
                }
            }
            Err(e) => eprintln!("❌ 读取注册表键失败: {}", e),
        }

        println!("✅ 完成注册表清理");
    }
}
