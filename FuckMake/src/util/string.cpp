#include "string.h"

String::String() : str(nullptr), length(0) { }

String::String(const char* const string) {
	ASSERT(string != nullptr);
	length = strlen(string);
	str = new char[length + 1];

	memcpy(str, string, length + 1);
}

String::String(const char* const string, uint64 len) : length(len) {
	ASSERT(string != nullptr && len != 0);
	str = new char[length + 1];
	str[length] = 0;
	memcpy(str, string, length);
}

String::String(const String& string) : length(string.length) {
	ASSERT(string.str != nullptr);
	str = new char[length + 1];
	memcpy(str, string.str, length + 1);
}

String::String(const String* string) : length(string->length) {
	ASSERT(string->str != nullptr);
	str = new char[length + 1];
	memcpy(str, string->str, length + 1);
}

String::String(String&& string) {
	str = string.str;
	length = string.length;
	string.str = nullptr;
	string.length = 0;
}

String::~String() {
	delete[] str;
}

String& String::operator=(const String& string) {
	if (this != &string) {
		delete[] str;
		str = new char[string.length + 1];
		length = string.length;
		memcpy(str, string.str, length + 1);
	}

	return *this;
}

String& String::operator=(String&& string) {
	if (this != &string) {
		delete[] str;
		str = string.str;
		length = string.length;

		string.str = nullptr;
		string.length = 0;
	}

	return *this;
}

String& String::Append(const String& string) {
	return Append(string.str);
}

String& String::Append(const char* const string) {
	ASSERT(string != nullptr);
	uint64 len = strlen(string);
	char* newStr = new char[length + len + 1];

	memcpy(newStr, str, length);
	memcpy(newStr + length, string, len + 1);

	delete[] str;

	str = newStr;
	length += len;

	return *this;
}

String& String::Remove(const String& start, const String& end) {
	return Remove(Find(start), Find(end) + end.length - 1);
}

String& String::Remove(const char* const start, const char* const end) {
	return Remove(Find(start), Find(end) + strlen(end) - 1);
}

String& String::Remove(uint64 start, uint64 end) {
	ASSERT(start <= end);
	ASSERT(end < length);

	char* tmp = str;

	uint64 remLen = end - start + 1;
	uint64 newLen = length - remLen;

	str = new char[newLen + 1];

	memcpy(str, tmp, start);
	memcpy(str + start, tmp + end + 1, length - (start + remLen) + 1);

	delete[] tmp;
	length = newLen;

	return *this;
}

uint64 String::Count(const String& string, uint64 offset, uint64 end) const {
	return Count(string.str, offset, end);
}

uint64 String::Count(const char* const string, uint64 offset, uint64 end) const {
	ASSERT(string != nullptr);

	uint64 index = offset - 1;
	uint64 count = 0;

	while ((index = Find(string, index + 1)) != ~0) {
		if (index > end) break;
		count++;
	}

	return count;
}

uint64 String::Find(const String& string, uint64 offset) const {
	return Find(string.str, offset);
}

uint64 String::Find(const char* const string, uint64 offset) const {
	ASSERT(string != nullptr);
	uint64 len = strlen(string);

	for (int64 i = offset; i < int64(length - (len - 1)); i++) {
		bool match = true;
		for (uint64 j = 0; j < len; j++) {
			if (str[i + j] != string[j]) {
				match = false;
				break;
			}
		}

		if (match) {
			return i;
		}
	}

	return ~0;
}

uint64 String::Find(const char character, uint64 offset) const {
	for (uint64 i = offset; i < length; i++) {
		if (str[i] == character) return i;
	}

	return ~0;
}

uint64 String::FindReversed(const String& string, uint64 offset) const {
	return FindReversed(string.str, offset);
}

uint64 String::FindReversed(const char* const string, uint64 offset) const {
	ASSERT(string != nullptr);
	uint64 len = strlen(string);

	offset = CLAMP(offset, len - 1, length - len);

	if (offset == 0) {
		offset = length - len;
	}

	for (uint64 i = offset; i >= 0; i--) {
		bool match = true;
		for (uint64 j = 0; j < len; j++) {
			if (str[i + j] != string[j]) {
				match = false;
				break;
			}
		}

		if (match) {
			return i;
		}
	}

	return ~0;
}

