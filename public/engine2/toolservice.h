#ifndef TOOLSERVICE_H
#define TOOLSERVICE_H

#ifdef _WIN32
#pragma once
#endif

#include "engine2/itoolservice.h"

class CToolService : public CBaseEngineService< IToolService >
{
public:
	virtual ~CToolService() = 0;
};

#endif // TOOLSERVICE_H
