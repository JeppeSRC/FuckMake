#include <stdio.h>
#include <core/core.h>

#include <parsing/parsing.h>
#include <util/util.h>

int main(int argc, char** argv) {

	String target("__default__");

	if (argc == 2) {
		target = argv[1];
	}

	FuckMake fMake("Fuckfile", target);

	return 0;
}