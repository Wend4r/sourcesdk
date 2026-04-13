#ifndef IENTITYLISTENER_H
#define IENTITYLISTENER_H

#pragma once

class CEntityInstance;

class IEntityListener
{
public:
	virtual void OnEntityCreated(CEntityInstance* pEntity) {};
	virtual void OnEntitySpawned(CEntityInstance* pEntity) {};
	virtual void OnEntityDeleted(CEntityInstance* pEntity) {};
	virtual void OnEntityParentChanged(CEntityInstance* pEntity, CEntityInstance* pNewParent) {};
};

#endif // !defined( IENTITYLISTENER_H )