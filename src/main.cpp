#include <stdio.h>
#include <core/core.h>

#include <parsing/parsing.h>
#include <util/util.h>


int main(int argc, char** argv) {
	String target("__default__");

	if (argc >= 2) {
		target = argv[1];
	}

	Log(LogLevel::Debug, "Starting FuckMake");
	printf("%s\n", argv[0]);

	FuckMake fMake(argv[0], "Fuckfile", target);

	return 0;
}