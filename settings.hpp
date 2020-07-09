/*
    Pterodon Recovery - Settings manager
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

#ifndef _PTERODON_GLOBAL_SETTINGS_HPP
#define _PTERODON_GLOBAL_SETTINGS_HPP


#include "aroma_internal.h"
#include "constants.h"
#include "DataBase.hpp"
#include <string>

#define PTERODON_PERSIST_SETTINGS "/persist/pterosettings"
#define PTERODON_DATA_SETTINGS "/data/media/0/pterodon/pterosettings"

namespace Pterodon {

class SettingsManager
{
public:
	static void SaveSettings(void);
	static void LoadSettings(void);
	static bool LoadSettingsFile(const std::string& file_path);
	static void SaveSettingsFile(const std::string& file_path);
	static void SetDefaultSettings(void);
	static void ClearSettings(void);

	static std::string GetStrVar(const std::string& mVarName);
	static int GetIntVar(const std::string& mVarName);
	static float GetFloatVar(const std::string& mVarName);
	static bool Contains(const std::string& mVarName);
	static void ToggleVar(const std::string& mVarName);
	
	static void SetVar(const std::string& mVarName, const std::string& mVarValue, bool save_to_settings = false);
	static void SetVar(const std::string& mVarName, const int mVarValue, bool save_to_settings = false);
	static void SetVar(const std::string& mVarName, const float mVarValue, bool save_to_settings = false);
	static void SetVar(const std::string& mVarName, unsigned long long& mVarValue, bool save_to_settings = false);
	
	static bool GetVar(const std::string& mVarName, std::string& result);
	static bool GetVar(const std::string& mVarName, int& result);
	static bool GetVar(const std::string& mVarName, float& result);
	static bool GetVar(const std::string& mVarName, unsigned long long& result);
private:
    static Pterodon::wrappers::DataBase settingsRuntimeOnly;
    static Pterodon::wrappers::DataBase settingsSavedToFile;
	static LIBAROMA_MUTEX settings_locker;
};

} // namespace Pterodon

#endif // _PTERODON_GLOBAL_SETTINGS_HPP

