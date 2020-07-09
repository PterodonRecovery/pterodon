/*
    Pterodon - metadata handler
    Copyright (C) <2020> ATGDroid <bythedroid@gmail.com>

    This file is part of Pterodon Recovery

    Pterodon is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Pterodon is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Pterodon.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef PTERODON_METADATA_HANDLER_HPP
#define PTERODON_METADATA_HANDLER_HPP

#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <utime.h>
#include "selinux/selinux.h"

namespace Pterodon {

namespace wrappers {

class metadata_handler
{
public:
	metadata_handler() {}
	virtual ~metadata_handler() {}
	bool GetMetadata(const std::string& path);
	void SetPermissions(const std::string& path);
	void SetAccessTime(const std::string& path);
	void SetIdentifiers(const std::string& path);
	void SetAll(const std::string& path);
	uid_t GetUserID(void);
	gid_t GetGroupID(void);
	void SetUserID(uid_t user_id);
	void SetGroupID(gid_t group_id);
	void SetPermissions(const mode_t mode);
	mode_t GetAccessMode(void);
	security_context_t GetSelinuxContext(void);
	bool GetSelinuxContext(const std::string& path);
	bool SetSelinuxContext(const std::string& path);
private:
	struct stat st;
	struct utimbuf st_time;
	security_context_t selinux_context;
};

} // namespace wrappers
} // namespace Pterodon


#endif // PTERODON_METADATA_HANDLER_HPP
