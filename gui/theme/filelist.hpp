/*
    Pterodon Recovery - file selection menu
    Copyright (C) <2019> ATGDroid <bythedroid@gmail.com>
    
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

#ifndef _PTERODON_FILEMANAGER_HPP_
#define _PTERODON_FILEMANAGER_HPP_

#include "aroma.h"
#include "FileManager.hpp"

using Pterodon::Gui::FileManager;

typedef struct{
  LIBAROMA_CONTROLP bar;
  LIBAROMA_CONTROLP menu;
  LIBAROMA_CONTROLP progress;
  LIBAROMA_CONTROLP buttonx_2_circle;
  LIBAROMA_CONTROLP buttony_3_circle;
  LIBAROMA_CONTROLP buttonz_3_circle;
  LIBAROMA_CONTROLP buttona_3_circle;
  LIBAROMA_CONTROLP buttonx_2_circle_2;
  LIBAROMA_CONTROLP buttony_3_circle_2;
  LIBAROMA_CONTROLP buttonz_3_circle_2;
  LIBAROMA_CONTROLP buttona_3_circle_2;
} SELECTMENU;

namespace Pterodon {
namespace Gui {

class FileList {
public:
  /*
    * Function name: Menu_Pool
    * Description:
    * @return void: 
    */
    static void Menu_Pool(LIBAROMA_WINDOWP win, const std::string& folder_path, const std::string& file_extension, SELECTMENU * var);
  /*
    * Function name: ShowMenu
    * Description: 
    * @param parent_id: 
    * @param rect: 
    * @return void: 
    */
    static std::string GetDisplayTime(const time_t ctime);
    static void ShowMenu(const std::string& folder_path = "/", const std::string& file_extension = "");
    static FileManager GetMenuList(LIBAROMA_WINDOWP win, SELECTMENU * var, const std::string& folder_path, const std::string& file_extension, LIBAROMA_CANVASP list_icon_folder, LIBAROMA_CANVASP list_icon_file, LIBAROMA_CANVASP list_icon_back);

}; // class FileManager

} // namespace Gui
} // namespace Pterodon


#endif  // _PTERODON_FILEMANAGER_HPP_
