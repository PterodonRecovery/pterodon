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

#include "fs_manager.h"

#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/vfs.h>
#include <sys/mount.h>
#include <sys/stat.h>

#include <sys/poll.h>
#include <linux/netlink.h>
#include <sys/socket.h>
#include <linux/types.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>
#include <string>
#include <vector>

#include <android-base/logging.h>
#include <android-base/properties.h>
#include <android-base/stringprintf.h>
#include <android-base/unique_fd.h>
#include <cryptfs.h>
#include <cutils/fs.h>
#include <ext4_utils/wipe.h>
#include <fs_mgr.h>
#include "constants.h"
#include "mounts.h"
#include "framework.hpp"
#include "env_service.hpp"

#ifdef __bitwise
#undef __bitwise
#endif
#include <blkid/blkid.h>

static struct fstab* fstab = nullptr;

extern struct selabel_handle* sehandle;

static void write_fstab_entry(const Volume* v, FILE* file) {
  if (v && strcmp(v->fs_type, "mtd") != 0 && strcmp(v->fs_type, "emmc") != 0 &&
      strcmp(v->fs_type, "bml") != 0 && !fs_mgr_is_voldmanaged(v) &&
      strncmp(v->blk_device, "/", 1) == 0 && strncmp(v->mount_point, "/", 1) == 0) {
    fprintf(file, "%s ", v->blk_device);
    fprintf(file, "%s ", v->mount_point);
    fprintf(file, "%s ", v->fs_type);
    fprintf(file, "%s 0 0\n", v->fs_options ? v->fs_options : "defaults");
  }
}

int get_num_volumes() {
  return fstab->num_entries;
}

Volume* get_device_volumes() {
  return fstab->recs;
}

static int is_datamedia = 0;
int is_data_media(void)
{
    return is_datamedia;
}

// is volume a physical primary storage ?
int is_volume_primary_storage(Volume* v)
{
    if (v == NULL)
        return 0;

    // Static mount point /sdcard is primary storage, except when it's
    // declared as datamedia
    if (strcmp(v->mount_point, "/sdcard") == 0) {
        if (strcmp(v->fs_type, "datamedia") == 0) {
            return 0;
        }
        return 1;
    }

    // Static mount points beginning with /mnt/media_rw/sdcard are primary
    // storage except when a non-zero digit follows (eg. sdcard[1-9])
    if (strcmp(v->mount_point, "/mnt/media_rw/sdcard0") == 0 || strcmp(v->mount_point, "/mnt/media_rw/sdcard") == 0) {
        return 1;
    }

    // Dynamic mount points which label begins with sdcard are primary storage
    // except when a non-zero digit follows (eg. sdcard[1-9])
    // load_volume_table() allows a custom moint point different from /storage/label
    if (fs_mgr_is_voldmanaged(v)) {
        if (strcmp(v->label, "sdcard0") == 0 || strcmp(v->label, "sdcard") == 0)
            return 1;
    }

    return 0;
}

// check if the volume is used as secondary storage
// we also allow voldmanaged usb volumes
int is_volume_extra_storage(Volume* v) {
	if (v->mount_point && get_primary_storage_path() == v->mount_point)
        return 0;

    if (strcmp(v->mount_point, "/external_sd") == 0 ||
            strncmp(v->mount_point, "/mnt/media_rw/sdcard", 20) == 0) {
        return 1;
    }

    // load_volume_table() allows a custom moint point different from /storage/label
    if (fs_mgr_is_voldmanaged(v)) {
        return 1;
    }

    return 0;
}

