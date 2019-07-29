#pragma once

#include <utility>
#include <core/core.h>

#define FM_PREALLOC_COUNT 128

template<typename T>
class List {
private:
uint64 count;
T* items;

uint64 allocated;

public:
	List() : count(0), items(nullptr), allocated(0) {}

	List(uint64 reserve) : count(0), allocated(reserve) {
		items = new T[reserve];
	}

	List(const List& other) {
		count = other.count;
		allocated = other.allocated;
		items = new T[allocated];

		for (uint64 i = 0; i < count; i++) {
			new (items + i) T(other.items[i]);
		}
	}

	List(const List* other) {
		count = other->count;
		allocated = other->allocated;
		items = new T[allocated];

		for (uint64 i = 0; i < count; i++) {
			new (items + i) T(other->items[i]);
		}
	}

	List(List&& other) {
		count = other.count;
		allocated = other.allocated;
		items = other.items;

		other.count = 0;
		other.allocated = 0;
		other.items = nullptr;
	}

	~List() {
		delete[] items;
	}

	inline List& operator=(const List& other) {
		if (this != &other) {
			delete[] items;
			count = other.count;
			allocated = other.allocated;
			items = new T[allocated];

			for (uint64 i = 0; i < count; i++) {
				new (items + i) T(other.items[i]);
			}
		}

		return *this;
	}

	inline List& operator=(List&& other) {
		if (this != &other) {
			delete[] items;
			count = other.count;
			allocated = other.allocated;
			items = other.items;

			other.count = 0;
			other.allocated = 0;
			other.items = nullptr;
		}

		return *this;
	}

	/*Resizes the list*/
	inline void Resize(uint64 count) {
		if (count > allocated) {
			Reserve(allocated + FM_PREALLOC_COUNT);
		}

		this->count = count;
	}

	/*Resizes the list and fills the new items with the specifed defaults. If the new sizes is large, all new items will have the default values. If the new size is smaller, all items will have the defaults*/
	template<typename ...Args>
	inline void Resize(uint64 count, Args&& ... defaults) {
		if (count > allocated) {
			Reserve(allocated + FM_PREALLOC_COUNT);
		}

		if (count > this->count) {
			for (uint64 i = this->count; i < count; i++) {
				new (items + i) T(defaults...);
			}
		}
		else {
			for (uint64 i = 0; i < count; i++) {
				new (items + i) T(defaults...);
			}
		}

		this->count = count;
	}

	/*Reserves space*/
	inline void Reserve(uint64 reserve) {
		if (reserve <= allocated) return;

		T* tmp = items;

		items = new T[reserve];

		for (uint64 i = 0; i < count; i++) {
			new (items + i) T(std::move(tmp[i]));
		}

		allocated = reserve;

		delete[] tmp;
	}

	/*Addes item at the end of the list*/
	inline void Add(const T& item) {
		if (count >= allocated) {
			Reserve(allocated + FM_PREALLOC_COUNT);
		}

		new (items + count++) T(item);
	}

	/*Addes item at the end of the list*/
	inline void Add(T&& item) {
		if (count >= allocated) {
			Reserve(allocated + FM_PREALLOC_COUNT);
		}

		new (items + count++) T(std::move(item));
	}

	//Appends another list
	inline void Add(const List<T>& other) {
		uint64 totalCount = count + other.count;

		if (totalCount > allocated) {
			Reserve(totalCount + FM_PREALLOC_COUNT);
		}

		for (uint64 i = 0; i < other.count; i++) {
			new (items + count + i) T(other[i]);
		}

		count = totalCount;
	}

	//Appends another list
	inline void Add(List<T>&& other) {
		uint64 totalCount = count + other.count;

		if (totalCount > allocated) {
			Reserve(totalCount + FM_PREALLOC_COUNT);
		}

		for (uint64 i = 0; i < other.count; i++) {
			new (items + count + i) T(std::move(other[i]));
		}

		count = totalCount;
	}

	/*Replaces item*/
	inline void ReplaceAt(uint64 index, const T& item) {
		ASSERT(index < count);

		items[index] = item;
	}

	/*Replaces item*/
	inline void ReplaceAt(uint64 index, T&& item) {
		ASSERT(index < count);

		items[index] = std::move(item);
	}

	/*Constructs an item at the end of the list*/
	template <typename ...Args>
	inline void Emplace(Args&& ... args) {
		if (count >= allocated) {
			Reserve(allocated + FM_PREALLOC_COUNT);
		}

		new (items + count++) T(args...);
	}

	/*Constructs a new item in the specified location*/
	template <typename ...Args>
	inline void EmplaceAt(uint64 index, Args&& ... args) {
		ASSERT(index < count);

		new (&items[index]) T(args...);
	}

