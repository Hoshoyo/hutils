#pragma once
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;
typedef int bool;

#define true 1
#define false 0

#define MIN(A, B) ((A < B) ?(A) : (B))
#define MAX(A, B) ((A > B) ?(A) : (B))

typedef struct {
	s64 length;
	s64 capacity;
	u32* data;
} ustring;

ustring ustring_new_utf8(const char* s);
ustring ustring_new(u64 allocate);
ustring ustring_copy(ustring s);
void    ustring_free(ustring* s);
ustring ustring_substring(ustring s, s64 start_index, s64 end_index);

// Appending to a string does not allocate a new one, instead
// it appends to the passed one.
void ustring_append(ustring* s, ustring appended);
void ustring_append_cstr(ustring* s, const char* appended);
void ustring_append_unicode(ustring* s, u32 unicode);
//void ustring_append_utf8(const char* appended);
void ustring_insert(ustring* s, ustring inserted, s32 index);
void ustring_insert_unicode(ustring* s, u32 unicode, s32 index);
void ustring_remove(ustring* s, s32 n, s32 index);

// comparisons
bool ustring_equal(ustring s1, ustring s2);
bool ustring_equal_str_cstr(ustring s1, const char* s2);
bool string_equal(const char* s1, const char* s2);
bool string_equal_len(const char* s1, s64 s1_length, const char* s2, s64 s2_length);

u32   ustring_get_unicode(u8* text, u32* advance);
// ustring_to_utf8 uses calloc to allocate new string, caller must free this memory after use
char* ustring_to_utf8(ustring s, s32* out_size_bytes);


#if defined(USTRING_IMPLEMENT)

#include <string.h>
#include <stdlib.h>

ustring ustring_new_utf8(const char* s) {
	ustring result = ustring_new(strlen(s) * sizeof(u32));

	while (*s) {
		u32 advance = 0;
		ustring_append_unicode(&result, ustring_get_unicode((u8*)s, &advance));
		s += advance;
	}
	return result;
}

ustring ustring_new(u64 allocate) {
	ustring result;

	if (allocate == 0) allocate = 1;

	result.length = 0;
	result.capacity = allocate * sizeof(u32);
	result.data = (u32*)malloc(allocate * sizeof(u32));

	return result;
}

ustring ustring_copy(ustring s) {
	ustring result;
	result.capacity = s.length * sizeof(u32);
	result.length = s.length;
	result.data = (u32*)malloc((size_t)result.capacity);
	memcpy(result.data, s.data, (size_t)(s.length * sizeof(u32)));
	return result;
}

ustring ustring_substring(ustring s, s64 start_index, s64 end_index) {
	ustring result = { 0 };
	result.capacity = 0;
	result.length = end_index - start_index;
	result.data = s.data + start_index;
	return result;
}

void ustring_free(ustring* s) {
	s->capacity = 0;
	s->length = 0;
	free(s->data);
}

// Appending to a string does not allocate a new one, instead
// it appends to the passed one.
void ustring_append(ustring* s, ustring appended) {
	if (s->capacity < ((s->length + appended.length) * (s64)sizeof(u32))) {
		// realloc to support new string
		s->data = (u32*)realloc(s->data, (s->length + appended.length) * sizeof(u32));
		s->capacity = (s->length + appended.length) * sizeof(u32);
	}
	memcpy(s->data + s->length, appended.data, appended.length * sizeof(u32));
	s->length += appended.length;
}

void ustring_append_cstr(ustring* s, const char* appended) {
	ustring temp = ustring_new_utf8(appended);
	ustring_append(s, temp);
	ustring_free(&temp);
}

void ustring_append_unicode(ustring* s, u32 unicode) {
	ustring a;
	a.capacity = 0;
	a.length = 1;
	a.data = (u32*)&unicode;
	ustring_append(s, a);
}

// comparisons
bool ustring_equal(ustring  s1, ustring s2) {
	if (s1.length != s2.length) return false;
	for (s64 i = 0; i < s1.length; ++i) {
		if (s1.data[i] != s2.data[i]) return false;
	}
	return true;
}

