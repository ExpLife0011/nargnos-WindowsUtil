#pragma once

#include "DirectoryBase.h"
namespace PeDecoder
{
	class ImportDescriptorIterator;
	// UNDONE: 
	class ImportDirectory :
		public Detail::DirectoryBase<ImportDirectory, DataDirectoryEntryType::Import>
	{
	public:
		typedef ImportDescriptorIterator iterator;
		// �����size��ʾ�����ṹ����thunk��name�Ĵ�С
		iterator begin();
		iterator end();
	
		friend TBase;
	protected:
		using DirectoryBase::DirectoryBase;
	};

	

}  // namespace PeDecoder
