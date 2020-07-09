/*
    Pterodon Recovery - env handler
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
#include "strdiv.hpp"
#include "env_service.hpp"
#include "constants.h"
#include "framework.hpp"
#include "version.hpp"

/* 
* communication bridge between the paths defined by
* environment variables and a recovery
* Created by ATGDroid (https://t.me/ATGDroid)
*/

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
		{ PTERO_TMP_FOLDER, "TMPDIR", "/tmp" },
		{ PTERO_VERSION, "PTERODON_VERSION", Pterodon::VersionManager::GetInfo(PTERODON_INFO_VERSION) },
		{ PTERO_CODENAME, "PTERODON_CODENAME", Pterodon::VersionManager::GetInfo(PTERODON_INFO_CODENAME) },
		{ 0, 0, ""},
	};

static bool use_default_value = false;

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
	
std::string Environment::GetVar(const std::string& varName) {
	        const char* env = getenv(varName.c_str());
			return (env || env[0] != '\0') ? std::string(env) : "";
}

bool Environment::VarExists(uint8_t kEnv) {
	const struct pterodon_env *environment = table;
	while (environment->type) {
		if (environment->type == kEnv) {
			const char* env = getenv(environment->varName);
			return (env || env[0] != '\0') ? true : false;
			}
			environment++;
		  }
		return false;
}

bool Environment::VarExists(const std::string& varName) {
			const char* env = getenv(varName.c_str());
			return (env || env[0] != '\0') ? true : false;
}

void Environment::InitializeVariables(void) {
const struct pterodon_env *environment = table;
	while (environment->type) {
    setenv(environment->varName, environment->default_value.c_str(), 0);
	environment++;
   }
}

void Environment::LogVariables(void) {
const struct pterodon_env *environment = table;
	while (environment->type) {
	//if (VarExists(environment->type))
	LOGI("'%s' = '%s'", environment->varName, GetVar(environment->type).c_str());
	environment++;
   }
}
	
void Environment::SetVar(uint8_t kEnv, const std::string& newVar) {
	const struct pterodon_env *environment = table;
	while (environment->type) {
		if (environment->type == kEnv) {
			setenv(environment->varName, newVar.c_str(), 1);
	     // if (environment->type == PTERO_TRUST_ZONE)
	       ///    tzset();
		       return;
			}
			environment++;
		  }
	  }

void Environment::SetVar(const std::string& varName, const std::string& varValue) {
			setenv(varName.c_str(), varValue.c_str(), 1);
			if (varName == "TZ") {
				 LOGI("Changing current timezone to %s", varValue.c_str());
			     tzset();
			}
}

void Environment::RemoveVar(uint8_t kEnv) {
	const struct pterodon_env *environment = table;
	while (environment->type) {
		if (environment->type == kEnv) {
			unsetenv(environment->varName);
			return;
			}
			environment++;
		  }
	  }
	
void Environment::RemoveVar(const std::string& varName) {
			unsetenv(varName.c_str());
}

/* which $name */
std::string Environment::LocateExecutable(const std::string& name) {
for (const uint8_t env_type : {PTERO_EXECUTABLES_PATH, PTERO_LDLIB_PATH, PTERO_TMP_FOLDER}) {
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
for (const uint8_t env_type : {PTERO_EXECUTABLES_PATH, PTERO_LDLIB_PATH, PTERO_TMP_FOLDER}) {
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
