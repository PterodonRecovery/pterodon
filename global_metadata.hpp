/*
    Pterodon - global storage metadata
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

#ifndef PTERODON_STORAGEMETADATA_HPP
#define PTERODON_STORAGEMETADATA_HPP

#include "metadata.hpp"
#include <string>
#include <sys/types.h>

namespace Pterodon {

class StorageMetadata {
public:
  /*
    * Function name: GetMetadata
    * Description: 
    * @param path: 
    * @return bool: 
    */
    static bool GetMetadata(const std::string& path);
  /*
    * Function name: SetIdentifiers
    * Description: 
    * @param path: 
    * @return void: 
    */
    static void SetIdentifiers(const std::string& path);
  /*
    * Function name: SetPermissions
    * Description: 
    * @param path: 
    * @return void: 
    */
    static void SetPermissions(const std::string& path);
  /*
    * Function name: SetPermissions
    * Description: 
    * @param mode: 
    * @return void: 
    */
    static void SetPermissions(const mode_t mode);
  /*
    * Function name: SetAccessTime
    * Description: 
    * @param path: 
    * @return void: 
    */
    static void SetAccessTime(const std::string& path);
  /*
    * Function name: SetAll
    * Description: 
    * @param path: 
    * @return void: 
    */
    static void SetAll(const std::string& path);
  /*
    * Function name: GetUserID
    * Description: 
    * @param GetUserID(void: 
    * @return uid_t: 
    */
    static uid_t GetUserID(void);
  /*
    * Function name: GetGroupID
    * Description: 
    * @param GetGroupID(void: 
    * @return gid_t: 
    */
    static gid_t GetGroupID(void);
  /*
    * Function name: SetUserID
    * Description: 
    * @param user_id: 
    * @return void: 
    */
    static void SetUserID(uid_t user_id);
  /*
    * Function name: SetGroupID
    * Description: 
    * @param group_id: 
    * @return void: 
    */
    static void SetGroupID(gid_t group_id);
  /*
    * Function name: GetAccessMode
    * Description: 
    * @param GetAccessMode(void: 
    * @return mode_t: 
    */
    static mode_t GetAccessMode(void);
  /*
    * Function name: GetSelinuxContext
    * Description: 
    * @param GetSelinuxContext(void: 
    * @return security_context_t: 
    */
    static security_context_t GetSelinuxContext(void);
  /*
    * Function name: GetSelinuxContext
    * Description: 
    * @param path: 
    * @return bool: 
    */
    static bool GetSelinuxContext(const std::string& path);
  /*
    * Function name: SetSelinuxContext
    * Description: 
    * @param path: 
    * @return bool: 
    */
    static bool SetSelinuxContext(const std::string& path);
private:
	static Pterodon::wrappers::metadata_handler metadata;
}; // class StorageMetadata

} // namespace Pterodon

#endif  // PTERODON_STORAGEMETADATA_HPP
