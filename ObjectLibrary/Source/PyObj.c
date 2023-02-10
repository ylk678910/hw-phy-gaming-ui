#include "PyObj.h"

ObjectControlStatic_t xMovePyObj(PyObj_t *PyObj)
{
    int MoveX, MoveY;
    Object_t *ObjectNow;
    PyObj_t *PyObjNow;
		ObjectFindTop_t result;

#ifdef OBJECT_GROUP_DEFINED
    if (!PyObj_Rebounded(PyObj))
    {
        ObjectGroup_t *NowGroup = xObjectGroup_GetTop();
        while (NowGroup != NULL)
        {
						if(NowGroup->Mode & GROUP_REBOUND_ACTIVE)
						{
								if (xObjectGroup_IsObjInGroup(NowGroup, PyObj_Obj(PyObj)))
								{
										uint8_t ptr = 0;
										ObjectNow = xObjectGroup_GetObj(NowGroup, ptr);
										while (ObjectNow != NULL)
										{
												//CODE WRITE HERE START
												if (PyObj_Obj(PyObj) != ObjectNow)
												{
														if (xGetPyObjByObj(&PyObjNow, ObjectNow) == OBJECT_SUCCESS)
														{
																if (PyObj_Mode(PyObjNow) & GROUP_REBOUND_ACTIVE)
																{
																		if (xIsObjectOverlappingTP(PyObj_Obj(PyObj), ObjectNow) == OBJECT_TRUE)
																		{
																				float V_Src_X = ((PyObj_Restitution(PyObjNow)+1)*PyObj_X_Speed(PyObjNow)+PyObj_X_Speed(PyObj)*(1-PyObj_Restitution(PyObj)*1))/2;
																				float V_Src_Y = ((PyObj_Restitution(PyObjNow)+1)*PyObj_Y_Speed(PyObjNow)+PyObj_Y_Speed(PyObj)*(1-PyObj_Restitution(PyObj)*1))/2;
																				float V_Cmp_X = ((PyObj_Restitution(PyObj)+1)*PyObj_X_Speed(PyObj)+PyObj_X_Speed(PyObjNow)*(1-PyObj_Restitution(PyObjNow)*1))/2;
																				float V_Cmp_Y = ((PyObj_Restitution(PyObj)+1)*PyObj_Y_Speed(PyObj)+PyObj_Y_Speed(PyObjNow)*(1-PyObj_Restitution(PyObjNow)*1))/2;
																			
																				int Xpre_Src = PyObj_x(PyObj) - (int)PyObj_X_Speed(PyObj);
																				int Ypre_Src = PyObj_y(PyObj) - (int)PyObj_Y_Speed(PyObj);
																				int Xpre_Cmp = Object_x(ObjectNow) - (int)PyObj_X_Speed(PyObjNow);
																				int Ypre_Cmp = Object_y(ObjectNow) - (int)PyObj_Y_Speed(PyObjNow);
																				float VectorX = Xpre_Src - Xpre_Cmp;
																				float VectorY = Ypre_Src - Ypre_Cmp;
																				float Length_Cmp = sqrt(VectorX*VectorX + VectorY*VectorY);
																				VectorX /= Length_Cmp;
																				VectorY /= Length_Cmp;
																				
																				//The place where collide happend.
																				float x1 = (float)Object_xMid(ObjectNow) + ((float)(Object_Width(ObjectNow)>>1)+1)*VectorX;
																				float y1 = (float)Object_yMid(ObjectNow) + ((float)(Object_Length(ObjectNow)>>1)+1)*VectorY;
																				float x2 = (float)PyObj_xMid(PyObj) + ((float)(PyObj_Width(PyObj)>>1)+1)*(-VectorX);
																				float y2 = (float)PyObj_yMid(PyObj) + ((float)(PyObj_Length(PyObj)>>1)+1)*(-VectorY);
																					
																				xMoveObject(PyObj_Obj(PyObj), x1-x2, y1-y2);
																			
																				PyObj_X_Speed(PyObj) = V_Src_X;
																				PyObj_Y_Speed(PyObj) = V_Src_Y;
																				PyObj_X_Speed(PyObjNow) = V_Cmp_X;
																				PyObj_Y_Speed(PyObjNow) = V_Cmp_Y;
																				PyObj_Rebounded(PyObjNow) = 1;
																				PyObj_Rebounded(PyObj) = 1;
																				goto EndFind;
																		}
																}
														}
												}
												//CODE WRITE HERE END
												ObjectNow = xObjectGroup_GetObj(NowGroup, ++ptr);
										}
										if (PyObj_Rebounded(PyObj))
												break;
								}
						}
            NowGroup = NowGroup->Next;
        }
    }
#endif
		
		EndFind:
		
		result = ucIsTouchBound_LR(PyObj_Obj(PyObj));
		if(result != NOT_FOUND)
		{
				PyObj_X_Speed(PyObj) *= PyObj_ReboundResis(PyObj);
				if(!(PyObj_Mode(PyObj) & GRAVITY_ACTIVE))
				{
						if (PyObj_Y_Speed(PyObj) > 0)
						{
								PyObj_Y_Speed(PyObj) -= PyObj_Resistance(PyObj);
						}
						else
						{
								PyObj_Y_Speed(PyObj) += PyObj_Resistance(PyObj);
						}
				}
		}
		
		result = ucIsTouchBound_TB(PyObj_Obj(PyObj));
		//printf("%d\n\r",PyObj_yMax(PyObj));
		if(result == AT_BOTTOM || (result == AT_TOP && !(PyObj_Mode(PyObj) & GRAVITY_ACTIVE)))
		{		
				if (PyObj_X_Speed(PyObj) > 0)
				{
						PyObj_X_Speed(PyObj) -= PyObj_Resistance(PyObj);
				}
				else
				{
						PyObj_X_Speed(PyObj) += PyObj_Resistance(PyObj);
				}
		}


    if (PyObj_Mode(PyObj) & GRAVITY_ACTIVE)
    {
        PyObj_Y_Accel(PyObj) = PyObj_Gravity(PyObj);
    }
    else
    {
        PyObj_Y_Accel(PyObj) = 0;
    }

    PyObj_X_Speed(PyObj) += PyObj_X_Accel(PyObj);
    PyObj_Y_Speed(PyObj) += PyObj_Y_Accel(PyObj);

    MoveX = (int)PyObj_X_Speed(PyObj);
    MoveY = (int)PyObj_Y_Speed(PyObj);

    if (PyObj_Mode(PyObj) & NOT_EXCEED_BOUNDARY)
    {
        if (PyObj_xMax(PyObj) + (int)PyObj_X_Speed(PyObj) >= RIGHT_BOUND)
        {
            if (PyObj_Mode(PyObj) & EDGE_REBOUND_ACTIVE)
            {
                PyObj_X_Speed(PyObj) = -PyObj_X_Speed(PyObj);
            }
            MoveX = RIGHT_BOUND - PyObj_xMax(PyObj);
        }
        else if (PyObj_x(PyObj) + (int)PyObj_X_Speed(PyObj) < LEFT_BOUND)
        {
            if (PyObj_Mode(PyObj) & EDGE_REBOUND_ACTIVE)
            {
                PyObj_X_Speed(PyObj) = -PyObj_X_Speed(PyObj);
            }
            MoveX = LEFT_BOUND - PyObj_x(PyObj);
        }

        if (PyObj_yMax(PyObj) + (int)PyObj_Y_Speed(PyObj) >= BOTTOM_BOUND)
        {
            if (PyObj_Mode(PyObj) & EDGE_REBOUND_ACTIVE)
            {
                PyObj_Y_Speed(PyObj) = -PyObj_Y_Speed(PyObj) * PyObj_ReboundResis(PyObj);
            }
            MoveY = BOTTOM_BOUND - PyObj_yMax(PyObj);
        }
        else if (PyObj_y(PyObj) + (int)PyObj_Y_Speed(PyObj) < TOP_BOUND)
        {
            if (PyObj_Mode(PyObj) & EDGE_REBOUND_ACTIVE)
            {
                PyObj_Y_Speed(PyObj) = -PyObj_Y_Speed(PyObj) * PyObj_ReboundResis(PyObj);
            }
            MoveY = TOP_BOUND - PyObj_y(PyObj);
        }
    }
    //printf("PyObj_yMax(PyObj)=%d, MoveY=%d\n\r", PyObj_yMax(PyObj), MoveY);
		
    return xMoveObject(PyObj_Obj(PyObj), MoveX, MoveY);
}

