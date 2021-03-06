#include "QDataStdAfx.h"
#include "util/Folder_Walker.h"

#if defined(Q_POSIX_API)

#include <utime.h> //utime
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

using namespace q;

static std::vector<std::pair<Path, bool>> enumerate_files(Path const& path)
{ 
#define Q_MAX_PATH	1024
	std::vector<std::pair<Path, bool>> data;
	if (path.get_as_string().size() >= Q_MAX_PATH - 5)
	{
		return data;
	}

	data.reserve(512);

    auto* cwd = getcwd(nullptr, 0);
    QLOGD("current folder: {}", cwd);
    free(cwd);

    QLOGD("*** +folder: {}", path);

    DIR* dip = nullptr;
    struct dirent* dit = nullptr;

    if ((dip = opendir(path.get_as_string().c_str())) == nullptr)
	{
        QLOGW("*** cannot open folder: {} - {}", path, strerror(errno));
		return data;
	}

    while ((dit = readdir(dip)) != nullptr)
	{
        if (util::strcmp(dit->d_name, "..") != 0 && util::strcmp(dit->d_name, ".") != 0)
		{
			data.emplace_back(Path(dit->d_name), dit->d_type == DT_DIR);
            QLOGD("***    file: {}", dit->d_name);
		}
	}	

    QLOGD("*** -folder: {}", path);

	closedir(dip);
#undef Q_MAX_PATH

	return data;
} 

#else

#include <sys/utime.h>//_utime
#include <windows.h> 
#include <direct.h>
#include <errno.h>

using namespace q;

static std::vector<std::pair<Path, bool>> enumerate_files(Path const& path)
{ 
	std::vector<std::pair<Path, bool>> data;

	if (path.size() >= _MAX_PATH - 5)
	{
		return data;
	}

	data.reserve(512);

	char findpath[_MAX_PATH];
	HANDLE fh; 
    WIN32_FIND_DATAA wfd;
	strcpy (findpath, path.get_as_string().c_str()); 
	strcat (findpath, "/*.*");

    fh = FindFirstFileA(findpath, &wfd);
	if (fh != INVALID_HANDLE_VALUE) 
	{ 
		do 
		{ 
			if (util::strcmp(wfd.cFileName, "..") && util::strcmp(wfd.cFileName, "."))
			{
				bool isDir = (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
				data.emplace_back(Path(wfd.cFileName), isDir);
			}
        } while (FindNextFileA(fh, &wfd));
		FindClose (fh); 
	} 

	return data;
} 

#endif

namespace q
{
namespace util
{
namespace fs
{

Folder_Walker::Folder_Walker()
{
	;
}
Folder_Walker::Folder_Walker(Path const& root, bool recurse)
	: m_root(root)
	, m_recurse(recurse)
{
	refresh();
}
void Folder_Walker::refresh(Path const& root, bool recurse)
{
	m_root = root;				  
	m_recurse = recurse;
	refresh();
}
Path const& Folder_Walker::get_root() const
{
	return m_root;
}

void Folder_Walker::refresh()
{
	std::vector<Path> paths;
	paths.reserve(100);
	paths.emplace_back(Path());

	m_all_collected_files.clear();
	m_all_collected_files.reserve(100);

	Path root(m_root);

    QLOGD("*** walker root: {}", root);

	while (!paths.empty())
	{
		Path path(paths.back());
		paths.pop_back();

		size_t size = root.size();
		root += path;
		auto d = enumerate_files(root);
		root.shrink(size);

		for(auto const& p: d)
		{
            QLOGD("*** walker x {}: {}", p.second, p.first);

			if (p.second)
			{
				paths.emplace_back(path + p.first);
			}
			else
			{
				m_all_collected_files.resize(m_all_collected_files.size() + 1);
				m_all_collected_files.back().path = path + p.first;
				m_all_collected_files.back().full_path = root + m_all_collected_files.back().path;

                QLOGD("*** walker y: {}", m_all_collected_files.back().full_path);
			}
		};

		if (!m_recurse)
		{
			break;
		}
	}
}

std::vector<Folder_Walker::File_Info> const& Folder_Walker::get_files() const
{
	return m_all_collected_files;
}

} //fs
} //util
} //q
