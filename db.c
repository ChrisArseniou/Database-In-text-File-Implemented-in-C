#include "stdio.h"
#include "string.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

//h dikia mas fread gia na ginetai o elegxos gia lathi edw kai oxi sto programma
int myfread(void *ptr, size_t size, size_t nmemb, FILE *f){
	int res=0;
	
	res = fread(ptr,size,nmemb, f);
	if(res < nmemb){
		if (feof(f)) {
			return 1;
		}
		else if (ferror(f)) {
			perror("\nError");
			fclose(f);
			return(0);
		}
	}
	else {
		return(1);
	}
	return(1);
	
}

//h dikia mas fwrite gia na ginetai o elegxos gia lathi edw kai oxi sto programma
int myfwrite(void *ptr, size_t size, size_t nmemb, FILE *f){
	int res=0;
	
	res = fwrite(ptr,size,nmemb, f);
	fflush(f);
	if(res < nmemb){
		if (feof(f)) {
			return 1;
		}
		else if (ferror(f)) {
			perror("\nError");
			fclose(f);
			return(0);
		}
	}
	else {
		return(1);
	}
	return(1);
}

//h dikia mas fseek gia na ginetai o elegxos gia lathi edw kai oxi sto programma
int myfseek(FILE *f, long int offset, int whence){
	int res=0;
	
	res = fseek(f, offset, whence);
	if(res == -1){
		if (feof(f)) {
			return 1;
		}
		else if (ferror(f)) {
			perror("\nError");
			fclose(f);
			return(0);
		}
	}
	else {
		return(1);
	}
	return(1);
}

void add(FILE *f, FILE *fn, int bytes_left, int *cond){
	char *transfer=NULL;
	int bytes_to_pass_by=0,add_controller=0,res=0;
	
	bytes_to_pass_by = bytes_left;
	if(bytes_to_pass_by <= 512){
		transfer = (char*)malloc(bytes_to_pass_by*sizeof(char));
		res = myfread(transfer, 1, bytes_to_pass_by, fn);
		if(res == 0){
			*cond = 1;
			return;
		}
		res = myfwrite(transfer, 1, bytes_to_pass_by, f);
		if(res == 0){
			*cond = 1;
			return;
		}
		free(transfer);
		return;
	}
	else {
		bytes_left = bytes_to_pass_by;
		while(1){
			add_controller = bytes_left - 512;
			if(add_controller >= 0){
				transfer = (char*)malloc(512);
				res = myfread(transfer, 1, 512, fn);
				if(res == 0){
					*cond = 1;
					return;
				}
				res = myfwrite(transfer, 1, 512, f);
				if(res == 0){
					*cond = 1;
					return;
				}
				free(transfer);
				if(bytes_left == 0){
					return;
				}
			}
			else {
				if(bytes_left != 0){
					transfer = (char*)malloc(bytes_left);
					res = myfread(transfer, 1, bytes_left, fn);
					if(res == 0){
						*cond = 1;
						return;
					}
					res = myfwrite(transfer, 1, bytes_left, f);
					if(res == 0){
						*cond = 1;
						return;
					}
					free(transfer);
					return;
				}
				if(bytes_left < 0){
					return;
				}
			}
		bytes_left = bytes_left - 512;
		}
	}
	
	return;
}



FILE *open_db(char base_name[], FILE *f, int *size, int *cond){
	char set[]="!DATABASE!", check[11];
	int error,res=0;
	
	//kleisimo ths proigoymenis vasis an yparxei
	if(f != NULL){
		fclose(f);
	}

	//xrhsimopoioume r+ gia na elegxoyme an yparxei hdh vash kai na mporoyme na diavasoyme kai na grapsoyme se/apo auth 
	f = fopen(base_name,"r+");
	
	if(f != NULL){
		error = ferror(f);
		if(error != 0 ){
			perror("\nError: \n");
			*cond = 1;
			return(f);
		}
	}
	
	if(f == NULL){
		//vash me tetoio onoma den yparxei, opote xrhsimopoioume to w+ gia na thn dimioyrghsoume kai na diavasoyme , grapsoyme se/apo ayth
		f = fopen(base_name, "w+");
		
		if(f != NULL){
			error = ferror(f);
			if(error != 0 ){
				perror("\nError: \n");
				*cond = 1;
				return(f);
			}
			//prosthetoyme to tag mas sthn vash
			res = myfwrite(set,1,10,f);
			if(res == 0){
				*cond = 1;
				return(f);
			}
			return(f);
		}
		else {
			printf("\nError opening %s\n", base_name);
			return(NULL);
		}
	}
	else {
		rewind(f);
		//diavasoyme ta prwta 10 bytes gia na doyme an exei thn swsth morfh
		res = myfread(check,1,10,f);
		if(res == 0){
			*cond = 1;
			return(f);
		}
		
		check[10] = '\0';
		if(strcmp(set, check) != 0){
			printf("\nInvalid db file %s\n",base_name);
		}
		else {
			return(f);
		}
	}
	return(NULL);
}

