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
section                              size1       size2        diff     precent
==============================================================================
.bss                                   296         296           0       0.00%
.comment                                87          87           0       0.00%
.data                                   48          48           0       0.00%
.data.rel.ro                         15400       13600       -1800     -11.69%
.debug_abbrev                        36998        3775      -33223     -89.80%
.debug_aranges                       55248       44640      -10608     -19.20%
.debug_gdb_scripts                      34           0         -34    -100.00%
.debug_info                        1325231      999375     -325856     -24.59%
.debug_line                         550854      468495      -82359     -14.95%
.debug_loc                            2823           0       -2823    -100.00%
.debug_pubnames                     745917      627652     -118265     -15.85%
.debug_pubtypes                     126224         198     -126026     -99.84%
.debug_ranges                       703824      656672      -47152      -6.70%
.debug_str                         1605583     1401061     -204522     -12.74%
.dynamic                               528         528           0       0.00%
.dynstr                               1526        1526           0       0.00%
.dynsym                               2472        2472           0       0.00%
.eh_frame                            47304       36864      -10440     -22.07%
.eh_frame_hdr                         9492        6148       -3344     -35.23%
.fini                                   13          13           0       0.00%
.fini_array                              8           8           0       0.00%
.gcc_except_table                     8508        6388       -2120     -24.92%
.gnu.hash                               48          36         -12     -25.00%
.gnu.version                           206         206           0       0.00%
.gnu.version_r                         320         320           0       0.00%
.got                                  2368        2432          64       2.70%
.init                                   27          27           0       0.00%
.init_array                             16          16           0       0.00%
.interp                                 28          28           0       0.00%
.note.ABI-tag                           32          32           0       0.00%
.note.gnu.build-id                      36          36           0       0.00%
.note.gnu.property                      32          32           0       0.00%
.plt                                    48          48           0       0.00%
.plt.got                                24           8         -16     -66.67%
.rela.dyn                            26448       24384       -2064      -7.80%
.rela.plt                               48          48           0       0.00%
.rodata                              48024       45416       -2608      -5.43%
.tbss                                   72          72           0       0.00%
.tdata                                  40          40           0       0.00%
.text                               438068      351076      -86992     -19.86%
==============================================================================
total                              5754303     4694103    -1060200     -18.42%
```