#include "stdio.h"
#include "db.h"

#define MAX_SIZE 256 

int main(int argc, char *argv[]){
	char option, dbname[MAX_SIZE], fname[MAX_SIZE], name[MAX_SIZE];
	FILE *f=NULL;
	int total_size=0,cond=0;
	
	while(1){
		scanf(" %c", &option);
		switch(option){
			case('o') : {
				scanf(" %255s", dbname);
				f = open_db(dbname ,f, &total_size, &cond);
				if( cond == 1){
					return(0);
				}
				break;
			}
			case('i') : {
				scanf(" %255s %255s", fname, name);
				import_db(fname, name,f,dbname, &cond);
				if(cond == 1){
					return(0);
				}
				break;
				
			}
			case('f') : {
				scanf(" %255s", name);
				find_in_db(f, name,dbname, &cond);
				if(cond == 1){
					return(0);
				}
				break;
			}
			case('e') : {
				scanf(" %255s %255s", name, fname);
				export_db(f, dbname, name, fname, &cond);
				if(cond == 1){
					return(0);
				}
				break;
			}
			case('d') : {
				scanf(" %255s", name);
				delete_in_db(f, name, dbname, &cond);
				if(cond == 1){
					return(0);
				}
				break;
			}
			case('c') : {
				f = close_db(f);
				break;
			}
			case('q') : {
				if( f != NULL){
					fclose(f);
					return(0);
				}
				else {
					return(0);
				}
			}
		}
	}
	
	return(0);
}
