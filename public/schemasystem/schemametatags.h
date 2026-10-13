#ifndef SCHEMAMETATAGS_H
#define SCHEMAMETATAGS_H

#ifdef _WIN32
#pragma once
#endif

//--------------------------------------------------------------------------------------------------
// Every schema meta and codegen tag the SDK declares. schemacompiler parses it ahead of the inputs,
// META and TYPEMETA expand to nothing, so the marked headers need not include the tag headers
//--------------------------------------------------------------------------------------------------
#include "schemasystem/schema.h"
#include "schemasystem/propertyeditormetadata.h"
#include "saverestore/saverestoremetadata.h"
#include "kv3lib/kv3transfer_schema.h"
#include "entity2/entitymetadata.h"
#include "networksystem/networkvar.h"
#include "resourcefile/resourcetype.h"
#include "vdata/vdatametadata.h"
#include "pulse/pulsemetadata.h"
#include "particles/particlesmetadata.h"
#include "model/modelmetadata.h"
#include "vphysics/vphysicsmetadata.h"
#include "tools/fgdmetadata.h"
#include "game/server/aidebugsnapshotmetadata.h"
#include "vscript/ivscript.h"

#endif // SCHEMAMETATAGS_H
