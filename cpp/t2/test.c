#include <stdio.h>

void log(const char * msg) {
	printf("msg : %s\n", msg);
}

void print_file_lines(FILE * fp) {
	char buf[1024];
	char * ptr = NULL;
	while((ptr = fgets(buf, 1024, fp)) != NULL) {
		fputs(ptr, stdout);
	}
}

void print_file_one_ch(FILE * fp) {
	int ch;
	while((ch = getc(fp)) != EOF) {
		putc(ch, stdout);
	}

	if(feof(fp)) {
		fputs("end of file\n", stdout);
	}

}

void print_file_binary(FILE * fp) {
	struct NAME {
		int len;
		char buf[1024];
	}; 

	FILE * tmpfp = tmpfile();
	
	struct NAME name;
	char buf[1024];
	char * ptr = NULL;
	while((ptr = fgets(buf, 1024, fp)) != NULL) {
		name.len = strlen(ptr);
		strcpy(name.buf, ptr);
		if(fwrite(&name, sizeof(struct NAME), 1, tmpfp)) {
//			printf("write log %s\n", name.buf);
//		} else {
//			printf("write log err\n" );
		}
	}

	fseek(tmpfp, 0, SEEK_SET);
	//rewind(tmpfp);
	while(fread(&name, sizeof(struct NAME), 1, tmpfp) > 0) {
		printf("%s", name.buf);
	}

}


int main(int argc,char * argv[]) {

	if(argc != 2) {
		return 0;
	}

	log(argv[1]);

	FILE * fp = fopen(argv[1], "r");

//	print_file_one_ch(fp);
	print_file_binary(fp);
	return 0;
}
