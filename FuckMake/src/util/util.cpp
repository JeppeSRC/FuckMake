#include "util.h"
#include <memory.h>

uint8* ReadFile(const char* const filename, uint64* size) {
	ASSERT(size != 0);

	FILE* file = fopen(filename, "rb");

	if (!file) {
		return 0;
	}

	_fseeki64(file, 0, SEEK_SET);
	_fseeki64(file, 0, SEEK_END);
	*size = (uint64)_ftelli64(file);
	_fseeki64(file, 0, SEEK_SET);

	uint8* data = new uint8[*size];

	fread(data, *size, 1, file);
	fclose(file);


	return data;
}

uint8 WriteFile(const char* const filename, const void* data, uint64 size) {
	FILE* file = fopen(filename, "wb");

	if (!file) {
		return 0;
	}

	fwrite(data, size, 1, file);
	fclose(file);

	return 1;
}