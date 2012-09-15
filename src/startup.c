#include <string.h>
#include <stdint.h>
#include <stdio.h>

#define LINE_BUF_LEN 256

struct
configuration
{
	char		*addr;
	uint16_t	port;
};

struct configuration
read_configuration(char *fn)
{
	char		*line_buf;
	size_t		linecap = LINE_BUF_LEN;
	struct configuration config;
	FILE *fp = fopen(fn, "r");

	getline(&line_buf, &linecap, fp);

	fclose(fp);

	return config;
}
