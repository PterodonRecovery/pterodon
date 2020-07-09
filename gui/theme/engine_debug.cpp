/*
    Pterodon Recovery - Debugging class for libaroma
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

#include "constants.h"
#include "aroma.h"
#include "engine_debug.hpp"

namespace Pterodon {
namespace Gui {
namespace Debug {

void Pterodon::Gui::Debug::Engine::LogMessages(LIBAROMA_MSGP msg) {
LOGD("-----------------------------------------------------------------");
switch (msg->msg) {
	    case LIBAROMA_MSG_KEY_RAWKEY:{
        LOGD("LogMessages: LIBAROMA_MSG_KEY_RAWKEY");
        LOGD("LogMessages: key: %d", msg->key);
        }
        break;
	    case LIBAROMA_MSG_KEY_FP:{
        LOGD("LogMessages: LIBAROMA_MSG_KEY_FP");
        }
        break;
        case LIBAROMA_MSG_EXIT:{
        LOGD("LogMessages: LIBAROMA_MSG_EXIT");
        }
        break;
        case LIBAROMA_MSG_OK:{
        LOGD("LogMessages: LIBAROMA_MSG_OK");
        }
        break;
        case LIBAROMA_MSG_BACK:{
        LOGD("LogMessages: LIBAROMA_MSG_BACK");
        }
        break;
        case LIBAROMA_MSG_NEXT:{
        LOGD("LogMessages: LIBAROMA_MSG_NEXT");
        }
        break;
        case LIBAROMA_MSG_MENU:{
        LOGD("LogMessages: LIBAROMA_MSG_MENU");
        }
        break;
        case LIBAROMA_MSG_TOUCH:{
        LOGD("LogMessages: LIBAROMA_MSG_TOUCH");
        }
        break;
        case LIBAROMA_MSG_KEY_SELECT:{
        LOGD("LogMessages: LIBAROMA_MSG_KEY_SELECT");
        }
        break;
        case LIBAROMA_MSG_KEY_MENU:{
        LOGD("LogMessages: LIBAROMA_MSG_KEY_MENU");
        }
        break;
        case LIBAROMA_MSG_KEY_BACK:{
        LOGD("LogMessages: LIBAROMA_MSG_KEY_BACK");
        }
        break;
        case LIBAROMA_MSG_KEY_UP:{
        LOGD("LogMessages: LIBAROMA_MSG_KEY_UP");
        }
        break;
        case LIBAROMA_MSG_KEY_DOWN:{
        LOGD("LogMessages: LIBAROMA_MSG_KEY_DOWN");
        }
        break;
     }  
     
     switch (msg->key) {     	
     case LIBAROMA_HID_EV_RET_EXIT:{
        LOGD("LogMessages: LIBAROMA_HID_EV_RET_EXIT");
        }
        break;    
       case LIBAROMA_HID_EV_RET_FP:{
        LOGD("LogMessages: LIBAROMA_HID_EV_RET_FP");
        }
        break; 
     	case LIBAROMA_HID_EV_RET_BACK:{
        LOGD("LogMessages: LIBAROMA_HID_EV_RET_BACK");
        }
        break;
        case LIBAROMA_HID_EV_RET_SELECT:{
        LOGD("LogMessages: LIBAROMA_HID_EV_RET_UP");
        }
        break;
       case LIBAROMA_HID_EV_RET_DOWN:{
        LOGD("LogMessages: LIBAROMA_HID_EV_RET_DOWN");
        }
        break;     
        case LIBAROMA_HID_EV_RET_UP:{
        LOGD("LogMessages: LIBAROMA_HID_EV_RET_UP");
        }
        break;
     }
     
     
     
     switch (msg->state) {     	
     case LIBAROMA_HID_EV_STATE_DOWN:{
        LOGD("LogMessages: LIBAROMA_HID_EV_STATE_DOWN");
        }
        break;          
     	case LIBAROMA_HID_EV_STATE_UP:{
        LOGD("LogMessages: LIBAROMA_HID_EV_STATE_UP");
        }
        break;
        case LIBAROMA_HID_EV_STATE_MOVE:{
        LOGD("LogMessages: LIBAROMA_HID_EV_STATE_MOVE");
        }
        break;
       case LIBAROMA_HID_EV_STATE_CANCEL:{
        LOGD("LogMessages: LIBAROMA_HID_EV_STATE_CANCEL");
        }
        break;     
     }
LOGD("-----------------------------------------------------------------");
}

} // namespace Debug
} // namespace Gui
} // namespace Pterodon


