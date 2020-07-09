/*
    Pterodon Recovery - Logger
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

//#include <cstdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/klog.h>
#include <unistd.h>
#include "framework.hpp"
#include "constants.h"
#include "pterologger.hpp"
#include "fs_manager.h"
#include "env_service.hpp"

static std::string current_log_io_path = "";

namespace Pterodon {

void Logger::RedirectStdIO(const std::string& new_log_io_path) {
	freopen(new_log_io_path.c_str(), "a", stdout); setbuf(stdout, NULL);
    freopen(new_log_io_path.c_str(), "a", stderr); setbuf(stderr, NULL);
    LOGI("Redirecting stdio to '%s'", new_log_io_path.c_str());
    current_log_io_path = Pterodon::Framework::DirName(new_log_io_path);
}

void Logger::WipeLogs(void) {
std::string cache_folder = Pterodon::Environment::GetVar(PTERO_ANDROID_CACHE);
if (cache_folder.empty()) return;
bool is_mounted = is_path_mounted(cache_folder.c_str());
if (!is_mounted && ensure_path_mounted(cache_folder.c_str()) != 0) return;
std::string full_path = cache_folder + "/recovery";
 if (Pterodon::Framework::DirExists(full_path)) {
   	Pterodon::Framework::delete_file(full_path + "/log");
       Pterodon::Framework::delete_file(full_path + "/last_log");
       Pterodon::Framework::delete_file(full_path + "/last_kmsg");
   }
   if (!is_mounted)
   ensure_path_unmounted(cache_folder.c_str());
}

/* CopyLogs("/sdcard/Pterodon/logs"); */
void Logger::CopyLogs(void) {
std::string cache_folder = Pterodon::Environment::GetVar(PTERO_ANDROID_CACHE);
if (cache_folder.empty()) {
LOGI("Unable to copy logs, environment variable for cache doesn't exist!");
return;
}
LOGI("Copying logs from %s to %s", current_log_io_path.c_str(), cache_folder.c_str());
bool is_mounted = is_path_mounted(cache_folder.c_str());
if (!is_mounted && ensure_path_mounted(cache_folder.c_str()) != 0) {
LOGI("Failed to copy logs, partition cannot be mounted!");
return;
}
   std::string full_path = cache_folder + "/recovery";
   if (!Pterodon::Framework::DirExists(full_path) && 
        !Pterodon::Framework::recursive_mkdir(full_path, 0777)) {
        return;
   }  
   std::string local_log_path, file = current_log_io_path;
   if (current_log_io_path != "/") file += "/";
   file += "recovery.log";   
    local_log_path = full_path + "/log";  
   if (Copy_Recovery_Log(file, local_log_path)) {
       chmod(local_log_path.c_str(), 0600);
       chown(local_log_path.c_str(), 1000, 1000);
     
    local_log_path = full_path + "/last_log";   
   if (Copy_Recovery_Log(file, local_log_path))
       chmod(local_log_path.c_str(), 0640);
   }
 
     local_log_path = full_path + "/last_kmsg";    
     if (Copy_Kernel_Log(local_log_path)) {
         chmod(local_log_path.c_str(), 0600);
         chown(local_log_path.c_str(), 1000, 1000);
       }
       
    if (!is_mounted)
   ensure_path_unmounted(cache_folder.c_str());
   
    sync();
}

bool Logger::Copy_Kernel_Log(const std::string& path) {
	LOGI("Copying kernel log to '%s'", path.c_str());
    const int log_length = klogctl(KLOG_SIZE_BUFFER, 0, 0);
    if (log_length < 1) {
    	LOGW("Failed to obtain size of the kernel log!");
        return false;
    }
    std::string log(log_length, 0);
    const int read = klogctl(KLOG_READ_ALL, &log[0], log_length);
    if (read < 0) {
    	LOGW("Failed to read kernel log!");
        return false;
    }
    log.resize(read);
    return Pterodon::Framework::WriteStringToFile(path, &log);
}

bool Logger::Copy_Recovery_Log(const std::string& src, const std::string& dst) {
  if (Pterodon::Framework::FileExists(dst))
  Pterodon::Framework::delete_file(dst);
  Pterodon::Framework::copy_file(src, dst);
  return true;
  
  static off_t tmplog_offset = 0;
  FILE* dest_fp;
  dest_fp = fopen(dst.c_str(), "ae");
  if (!dest_fp) {
    LOGW("Failed to open destination path '%s'", dst.c_str());
    return false;
   }
    FILE* source_fp = fopen(src.c_str(), "re");
    if (!source_fp) {
    tmplog_offset = 0;
    LOGW("Failed to open log file '%s'", src.c_str());
    fflush(dest_fp);
	fclose(dest_fp);
    return false;
    }
      fseeko(source_fp, tmplog_offset, SEEK_SET);  /* Since last write */
      char buf[4096];
      size_t bytes;
      while ((bytes = fread(buf, 1, sizeof(buf), source_fp)) > 0) {
        fwrite(buf, 1, bytes, dest_fp);
      }
      tmplog_offset = ftello(source_fp);
      fflush(source_fp);
	  fclose(source_fp);
	  fflush(dest_fp);
	  fclose(dest_fp);
      return true;
}


} // namespace Pterodon