void import_db(char fname[], char name[], FILE *f, char dbname[], int *cond){
	FILE *fn=NULL;
	char set[]="!DATABASE!", name_check[256];
	int name_size=0,fname_size=0,res=0;
	struct stat filestat1, filestat2;
	long int pos;
	
	//an den yparxei anoixth vash 
	if(f == NULL){
		printf("\nNo open db file.\n");
		return;
	}
	//anoigma toy antikeimenoy gia diavasma
	fn = fopen(fname,"r");
	//se periptwsh poy den yparxei to antikeimeno
	if(fn == NULL){
		printf("\nFile %s not found.\n", fname);
		return;
	}
	
	pos = 10;
	
	//thetoyme thn thesi grapsimatos/anagnwsis meta to tag
	res = myfseek(f, strlen(set), SEEK_SET);
	if(res == 0){
		*cond = 1;
		fclose(fn);
		return;
	}
	
	stat(dbname, &filestat1);
	
	//elegxos gia to an yparxei to antikeimeno hdh
	while(1){
		//an to pos exei ftasei sto telos kanei break
		if(pos == filestat1.st_size){
			break;
		}
		
		res = myfread(&name_size,sizeof(int),1, f);
		if(res == 0){
			*cond = 1;
			fclose(fn);
			return;
		}
		pos = pos + sizeof(int);
		
		res = myfread(name_check, 1, name_size, f);
		if(res == 0){
			*cond = 1;
			fclose(fn);
			return;
		}
		pos = pos + name_size;
		
		name_check[name_size] = '\0';
	
		if(name_size == strlen(name) && strcmp(name, name_check) == 0){
			printf("\nObject %s already in db.\n", name);
			return;
		}
		else {
			res = myfread(&fname_size, 1, sizeof(int), f);
			if(res == 0){
				*cond = 1;
				fclose(fn);
				return;
			}
			pos = pos + sizeof(int);
			
 			res = myfseek(f, fname_size, SEEK_CUR);
			if(res == 0){
				*cond = 1;
				fclose(fn);
				return;
			}
			pos = pos + fname_size;
			continue;
		}
	}

	name_size = strlen(name);
	
	res = myfseek(f, 0, SEEK_END);//metakinw thn thesi eggrafis sto telos
	if(res == 0){
		*cond = 1;
		fclose(fn);
		return;
	}
	
	//egkatastoyme to antikemeno sthn vash xrhsimopoiontas thn domh poy exoyme epileksei
	res = myfwrite(&name_size, 1, sizeof(int), f);
	if(res == 0){
		*cond = 1;
		fclose(fn);
		return;
	}
	res = myfwrite(name, 1, sizeof(char)*name_size, f);
	if(res == 0){
		*cond = 1;
		fclose(fn);
		return;
	}
	stat(fname, &filestat2);
	fname_size = filestat2.st_size;
	res = myfwrite(&fname_size, 1, sizeof(int), f);
	if(res == 0){
		*cond = 1;
		fclose(fn);
		return;
	}
	//prosthetoyme to antikeimeno
	add(f,fn, filestat2.st_size, cond);
	fclose(fn);
	return;
}

