use std::io::Read;
use std::process::{Command, Stdio};

/// 执行命令并返回 UTF-8 编码的输出（支持 lossy 转换）
pub fn run_command_and_capture_output(command: &str, args: &[&str]) -> Result<String, String> {
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
    if let Err(e) = Command::new(command).args(args).stdout(Stdio::null()).stderr(Stdio::null()).spawn() {
        eprintln!("静默执行命令失败: {}: {:?}", command, e);
    }
}
