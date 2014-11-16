#ifndef FOLDER_HPP
#define FOLDER_HPP

#include <string>
#include <vector>

#include "Util.hpp"
#include "IFolder.hpp"

namespace FileSystem
{
	class Storage;
	class IFile;

	class Folder final : public IFolder
	{
	public:

		virtual std::wstring GetName() const override;
		virtual std::wstring GetFullPath() const override;
		virtual std::shared_ptr<IFolder> GetParent() override;
		virtual std::vector<std::shared_ptr<IFolder>> GetSubfolders() override;
		virtual std::vector<std::shared_ptr<IFile>> GetFiles() override;
		virtual bool ContainsFile(const std::wstring& fileName) override;
		virtual std::shared_ptr<IFile> GetFile(const std::wstring& fileName) override;

		virtual std::shared_ptr<IFile> CreateNewFile(const std::wstring& filename) override;

		virtual bool operator==(const IFolder& rhs) const override;
		virtual bool operator!=(const IFolder& rhs) const override;

#ifdef __cplusplus_winrt
		Folder(Windows::Storage::StorageFolder^ folder);
	private:
		Windows::Storage::StorageFolder^ m_folder;
#endif

		friend Storage; // so the static folders ( ie. music library) can be instantiated
	};
}

#endif