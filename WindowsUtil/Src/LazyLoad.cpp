#include <Process\LazyLoad\LazyLoad.h>
namespace LazyLoad
{
#pragma region ��������

#define INIT_FUNC(result,name,params) FuncType_##name _##name=Def_##name;result WINAPI Def_##name params

	VOID NTAPI _RtlInitUnicodeString(IN OUT PUNICODE_STRING DestinationString, IN PCWSTR SourceString)
	{
		SIZE_T Size;
		CONST SIZE_T MaxSize = (MAXUINT16 & ~1) - sizeof(UNICODE_NULL);

		if (SourceString)
		{
			Size = wcslen(SourceString) * sizeof(WCHAR);
			__analysis_assume(Size <= MaxSize);

			if (Size > MaxSize)
				Size = MaxSize;
			DestinationString->Length = (USHORT)Size;
			DestinationString->MaximumLength = (USHORT)Size + sizeof(UNICODE_NULL);
		}
		else
		{
			DestinationString->Length = 0;
			DestinationString->MaximumLength = 0;
		}

		DestinationString->Buffer = (PWCHAR)SourceString;
	}

	INIT_FUNC(
		NTSTATUS, LdrLoadDll, (_In_opt_ PWSTR SearchPath, _In_opt_ PULONG LoadFlags, _In_ PUNICODE_STRING Name, _Out_opt_ PVOID *BaseAddress)
		)
	{
		auto ntDll = Peb::FindLoadedModuleHandle(L"ntdll.dll");
		if (!ntDll)
		{
			// �Ҳ���ntdll
			return STATUS_DLL_NOT_FOUND;
		}
		_LdrLoadDll = (FuncType_LdrLoadDll)PE::Export::GetProcAddress(ntDll, "LdrLoadDll");
		return _LdrLoadDll(SearchPath, LoadFlags, Name, BaseAddress);
	};


	HINSTANCE WINAPI _LoadLibraryExW(LPCWSTR lpLibFileName, HANDLE ignore, DWORD dwFlags)
	{
		UNICODE_STRING DllName;
		HINSTANCE hInst;
		ULONG DllCharacteristics = 0;
		if (dwFlags & DONT_RESOLVE_DLL_REFERENCES)
		{
			DllCharacteristics = IMAGE_FILE_EXECUTABLE_IMAGE;
		}
		hInst = Peb::FindLoadedModuleHandle(lpLibFileName);
		if (hInst)
		{
			return hInst;
		}
		_RtlInitUnicodeString(&DllName, (PCWSTR)lpLibFileName);
		if (!NT_SUCCESS(_LdrLoadDll(NULL, &DllCharacteristics, &DllName, (PVOID*)&hInst)))
		{
			return NULL;
		}
		return hInst;
	}


	HINSTANCE WINAPI _LoadLibraryW(LPCWSTR lpLibFileName)
	{
		return _LoadLibraryExW(lpLibFileName, NULL, 0);
	}

	FARPROC _GetProcAddress(HMODULE module, LPCSTR lpProcName)
	{
		return PE::Export::GetProcAddress(module, lpProcName);
	}

#pragma endregion



#pragma region ���ú�������
#define DEF_DEFAULT_FUNC
#ifdef DEF_DEFAULT_FUNC
	// FIX: 32λʵ�ֿ�����__declspec(naked) __stdcall�������ں�����jmp��api��ַʵ����ת, ����64λ�������������,��������ʱ��һ��ͨ�õķ���
	// �����MessageBoxA������
	INIT_FUNC(int,MessageBoxA,(
		_In_opt_ HWND hWnd,
		_In_opt_ LPCSTR lpText,
		_In_opt_ LPCSTR lpCaption,
		_In_ UINT uType))
	{
		auto dll = _LoadLibraryW(L"user32.dll");
		_MessageBoxA = (FuncType_MessageBoxA)_GetProcAddress(dll, "MessageBoxA");
		return _MessageBoxA(hWnd,lpText,lpCaption,uType);
	}
	

#endif
#pragma endregion

}