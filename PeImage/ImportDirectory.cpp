#include "stdafx.h"
#include "ImportDirectory.h"
#include "ImportDirectoryIterator.h"
namespace PeDecoder
{
	ImportDirectory::ImportDirectory(PeImage & pe) :
		DirectoryBase(pe)
	{

	}
	typename ImportDirectory::iterator ImportDirectory::begin()
	{
		return iterator(*this, GetPtr());
	}
	typename ImportDirectory::iterator ImportDirectory::end()
	{
		return iterator(*this, nullptr);
	}
	
}  // namespace PeDecoder