bool ustring_equal_str_cstr(ustring s1, const char* s2) {
	ustring temp = ustring_new_utf8(s2);
	bool r = ustring_equal(s1, temp);
	ustring_free(&temp);
	return r;
}

bool string_equal(const char* s1, const char* s2) {
	return (strcmp(s1, s2) == 0);
}

bool string_equal_len(const char* s1, s64 s1_length, const char* s2, s64 s2_length) {
	if (s1_length != s2_length) {
		return false;
	}
	return string_equal(s1, s2);
}

u32 ustring_get_unicode(u8* text, u32* advance) {
	u32 result = 0;
	if (text[0] & 128) {
		// 1xxx xxxx
		if (text[0] >= 0xF0) {
			// 4 bytes
			*advance = 4;
			result = ((text[0] & 0x07) << 18) | ((text[1] & 0x3F) << 12) | ((text[2] & 0x3F) << 6) | (text[3] & 0x3F);
		} else if (text[0] >= 0xE0) {
			// 3 bytes
			*advance = 3;
			result = ((text[0] & 0x0F) << 12) | ((text[1] & 0x3F) << 6) | (text[2] & 0x3F);
		} else if (text[0] >= 0xC0) {
			// 2 bytes
			*advance = 2;
			result = ((text[0] & 0x1F) << 6) | (text[1] & 0x3F);
		} else {
			// continuation byte
			*advance = 1;
			result = text[0];
		}
	} else {
		*advance = 1;
		result = (u32)text[0];
	}
	return result;
}

void ustring_insert(ustring* s, ustring inserted, s32 index) {
	if (s->length == index) {
		ustring_append(s, inserted);
	} else if (s->length > index) {
		ustring c = *s;
		c.data = c.data + index;
		c.length -= index;
		c.capacity -= index * sizeof(u32);
		ustring temp = ustring_copy(c);

		s->length = index;
		ustring_append(s, inserted);
		ustring_append(s, temp);
		ustring_free(&temp);
	} else {
		// TODO(psv): assert or ignore this
	}
}

void ustring_insert_unicode(ustring* s, u32 unicode, s32 index) {
	ustring in;
	in.capacity = 0;
	in.length = 1;
	in.data = &unicode;
	ustring_insert(s, in, index);
}

void ustring_remove(ustring* s, s32 n, s32 index) {
	n = MIN(n, (s32)(s->length - index));
	if (n == 0) return;

	memcpy(s->data + index, s->data + index + n, (s->length - index - n) * sizeof(u32));
	s->length -= n;
}

char*
ustring_to_utf8(ustring s, s32* out_size_bytes) {
    u8* result = (u8*)calloc(1, s.length * sizeof(u32));
    u8* start = result;
    for(s64 i = 0; i < s.length; ++i) {
        u32 unicode = s.data[i];
        if(unicode <= 0x7f) {
            *result++ = (u8)unicode;
        } else if(unicode >= 0x80 && unicode <= 0x7ff) {
            u8 b1 = 0xc0 | (unicode >> 6);
            u8 b2 = 0x80 | ((unicode & 0x3f) | 0x30000000);
            *result++ = b1;
            *result++ = b2;
        } else if(unicode >= 0x800 && unicode <= 0xffff) {
            u8 b1 = 0xe0 | (unicode >> 12);
            u8 b2 = 0x80 | (((unicode >> 6) & 0x3f) | 0x30000000);
            u8 b3 = 0x80 | ((unicode & 0x3f) | 0x30000000);
            *result++ = b1;
            *result++ = b2;
            *result++ = b3;
        } else if(unicode >= 0x00010000 && unicode <= 0x001fffff) {
            u8 b1 = 0xf0 | (unicode >> 18);
            u8 b2 = 0x80 | (((unicode >> 12) & 0x3f) | 0x30000000);
            u8 b3 = 0x80 | (((unicode >> 6) & 0x3f) | 0x30000000);
            u8 b4 = 0x80 | ((unicode & 0x3f) | 0x30000000);
            *result++ = b1;
            *result++ = b2;
            *result++ = b3;
            *result++ = b4;
        }
    }
    *out_size_bytes = result - start;
    return (char*)start;
}

#endif