//Initialize PyObj library
ObjectControlStatic_t xInitPyObj(void)
{
    return xPyObjData(0, INITIAL, 0);
}

//Create a new PyObj and store it into PyObj database   !!!MUST POINT PyObj TO NULL IF IT IS NEW PyObj
ObjectControlStatic_t xNewPyObj(PyObj_t **PyObj)
{
    return xPyObjData(PyObj, NEW, 0);
}

//Get PyObj in the database[ptr] by using number(ptr)
ObjectControlStatic_t xGetPyObj(PyObj_t **PyObj, uint32_t ptr)
{
    return xPyObjData(PyObj, GET, ptr);
}

//Get PyObj in the database[ptr] by using Mark(char*=>uint32_t)
ObjectControlStatic_t xGetPyObjByMark(PyObj_t **PyObj, Mark_t Mark)
{
    return xPyObjData(PyObj, GET_BY_MARK, (uint32_t)Mark);
}

//Get PyObj by Object
ObjectControlStatic_t xGetPyObjByObj(PyObj_t **PyObj, Object_t *Object)
{
    uint32_t ptr;
    for (ptr = 0; ptr < MAX_OBJECT; ptr++)
    {
        if (xPyObjData(PyObj, GET, ptr) == OBJECT_SUCCESS)
        {
            if (PyObj_Obj(*PyObj) == Object)
            {
                return OBJECT_SUCCESS;
            }
        }
    }
    return OBJECT_FAIL;
}

