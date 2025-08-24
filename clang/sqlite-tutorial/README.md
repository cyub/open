# SQLite C

源码来自下面两个教程里面，额外引入vcpkg进行包依赖管理, cmake进行编译管理：
- [SQLite C](https://zetcode.com/db/sqlitec/)
- [SQLite - C/C++](https://www.tutorialspoint.com/sqlite/sqlite_c_cpp.htm)

### 构建命令

```c
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake # 配置vcpkg作为cmake工具链
cmake --build build
```