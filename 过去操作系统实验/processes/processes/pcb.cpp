#include "pcb.h"

std::string getStateString(int state){
    switch (state) {
    case 0:
        return "后备";
    case 1:
        return "就绪";
    case 2:
        return "运行";
    case 3:
        return "挂起";
    default:
        break;
    }
    return "";
}
