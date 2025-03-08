#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define LEN(x) (sizeof(x)/sizeof(*x))

char *getcolor(const char *p)
{
	int i;
	char *s;
	int64_t n;
	struct colname *co;

	if (p[0] == '#') {
		p++;
		switch (strlen(p)) {
		case 6:
			n = strtoll(p, &s, 16);
			if (s[0] != '\0')
				return NULL;
			break;
		default:
			return NULL;
			break;
		}
		p--;
		return p;
	}
	return p;
}
