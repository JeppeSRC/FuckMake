#include "util.h"
#include <memory.h>
#include <Windows.h>
#include <stdarg.h>

uint8* ReadFile(const String& filename, uint64* size) {
	ASSERT(size != 0);

	FILE* file = fopen(filename.str, "rb");

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

uint8 WriteFile(const String& filename, const void* data, uint64 size) {
	FILE* file = fopen(filename.str, "wb");

	if (!file) {
		return 0;
	}

	fwrite(data, size, 1, file);
	fclose(file);

	return 1;
}

void CreateFolderAndFile(const String& filename) {
	if (filename.Count("/") == 0) {
		CloseHandle(CreateFile(filename.str, GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0));
		return;
	}

	List<String> folders = filename.Split("/");

	String path = folders[0] + "/";

	CreateDirectory(path.str, 0);

	for (uint64 i = 1; i < folders.GetCount() - 1; i++) {
		CreateDirectory((path.Append(folders[i] + "/")).str, 0);
	}

	CloseHandle(CreateFile(path.Append(folders[folders.GetCount()-1]).str, GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0));
}

List<FileInfo> ScanDirectory(const String& directory) {
	WIN32_FIND_DATA fData;
	HANDLE handle = FindFirstFile((directory + "*").str, &fData);

	if (handle == INVALID_HANDLE_VALUE) {
		//TODO:
	}

	List<FileInfo> files;

	uint8 failCount = 0;

	while (true) {
		bool result = FindNextFile(handle, &fData);

		if (!result) {
			if (GetLastError() == ERROR_NO_MORE_FILES) {
				break;
			} else {
				failCount++;

				if (failCount == 10) {
					break;
				}

				continue;
			}
		}

		if (fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			String dirName(fData.cFileName);
			if (dirName == "." || dirName == "..") continue;

			files.Add(ScanDirectory(directory + dirName + "/"));
		} else {
			FileInfo info;

			info.filename = directory + fData.cFileName;

			files.Add(info);
		}

		
	}

	FindClose(handle);

	return files;
}

#define COLOR_INFO 0b00001111
#define COLOR_DEBUG 0b00001010
#define COLOR_WARNING 0b00001110
#define COLOR_ERROR 0b00001100

void Log(LogLevel level, const char* message, ...) {
	va_list list;
	va_start(list, message);

	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(h, &info);

	switch (level) {
	case LogLevel::Info:
		SetConsoleTextAttribute(h, COLOR_INFO);
		break;
	case LogLevel::Debug:
		SetConsoleTextAttribute(h, COLOR_DEBUG);
		break;
	case LogLevel::Error:
		SetConsoleTextAttribute(h, COLOR_ERROR);
		break;
	}

	vprintf(message, list);

	SetConsoleTextAttribute(h, info.wAttributes);

	printf("\n");
}