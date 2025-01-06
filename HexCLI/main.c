#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <Windows.h>

#define BYTES 16
#define LINES 28
#define RBYTES (BYTES * LINES)

#define UP 1
#define DOWN 2
#define CLOSE 3

static long get_file_size(FILE* file);

int main(int argc, const char* argv[]) {
	if (argc < 2) return 1;

	uint8_t buff[RBYTES];
	
	// maybe for future
	int bytes = BYTES;
	int lines = LINES;

	int pos_now = 1;
	int status = DOWN;

	FILE* file = fopen(argv[1], "rb");
	if (file == NULL) return 2;

	const long file_size = get_file_size(file);

	int all_positions = file_size / (lines * bytes);
	if (all_positions == 0) all_positions = 1;

	while (status != CLOSE) {

		fread(buff, RBYTES, 1, file);

		lines = file_size / bytes;
		if (all_positions - pos_now == 0) {
			if (lines == 0 || file_size - lines*bytes > 0)
				lines++;
			
		} else {
			bytes = BYTES;
			lines = LINES;
		}


REPEAT:
		system("cls");
		printf("%d/%d | file size: %ld bytes\n\n", pos_now, all_positions, file_size);

		for (int i = 0; i < lines; i++) {

			printf("|0x%.8X| -> |", ((int)ftell(file) - bytes * lines) + i * bytes);

			for (int j = 0; j < bytes; j++) {
				if (buff[j + i * bytes] > 15) printf("%X ", buff[j + i * bytes]);
				else printf("0%X ", buff[j + i * bytes]);
			}
			
			printf("| -> |");

			for (int j = 0; j < bytes; j++) {
				if (buff[j + i * bytes] > 0x7f || buff[j + i * bytes] < 0x21 || buff[j + i * bytes] == '\n' || buff[j + i * bytes] == 0)
					putc('.', stdout); // buff[k] > 0x7f
				else
					putc(buff[j + i * bytes], stdout);
			}
			puts("|");

		}

		switch (getchar())
		{
		case '1':
			status = UP;
			if (pos_now != 1) {
				pos_now--;
				fseek(file, -(RBYTES * 2), SEEK_CUR); // lines * bytes
			} else goto REPEAT;

			break;
		case '2':
			status = DOWN;
			if (pos_now != all_positions) pos_now++;
			break;
		case 'x':
			status = CLOSE;
			break;
		default:
			goto REPEAT;
		}
	}

	fclose(file);
	
	return 0;
}

static long get_file_size(FILE* file) {
	long size = 0L;

	fseek(file, 0L, SEEK_END);
	size = ftell(file);
	
	rewind(file);

	return size;
}