void load_volume_table(void)
{
    int i;
    int ret;

    fs_mgr_free_fstab(fstab);
    fstab = fs_mgr_read_fstab("/etc/recovery.fstab");
    if (!fstab) {
        LOGI("Failed to read default fstab");
        return;
    }

    ret = fs_mgr_add_entry(fstab, "/tmp", "ramdisk", "ramdisk");
    if (ret < 0 ) {
        LOGI("failed to add /tmp entry to fstab");
        fs_mgr_free_fstab(fstab);
        fstab = NULL;
        return;
    }
    
    for (i = 0; i < fstab->num_entries; ++i) {
        Volume* v = &fstab->recs[i];

        // Process vold-managed volumes with mount point "auto"
        // we also allow a custom moint point different from /storage/label
        // https://source.android.com/devices/tech/storage/config-example.html
        if (fs_mgr_is_voldmanaged(v) && strcmp(v->mount_point, "auto") == 0) {
            char path[PATH_MAX];
            snprintf(path, PATH_MAX, "/storage/%s", v->label);
            free(v->mount_point);
            v->mount_point = strdup(path);
        }
    }

#ifdef USE_F2FS
    // allow switching between f2fs/ext4 depending on actual real format
    // if fstab entry matches the real device fs_type, do nothing
    // also skip vold managed devices as vold relies on the defined flags.
    // vold managed devices should be set to auto fstype for free formatting
    LOGI("checking ext4 <-> f2fs conversion...");
    for (i = 0; i < fstab->num_entries; ++i) {
        Volume* v = &fstab->recs[i];

        if (strcmp(v->fs_type, "ext4") == 0 || strcmp(v->fs_type, "f2fs") == 0) {
            char* real_fstype = get_real_fstype(v);
            if (real_fstype == NULL || strcmp(real_fstype, v->fs_type) == 0 || fs_mgr_is_voldmanaged(v))
                continue;

            if (strcmp(real_fstype, "ext4") == 0 || strcmp(real_fstype, "f2fs") == 0) {
                char *fstab_fstype;
                char *fstab_options = NULL;
                    // no fs_options2: drop to bare minimal default fs_options
                    fstab_fstype = strdup(v->fs_type);
                    free(v->fs_type);
                    v->fs_type = strdup(real_fstype);

                    if (v->fs_options != NULL)
                        free(v->fs_options);

                    if (strcmp(v->fs_type, "f2fs") == 0) {
                        v->fs_options = strdup("rw,noatime,nodev,nodiratime,inline_xattr");
                    } else {
                        // ext4: default options will be set in try_mount()
                        v->fs_options = NULL;
                    }
                }
                LOGI("  %s: %s -> %s", v->mount_point, fstab_fstype, v->fs_type);
                free(fstab_fstype);
            }
        }
    }
    putc('\n', stdout);
#endif

    // Create /etc/fstab so tools like Busybox mount work
    FILE *file;
    file = fopen("/etc/mtab", "a");
    fclose(file);
    file = fopen("/etc/fstab", "w");
    if (file == NULL) {
        LOGW("Unable to create /etc/fstab!");
    }

    is_datamedia = 1;

    LOGI("recovery filesystem table");
    LOGI("=========================");
    for (i = 0; i < fstab->num_entries; ++i) {
        Volume* v = &fstab->recs[i];
        LOGI("  %d %s %s %s %lld", i, v->mount_point, v->fs_type,
                v->blk_device, v->length);

        if (is_volume_primary_storage(v)) {
            is_datamedia = 0;
        }

        write_fstab_entry(v, file);
    }

    if (file != NULL)
        fclose(file);

    putc('\n', stdout);
   // setup_uevent();
}

Volume* volume_for_mount_point(const std::string& mount_point) {
  return fs_mgr_get_entry_for_mount_point(fstab, mount_point);
}


// Finds the volume specified by the given path. fs_mgr_get_entry_for_mount_point() does exact match
// only, so it attempts the prefixes recursively (e.g. "/cache/recovery/last_log",
// "/cache/recovery", "/cache", "/" for a given path of "/cache/recovery/last_log") and returns the
// first match or nullptr.
Volume* volume_for_path(const char* path) {
  if (path == nullptr || path[0] == '\0') return nullptr;
  std::string str(path);
  while (true) {
    Volume* result = fs_mgr_get_entry_for_mount_point(fstab, str);
    if (result != nullptr || str == "/") {
      return result;
    }
    size_t slash = str.find_last_of('/');
    if (slash == std::string::npos) return nullptr;
    if (slash == 0) {
      str = "/";
    } else {
      str = str.substr(0, slash);
    }
  }
  return nullptr;
}

Volume* volume_for_label(const char* label) {
  int i;
  for (i = 0; i < get_num_volumes(); i++) {
    Volume* v = get_device_volumes() + i;
    if (v->label && !strcmp(v->label, label)) {
      return v;
    }
  }
  return nullptr;
}
char* get_real_fstype(Volume* rec) {
if (strcmp(rec->fs_type, "ext4") == 0 || strcmp(rec->fs_type, "f2fs") == 0 ||
     strcmp(rec->fs_type, "vfat") == 0) {
    char* detected_fs_type = blkid_get_tag_value(nullptr, "TYPE", rec->blk_device);
    if (detected_fs_type == nullptr) {
      return nullptr;
    }
    return detected_fs_type;
  }
 return nullptr;
}

