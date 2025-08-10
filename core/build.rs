#[cfg(windows)]
extern crate winres;

#[cfg(windows)]
fn main() {
    if cfg!(target_os = "windows") {
        let res = winres::WindowsResource::new();
        res.compile().unwrap();
    } else {
        println!("当前操作系统不是windows,跳过资源编译.");
    }
}
