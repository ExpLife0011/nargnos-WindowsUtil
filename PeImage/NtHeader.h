#pragma once
#include <windows.h>
#include <memory>
#include "INtHeader.h"
#include "FileHeaderCharacteristics.h"
namespace PeDecoder
{
	class NtHeader :
		public INtHeader
	{
	public:
		virtual ~NtHeader();
		static NtHeaderType GetHeaderType(const void* ptr);
		static void* GetNtHeaderPtr(const PIMAGE_DOS_HEADER dosHeader);
		virtual NtHeaderType GetHeaderType() const override;
		virtual MachineType GetMachineType() const override;
		virtual FileHeaderCharacteristics GetFileHeaderCharacteristics() const override;
		virtual bool IsValid() const override;
		virtual PIMAGE_FILE_HEADER GetFileHeader() const override;
		virtual WORD GetNumberOfSections() const override;
		virtual unsigned char* RawPtr() const override;
	protected:
		NtHeader(const NtHeader&) = delete;
		NtHeader& operator=(const NtHeader&) = delete;
		explicit NtHeader(void* ntHeaderRawPtr);
		PIMAGE_NT_HEADERS32 GetHeader32() const;
		void* ptr_;
	};
}  // namespace PeDecoder
