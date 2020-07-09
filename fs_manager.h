/*
    Pterodon - filesystem manager
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

#ifndef RECOVERY_ROOTS_H_
#define RECOVERY_ROOTS_H_

#include <string>
#include <sys/poll.h>

typedef struct fstab_rec Volume;
// Load and parse volume data from /etc/recovery.fstab.
void load_volume_table();

// Return the Volume* record for this mount point (or nullptr).
Volume* volume_for_mount_point(const std::string& mount_point);
Volume* get_device_volumes();
Volume* volume_for_path(const char* path);

// Make sure that the volume 'path' is on is mounted.  Returns 0 on
// success (volume is mounted).
int ensure_volume_mounted(Volume* v);
int ensure_path_mounted(const char* path);

// Similar to ensure_path_mounted, but allows one to specify the mount_point.
int ensure_path_mounted_at(const char* path, const char* mount_point);

// Make sure that the volume 'path' is on is unmounted.  Returns 0 on
// success (volume is unmounted);
int ensure_volume_unmounted(const Volume* v, bool detach = false);
int ensure_path_unmounted(const char* path);

// Reformat the given volume (must be the mount point only, eg
// "/cache"), no paths permitted.  Attempts to unmount the volume if
// it is mounted.
int format_volume(const char* volume);

// Reformat the given volume (must be the mount point only, eg
// "/cache"), no paths permitted.  Attempts to unmount the volume if
// it is mounted.
// Copies 'directory' to root of the newly formatted volume
int format_volume(const char* volume, const char* directory);

// Ensure that all and only the volumes that packages expect to find
// mounted (/tmp and /cache) are mounted.  Returns 0 on success.
int setup_install_mounts();

// storage
std::string get_primary_storage_path(void);
std::vector<std::string> get_extra_storage_paths(void);
std::string get_android_secure_path();
void setup_legacy_storage_paths();
int get_num_extra_volumes();
int get_num_volumes();
int is_primary_storage_voldmanaged();

Volume* get_device_volumes();


bool IsFilesystemSupported(const char* filesystem);
bool Is_Extended_Fs(const std::string& path);
int Get_Size_Via_statfs(const char* Path);
int Find_Partition_Size(const char* Path);
char* readlink_device_blk(const char* Path);

int get_num_volumes();

bool is_path_mounted(const char* mount_point);

char* get_real_fstype(Volume* rec);


int is_data_media();
void setup_data_media(int mount);
int is_data_media_volume_path(const char* path);
void preserve_data_media(int val);
int is_data_media_preserved();

// check if it is an extra storage volume
int is_volume_primary_storage(Volume* v);
int is_volume_extra_storage(Volume* v);

int has_datadata();

#define MAX_NUM_MANAGED_VOLUMES 10

#endif  // RECOVERY_ROOTS_H_