// Mount the volume specified by path at the given mount_point.
int ensure_path_mounted_at(const char* path, const char* mount_point) {
  Volume* v = volume_for_path(path);
  if (v == nullptr) {
    LOG(ERROR) << "unknown volume for path [" << path << "]";
    return -1;
  }
  if (strcmp(v->fs_type, "ramdisk") == 0) {
    // The ramdisk is always mounted.
    return 0;
  }

  if (!mount_point) {
    mount_point = v->mount_point;
  }

  if (!fs_mgr_is_voldmanaged(v)) {
    if (is_path_mounted(mount_point)) {
      // volume is already mounted
      return 0;
    }
  }

  fs_mkdirs(mount_point, 0755);  // in case it doesn't already exist

  if (strcmp(v->fs_type, "ext4") == 0 || strcmp(v->fs_type, "squashfs") == 0 ||
      strcmp(v->fs_type, "vfat") == 0 || strcmp(v->fs_type, "f2fs") == 0) {
    int result = mount(v->blk_device, mount_point, v->fs_type, v->flags, v->fs_options);
    if (result == -1) {
      PLOG(ERROR) << "Failed to mount " << mount_point;
      return -1;
    }
    return 0;
  }

  LOG(ERROR) << "unknown fs_type \"" << v->fs_type << "\" for " << mount_point;
  return -1;
}

int ensure_volume_mounted(Volume* v) {
  if (v == nullptr) {
    LOG(ERROR) << "cannot mount unknown volume";
    return -1;
  }
  return ensure_path_mounted_at(v->mount_point, nullptr);
}

int ensure_path_mounted(const char* path) {
  // Mount at the default mount point.
  return ensure_path_mounted_at(path, nullptr);
}

int ensure_path_unmounted(const char* path) {
  const Volume* v = volume_for_path(path);
  if (v == nullptr) {
    LOG(ERROR) << "unknown volume for path [" << path << "]";
    return -1;
  }
  if (strcmp(v->fs_type, "ramdisk") == 0) {
    // The ramdisk is always mounted; you can't unmount it.
    return -1;
  }

  if (!is_path_mounted(v->mount_point)) {
      // volume is already mounted
      return 0;
    }

  return umount(v->mount_point);
}

static int exec_cmd(const std::vector<std::string>& args) {
  CHECK_NE(static_cast<size_t>(0), args.size());

  std::vector<char*> argv(args.size());
  std::transform(args.cbegin(), args.cend(), argv.begin(),
                 [](const std::string& arg) { return const_cast<char*>(arg.c_str()); });
  argv.push_back(nullptr);

  pid_t child;
  if ((child = fork()) == 0) {
    execv(argv[0], argv.data());
    _exit(EXIT_FAILURE);
  }

  int status;
  waitpid(child, &status, 0);
  if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
    LOG(ERROR) << args[0] << " failed with status " << WEXITSTATUS(status);
  }
  return WEXITSTATUS(status);
}

static int64_t get_file_size(int fd, uint64_t reserve_len) {
  struct stat buf;
  int ret = fstat(fd, &buf);
  if (ret) return 0;

  int64_t computed_size;
  if (S_ISREG(buf.st_mode)) {
    computed_size = buf.st_size - reserve_len;
  } else if (S_ISBLK(buf.st_mode)) {
    uint64_t block_device_size = get_block_device_size(fd);
    if (block_device_size < reserve_len ||
        block_device_size > std::numeric_limits<int64_t>::max()) {
      computed_size = 0;
    } else {
      computed_size = block_device_size - reserve_len;
    }
  } else {
    computed_size = 0;
  }

  return computed_size;
}