//Fully delete PyObj(Including database and PyObj pointer)
ObjectControlStatic_t xDeletePyObj(PyObj_t **PyObj)
{
    return xPyObjData(PyObj, DEL, 0);
}

//Functions which controls all PyObjs. All PyObj pointer are in here  !!!MUST POINT PyObj TO NULL IF IT IS NEW PyObj
ObjectControlStatic_t xPyObjData(PyObj_t **PyObj, const ObjectControl_t Control, uint32_t ptr)
{
    static PyObj_t *PyObjList[MAX_OBJECT];
    static bool PyObjEmpty[MAX_OBJECT];

    switch (Control)
    {
    case INITIAL:
        for (ptr = 0; ptr < MAX_OBJECT; ptr++)
        {
            free(PyObjList[ptr]);
            //PyObjList[ptr] = NULL;
            PyObjEmpty[ptr] = true;
        }
        return OBJECT_SUCCESS;
#ifdef DEBUG
        break;
#endif
    case NEW:
        for (ptr = 0; ptr < MAX_OBJECT; ptr++)
        {
            if (PyObjEmpty[ptr])
            {
                PyObj_t *NewPyObj = NULL;
                if (*PyObj != NULL)
                    free(*PyObj);
                NewPyObj = (PyObj_t *)calloc(1, sizeof(PyObj_t));
                if (NewPyObj == NULL)
                {
                    return NO_MEMORY_SPACE;
                }

                if (xNewObject(&PyObj_Obj(NewPyObj)) == NO_MEMORY_SPACE)
                {
                    free(NewPyObj);
                    return NO_MEMORY_SPACE;
                }

                *PyObj = NewPyObj;
                PyObjEmpty[ptr] = false;
                PyObjList[ptr] = *PyObj;

                return OBJECT_SUCCESS;
            }
            else
            {
                if (ptr == MAX_OBJECT - 1)
                    return OBJECT_FAIL;
            }
        }
#ifdef DEBUG
        break;
#endif
    case GET:
        if (PyObjEmpty[ptr])
        {
            return OBJECT_FAIL;
        }
        else
        {
            *PyObj = PyObjList[ptr];
            return OBJECT_SUCCESS;
        }
#ifdef DEBUG
        break;
#endif
    case GET_BY_MARK:
        for (uint32_t ptr_temp = 0; ptr_temp < MAX_OBJECT; ptr_temp++)
        {
            if (!PyObjEmpty[ptr_temp])
            {
                if ((PyObj_Mark(PyObjList[ptr_temp])) == (Mark_t)ptr) //Find
                {
                    *PyObj = PyObjList[ptr_temp];
                    return OBJECT_SUCCESS;
                }
            }
        }
        return OBJECT_FAIL;
#ifdef DEBUG
        break;
#endif
    case DEL:
        if (xDeleteObject(&PyObj_Obj(*PyObj)) == OBJECT_FAIL)
        {
            return OBJECT_FAIL;
        }
        for (ptr = 0; ptr < MAX_OBJECT; ptr++)
        {
            if (PyObjList[ptr] == *PyObj)
            {
                free(*PyObj);
                *PyObj = NULL;
                PyObjEmpty[ptr] = NULL;
                PyObjEmpty[ptr] = true;
                return OBJECT_SUCCESS;
            }
        }
        return OBJECT_FAIL;
#ifdef DEBUG
        break;
#endif
    default:
        return CONTROL_STATE_WRONG;
#ifdef DEBUG
        break;
#endif
    }
}
