extern crate winres;

fn main() {
    if cfg!(target_os = "windows") {
        let mut res = winres::WindowsResource::new();
        // res.set_icon("path/to/icon.ico"); // 如果有图标的话可以设置
        res.compile().unwrap();
    }
}