#include "ObjectGroup.h"

uint8_t ObjectGroup_Number = 0;
uint8_t ObjectGroup_BiggistNum = 0;
ObjectGroup_t *GroupTop = NULL;

void vPressMove_Group(ObjectGroup_t *Group)
{
    static uint8_t GroupNum_pre;
    static uint8_t *ObjNum_pre;
		uint8_t ObjNum, GroupNum;
	
    static ObjectGroup_t **Group_ptr;
    static uint16_t **PressX_pre_ptr, **PressY_pre_ptr;
	
		uint8_t G_ptr;
	
		GroupNum = ucObjectGroup_GetGroupNum();
	
    //New or Delete object from group
    if (GroupNum != GroupNum_pre)
    {
				for(uint8_t i=0; i<GroupNum_pre; i++)
				{
						free(PressX_pre_ptr[i]);
						free(PressY_pre_ptr[i]);
				}
        free(PressX_pre_ptr);
        free(PressY_pre_ptr);
        PressX_pre_ptr = (uint16_t **)calloc(GroupNum, sizeof(uint16_t*));
        PressY_pre_ptr = (uint16_t **)calloc(GroupNum, sizeof(uint16_t*));
				
        free(Group_ptr);
        Group_ptr = (ObjectGroup_t **)calloc(GroupNum, sizeof(ObjectGroup_t*));
        GroupNum_pre = GroupNum;
			
				free(ObjNum_pre);
				ObjNum_pre = (uint8_t *)calloc(GroupNum, sizeof(uint8_t));
    }
		
		for(G_ptr = 0; G_ptr < GroupNum; G_ptr++)
		{
				if(Group_ptr[G_ptr] == Group)
				{
						break;
				}
				else if(Group_ptr[G_ptr] == NULL)
				{
						Group_ptr[G_ptr] = Group;
						PressX_pre_ptr[G_ptr] = NULL;
						PressY_pre_ptr[G_ptr] = NULL;
						break;
				}
		}

		ObjNum = ucObjectGroup_GetGroupObjNum(Group);
		
    //New or Delete object from group
    if (ObjNum != ObjNum_pre[G_ptr])
    {
				//printf("%x\n\r", PressX_pre_ptr[G_ptr]);
				if(PressX_pre_ptr[G_ptr] != NULL)
						free(PressX_pre_ptr[G_ptr]);
				if(PressY_pre_ptr[G_ptr] != NULL)
						free(PressY_pre_ptr[G_ptr]);
        PressX_pre_ptr[G_ptr] = (uint16_t *)calloc(ObjNum, sizeof(uint16_t));
        PressY_pre_ptr[G_ptr] = (uint16_t *)calloc(ObjNum, sizeof(uint16_t));

        ObjNum_pre[G_ptr] = ObjNum;
    }

    //Start detect
    uint16_t *X_Press = (uint16_t *)calloc(ObjNum, sizeof(uint16_t));
    uint16_t *Y_Press = (uint16_t *)calloc(ObjNum, sizeof(uint16_t));
    uint16_t x, y;
    uint8_t TS_PressSet = 0;
    Object_t *ObjectNow;

    TS_StateTypeDef TS_State;
    BSP_TS_GetState(&TS_State); //TS_State.touchDetected

    if (TS_State.touchDetected)
    {
        for (uint8_t TS_ptr = 0; TS_ptr < TS_State.touchDetected; TS_ptr++)
        {
            x = TS_State.touchX[TS_ptr];
            y = TS_State.touchY[TS_ptr];
            for (uint8_t ptr = 0; ptr < ObjNum; ptr++)
            {
                for (uint8_t ptr_tmp = 0; ptr_tmp < ObjNum; ptr_tmp++)
                {
                    ObjectNow = xObjectGroup_GetObj(Group, ptr_tmp);
										if (xPointInObject(ObjectNow, x, y) == OBJECT_SUCCESS && Object_Pressed(ObjectNow))
										{
												X_Press[ptr_tmp] = x;
												Y_Press[ptr_tmp] = y;
												TS_PressSet = 1;
												break;
										}
                }
                if (TS_PressSet)
                    break;
                if (xPointInObject(xObjectGroup_GetObj(Group, ptr), x, y) == OBJECT_SUCCESS)
                {
                    X_Press[ptr] = x;
                    Y_Press[ptr] = y;
                    break;
                }
            }
            TS_PressSet = 0;
        }
    }

    {
        uint8_t ptr = 0;
        ObjectNow = xObjectGroup_GetObj(Group, ptr);
        while (ObjectNow != NULL)
        {
            //CODE WRITE HERE START
            if (X_Press[ptr] > 0 && Y_Press[ptr] > 0) //Obj1 Press
            {
                if (!Object_Pressed(ObjectNow))
                {
                    Object_Pressed(ObjectNow) = 1;
                }
                else
                {
                    xMoveObject(ObjectNow, X_Press[ptr] - PressX_pre_ptr[G_ptr][ptr], Y_Press[ptr] - PressY_pre_ptr[G_ptr][ptr]);

                    PyObj_t *PyObjNow;
                    xGetPyObjByObj(&PyObjNow, ObjectNow);
                    PyObj_X_Speed(PyObjNow) = X_Press[ptr] - PressX_pre_ptr[G_ptr][ptr];
                    PyObj_Y_Speed(PyObjNow) = Y_Press[ptr] - PressY_pre_ptr[G_ptr][ptr];
                }
                PressX_pre_ptr[G_ptr][ptr] = X_Press[ptr];
                PressY_pre_ptr[G_ptr][ptr] = Y_Press[ptr];
            }
            else
            {
                Object_Pressed(ObjectNow) = 0;
            }
            //CODE WRITE HERE END
            ObjectNow = xObjectGroup_GetObj(Group, ++ptr);
        }
    }

    free(X_Press);
    free(Y_Press);
}

