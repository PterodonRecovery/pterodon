/*
    Pterodon Recovery - AOSP property API wrapper
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


#ifndef _PTERODON_PROPERTYMANAGER_HPP
#define _PTERODON_PROPERTYMANAGER_HPP

#include <string>
#include "device_driver.hpp" // BootableDevice

namespace Pterodon {

class PropertyManager {
public:
     static bool GetProperty(const std::string& prop, std::string &prop_value, const std::string& default_value = "");
     static bool GetProperty(const std::string& prop, int &prop_value, const int default_value);
     static std::string GetProperty(const std::string& prop, const std::string& default_value = "");
     static bool GetBoolProperty(const std::string& prop, bool default_value = true);
     static int GetIntProperty(const std::string& prop, const int default_value);
     static bool PropertyExists(const std::string& prop);
     static bool SetProperty(const std::string& prop, const std::string& prop_value);
	 static bool FileSetProperty(const std::string& file_path, const std::string& prop, const std::string& value);
	 static bool FileGetProperty(const std::string& file_path, const std::string& prop, std::string &value, const std::string& default_value = "");
	 static bool get_kernel_command_line_property(const std::string& prop, std::string &value, const std::string& default_value = "");
	 static bool get_boot_property(const std::string& prop, std::string &value, const std::string& default_value = "");
	 static void SetPerfMode(bool enable = true);
	 static void SetAdbStatus(bool enable = true);
	 static void RequestReboot(BootableDevice device);
	 static std::string Derive_Build_Fingerprint(void);
};

} // namespace Pterodon

#endif // _PTERODON_PROPERTYMANAGER_HPP
		