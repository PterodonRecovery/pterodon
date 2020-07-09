/*
    Pterodon Recovery - framework
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

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <vector>
#include <sys/ioctl.h>
#include <linux/fs.h>

#include "constants.h"
#include "framework.hpp"
#include "env_service.hpp"
#include "strdiv.hpp"
#include "roots.h"
#include "global_metadata.hpp"

extern "C" {
	#include "libcrecovery/common.h"
}


/*
* Pterodon Recovery Framework
* Created by ATGDroid (https://t.me/ATGDroid)
*
* This file includes a Framework class
* which is a primary place for all our main custom
* function calls.
* This class is expected to be documented and easy to
* understand as much as possible, so even a complete beginner
* should be able to understand every part of it.
* Every function should include few lines of comments which describe
* what it does and how to use it, would be nice to also include some example.
*/

namespace Pterodon {

/*
*   time using gettimeofday()
*   to get time in usec, we call timenow_usec() which will link here if clock_gettime fails
*/
long long Pterodon::Framework::gettime_usec(void) {
    struct timeval now;
    long long useconds;
    gettimeofday(&now, NULL);
    useconds = (long long)(now.tv_sec) * 1000000LL;
    useconds += (long long)now.tv_usec;
    return useconds;
}


/*
*   use clock_gettime for elapsed time
*   this is nsec precise + less prone to issues for elapsed time
*   unsigned integers cannot be negative (overflow): set error return code to 0 (1.1.1970 00:00)
*/
unsigned long long Pterodon::Framework::gettime_nsec(void) {
    struct timespec ts;
    static int err = 0;

    if (err) return 0;

    if (clock_gettime(CLOCK_MONOTONIC, &ts) < 0) {
        LOGI("clock_gettime(CLOCK_MONOTONIC) failed: %s", strerror(errno));
        ++err;
        return 0;
    }

    unsigned long long nseconds = (unsigned long long)(ts.tv_sec) * 1000000000ULL;
    nseconds += (unsigned long long)(ts.tv_nsec);
    return nseconds;
}

/* returns the current time in usec: */
long long Pterodon::Framework::timenow_usec(void) {
    /* try clock_gettime */
    unsigned long long nseconds;
    nseconds = gettime_nsec();
    if (nseconds == 0) {
        // LOGI("dropping to gettimeofday()");
        return gettime_usec();
    }

    return (long long)(nseconds / 1000ULL);
}

/* returns the current time in msec: */
long long Pterodon::Framework::timenow_msec(void) {
    /* first try using clock_gettime */
    unsigned long long nseconds;
    nseconds = gettime_nsec();
    if (nseconds == 0) {
        // LOGI("dropping to gettimeofday()");
        return (gettime_usec() / 1000LL);
    }

    return (long long)(nseconds / 1000000ULL);
}

bool Pterodon::Framework::copy_file(const std::string& src, const std::string& dest) {
        LOGI("Copying file '%s' to '%s'", src.c_str(), dest.c_str());
	    if (src == dest) {
        LOGI("source and destination files are same, skipping copy.");
        return false;
    }
    int dstfd, srcfd;
    bool ret = true;
    srcfd = TEMP_FAILURE_RETRY(open(src.c_str(), O_RDONLY
#ifdef O_BINARY
| O_BINARY
#endif
));
    if (srcfd < 0) {
    	LOGW("Failed to open source file '%s' (%s)", src.c_str(), strerror(errno));
        return false;
        }
    dstfd = TEMP_FAILURE_RETRY(open(dest.c_str(), O_WRONLY | O_CREAT | O_TRUNC
#ifdef O_BINARY
| O_BINARY
#endif
, 0644));
    if (dstfd < 0) {
    	LOGW("Failed to open destination file '%s' (%s)", dest.c_str(), strerror(errno));
        close(srcfd);
        return false;
    }
    ssize_t bytes_read = 0;
    /* we're on stack */
    char buf[BUFSIZ];
    do {
    /* use true as the last argument to let the function know
     * that we don't mind if read_all returns 0, which means that it
     * shouldn't be considered as error and that the function should also
     * in such case return this value. */
    bytes_read = read_all(srcfd, buf, BUFSIZ, true);
       if (bytes_read == 0) {
       	/* we're done, hopefully */
    	   break;
    }
       if (bytes_read < 0) {
    	   LOGW("Failed to read from file '%s' (%s)", src.c_str(), strerror(errno));
		   ret = false;
		   break;
		}
		/* this also handles EINTR */
	   if (!write_all(dstfd, buf, bytes_read)) {
		   LOGW("Failed to write to file '%s' (%s)", dest.c_str(), strerror(errno));
           ret = false;
		   break;
        }
    } while (true);
    close(dstfd);
    close(srcfd);
    return ret;
 }

/* Get first non existing object specified by path, increase the number until
*   specified path does not exists */
std::string Pterodon::Framework::Get_First_Entry(const std::string& path) {
std::string ret;
/* of course i don't expect us to get up to 1K,
*   but just in case */
for (int i = 0; i < 1000; i++) {
ret = path + std::to_string(i);
if (!PathExists(ret)) /* we found it, YAY! */
return ret;
}
return path;
}

/* calculates the optimal read/write block size for filesystem I/O */
size_t Pterodon::Framework::get_io_blksize(int fd) {
struct stat st;
/* don't bother with the increase of the default value,
* as far as i tested it 128KB seems to be fine  */
int ret = 128*1024;
if (fstat(fd, &st) < 0) {
LOGW("io_blksize: Failed to fstat fd! (%s)", strerror(errno));
return ret;
}
/* if the optimal read/write value from st_blksize is bigger than 128KB, then use it */
if (st.st_blksize > ret)
ret = st.st_blksize;
/* caller is expected to already log some current active process,
*   so that this info isn't completely useless                                     */
LOGI("Optimal read/write block size: %s", CalculateDisplaySize(ret).c_str());
return ret;
}

/*
* call unlink() on object in a filesystem specified by path and on failure
* only check if the errno was not set to ENOENT, which would mean that the
* failure wasn't due to the non existing object in a filesystem but due to the
* actual unlink() failure. */
bool Pterodon::Framework::delete_file(const std::string& path) {
  if (unlink(path.c_str()) < 0 && errno != ENOENT) {
  LOGW("Failed to unlink '%s' (%s)", path.c_str(), strerror(errno));
  return false;
  }
  return true;
}

/*
* call rmdir() on object in a filesystem specified by path and on failure
* only check if the errno was not set to ENOENT, which would mean that the
* failure wasn't due to the non existing object in a filesystem but due to the
* actual rmdir() failure */
bool Pterodon::Framework::delete_directory(const std::string& path) {
  if (rmdir(path.c_str()) < 0 && errno != ENOENT) {
  LOGW("Failed to rmdir '%s' (%s)", path.c_str(), strerror(errno));
  return false;
  }
  return true;
}

/*
* call rename() on object in a filesystem specified by "previous_path" / "new_path"
* and on failure only check if the errno was not set to ENOENT, which would mean that the
* failure wasn't due to the non existing object in a filesystem but due to the
* actual rename() failure 
*
* Warning: Rename can't be used to move file from 
* filesystem of type "a" to filesystem of type "b" */
bool Pterodon::Framework::rename_file(const std::string& previous_path, const std::string& new_path) {
  if (rename(previous_path.c_str(), new_path.c_str()) < 0 && errno != ENOENT) {
  LOGW("Failed to rename '%s' to '%s' (%s)", previous_path.c_str(), new_path.c_str(), strerror(errno));
  return false;
  }
  return true;
}

/*
* call symlink() on object in a filesystem specified by "previous_path" / "new_path"
* and on failure only check if the errno was not set to ENOENT, which would mean that the
* failure wasn't due to the non existing object in a filesystem but due to the
* actual symlink() failure  */
bool Pterodon::Framework::symlink_path(const std::string& previous_path, const std::string& new_path) {
  if (symlink(previous_path.c_str(), new_path.c_str()) < 0 && errno != ENOENT) {
  LOGW("Failed to symlink '%s' to '%s' (%s)", previous_path.c_str(), new_path.c_str(), strerror(errno));
  return false;
  }
  return true;
}

/*
*  Set SELinux file context on object in a filesystem specified by path
*  Don't check for errors here, to prevent log spam on file systems which don't support this */
/*
void Pterodon::Framework::set_file_context(const std::string& path, const std::string& context) {
  setfilecon(path.c_str(), (security_context_t)context.c_str());
}
*/




/*
* we don't use rename, so we are able to even move file from filesystem
* "a" to filesystem "b", otherwise rename would fail, so the only thing which we do
* is just copying a file from path "a" to path "b" and then deleting a file specified by path "a" */
bool Pterodon::Framework::move_file(const std::string& previous_path, const std::string& new_path) {
return (copy_file(previous_path, new_path) && delete_file(previous_path));
}

/*
* returns anything after the last "/" slash character in path,
* so for example "dog.jpg" from "/sdcard/pictures/animals/dog.jpg" */
std::string Pterodon::Framework::BaseName(const std::string& path) {
  size_t pos = path.find_last_of("/");
  if(pos == std::string::npos)
    return path;
  else if (pos == 0)
    return path.substr(1);
  else if (pos == path.size()-1) {
    std::string new_path = path;
    while (pos == new_path.size()-1) {
    new_path = std::string(new_path.begin(),new_path.end()-1);
    pos = new_path.find_last_of("/");
    }
    return new_path.substr(pos + 1);
  }
  return path.substr(pos + 1);
}

/*
* returns anything before the last "/" slash character in path,
* so for example "/sdcard/pictures/animals" from "/sdcard/pictures/animals/dog.jpg" */
std::string Pterodon::Framework::DirName(const std::string& path) {
  size_t pos = path.find_last_of("/");
  if(pos == std::string::npos)
    return ".";
  else if (pos == 0)
    return "/";
  else if (pos == path.size()-1) {
    std::string new_path = path;
    while (pos == new_path.size()-1) {
    new_path = std::string(new_path.begin(),new_path.end()-1);
    pos = new_path.find_last_of("/");
    }
    return new_path.substr(0, pos);
  }
  return path.substr(0, pos);
}

/*
* returns first directory entry of path,
* so for example "/sdcard" from "/sdcard/pictures/animals/dog.jpg" */
std::string Pterodon::Framework::RootName(const std::string& path) {
  size_t pos = path.find_first_of("/");
  if(pos == std::string::npos)
    return "/" + path;
  else if (pos > 0)
    return "/" + path.substr(0, pos);
  else if (pos == 0) {
  pos = path.find_first_of("/", 1);
  if (pos == std::string::npos)
  return path;
  else
  return path.substr(0, pos);
  }
  return "/"; /* shouldn't reach here, unless we are in different universe */
}

/*
* returns anything after the last "." dot character in path,
* so for example "zip" from "/sdcard/phoenix.zip" */
std::string Pterodon::Framework::ExtensionName(const std::string& path) {
const size_t pos = path.find_last_of(".");
return (pos != std::string::npos ? path.substr(pos + 1) : path);
}

/*
* call lstat() on object in a filesystem specified by path and if it exists
* then return the size of it in bytes (st_size), otherwise just return 0 */
size_t Pterodon::Framework::Get_Stat_Size_By_Path(const std::string& path) {
    struct stat st;
    return stat_path(path, st) ? st.st_size : 0;
}

/*
* call lstat() on object in a filesystem specified by path and if it exists,
* then return the protection mode of it (st_mode), otherwise return 0,
* since mode_t is just a unsigned typedef variable */
mode_t Pterodon::Framework::Get_Stat_Perms_By_Path(const std::string& path) {
    struct stat st;
    return stat_path(path, st) ? st.st_mode : 0;
}

/*
*   call lstat() on object in a filesystem specified by path and if it exists,
*   then check it's protection mode (st_mode) to see if the file is executable */
bool Pterodon::Framework::Is_File_Executable(const std::string& path) {
struct stat st;
return (stat_path(path, st) && st.st_mode & S_IXUSR);
}

/*
*   call lstat() on object in a filesystem specified by path and on failure
*   only check if the errno was not set to ENOENT or ENOTDIR, which would mean that the
*   failure wasn't due to a non existing object in a filesystem but due to the
*   actual lstat failure.
*
*    struct stat st;
*    if (stat_path("/bin", st))
*    printf("/bin exists");
*
*/
bool Pterodon::Framework::stat_path(const std::string& path, struct stat &st) {
  if (lstat(path.c_str(), &st) != 0) {
    if (errno != ENOENT && errno != ENOTDIR)
    LOGW("Failed to lstat '%s' (%s)", path.c_str(), strerror(errno));
	return false;
	}
	return true;
}

/*
* faster version of to_int(), use only when you're sure that the input is numeric,
* for example when reading some sysfs entry like a battery percentage
*
* For atoi: If the value of the result cannot be represented, the behavior is undefined. 
*
* function conditions: input >= 0 && input <= INT_MAX
*/
int Pterodon::Framework::to_uint(const char* str) {
	 int ret = 0;
    while(*str) {
        ret = (ret << 4) - (ret << 2) - (ret << 1) + (*str++ - '0');
    }
    return ret;
}


int Pterodon::Framework::to_int(const char* str) {
	int ret = 0, sign = 1;
	if( *str == '+' || *str == '-' ) {
       if( *str == '-' ) sign = -1;
       str++;
    }
    while(*str) {
        ret = (ret << 4) - (ret << 2) - (ret << 1) + (*str++ - '0');
    }
    return ret * sign;
}

size_t Pterodon::Framework::to_sizet(const std::string& str) {
std::stringstream sstream(str);
size_t result;
sstream >> result;
return result;
}

bool Pterodon::Framework::CreateUserFolder(const std::string local_path) {
std::string folder_path = Pterodon::Environment::GetVar(PTERO_ANDROID_DATA);
if (ensure_path_mounted(folder_path.c_str()) != 0)
return false;
folder_path += "/media/0";
if (!Pterodon::StorageMetadata::GetMetadata(folder_path)) {
if (!recursive_mkdir(folder_path, Pterodon::StorageMetadata::GetAccessMode(), Pterodon::StorageMetadata::GetUserID(), Pterodon::StorageMetadata::GetGroupID()))
return false;
}
if (local_path.empty()) return true;
if (!recursive_mkdir(folder_path + "/" + local_path, 0775, Pterodon::StorageMetadata::GetUserID(), Pterodon::StorageMetadata::GetGroupID()))
return false;

return true;
}

bool Pterodon::Framework::is_number(const char* s) {
const char* p = s;
if( *p == '+' || *p == '-' ) p++;
while (*p != '\0') {
if (*p != '.' && !((uint8_t)(*p) - '0' <= 9))
return false;
p++;
}
return true;
}

/*
* case insensitive C-string compare  */
int Pterodon::Framework::strcmpi(const char *str1, const char *str2) {
    int i = 0;
    int ret = 0;

    while (ret == 0 && str1[i] && str2[i]) {
        ret = tolower(str1[i]) - tolower(str2[i]);
        ++i;
    }

    return ret;
}

bool Pterodon::Framework::get_block_device_size(const std::string& path, uint64_t* block_size) {
  int fd = TEMP_FAILURE_RETRY(open(path.c_str(), O_RDONLY));
    if (fd < 0) {
    	*block_size = 0;
        LOGW("get_block_device_size: Failed to open block device '%s' (%s)", path.c_str(), strerror(errno));
        return false;
        }
      /* It's 2020, so use BLKGETSIZE64 */
  if ((ioctl(fd, BLKGETSIZE64, &block_size)) == -1) {
    LOGW("get_block_device_size: Failed to get block device size of '%s' (%s)", path.c_str(), strerror(errno));
    *block_size = 0;
    close(fd);
    return false;
  }
  close(fd);
  return true;
}

std::string Pterodon::Framework::CalculateSizeVariant(Size_Variants real_variant, Size_Variants specified_variant) {
struct size_variants_table {
		const int variant_enum;
		const std::string variant_str;
	} sizeVariantsTable[] = {
		{ UNKNOWNBYTE, "" },
		{ BYTE, "B" },
		{ KILOBYTE, "KB" },
		{ MEGABYTE, "MB" },
		{ GIGABYTE, "GB" },
		{ TERABYTE, "TB" },
		{ PETABYTE, "PB" },
		{ EXABYTE, "EB" },
		{ 0, ""},
};

   const struct size_variants_table *sizeVariants_ptr = sizeVariantsTable;
	while (sizeVariants_ptr->variant_enum) {
    if (specified_variant != UNKNOWNBYTE) {
       if (sizeVariants_ptr->variant_enum == real_variant) {
    	sizeVariants_ptr++;
        return sizeVariants_ptr->variant_str;
      }
    } else {
    	if (sizeVariants_ptr->variant_enum == real_variant)
        return sizeVariants_ptr->variant_str;
    }
      sizeVariants_ptr++;
    }
    return "";
}

std::string Pterodon::Framework::CalculateDisplaySize(const size_t &size, Size_Variants size_enum) {
/* SIZE_MAX: ((size_t) -1) 18446744073709551615 */
/* size_t can take max up to exactly 16 exa bytes */
char display_size[32];
if (1023U >= size)
/* Byte */
sprintf(display_size,"%zuB", size);
else if (1048575U >= size)
/* Kilobyte */
sprintf(display_size,"%9.2lf%s", (double)size / 1024, CalculateSizeVariant(KILOBYTE, size_enum).c_str());
else if (1073741823U >= size)
/* Megabyte */
sprintf(display_size,"%9.2lf%s", (double)size / 1024 / 1024, CalculateSizeVariant(MEGABYTE, size_enum).c_str());
else if (1099511627775U >= size)
/* Gigabyte */
sprintf(display_size,"%9.2lf%s", (double)size / 1024 / 1024 / 1024, CalculateSizeVariant(GIGABYTE, size_enum).c_str());
else if (1125899906842623U >= size)
/* Terabyte */
sprintf(display_size,"%9.2lf%s", (double)size / 1024 / 1024 / 1024 / 1024, CalculateSizeVariant(TERABYTE, size_enum).c_str());
else if (1152921504606846975U >= size)
/* Petabyte */
sprintf(display_size,"%9.2lf%s", (double)size / 1024 / 1024 / 1024 / 1024 / 1024, CalculateSizeVariant(PETABYTE, size_enum).c_str());
/* this is the maximum value of what size_t can handle, exactly 16,00EB */
else if (18446744073709551614U >= size)
/* Exabyte */
sprintf(display_size,"%9.2lf%s", (double)size / 1024 / 1024 / 1024 / 1024 / 1024 / 1024, CalculateSizeVariant(EXABYTE, size_enum).c_str());
else
/* bigger than SIZE_MAX */
return "(Too big to display)"; /* max. value of size_t was probably changed, so this function should be updated */

/* 
Byte (B)
Kilobyte (KB)
Megabyte (MB)
Gigabyte (GB)
Terabyte (TB)
Petabyte (PB)
Exabyte (EB)
... (Unsupported for now, may be i won't even live to this year, lol)
Zettabyte (ZB)
Yottabyte (YB)
...
...
...
...
*/

/* remove spaces and replace dot by comma */
char* ptr1 = display_size;
char* ptr2 = display_size;
  while(*ptr2 != 0) {
    *ptr1 = *ptr2++;
    if (*ptr1 == '.')
    *ptr1++ = ',';
     else if (*ptr1 != ' ')
      ptr1++;
  }
  *ptr1 = 0;
/* return us to the caller */
return std::string(display_size);
}

std::string Pterodon::Framework::ReadLineToString(const std::string& path) {  
    std::ifstream file(path.c_str());
    if(file.fail()) {
    LOGW("Failed to open file '%s'", path.c_str());
    return "";
    }
    std::string line;
    getline(file,line);
    return line;
}

/*
*     std::string data;
*     if (Pterodon::Framework::ReadFileToString("text.txt", &data))
*     //do something with "data"
*/
bool Pterodon::Framework::ReadFileToString(const std::string& path, std::string *buf) {
  /*
  *   open input stream as binary and use std::ios::ate to get us directly at the
  *   end of the input stream, so we can then get a full size of this stream by call to file.tellg()
  */
  std::ifstream file(path.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
  if (file.fail()) {
  LOGW("ReadFileToString: Failed to open '%s'", path.c_str());
  return false;
  }
  /* Clear any previous data in a std::string */
  //  (*buf).clear();
  std::ifstream::pos_type size = file.tellg();
  if (size < 0) {
  LOGW("ReadFileToString: tellg() failed on: '%s'", path.c_str());
  return false;
  } else
  LOGI("Reading file '%s' of size %s to buffer...", path.c_str(), CalculateDisplaySize(size).c_str());

  /* resize our std::string so it can fit
   *  the whole input stream  */
  (*buf).resize(size);
  /* go back at the begining of the stream */
  file.seekg(0, std::ios::beg);
  /* read the whole stream to our buffer */
  file.read(&(*buf)[0], (*buf).size());
  
  /*
   * I/O is closed by the object destructor of std::ifstream, so 
   * we don't have to call file.close() here
   */
  
  /* check for errors */
   return !file.bad();
}

bool Pterodon::Framework::WriteStringToFile(const std::string& path, const std::string *buf) {
  std::ofstream file(path.c_str(), std::ios::out | std::ios::binary);
  if (file.fail()) {
  LOGW("WriteStringToFile: Failed to open '%s'", path.c_str());
  return false;
  }

  /* write the whole buffer to our file */
  file.write(&(*buf)[0], (*buf).size());
  
  /*
   * I/O is closed by the object destructor of std::ofstream, so 
   * we don't have to call file.close() here
   */
  
  /* check for errors */
  return !file.bad();
}

bool Pterodon::Framework::AppendStringToFile(const std::string& path, std::string *buf) {
  std::ofstream file(path.c_str(), std::ios::out | std::ios::binary | std::ios::app);
  if (file.fail()) {
  LOGW("AppendStringToFile: Failed to open '%s'", path.c_str());
  return false;
  }

  /* write the whole buffer at the end of our file */
  file.write(&(*buf)[0], (*buf).size());
  
  /*
   * I/O is closed by the object destructor of std::ofstream, so 
   * we don't have to call file.close() here
   */
  
  /* check for errors */
  return !file.bad();
}

/* check if our integer points to a valid file descriptor */
bool Pterodon::Framework::Is_Valid_Fd(int fd)
{
#if defined(F_GETFD)
    return fcntl(fd, F_GETFD) != -1;
#elif defined(F_GETFL)
    return fcntl(fd, F_GETFL) != -1;
#else
    struct stat st;
    return fstat(fd, &st) == 0;
#endif
}

int Pterodon::Framework::exec_cmd(const std::string& command)
{
	/* /sbin should be before any other path in $PATH variable, since we
	    are in recovery, so /sbin is the preffered location here */
	std::string environment_path = Pterodon::Environment::LocateExecutable("sh");
	if (environment_path.empty()) {
	LOGI("Unable to locate shell for execution of '%s'", command.c_str());
	return (-1);
	}
	
    pid_t pid;
	switch(pid = fork())
	{
		case -1:
      /*
       * When fork() returns -1, an error happened
       * (for example, number of processes reached the limit).
       */
			LOGE("exec_cmd: fork failed (%s)", strerror(errno));
			return(-1);
		case 0:
	 /*
       * When fork() returns 0, we are in
       * the child process.
       */
			execl(environment_path.c_str(), "sh", "-c", command.c_str(), NULL);
			_exit(127);
    }
     /* 
       * When fork() returns a positive number, we are in the parent process
       * (the fork return value is the PID of the newly created child process).
       * Here we just use waitpid() to wait for this process to finish
       */
	int status;
	/* don't forget to handle EINTR */
	pid = TEMP_FAILURE_RETRY(waitpid(pid, (int *)&status, 0));
	if (pid < 0) {
	LOGW("exec_cmd: '%s' process failed with waitpid: %s", command.c_str(), strerror(errno));
	return (-1);
	}
	if (WIFEXITED(status)) {
    if (WEXITSTATUS(status) != 0) {
      LOGW("exec_cmd: '%s' process exited with status: %d", command.c_str(), WEXITSTATUS(status));
      return WEXITSTATUS(status); /* the real execution result */
    }
  } else if (WIFSIGNALED(status)) {
    LOGE("exec_cmd: '%s' process terminated by signal: %d", command.c_str(), WTERMSIG(status));
    return (-1);
  }
	return 0;
}

int Pterodon::Framework::exec_cmd(const std::string& command, std::string &result) {
	FILE* exec;
	char buffer[128];
	exec = __popen(command.c_str(), "r");
	if (!exec) /* most probably fork failed */
    return (-1); 
		while (fgets(buffer, 128, exec) != NULL) {
		    result.append(buffer);
		}
	return __pclose(exec);
}

/*
* Check if a path is a mountpoint
* This method is independent on /proc fs, but not able to detect bind mounts
*/
bool Pterodon::Framework::Is_MountPoint(const std::string& path) {
struct stat directory_stat, parent_stat;

/* Hmm, should this be handled by caller? */
// const std::string root_path = RootName(path);

/* Call stat on the directory and it's parent folder */
if (!stat_path(path, directory_stat) || !stat_path((path + "/.."), parent_stat))
      return false;

/*
* if directory and parent have different device ids, then the directory is a mount point
* or, if they refer to the same directory,
* then it's probably the root directory 
* and therefore a mountpoint
*/
if (directory_stat.st_dev != parent_stat.st_dev || 
    directory_stat.st_ino == parent_stat.st_ino)
     return true;
     
     return false; /* Directory is not mount point */
}

/* Check if a specific std::string exists in a file */
bool Pterodon::Framework::StringExistsInFile(const std::string& filepath, const std::string& word) {
/*
*  call std::ifstream on object in a filesystem specified by filepath.
*   If it exists and we were able to open it, then recursively go 
*   through all the lines in a file and search for our specified std::string in "word" argument.
*/
    std::ifstream file(filepath.c_str());
    if(file.fail()) {
    LOGW("StringExistsInFile: Failed to open file '%s' to check for '%s'", filepath.c_str(), word.c_str());
    return false;
    }
    std::string line;
    while (getline(file,line)) {
     if (line.find(word) != std::string::npos)
      return true;
        }
    return false;
}

/* convert celsius temperature value to fahrenheit */
int Pterodon::Framework::CelsiusToFahrenheit(const int& celsius) {
return ((int)((double)((9.0 / 5.0) * celsius + 32.0)));
}

/* convert fahrenheit temperature value to celsius */
int Pterodon::Framework::FahrenheitToCelsius(const int& fahrenheit) {
return ((int)((double)((5.0 / 9.0) * fahrenheit - 32.0)));
}

/* check if a d_type of object specified by path is equal to DT_REG (regular file) */
bool Pterodon::Framework::FileExists(const std::string& path) {
	switch (Get_D_Type_By_Path(path)) {
		case DT_REG:
		case DT_UNKNOWN:
		return true;
	    default:
	    return false;
	  }
}

/* check if a object specified by path exists in a filesystem */
bool Pterodon::Framework::PathExists(const std::string& path) {
	/* may be we're on filesystem where even stat isn't able to detect d_type */
	// return Get_D_Type_By_Path(path) != DT_UNKNOWN;
    struct stat st;
	return stat_path(path, st);
}

/* check if a d_type of object specified by path is equal to DT_DIR (directory) */
bool Pterodon::Framework::DirExists(const std::string& path) {
	switch (Get_D_Type_By_Path(path)) {
		case DT_DIR:
		return true;
		case DT_UNKNOWN:
		return PathExists(path + "/.");
	    default:
	    return false;
	  }
}

unsigned Pterodon::Framework::Get_D_Type_By_Path(const std::string& path) {
/*
#define	DT_UNKNOWN	 0
#define	DT_FIFO		 1
#define	DT_CHR		 2
#define	DT_DIR		 4
#define	DT_BLK		 6
#define	DT_REG		 8
#define	DT_LNK		10
#define	DT_SOCK		12
*/

/* 
* We use a custom value in header file of the framework
* class to let the caller know also about a possible stat_path() failure
* #define	PTERO_DT_NOTFOUND		99
*/
    struct stat st;
	if (!stat_path(path, st)) {
	/*  file does not exists or there was some error, which is anyway   */
    /*  already logged by stat_path(), so just return     */
	return PTERO_DT_NOTFOUND;
	}
	switch (st.st_mode & S_IFMT) {
        case S_IFREG:
        /* regular file */
            return DT_REG;
        case S_IFDIR:
        /* directory */
            return DT_DIR;
        case S_IFLNK:
        /* symbolic link */
            return DT_LNK;
        case S_IFBLK:
        /* block device */
            return DT_BLK;
        case S_IFIFO:
        /* fifo/pipe */
            return DT_FIFO;
        case S_IFSOCK:
        /* socket */
            return DT_SOCK;
        case S_IFCHR:
        /* character device */
            return DT_CHR;
        default:
        /* unknown object */
            /* should we even care? */
            LOGW("Unable to detect d type of '%s'", path.c_str());
            return DT_UNKNOWN;
     }
  }
  
/*
* Translate directory entry type to a real std::string
*/
std::string Pterodon::Framework::Convert_D_Type_To_String(const unsigned int d_type) {
     switch (d_type) {
           case DT_BLK:  
              return "block device";
           case DT_CHR:  
              return "character device";
           case DT_DIR:  
              return "directory";
           case DT_FIFO:  
              return "FIFO/pipe";
           case DT_LNK:  
              return "symlink";
           case DT_REG:  
              return "regular file";
           case DT_SOCK: 
              return "socket";
           case PTERO_DT_NOTFOUND:
              return "(No such file or directory)";
           default:
              return "(unknown directory type)";
           }
}

bool Pterodon::Framework::directory_include_devices(const std::string& path) {
   DIR* d;
	if ((d = opendir(path.c_str())) == NULL) {
          LOGW("Failed to opendir '%s' (%s)", path.c_str(), strerror(errno));
          return false;
       }
    std::string full_path;
	struct dirent* de;
    do {
    /* reset any previous defined error number, so we
     * can then check if it was changed */
    errno = 0;
    /*
	* readdir() returns NULL on error or at the end of the directory search, so
    * to handle the error we need to check if errno was changed
    */
	if (!(de = readdir(d))) {
		if (errno) {
			LOGW("Failed to readdir '%s' (%s)", path.c_str(), strerror(errno));
		}
		break;
		}
		   /* ignore "." and ".." entries, they are useless for us here */
				if ((de->d_name[0] == '.') && ((de->d_name[1] == 0) || ((de->d_name[1] == '.') && (de->d_name[2] == 0))))
                    continue;
                  
				/*
				 *  create a full path to the object, so we can then directly call lstat on it by
				 *  Get_D_Type_By_Path()
				*/
			    full_path = path + "/";
                full_path.append(de->d_name);
			/*
            * On some filesystems dirent isn't able to get the d_type of the existing
            * object in the filesystem, so we need to update the d_type value by calling
            * lstat() and then trying to get the true permission mode bits from st_mode, this is handled by Get_D_Type_By_Path()
            */
			if (de->d_type == DT_UNKNOWN &&
           ((de->d_type = Get_D_Type_By_Path(full_path)) ==
	       DT_UNKNOWN))
	       break; /* shouldn't ever happen, if it does, then good luck */
	
	       if ((de->d_type == DT_DIR && directory_include_devices(full_path)) || de->d_type == DT_BLK || de->d_type == DT_CHR) {
		   /* We found a device, YAY! */
		   closedir(d);
	       return true;
		}
	   } while (true);
	closedir(d);
	return false;       
}
  
/*
* Deletes everything in a directory except filenames which are equal to "except"
* argument. If you don't want to exclude anything then just pass NULL instead.
*/
bool Pterodon::Framework::recursive_rmdir(const std::string& path, const char* except) {
	DIR* d;
	if ((d = opendir(path.c_str())) == NULL) {
          LOGW("Failed to opendir '%s' (%s)", path.c_str(), strerror(errno));
          return false;
       }
    std::string full_path;
	struct dirent* de;
	do {
	 /* reset any previous defined error number, so we
     * can then check if it was changed */
	errno = 0;
	/*
	* readdir() returns NULL on error or at the end of the directory search, so
    * to handle the error we need to check if errno was changed
    */
	if (!(de = readdir(d))) {
		if (errno) {
			LOGW("Failed to readdir '%s' (%s)", path.c_str(), strerror(errno));
			goto failure;
		}
		break;
		}
		    /*
		    * ignore "." and ".." entries, they are useless for us and also
		    * check if the filename is equal to the "except" argument,
		    * in case that we don't want to delete some specific directory in this
		    * path. (For example when this function is called to recursively delete files under
            * /data, except /data/media)
            */
            
                if (((de->d_name[0] == '.') && ((de->d_name[1] == 0) || ((de->d_name[1] == '.') && (de->d_name[2] == 0)))) || (except && strcmp(de->d_name, except) == 0))
                    continue;
			
				/*
				 *  create a full path to the file, so we can then directly call lstat on it by
				 *  Get_D_Type_By_Path() */
			    full_path = path + "/";
                full_path.append(de->d_name);
            
            /*
            * On some filesystems dirent isn't able to get the d_type of the existing
            * object in the filesystem, so we need to update the d_type value by calling
            * lstat() and then trying to get true permission mode bits from st_mode, this is handled by Get_D_Type_By_Path()
            */
			if (de->d_type == DT_UNKNOWN &&
           ((de->d_type = Get_D_Type_By_Path(full_path)) ==
	       DT_UNKNOWN))
	       goto failure; /* shouldn't ever happen, if it does, then good luck */
	       if (de->d_type == DT_DIR) {
		   if (!recursive_rmdir(full_path))
                goto failure;
            } else {
              if (!delete_file(full_path))
		          goto failure;
		   }
	   } while (true);
	closedir(d);
	return true;
	
	failure:
	closedir(d);
	return false;
}

/*
* Deletes everything inside a directory including a directory itself
*/
bool Pterodon::Framework::recursive_rmdir(const std::string& path) {
return (recursive_rmdir(path, NULL) && delete_directory(path));
}


/*
* check if a c-string is numeric (0123456789)
*
* const char* str = "2048";
* if (Pterodon::Framework::is_numeric(str))
* printf("string '%s' is numeric!", str);
* else
* printf("string '%s' is not numeric!", str);
*/
bool Pterodon::Framework::is_numeric(const char* s) {
for (const char* p = s; *p != '\0'; p++) {
   if (!((uint8_t)(*p) - '0' <= 9))
        return false;
      }
       return true;
}

/*
* check if a c++ std::string is numeric (0123456789)
*
* const std::string str = "2048";
* if (Pterodon::Framework::is_numeric(str))
* printf("string '%s' is numeric!", str.c_str());
* else
* printf("string '%s' is not numeric!", str.c_str());
*/
bool Pterodon::Framework::is_numeric(const std::string& s) {
/* iterator method is faster */
// return !s.empty() && s.find_first_not_of("0123456789") == std::string::npos;
    if (s.empty())
    return false;
    
    std::string::const_iterator it = s.begin();
    while (it != s.end() && (uint8_t)(*it) - '0' <= 9) 
    ++it;
    return it == s.end();
}


/*
* basically a c++ version of strdup()
* we need this when we want to copy c++ std::string
* to non const c-string, since .c_str() will return const char*
*/
char *Pterodon::Framework::to_char(const std::string& s) {
char *ret = new char[s.length() + 1]; 
strcpy(ret, s.c_str());
return ret;
}

File_Compression Pterodon::Framework::Get_File_Compression(const std::string& path) {
	/* Open the archive file as binary */
    FILE *archive = fopen(path.c_str(), "rb");
    if (archive == NULL) {
        LOGW("Failed to open archive: %s", path.c_str());
        return UNKNOWN;
    }
    
    char buff[3];
    /* read first two bytes */
    fread(buff, 1, 2, archive);
    
    /* close the opened archive */
    if (fclose(archive) != 0) {
        LOGW("Failed to close archive: '%s'", path.c_str());
        return UNKNOWN;
    }
    
   
    /*
     * we check only for first 2 bytes, GZIP is slightly complicated to handle, since
     * there are different compression variants with different second bytes, such as 0x9e or 0x9d but i don't
     * think that we really need to care about these
     */
     const char bytes[12] = {0x1f, 0x8b,0x02, 0x21,0x42, 0x5a, 0xfd, 0x37, 0x89, 0x4c, 0x5d, 0x00};
     uint8_t i;
     
     /* compare the bytes from archive with our defined file magic bytes */
 for (i=0;i<sizeof(bytes);i+=2) {
   if (buff[0] == bytes[i] && buff[1] == bytes[i+1]) {
 switch (i) {
   case GZIP:
      return GZIP;
   case LZ4:
      return LZ4;
   case BZIP2:
      return BZIP2;
   case XZ:
      return XZ;
   case LZOP:
      return LZOP;
   case LZMA:
      return LZMA;
   default:
      break;
    }
   }
 }
 LOGW("Unable to detect compression format of '%s', extracted bytes: '%X/%X'", path.c_str(), buff[0], buff[1]);
 return UNKNOWN;
}

/*
*  check if our std::string starts with specified characters, for example
*  if "Android_Recovery" starts with "Android"
*  Pterodon::Framework::StartsWith("Android_Recovery", "Android");
*/
bool Pterodon::Framework::StartsWith(const std::string& start, const std::string& search) {
return (start.size() >= search.size() && start.compare(0, search.size(), search) == 0);
}

bool Pterodon::Framework::StartsWith(const char* start, const char* search) {
return (strncmp(start, search, strlen(search)) == 0);
}

/*
*  check if our std::string ends with specified characters, for example
*  if "Android_Recovery" ends with "Recovery"
*  Pterodon::Framework::EndsWith("Android_Recovery", "Recovery");
*/
bool Pterodon::Framework::EndsWith(const std::string& start, const std::string& search) {
return (start.size() >= search.size() && start.compare(start.size()-search.size(), search.size(), search) == 0);
}

bool Pterodon::Framework::EndsWith(const char* start, const char* search) {
return (strcmp(start + strlen(start) - strlen(search), search) == 0);
}


/* check if path is in ramdisk since volume_for_path() will be NULL on these */
bool Pterodon::Framework::is_path_ramdisk(const std::string& path) {
    return StartsWith(path, "/sbin/") || StartsWith(path, "/res/") || StartsWith(path, "/tmp/");
}


/* 
* Create a directory specified by path, if we failed and errno was set to EEXIST, then the 
* directory already exists and so we can just ignore this error
*/
bool Pterodon::Framework::create_dir(const std::string& path, const mode_t mode) {
if (mkdir(path.c_str(), mode) != 0 && errno != EEXIST) {
     LOGE("Failed to mkdir '%s' %s.", path.c_str(), strerror(errno));
     return false;
    }
    return true;
}

/*
* check if a specified time interval is passed between 2 calls to this function
* before calling, reset timer by passing 0
* when timer reached, it will return 1 AND will reset the timer
* next call will start again from that point without needing to pass 0 to reset */
static long long interval_passed_t_timer = 0;

int Pterodon::Framework::is_time_interval_passed(long long msec_interval) {
    long long t = timenow_msec();
    if (msec_interval != 0 && t - interval_passed_t_timer < msec_interval)
        return 0;

    interval_passed_t_timer = t;
    return 1;
}

/* Search for a regular file in directory specified by path and it's all sub directories    */
std::string Pterodon::Framework::find_file_in_path(const std::string& path, const char* filename) {
	DIR* d;
	if ((d = opendir(path.c_str())) == NULL) {
          LOGW("Failed to opendir '%s' (%s)", path.c_str(), strerror(errno));
          return "";
       }
    std::string full_path;
   /* We read directory entries two times, for the first time we only compare
    * the file names of regular files with our argument in filename.
    * For the second time we ignore everything except directories, on which we call the find_file_in_path()
    * function again to continue in checking for our file name.
    */
    bool its_rewind_time = true;
	struct dirent* de;
    do {
    /* reset any previous defined error number, so we
     * can then check if it was changed */
    errno = 0;
    /*
	* readdir() returns NULL on error or at the end of the directory search, so
    * to handle the error we need to check if errno was changed
    */
	if (!(de = readdir(d))) {
		if (errno) {
			LOGW("Failed to readdir '%s' (%s)", path.c_str(), strerror(errno));
			break;
		}
		if (its_rewind_time) {
		/* reset the position of the directory stream to which DIR* d refers to the beginning of the directory. */
		rewinddir(d);
		its_rewind_time = false;
		continue;
		}
		break;
		}
		   /* ignore "." and ".." entries, they are useless for us here */
			if (((de->d_name[0] == '.') && 
                 ((de->d_name[1] == 0) || 
                  ((de->d_name[1] == '.') && 
                  (de->d_name[2] == 0)))) || 
                   (its_rewind_time && 
                   strcmp(de->d_name, filename) != 0))
                  continue;

				/*
				 *  create a full path to the object, so we can then directly call lstat on it by
				 *  Get_D_Type_By_Path() */
			    full_path = path + "/";
                full_path.append(de->d_name);
			/*
            * On some filesystems dirent isn't able to get the d_type of the existing
            * object in the filesystem, so we need to update the d_type value by calling
            * lstat() and then trying to get the true permission mode bits from st_mode, 
            * this is handled by Get_D_Type_By_Path()
            */
			if (de->d_type == DT_UNKNOWN &&
           ((de->d_type = Get_D_Type_By_Path(full_path)) ==
	       DT_UNKNOWN))
	       break; /* shouldn't ever happen, if it does, then good luck */
	       if (de->d_type == DT_DIR) {
		   /* object is a directory */
		   if (!its_rewind_time) {
			full_path = find_file_in_path(full_path, filename);
		    if (!full_path.empty()) {
			/* We found the matching filename, YAY! */
            closedir(d);
            return full_path;
		   }
		}
            } else if (its_rewind_time && de->d_type == DT_REG) /* object is a file */
                          {   
         /* We found the matching filename, YAY! */
         closedir(d);
		 return full_path;
		}
	   } while (true);
	closedir(d);
	return "";
}

/*
* Create recursively all the parent directories as needed
*
* Pterodon::pterodon_str_divider will make sure to correctly split our path argument at every "/" slash character.
* After that we will only append the new part of a std::string argument with new slash character at
* a start to another already existing std::string (full_path) and call create_dir() to make sure that
* the directory is created if it does not already exists.
*/
bool Pterodon::Framework::recursive_mkdir(const std::string& path, mode_t mode) {
  Pterodon::pterodon_str_divider Divider;
  std::string local, full_path;
  while (Divider.Split(path, "/", local)) {
  	full_path += "/" + local;
      if (!create_dir(full_path, mode))
        return false;
    }
  return true;
}

/*
* Create recursively all the parent directories as needed
*
* Pterodon::pterodon_str_divider will make sure to correctly split our path argument at every "/" slash character.
* After that we will only append the new part of a std::string argument with new slash character at
* a start to another already existing std::string (full_path) and call create_dir() to make sure that
* the directory is created if it does not already exists.
*/
bool Pterodon::Framework::recursive_mkdir(const std::string& path, mode_t mode, uid_t uid, gid_t gid) {
  Pterodon::pterodon_str_divider Divider;
  std::string local, full_path;
  while (Divider.Split(path, "/", local)) {
  	full_path += "/" + local;
      if (!create_dir(full_path, mode))
            return false;
      chown(full_path.c_str(), uid, gid);
    }
  return true;
}

/*
* Split the entire std::string at a requested character and return it in a vector
*
* This function is also able to split for example line from file with multiple
* whitespaces
*/
std::vector<std::string> Pterodon::Framework::Split(const std::string& str, const std::string& delimiter) {
  Pterodon::pterodon_str_divider Divider;
  std::string local;
  std::vector<std::string> ret;
  while (Divider.Split(str, delimiter, local)) {
  ret.push_back(local);
  }
  return ret;
}

/*
* Fix trailing slashes in path
*/
void Pterodon::Framework::Split_Trailing_Slashes(std::string &str) {
  /* there isn't what to split */
  if (str.empty() || str.find("//") == std::string::npos)
  return;
  Pterodon::pterodon_str_divider Divider;
  std::string part_char, full_char;
  while (Divider.Split(str, "/", part_char)) {
  full_char += "/" + part_char;
  }
  str = full_char;
}

bool Pterodon::Framework::write_all(int fd, const void *buf, size_t count)
{
	while (count) {
		ssize_t tmp;
		errno = 0;
		tmp = write(fd, buf, count);
		if (tmp > 0) {
			count -= tmp;
			if (count)
				buf = (void *) ((char *) buf + tmp);
		} else if (errno == EAGAIN)	/* Try later */
			          usleep(250000);
		  else if (errno != EINTR) /* we got some real write error, so return false */
			               return false;
	}
	return true;
}

ssize_t Pterodon::Framework::read_all(int fd, void *buf, size_t count, bool allow_empty_fd)
{
	ssize_t ret;
	ssize_t c = 0;
	uint8_t tries = 0;

	memset(buf, 0, count);
	while (count > 0) {
		ret = read(fd, buf, count);
		if (ret <= 0) {
			if ((errno == EAGAIN || (!allow_empty_fd && ret == 0)) &&
			/* 5 repeated calls is enough i guess */
			    (tries++ < 5)) {
				/* compared to EINTR we should also wait for a while
				    before the next request */
				usleep(250000);
				continue;
			} else if (errno == EINTR) continue;
			return c ? c : allow_empty_fd ? 0 : -1;
		}
		if (ret > 0)
			tries = 0;
		count -= ret;
		buf = (void *) ((char *) buf + ret);
		c += ret;
	}
	return c;
}

/*
* formats a std::string to be compliant with filenames standard and limits its length to max_length */
void Pterodon::Framework::FormatString(std::string &str, size_t max_length) {
    const std::string whitelist = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                              "abcdefghijklmnopqrstuvwxyz"
                                              "0123456789"
                                              "_-";
    size_t pos = 0;
    while ((pos = str.find_first_not_of(whitelist, pos)) != std::string::npos && 
                 pos <= max_length) {
        (str)[pos] = '_';
    }
    /* caller set some max. length of a std::string 
        to which we should resize it, so do it. */  
    if (max_length != std::string::npos)
    str.resize(max_length);
}

uint64_t Pterodon::Framework::Get_Directory_Stat_Size(const std::string& path) {
	DIR *d = opendir(path.c_str());
	if (!d) {
	LOGW("Failed to opendir '%s' (%s)", path.c_str(), strerror(errno));
    return 0;
	}
	struct dirent *de;
	struct stat st;
	std::string full_path;
	uint64_t full_size = 0;
	do {
	/* reset any previous defined error number, so we
     * can then check if it was changed */
	errno = 0;
	/* readdir() returns NULL on error or at the end of the directory search, so
     * to handle the error we need to check if the errno was changed */
	if (!(de = readdir(d))) {
		closedir(d);
		if (errno) {
			LOGW("Failed to readdir '%s' (%s)", path.c_str(), strerror(errno));
	        return 0;
		}
		return full_size;
		}
	       /* ignore "." and ".." entries, they are useless for us here */
			if ((de->d_name[0] == '.') && ((de->d_name[1] == 0) || ((de->d_name[1] == '.') && (de->d_name[2] == 0))))
                 continue;
             
          full_path = path + "/";
          full_path.append(de->d_name);
    /* On some filesystems dirent isn't able to get the d_type of the existing
     * object in the filesystem, so we need to update the d_type value by calling
     * lstat() and then trying to get the true permission mode bits from st_mode */
	if (de->d_type == DT_UNKNOWN) {
	if (!stat_path(full_path, st))
	continue;
	switch (st.st_mode & S_IFMT) {
        case S_IFREG:
        case S_IFLNK:
            full_size += (uint64_t)(st.st_size);
            continue;
        case S_IFDIR:
            full_size += Get_Directory_Stat_Size(full_path);
            continue;
        case S_IFCHR:
        case S_IFBLK:
        case S_IFIFO:
        case S_IFSOCK:
        continue;
        default:
            /* shouldn't ever happen, if it does, then good luck */
            LOGW("Unable to detect d_type of '%s'", full_path.c_str());
            continue;
     }
   }
     if (de->d_type == DT_DIR)
	full_size += Get_Directory_Stat_Size(full_path);
	else if (de->d_type == DT_REG || de->d_type == DT_LNK) {
	if (!stat_path(full_path, st))
	continue;
	full_size += (uint64_t)(st.st_size);
   }
  } while (true);
 }
 

std::string Pterodon::Framework::ListDirectoryToSettings(const std::string& path, unsigned file_type, const std::string& file_extension) {
   std::vector<std::string> ret;
   DIR* d;
	if ((d = opendir(path.c_str())) == NULL) {
          LOGW("Failed to opendir '%s' (%s)", path.c_str(), strerror(errno));
          return "";
       }
	struct dirent* de;
	std::string full_path;
    do {
    errno = 0;
    if (!(de = readdir(d))) {
		if (errno) {
			LOGW("Failed to readdir '%s' (%s)", path.c_str(), strerror(errno));
		}
		break;
		}
		   /* ignore "." and ".." entries, they are useless for us here */
			if ((de->d_name[0] == '.') && ((de->d_name[1] == 0) || ((de->d_name[1] == '.') && (de->d_name[2] == 0))))
                 continue;

             if (file_type != DT_UNKNOWN) {
				/*
				 *  create a full path to the object, so we can then directly call lstat on it by
				 *  Get_D_Type_By_Path() */
			    full_path = path + "/";
                full_path.append(de->d_name);
			/*
            * On some filesystems dirent isn't able to get the d_type of the existing
            * object in the filesystem, so we need to update the d_type value by calling
            * lstat() and then trying to get the true permission mode bits from st_mode, 
            * this is handled by Get_D_Type_By_Path()
            */
			if (de->d_type == DT_UNKNOWN &&
           ((de->d_type = Get_D_Type_By_Path(full_path)) ==
	       DT_UNKNOWN))
	       break; /* shouldn't ever happen, if it does, then good luck */
	       
	       if (de->d_type != file_type)
	       continue;
	}
	if (!file_extension.empty() && !EndsWith(de->d_name, file_extension.c_str()))
	    continue;
	std::string filename(de->d_name);
	ret.push_back(filename);
    } while (true);
  closedir(d);
  std::string fileList;
  for(std::vector<std::string>::const_iterator i = ret.begin(); i != ret.end(); ++i) {
    fileList += *i + ";";
}
  return fileList;
}


 
/*
Read a file to buffer: buffer must be freed by caller if return value is not NULL.
len is the total bytes we read if successful
That way we can use fwrite(str, len, fd) to copy the file
*/
char* Pterodon::Framework::read_file_to_buffer(const std::string& path, size_t *len) {
    ssize_t size = Get_Stat_Size_By_Path(path);
    if (size == 0) { /* file probably doesn't exist */
    LOGW("Unable to read '%s' to buffer, size of file is 0 bytes!", path.c_str());
    return NULL;
    }
    
    LOGI("Reading file '%s' of size '%s' to buffer", path.c_str(), CalculateDisplaySize(size).c_str()); 
    
    char* buffer = (char*) malloc(size + 1);
	if (!buffer) {
	LOGW("Unable to allocate enough memory...");
	return NULL;
	}
	
	   int fd = TEMP_FAILURE_RETRY(open(path.c_str(), O_RDONLY));
       if (fd < 0) {
    	LOGW("Failed to open source file '%s' (%s)", path.c_str(), strerror(errno));
        free(buffer);
        return NULL;
        }
        
        ssize_t read_len = read_all(fd, buffer, size, false);
        if (size != read_len) {
        LOGW("read_file_to_buffer: read error");
        free(buffer);
        close(fd);
        return NULL;
    }
    
    close(fd);
    *len = read_len;
	return buffer;
}

} // namespace Pterodon
