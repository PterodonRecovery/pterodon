/*
    Pterodon - version manager
    Copyright (C) <2020> ATGDroid <bythedroid@gmail.com>

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

#ifndef _pterodon_version_header_hpp
#define _pterodon_version_header_hpp

#include <string>

#define PTERODON_INFO_NAME        0
#define PTERODON_INFO_VERSION     1
#define PTERODON_INFO_AUTHOR      2
#define PTERODON_INFO_YEAR          3
#define PTERODON_INFO_CODENAME    4
#define PTERODON_INFO_COPYRIGHT   5

namespace Pterodon {
	
class VersionManager
{
public:
    static std::string GetInfo(int info_type);
private:
    static char* GetVersion();
}; // VersionManager

} // namespace Pterodon


#endif // _pterodon_version_header_hpp

