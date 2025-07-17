use std::fs;
use std::io::Write;
use std::path::Path;

use serde::Deserialize;

#[derive(Deserialize, Debug)]
struct Config {
    pub jdk_path: String,
    pub servers: Vec<Service>,
}

/// 服务状态枚举
#[derive(Deserialize, Clone, Debug)]
pub struct Service {
    pub name: String,
    pub unify: bool,
}

/// 读取JSON配置文件后返回读取的内容
fn read_json_file() -> String {
    let file_path = "./config.json";

    // 如果文件存在，直接读取
    if Path::new(file_path).exists() {
        return fs::read_to_string(file_path).expect("无法读取配置文件");
    }

    // 否则创建默认配置文件
    let default_config = r#"
{
  "jdk_path": "jdk文件夹上一层",
  "servers": [
    { "name": "服务名称", "unify": true }
  ]
}
"#;

    let mut file = fs::File::create(file_path).expect("无法创建配置文件");
    file.write_all(default_config.as_bytes()).expect("无法写入默认配置");

    eprintln!("❌ 未找到配置文件，已生成默认配置到 config.json");
    eprintln!("📝 请先编辑该文件以匹配你的实际环境，然后重新运行程序");
    std::process::exit(0); // 直接退出程序
}

/// 获取服务配置列表
pub fn get_services() -> Vec<Service> {
    let connect = read_json_file();
    let config: Config = serde_json::from_str(&connect).expect("无法解析JSON文件");
    config.servers
}

/// 获取JDK路径
pub fn get_jdk_path() -> String {
    let connect = read_json_file();
    let config: Config = serde_json::from_str(&connect).expect("无法解析JSON文件");
    config.jdk_path
}
