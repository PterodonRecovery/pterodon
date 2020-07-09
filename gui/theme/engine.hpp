/*
    Pterodon Recovery - gui engine custom functions
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

#ifndef _PTERODON_ENGINE_HPP_
#define _PTERODON_ENGINE_HPP_

#include <string>
#include "aroma_internal.h"

#define PTEROGUI_BLACK_COLOR_MODE 0
#define PTEROGUI_WHITE_COLOR_MODE 1
#define PTEROGUI_BLUE_COLOR_MODE 2
#define PTEROGUI_RED_COLOR_MODE 3
#define PTEROGUI_GREEN_COLOR_MODE 4

#define PTEROGUI_BLACK_BACKGROUND 100
#define PTEROGUI_WHITE_BACKGROUND 101

namespace Pterodon {
namespace Gui {
	
class Engine {
public:
  /*
    * Function name: LoadImage
    * Description: 
    * @param image_name: 
    * @return LIBAROMA_CANVASP: 
    */
    static LIBAROMA_CANVASP LoadImage(const std::string& image_name);
  /*
    * Function name: LoadImage
    * Description: 
    * @param image_name: 
    * @param win: 
    * @return LIBAROMA_CANVASP: 
    */
    static LIBAROMA_CANVASP LoadImage(const std::string& image_name, LIBAROMA_WINDOWP win);
  /*
    * Function name: LoadFont
    * Description: 
    * @param font_name: 
    * @return void: 
    */
    static void LoadFont(const std::string& font_name);
  /*
    * Function name: fill_image_accent
    * Description: 
    * @param canvas: 
    * @return void: 
    */
    static void fill_image_accent(LIBAROMA_CANVASP canvas);
  /*
    * Function name: Screenshot
    * Description:
    * @return void: 
    */
    static void Screenshot(void);
  /*
    * Function name: menu_transition
    * Description:
    * @return void: 
    */
    static void menu_transition(
  LIBAROMA_CANVASP dst,
  LIBAROMA_CANVASP bottom,
  LIBAROMA_CANVASP top,
  float state,
  LIBAROMA_RECTP r1,
  LIBAROMA_RECTP r2
);
  /*
    * Function name: SetBarTitle
    * Description: 
    * @param title: 
    * @param subtitle: 
    * @return void: 
    */
    static void SetBarTitle(const char* title, const char* subtitle = NULL);
  /*
    * Function name: ShowSplash
    * Description: 
    * @param splash_image_name: 
    * @param time_to_sleep: 
    * @return void: 
    */
    static void ShowSplash(const std::string& splash_image_name);
    static void SetTheme(LIBAROMA_WINDOWP win);
    static void WindowInitHandler(LIBAROMA_WINDOWP win);
    static void DisplayDriver(bool enable);
    static byte MessageThreadHandler();
    static byte UIHandler();
    static void UpdateStatusbar(void);
    static byte MessageHandler(LIBAROMA_WMP win, LIBAROMA_MSGP msg);
    static bool IsDisplayEnabled(void);
    static void ShowDialog(const std::string& message_title, const std::string& message_description);
    static byte PowerMenuDialog();
    static void SetFontSize(byte size);
    static void SetWorkspace(int dp);
    static void UpdateTimeSettings(void);
    static std::string GetTime(int type);
    static std::string TranslateStatusbarSelection(int side);
    static void ResetFlashlightTimer(void);
    static byte DeviceInfoThreadHandler();
    
  static int DialogList(
  const char * title,
  std::string data_variable,
  int selected_option, 
  std::vector<std::string> *options);
    
    
    //static void WindowPool(LIBAROMA_WINDOWP win);
}; // class Engine

} // namespace Gui
} // namespace Pterodon

#endif  // _PTERODON_ENGINE_HPP_
