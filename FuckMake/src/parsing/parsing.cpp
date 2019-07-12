#include "parsing.h"

FuckMake::FuckMake(const String& filename) {
	uint64 size = 0;
	uint8* data = ReadFile(filename.str, &size);

	String string((const char* const)data, size);

	Parse(string);
}

void FuckMake::Parse(String string) {

}