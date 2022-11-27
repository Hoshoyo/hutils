#ifndef HO_OS
#define HO_OS

#if defined(_WIN32) || defined(_WIN64)
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#error "OS not supported"
#endif

#include <stdint.h>
#include <stdlib.h>

/* Definitions */
#define OS_OK 0
#define OS_ERROR (-1)

typedef enum {
	OS_FILE_ATTRIBUTE_READONLY = 0x1,
	OS_FILE_ATTRIBUTE_HIDDEN = 0x2,
	OS_FILE_ATTRIBUTE_SYSTEM = 0x4,
	OS_FILE_ATTRIBUTE_DIRECTORY = 0x10,
	OS_FILE_ATTRIBUTE_DEVICE = 0x40,
	OS_FILE_ATTRIBUTE_NORMAL = 0x80,
	OS_FILE_ATTRIBUTE_TEMPORARY = 0x100,
} OS_FileAttributes;

/* Types */
typedef struct {
	double start;
	double elapsed;
} OS_Timer;

typedef struct {
	uint16_t year;
	uint16_t month;
	uint16_t day_week;
	uint16_t day;
	uint16_t hour;
	uint16_t minute;
	uint16_t second;
	uint16_t milliseconds;
} OS_Datetime;

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

typedef struct {
	uint64_t creation_time;
	uint64_t access_time;
	uint64_t write_time;
	char     filename[MAX_PATH];
	OS_FileAttributes attributes;
} OS_FileData;

/* Timing */
void     os_datetime(OS_Datetime* systime);
void     os_time_init();
double   os_time_ms();
double   os_time_us();
double   os_time_ns();
double   os_time_sec();
OS_Timer os_timer_new(int start);
void     os_timer_reset(OS_Timer* timer);
void     os_timer_stop(OS_Timer* timer);
void     os_timer_start(OS_Timer* timer);
double   os_timer_elapsed_ms(OS_Timer* timer);
double   os_timer_elapsed_us(OS_Timer* timer);
double   os_timer_elapsed_ns(OS_Timer* timer);

/* Files */
int               os_file_exists(const char* in_filename);
int64_t           os_file_size(const char* in_filename);
uint32_t          os_file_write(const char* filename, void* mem, uint32_t size);
uint32_t          os_file_append(const char* filename, void* mem, uint32_t size);
void*             os_file_read(const char* in_filename, int64_t* out_size);
uint64_t          os_file_read_to_mem(const char* in_filename, int64_t in_size, void* in_memory);
const char*       os_file_name_from_path(const char* path);
const char*       os_file_extension(const char* filename);
OS_FileAttributes os_file_attributes(const char* filename);
char*             os_file_fullpath(const char* path);
OS_FileData*      os_file_list_dir(const char* path);
int               os_file_delete(const char* path);

#if defined(_WIN32) || defined(_WIN64)
#if defined(HO_OS_IMPLEMENT)

void* __cdecl memcpy(void* dst, void const* src, size_t size);

/* Timing */
static int64_t perf_frequency;
void os_time_init()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&perf_frequency);
}
double os_time_ns()
{
	int64_t li;
	QueryPerformanceCounter((LARGE_INTEGER*)&li);
	return ((double)(li) / perf_frequency) * 1000000000.0;
}
double os_time_us()
{
	int64_t li;
	QueryPerformanceCounter((LARGE_INTEGER*)&li);
	return ((double)(li) / perf_frequency) * 1000000.0;
}
double os_time_ms()
{
	int64_t li;
	QueryPerformanceCounter((LARGE_INTEGER*)&li);
	return ((double)(li) / perf_frequency) * 1000.0;
}
double os_time_sec()
{
	int64_t li;
	QueryPerformanceCounter((LARGE_INTEGER*)&li);
	return ((double)(li) / perf_frequency);
}

void os_datetime(OS_Datetime* systime)
{
	GetSystemTime((SYSTEMTIME*)&systime);
}

