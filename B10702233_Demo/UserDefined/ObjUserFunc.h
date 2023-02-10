#include "stdint.h"
#include "main.h"

#ifndef OBJ_USER_FUNC_DEFINED
	#define OBJ_USER_FUNC_DEFINED
	
	#define BALL_COLOR_1 LCD_COLOR_ORANGE
	#define BALL_COLOR_2 LCD_COLOR_LIGHTGRAY
	
	uint8_t bTS_Debounce(void);
	void vTS_PressControl(void (*fptr)(uint8_t, uint8_t, uint16_t, uint16_t));
	void vDetectPress_NewDel(uint8_t Which, uint8_t Touchs, uint16_t x, uint16_t y);
	
#ifndef PYOBJ_DEFINED
	void vDropBall_Group(ObjectGroup_t *Group);
#else
	void vBallJump_Group(ObjectGroup_t *Group);
#endif
	
	extern uint32_t *Pic_OrangeBall, *Pic_BlueBall;
	extern ObjectGroup_t *BallGroup_Orange, *BallGroup_Blue;
	extern ObjectPicture_t Picture_Orange, Picture_Blue;
#endif
