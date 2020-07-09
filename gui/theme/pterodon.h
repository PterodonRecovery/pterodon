/*
    Pterodon Recovery - Gui structure handler
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

#ifndef _pterodon_gui_macros_h
#define _pterodon_gui_macros_h

#include "aroma.h"
#include <string>

// #define PTEROGUI_RESOURCE_FOLDER "/pterodon"
#define PTEROGUI_SHOW_STATUSBAR 1
// #define PTEROGUI_STATUSBAR_DP 24

/* gui structure */
typedef struct{
  /* statusbar */
  std::string StatusbarLeftText;
  std::string StatusbarCenteredText;
  std::string StatusbarRightText;
  
  LIBAROMA_CANVASP Statusbar_Canvas;
  LIBAROMA_CANVASP NavigationBar_Canvas;
  word Statusbar_Prv_Color;
  word Statusbar_Req_Color;
  word Statusbar_Bg_Color;
  long Statusbar_Update_Start;
  byte color_mode;
  byte dark_mode;
  int max_display_brightness;
  int current_display_brightness;
  int device_vibration_level;
  
  /* main ui */
  LIBAROMA_WINDOWP  Window;
  LIBAROMA_WINDOWP  Sidebar;
  LIBAROMA_CONTROLP Bar;
  LIBAROMA_CONTROLP Tabs;
  LIBAROMA_CONTROLP Pager;
  LIBAROMA_CONTROLP Fragment;
  LIBAROMA_CONTROLP Sidemenu;
  int Statusbar_dp;
  byte Statusbar_Force_Update;
  byte Statusbar_Side_Opa;
  int Statusbar_Side_Width;
  LIBAROMA_CANVASP Statusbar_Overlay_Canvas;
} RECOVERYGUI, * RECOVERYGUIP;

RECOVERYGUIP Pterogui();

#endif // _pterodon_gui_macros_h