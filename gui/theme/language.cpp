/*
    Pterodon Recovery - Languages manager
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

#include "aroma_internal.h"
#include <time.h>
#include <string>
#include <unordered_map> // members of the unordered_map
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <cctype>
#include <dirent.h>
#include "settings.hpp"
#include "DataBase.hpp"
#include "framework.hpp"
#include "language.hpp"
#include "constants.h"

/* As per this post https://stackoverflow.com/questions/15636850/what-is-the-difference-between-pthread-recursive-mutex-initializer-and-pthread-r
we should prefer the variant with _NP, so this macro should handle it
*/
#ifdef PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
LIBAROMA_MUTEX Pterodon::LanguageManager::languages_locker = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
#else
LIBAROMA_MUTEX Pterodon::LanguageManager::languages_locker = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
#endif

std::string                                  Pterodon::LanguageManager::active_language;
Pterodon::wrappers::DataBase                             Pterodon::LanguageManager::language;

namespace Pterodon {

void LanguageManager::ClearLanguages(void)
{
	libaroma_mutex_lock(languages_locker);
	language.clear();
	libaroma_mutex_unlock(languages_locker);
}

void LanguageManager::LoadLanguages(const std::string& language_folder, const std::string& select_language) {
libaroma_mutex_lock(languages_locker);
language.clear();
	DIR* d;
	if ((d = opendir(language_folder.c_str())) == NULL) {
          LOGW("Failed to opendir '%s' (%s)", language_folder.c_str(), strerror(errno));
          libaroma_mutex_unlock(languages_locker);
          return;
       }
    std::string full_path, filename, language_name, language_full;
    std::string key, value;
    size_t pos_one, pos_two;

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
			LOGW("Failed to readdir '%s' (%s)", language_folder.c_str(), strerror(errno));
		}
		break;
		}
		   /* ignore "." and ".." entries, they are useless for us here */
				if ((de->d_name[0] == '.') && ((de->d_name[1] == 0) || ((de->d_name[1] == '.') && (de->d_name[2] == 0))))
                    continue;
                
                if (!Pterodon::Framework::EndsWith(de->d_name, ".xml"))
                     continue;
                  
				/*
				 *  create a full path to the object, so we can then directly call lstat on it by
				 *  Get_D_Type_By_Path()
				*/
				filename = de->d_name;
			    full_path = language_folder + "/" + filename;
			    
			/*
            * On some filesystems dirent isn't able to get the d_type of the existing
            * object in the filesystem, so we need to update the d_type value by calling
            * lstat() and then trying to get the true permission mode bits from st_mode, this is handled by Get_D_Type_By_Path()
            */
			if (de->d_type == DT_UNKNOWN &&
           ((de->d_type = Pterodon::Framework::Get_D_Type_By_Path(full_path)) ==
	       DT_UNKNOWN))
	       break; /* shouldn't ever happen, if it does, then good luck */
	
	   if (de->d_type != DT_REG)
	         continue;

pos_one = filename.find_last_of(".");
language_name = filename.substr(0, pos_one);
if (!select_language.empty() && select_language != language_name) continue;
language_full += filename + "=" + language_name + ";";
std::ifstream file(full_path.c_str());
if(file.fail()) {
    continue;
    }

std::string line;
while (std::getline(file,line)) {
pos_one = line.find_first_of("<");
if (pos_one == std::string::npos) continue;
if (pos_one != 0)
line = line.substr(pos_one);
if (!Pterodon::Framework::StartsWith(line, "<string")) continue;
pos_one = line.find_first_of("\"");
pos_one++;
pos_two = line.find_first_of("\"", pos_one);
key = line.substr(pos_one, pos_two - pos_one);
pos_two = line.find_first_of(">", pos_two);
pos_two++;
pos_one = line.find_last_of("<");
value = line.substr(pos_two, pos_one - pos_two);
key += "_" + language_name;
language.NewVar(key, value);
         }
	   } while (true);
	closedir(d);
	libaroma_mutex_unlock(languages_locker);
	Pterodon::SettingsManager::SetVar("pterodon_list_of_languages", language_full);
 }

void LanguageManager::Print(void) {
libaroma_mutex_lock(languages_locker);
for (std::unordered_map<std::string, std::string>::const_iterator it = language.begin(); it != language.end(); ++it) {
    LOGI("'%s' --> '%s'", it->first.c_str(), it->second.c_str());
 }
libaroma_mutex_unlock(languages_locker);
}

void LanguageManager::SetLanguage(const std::string& language_name)
{
	active_language = language_name;
	LoadLanguages("/pterodon/language", language_name);
	LOGI("Set new language: %s\n", language_name.c_str());
}

std::string LanguageManager::parse_language_string(const std::string& string_variable, const std::string& default_value) {
std::string ret;
libaroma_mutex_lock(languages_locker);
if (!language.GetVar(string_variable + "_" + active_language, ret))
     ret = default_value;
libaroma_mutex_unlock(languages_locker);
return ret;
}

} // namespace Pterodon
