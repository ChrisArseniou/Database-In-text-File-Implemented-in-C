#ifndef _DB_H
#define _DB_H

FILE *open_db(char base_name[], FILE *f, int *size, int *cond);

void import_db(char fname[], char name[], FILE *f, char dbname[], int *cond);

void find_in_db(FILE *f, char name[], char dbname[],int *cond);

void export_db(FILE *f , char dbname[], char name[], char fname[], int *cond);

void delete_in_db(FILE *f, char name[], char dbname[],int *cond);

FILE *close_db(FILE *f);

#endif
 
