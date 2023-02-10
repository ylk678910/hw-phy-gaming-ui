#include "stdbool.h"
#include "math.h"
#include "Object.h"

#ifndef PYOBJ_DEFINED
#define PYOBJ_DEFINED

//PyObj Mode
#define NOT_EXCEED_BOUNDARY 0x1 << 0
#define GRAVITY_ACTIVE 0x1 << 1
#define EDGE_REBOUND_ACTIVE 0x1 << 2

typedef struct
{
	Object_t *Obj;
	float X_Speed, Y_Speed;
	float X_Accel, Y_Accel; 		//Acceleration 
	float Gravity, Resistance, ReboundResis;	//Gravity only active at Y-axis, Resistance active at bottom, ReboundResis active at when touch left, right, top or bottom
	float Restitution;
	uint8_t Rebounded;
	uint8_t Mode;
} PyObj_t;

#define PyObj_x(PyObj) ((PyObj->Obj)->x)
#define PyObj_y(PyObj) ((PyObj->Obj)->y)
#define PyObj_xMax(PyObj) (((PyObj)->Obj)->x + (PyObj->Obj)->Width)
#define PyObj_yMax(PyObj) (((PyObj)->Obj)->y + (PyObj->Obj)->Length)
#define PyObj_xMid(PyObj) (Object_xMid((PyObj->Obj)))
#define PyObj_yMid(PyObj) (Object_yMid((PyObj->Obj)))
#define PyObj_Width(PyObj) (((PyObj)->Obj)->Width)
#define PyObj_Length(PyObj) (((PyObj)->Obj)->Length)
#define PyObj_Pic(PyObj) (((PyObj)->Obj)->Pic)
#define PyObj_Color(PyObj) (((PyObj)->Obj)->Color)
#define PyObj_Num_of_PicSize(PyObj) (((PyObj)->Obj)->Num_of_PicSize)
#define PyObj_Visible(PyObj) (((PyObj)->Obj)->Visible)
#define PyObj_Mark(PyObj) (((PyObj)->Obj)->Mark)
#define PyObj_Pressed(PyObj) (((PyObj)->Obj)->Pressed)
#define PyObj_Obj(PyObj) ((PyObj)->Obj)
#define PyObj_Priority(PyObj) (((PyObj)->Obj)->Priority)
#define PyObj_X_Speed(PyObj) ((PyObj)->X_Speed)
#define PyObj_Y_Speed(PyObj) ((PyObj)->Y_Speed)
#define PyObj_X_Accel(PyObj) ((PyObj)->X_Accel)
#define PyObj_Y_Accel(PyObj) ((PyObj)->Y_Accel)
#define PyObj_Gravity(PyObj) ((PyObj)->Gravity)
#define PyObj_Resistance(PyObj) ((PyObj)->Resistance)
#define PyObj_ReboundResis(PyObj) ((PyObj)->ReboundResis)
#define PyObj_Rebounded(PyObj) ((PyObj)->Rebounded)
#define PyObj_Restitution(PyObj) ((PyObj)->Restitution)
#define PyObj_Mode(PyObj) (PyObj->Mode)

ObjectControlStatic_t xMovePyObj(PyObj_t *PyObj);

ObjectControlStatic_t xInitPyObj(void);
ObjectControlStatic_t xNewPyObj(PyObj_t **PyObj);
ObjectControlStatic_t xGetPyObj(PyObj_t **PyObj, uint32_t ptr);
ObjectControlStatic_t xGetPyObjByObj(PyObj_t **PyObj, Object_t *Object);
ObjectControlStatic_t xGetPyObjByMark(PyObj_t **PyObj, Mark_t Mark);
ObjectControlStatic_t xDeletePyObj(PyObj_t **PyObj);
ObjectControlStatic_t xPyObjData(PyObj_t **PyObj, const ObjectControl_t Control, uint32_t ptr);
#endif
