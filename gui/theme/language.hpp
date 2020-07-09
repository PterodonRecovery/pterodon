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

#ifndef _PTERODON_LANGUAGE_MANAGER_HPP
#define _PTERODON_LANGUAGE_MANAGER_HPP

#include <string>
#include <vector>
#include "aroma_internal.h"
#include "constants.h"
#include "DataBase.hpp"

namespace Pterodon {

class LanguageManager
{
public:
   static std::string parse_language_string(const std::string& string_variable, const std::string& default_value);
	static void ClearLanguages(void);
	static void LoadLanguages(const std::string& language_folder, const std::string& select_language = "");
	static void Print(void);
	static void SetLanguage(const std::string& language_name);
private:
    static Pterodon::wrappers::DataBase language;
    static std::string active_language;
	static LIBAROMA_MUTEX languages_locker;
};

} // namespace Pterodon

#endif // _PTERODON_LANGUAGE_MANAGER_HPP

