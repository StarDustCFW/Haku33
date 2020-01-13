#include "FileSystem.hpp"
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <memory>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <dirent.h>

#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <dirent.h>
#include <vector>
#include <string>
#include <cstdio>
#include <filesystem>

#include <unistd.h>
#include <sys/socket.h>

#include <string>
#include <iostream>
#include <filesystem>
#include <switch.h>

#include <string>
#include <cstring>
#include <fstream>
#include <vector>

#include <sys/stat.h>


namespace fs
{

	bool IsExist(std::string Path) 
	{
		std::ifstream ifs(Path);
		bool ex = ifs.good();
		ifs.close();
		return ex;
	}

	bool DirExists(const char* const path)
	{
		struct stat info;

		int statRC = stat(path, &info);
		if (statRC != 0)
		{
			if (errno == ENOENT) { return 0; } // something along the path does not exist
			if (errno == ENOTDIR) { return 0; } // something in path prefix is not a dir
			return false;
		}

		return (info.st_mode & S_IFDIR) ? true : false;
	}

	bool IsFile(std::string Path)
	{
		bool is = false;
		struct stat st;
		if (stat(Path.c_str(), &st) == 0) if (st.st_mode & S_IFREG) is = true;
		return is;
	}
	void CreateFile(std::string Path)
	{
		std::ofstream ofs(Path);
		ofs.close();
	}
	void CreateDir(std::string Path) 
	{
		mkdir(Path.c_str(), 777);
	}
	void DeleteFile(std::string Path)
	{
		if(IsExist(Path))
			remove(Path.c_str());
	}

	void DeleteDir(std::string Path)
	{
		DIR *d = opendir(Path.c_str());
		if (d)
		{
			struct dirent *dent;
			while (true)
			{
				dent = readdir(d);
				if (dent == NULL) break;
				std::string nd = dent->d_name;
				std::string pd = Path + "/" + nd;
				if (fs::IsFile(pd)) DeleteFile(pd);
				else DeleteDir(pd);
			}
		}
		closedir(d);
	}

	void WriteFile(std::string Path, std::string Content)
	{
		DeleteFile(Path);
		std::ofstream file(Path);
		if (file.is_open())
		{
			file << Content;
			file.close();
		}
	}
/*
	bool Unzip(std::string Path)
	{	
		//DeleteDir("sdmc://Update/tmp");
		//CreateDir("sdmc://Update/tmp");
		//Zipper zipper(NULL);
		zipper::Unzipper unzipper(Path);// = new Unzipper(true);
		bool res = unzipper.extract("sdmc://");
		unzipper.close();

		if (res)
			DeleteFile(Path);

		return res;
	}

	bool UnzipTo(std::string Path, std::string unzipTo)
	{
		//DeleteDir("sdmc://Update/tmp");
		//CreateDir("sdmc://Update/tmp");
		//Zipper zipper(NULL);
		zipper::Unzipper unzipper(Path);// = new Unzipper(true);
		bool res = unzipper.extract(unzipTo);
		unzipper.close();

		if (res)
			DeleteFile(Path);

		return res;
	}
*/
	std::string ReadFile(std::string Path)
	{
		std::ifstream file(Path);
		if (file.is_open())
		{
			std::string res((std::istreambuf_iterator<char>(file)),
				std::istreambuf_iterator<char>());

			return res;
		}
		else
			return "";
	}

	void copyDirToDir(const std::string& from, const std::string& to)
	{
		dirList list;
		list.assign(from);

		for (unsigned i = 0; i < list.getCount(); i++)
		{
			if (list.isDir(i))
			{
				std::string newFrom = from + list.getItem(i) + "/";
				std::string newTo = to + list.getItem(i);
				if(!fs::DirExists(newTo.c_str()))
					mkdir(newTo.c_str(), 0777);
				newTo += "/";

				copyDirToDir(newFrom, newTo);
			}
			else
			{
				std::string fullFrom = from + list.getItem(i);
				std::string fullTo = to + list.getItem(i);

				copyFile(fullFrom, fullTo);
			}
		}
	}

	void copyFile(const std::string& from, const std::string& to)
	{
		std::fstream f(from, std::ios::in | std::ios::binary);
		std::fstream t(to, std::ios::out | std::ios::binary);

		f.seekg(0, f.end);
		size_t fileSize = f.tellg();
		f.seekg(0, f.beg);

		uint8_t* buff = new uint8_t[0x80000];
		for (unsigned i = 0; i < fileSize; )
		{
			f.read((char*)buff, 0x80000);
			t.write((char*)buff, f.gcount());

			i += f.gcount();
		}

		delete[] buff;

		f.close();
		t.close();
	}

/*	char* FS_GetFilePermission(const char* filename)
	{
		static char perms[11];
		struct stat attr;

		if (!R_SUCCEEDED(stat(filename, &attr)))
			return "f";

		snprintf(perms, 11, "%s%s%s%s%s%s%s%s%s%s", (S_ISDIR(attr.st_mode)) ? "d" : "-", (attr.st_mode & S_IRUSR) ? "r" : "-",
			(attr.st_mode & S_IWUSR) ? "w" : "-", (attr.st_mode & S_IXUSR) ? "x" : "-", (attr.st_mode & S_IRGRP) ? "r" : "-",
			(attr.st_mode & S_IWGRP) ? "w" : "-", (attr.st_mode & S_IXGRP) ? "x" : "-", (attr.st_mode & S_IROTH) ? "r" : "-",
			(attr.st_mode & S_IWOTH) ? "w" : "-", (attr.st_mode & S_IXOTH) ? "x" : "-");

		return perms;
	}*/
	void dirList::assign(const std::string& _path)
	{
		path = _path;

		d = opendir(path.c_str());

		item.clear();

		while ((ent = readdir(d)))
		{
			char ext[5];
			memset(ext, 0, 5);
			memcpy(ext, &ent->d_name[strlen(ent->d_name) - 4], 4);
			item.push_back(ent->d_name);
		}

		closedir(d);
	}
	void dirList::rescan()
	{
		item.clear();
		d = opendir(path.c_str());

		while ((ent = readdir(d)))
			item.push_back(ent->d_name);

		closedir(d);
	}
	std::string dirList::getItem(int index)
	{
		return item[index];
	}
	bool dirList::isDir(int index)
	{
		std::string fullPath = path + item[index];
		struct stat s;
		return stat(fullPath.c_str(), &s) == 0 && S_ISDIR(s.st_mode);
	}
	unsigned dirList::getCount()
	{
		return item.size();
	}
}