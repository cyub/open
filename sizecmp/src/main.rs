use std::{
    collections::{HashMap, HashSet},
    env,
    io::{self, Error, ErrorKind},
    process::{Command, Output},
};

fn main() -> io::Result<()> {
    let mut arg = env::args().skip(1); // Skip the program name

    if arg.len() != 2 {
        return Err(Error::new(
            ErrorKind::InvalidInput,
            "usage: sizecmp binary1 binary2",
        ));
    }

    let f1 = arg.next().unwrap_or_default();
    let f2 = arg.next().unwrap_or_default();

    let size1 = read_size(&f1);
    let size2 = read_size(&f2);

    let keys: HashSet<_> = size1.keys().chain(size2.keys()).collect(); // 合并所有键
    let mut keys: Vec<_> = keys.into_iter().collect(); // 去除重复值并转换为 Vec
    keys.sort(); // 排序所有键

    println!(
        "{:30} {:>11} {:>11} {:>11} {:>11}",
        "section", "size1", "size2", "diff", "percent"
    );
    println!("{}", "=".repeat(78));

    let mut total1 = 0.0;
    let mut total2 = 0.0;

    for &k in &keys {
        let s1 = size1.get(k).copied().unwrap_or(0.0);
        let s2 = size2.get(k).copied().unwrap_or(0.0);
        let diff = s2 - s1;

        let percent = if s1 != 0.0 {
            format!("{}%", format_number((diff / s1) * 100.0))
        } else {
            "-".to_string()
        };

        println!("{:30} {:11} {:11} {:11} {:>11}", k, s1, s2, diff, percent);

        total1 += s1;
        total2 += s2;
    }

    let total_diff = total2 - total1;
    let total_percent = if total1 != 0.0 {
        format!("{}%", format_number((total_diff / total1) * 100.0))
    } else {
        "-".to_string()
    };

    println!("{}", "=".repeat(78));
    println!(
        "{:30} {:11} {:11} {:11} {:>11}",
        "total", total1, total2, total_diff, total_percent
    );

    Ok(())
}

fn read_size(f: &str) -> HashMap<String, f64> {
    let empty_output = Output {
        status: Default::default(),
        stdout: Vec::new(),
        stderr: Vec::new(),
    };

    let output = Command::new("size")
        .args(&["-A", "-d", f])
        .output()
        .unwrap_or(empty_output);

    if !output.status.success() {
        return HashMap::new();
    }

    let mut result = HashMap::new();
    if let Ok(size_info) = String::from_utf8(output.stdout) {
        for line in size_info
            .lines()
            .skip(2)
            .filter(|&line| !line.trim().is_empty())
        {
            let fields: Vec<&str> = line.split_whitespace().collect();
            if let [k, v, ..] = fields.as_slice() {
                if *k != "Total" {
                    if let Ok(v) = v.parse::<f64>() {
                        result.insert(k.to_string(), v);
                    }
                }
            }
        }
    }
    result
}

fn format_number(num: f64) -> String {
    if num == 0.0 {
        format!("{:.0}", num)
    } else {
        format!("{:+.2}", num)
    }
}
