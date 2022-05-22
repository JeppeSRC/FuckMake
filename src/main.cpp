#include <stdio.h>
#include <string>
#include <core/core.h>

#include <core/fuckmake.h>
#include <util/util.h>

#define FUCKMAKE_VERSION "V1.33"

void PrintVersion() {
	String version;

#ifdef _MSC_VER
	version = String("MSVC ") + std::to_string(_MSC_VER).c_str();
#elif defined(__VERSION__)
	version = String("g++ ") + __VERSION__;
#else
	version = "Unknown";
#endif

	printf("FuckMake: %s\nBuild: %s (%s)\n", FUCKMAKE_VERSION, __DATE__, version.str);
}

bool ProcessArgument(const String& arg) {
	if (arg == "-dmsg") {
		FuckMake::PrintDebugMessages = true;
		return true;
	} if (arg == "--version") {
		PrintVersion();
		exit(0);
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