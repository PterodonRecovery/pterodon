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


#ifndef _PTERO_ENVHANDLER_HPP
#define _PTERO_ENVHANDLER_HPP

#include <string>
#include <stdlib.h>
#include <vector>

#define PTERO_ANDROID_ROOT 1
#define PTERO_ANDROID_DATA 2
#define PTERO_EXTERNAL_STORAGE 3
#define PTERO_ANDROID_STORAGE 4
#define PTERO_EXECUTABLES_PATH 5
#define PTERO_LDLIB_PATH 6
#define PTERO_ANDROID_CACHE 7
#define PTERO_TMP_FOLDER 8
#define PTERO_VERSION 9
#define PTERO_CODENAME 10

namespace Pterodon {
	
class Environment
{
public:
    static std::string GetVar(uint8_t var);
    static std::string GetVar(const std::string& varName);
    static bool VarExists(const std::string& varName);
    static std::string LocateExecutable(const std::string& name);
    static std::vector<std::string> LocateExecutableAll(const std::string& name);
    static bool ExecutableExists(const std::string& name);
    static void SetVar(uint8_t kEnv, const std::string& newVar);
    static void SetVar(const std::string& varName, const std::string& varValue);
    static void RemoveVar(const std::string& varName);
    static void prefer_default_values(bool val);
    static std::string TranslatePath(const std::string& path);
    static void InitializeVariables(void);
    static void LogVariables(void);
    static bool VarExists(uint8_t kEnv);
    static void RemoveVar(uint8_t kEnv);
};

} // namespace Pterodon

#endif // _PTERO_ENVHANDLER_HPP


