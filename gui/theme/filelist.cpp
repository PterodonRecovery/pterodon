/*
    Pterodon Recovery - filemanager menu
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
#include "engine.hpp"
#include <string>
#include "control_id.h"
#include "aroma.h"
#include "pterodon.h"
#include "statusbar.hpp"
#include "sidebar.hpp"
#include <dirent.h>
#include <sys/stat.h>
#include "framework.hpp"
#include "constants.h"
#include "FileManager.hpp"
#include "filelist.hpp"
#include "timer.hpp"
#include "windowpool.hpp"

namespace Pterodon {
namespace Gui {

std::string Pterodon::Gui::FileList::GetDisplayTime(const time_t ctime) {
char date[64];
strftime(date, 64, "%d-%m-%y %H:%M", localtime(&(ctime)));
return std::string(date);
}

FileManager Pterodon::Gui::FileList::GetMenuList(LIBAROMA_WINDOWP win, SELECTMENU * var, const std::string& folder_path, const std::string& file_extension, LIBAROMA_CANVASP list_icon_folder, LIBAROMA_CANVASP list_icon_file, LIBAROMA_CANVASP list_icon_back) {
/* menu flags */
  word menuflags =
    LIBAROMA_LISTITEM_MENU_INDENT_NOICON|
    LIBAROMA_LISTITEM_MENU_SMALL_ICON|
    LIBAROMA_LISTITEM_CHECK_SHARED_ICON|
    LIBAROMA_LISTITEM_WITH_SEPARATOR|
    LIBAROMA_LISTITEM_MENU_SMALL;
   
   /* 
    LIBAROMA_CONTROLP progress2 = libaroma_ctl_progress(
    win, 51,
    0, 0, 48, 48,
    LIBAROMA_CTL_PROGRESS_INDETERMINATE|LIBAROMA_CTL_PROGRESS_CIRCULAR,
    100,
    0
  );
  */
    
    LOGI("Listing folder: %s", folder_path.c_str()); 
    libaroma_listitem_caption_color(var->menu, 90, folder_path.c_str(), libaroma_colorget(NULL,win)->accent, -1);
    FileManager fileBrowser;
    if (!file_extension.empty())
    fileBrowser.SetExtension(file_extension);
    if (!fileBrowser.ChangeDirectory(folder_path)) {
  //  libaroma_window_del(win, progress2);
    return fileBrowser;
    }
    //Pterodon::Gui::Engine::BusyDialog();
   // libaroma_ctl_bar_set_title(var->bar, "Getting files", 1);
    std::vector<Pterodon::Gui::FileManager::ObjectData>::const_iterator iter;
    size_t i = 0, max_position = fileBrowser.size();
    libaroma_ctl_progress_max(var->progress,max_position);
    std::string objectDescription;
    //char bar_description[128];
    int percent_progress = 0, new_progress = 0;
#ifdef PTERODON_DEBUG_MODE
    Timer menu_timer("Listing objects on menu");
#endif
    for (iter = fileBrowser.folderBegin(); iter != fileBrowser.folderEnd(); iter++, i++) {
    	    LOGI("Folder: %zu: %s", 100+i, (*iter).fileName_.c_str());
    	    if ((*iter).fileName_ != "..") {
    	    objectDescription = "<" + Pterodon::Framework::Convert_D_Type_To_String((*iter).fileType_) + ">" + " " + GetDisplayTime((*iter).cTime_);
            libaroma_listitem_menu(var->menu, 100+i, (*iter).fileName_.c_str(), objectDescription.c_str(), list_icon_folder, menuflags, -1);
            } else {
           libaroma_listitem_menu(var->menu, 100+i, (*iter).fileName_.c_str(), objectDescription.c_str(), list_icon_back, menuflags, -1);
           }
           new_progress = (100*i)/max_position;
           if (percent_progress != new_progress) {
           percent_progress = new_progress;
           libaroma_ctl_progress_value(var->progress,percent_progress);
           }
        }
    for (iter = fileBrowser.fileBegin(); iter != fileBrowser.fileEnd(); iter++, i++) {
    	   LOGI("File: %zu: %s", 100+i, (*iter).fileName_.c_str());
    	   objectDescription = Pterodon::Framework::CalculateDisplaySize((*iter).fileSize_) + " <" + Pterodon::Framework::Convert_D_Type_To_String((*iter).fileType_) + ">" + " " + GetDisplayTime((*iter).cTime_);
           libaroma_listitem_menu(var->menu, 100+i, (*iter).fileName_.c_str(), objectDescription.c_str(), list_icon_file, menuflags, -1);
           new_progress = (100*i)/max_position;
           if (percent_progress != new_progress) {
           percent_progress = new_progress;
           libaroma_ctl_progress_value(var->progress,percent_progress);
           }
         }
        //libaroma_dialog_free(libaroma_wm_get_active_window());
        //libaroma_window_anishow(win,0,0);
        return fileBrowser;
}

