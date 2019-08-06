#include "util.h"
#include <stdarg.h>
#include <sys/stat.h>

uint8* ReadFile(const String& filename, uint64* size) {
	ASSERT(size != 0);

	struct stat info;
	*size = stat(filename.str, &info) < 0 ? 0 : (uint64)info.st_size;

	uint8* data = new uint8[*size];

	FILE* file = fopen(filename.str, "rb");

	if (!file) {
		return 0;
	}

	fread(data, *size, 1, file);
	fclose(file);

	return data;
}

uint8 WriteFile(const String& filename, const void* data, uint64 size) {
	FILE* file = fopen(filename.str, "wb");

	if (!file) {
		return 0;
	}

	fwrite(data, size, 1, file);
	fclose(file);

	return 1;
}

#ifdef _WIN32
	#include "dirent.h"
#else
	#include <dirent.h>
	#define CreateDirectory(dir, opts) mkdir(dir, 0755);
#endif

void CreateFolderAndFile(const String& filename) {
	if (filename.Count("/") != 0) {
		List<String> folders = filename.Split("/");

		String path("");

		for (uint64 i = 0; i < folders.GetCount() - 1; i++) {
			(void)CreateDirectory(path.Append(folders[i] + "/").str, 0);
		}
	}

	FILE* file = fopen(filename.str, "w");
	fclose(file);
}

List<String> ScanDirectory(const String& directory) {
	List<String> ret;

	DIR *dir = opendir(directory.str);
	if (dir == NULL) return ret;

	struct dirent *ent;
	while ((ent = readdir(dir)) != NULL) {
		String filename = ent->d_name;

		if (filename == "." || filename == "..") continue; // skip directory . & ..
		if (ent->d_type == DT_DIR) ret.Add(ScanDirectory(directory + filename + "/"));
		if (ent->d_type == DT_REG) { ret.Add(directory + filename);	}
	}

	closedir(dir);
	return ret;
}

#ifdef _WIN32

#define COLOR_INFO 0b00001111
#define COLOR_DEBUG 0b00001010
#define COLOR_WARNING 0b00001110
#define COLOR_ERROR 0b00001100
#define COLOR_RESET 0b11111111

void SetColor(WORD color) {
	static WORD defaultAttributes = 0;

	if (!defaultAttributes) {
		CONSOLE_SCREEN_BUFFER_INFO info;
		if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info)) return;
		defaultAttributes = info.wAttributes;
	}

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (color == COLOR_RESET ? defaultAttributes : color));
}

#else

#define COLOR_INFO "\u001b[37m"
#define COLOR_DEBUG "\u001b[32m"
#define COLOR_WARNING "\u001b[33m"
#define COLOR_ERROR "\u001b[31m"
#define COLOR_RESET "\u001b[0m"

void SetColor(const char* color) {
	printf("%s", color);
}
#endif

void Log(LogLevel level, const char* message, ...) {
	va_list list;
	va_start(list, message);

	switch (level) {
	case LogLevel::Info:
		SetColor(COLOR_INFO);
		break;
	case LogLevel::Debug:
		SetColor(COLOR_DEBUG);
		break;
	case LogLevel::Error:
		SetColor(COLOR_ERROR);
		break;
	}

	vprintf(message, list);
	printf("\n");

	SetColor(COLOR_RESET);
}