#include "stdafx.h"
#include "NtDll.h"
namespace Process
{
	namespace LazyLoad
	{
		namespace NtDll
		{
			namespace Detail
			{
				const LoadNtDll& Instance()
				{
					static LoadNtDll instance;
					return instance;
				}
				LoadNtDll::LoadNtDll() :
					LoadDllBase(L"ntdll.dll"),
					LdrLoadDll(*this, "LdrLoadDll"),
					NtOpenProcess(*this, "NtOpenProcess"),
					NtSetContextThread(*this, "NtSetContextThread"),
					NtProtectVirtualMemory(*this, "NtProtectVirtualMemory"),
					NtReadVirtualMemory(*this, "NtReadVirtualMemory"),
					NtWriteVirtualMemory(*this, "NtWriteVirtualMemory"),
					NtFlushInstructionCache(*this, "NtFlushInstructionCache"),
					NtAllocateVirtualMemory(*this, "NtAllocateVirtualMemory"),
					NtQueryVirtualMemory(*this, "NtQueryVirtualMemory"),
					NtFreeVirtualMemory(*this, "NtFreeVirtualMemory")
				{
				}
			}  // namespace Detail

		}  // namespace NtDll

	}  // namespace LazyLoad
}  // namespace Process
