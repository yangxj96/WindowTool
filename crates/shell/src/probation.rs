use crate::shell::{run_command_and_capture_output, run_silent_command};

/// 列出指定注册表路径下的所有子项
fn list_registry_keys(path: &str) -> Result<Vec<String>, String> {
    let output = run_command_and_capture_output("reg", &["query", path])?;
    Ok(output.lines().filter(|line| line.starts_with("HKEY_")).map(|s| s.to_string()).collect())
}

/// 检查注册表项是否包含某个字符串
fn registry_key_contains(key: &str, term: &str) -> Result<bool, String> {
    let output = run_command_and_capture_output("reg", &["query", key, "/s", "/f", term])?;
    Ok(output.contains(term))
}

/// 执行完整的注册表清理逻辑
pub fn navicat_registry_cleanup() {
    println!("⏳ 正在执行Navicat注册表清理...");

    let reg_delete = |path: &str| {
        run_silent_command("reg", &["delete", path, "/f"]);
    };

    reg_delete("HKEY_CURRENT_USER\\Software\\PremiumSoft\\NavicatPremium\\Registration17XCS");
    reg_delete("HKEY_CURRENT_USER\\Software\\PremiumSoft\\NavicatPremium\\Update");

    let reg_path = "HKEY_CURRENT_USER\\Software\\Classes\\CLSID";
    let search_terms = ["Info", "ShellFolder"];

    match list_registry_keys(reg_path) {
        Ok(keys) => {
            for key in keys {
                for &term in &search_terms {
                    if let Ok(true) = registry_key_contains(&key, term) {
                        run_silent_command("reg", &["delete", &key, "/f"]);
                    }
                }
            }
        }
        Err(e) => eprintln!("❌ 读取注册表键失败: {}", e),
    }

    println!("✅ 完成注册表清理");
}
