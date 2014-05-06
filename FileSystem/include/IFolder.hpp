#ifndef IFOLDER_HPP
#define IFOLDER_HPP

#include <memory>
#include <vector>

#include "IFile.hpp"
#include "Export.hpp"

namespace FileSystem
{
	class EXPORT IFolder
	{
	public:
		virtual std::wstring GetName() = 0;
		virtual std::vector<std::shared_ptr<IFolder>> GetSubfolders() = 0;
		virtual std::vector<std::shared_ptr<IFile>> GetFiles() = 0;
		virtual bool ContainsFile(const std::wstring& fileName) = 0;
		virtual std::shared_ptr<IFile> GetFile(const std::wstring& fileName) = 0;

		virtual std::shared_ptr<IFile> CreateNewFile(const std::wstring& fileName) = 0;
	};
}

#endif