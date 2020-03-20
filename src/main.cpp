#include <stdio.h>
#include <core/core.h>

#include <core/fuckmake.h>
#include <util/util.h>

int main(int argc, char** argv) {
	String target("__default__");

	if (argc >= 2) {
		target = argv[1];
	}

	Log(LogLevel::Debug, "Starting FuckMake");

	char tmp[1024];

#ifdef _WIN32
	GetCurrentDirectory(1024, tmp);
#else
	getcwd(tmp, 1024);
#endif

	String path(tmp);

	if (!path.EndsWith("/")) {
		path.Append("/");
	}

	FuckMake fMake(path, "Fuckfile", target);

	return 0;
}