int format_volume(const char* volume, const char* directory) {
  const Volume* v = volume_for_path(volume);
  if (v == nullptr) {
    LOG(ERROR) << "unknown volume \"" << volume << "\"";
    return -1;
  }
  if (strcmp(v->fs_type, "ramdisk") == 0) {
    LOG(ERROR) << "can't format_volume \"" << volume << "\"";
    return -1;
  }
  if (strcmp(v->mount_point, volume) != 0) {
    LOG(ERROR) << "can't give path \"" << volume << "\" to format_volume";
    return -1;
  }
  if (ensure_path_unmounted(v->mount_point) != 0) {
    LOG(ERROR) << "format_volume: Failed to unmount \"" << v->mount_point << "\"";
    return -1;
  }
  if (strcmp(v->fs_type, "ext4") != 0 && strcmp(v->fs_type, "f2fs") != 0) {
    LOG(ERROR) << "format_volume: fs_type \"" << v->fs_type << "\" unsupported";
    return -1;
  }

  // If there's a key_loc that looks like a path, it should be a block device for storing encryption
  // metadata. Wipe it too.
  if (v->key_loc != nullptr && v->key_loc[0] == '/') {
    LOG(INFO) << "Wiping " << v->key_loc;
    int fd = open(v->key_loc, O_WRONLY | O_CREAT, 0644);
    if (fd == -1) {
      PLOG(ERROR) << "format_volume: Failed to open " << v->key_loc;
      return -1;
    }
    wipe_block_device(fd, get_file_size(fd));
    close(fd);
  }

  int64_t length = 0;
  if (v->length > 0) {
    length = v->length;
  } else if (v->length < 0 ||
             (v->key_loc != nullptr && strcmp(v->key_loc, "footer") == 0)) {
    android::base::unique_fd fd(open(v->blk_device, O_RDONLY));
    if (fd == -1) {
      PLOG(ERROR) << "format_volume: failed to open " << v->blk_device;
      return -1;
    }
    length =
        get_file_size(fd.get(), v->length ? -v->length : CRYPT_FOOTER_OFFSET);
    if (length <= 0) {
      LOG(ERROR) << "get_file_size: invalid size " << length << " for "
                 << v->blk_device;
      return -1;
    }
  }

  if (fs_mgr_is_voldmanaged(v)) {
    LOG(ERROR) << "can't format vold volume \"" << volume << "\"";
    return -1;
  }

  if (strcmp(v->fs_type, "ext4") == 0) {
    static constexpr int kBlockSize = 4096;
    std::vector<std::string> mke2fs_args = {
      "/sbin/mke2fs_static", "-F", "-t", "ext4", "-b", std::to_string(kBlockSize),
    };

    int raid_stride = v->logical_blk_size / kBlockSize;
    int raid_stripe_width = v->erase_blk_size / kBlockSize;
    // stride should be the max of 8KB and logical block size
    if (v->logical_blk_size != 0 && v->logical_blk_size < 8192) {
      raid_stride = 8192 / kBlockSize;
    }
    if (v->erase_blk_size != 0 && v->logical_blk_size != 0) {
      mke2fs_args.push_back("-E");
      mke2fs_args.push_back(
          android::base::StringPrintf("stride=%d,stripe-width=%d", raid_stride, raid_stripe_width));
    }
    mke2fs_args.push_back(v->blk_device);
    if (length != 0) {
      mke2fs_args.push_back(std::to_string(length / kBlockSize));
    }

    int result = exec_cmd(mke2fs_args);
    if (result == 0 && directory != nullptr) {
      std::vector<std::string> e2fsdroid_args = {
        "/sbin/e2fsdroid_static",
        "-e",
        "-f",
        directory,
        "-a",
        volume,
        v->blk_device,
      };
      result = exec_cmd(e2fsdroid_args);
    }

    if (result != 0) {
      PLOG(ERROR) << "format_volume: Failed to make ext4 on " << v->blk_device;
      return -1;
    }
    return 0;
  }

  // Has to be f2fs because we checked earlier.
  static constexpr int kSectorSize = 4096;
  std::string cmd("/sbin/mkfs.f2fs");
  // clang-format off
  std::vector<std::string> make_f2fs_cmd = {
    cmd,
    "-d1",
    "-f",
    "-O", "encrypt",
    "-O", "quota",
    "-O", "verity",
    "-w", std::to_string(kSectorSize),
    v->blk_device,
  };
  // clang-format on
  if (length >= kSectorSize) {
    make_f2fs_cmd.push_back(std::to_string(length / kSectorSize));
  }

  int result = exec_cmd(make_f2fs_cmd);
  if (result == 0 && directory != nullptr) {
    cmd = "/sbin/sload.f2fs";
    // clang-format off
    std::vector<std::string> sload_f2fs_cmd = {
      cmd,
      "-f", directory,
      "-t", volume,
      v->blk_device,
    };
    // clang-format on
    result = exec_cmd(sload_f2fs_cmd);
  }
  if (result != 0) {
    PLOG(ERROR) << "format_volume: Failed " << cmd << " on " << v->blk_device;
    return -1;
  }
  return 0;
}

