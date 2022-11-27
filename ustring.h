#ifndef H_USTRING
#define H_USTRING

#include <stdint.h>

typedef struct {
	int64_t   length;
	int64_t   capacity;
	uint32_t* data;
} ustring;

ustring ustring_new_utf8(const char* s);
ustring ustring_new(uint64_t allocate);
ustring ustring_copy(ustring s);
void    ustring_free(ustring* s);
ustring ustring_substring(ustring s, int64_t start_index, int64_t end_index);

/* Appending to a string does not allocate a new one, instead */
/* it appends to the passed one. */
void ustring_append(ustring* s, ustring appended);
void ustring_append_cstr(ustring* s, const char* appended);
void ustring_append_unicode(ustring* s, uint32_t unicode);
void ustring_insert(ustring* s, ustring inserted, int32_t index);
void ustring_insert_unicode(ustring* s, uint32_t unicode, int32_t index);
void ustring_remove(ustring* s, int32_t n, int32_t index);

/* Comparisons */
bool ustring_equal(ustring s1, ustring s2);
bool ustring_equal_str_cstr(ustring s1, const char* s2);
bool string_equal(const char* s1, const char* s2);
bool string_equal_len(const char* s1, int64_t s1_length, const char* s2, int64_t s2_length);

uint32_t ustring_get_unicode_from_utf8(uint8_t* text, uint32_t* advance);
/* ustring_to_utf8 uses calloc to allocate new string, caller must free this memory after use */
char* ustring_to_utf8(ustring s, int32_t* out_size_bytes);


#if defined(USTRING_IMPLEMENT)

#define USTRING_TRUE 1
#define USTRING_FALSE 0

#define USTRING_MIN(A, B) ((A < B) ?(A) : (B))

#include <string.h>
#include <stdlib.h>

ustring ustring_new_utf8(const char* s) {
	ustring result = ustring_new(strlen(s) * sizeof(uint32_t));

	while (*s) {
		uint32_t advance = 0;
		ustring_append_unicode(&result, ustring_get_unicode_from_utf8((uint8_t*)s, &advance));
		s += advance;
	}
	return result;
}

ustring ustring_new(uint64_t allocate) {
	ustring result;

	if (allocate == 0) allocate = 1;

	result.length = 0;
	result.capacity = allocate * sizeof(uint32_t);
	result.data = (uint32_t*)malloc(allocate * sizeof(uint32_t));

	return result;
}

ustring ustring_copy(ustring s) {
	ustring result;
	result.capacity = s.length * sizeof(uint32_t);
	result.length = s.length;
	result.data = (uint32_t*)malloc((size_t)result.capacity);
	memcpy(result.data, s.data, (size_t)(s.length * sizeof(uint32_t)));
	return result;
}