void find_in_db(FILE *f, char name[],char dbname[], int *cond){
	int name_size=0,bytes_to_pass_by=0, outcome=0;
	long int pos=0;
	char name_to_check[256],*res=NULL;
	struct stat filestat;

	if(f == NULL){
		printf("\nNo open db file.\n");
		return;
	}
	
	outcome = myfseek(f, 10, SEEK_SET);//metakinw thn thesi eggrafis meta to tag
	if(outcome == 0){
		*cond = 1;
		return;
	}
	pos = 10;
	stat(dbname, &filestat);
	
	printf("\n##\n");
	//ektipwsi kathe antikeimenoy poy vriskete sthn vash
	if(strcmp(name, "*") == 0){
		while(1){
			if(pos == filestat.st_size){
				break;
			}
			
			outcome = myfread(&name_size, 1, sizeof(int), f);
			if(outcome == 0){
				*cond = 1;
				return;
			}
			pos = pos + sizeof(int);
			
			outcome = myfread(name_to_check, 1 , name_size, f);
			if(outcome == 0){
				*cond = 1;
				return;
			}
			pos = pos + name_size;
			name_to_check[name_size] = '\0';
			printf("%s\n", name_to_check);
			outcome = myfread(&bytes_to_pass_by, 1, sizeof(int), f);
			pos = pos + sizeof(int);
			if(outcome == 0){
				*cond = 1;
				return;
			}
			outcome = myfseek(f, bytes_to_pass_by, SEEK_CUR);
			if(outcome == 0){
				*cond = 1;
				return;
			}
			pos = pos + bytes_to_pass_by;
		}
	}
	else {
		while(1){
			if(pos == filestat.st_size){
				break;
			}
			outcome = myfread(&name_size, sizeof(int), 1, f);
			pos = pos + sizeof(int);
			if(outcome == 0){
				*cond = 1;
				return;
			}
			outcome = myfread(name_to_check, name_size, 1, f);
			if(outcome == 0){
				*cond = 1;
				return;
			}
			pos = pos + name_size;
			name_to_check[name_size] = '\0';
			outcome = myfread(&bytes_to_pass_by, sizeof(int), 1, f);
			if(outcome == 0){
				*cond = 1;
				return;
			}
			pos = pos + sizeof(int);
			pos = pos + bytes_to_pass_by;
			if(strlen(name_to_check) == strlen(name)){
				if(strcmp(name_to_check, name) == 0){
					printf("%s\n", name);
					outcome = myfseek(f, bytes_to_pass_by, SEEK_CUR);
					if(outcome == 0){
						*cond = 1;
						return;
					}
					continue;
				}
				else{
					outcome = myfseek(f, bytes_to_pass_by, SEEK_CUR);
					if(outcome == 0){
						*cond = 1;
						return;
					}
					continue;
				}
			}
			else if(strlen(name_to_check) < strlen(name)){
				outcome = myfseek(f, bytes_to_pass_by, SEEK_CUR);
				if(outcome == 0){
						*cond = 1;
						return;
				}
				continue;
			}
			else if(strlen(name_to_check) > strlen(name)){
				res = strstr(name_to_check, name);
				if(res != NULL){
					printf("%s\n", name_to_check);
				}
				outcome = myfseek(f, bytes_to_pass_by, SEEK_CUR);
				if(outcome == 0){
					*cond = 1;
					return;
				}
			}
		}
	}
	return;
}

void export_db(FILE *f, char dbname[], char name[], char fname[], int *cond){
	FILE *fn=NULL;
	int name_size=0, bytes_to_pass_by=0,res=0;
	long int pos = 0;
	char name_to_check[256];
	struct stat filestat;
	
	if(f == NULL){
		printf("\nNo open db file.\n");
		return;
	}
	res = myfseek(f, 10, SEEK_SET);//metakinw thn thesi eggrafis meta to tag
	if(res == 0){
		*cond = 1;
		return;
	}
	pos = 10;
	//elegxoyme an yparxei hdh to file poy theloyme na apothikeusoyme to antikeimeno mas
	fn = fopen(fname, "r+");
	if(fn != NULL){
		printf("\nFile %s exists.\n", fname);
		return;
	}
	stat(dbname, &filestat);
	while(1){
		if(pos == filestat.st_size){
			printf("\nObject %s not in db.\n", name);
			return;
		}
		
		res = myfread(&name_size, sizeof(int), 1, f);
		if(res == 0){
			*cond = 1;
			fclose(fn);
			return;
		}
		pos = pos + sizeof(int);
		
		res = myfread(name_to_check, name_size, 1, f);
		if(res == 0){
			*cond = 1;
			fclose(fn);
			return;
		}
		pos = pos + name_size;
		name_to_check[name_size] = '\0';
		
		res = myfread(&bytes_to_pass_by, sizeof(int), 1, f);
		if(res == 0){
			*cond = 1;
			fclose(fn);
			return;
		}
		pos = pos + sizeof(int);
		pos = pos + bytes_to_pass_by;
		if(strlen(name_to_check) == strlen(name)){
			if(strcmp(name_to_check, name) == 0){
				fn = fopen(fname, "w+b+");
				add(fn,f, bytes_to_pass_by, cond);
				fclose(fn);
				return;
			}
		}
		res = myfseek(f, bytes_to_pass_by, SEEK_CUR);
		if(res == 0){
			*cond = 1;
			fclose(fn);
			return;
		}
	}
}

