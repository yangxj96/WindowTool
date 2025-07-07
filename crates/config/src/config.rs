use std::fs;

use serde::Deserialize;

#[derive(Deserialize, Debug)]
struct Config {
    servers: Vec<Service>,
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
    if !std::path::Path::new(file_path).exists() {
        panic!("没找到配置文件: {}", file_path);
    }
    return fs::read_to_string(file_path).expect("无法读取文件");
}

/// 获取服务配置列表
pub fn get_services() -> Vec<Service> {
    let connect = read_json_file();
    let config: Config = serde_json::from_str(&connect).expect("无法解析JSON文件");
    config.servers
}
