use std::fs;
use std::path::PathBuf;
use std::sync::Mutex;
use crate::config::dect_os;
use dirs;

static PATH_INDEX : Mutex<String> = Mutex::new(String::new());

fn write_value(val: &str) {
    let mut data = PATH_INDEX.lock().unwrap();
    *data = val.to_string();
}

fn read_value() -> String {
    let data = PATH_INDEX.lock().unwrap();
    data.clone()
}

pub fn save_index(content: &str) -> bool {
    let os: i32 = dect_os();

    let mut dir = match dirs::home_dir() {
        Some(path) => path,
        None => return false,
    };

    if os == 1 {
        // WINDOWS
        dir.push("AppData");
        dir.push("Roaming");
        dir.push("arrera-hub");
    } else if os == 2 || os == 3 {
        // LINUX & MAC
        dir.push(".config");
        dir.push("arrera-hub");
    } else {
        return false;
    }

    dir.push("index.json");

    if let Some(parent) = dir.parent() {
        if fs::create_dir_all(parent).is_err() {
            return false;
        }
    }

    fs::write(dir, content).is_ok()
}