ustring ustring_substring(ustring s, int64_t start_index, int64_t end_index) {
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

/* Appending to a string does not allocate a new one, instead
   it appends to the passed one. */
void ustring_append(ustring* s, ustring appended) {
	if (s->capacity < ((s->length + appended.length) * (int64_t)sizeof(uint32_t))) {
		/* realloc to support new string */
		s->data = (uint32_t*)realloc(s->data, (s->length + appended.length) * sizeof(uint32_t));
		s->capacity = (s->length + appended.length) * sizeof(uint32_t);
	}
	memcpy(s->data + s->length, appended.data, appended.length * sizeof(uint32_t));
	s->length += appended.length;
}

void ustring_append_cstr(ustring* s, const char* appended) {
	ustring temp = ustring_new_utf8(appended);
	ustring_append(s, temp);
	ustring_free(&temp);
}

void ustring_append_unicode(ustring* s, uint32_t unicode) {
	ustring a;
	a.capacity = 0;
	a.length = 1;
	a.data = (uint32_t*)&unicode;
	ustring_append(s, a);
}

/* Comparisons */
bool ustring_equal(ustring  s1, ustring s2) {
	if (s1.length != s2.length) return USTRING_FALSE;
	for (int64_t i = 0; i < s1.length; ++i) {
		if (s1.data[i] != s2.data[i]) return USTRING_FALSE;
	}
	return USTRING_TRUE;
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

bool string_equal_len(const char* s1, int64_t s1_length, const char* s2, int64_t s2_length) {
	if (s1_length != s2_length) {
		return USTRING_FALSE;
	}
	return string_equal(s1, s2);
}

uint32_t ustring_get_unicode_from_utf8(uint8_t* text, uint32_t* advance) {
	uint32_t result = 0;
	if (text[0] & 128) {
		/* 1xxx xxxx */
		if (text[0] >= 0xF0) {
			/* 4 bytes */
			*advance = 4;
			result = ((text[0] & 0x07) << 18) | ((text[1] & 0x3F) << 12) | ((text[2] & 0x3F) << 6) | (text[3] & 0x3F);
		} else if (text[0] >= 0xE0) {
			/* 3 bytes */
			*advance = 3;
			result = ((text[0] & 0x0F) << 12) | ((text[1] & 0x3F) << 6) | (text[2] & 0x3F);
		} else if (text[0] >= 0xC0) {
			/* 2 bytes */
			*advance = 2;
			result = ((text[0] & 0x1F) << 6) | (text[1] & 0x3F);
		} else {
			/* Continuation byte */
			*advance = 1;
			result = text[0];
		}
	} else {
		*advance = 1;
		result = (uint32_t)text[0];
	}
	return result;
}

void ustring_insert(ustring* s, ustring inserted, int32_t index) {
	if (s->length == index) {
		ustring_append(s, inserted);
	} else if (s->length > index) {
		ustring c = *s;
		c.data = c.data + index;
		c.length -= index;
		c.capacity -= index * sizeof(uint32_t);
		ustring temp = ustring_copy(c);

		s->length = index;
		ustring_append(s, inserted);
		ustring_append(s, temp);
		ustring_free(&temp);
	}
}

void ustring_insert_unicode(ustring* s, uint32_t unicode, int32_t index) {
	ustring in;
	in.capacity = 0;
	in.length = 1;
	in.data = &unicode;
	ustring_insert(s, in, index);
}

void ustring_remove(ustring* s, int32_t n, int32_t index) {
	n = USTRING_MIN(n, (int32_t)(s->length - index));
	if (n == 0) return;

	memcpy(s->data + index, s->data + index + n, (s->length - index - n) * sizeof(uint32_t));
	s->length -= n;
}

char*
ustring_to_utf8(ustring s, int32_t* out_size_bytes) {
    uint8_t* result = (uint8_t*)calloc(1, s.length * sizeof(uint32_t));
    uint8_t* start = result;
    for(int64_t i = 0; i < s.length; ++i) {
        uint32_t unicode = s.data[i];
        if(unicode <= 0x7f) {
            *result++ = (uint8_t)unicode;
        } else if(unicode >= 0x80 && unicode <= 0x7ff) {
            uint8_t b1 = 0xc0 | (unicode >> 6);
            uint8_t b2 = 0x80 | ((unicode & 0x3f) | 0x30000000);
            *result++ = b1;
            *result++ = b2;
        } else if(unicode >= 0x800 && unicode <= 0xffff) {
            uint8_t b1 = 0xe0 | (unicode >> 12);
            uint8_t b2 = 0x80 | (((unicode >> 6) & 0x3f) | 0x30000000);
            uint8_t b3 = 0x80 | ((unicode & 0x3f) | 0x30000000);
            *result++ = b1;
            *result++ = b2;
            *result++ = b3;
        } else if(unicode >= 0x00010000 && unicode <= 0x001fffff) {
            uint8_t b1 = 0xf0 | (unicode >> 18);
            uint8_t b2 = 0x80 | (((unicode >> 12) & 0x3f) | 0x30000000);
            uint8_t b3 = 0x80 | (((unicode >> 6) & 0x3f) | 0x30000000);
            uint8_t b4 = 0x80 | ((unicode & 0x3f) | 0x30000000);
            *result++ = b1;
            *result++ = b2;
            *result++ = b3;
            *result++ = b4;
        }
    }
    *out_size_bytes = result - start;
    return (char*)start;
}
#endif /* USTRING_IMPLEMENT */
#endif /* H_USTRING */