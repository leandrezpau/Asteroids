#include <stdio.h>
#include <stdlib.h>
#include <esat_extra/sqlite3.h> 

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

int main(int argc, char* argv[]) {
   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   char *sql;

   /* Open database */
   rc = sqlite3_open("usuarios.db", &db);
   
   if( rc ) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return(0);
   } else {
      fprintf(stdout, "Opened database successfully\n");
   }

   /* Create SQL statement */
   sql = "CREATE TABLE USUARIOS("  \
      "USERNAME CHAR(12) PRIMARY KEY    NOT NULL," \
      "PASSWORD           CHAR(12)     NOT NULL," \
      "NAME            CHAR(12)     NOT NULL," \
      "LASTNAME        CHAR(20)," \
      "EMAIL           CHAR(20)," \
      "CITY            CHAR(20)," \
      "COUNTRY         CHAR(20)," \
      "BIRTHDATE       CHAR(20)," \
      "CREDITS         INT          NOT NULL," \
      "SCORE           INT          NOT NULL);"; 

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   } else {
      fprintf(stdout, "Table created successfully\n");
   }
   sqlite3_close(db);
   return 0;
}