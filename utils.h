#ifndef UTILS_H_
#define UTILS_H_

#include <string.h>
#include <stdio.h>

inline char* concat(const char* str1, const char* str2) {
	size_t size = strlen(str1) + strlen(str2) + 1;
	char* result = malloc(size);
	snprintf(result, size, "%s%s", str1, str2);
	return result;
}

#endif
