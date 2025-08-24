#include <sqlite3.h>
#include <stdio.h>

int main(void) {
  sqlite3 *db;
  char *errmsg = 0;

  int rc = sqlite3_open(":memory:", &db);
  if (rc != SQLITE_OK) {
    sqlite3_close(db);
    return 1;
  }

  char *sql =
      "CREATE TABLE Friends(Id INTEGER PRIMARY KEY, Name TEXT);"
      "INSERT INTO Friends(Name) VALUES ('Tom');"
      "INSERT INTO Friends(Name) VALUES ('Rebecca');"
      "INSERT INTO Friends(Name) VALUES ('Jim');"
      "INSERT INTO Friends(Name) VALUES ('Roger');"
      "INSERT INTO Friends(Name) VALUES ('Robert');";

  rc = sqlite3_exec(db, sql, 0, 0, &errmsg);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to create table \n");
    fprintf(stderr, "SQL error: %s\n", errmsg);
    sqlite3_free(errmsg);
  } else {
    fprintf(stdout, "Table Friends created successfully\n");
  }

  int last_id = sqlite3_last_insert_rowid(db);
  printf("The last Id of the inserted row is %d\n", last_id);
  sqlite3_close(db);
  return 0;
}