#ifndef PARTICLESMETADATA_H
#define PARTICLESMETADATA_H

#ifdef _WIN32
#pragma once
#endif

//--------------------------------------------------------------------------------------------------
// Particle tags
//--------------------------------------------------------------------------------------------------
#include "schemasystem/schemametatag.h"

DECLARE_SCHEMA_META_TAG( MParticleMinVersion, META_TAG_ON_FIELD, META_VALUE( int ) );
DECLARE_SCHEMA_META_TAG( MParticleMaxVersion, META_TAG_ON_FIELD, META_VALUE( int ) );

#endif // PARTICLESMETADATA_H
