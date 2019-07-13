#include "util.h"
#include <memory.h>
#include <Windows.h>

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

	return files;
}