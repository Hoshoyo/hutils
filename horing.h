/*
int main()
{
	char* a = (char*)horing_new(4096, 3);

	*a = 12;

	printf("a[   0] = %d\n", a[0]);
	printf("a[4096] = %d\n", a[4096]);
	printf("a[8192] = %d\n", a[4096*2]);

	horing_free(a, 4096, 3);

	return (uint32_t)*a;
}
*/

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <stdint.h>

void* horing_new(uint32_t size, uint32_t repeat_count)
{
	if (size % 4096 != 0)
		return 0;

	void* result = 0;

	typedef PVOID WINAPI VirtualAlloc2_func_t(HANDLE Process,
		PVOID BaseAddress,
		SIZE_T Size,
		ULONG AllocationType,
		ULONG PageProtection,
		MEM_EXTENDED_PARAMETER* ExtendedParameters,
		ULONG ParameterCount
	);

	typedef PVOID WINAPI MapViewOfFile3_func_t(
		HANDLE FileMapping,
		HANDLE Process,
		PVOID BaseAddress,
		ULONG64 Offset,
		SIZE_T ViewSize,
		ULONG AllocationType,
		ULONG PageProtection,
		MEM_EXTENDED_PARAMETER* ExtendedParameters,
		ULONG ParameterCount
	);

	static VirtualAlloc2_func_t* Dyn_VirtualAlloc2;
	static MapViewOfFile3_func_t* Dyn_MapViewOfFile3;

	if (Dyn_VirtualAlloc2 == 0)
	{
		HMODULE kernel = LoadLibraryA("kernelbase.dll");
		if (kernel)
		{
			Dyn_VirtualAlloc2 = (VirtualAlloc2_func_t*)GetProcAddress(kernel, "VirtualAlloc2");
			Dyn_MapViewOfFile3 = (MapViewOfFile3_func_t*)GetProcAddress(kernel, "MapViewOfFile3");
		}
	}

	if (Dyn_VirtualAlloc2 && Dyn_MapViewOfFile3)
	{
		void* first = Dyn_VirtualAlloc2(0, 0, repeat_count * size, MEM_RESERVE | MEM_RESERVE_PLACEHOLDER, PAGE_NOACCESS, 0, 0);

		if (first)
		{
			HANDLE mapping = CreateFileMappingA(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, size, 0);
			if(mapping != INVALID_HANDLE_VALUE && first)
			{
				bool mapped = true;
				for (uint32_t i = 0; i < repeat_count; ++i)
				{
					VirtualFree((char*)first + i * size, size, MEM_RELEASE | MEM_PRESERVE_PLACEHOLDER);
					if (!Dyn_MapViewOfFile3(mapping, 0, (char*)first + i * size, 0, size, MEM_REPLACE_PLACEHOLDER, PAGE_READWRITE, 0, 0))
					{
						mapped = false;
					}
				}
				if (mapped)
					result = first;
				CloseHandle(mapping);
			}
		}
	}

	return result;
}

void horing_free(void* base, uint32_t size, uint32_t repeat_count)
{
	for (uint32_t i = 0; i < repeat_count; ++i)
	{
		UnmapViewOfFile((char*)base + i * size);
	}
}

#else
#error not yet implemented
#endif