#include <stdio.h>
#include <core/core.h>

#include <core/fuckmake.h>
#include <util/util.h>

bool ProcessArgument(const String& arg) {
	if (arg == "-dmsg") {
		FuckMake::PrintDebugMessages = true;
		return true;
	}

	return false;
}

int main(int argc, char** argv) {
	String target = FuckMake::DefaultTargetName;

	for (int i = 1; i < argc; i++) {
		if (!ProcessArgument(argv[i])) {
			if (target != FuckMake::DefaultTargetName){
				Log(LogLevel::Warning, "Warning: target already set \"%s\" overwriting with \"%s\"", target.str, argv[i]);
			}

			target = argv[i];
		}
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

	FuckMake::InitLock();

	FuckMake fMake(path, "Fuckfile");

	fMake.Run(target);

	FuckMake::DestroyLock();

	return 0;
}