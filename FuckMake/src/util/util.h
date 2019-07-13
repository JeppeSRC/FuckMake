#pragma once

#include <core/core.h>
#include <stdio.h>
#include "string.h"

#define CLAMP(x, min, max) (x > max ? max : x < min ? min : x)

uint8* ReadFile(const String& filename, uint64* size);
uint8  WriteFile(const String& filename, const void* data, uint64 size);

struct FileInfo {
	String filename;
};

List<FileInfo> ScanDirectory(const String& directory);