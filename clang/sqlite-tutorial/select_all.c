#include <sqlite3.h>
#include <stdio.h>

int callback(void *, int, char **, char **);

int main(void) {
  sqlite3 *db;
  char *errmsg = 0;
  int rc = sqlite3_open("test.db", &db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Open db error: %s\n", sqlite3_errmsg(db));
    return 1;
  }

  char *sql = "SELECT * FROM Cars";
  rc = sqlite3_exec(db, sql, callback, NULL, &errmsg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to select data\n");
    fprintf(stderr, "SQL error: %s\n", errmsg);
    sqlite3_free(errmsg);
    sqlite3_close(db);
    return 1;
  }

  sqlite3_close(db);
  return 0;
}

int callback(void *NotUsed, int argc, char **argv, char **azColName) {
  NotUsed = 0;

  for (int i = 0; i < argc; i++) {
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}