int format_volume(const char* volume) {
  return format_volume(volume, nullptr);
}

bool is_path_mounted(const char* mount_point) {
  bool ret = false;
  FILE * fp = fopen("/proc/mounts", "rb");
  
  if (fp) {
    int c = EOF;    
    do {
      c = 0;
      
      do {
        c = fgetc(fp);
        
        if (c == EOF) {
          goto done;
        }
        else if (isspace(c)) {
          break;
        }
      }
      while (c != EOF);
      
      char p[256];
      memset(p, 0, 256);
      int pl = 0;
      
      do {
        c = fgetc(fp);
        
        if (c == EOF) {
          goto done;
        }
        else if (isspace(c)) {
          break;
        }
        
        p[pl++] = c;
      }
      while (c != EOF);
      
      p[pl++] = 0;
      
      if (strcmp(p, mount_point) == 0) {
        ret = true;
        goto done;
      }
      
      do {
        c = fgetc(fp);
        
        if (c == EOF) {
          goto done;
        }
        else if (c == '\n') {
          break;
        }
      }
      while (c != EOF);
    }
    while (c != EOF);
    
  done:
    fclose(fp);
  }
  
  return ret;
}

static std::string primary_storage_path = "";
std::string get_primary_storage_path(void) {
    int i = 0;
    if (primary_storage_path.empty()) {
        primary_storage_path = "/sdcard";
        for (i = 0; i < fstab->num_entries; ++i) {
            Volume* v = &fstab->recs[i];
            if (is_volume_primary_storage(v)) {
                primary_storage_path = v->mount_point;
                break;
            }
        }
    }
    return primary_storage_path;
}


int is_primary_storage_voldmanaged(void) {
    Volume* v = volume_for_path(get_primary_storage_path().c_str());
    return fs_mgr_is_voldmanaged(v);
}

int get_num_extra_volumes(void) {
    int num = 0;
    int i;
    for (i = 0; i < get_num_volumes(); ++i) {
        Volume* v = get_device_volumes() + i;
        if (is_volume_extra_storage(v)) {
            num++;
        }
    }
    return num;
}

std::vector<std::string> get_extra_storage_paths(void) {
    std::vector<std::string> paths;
    int num_extra_volumes = get_num_extra_volumes();
    int i = 0;

    if (num_extra_volumes == 0)
        return paths;
        
    for (i = 0; i < get_num_volumes(); ++i) {
        Volume* v = get_device_volumes() + i;
        if (is_volume_extra_storage(v)) {
            paths.push_back(std::string(v->mount_point));
        }
    }
    return paths;
}

static std::string android_secure_path;
std::string get_android_secure_path(void) {
    if (android_secure_path.empty()) {
        android_secure_path = get_primary_storage_path() + "/.android_secure";
    }
    return android_secure_path;
}

// load_volume_table() must have been called at this stage, so we can use ensure_path_mounted()
// data partition must be mounted if we pass in argument mount = 0
// setup_data_media() is either called by:
//  - ensure_path_mounted() which will mount /data
//  - on recovery start AFTER load_volume_table() AND with mount = true argument (setup_data_media(mount = true))
//  - in show_advanced_menu() to toggle /data/media target
//  - in show_partition_menu() and show_format_ext4_or_f2fs_menu() after format /data to recreate sdcard link
void setup_data_media(int mount) {
    if (!is_data_media())
        return;

    if (mount) {
        int count = 0;
        int ret = -1;
        while (count < 5 && ret != 0) {
            usleep(500000); // wait before first trial to avoid busy device on recovery start
            ret = ensure_path_mounted("/data");
            ++count;
        }
        if (ret != 0) {
            LOGE("could not mount /data to setup /data/media path!");
            return;
        }
    }

    int i;
    std::string mount_point = "/sdcard";
    for (i = 0; i < get_num_volumes(); ++i) {
        Volume* vol = get_device_volumes() + i;
        if (strcmp(vol->fs_type, "datamedia") == 0) {
            mount_point = vol->mount_point;
            break;
        }
    }

    // recreate /data/media with proper permissions
    rmdir(mount_point.c_str());
    if (Pterodon::Framework::recursive_mkdir("/data/media/0", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH))
    symlink("/data/media/0", mount_point.c_str());

    if (mount) {
        int count = 0;
        int ret = -1;
        while (count < 5 && ret != 0) {
            ret = ensure_path_unmounted("/data");
            usleep(500000);
            ++count;
        }
        if (ret != 0)
            LOGE("could not unmount /data after /data/media setup");
    }
}

