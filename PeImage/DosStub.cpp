#include "stdafx.h"
#include "DosStub.h"
#include "PeImage.h"
#include "DosHeader.h"
#include "NtHeader.h"

namespace PeDecoder
{

	DosStub::DosStub(const _STD shared_ptr<IPeImage>& pe) :
		size_(0),
		ptr_(nullptr),
		pe_(pe)
	{
		auto& dos = *pe->GetDosHeader();
		auto& nt = *pe->GetNtHeader();
		auto ntPtr = nt.RawPtr();  // NtHeaderλ��
		auto ptr = reinterpret_cast<unsigned char*>(dos.RawPtr() + 1); // dosStubӦ���ڵ�λ��
		auto size = ntPtr - ptr; // ʵ�ʴ�С��NtHeader��DosHeader�ص�ʱ������Ϊ�޸ģ�û��DosStub
		if (size > 0)
		{
			ptr_ = ptr;
			size_ = size;
		}
	}
	bool DosStub::IsExist() const
	{
		return GetSize() != 0;
	}
	DWORD DosStub::GetSize() const
	{
		return size_;
	}
	DosStub::operator bool() const
	{
		return IsExist();
	}
	DosStub::iterator DosStub::begin() const
	{
		assert(ptr_);
		return ptr_;
	}

	DosStub::iterator DosStub::end() const
	{
		return begin() + GetSize();
	}

	DosStub::~DosStub()
	{
		ptr_ = nullptr;
	}

}  // namespace PeDecoder
