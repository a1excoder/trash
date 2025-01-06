#ifndef MEMORY_H
#define MEMORY_H

#include <Windows.h>
#include <assert.h>

void* heap_alloc(size_t bytes) {
	assert(bytes >= 1);
	return HeapAlloc(GetProcessHeap(), 0, bytes);
}

BOOL heap_free(void* lp_heap) {
	assert(lp_heap != NULL);
	return HeapFree(GetProcessHeap(), 0, lp_heap);
}

#endif MEMORY_H