#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <assert.h>
#include <stdbool.h>
#include <ctype.h>
#include "symbol_table.h"
#include "emu_registers.h"

#define MAX_LINE_LEN 1024

bool is_empty_line(char *str) {
	for (; *str; ++str) {
		if (!isspace(*str)) {
			return false;
		}
	}
	return true;
}

int main(int argc, char **argv) {
	assert(argc == 3);
	assert(pc_load() == 0);

	FILE *in = fopen(argv[1], "r");
	if (!in) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}
	
	char buffer[MAX_LINE_LEN];
	const char *label_pattern = "^\\s*([a-zA-Z_\\.][a-zA-Z0-9$_\\.]*):\\s*\n$";
	regex_t label_regex;
	if (regcomp(&label_regex, label_pattern, REG_EXTENDED)) {
		fprintf(stderr, "Failed to compile regex\n");
		exit(EXIT_FAILURE);
	}
	symbol_table_create(16);
	regmatch_t matches[2];
	while (fgets(buffer, sizeof(buffer), in)) {
		if (regexec(&label_regex, buffer, 2, matches, 0) == 0) { // matches labels
			int start = matches[1].rm_so;
			int end = matches[1].rm_eo;
			int len = end - start;
			char *label = malloc(len + 1);
			strncpy(label, buffer + start, len);
			label[len] = '\0';
			symbol_table_put(label, pc_load());	
		} else if (!is_empty_line(buffer)) {
			pc_jump(1); // skips empty lines
		}
	}
	fclose(in);
		
	assert(symbol_table_get(".label1") == 0);
	assert(symbol_table_get("label2") == 12);
	assert(symbol_table_get("l_abel3") == 20);
	assert(symbol_table_get("labe$l4") == 24);
	regfree(&label_regex);
	return EXIT_SUCCESS;
}