// handle /sdcard/ path when not defined in recovery.fstab
int is_data_media_volume_path(const char* path) {
    if (!is_data_media())
        return 0;

    Volume* v = volume_for_path(path);
    if (v != NULL)
        return strcmp(v->fs_type, "datamedia") == 0;

    if (strcmp(path, "/sdcard") == 0 || strncmp(path, "/sdcard/", 8) == 0)
        return 1;

    return 0;
}

/*   check if a specified path is on a extended file system 
*     btw. even if this function returns true, then we unfortunately
*     don't know which version of extended fs it is, since the magic
*     number is same for all of them
*     At least we can use this to know that we aren't running on f2fs
*     or something like that.
*/
bool Is_Extended_Fs(const std::string& path)
{
	struct statfs sfs;
	return !statfs(path.c_str(), &sfs) && sfs.f_type == 0xEF53;
}

/* check if a specific filesystem is supported by the kernel */
bool IsFilesystemSupported(const char* filesystem) {
//  if (strcmp(filesystem, "fuse") == 0 || strcmp(filesystem, "ntfs") == 0)
//  return true;
  bool ret = false;
  FILE * fp = fopen("/proc/filesystems", "rb");
  if (fp) {
    int c = EOF;
    do {
    char p[256];
    memset(p, 0, 256);
    int pl = 0;
    c = 0;
    
    c = fgetc(fp);
    if (c == EOF) {
          goto done;
        }
       
     if (!isspace(c))
     goto finish_line;
        
      do {
        c = fgetc(fp);
        
        if (c == EOF) {
          goto done;
        }
        else if (isspace(c)) {
          break;
        }
        
        p[pl++] = c;
      }
      while (c != EOF);
      
      p[pl++] = 0;
      
      if (strcmp(p, filesystem) == 0) {
        ret = true;
        goto done;
      }
      
      finish_line:
      do {
        c = fgetc(fp);
        
        if (c == EOF) {
          goto done;
        }
        else if (c == '\n') {
          break;
        }
      }
      while (c != EOF);
    }
    while (c != EOF);
    
  done:
    fclose(fp);
    return ret;
  }
return true;
}

// get partition size info (adapted from Dees_Troy - TWRP)
unsigned long long Total_Size = 0; // Overall size of the partition
unsigned long long Used_Size = 0; // Overall used space
unsigned long long Free_Size = 0; // Overall free space

// this needs the volume to be mounted by caller
// do not ensure path mounted here to avoid time loss when refreshing backup size during nandroid operations
int Get_Size_Via_statfs(const char* Path) {
    struct statfs st;
    Volume* volume;
    if (is_data_media_volume_path(Path))
        volume = volume_for_path("/data");
    else
        volume = volume_for_path(Path);
    if (NULL == volume) {
        LOGE("No volume found to get size from '%s'", Path);
        return -1;
    }

    if (volume->mount_point == NULL || statfs(volume->mount_point, &st) != 0) {
        LOGE("Unable to statfs for size '%s'", Path);
        return -1;
    }

    Total_Size = (unsigned long long)(st.f_blocks) * (unsigned long long)(st.f_bsize);
    Free_Size = (unsigned long long)(st.f_bfree) * (unsigned long long)(st.f_bsize);
    Used_Size = Total_Size - Free_Size;
    // LOGI("%s: tot=%llu, use=%llu, free=%llu\n", volume->mount_point, Total_Size, Used_Size, Free_Size); // debug
    return 0;
}

