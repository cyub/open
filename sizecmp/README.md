# sizecmp

比较两个二进制文件elf section大小

## 用法

```bash
cargo build
cargo build --release
cargo run -- target/debug/sizecmp target/release/sizecmp
```

输出内容示例：

```shell
➜  sizecmp git:(main) ✗ cargo run -- target/debug/sizecmp target/release/sizecmp
    Finished dev [unoptimized + debuginfo] target(s) in 0.00s
     Running `target/debug/sizecmp target/debug/sizecmp target/release/sizecmp`
.bss                                   296         296           0
.comment                                87          87           0
.data                                   48          48           0
.data.rel.ro                         15112       13504       -1608
.debug_abbrev                        36024        3775      -32249
.debug_aranges                       55008       44640      -10368
.debug_gdb_scripts                      34           0         -34
.debug_info                        1316504      999375     -317129
.debug_line                         549120      468495      -80625
.debug_loc                            2745           0       -2745
.debug_pubnames                     744722      627652     -117070
.debug_pubtypes                     123593         198     -123395
.debug_ranges                       703264      656672      -46592
.debug_str                         1604459     1401061     -203398
.dynamic                               528         528           0
.dynstr                               1526        1526           0
.dynsym                               2472        2472           0
.eh_frame                            46736       36864       -9872
.eh_frame_hdr                         9372        6148       -3224
.fini                                   13          13           0
.fini_array                              8           8           0
.gcc_except_table                     8316        6388       -1928
.gnu.hash                               48          36         -12
.gnu.version                           206         206           0
.gnu.version_r                         320         320           0
.got                                  2360        2432          72
.init                                   27          27           0
.init_array                             16          16           0
.interp                                 28          28           0
.note.ABI-tag                           32          32           0
.note.gnu.build-id                      36          36           0
.note.gnu.property                      32          32           0
.plt                                    48          48           0
.plt.got                                24           8         -16
.rela.dyn                            25992       24240       -1752
.rela.plt                               48          48           0
.rodata                              47896       45416       -2480
.tbss                                   72          72           0
.tdata                                  40          40           0
.text                               430996      349940      -81056
total                              5728208     4692727    -1035481
```