ObjectGroup_t *xObjectGroup_GetTop(void)
{
    return GroupTop;
}

static uint8_t xObjectGroup_FindEmptyGroupNum(void)
{
    ObjectGroup_t *Now = NULL;
    uint8_t NowGroupNum;

    if (ObjectGroup_Number != 0)
    {
        Now = GroupTop;
        for (NowGroupNum = 0; NowGroupNum <= ObjectGroup_BiggistNum; NowGroupNum++)
        {
            for (uint8_t ptr = 0; ptr < ObjectGroup_Number; ptr++)
            {
                if (Now->GroupNum == NowGroupNum) //Found a repeated GroupNum
                {
                    Now = Now->Next;
                    break;
                }
                else if (ptr == ObjectGroup_Number - 1) //Found a GroupNum can be used
                {
                    return NowGroupNum;
                }
            }
        }
        ObjectGroup_BiggistNum += 1;
        return ObjectGroup_BiggistNum;
    }
    else
    {
        ObjectGroup_BiggistNum = 0;
        return ObjectGroup_BiggistNum;
    }
}

ObjectGroup_t *xObjectGroup_New(void)
{
    ObjectGroup_t *Now = NULL, *ObjGroup;
    uint8_t GroupNum = 0;

    ObjGroup = (ObjectGroup_t *)calloc(1, sizeof(ObjectGroup_t));
    if (ObjGroup)
    {

        if (ObjectGroup_Number == 0)
        {
            GroupTop = ObjGroup;
            Now = GroupTop;
        }
        else
        {
            Now = GroupTop;
            for (uint8_t ptr = 0; ptr < ObjectGroup_Number - 1; ptr++) //Find last
            {
                Now = Now->Next;
            }
            Now->Next = ObjGroup;
        }

        GroupNum = xObjectGroup_FindEmptyGroupNum();

        ObjGroup->GroupNum = GroupNum;

        ObjectGroup_Number++;
        return ObjGroup;
    }
    else
    {
        return NULL;
    }
}

ObjectGroup_t *xObjectGroup_Get(const uint8_t GroupNum)
{
    if (GroupNum > ObjectGroup_BiggistNum)
        return NULL;

    ObjectGroup_t *Now = GroupTop;

    while (Now != NULL)
    {
        if (Now->GroupNum == GroupNum)
            return Now;
        Now = Now->Next;
    }
    return NULL;
}

