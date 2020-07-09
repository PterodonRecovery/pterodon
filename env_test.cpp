/*
    Pterodon - env handler test
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

#include <string>
#include <stdlib.h>
#include <vector>
#include <chrono>
#include <unistd.h>
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fs.h>


#define PTERO_ANDROID_ROOT 1
#define PTERO_ANDROID_DATA 2
#define PTERO_EXTERNAL_STORAGE 3
#define PTERO_ANDROID_STORAGE 4
#define PTERO_EXECUTABLES_PATH 5
#define PTERO_LDLIB_PATH 6
#define PTERO_ANDROID_CACHE 7

#define PTERO_DT_NOTFOUND		99

static struct pterodon_env {
		uint8_t type;
		const char *varName;
		const std::string default_value;
	} table[] = {
		{ PTERO_ANDROID_ROOT, "ANDROID_ROOT", "/system" },
		{ PTERO_ANDROID_DATA, "ANDROID_DATA", "/data" },
		{ PTERO_ANDROID_CACHE, "ANDROID_CACHE", "/cache" },
		{ PTERO_EXTERNAL_STORAGE, "EXTERNAL_STORAGE", "/sdcard" },
		{ PTERO_ANDROID_STORAGE, "ANDROID_STORAGE", "/storage" },
		{ PTERO_EXECUTABLES_PATH, "PATH", "/sbin:/system/bin:/system/xbin:/vendor/bin" },
		{ PTERO_LDLIB_PATH, "LD_LIBRARY_PATH", "/sbin" },
		{ 0, 0, ""},
	};
	
bool stat_path(const std::string& path, struct stat &st) {
  if (lstat(path.c_str(), &st) != 0) {
    if (errno != ENOENT && errno != ENOTDIR)
    printf("Failed to lstat '%s' (%s)\n", path.c_str(), strerror(errno));
	return false;
	}
	return true;
}

unsigned Get_D_Type_By_Path(const std::string& path) {
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
            printf("Unable to detect d type of '%s'\n", path.c_str());
            return DT_UNKNOWN;
     }
  }
  

namespace Pterodon {
class Framework
{
public:
static bool FileExists(const std::string& path) {
switch (Get_D_Type_By_Path(path)) {
		case DT_REG:
		case DT_UNKNOWN:
		return true;
	    default:
	    return false;
	  }
  }
 };
} // namespace Pterodon

namespace Pterodon {
	
class pterodon_str_divider
{
public:
	pterodon_str_divider();
	~pterodon_str_divider() {}
	void Reset();
	bool Split(const std::string& input, const std::string& divider, std::string &output);
	private:
	size_t first_pos, second_pos;
};

} // namespace Pterodon

namespace Pterodon {

pterodon_str_divider::pterodon_str_divider() {
	Reset();
}

void pterodon_str_divider::Reset() {
	first_pos = 0; 
    second_pos = 0;
}

bool pterodon_str_divider::Split(const std::string& input, const std::string& divider, std::string &output) {
if (second_pos == std::string::npos || 
((first_pos = input.find_first_not_of(divider, second_pos)) == std::string::npos)) return false;
if (((second_pos = input.find(divider, first_pos)) != std::string::npos)) {
output = input.substr(first_pos, second_pos - first_pos);
return true;
}
output = input.substr(first_pos);
return true;
}

} // namespace Pterodon

namespace Pterodon {
	
class Environment
{
public:
    static std::string GetVar(uint8_t var);
    static std::string LocateExecutable(const std::string& name);
    static std::vector<std::string> LocateExecutableAll(const std::string& name);
    static bool ExecutableExists(const std::string& name);
    static void SetVar(uint8_t kEnv, const std::string& newVar);
    static void prefer_default_values(bool val);
    static std::string TranslatePath(const std::string& path);
};

} // namespace Pterodon

static bool use_default_value = true;

namespace Pterodon {

std::string Environment::GetVar(uint8_t kEnv) {
	const struct pterodon_env *environment = table;
	while (environment->type) {
		if (environment->type == kEnv) {
			if (use_default_value)
			return environment->default_value;
			const char* env = getenv(environment->varName);
			return (env || env[0] != '\0') ? std::string(env) : environment->default_value;
			}
			environment++;
		  }
		return "";
	  }
	
void Environment::SetVar(uint8_t kEnv, const std::string& newVar) {
	const struct pterodon_env *environment = table;
	while (environment->type) {
		if (environment->type == kEnv) {
			setenv(environment->varName, newVar.c_str(), 1);
			return;
			}
			environment++;
		  }
	  }


/* which $name */
std::string Environment::LocateExecutable(const std::string& name) {
for (const uint8_t env_type : {PTERO_EXECUTABLES_PATH, PTERO_LDLIB_PATH}) {
std::string path_env = GetVar(env_type);
if (path_env.empty())
continue;
pterodon_str_divider div;
std::string split_path, full_path;
while (div.Split(path_env, ":", split_path)) {
full_path = split_path + "/" + name;
if (Pterodon::Framework::FileExists(full_path))
return full_path;
}
}
return "";
}

/* which -a $name */
std::vector<std::string> Environment::LocateExecutableAll(const std::string& name) {
std::vector<std::string> ret;
for (const uint8_t env_type : {PTERO_EXECUTABLES_PATH, PTERO_LDLIB_PATH}) {
std::string path_env = GetVar(env_type);
if (path_env.empty())
continue;
pterodon_str_divider div;
std::string split_path, full_path;
while (div.Split(path_env, ":", split_path)) {
full_path = split_path + "/" + name;
if (Pterodon::Framework::FileExists(full_path))
ret.push_back(full_path);
}
}
return ret;
}

bool Environment::ExecutableExists(const std::string& name) {
return (!LocateExecutable(name).empty());
}

std::string Environment::TranslatePath(const std::string& path) {
	if (use_default_value) return path;
	const struct pterodon_env *environment = table;
	while (environment->type) {
		if (environment->default_value == path) {
			const char* env = getenv(environment->varName);
			return (env || env[0] != '\0') ? std::string(env) : environment->default_value;
			}
			environment++;
		  }
		return path;
  }

	
void Environment::prefer_default_values(bool val) 
{
	  use_default_value = val;
}
	
} // namespace Pterodon


class Timer
{
public:
void start() {
time = std::chrono::system_clock::now();
process_ended = false;
}
Timer(std::string process = "Process")
			: process_name(process)
		{
		start();
		}
virtual ~Timer() {
end();
}

public:
void end() {
if (!process_ended) {
std::chrono::duration<double> duration = std::chrono::system_clock::now() - time;
printf("%s took %.9f s.\n", process_name.c_str(), duration.count());
process_ended = true;
}
}
private:
std::string process_name;
bool process_ended;
std::chrono::time_point<std::chrono::system_clock> time;
};

int main(void) {
Timer lol("Executable exists()");
std::string env_sys = Pterodon::Environment::GetVar(PTERO_ANDROID_CACHE);
printf("env: %s\n", env_sys.c_str());
}



