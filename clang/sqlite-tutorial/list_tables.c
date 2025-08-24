#include <sqlite3.h>
#include <stdio.h>

int callback(void *, int, char **, char **);

int main(void) {
  sqlite3 *db;
  char *errmsg = 0;
  int rc = sqlite3_open("test.db", &db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Open database error: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1;
  }

  char *sql = "SELECt name FROM sqlite_master WHERE type = 'table'";
  rc = sqlite3_exec(db, sql, callback, NULL, &errmsg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "sql execute error: %s", errmsg);
    sqlite3_free(errmsg);
    sqlite3_close(db);
    return 1;
  }

  sqlite3_close(db);
  return 0;
}

int callback(void *NotUsed, int argc, char **argv, char **azColName) {
  NotUsed = NULL;

  for (int i = 0; i < argc; i++) {
    printf("%s\n", argv[i] ? argv[i] : "NULL");
  }
  return 0;
}