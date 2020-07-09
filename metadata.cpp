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

#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <utime.h>
#include "metadata.hpp"
#include "framework.hpp"
#include "selinux/selinux.h"


namespace Pterodon {

namespace wrappers {

bool metadata_handler::GetMetadata(const std::string& path) {
if (!Pterodon::Framework::stat_path(path, st)) {
st.st_size = -1;
return false;
}
st_time.actime = st.st_atime;
st_time.modtime = st.st_mtime;
return true;
}

void metadata_handler::SetIdentifiers(const std::string& path) {
if (st.st_size != -1) {
chown(path.c_str(), st.st_uid, st.st_gid);
}
}

void metadata_handler::SetPermissions(const std::string& path) {
chmod(path.c_str(), st.st_mode);
}

void metadata_handler::SetAccessTime(const std::string& path) {
if (st.st_size != -1) {
utime(path.c_str(), &st_time);
}
}

void metadata_handler::SetAll(const std::string& path) {
if (st.st_size != -1) {
SetIdentifiers(path);
SetPermissions(path);
SetAccessTime(path);
   }
}

uid_t metadata_handler::GetUserID(void) {
return st.st_uid;
}

gid_t metadata_handler::GetGroupID(void) {
return st.st_gid;
}

void metadata_handler::SetUserID(uid_t user_id) {
st.st_uid = user_id;
}

void metadata_handler::SetGroupID(gid_t group_id) {
st.st_gid = group_id;
}

void metadata_handler::SetPermissions(const mode_t mode) {
st.st_mode = mode;
}

mode_t metadata_handler::GetAccessMode(void) {
return st.st_mode;
}

security_context_t metadata_handler::GetSelinuxContext(void) {
return selinux_context;
}

bool metadata_handler::GetSelinuxContext(const std::string& path) {
  if (lgetfilecon(path.c_str(), &selinux_context) < 0) {
	selinux_context = NULL;
	return false;
    }
    return true;
}

bool metadata_handler::SetSelinuxContext(const std::string& path) {
if (!selinux_context || lsetfilecon(path.c_str(), selinux_context) < 0) {
	return false;
	}
	return true;
}

} // namespace wrappers
} // namespace Pterodon

