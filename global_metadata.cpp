/*
    Pterodon - global storage metadata
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

#include "global_metadata.hpp"
#include "metadata.hpp"
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include "framework.hpp"
#include <private/android_filesystem_config.h>

Pterodon::wrappers::metadata_handler Pterodon::StorageMetadata::metadata;

namespace Pterodon {

bool StorageMetadata::GetMetadata(const std::string& path) {
bool ret = metadata.GetMetadata(path);
if (!ret) {
SetUserID(AID_MEDIA_RW);
SetGroupID(AID_MEDIA_RW);
SetPermissions(0660);
}
return ret;
}

void StorageMetadata::SetIdentifiers(const std::string& path) {
metadata.SetIdentifiers(path);
}

void StorageMetadata::SetPermissions(const std::string& path) {
if (Pterodon::Framework::FileExists(path))
chmod(path.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
else
metadata.SetPermissions(path);
}

void StorageMetadata::SetPermissions(const mode_t mode) {
metadata.SetPermissions(mode);
}

void StorageMetadata::SetAccessTime(const std::string& path) {
metadata.SetAccessTime(path);
}

void StorageMetadata::SetAll(const std::string& path) {
metadata.SetAll(path);
}

uid_t StorageMetadata::GetUserID(void) {
return metadata.GetUserID();
}

gid_t StorageMetadata::GetGroupID(void) {
return metadata.GetGroupID();
}

void StorageMetadata::SetUserID(uid_t user_id) {
metadata.SetUserID(user_id);
}

void StorageMetadata::SetGroupID(gid_t group_id) {
metadata.SetGroupID(group_id);
}

mode_t StorageMetadata::GetAccessMode(void) {
return metadata.GetAccessMode();
}

security_context_t StorageMetadata::GetSelinuxContext(void) {
return metadata.GetSelinuxContext();
}

bool StorageMetadata::GetSelinuxContext(const std::string& path) {
  return metadata.GetSelinuxContext(path);
}

bool StorageMetadata::SetSelinuxContext(const std::string& path) {
return metadata.SetSelinuxContext(path);
}

} // namespace Pterodon
