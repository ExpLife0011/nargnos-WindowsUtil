#pragma once
#include <Windows.h>
#include <WinBase.h>
#include <memory>
#include "ISectionHeaders.h"
namespace PeDecoder
{
	class PeImage;
	class SectionHeaders :
		public ISectionHeaders
	{
	public:
		typedef Detail::Section* DataPtr;
		typedef DataPtr iterator;
		virtual DataPtr RawPtr() const override;
		virtual WORD GetSize() const override;
		virtual DWORD GetSectionAlignment() const override;

		virtual DataPtr RvaToSectionHeader(DWORD rva) const override;
		virtual DataPtr OffsetToSectionHeader(DWORD fileOffset) const override;

		// Ϊ�˼���c++�㷨������ĸ����д��
		virtual iterator begin() const override;
		// ��β����һԪ�ص�ַ
		virtual iterator end() const override;
		static _STD unique_ptr<ISectionHeaders> Create(const PeImage& pe);
		virtual ~SectionHeaders();
	protected:
		SectionHeaders(const SectionHeaders&) = delete;
		SectionHeaders& operator=(const SectionHeaders&) = delete;
		explicit SectionHeaders(const PeImage& pe);
		DataPtr ptr_;
		DWORD alignment_;
		WORD size_;
	};

}  // namespace PeDecoder
