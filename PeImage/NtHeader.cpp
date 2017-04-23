#include "stdafx.h"
#include "DosHeader.h"
#include "NtHeader.h"
#include "DataDirectoryEntries.h"
namespace PeDecoder
{

	NtHeader::~NtHeader()
	{
		ptr_ = nullptr;
	}
	NtHeaderType NtHeader::GetHeaderType(const void* ptr)
	{
		switch (reinterpret_cast<const IMAGE_NT_HEADERS32*>(ptr)->OptionalHeader.Magic)
		{
		case IMAGE_NT_OPTIONAL_HDR64_MAGIC:
			return NtHeaderType::NtHeader64;
		case IMAGE_NT_OPTIONAL_HDR32_MAGIC:
			return NtHeaderType::NtHeader32;
		case IMAGE_ROM_OPTIONAL_HDR_MAGIC:
			return NtHeaderType::Rom;
		default:
			// UnKnown
			break;
		}
		return NtHeaderType::UnKnown;
	}
	void * NtHeader::GetNtHeaderPtr(const PIMAGE_DOS_HEADER dosHeader)
	{
		return reinterpret_cast<unsigned char*>(dosHeader) + dosHeader->e_lfanew;
	}
	NtHeaderType NtHeader::GetHeaderType() const
	{
		return GetHeaderType(RawPtr());
	}
	MachineType NtHeader::GetMachineType() const
	{
		return static_cast<MachineType>(GetFileHeader()->Machine);
	}
	FileHeaderCharacteristics NtHeader::GetFileHeaderCharacteristics() const
	{
		return static_cast<FileHeaderCharacteristics>(GetFileHeader()->Characteristics);
	}
	bool NtHeader::IsValid() const
	{
		// ����ö��ֵ��������ݣ���֤����GetHeaderType���ر�ΪNtHeader32��NtHeader64֮һ
		// ��Ҫ����֤NtHeader32��NtHeader64��IMAGE_NT_SIGNATURE��ʶ��Rom�Ȳ���
		static const bool map[]{ false, false, false, false, false, false, true, true };
		auto ntHeader = reinterpret_cast<const PIMAGE_NT_HEADERS32>(const_cast<unsigned char*>(RawPtr()));

		auto pos = static_cast<unsigned char>(GetHeaderType()) +
			(static_cast<unsigned char>(ntHeader->Signature == IMAGE_NT_SIGNATURE) << 2);
		return map[pos];
	}
	PIMAGE_FILE_HEADER NtHeader::GetFileHeader() const
	{
		return &GetHeader32()->FileHeader;
	}
	WORD NtHeader::GetNumberOfSections() const
	{
		return GetFileHeader()->NumberOfSections;
	}
	unsigned char * NtHeader::RawPtr() const
	{
		assert(ptr_);
		return static_cast<unsigned char *>(ptr_);
	}
	NtHeader::NtHeader(void * ntHeaderRawPtr) :
		ptr_(ntHeaderRawPtr)
	{
	}
	PIMAGE_NT_HEADERS32 NtHeader::GetHeader32() const
	{
		return reinterpret_cast<PIMAGE_NT_HEADERS32>(RawPtr());
	}
}  // namespace PeDecoder
