#include "stdafx.h"
#include "DosHeader.h"
namespace PeDecoder
{
	DosHeader::DosHeader(PIMAGE_DOS_HEADER ptr) :
		ptr_(ptr)
	{

	}
	bool DosHeader::IsValid(const PIMAGE_DOS_HEADER ptr)
	{
		assert(ptr);
		return ptr->e_magic == IMAGE_DOS_SIGNATURE;
	}

	_STD unique_ptr<IDosHeader> DosHeader::Create(const PIMAGE_DOS_HEADER rawptr)
	{
		return _STD unique_ptr<IDosHeader>(new DosHeader(rawptr));
	}

	bool DosHeader::IsValid() const
	{
		return IsValid(RawPtr());
	}

	PIMAGE_DOS_HEADER DosHeader::RawPtr() const
	{
		assert(ptr_);
		return ptr_;
	}

	DosHeader::~DosHeader()
	{
		// ��ʵ����Ҫ������Ҫ��ֹ�������õ�����������飬������ͬ
		ptr_ = nullptr;
	}


}  // namespace PeDecoder
