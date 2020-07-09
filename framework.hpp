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

#ifndef _PTERODON_FRAMEWORK_HPP
#define _PTERODON_FRAMEWORK_HPP

/*
 *   FRAMEWORK - CLASS WITH PTERODON RECOVERY ENVIRONMENT FUNCTIONS
 *   Namespace : Pterodon::Framework
 *   Maintainers : (ATGDroid)[https://t.me/ATGDroid]
 */

#include <string>
#include <vector>
#include <stdlib.h>

/* Custom value for stat_path failure on d_type detection */
#define PTERO_DT_NOTFOUND 99

enum File_Compression {
	UNKNOWN = 1,
	GZIP = 0,
	LZ4 = 2, /* May be also add LZ4 legacy? */
	BZIP2 = 4,
	XZ = 6,
	LZOP = 8,
	LZMA = 10
};

enum Size_Variants {
	UNKNOWNBYTE = 1,
	BYTE = 2,
	KILOBYTE = 3,
	MEGABYTE = 4,
	GIGABYTE = 5,
	TERABYTE = 6,
	PETABYTE = 7,
	EXABYTE = 8
};

/* Pterodon Recovery is where we live */
namespace Pterodon {

class Framework {
public:
  /*
    * Function name: gettime_usec
    * Description: 
    * @return long long: 
    */
    static long long gettime_usec(void);
  /*
    * Function name: gettime_nsec
    * Description: 
    * @return unsigned long long: 
    */
    static unsigned long long gettime_nsec(void);
  /*
    * Function name: timenow_usec
    * Description: 
    * @return long long: 
    */
    static long long timenow_usec(void);
  /*
    * Function name: timenow_msec
    * Description: 
    * @return long long: 
    */
    static long long timenow_msec(void);
  /*
    * Function name: copy_file
    * Description: Copy file from one path to another
    * @param src: Path to a file which we want to copy
    * @param dest: Destination path to which we want to copy our file
    * @return bool: true if file was successfully copied, false otherwise
    */
    static bool copy_file(const std::string& src, const std::string& dest);
  /*
    * Function name: Get_First_Entry
    * Description: Get first nonexistent entry for path in a filesystem
    * @param path: Path in a filesystem which might exist or not
    * @return std::string: Path in a filesystem which doesn't exist
    */
    static std::string Get_First_Entry(const std::string& path);
  /*
    * Function name: get_io_blksize
    * Description: Get optimal read/write speed for filesyste I/O
    * @param fd: file descriptor of our file
    * @return size_t: optimal size for filesystem I/O in bytes
    */
    static size_t get_io_blksize(int fd);
  /*
    * Function name: delete_file
    * Description: Delete file from filesystem
    * @param path: Path to a file which we want to delete
    * @return bool: true if file was deleted or it didn't exist, false otherwise
    */
    static bool delete_file(const std::string& path);
  /*
    * Function name: delete_directory
    * Description: Delete directory from filesystem
    * @param path: Path to a directory which we want to delete
    * @return bool: true if directory was deleted or it didn't exist, false otherwise
    */
    static bool delete_directory(const std::string& path);
  /*
    * Function name: rename_file
    * Description: Rename or move file in a filesystems of same type
    * @param previous_path: current path to a file
    * @param new_path: new path to a file
    * @return bool: returns true if file was renamed / moved or it didn't exist, false otherwise
    */
    static bool rename_file(const std::string& previous_path, const std::string& new_path);
  /*
    * Function name: symlink_path
    * Description: Symlink file from one path to another
    * @param previous_path: current path to a file
    * @param new_path: new path to a file
    * @return bool: returns true if file was symlinked, false otherwise
    */
    static bool symlink_path(const std::string& previous_path, const std::string& new_path);
  /*
    * Function name: set_file_context
    * Description: Set file context on file in a filesystem
    * @param path: Path of file in a filesystem to set context on
    * @param context: selinux context which we want to set on this file
    * @return void: There's not any return value, since some filesystems don't support file contexts at all
    */
    static void set_file_context(const std::string& path, const std::string& context);
  /*
    * Function name: move_file
    * Description: Move file in a filesystem from one location to another
    * @param previous_path: Current path to a file in a filesystem
    * @param new_path: New path of a file after it's moved
    * @return bool: returns true if file was moved, false otherwise
    */
    static bool move_file(const std::string& previous_path, const std::string& new_path);
  /*
    * Function name: BaseName
    * Description: Returns filename from full path to file
    * @param path: Path to a file
    * @return std::string: Filename of path
    */
    static std::string BaseName(const std::string& path);
  /*
    * Function name: DirName
    * Description: Returns a directory which is one level up from the current directory
    * @param path: Path to a file or directory
    * @return std::string: Path to a directory which is one level up from previous path
    */
    static std::string DirName(const std::string& path);
  /*
    * Function name: RootName
    * Description: Returns the first directory entry from path
    * @param path: Path to a file or directory
    * @return std::string: First entry from path ("/", "/sdcard", "/root")
    */
    static std::string RootName(const std::string& path);
  /*
    * Function name: ExtensionName
    * Description: Returns extension name from full path to file
    * @param path: Path to a file, for example (/sdcard/pterodon_recovery.zip)
    * @return std::string: Returns extension from a path to file, for example (zip)
    */
    static std::string ExtensionName(const std::string& path);
  /*
    * Function name: Get_Stat_Size_By_Path
    * Description:  Returns file size of file via call to stat()
    * @param path: Path to a file in filesystem
    * @return size_t: Size of file, might be 0 if file doesn't exist
    */
    static size_t Get_Stat_Size_By_Path(const std::string& path);
  /*
    * Function name: Get_Stat_Perms_By_Path
    * Description: Returns permissiom bits of file in a filesystem
    * @param path: Path to file in a filesystem
    * @return mode_t: Permission mode bits of specified file
    */
    static mode_t Get_Stat_Perms_By_Path(const std::string& path);
  /*
    * Function name: Is_File_Executable
    * Description: Checks whether a file in a filesystem is executable
    * @param path: Path to a file which exists in a filesystem
    * @return bool: true if file exists and is executable, false otherwise
    */
    static bool Is_File_Executable(const std::string& path);
  /*
    * Function name: stat_path
    * Description: Calls stat() on a file in a filesystem and in case of error, logs it
    * @param path: Path to file in a filesystem
    * @param &st: stat structure for informations about specified file
    * @return bool: true if file exist and there wasn't any error, false otherwise
    */
    static bool stat_path(const std::string& path, struct stat &st);
  /*
    * Function name: to_uint
    * Description: Convert char* to unsigned integer (char* must be unsigned!)
    * @param str: Unsigned value which we need to convert
    * @return int: unsigned integer value
    */
    static int to_uint(const char* str);
  /*
    * Function name: to_int
    * Description: Convert char* to integer
    * @param str: integer value which we need to convert
    * @return int: integer value
    */
    static int to_int(const char* str);
    