uint64 String::FindReversed(const char character, uint64 offset) const {
	offset = CLAMP(offset, 0, length - 1);

	if (offset == 0) {
		offset = length - 1;
	}

	for (uint64 i = offset; i >= 0; i--) {
		if (str[i] == character) return i;
	}

	return ~0;
}

bool String::StartsWith(const String& string) const {
	return StartsWith(string.str);
}

bool String::StartsWith(const char* const string) const {
	ASSERT(string != nullptr);
	uint64 len = strlen(string);

	if (len > length) return false;

	for (uint64 i = 0; i < len; i++) {
		if (str[i] != string[i]) return false;
	}

	return true;
}

bool String::EndsWith(const String& string) const {
	return EndsWith(string.str);
}

bool String::EndsWith(const char* const string) const {
	ASSERT(string != nullptr);
	uint64 len = strlen(string);

	if (len > length) return false;

	uint64 offset = length - len;

	for (uint64 i = 0; i < len; i++) {
		if (str[offset + i] != string[i]) return false;
	}

	return true;
}

String String::SubString(uint64 start, uint64 end) const {
	ASSERT(start != ~0 && end != ~0);
	ASSERT(end >= start);

	uint64 len = end - start + 1;

	ASSERT(start + len <= length);

	return String(str + start, len);
}

String String::SubString(const String& start, const String& end) const {
	return SubString(start.str, end.str);
}

String String::SubString(const char* const start, const char* const end) const {
	ASSERT(start != nullptr && end != nullptr);
	return SubString(Find(start), Find(end));
}

List<String> String::Split(const String& delimiters, bool includeEmtyLines) const {
	return Split(delimiters.str);
}

void String::Insert(uint64 start, uint64 end, const String& string) {
	Insert(start, end, string.str);
}

void String::Insert(uint64 start, uint64 end, const char* const string) {
	ASSERT(start <= end);
	ASSERT(string != nullptr);

	uint64 strLen = strlen(string);

	Remove(start, end);

	char* tmp = str;
	str = new char[length + strLen + 1];

	memcpy(str, tmp, start);
	memcpy(str + start, string, strLen);
	memcpy(str + start + strLen, tmp + start, length - start + 1);

	length = length + strLen;

	delete[] tmp;
}

List<String> String::Split(const char* const delimiters, bool includeEmtyLines) const {
	List<String> list;

	uint64 numDelimiters = strlen(delimiters);

	uint64 lastIndex = 0;

	for (uint64 i = 0; i < length; i++) {
		for (uint64 j = 0; j < numDelimiters; j++) {
			if (str[i] == delimiters[j]) {
				if (lastIndex == i - 1 || lastIndex == i) {
					if (includeEmtyLines) {
						list.Add(String(""));
						lastIndex++;
					}
					else {
						lastIndex++;
					}
				}
				else {
					list.Add(SubString(lastIndex, i - 1));
					lastIndex = i + 1;
				}
				break;
			}
		}
	}

	if (lastIndex < length) {
		list.Add(SubString(lastIndex, length - 1));
	}

	return list;
}

char& String::operator[](uint64 index) {
	ASSERT(index < length);
	return str[index];
}

char String::operator[](uint64 index) const {
	ASSERT(index < length);
	return str[index];
}

String String::operator+(const String& string) const {
	return operator+(string.str);
}

String String::operator+(const char* const string) const {
	String tmp(this);

	tmp.Append(string);

	return tmp;
}

String& String::operator+=(const String& string) {
	return Append(string);
}

String& String::operator+=(const char* const string) {
	return Append(string);
}

bool String::operator==(const String& string) const {
	return operator==(string.str);
}

bool String::operator==(const char* const string) const {
	ASSERT(string != nullptr);
	uint64 len = strlen(string);

	if (len != length) return false;

	for (uint64 i = 0; i < len; i++) {
		if (str[i] != string[i]) return false;
	}

	return true;
}