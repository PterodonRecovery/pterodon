/*
    Pterodon Recovery - Main GUI functions
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

#ifndef _PTERODON_GUI_HPP_
#define _PTERODON_GUI_HPP_

#include "aroma.h"
#include "device_driver.hpp"

namespace Pterodon {
namespace Gui {

class Core {
public:
  /*
    * Function name: dialog_demo
    * Description:
    * @return int: 
    */
    static int dialog_demo(LIBAROMA_WINDOWP parent);
    static int install_dialog(LIBAROMA_WINDOWP parent);
    static int theme_dialog(LIBAROMA_WINDOWP parent);
    static int reboot_dialog(LIBAROMA_WINDOWP parent, BootableDevice device);
    static int background_dialog(LIBAROMA_WINDOWP parent);
    static int select_font_dialog(LIBAROMA_WINDOWP parent);
    static int set_font_size(LIBAROMA_WINDOWP parent);
    static int show_buttons_menu(LIBAROMA_WINDOWP parent);
    static int show_display_menu(LIBAROMA_WINDOWP parent);
    static void init_display_menu(LIBAROMA_WINDOWP win, LIBAROMA_CONTROLP menu_list, bool init);
    static void init_buttons_menu(LIBAROMA_WINDOWP win, LIBAROMA_CONTROLP menu_list, bool init);
    static void init_date_menu(LIBAROMA_WINDOWP win, LIBAROMA_CONTROLP menu_list, bool init);
    static int show_date_menu(LIBAROMA_WINDOWP parent);
    static void init_statusbar_menu(LIBAROMA_WINDOWP win, LIBAROMA_CONTROLP menu_list, bool init);
    static int show_statusbar_menu(LIBAROMA_WINDOWP parent);
    static int show_memory_menu(LIBAROMA_WINDOWP parent);
    static int show_vibration_menu(LIBAROMA_WINDOWP parent);
    static int show_battery_menu(LIBAROMA_WINDOWP parent);
    
    //static int show_vibration_menu(LIBAROMA_WINDOWP parent);
  /*
    * Function name: init
    * Description:
    * @return void: 
    */
    static void init(void);
  /*
    * Function name: end
    * Description:
    * @return byte: 
    */
    static byte end();
  /*
    * Function name: start
    * Description: 
    * @return void: 
    */
    static void start(void);
}; // class Core

} // namespace Gui
} // namespace Pterodon

#endif  // _PTERODON_GUI_HPP_
