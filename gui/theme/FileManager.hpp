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

#ifndef _PTERODON_FILEMANAGER_HPP
#define _PTERODON_FILEMANAGER_HPP

#include <string>
#include <vector>
#include <time.h>

namespace Pterodon {

namespace Gui {

class FileManager
{
public:
	FileManager() = default;
	virtual ~FileManager();

struct ObjectData {
		std::string fileName_;
		unsigned int fileType_;
		size_t fileSize_;
		time_t cTime_;
		ObjectData() : fileName_(""), fileType_(0), fileSize_(0), cTime_(0)
        {
        }
		bool operator<(const ObjectData& p) const
    {
        return fileName_ < p.fileName_;
    }
};

std::vector<ObjectData>::iterator fileBegin() { return files_.begin(); }
std::vector<ObjectData>::iterator fileEnd() { return files_.end(); }
std::vector<ObjectData>::iterator folderBegin() { return folders_.begin(); }
std::vector<ObjectData>::iterator folderEnd() { return folders_.end(); }

std::vector<ObjectData>::const_iterator fileBegin() const { return files_.begin(); }
std::vector<ObjectData>::const_iterator fileEnd() const { return files_.end(); }
std::vector<ObjectData>::const_iterator folderBegin() const { return folders_.begin(); }
std::vector<ObjectData>::const_iterator folderEnd() const { return folders_.end(); }

public:
ObjectData folderAt(const size_t n) const;
ObjectData fileAt(const size_t n) const;
void SetExtension(const std::string& ext);
bool ChangeDirectory(const std::string& folder_path);
void clear(void);
size_t size(void) const;
size_t folderSize(void) const;
size_t fileSize(void) const;
void print(void);
bool empty(void) const;
private:
static bool sortobjects(const ObjectData& firstObject, const ObjectData& secondObject);
std::string current_directory_;
std::string fileExtension_;
std::vector<ObjectData> folders_;
std::vector<ObjectData> files_;
};

} // namespace wrappers
} // namespace Pterodon

#endif // _PTERODON_FILEMANAGER_HPP
		