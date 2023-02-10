#include "main.h"
#include "ObjectTime.h"

volatile unsigned int ObjecTime;

void ObjectGUI_delay(unsigned int ObjDelayTime)
{
    static unsigned int ObjecTime_pre;

    ObjecTime_pre = ObjecTime;
    while (ObjecTime - ObjecTime_pre < ObjDelayTime)
    {
        //ObjectLCDShow();
    }
}

void ObjectGUI_TimeAdd(void)
{
    ObjecTime++;
}