/* init controls */
void Pterodon::Gui::FileList::Menu_Pool(LIBAROMA_WINDOWP win, const std::string& folder_path, const std::string& file_extension, SELECTMENU * var){
	var->bar = libaroma_ctl_bar(
    win, 30, 0, 0, LIBAROMA_SIZE_FULL, libaroma_dp(20),
    "Installation menu",
    libaroma_colorget(NULL, win)->control_bg,
    libaroma_colorget(NULL, win)->control_primary_text
  );
  
  var->progress = libaroma_ctl_progress(
    win, 31,
    0, libaroma_dp(20), LIBAROMA_SIZE_FULL, libaroma_dp(4),
    LIBAROMA_CTL_PROGRESS_DETERMINATE,
    100,
    0
  );
  
  libaroma_ctl_label_valign(win,32, "This operation may install <u>incompatible software</u>",
      0,
      libaroma_dp(28),
      LIBAROMA_SIZE_FULL,libaroma_dp(11),
      libaroma_colorget(NULL, win)->accent, 0,3,
      LIBAROMA_TEXT_CENTER|
      LIBAROMA_TEXT_BOLD|
      LIBAROMA_TEXT_ITALIC|
      LIBAROMA_TEXT_FIXED_INDENT|
      LIBAROMA_TEXT_NOHR,
      120,10
    );
    
    libaroma_ctl_label_valign(win, 33, "and render your device <u>unusable</u>.",
      0,
      libaroma_dp(40),
      LIBAROMA_SIZE_FULL,libaroma_dp(11),
      libaroma_colorget(NULL, win)->accent, 0,3,
      LIBAROMA_TEXT_CENTER|
      LIBAROMA_TEXT_BOLD|
      LIBAROMA_TEXT_ITALIC|
      LIBAROMA_TEXT_FIXED_INDENT|
      LIBAROMA_TEXT_NOHR,
      120,10
    );
    
    
    // progress y = 66
    // scroll list y = 181
  
  // x1 = 66
  // x2 = 181
  // difference = 115
  // 115 / 2 = 57
  
  /*
  int circle_button_size = libaroma_dp(26); 
  
  var->buttonx_2_circle = libaroma_ctl_button(
    win, 51, 0, 680, circle_button_size, circle_button_size,
    "FOLD", LIBAROMA_CTL_BUTTON_COLORED|LIBAROMA_CTL_BUTTON_RAISED|LIBAROMA_CTL_BUTTON_CIRCLE,
    libaroma_colorget(NULL,win)->control_bg
  );
  
  var->buttony_3_circle = libaroma_ctl_button(
    win, 52, circle_button_size, 680, circle_button_size, circle_button_size,
    "Y", LIBAROMA_CTL_BUTTON_COLORED|LIBAROMA_CTL_BUTTON_RAISED|LIBAROMA_CTL_BUTTON_CIRCLE,
    libaroma_colorget(NULL,win)->control_bg
  );
  
  var->buttonz_3_circle = libaroma_ctl_button(
    win, 53, circle_button_size*2, 680, circle_button_size, circle_button_size,
    "Z", LIBAROMA_CTL_BUTTON_COLORED|LIBAROMA_CTL_BUTTON_RAISED|LIBAROMA_CTL_BUTTON_CIRCLE,
    libaroma_colorget(NULL,win)->control_bg
  );
  
  var->buttona_3_circle = libaroma_ctl_button(
    win, 54, circle_button_size*3, 680, circle_button_size, circle_button_size,
    "A", LIBAROMA_CTL_BUTTON_COLORED|LIBAROMA_CTL_BUTTON_RAISED|LIBAROMA_CTL_BUTTON_CIRCLE,
    libaroma_colorget(NULL,win)->control_bg
  );
  
  
  
  
  var->buttonx_2_circle_2 = libaroma_ctl_button(
    win, 55, 0, 680+circle_button_size, circle_button_size, circle_button_size,
    "B", LIBAROMA_CTL_BUTTON_COLORED|LIBAROMA_CTL_BUTTON_RAISED|LIBAROMA_CTL_BUTTON_CIRCLE,
    libaroma_colorget(NULL,win)->control_bg
  );
  
  var->buttony_3_circle_2 = libaroma_ctl_button(
    win, 56, circle_button_size, 680+circle_button_size, circle_button_size, circle_button_size,
    "C", LIBAROMA_CTL_BUTTON_COLORED|LIBAROMA_CTL_BUTTON_RAISED|LIBAROMA_CTL_BUTTON_CIRCLE,
    libaroma_colorget(NULL,win)->control_bg
  );
  
  var->buttonz_3_circle_2 = libaroma_ctl_button(
    win, 57, circle_button_size*2, 680+circle_button_size, circle_button_size, circle_button_size,
    "D", LIBAROMA_CTL_BUTTON_COLORED|LIBAROMA_CTL_BUTTON_RAISED|LIBAROMA_CTL_BUTTON_CIRCLE,
    libaroma_colorget(NULL,win)->control_bg
  );
  
  var->buttona_3_circle_2 = libaroma_ctl_button(
    win, 58, circle_button_size*3, 680+circle_button_size, circle_button_size, circle_button_size,
    "E", LIBAROMA_CTL_BUTTON_COLORED|LIBAROMA_CTL_BUTTON_RAISED|LIBAROMA_CTL_BUTTON_CIRCLE,
    libaroma_colorget(NULL,win)->control_bg
  );
  */
  
  libaroma_ctl_bar_set_icon(
  var->bar,NULL,0,LIBAROMA_CTL_BAR_ICON_ARROW,0
  );
  
  libaroma_ctl_bar_set_icon_mask(var->bar,1,0);
  libaroma_ctl_bar_set_textgap(var->bar,1,0);
  
  libaroma_ctl_bar_set_touchable_title(var->bar,1);
  libaroma_ctl_bar_set_subtitle(var->bar, "Select file to install", 0);
    
  //Pterodon::Gui::Engine::text_draw(win, "Some random text", RGB(444444), LIBAROMA_SIZE_FULL, 140, LIBAROMA_SIZE_HALF, 80);
  
  /* create menu list */
  var->menu = libaroma_ctl_list(
    win, 34,
    LIBAROMA_SIZE_FULL, libaroma_dp(66), LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL, //libaroma_dp(182),
    0, 0,
    libaroma_colorget(NULL,win)->control_bg,
    LIBAROMA_CTL_SCROLL_WITH_SHADOW
    |LIBAROMA_CTL_SCROLL_WITH_HANDLE
  );
  
  LIBAROMA_CANVASP list_icon_folder = Pterodon::Gui::Engine::LoadImage("folder",win);
  LIBAROMA_CANVASP list_icon_file = Pterodon::Gui::Engine::LoadImage("file",win);
  LIBAROMA_CANVASP list_icon_back = Pterodon::Gui::Engine::LoadImage("direction",win);
  std::string current_folder = folder_path;
  
   Pterodon::Gui::FileManager fileList = GetMenuList(win, var, current_folder, file_extension, list_icon_folder, list_icon_file, list_icon_back);
   
   while (fileList.empty() && current_folder != "/") {
   current_folder = Pterodon::Framework::DirName(current_folder);
   fileList = GetMenuList(win, var, current_folder, file_extension, list_icon_folder, list_icon_file, list_icon_back);
   }
   
 
  byte onpool=1;
  libaroma_sync();
  LIBAROMA_MSG msg;
  libaroma_window_anishow(win, LIBAROMA_WINDOW_SHOW_ANIMATION_FADE, 200);
  Pterodon::Gui::WindowPooler pooler;
  do{
    pooler.Pool(win,&msg);
        
    if (pooler.cmd){
    if (msg.msg==LIBAROMA_MSG_BACK){
    onpool = 0;
    } else if (pooler.cmd==LIBAROMA_CMD_HOLD){
    if (pooler.id==34){
    size_t actual_list_id = msg.key - 100;
    LOGI("--- FileManager Touch: [%s] [%zu]",actual_list_id > (fileList.folderSize() - 1) ? "FILE" : "FOLDER", actual_list_id > (fileList.folderSize() - 1) ? (actual_list_id - fileList.folderSize()) : (actual_list_id));
    Pterodon::Gui::FileManager::ObjectData new_path = (actual_list_id > (fileList.folderSize() - 1) ? fileList.fileAt(actual_list_id - fileList.folderSize()) : fileList.folderAt(actual_list_id));
    int ret=libaroma_dialog_list(
    new_path.fileName_.c_str(),
    "OK",
    "Back",
    NULL,
    LIBAROMA_DIALOG_DIM_PARENT|LIBAROMA_DIALOG_WITH_SHADOW|
    LIBAROMA_DIALOG_ACCENT_BUTTON|LIBAROMA_DIALOG_CANCELABLE,
    1,
    "Copy",
    "Move",
    "Delete",
    NULL
  );
      libaroma_window_anishow(win,0,0);
      LOGI("--- HOLD : %i = %i - %i",msg.key, pooler.id, pooler.param);
     }
    }
    else if (pooler.cmd==LIBAROMA_CMD_CLICK){
      if (pooler.id==34){
      	size_t actual_list_id = msg.key - 100;
          LOGI("--- FileManager Touch: [%s] [%zu]",actual_list_id > (fileList.folderSize() - 1) ? "FILE" : "FOLDER", actual_list_id > (fileList.folderSize() - 1) ? (actual_list_id - fileList.folderSize()) : (actual_list_id));
          Pterodon::Gui::FileManager::ObjectData new_path = (actual_list_id > (fileList.folderSize() - 1) ? fileList.fileAt(actual_list_id - fileList.folderSize()) : fileList.folderAt(actual_list_id));
        if (new_path.fileType_ == DT_DIR) {
	     if (new_path.fileName_ == "..") {
	      current_folder = Pterodon::Framework::DirName(current_folder);
	      } else {
		  if (current_folder != "/") current_folder+= "/";
              current_folder+= new_path.fileName_;
          } 
        for (size_t i=0; i<fileList.size()+1; i++) {
        	libaroma_ctl_list_del_item_internal(
            var->menu,0,0 /* delete first index */
            );
      }      
        fileList = GetMenuList(win, var, current_folder, file_extension, list_icon_folder, list_icon_file, list_icon_back);
        while (fileList.empty() && current_folder != "/") {
        current_folder = Pterodon::Framework::DirName(current_folder);
        fileList = GetMenuList(win, var, current_folder, file_extension, list_icon_folder, list_icon_file, list_icon_back);
     }
        } else {
        const std::string dialog_confirm_message = "Are you sure that you want to install \"" + new_path.fileName_ + "\"?";
        int ret=libaroma_dialog_confirm(
         "Confirm installation",
         dialog_confirm_message.c_str(),
         "OK",
         "Cancel",
          NULL,
          LIBAROMA_DIALOG_DIM_PARENT|LIBAROMA_DIALOG_WITH_SHADOW|
          LIBAROMA_DIALOG_ACCENT_BUTTON|LIBAROMA_DIALOG_CANCELABLE
        );
        libaroma_window_anishow(win,0,0);
        LOGI("--- FILE: %i = %zu - %i",msg.key, actual_list_id - fileList.folderSize() - 1, pooler.param);
        }
      } else if (pooler.id==30){
        LOGI("--- APPBAR: p:%i, k:%i, y:%i", pooler.param,msg.key,msg.y);
        onpool = 0;
      } else{
        LOGI("--- CLICK: %i - %i", pooler.id, pooler.param);
    }
   }
  }
 }
  while(onpool);
}

/* main menu activity */
void Pterodon::Gui::FileList::ShowMenu(const std::string& folder_path, const std::string& file_extension){
  SELECTMENU var={0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  
  /* init window */
  LIBAROMA_WINDOWP win = libaroma_window(
      NULL, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL
    );
  
  if (win) {
        Menu_Pool(win, folder_path, file_extension, &var);
        libaroma_window_free(win);
     }
}

 } // Gui namespace
} // Pterodon namespace
