#include "parsing.h"

void RemoveComments(String& file) {
	uint64 index = 0;

	while ((index = file.Find("/*")) != String::npos) {
		uint64 end = file.Find("*/");

		if (end == String::npos) end = file.length - 2;

		file.Remove(index, end+1);
	}

	while ((index = file.Find("//")) != String::npos) {
		uint64 newLine = file.Find("\n", index);

		if (newLine == String::npos) {
			newLine = file.Find("\r", index);

			if (newLine == String::npos) newLine = file.length - 1;
		}

		file.Remove(index, newLine);
	}
}

void Error(String filename) {
	Log(LogLevel::Error, "CheckInclude: Invalid include statement in \"%s\"", filename.str);
	exit(1);
}

bool CheckInclude(String currentFile, const String& include, const List<String>& dirs, struct stat outFile, bool noLocal) {
	uint64 index = currentFile.FindReversed('/');

	if (index == String::npos) {
		index = currentFile.FindReversed('\\');
	}

	if (index == String::npos) {
		currentFile = "";
	} else {
		currentFile.Remove(index+1, currentFile.length - 1);
	}

	struct stat file;

	if (!noLocal) {
		if (stat((currentFile + include).str, &file) >= 0) {
			if (file.st_mtime >= outFile.st_mtime) return true;
			return CheckIncludes(currentFile + include, dirs, outFile);
		}
	}

	for (uint64 i = 0; i < dirs.GetCount(); i++) {
		String dir = dirs[i];

		if (!dir.EndsWith("/") && !dir.EndsWith("\\")) dir.Append("/");

		if (stat((dir + include).str, &file) >= 0) {
			if (file.st_mtime >= outFile.st_mtime) return true;
			else if (CheckIncludes(dir + include, dirs, outFile)) {
				return true;
			}
		}
	}
	
	return false;
}

bool CheckIncludes(const String& filename, const List<String>& includeDirs, struct stat outFile) {
	uint64 size = 0;
	uint8* buffer = ReadFile(filename, &size);

	if (buffer == nullptr) {
		Log(LogLevel::Error, "CheckIncludes: \"%s\" doesn't exist", filename.str);
		exit(1);
	}

	String file((const char* const)buffer, size);

	delete[] buffer;

	RemoveComments(file);

	uint64 index = 0;

	while ((index = file.Find("#include")) != String::npos) {
		uint64 newLine = file.Find("\n", index);

		if (newLine == String::npos) {
			newLine = file.Find("\r", index);

			if (newLine == String::npos) newLine = file.length - 1;
		}

		uint64 num = file.Count("\"", index, newLine);

		if (num == 2) {
			uint64 start = file.Find("\"", index);
			uint64 end = file.Find("\"", start + 1);

			String include = file.SubString(start + 1, end - 1);

			if (CheckInclude(filename, include, includeDirs, outFile, false)) return true;
		} else if (num == 0) {
			uint64 start = file.Find("<", index);
			uint64 end = file.Find(">", index);
			
			if (start > newLine || end > newLine) {
				Error(filename);
			}

			String include = file.SubString(start + 1, end - 1);

			if (CheckInclude(filename, include, includeDirs, outFile, true)) return true;
		} else {
			Error(filename);
		}

		file.Remove(index, newLine);
	}

	return false;
}
