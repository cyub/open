use std::{collections::HashMap, env, io, process::Command};

fn main() -> io::Result<()> {
    let mut arg = env::args();
    if arg.len() != 3 {
        return Err(io::Error::new(
            io::ErrorKind::InvalidInput,
            "usage: sizecmp binary1 binary2",
        ));
    }

    arg.next();
    let f1 = arg.nth(0).unwrap_or("".to_string());
    let f2 = arg.nth(0).unwrap_or("".to_string());

    let size1 = read_size(&f1);
    let size2 = read_size(&f2);
    let mut keys: Vec<&str> = vec![];
    for (k, _) in size1.iter() {
        keys.push(k);
    }

    for (k, _) in size2.iter() {
        match size1.get(k) {
            None => keys.push(k),
            Some(_) => {}
        }
    }
    keys.sort();

    let mut total1: f64 = 0.0;
    let mut total2: f64 = 0.0;

    println!(
        "{:30} {:>11} {:>11} {:>11} {:>11}",
        "section", "size1", "size2", "diff", "precent",
    );
    println!("{}", "=".repeat(78),);

    for k in keys {
        let s1 = size1.get(k).unwrap_or(&0.0);
        let s2 = size2.get(k).unwrap_or(&0.0);
        let diff = s2 - s1;
        let mut percent = String::from("-");
        if s1.ne(&0.0) {
            percent = format!("{:.2}%", (diff / s1) * 100.0);
        }
        println!("{:30} {:11} {:11} {:11} {:>11}", k, s1, s2, diff, percent);
        total1 += size1.get(k).unwrap_or(&0.0);
        total2 += size2.get(k).unwrap_or(&0.0);
    }

    let total_diff = total2 - total1;
    let mut total_percent = String::from("-");
    if total1 != 0.0 {
        total_percent = format!("{:.2}%", (total_diff / total1) * 100.0);
    }

    println!("{}", "=".repeat(78),);
    println!(
        "{:30} {:11} {:11} {:11} {:>11}",
        "total", total1, total2, total_diff, total_percent,
    );
    Ok(())
}

fn read_size(f: &str) -> HashMap<String, f64> {
    let output = Command::new("size")
        .arg("-A")
        .arg("-d")
        .arg(f)
        .output()
        .expect("failed to execute size command");

    if !output.status.success() {
        return HashMap::new();
    }

    let mut result = HashMap::new();
    if let Ok(size_info) = String::from_utf8(output.stdout) {
        for (i, line) in size_info.lines().enumerate() {
            if i < 2 {
                continue;
            }
            let fields: Vec<&str> = line.split(" ").filter(|x| !x.is_empty()).collect();

            if fields.len() < 2 || fields[0] == "Total" {
                continue;
            }

            let k = fields[0].to_string();
            let v = fields[1].parse::<f64>().unwrap();
            result.insert(k, v);
        }
        return result;
    } else {
        return HashMap::new();
    }
}
