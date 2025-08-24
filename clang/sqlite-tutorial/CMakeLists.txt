cmake_minimum_required(VERSION 3.10)
project(sqlite-tutorial VERSION 0.1.0 LANGUAGES C)

# 定义源文件列表
set(SOURCE_FILES
    version.c
    version2.c
    insert_data.c
    last_row_id.c
    select_all.c
    parameterized.c
    named_placeholders.c
    column_names.c
    list_tables.c
    get_ac_mode.c
    autocommit.c
    transaction.c
    create_table.c
    insert.c
    delete_record.c
)

find_package(unofficial-sqlite3 CONFIG REQUIRED)


foreach(SOURCE_FILE ${SOURCE_FILES})
    get_filename_component(TARGET_NAME ${SOURCE_FILE} NAME_WE) # 获取文件名称
    add_executable(${TARGET_NAME} ${SOURCE_FILE})
    target_link_libraries(${TARGET_NAME} PRIVATE unofficial::sqlite3::sqlite3)
endforeach()


