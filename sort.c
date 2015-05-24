#include <errno.h>
#include <stdio.h>
#include <string.h>

int close_f (FILE *f, char *name) {
	int st = fclose(f);
	if (st != 0) {
		printf("Can't close file %s\n", name);
		return -1;
	}
	return 0;
}

int cmp (const void *a, const void *b) {
	if ( (*(int*)a < 0 && *(int*)b > 0) )
		return -1;
	if ((*(int*)a > 0 && *(int*)b < 0) ) 
		return 1;
	return (*(int*)a - *(int*)b);
}

int main (int argc, char *argv[]) {
	if (argc < 3) {
		printf("Usage: ./sort file1 file2 ... outputfile\n");
		return -1;
	}
	argc--;
	FILE *out = fopen(argv[argc], "w");
	if (out == NULL) {
		printf("Can't open file %s: %s\n", argv[argc], strerror(errno));
		return -1;
	}
	argc--;
	int nums[256];
	int i = 0;
	char str[20];
	while (argc > 0) {
		FILE *in = fopen(argv[argc], "r");
		if (in == NULL) {
			printf("Can't open file %s: %s\n", argv[argc], strerror(errno));
			argc--;
			continue;
		}
		char c = fgetc(in);
		if (c == EOF)
			if (strcmp(strerror(errno),"Success") == 0) {
				printf("Can't read from file %s: Empty file\n", argv[argc]);
				close_f(in, argv[argc]);
				argc--;
				continue;
			} else {
				printf("Can't read from file %s: %s\n", argv[argc], strerror(errno));
				close_f(in, argv[argc]);
				argc--;
				continue;
			}
		int l = 0, j = 0;
		int new_str = 1;
		int num;
		while (c != EOF) {
			if (c == '-') {
				if (new_str != 1) {
					printf("Wrong format of file %s. Must be\nnumber1\nnumber2\n...\n", argv[argc]);
					break;	
				}
				new_str = 0;
				str[j++] = c;
			} else {
				if (c > 47 && c < 58) {
					if (l > 10) {
						printf("Too long number in file %s\n", argv[argc]);
						while (c != 10)
							c = fgetc(in);
						continue;
					}
					new_str = 0;
					str[j++] = c;
					l++;
				} else {
					if (c == 10) {
						num = atoi(str);
						nums[i++] = num;
						l = j = 0;
						memset(str, 0, sizeof(str));
						new_str = 1;
					} else {
						printf("Wrong format of file %s. Must be\nnumber1\nnumber2\n...\n", argv[argc]);
						break;
					}
				}
			}
			c = fgetc(in);
		}
		if (strcmp(strerror(errno),"Success") != 0) {
			printf("Can't read from file %s: %s\n", argv[argc], strerror(errno));
			close_f(in, argv[argc]);
			argc--;
			continue;
		}
		close_f(in, argv[argc]);
		argc--;
	}
	qsort(nums, i, sizeof(int), cmp);
	while (i != 0) {
		int st = fprintf(out, "%d\n", nums[i-1]);
		if (st < 0) {
			printf("Can't write to file: %s\n", strerror(errno));
			close_f(out,"output");
			return -1;
		}
		i--;
	}
	close_f(out,"output");
	return 0;
}
