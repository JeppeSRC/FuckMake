#pragma once

#include <core/core.h>
#include <stdio.h>

#define CLAMP(x, min, max) (x > max ? max : x < min ? min : x)

uint8* ReadFile(const char* const filename, uint64* size);
uint8  WriteFile(const char* const filename, const void* data, uint64 size);