	/*Inserts item*/
	template<typename ...Args>
	inline void EmplaceInsert(uint64 index, Args&& ... args) {
		ASSERT(index < count);

		if (count + 1 > allocated) {
			Reserve(allocated + FM_PREALLOC_COUNT);
		}

		uint64 itemsToMove = count++ - index;

		for (uint64 i = 0; i < itemsToMove; i++) {
			uint64 location = count - i - 1;
			new (items + location) T(std::move(items[itemsToMove + index - i - 1]));
		}

		new (items + index) T(args...);
	}

	/*Inserts item*/
	inline void Insert(uint64 index, const T& item) {
		ASSERT(index < count);

		if (count + 1 > allocated) {
			Reserve(allocated + FM_PREALLOC_COUNT);
		}

		uint64 itemsToMove = count++ - index;


		for (uint64 i = 0; i < itemsToMove; i++) {
			uint64 location = count - i - 1;
			new (items + location) T(std::move(items[itemsToMove + index - i - 1]));
		}

		new (items + index) T(item);
	}

	/*Inserts item*/
	inline void Insert(uint64 index, T&& item) {
		ASSERT(index < count);

		if (count + 1 > allocated) {
			Reserve(allocated + FM_PREALLOC_COUNT);
		}

		uint64 itemsToMove = count++ - index;


		for (uint64 i = 0; i < itemsToMove; i++) {
			uint64 location = count - i - 1;
			new (items + location) T(std::move(items[itemsToMove + index - i - 1]));
		}

		new (items + index) T(std::move(item));
	}

	/*Inserts items from another list*/
	inline void InsertList(uint64 index, const List& other) {
		ASSERT(index <= count);

		uint64 totalCount = count + other.count;

		if (totalCount > allocated) {
			Reserve(totalCount + FM_PREALLOC_COUNT);
		}

		uint64 itemsToMove = count - index;

		for (uint64 i = index + itemsToMove; i > index; i--) {
			new (items + i + other.count - 1) T(items[i - 1]);
		}

		for (uint64 i = 0; i < other.count; i++) {
			new (items + index + i) T(other[i]);
		}

		count = totalCount;
	}

	/*Inserts items from another list*/
	inline void InsertList(uint64 index, List&& other) {
		ASSERT(index <= count);

		uint64 totalCount = count + other.count;

		if (totalCount > allocated) {
			Reserve(totalCount + FM_PREALLOC_COUNT);
		}

		uint64 itemsToMove = count - index;

		for (uint64 i = index + itemsToMove; i > index; i--) {
			new (items + i + other.count - 1) T(std::move(items[i - 1]));
		}

		for (uint64 i = 0; i < other.count; i++) {
			new (items + index + i) T(std::move(other.items[i]));
		}

		count = totalCount;

		other.count = 0;
		other.allocated = 0;
	}

	/*Removes item at the specified location*/
	inline T RemoveAt(uint64 index) {
		ASSERT(index < count);

		T tmp(std::move(items[index]));

		memcpy(items + index, items + index + 1, (--count - index) * sizeof(T));

		memset(items + count, 0, sizeof(T));

		return tmp;
	}

	/*Removes the specifed item*/
	inline T Remove(const T& item) {
		return RemoveAt(Find(item));
	}

	/*Removes a range of items*/
	inline void Remove(uint64 start, uint64 end) {
		ASSERT(start >= 0 && end < count);
		uint64 count = end - start + 1;

		for (uint64 i = 0; i < count; i++) {
			RemoveAt(start);
		}
	}

	/*Finds the item*/
	inline uint64 Find(const T& item, uint64 offset = 0) const {
		ASSERT(offset <= count);
		for (uint64 i = offset; i < count; i++) {
			if (items[i] == item) return i;
		}

		return (uint64)~0;
	}

	template<typename K>
	inline uint64 Find(const K& item, bool(*CmpFunc)(const T&, const K&), uint64 offset = 0) const {
		ASSERT(offset <= count);
		for (uint64 i = offset; i < count; i++) {
			if (CmpFunc(items[i], item)) return i;
		}

		return (uint64)~0;
	}

	inline T& operator[](uint64 index) {
		ASSERT(index < count);
		return items[index];
	}

	inline const T& operator[](uint64 index) const {
		ASSERT(index < count);
		return items[index];
	}

	inline T* GetData() { return items; }
	inline const T* GetData() const { return items; }

	inline uint64 GetCount() const { return count; }
	inline uint64 GetSize() const { return count * sizeof(T); }

	inline uint64 GetAllocatedCount() const { return allocated; }
	inline uint64 GetAllocatedSize() const { return allocated * sizeof(T); }
	};