// alternate method for statfs (emmc, mtd, mtk...)
int Find_Partition_Size(const char* Path) {
    char line[512];
    char tmpdevice[1024];
    FILE* fp;
    Volume* volume;

    if (is_data_media_volume_path(Path))
        volume = volume_for_path("/data");
    else
        volume = volume_for_path(Path);

    if (volume == NULL) {
        LOGE("Failed to find partition size '%s'", Path);
        LOGE("  > invalid volume %s", Path);
        return -1;
    }

    // In this case, we'll first get the partitions we care about (with labels)
/*
    --> Start by checking if it is an MTK based device (cat /proc/dumchar_info)
    Part_Name    Size               StartAddr         Type   MapTo
    preloader    0x0000000000040000 0x0000000000000000   2   /dev/misc-sd
    dsp_bl       0x00000000005c0000 0x0000000000040000   2   /dev/misc-sd
    mbr          0x0000000000004000 0x0000000000000000   2   /dev/block/mmcblk0
    ebr1         0x0000000000004000 0x0000000000004000   2   /dev/block/mmcblk0p1
    pmt          0x0000000000400000 0x0000000000008000   2   /dev/block/mmcblk0
    nvram        0x0000000000500000 0x0000000000408000   2   /dev/block/mmcblk0
    seccfg       0x0000000000020000 0x0000000000908000   2   /dev/block/mmcblk0
    uboot        0x0000000000060000 0x0000000000928000   2   /dev/block/mmcblk0
    bootimg      0x0000000000600000 0x0000000000988000   2   /dev/block/mmcblk0
    recovery     0x0000000000600000 0x0000000000f88000   2   /dev/block/mmcblk0
    sec_ro       0x0000000000600000 0x0000000001588000   2   /dev/block/mmcblk0p2
    misc         0x0000000000060000 0x0000000001b88000   2   /dev/block/mmcblk0
    logo         0x0000000000300000 0x0000000001be8000   2   /dev/block/mmcblk0
    expdb        0x0000000000200000 0x0000000001ee8000   2   /dev/block/mmcblk0
    android      0x0000000020100000 0x00000000020e8000   2   /dev/block/mmcblk0p3
    cache        0x0000000020100000 0x00000000221e8000   2   /dev/block/mmcblk0p4
    usrdata      0x0000000020100000 0x00000000422e8000   2   /dev/block/mmcblk0p5
    fat          0x00000000854f8000 0x00000000623e8000   2   /dev/block/mmcblk0p6
    bmtpool      0x0000000001500000 0x00000000ff9f00a8   2   /dev/block/mmcblk0
    Part_Name:Partition name you should open;
    Size:size of partition
    StartAddr:Start Address of partition;
    Type:Type of partition(MTD=1,EMMC=2)
    MapTo:actual device you operate
*/
    fp = fopen("/proc/dumchar_info", "rt");
    if (fp != NULL) {
        while (fgets(line, sizeof(line), fp) != NULL) {
            char label[32], device[32];
            unsigned long size = 0;

            sscanf(line, "%s %lx %*x %*u %s", label, &size, device);

            // Skip header, annotation  and blank lines
            if ((strncmp(device, "/dev/", 5) != 0) || (strlen(line) < 8))
                continue;

            sprintf(tmpdevice, "/dev/");
            strcat(tmpdevice, label);
            if (volume->blk_device != NULL && strcmp(tmpdevice, volume->blk_device) == 0) {
                Total_Size = size;
                fclose(fp);
                return 0;
            }
        }

        fclose(fp);
    }

/*  It is not an MTK device entry:
    --> Try mtd / emmc devices (cat /proc/partitions):
    major minor #blocks name
    179  0 15388672 mmcblk0
    179  1    65536 mmcblk0p1
    179  2      512 mmcblk0p2
    179  3      512 mmcblk0p3
    179  4     2048 mmcblk0p4
    179  5      512 mmcblk0p5
    179  6    22528 mmcblk0p6
    179  7    22528 mmcblk0p7
*/
    int ret = -1;
    fp = fopen("/proc/partitions", "rt");
    if (fp != NULL) {
        // try to read blk_device link target for devices not using /dev/block/xxx in recovery.fstab
        char* mmcblk_from_link = readlink_device_blk(Path);
        while (ret && fgets(line, sizeof(line), fp) != NULL) {
            unsigned long major, minor, blocks;
            char device[512];

            if (strlen(line) < 7 || line[0] == 'm')
                continue;

            sscanf(line + 1, "%lu %lu %lu %s", &major, &minor, &blocks, device);
            sprintf(tmpdevice, "/dev/block/");
            strcat(tmpdevice, device);

            if (volume->blk_device != NULL && strcmp(tmpdevice, volume->blk_device) == 0) {
                Total_Size = blocks * 1024ULL;
                //LOGI("%s(%s)=%llu\n", Path, volume->blk_device, Total_Size); // debug
                ret = 0;
            } else if (mmcblk_from_link != NULL && strcmp(tmpdevice, mmcblk_from_link) == 0) {
                Total_Size = blocks * 1024ULL;
                ret = 0;
            }
        }

        fclose(fp);
        if (mmcblk_from_link != NULL)
            free(mmcblk_from_link);
    }

    if (ret != 0)
        LOGE("Failed to find partition size '%s'", Path);
    return ret;
}