ObjectControlStatic_t xObjectGroup_Delete(ObjectGroup_t **ObjGroup)
{
    //Now->GroupNum ObjectGroup_BiggistNum
    if (*ObjGroup == NULL)
        return OBJECT_FAIL;
    if (ObjectGroup_Number == 0)
        return OBJECT_FAIL;

    ObjectGroup_t *Now, *Now_pre;
    uint8_t BiggestNum = 0;
    uint8_t ptr = 0;

    if (GroupTop != *ObjGroup)
    {
        Now_pre = GroupTop;
        Now = GroupTop->Next;
        for (; ptr < ObjectGroup_Number - 1; ptr++)
        {
            if (Now == *ObjGroup)
            {
                break;
            }
            else
            {
                if (Now->GroupNum > BiggestNum)
                    BiggestNum = Now->GroupNum;
            }
            Now_pre = Now;
            Now = Now->Next;
        }
        if (Now != *ObjGroup)
            return OBJECT_FAIL;

        Now_pre->Next = Now->Next;

        if (ObjectGroup_BiggistNum == Now->GroupNum)
        {
            for (; ptr < ObjectGroup_Number; ptr++)
            {
                if (Now->Next == NULL)
                    break;
                if (Now->GroupNum > BiggestNum)
                    BiggestNum = Now->GroupNum;
                Now = Now->Next;
            }
            ObjectGroup_BiggistNum = BiggestNum;
        }

        if (Now->Member != NULL)
        {
            ObjectGroupMember_t *Now_Member = Now->Member;
            ObjectGroupMember_t *Pre_Member;
            while (1)
            {
                Pre_Member = Now_Member;
                if (Now_Member->Next != NULL)
                {
                    Now_Member = Now_Member->Next;
                    free(Pre_Member);
                }
                else
                {
                    free(Pre_Member);
                    break;
                }
            }
        }

        free(Now);
    }
    else //Is GroupTop
    {
        if (ObjectGroup_Number == 1)
        {
            if (GroupTop->Member != NULL)
            {
                ObjectGroupMember_t *Now_Member = GroupTop->Member;
                ObjectGroupMember_t *Pre_Member;
                while (1)
                {
                    Pre_Member = Now_Member;
                    if (Now_Member->Next != NULL)
                    {
                        Now_Member = Now_Member->Next;
                        free(Pre_Member);
                    }
                    else
                    {
                        free(Pre_Member);
                        break;
                    }
                }
            }
            free(GroupTop);
            GroupTop = NULL;
            ObjectGroup_Number = 0;
            ObjectGroup_BiggistNum = 0;
            *ObjGroup = NULL;
            return OBJECT_SUCCESS;
        }

        Now = GroupTop->Next;
        if (ObjectGroup_BiggistNum == GroupTop->GroupNum)
        {
            for (; ptr < ObjectGroup_Number - 1; ptr++)
            {
                if (Now->GroupNum > BiggestNum)
                    BiggestNum = Now->GroupNum;
                Now = Now->Next;
            }
            ObjectGroup_BiggistNum = BiggestNum;
        }

        Now = GroupTop->Next;

        if (Now->Member != NULL)
        {
            ObjectGroupMember_t *Now_Member = Now->Member;
            ObjectGroupMember_t *Pre_Member;
            while (1)
            {
                Pre_Member = Now_Member;
                if (Now_Member->Next != NULL)
                {
                    Now_Member = Now_Member->Next;
                    free(Pre_Member);
                }
                else
                {
                    free(Pre_Member);
                    break;
                }
            }
        }

        free(GroupTop);
        GroupTop = Now;
    }
    ObjectGroup_Number--;
    *ObjGroup = NULL;
    return OBJECT_SUCCESS;
}

