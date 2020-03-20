#pragma once

#include <util/util.h>

bool CheckIncludes(const String& filename, const List<String>& includeDirs, struct stat outFile);