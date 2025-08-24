#include <sqlite3.h>
#include <stdio.h>

int main(void) {
  sqlite3 *db;
  char *errmsg = 0;

  int rc = sqlite3_open("test.db", &db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Open database error: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1;
  }

  char *sql =
      "DROP TABLE IF EXISTS Friends;"
      "CREATE TABLE Friends(Id INTEGER PRIMARY KEY, Name TEXT);"
      "INSERT INTO Friends(Name) VALUES ('Tom');"
      "INSERT INTO Friends(Name) VALUES ('Rebecca');"
      "INSERT INTO Friends(Name) VALUES ('Jim');"
      "INSERT INTO Friend(Name) VALUES ('Robert');";

  rc = sqlite3_exec(db, sql, NULL, NULL, &errmsg);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", errmsg);
    sqlite3_free(errmsg);
    sqlite3_close(db);
    return 1;
  }

  sqlite3_close(db);
  return 0;
}