OS_Timer os_timer_new(int start)
{
	return (OS_Timer)
	{
		.start = (start) ? os_time_ns() : 0,
		.elapsed = 0
	};
}
void os_timer_reset(OS_Timer* timer)
{
	timer->start = 0;
	timer->elapsed = 0;
}

void os_timer_stop(OS_Timer* timer)
{
	timer->elapsed = os_time_ns() - timer->start;
	timer->start = 0;
}
void os_timer_start(OS_Timer* timer)
{
	timer->start = os_time_ns();
}
double os_timer_elapsed_ms(OS_Timer* timer)
{
	return (timer->start) ? (os_time_ns() - timer->start + timer->elapsed) / 1000000.0 : timer->elapsed / 1000000.0;
}
double os_timer_elapsed_us(OS_Timer* timer)
{
	return (timer->start) ? (os_time_ns() - timer->start + timer->elapsed) / 1000.0 : timer->elapsed / 1000.0;
}
double os_timer_elapsed_ns(OS_Timer* timer)
{
	return (timer->start) ? (os_time_ns() - timer->start + timer->elapsed) : timer->elapsed;
}

/* Files */
int os_file_exists(const char* in_filename)
{
	return (GetFileAttributesA(in_filename) != INVALID_FILE_ATTRIBUTES);
}
int64_t os_file_size(const char* in_filename)
{
	uint64_t filesize = 0;
	void* handle = CreateFileA(in_filename, FILE_READ_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN, 0);
	if (handle != INVALID_HANDLE_VALUE)
	{
		GetFileSizeEx(handle, (LARGE_INTEGER*)&filesize);
		CloseHandle(handle);
		return (int64_t)filesize;
	}
	else
	{
		return -1;
	}
}

const char* os_file_name_from_path(const char* path)
{
	int sep_index = 0;
	const char* at = path;
	while (*at)
	{
		char c = *at;
		if (c == '\\' || c == '/')
			sep_index = (int)(at - path + 1);
		at++;
	}
	return path + sep_index;
}

const char* os_file_extension(const char* filename)
{
	int dot_index = 0;
	const char* at = filename;
	while (*at)
	{
		char c = *at;
		if (c == '.')
			dot_index = (int)(at - filename + 1);
		at++;
	}
	return filename + dot_index;
}

OS_FileAttributes os_file_attributes(const char* filename)
{
	return GetFileAttributesA(filename);
}

char* os_file_fullpath(const char* path)
{
	uint32_t len = 512;
	char* buffer = calloc(1, len);
	if (buffer)
	{
		uint32_t pathlen = GetFullPathNameA(path, len, buffer, 0);
		if (pathlen != len)
		{
			char* rbuffer = realloc(buffer, pathlen);
			if (rbuffer)
				buffer = rbuffer;
			else
			{
				free(buffer);
				buffer = 0;
			}
		}
		GetFullPathNameA(path, len, buffer, 0);
	}
	return buffer;
}

int os_file_delete(const char* path)
{
	if (DeleteFileA(path)) 
		return OS_OK;
	return OS_ERROR;
}

OS_FileData* os_file_list_dir(const char* path)
{
	int capacity = 4;
	OS_FileData* data_array = calloc(capacity, sizeof(OS_FileData));
	WIN32_FIND_DATAA find_data;
	if (data_array)
	{
		void* search_handle = FindFirstFileA(path, &find_data);
		int index = 0;
		do {
			if (index >= capacity)
			{
				capacity *= 2;
				void* new_array = realloc(data_array, capacity * sizeof(OS_FileData));
				if (new_array)
					data_array = new_array;
				else
					break;
			}
			data_array[index].access_time = *(uint64_t*)&find_data.ftLastAccessTime;
			data_array[index].creation_time = *(uint64_t*)&find_data.ftCreationTime;
			data_array[index].write_time = *(uint64_t*)&find_data.ftLastWriteTime;
			data_array[index].attributes = (OS_FileAttributes)find_data.dwFileAttributes;
			memcpy(data_array[index].filename, find_data.cFileName, MAX_PATH);

			++index;
		} while (FindNextFileA(search_handle, &find_data));
		FindClose(search_handle);
	}
	return data_array;
}

