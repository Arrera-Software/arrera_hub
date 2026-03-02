pub mod gest_index;

pub mod user_conf;

pub fn dect_os() -> i32 {
    if cfg!(target_os = "windows") {
        return 1
    } else if cfg!(target_os = "linux") {
        return 2
    } else if cfg!(target_os = "macos") {
        return 3
    } else {
        return 0
    }
}