ObjectControlStatic_t xObjectGroup_AddObj(ObjectGroup_t *ObjGroup, Object_t *Object)
{
    ObjectGroupMember_t *Now, *Now_pre;

    if (ObjGroup == NULL)
        return OBJECT_FAIL;
    if (ObjGroup->Member == NULL)
    {
        ObjGroup->Member = (ObjectGroupMember_t *)calloc(1, sizeof(ObjectGroupMember_t));
        if (ObjGroup->Member == NULL)
            return OBJECT_FAIL;
        else
            ObjGroup->Member->Obj = Object;
        return OBJECT_SUCCESS;
    }
    else
    {
        Now_pre = NULL;
        Now = ObjGroup->Member;
        while (Now != NULL)
        {
            if (Now->Obj == Object)
            {
                return OBJECT_FAIL;
            }
            Now_pre = Now;
            Now = Now->Next;
        }

        Now = (ObjectGroupMember_t *)calloc(1, sizeof(ObjectGroupMember_t));
        if (Now == NULL)
            return OBJECT_FAIL;
        else
        {
            Now->Obj = Object;
            Now_pre->Next = Now;
        }
        return OBJECT_SUCCESS;
    }
}


uint8_t ucObjectGroup_GetGroupNum(void)
{
		return ObjectGroup_Number;
}

uint8_t ucObjectGroup_GetGroupObjNum(const ObjectGroup_t *ObjGroup)
{
    if (ObjGroup == NULL)
        return 0;
		
		uint8_t ObjNum = 0;

    ObjectGroupMember_t *Now = ObjGroup->Member;

    while (Now != NULL)
    {
				ObjNum++;
        Now = Now->Next;
    }

    return ObjNum;
}

Object_t *xObjectGroup_GetObj(const ObjectGroup_t *ObjGroup, const uint8_t ObjectNum)
{
    if (ObjGroup == NULL)
        return NULL;

    ObjectGroupMember_t *Now = ObjGroup->Member;
    uint8_t ptr = 0;

    while (Now != NULL)
    {
        if (ptr++ == ObjectNum)
            return Now->Obj;

        Now = Now->Next;
    }

    return NULL;
}

ObjectControlStatic_t xObjectGroup_RemoveObj(ObjectGroup_t *ObjGroup, const Object_t *Object)
{
    //Now->GroupNum ObjectGroup_BiggistNum
    if (ObjGroup == NULL)
        return OBJECT_FAIL;
    if (ObjGroup->Member == NULL)
        return OBJECT_FAIL;

    ObjectGroupMember_t *Now = ObjGroup->Member, *Now_pre = NULL;

    while (Now != NULL)
    {
        if (Now->Obj == Object)
        {
            if (Now_pre == NULL)
            {
                ObjGroup->Member = Now->Next;
            }
            else
            {
                Now_pre->Next = Now->Next;
            }
            free(Now);
            return OBJECT_SUCCESS;
        }

        Now_pre = Now;
        Now = Now->Next;
    }
    return OBJECT_FAIL;
}

ObjectControlStatic_t xObjectGroup_RemoveObjFromAllGroup(const Object_t *Object)
{
    if (ObjectGroup_Number == 0)
        return OBJECT_FAIL;

    //uint8_t Obj_ptr;
    ObjectGroup_t *NowGroup = GroupTop;
    ObjectGroupMember_t *Now_Member, *Pre_Member = NULL;

    while (NowGroup != NULL)
    {
        Now_Member = NowGroup->Member;
        Pre_Member = NULL;
        while (Now_Member != NULL)
        {
            if (Now_Member->Obj == Object)
            {
                if (Pre_Member == NULL)
                {
                    NowGroup->Member = Now_Member->Next;
                }
                else
                {
                    Pre_Member->Next = Now_Member->Next;
                }
                free(Now_Member);
                break;
            }
            Pre_Member = Now_Member;
            Now_Member = Now_Member->Next;
        }

        NowGroup = NowGroup->Next;
    }
    return OBJECT_SUCCESS;
}

uint8_t xObjectGroup_IsObjInGroup(const ObjectGroup_t *ObjGroup, const Object_t *Object)
{
		ObjectGroupMember_t *Now_Member;
    Now_Member = ObjGroup->Member;
    while (Now_Member != NULL)
    {
        if (Now_Member->Obj == Object)
        {
						return 1;
        }
        Now_Member = Now_Member->Next;
    }
		return 0;
}
