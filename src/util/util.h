#pragma once

#include <core/core.h>
#include <stdio.h>
#include <sys/stat.h>
#include "string.h"

#define CLAMP(x, min, max) (x > max ? max : (x < min ? min : x))

uint8* ReadFile(const String& filename, uint64* size);
uint8  WriteFile(const String& filename, const void* data, uint64 size);

void CreateFolderAndFile(const String& filename);

List<String> ScanDirectory(const String& directory);

struct FileInfo {
	String filename;
	struct stat fInfo;
};

List<FileInfo> GetFileInfo(const String& files);

enum class LogLevel {
	Info,
	Debug,
	Error
};

void Log(LogLevel level, const char* message, ...);
