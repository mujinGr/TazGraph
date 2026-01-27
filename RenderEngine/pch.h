// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H


#include <iostream>
#include <string>
#include <ctime>

#include <unordered_map>
#include <memory>
#include <map>
#include <vector>
#include <fstream>

// add headers that you want to pre-compile here
//#include "framework.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "GL/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtx/rotate_vector.hpp>
#include "glm/gtc/type_ptr.hpp"

#include <imgui.h>

#include <SDL2/SDL.h>

#include <tracy/public/tracy/Tracy.hpp>

//namespace TracyAligned {
//
//#if defined(_WIN32) || defined(__CYGWIN__)
//#include <malloc.h>
//	inline void* aligned_alloc(size_t alignment, size_t size) {
//		return _aligned_malloc(size, alignment);
//	}
//	inline void aligned_free(void* ptr) {
//		_aligned_free(ptr);
//	}
//#elif defined(__linux__) || defined(__APPLE__)
//#include <stdlib.h>
//	inline void* aligned_alloc(size_t alignment, size_t size) {
//		void* ptr = nullptr;
//		if (posix_memalign(&ptr, alignment, size) != 0) {
//			return nullptr;
//		}
//		return ptr;
//	}
//	inline void aligned_free(void* ptr) {
//		free(ptr);
//	}
//#else
//	inline void* aligned_alloc(size_t alignment, size_t size) {
//		size_t extra = alignment - 1 + sizeof(void*);
//		void* original = malloc(size + extra);
//		if (!original) return nullptr;
//
//		void* ptr = reinterpret_cast<void*>(
//			(reinterpret_cast<uintptr_t>(original) + extra) & ~(alignment - 1));
//		reinterpret_cast<void**>(ptr)[-1] = original;
//		return ptr;
//	}
//
//	inline void aligned_free(void* ptr) {
//		if (!ptr) return;
//		void* original = reinterpret_cast<void**>(ptr)[-1];
//		free(original);
//	}
//#endif
//
//	// Tracy-profiled aligned allocation
//	inline void* tracy_aligned_alloc(size_t size, size_t alignment) {
//		void* ptr = aligned_alloc(alignment, size);
//#ifdef TRACY_ENABLE
//		if (ptr) {
//			TracyAlloc(ptr, size);
//		}
//#endif
//		return ptr;
//	}
//
//	inline void tracy_aligned_free(void* ptr) {
//		if (!ptr) return;
//#ifdef TRACY_ENABLE
//		TracyFree(ptr);
//#endif
//		aligned_free(ptr);
//	}
//}
//
//// Overload global new/delete with Tracy support
//inline void* operator new(size_t size, std::align_val_t al) {
//	return TracyAligned::tracy_aligned_alloc(size, static_cast<size_t>(al));
//}
//
//inline void* operator new[](size_t size, std::align_val_t al) {
//	return operator new(size, al);
//}
//
//inline void* operator new(size_t size) {
//#ifdef TRACY_ENABLE
//	void* ptr = std::malloc(size);
//	TracyAlloc(ptr, size);
//	return ptr;
//#else
//	return std::malloc(size);
//#endif
//}
//
//inline void operator delete(void* ptr, std::align_val_t al) noexcept {
//	TracyAligned::tracy_aligned_free(ptr);
//}
//
//inline void operator delete[](void* ptr, std::align_val_t al) noexcept {
//	operator delete(ptr, al);
//}
//
//inline void operator delete(void* ptr) noexcept {
//#ifdef TRACY_ENABLE
//	TracyFree(ptr);
//#endif
//	std::free(ptr);
//}

#endif //PCH_H