    static size_t to_sizet(const std::string& str);
  /*
    * Function name: is_number
    * Description: Checks whether string is a number
    * @param s: String which we need to check
    * @return bool: true if file is number, false otherwise
    */
    static bool is_number(const char* s);
  /*
    * Function name: strcmpi
    * Description: case insensitively compare C-string
    * @param *str1: First string to compare
    * @param *str2: Second string to compare
    * @return int: 0 if strings are same, -1 otherwise
    */
    static int strcmpi(const char *str1, const char *str2);
  /*
    * Function name: get_block_device_size
    * Description: Get size of block device via ioctl
    * @param path: Path to block device
    * @param block_size: Pointer to unsigned 64bit integer to save result
    * @return bool: true if size of block device was obtained, false otherwise
    */
    static bool get_block_device_size(const std::string& path, uint64_t* block_size);
  /*
    * Function name: CalculateDisplaySize
    * Description: Calculate properly formatted human size from bytes
    * @param &size: unsigned variable which we use for calculations
    * @return std::string: properly formatted human readable size
    */
    static std::string CalculateDisplaySize(const size_t &size, Size_Variants size_enum = UNKNOWNBYTE);
  /*
    * Function name: ReadFileToString
    * Description: Read contents of file in a filesystem to string buffer
    * @param path: Path to file in a filesystem which we want to read
    * @param *buf: pointer to buffer to which we will save the contents of file
    * @return bool: true if file contents was successfully saved to our buffer, false otherwise
    */
    static bool ReadFileToString(const std::string& path, std::string *buf);
    static std::string ReadLineToString(const std::string& path);
  /*
    * Function name: WriteStringToFile
    * Description: Write contents of string to a file in a filesystem
    * @param path: Path in a filesystem to which we want to write our string
    * @param *buf: Pointer to string buffer which contains contents which we want to write to a file
    * @return bool: true if contents of our string was successfully written to a file
    */
    static bool WriteStringToFile(const std::string& path, const std::string *buf);
  /*
    * Function name: AppendStringToFile
    * Description: Append string at the end of file in a filesystem
    * @param path: Path to file in a filesystem
    * @param *buf: Pointer to string buffer which contains contents which we want to append to a file
    * @return bool: true if contents of our string was successfully appended to a file
    */
    static bool AppendStringToFile(const std::string& path, std::string *buf);
  /*
    * Function name: Is_Valid_Fd
    * Description: Checks whether a file descriptor is opened
    * @param fd: file descriptor value
    * @return bool: true if file descriptor is opened/valid, false otherwise
    */
    static bool Is_Valid_Fd(int fd);
  /*
    * Function name: exec_cmd
    * Description: Execute command through shell (shell path might be based on environment variables)
    * @param command: shell commands which we want to execute
    * @return int: result value of this execution, 0 is usually equal to true
    */
    static int exec_cmd(const std::string& command);
  /*
    * Function name: exec_cmd
    * Description: Execute command through shell and save the result of this execution to string
    * @param command: shell commands which we want to execute
    * @param &result: pointer to string where we will save the result
    * @return int: result value of this execution, 0 is usually equal to true
    */
    static int exec_cmd(const std::string& command, std::string &result);
  /*
    * Function name: Is_MountPoint
    * Description: Checks whether a path is a mountpoint (this isn't able to detect bind mounts)
    * @param path: Path to directory to check whether it's a mountpoint
    * @return bool: true if path is s mountpoint, false otherwise
    */
    static bool Is_MountPoint(const std::string& path);
  /*
    * Function name: StringExistsInFile
    * Description: Checks whether a string exists in a file
    * @param filepath: Path to file in a filesystem
    * @param word: String to search for
    * @return bool: true if file exists and string exists, false otherwise
    */
    static bool StringExistsInFile(const std::string& filepath, const std::string& word);
  /*
    * Function name: CelsiusToFahrenheit
    * Description: 
    * @param celsius: 
    * @return int: 
    */
    static int CelsiusToFahrenheit(const int& celsius);
  /*
    * Function name: FahrenheitToCelsius
    * Description: 
    * @param fahrenheit: 
    * @return int: 
    */
    static int FahrenheitToCelsius(const int& fahrenheit);
  /*
    * Function name: FileExists
    * Description: 
    * @param path: 
    * @return bool: 
    */
    static bool FileExists(const std::string& path);
  /*
    * Function name: PathExists
    * Description: 
    * @param path: 
    * @return bool: 
    */
    static bool PathExists(const std::string& path);
  /*
    * Function name: DirExists
    * Description: 
    * @param path: 
    * @return bool: 
    */
    static bool DirExists(const std::string& path);
  /*
    * Function name: Get_D_Type_By_Path
    * Description: 
    * @param path: 
    * @return unsigned: 
    */
    static unsigned Get_D_Type_By_Path(const std::string& path);
  /*
    * Function name: Convert_D_Type_To_String
    * Description: 
    * @param d_type: 
    * @return std::string: 
    */
    static std::string Convert_D_Type_To_String(const unsigned int d_type);
  /*
    * Function name: directory_include_devices
    * Description: 
    * @param path: 
    * @return bool: 
    */
    static bool directory_include_devices(const std::string& path);
  /*
    * Function name: recursive_rmdir
    * Description: 
    * @param path: 
    * @param except: 
    * @return bool: 
    */
    static bool recursive_rmdir(const std::string& path, const char* except);
  /*
    * Function name: recursive_rmdir
    * Description: 
    * @param path: 
    * @return bool: 
    */
    static bool recursive_rmdir(const std::string& path);
  /*
    * Function name: is_numeric
    * Description: 
    * @param s: 
    * @return bool: 
    */
    static bool is_numeric(const char* s);
  /*
    * Function name: is_numeric
    * Description: 
    * @param s: 
    * @return bool: 
    */
    static bool is_numeric(const std::string& s);
  /*
    * Function name: *to_char
    * Description: 
    * @param s: 
    * @return char: 
    */
    static char *to_char(const std::string& s);
  /*
    * Function name: Get_File_Compression
    * Description: 
    * @param path: 
    * @return File_Compression: 
    */
    static File_Compression Get_File_Compression(const std::string& path);
  /*
    * Function name: StartsWith
    * Description: 
    * @param start: 
    * @param search: 
    * @return bool: 
    */
    static bool StartsWith(const std::string& start, const std::string& search);
  /*
    * Function name: StartsWith
    * Description: 
    * @param start: 
    * @param search: 
    * @return bool: 
    */
    static bool StartsWith(const char* start, const char* search);
  /*
    * Function name: EndsWith
    * Description: 
    * @param start: 
    * @param search: 
    * @return bool: 
    */
    static bool EndsWith(const std::string& start, const std::string& search);
  /*
    * Function name: EndsWith
    * Description: 
    * @param start: 
    * @param search: 
    * @return bool: 
    */
    static bool EndsWith(const char* start, const char* search);
  /*
    * Function name: is_path_ramdisk
    * Description: 
    * @param path: 
    * @return bool: 
    */
    static bool is_path_ramdisk(const std::string& path);
  /*
    * Function name: create_dir
    * Description: 
    * @param path: 
    * @param mode: 
    * @return bool: 
    */
    static bool create_dir(const std::string& path, const mode_t mode);
  /*
    * Function name: is_time_interval_passed
    * Description: 
    * @param msec_interval: 
    * @return int: 
    */
    static int is_time_interval_passed(long long msec_interval);
  /*
    * Function name: find_file_in_path
    * Description: 
    * @param path: 
    * @param filename: 
    * @return std::string: 
    */
    static std::string find_file_in_path(const std::string& path, const char* filename);
  /*
    * Function name: recursive_mkdir
    * Description: 
    * @param path: 
    * @param mode: 
    * @return bool: 
    */
    static bool recursive_mkdir(const std::string& path, mode_t mode);
  /*
    * Function name: Split
    * Description: 
    * @param str: 
    * @param delimiter: 
    * @return std::vector<std::string>: 
    */
    static std::vector<std::string> Split(const std::string& str, const std::string& delimiter);
  /*
    * Function name: Split_Trailing_Slashes
    * Description: 
    * @param &str: 
    * @return void: 
    */
    static void Split_Trailing_Slashes(std::string &str);
  /*
    * Function name: write_all
    * Description: 
    * @param fd: 
    * @param *buf: 
    * @param count: 
    * @return bool: 
    */
    static bool write_all(int fd, const void *buf, size_t count);
  /*
    * Function name: read_all
    * Description: 
    * @param fd: 
    * @param *buf: 
    * @param count: 
    * @param allow_empty_fd: 
    * @return ssize_t: 
    */
    static ssize_t read_all(int fd, void *buf, size_t count, bool allow_empty_fd);
  /*
    * Function name: FormatString
    * Description: 
    * @param &str: 
    * @param max_length: 
    * @return void: 
    */
    static void FormatString(std::string &str, size_t max_length);
  /*
    * Function name: Get_Directory_Stat_Size
    * Description: 
    * @param path: 
    * @return uint64_t: 
    */
    static uint64_t Get_Directory_Stat_Size(const std::string& path);
  /*
    * Function name: read_file_to_buffer
    * Description: 
    * @param path: 
    * @param *len: 
    * @return char*: 
    */
    static char* read_file_to_buffer(const std::string& path, size_t *len);
    static bool recursive_mkdir(const std::string& path, mode_t mode, uid_t uid, gid_t gid);
    static bool CreateUserFolder(const std::string local_path = "");
    static std::string ListDirectoryToSettings(const std::string& path, unsigned file_type, const std::string& file_extension);
    static std::string CalculateSizeVariant(Size_Variants real_variant, Size_Variants specified_variant);
}; // class Framework

} // namespace Pterodon

#endif  // _PTERODON_FRAMEWORK_HPP
