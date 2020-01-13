#pragma once
#include <string>
#include <filesystem>
#include <fstream>
#include <dirent.h>
#include <vector>

namespace fs
{
	bool IsExist(std::string Path);
	bool IsFile(std::string Path);
	bool DirExists(const char* const path);
	void CreateFile(std::string Path);
	void CreateDir(std::string Path);
	void DeleteFile(std::string Path);
	void DeleteDir(std::string Path);
	void WriteFile(std::string Path, std::string Content);
//	bool Unzip(std::string Path);
//	bool UnzipTo(std::string Path, std::string unzipTo);
	std::string ReadFile(std::string Path);
	void copyDirToDir(const std::string& from, const std::string& to);
	void copyFile(const std::string& from, const std::string& to);
	char *FS_GetFilePermission(const char* filename);

	class dirList
	{
	public:
		void assign(const std::string& _path);
		void rescan();

		std::string getItem(int index);
		bool isDir(int index);
		unsigned getCount();

	private:
		DIR* d;
		struct dirent* ent;
		std::string path;
		std::vector<std::string> item;
	};
}