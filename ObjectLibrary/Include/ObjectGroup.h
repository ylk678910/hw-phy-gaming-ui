#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "Object.h"

#ifndef OBJECT_GROUP_DEFINED
#define OBJECT_GROUP_DEFINED

//Group Mode
#define GROUP_REBOUND_ACTIVE 0x1 << 0
//

typedef struct ObjectGroup_t ObjectGroup_t;
typedef struct ObjectGroupMember_t ObjectGroupMember_t;

struct ObjectGroup_t
{
    uint8_t GroupNum;
    ObjectGroupMember_t *Member;
		uint8_t Mode;
    ObjectGroup_t *Next;
};

struct ObjectGroupMember_t
{
    Object_t *Obj;
    ObjectGroupMember_t *Next;
};

#define ObjectGroup_GroupNum(Group) ((Group)->GroupNum)
#define ObjectGroup_Member(Group) ((Group)->Member)
#define ObjectGroup_Mode(Group) ((Group)->Mode)
#define ObjectGroup_Next(Group) ((Group)->Next)

#define ObjectGroupMember_Obj(Member) ((Member)->Obj)
#define ObjectGroupMember_Next(Member) ((Member)->Next)
#define ObjectGroupMember_NextObj(Member) (((Member)->Next)->Obj)


void vPressMove_Group(ObjectGroup_t *Group);

ObjectGroup_t *xObjectGroup_New(void);
ObjectGroup_t *xObjectGroup_GetTop(void);
ObjectGroup_t *xObjectGroup_Get(const uint8_t GroupNum);
ObjectControlStatic_t xObjectGroup_Delete(ObjectGroup_t **ObjGroup);
static uint8_t xObjectGroup_FindEmptyGroupNum(void);

ObjectControlStatic_t xObjectGroup_AddObj(ObjectGroup_t *ObjGroup, Object_t *Object);

uint8_t ucObjectGroup_GetGroupNum(void);
uint8_t ucObjectGroup_GetGroupObjNum(const ObjectGroup_t *ObjGroup);
Object_t *xObjectGroup_GetObj(const ObjectGroup_t *ObjGroup, const uint8_t ObjectNum);

ObjectControlStatic_t xObjectGroup_RemoveObj(ObjectGroup_t *ObjGroup, const Object_t *Object);
ObjectControlStatic_t xObjectGroup_RemoveObjFromAllGroup(const Object_t *Object);
uint8_t xObjectGroup_IsObjInGroup(const ObjectGroup_t *ObjGroup, const Object_t *Object);

#endif