void delete_in_db(FILE *f, char name[], char dbname[],int *cond){
	int name_size=0,bytes_left=0,pos=0,res=0,fname_size=0,size_of_object=0;
	long int read_pos=0,write_pos=0;
	char name_check[256], *transfer=NULL;
	struct stat filestat;
	
	if(f == NULL){
		printf("\nNo open db file.\n");
		return;
	}
	
	//metakinoyme thn thesi meta tag
	res = myfseek(f, 10, SEEK_SET);
	if(res == 0){
		*cond = 1;
		return;
	}
		
	pos = 10;
	stat(dbname, &filestat);
	while(1){
		//an to pos exei ftasei sto telos kanei return
		if(pos == filestat.st_size){
			printf("\nObject %s not in db.\n", name);
			return;
		}
		//diavazoyme kai kanoyme skip thn domh twn antikeimenwn me vash thn morfh poy ta prosthetoyme gia pio grhgora apotelesmata
		res = myfread(&name_size,sizeof(int),1, f);
		if(res == 0){
			*cond = 1;
			return;
		}
		pos = pos + sizeof(int);

		res = myfread(name_check, 1, name_size, f);
		if(res == 0){
			*cond = 1;
			return;
		}
		pos = pos + name_size;
		
		name_check[name_size] = '\0';
		
		res = myfread(&fname_size, 1, sizeof(int), f);
		if(res == 0){
			*cond = 1;
			return;
		}
		pos = pos + sizeof(int);
		
		res = myfseek(f, fname_size, SEEK_CUR);
		if(res == 0){
			*cond = 1;
			return;
		}
		pos = pos + fname_size;
		
		if(name_size == strlen(name) && (strcmp(name, name_check) == 0)){
			break;
		}
	}
	//exoyme dyo diaforetikes thesis.H write deixnei ekei poy vriskete poy theloyme na diagrapsoyme enw h read deixnei sto telos toy antikeimenoy(gia arxh)
	read_pos = pos;
	size_of_object = 2*sizeof(int) + name_size + fname_size;
	write_pos = read_pos - size_of_object;

	//prepei na metaferoyme ta ypoloipa stoixeia sthn thesi toy antikeimenoy poy theloyme na diagrapsoyme
	while(1){
		if(read_pos == filestat.st_size){
			fclose(f);
			//kovei to arxeio oso to synoliko megethos toy antikeimenoy pou diagraftike(mazi me thn pliroforia poy kratame gia ayto
			res = truncate(dbname, filestat.st_size - size_of_object);
			if(res == -1){
				*cond = 1;
				return;
			}
			
			f = fopen(dbname, "r+");
			
			res = myfseek(f, 0, SEEK_END);
			if(res == 0){
				*cond = 1;
				return;
			}
			return;
		}
		//an ta ypoloipa bytes poy theloyme na metaferoyme einai ligotera twn 512
		if((read_pos + 512) >= filestat.st_size){
			bytes_left = filestat.st_size - read_pos;
			transfer = (char*)malloc(bytes_left*sizeof(char));
			res = myfseek(f, read_pos, SEEK_SET);
			if(res == 0){
				*cond = 1;
				return;
			}
			res = myfread(transfer, 1, bytes_left, f);
			if(res == 0){
				*cond = 1;
				return;
			}
			res = myfseek(f, write_pos, SEEK_SET);
			if(res == 0){
				*cond = 1;
				return;
			}
			res = myfwrite(transfer, 1, bytes_left, f);
			if(res == 0){
				*cond = 1;
				return;
			}
			free(transfer);
			read_pos = read_pos + bytes_left;
			write_pos = write_pos  + bytes_left;
		}
		else {
			bytes_left = 512;
			transfer = (char*)malloc(bytes_left*sizeof(char));
			res = myfseek(f, read_pos, SEEK_SET);
			if(res == 0){
				*cond = 1;
				return;
			}
			res = myfread(transfer, 1, bytes_left, f);
			if(res == 0){
				*cond = 1;
				return;
			}
			res = myfseek(f, write_pos, SEEK_SET);
			if(res == 0){
				*cond = 1;
				return;
			}
			res = myfwrite(transfer, 1, bytes_left, f);
			if(res == 0){
				*cond = 1;
				return;
			}
			free(transfer);
			read_pos = read_pos + bytes_left;
			write_pos = write_pos  + bytes_left;
		}
	}
	return;
}

FILE *close_db(FILE *f){
	
	if( f != NULL){
		fclose(f);
	}
	else {
		printf("\nNo open db file.\n");
	}
	return(NULL);
}
