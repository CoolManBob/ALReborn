/*
	Notices: Copyright (c) NHN Studio 2003
	Created by: Bryan Jeong (2003/12/24)
 */

// AsObject.h: interface for the AsObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ASOBJECT_H__2DC84333_E86F_4E12_8A53_37359C33C0E2__INCLUDED_)
#define AFX_ASOBJECT_H__2DC84333_E86F_4E12_8A53_37359C33C0E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum EnumObjectType
{
	ASOBJECT_NONE		= 0,
	ASOBJECT_LISTENER,
	ASOBJECT_SOCKET,
	ASOBJECT_TIMER,
	ASOBJECT_SYSTEM
};

enum EnumSystemCommand
{
	ASSYSTEM_COMMAND_NONE	= 0,
	ASSYSTEM_COMMAND_TERMINATE
};

class AsObject  
{
private:
	EnumObjectType m_eType;

public:
	AsObject(EnumObjectType eType = ASOBJECT_NONE) : m_eType(eType) {};
	virtual ~AsObject() {};

	EnumObjectType GetType() {return m_eType;}
};

class AsSystemCommand : public AsObject
{
public:
	EnumSystemCommand m_eCommand;

public:
	AsSystemCommand() : AsObject(ASOBJECT_SYSTEM)
	{
	}
};

#endif // !defined(AFX_ASOBJECT_H__2DC84333_E86F_4E12_8A53_37359C33C0E2__INCLUDED_)
