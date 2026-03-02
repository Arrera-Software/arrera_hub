use std::sync::Mutex;
use crate::config::dect_os;
use dirs;

static PATH_CONF : Mutex<String> = Mutex::new(String::new());

fn write_value(val: &str) {
    let mut data = crate::config::gest_index::PATH_CONF.lock().unwrap();
    *data = val.to_string();
}

fn read_value() -> String {
    let data = crate::config::gest_index::PATH_CONF.lock().unwrap();
    data.clone()
}