#ifndef TIER1APP_H
#define TIER1APP_H

#ifdef _WIN32
#pragma once
#endif

#include "appframework/appsystemdict.h"

class CTier1AppSystemDict : public CAppSystemDict
{
public:
	bool m_bNoExeCheck;
};

#endif // TIER1APP_H
