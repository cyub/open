use std::{
    fs::File,
    io::{self, BufRead},
};

fn main() -> std::io::Result<()> {
    let file = File::open("/proc/cpuinfo")?;
    let lines = io::BufReader::new(file).lines();
    for line in lines {
        if let Ok(l) = line {
            println!("{}", l);
        }
    }
    Ok(())
}
