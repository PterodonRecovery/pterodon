/*
    Pterodon Recovery - File manager
    Copyright (C) <2019> ATGDroid <bythedroid@gmail.com>

    This file is part of Pterodon Recovery Project

    Pterodon Recovery is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Pterodon Recovery is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Pterodon Recovery.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <dirent.h> // readdir, opendir, closedir, rewinddir
#include <string> // string
#include <sstream> // istringstream()
#include <sys/stat.h>
#include <fstream> // ifstream
#include <vector>
#include "FileManager.hpp" // header
#include "framework.hpp" // header
#include "constants.h"
#include "timer.hpp"

namespace Pterodon {

namespace Gui {
	
FileManager::~FileManager(void) {
clear();
}

size_t FileManager::size() const
{
	return folders_.size() + files_.size();
}

size_t FileManager::folderSize() const
{
	return folders_.size();
}

Pterodon::Gui::FileManager::ObjectData FileManager::folderAt(const size_t n) const
{
	return folders_.at(n);
}

Pterodon::Gui::FileManager::ObjectData FileManager::fileAt(const size_t n) const
{
	return files_.at(n);
}

size_t FileManager::fileSize() const
{
	return files_.size();
}

void FileManager::clear(void) {
folders_.clear();
files_.clear();
}

bool FileManager::empty(void) const {
return folders_.empty() && files_.empty();
}

void FileManager::SetExtension(const std::string& ext) {
fileExtension_ = ext;
}

bool FileManager::sortobjects(const ObjectData& firstObject, const ObjectData& secondObject) {
return firstObject < secondObject;
}

bool FileManager::ChangeDirectory(const std::string& folder_path)
{
#ifdef PTERODON_DEBUG_MODE
	Timer filemanager_timer("Listing of directory");
#endif
	DIR* d;
	clear();
	d = opendir(folder_path.c_str());
	if (d == NULL) {
		LOGW("Unable to open '%s'", folder_path.c_str());
		return false;
	}
	struct dirent* de;
	bool is_root = (folder_path == "/");

	while ((de = readdir(d)) != NULL) {
		if ((de->d_name[0] == '.')&& (de->d_name[1] == 0))
                continue;
           
        if ((de->d_name[0] == '.')&& (de->d_name[1] == '.') && (de->d_name[2] == 0) && is_root)
                continue;
                
		ObjectData fileData;
		std::string full_path = folder_path;
		fileData.fileName_ = de->d_name;
		
		if (!is_root) full_path += "/";
		  full_path += fileData.fileName_;
		  struct stat st;
	    if (!Framework::stat_path(full_path, st)) continue;
		switch (st.st_mode & S_IFMT) {
        case S_IFREG:
        /* regular file */
            fileData.fileType_ = DT_REG;
            break;
        case S_IFDIR:
        /* directory */
            fileData.fileType_ = DT_DIR;
            break;
        case S_IFLNK:
        /* symbolic link */
            fileData.fileType_ = DT_LNK;
            break;
        case S_IFBLK:
        /* block device */
            fileData.fileType_ = DT_BLK;
            break;
        case S_IFIFO:
        /* fifo/pipe */
            fileData.fileType_ = DT_FIFO;
            break;
        case S_IFSOCK:
        /* socket */
            fileData.fileType_ = DT_SOCK;
            break;
        case S_IFCHR:
        /* character device */
            fileData.fileType_ = DT_CHR;
            break;
        default:
        /* unknown object */
            /* should we even care? */
            LOGW("Unable to detect d type of '%s'\n", full_path.c_str());
            //fileData.fileType_ = DT_UNKNOWN;
            continue;
     }
     
        fileData.fileSize_ = st.st_size;
        fileData.cTime_ = st.st_ctime;
        
		if (fileData.fileType_ == DT_REG || fileData.fileType_ == DT_LNK || fileData.fileType_ == DT_BLK) {
		if (fileExtension_.empty() || (fileData.fileName_.length() > fileExtension_.length() && fileData.fileName_.substr(fileData.fileName_.length() - fileExtension_.length()) == fileExtension_)) {
			files_.push_back(fileData);
		  }
		} else if (fileData.fileType_ == DT_DIR) {
            folders_.push_back(fileData);
		}
	  }
	
    closedir(d);
	std::sort(folders_.begin(), folders_.end(), sortobjects);
	std::sort(files_.begin(), files_.end(), sortobjects);

    return true;
}

void FileManager::print(void) {
std::vector<ObjectData>::const_iterator it; 
for (it = folders_.begin(); it != folders_.end(); it++) {
LOGI("Folder: %s", it->fileName_.c_str());
}
for(it = files_.begin(); it != files_.end(); it++) {
LOGI("File: %s", it->fileName_.c_str());
 }
}

} // namespace Gui
} // namespace Pterodon