uint32_t os_file_write(const char* filename, void* mem, uint32_t size)
{
	void* fhandle = CreateFileA(filename, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (fhandle != INVALID_HANDLE_VALUE)
	{
		uint32_t written = 0;
		if (WriteFile(fhandle, mem, size, &written, 0))
		{
			CloseHandle(fhandle);
			return written;
		}
		CloseHandle(fhandle);
	}
	return 0;
}

uint32_t os_file_append(const char* filename, void* mem, uint32_t size)
{
	void* fhandle = CreateFileA(filename, GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (fhandle != INVALID_HANDLE_VALUE)
	{
		LARGE_INTEGER zero = { 0 };
		if (SetFilePointerEx(fhandle, zero, 0, FILE_END))
		{
			uint32_t written = 0;
			if (WriteFile(fhandle, mem, size, &written, 0))
			{
				CloseHandle(fhandle);
				return written;
			}
		}
		CloseHandle(fhandle);
	}
	return 0;
}

uint64_t os_file_read_to_mem(const char* in_filename, int64_t in_size, void* in_memory)
{
	void* fhandle = CreateFileA(in_filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (fhandle != INVALID_HANDLE_VALUE)
	{
		uint64_t file_size = in_size;
		const max_chunk = 0xffffffff;
		if (in_memory)
		{
			if (file_size > max_chunk)
			{
				uint64_t total_read = 0;
				uint64_t to_read = file_size;
				do {
					uint32_t chunk_size = (to_read > max_chunk) ? max_chunk : (uint32_t)to_read;
					uint32_t bytes_read;

					if (ReadFile(fhandle, in_memory, chunk_size, &bytes_read, 0))
					{
						to_read -= bytes_read;
						total_read += bytes_read;
					}
					else
						break;
				} while (to_read > 0);
				CloseHandle(fhandle);
				return total_read;
			}
			else
			{
				uint32_t bytes_read;
				if (ReadFile(fhandle, in_memory, (uint32_t)file_size, &bytes_read, 0))
				{
					CloseHandle(fhandle);
					return bytes_read;
				}
				CloseHandle(fhandle);
			}
		}
	}
	return 0;
}

void* os_file_read(const char* in_filename, int64_t* out_size)
{
	void* fhandle = CreateFileA(in_filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (fhandle != INVALID_HANDLE_VALUE)
	{
		uint64_t file_size = 0;
		GetFileSizeEx(fhandle, (LARGE_INTEGER*)&file_size);
		void* mem = calloc(1, file_size);
		const max_chunk = 0xffffffff;
		if (mem)
		{
			if (file_size > max_chunk)
			{
				uint64_t total_read = 0;
				uint64_t to_read = file_size;
				do {
					uint32_t chunk_size = (to_read > max_chunk) ? max_chunk : (uint32_t)to_read;
					uint32_t bytes_read;

					if (ReadFile(fhandle, mem, chunk_size, &bytes_read, 0))
					{
						to_read -= bytes_read;
						total_read += bytes_read;
					}
					else
						break;
				} while (to_read > 0);
				if (out_size) *out_size = (int64_t)total_read;
				CloseHandle(fhandle);
				return mem;
			}
			else
			{
				uint32_t bytes_read;
				if (ReadFile(fhandle, mem, (uint32_t)file_size, &bytes_read, 0))
				{
					if (out_size) *out_size = bytes_read;
					CloseHandle(fhandle);
					return mem;
				}
				CloseHandle(fhandle);
			}
		}
	}
	return 0;
}
#endif

#else
#error "OS not supported"
#endif

#endif // HO_OS