// try to resolve link from blk_device to real /dev/block/mmcblk or /dev/block/mtdblock
// free by caller
char* readlink_device_blk(const char* Path)
{
    char* mmcblk_from_link = NULL;
    Volume* vol;
    if (is_data_media_volume_path(Path))
        vol = volume_for_path("/data");
    else
        vol = volume_for_path(Path);
    if (vol == NULL)
        return NULL;

    char buf[1024];
    ssize_t len = readlink(vol->blk_device, buf, sizeof(buf)-1);
    if (len == -1) {
        // LOGI("failed to get device mmcblk link '%s'\n", vol->blk_device);
        return NULL;
    }

    buf[len] = '\0';
    mmcblk_from_link = strdup(buf);
    if (mmcblk_from_link == NULL) {
        LOGE("readlink_device_blk: memory error");
    } else {
        LOGI("found device mmcblk link: '%s' -> '%s'", vol->blk_device, mmcblk_from_link);
    }

    return mmcblk_from_link;
}

// mount /cache and unmount all other partitions before installing zip file
int setup_install_mounts() {
    if (fstab == NULL) {
        LOGE("can't set up install mounts: no fstab loaded");
        return -1;
    }

    int i;
    for (i = 0; i < fstab->num_entries; ++i) {
        Volume* v = fstab->recs + i;

        // do not unmount vold managed devices (we need this for aroma file manager zip installer to be able to see the vold devices)
        if (fs_mgr_is_voldmanaged(v))
            continue;
        if (strcmp(v->mount_point, "/tmp") == 0 ||
                strcmp(v->mount_point, "/cache") == 0) {
            if (ensure_path_mounted(v->mount_point) != 0) return -1;

        } else {
            if (ensure_path_unmounted(v->mount_point) != 0) return -1;
        }
    }
    return 0;
}

/* by default: 
    - do not unmount /data if user requests unmounting /sdcard
    - do not format whole /data (include /data/media) when format /data is requested
      instead, do rm -rf with /data/media excluded
*/
static int data_media_preserved_state = 1;
void preserve_data_media(int val) {
    data_media_preserved_state = val;
}

int is_data_media_preserved() {
    return data_media_preserved_state;
}

bool is_filesystem(const char* filesystem) {
return ((strcmp(filesystem, "ext2") == 0) ||
(strcmp(filesystem, "ext3") == 0) ||
(strcmp(filesystem, "ext4") == 0) ||
(strcmp(filesystem, "f2fs") == 0) ||
(strcmp(filesystem, "ntfs") == 0) ||
(strcmp(filesystem, "exfat") == 0) ||
(strcmp(filesystem, "squashfs") == 0) ||
(strcmp(filesystem, "vfat") == 0) ||
(strcmp(filesystem, "yaffs2") == 0) ||
(strcmp(filesystem, "auto") == 0));
}

bool is_image(const char* image) {
return ((strcmp(image, "emmc") == 0) ||
(strcmp(image, "mtd") == 0) ||
(strcmp(image, "bml") == 0));
}

void setup_legacy_storage_paths(void) {
    // sdcard symlink is done in setup_data_media()
    if (is_data_media())
        return;

    std::string primary_path = get_primary_storage_path();
    if (primary_path != "/sdcard") {
        rmdir("/sdcard");
        symlink(primary_path.c_str(), "/sdcard");
    }
}

int has_datadata() {
    Volume *vol = volume_for_path("/datadata");
    return vol != NULL;
}

