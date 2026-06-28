//========= Source2 VProf reverse-engineered layouts =========================
//
// Reconstructed from CS2 libtier0.so (IDA, dump 28.05.2026) because the legacy
// CVProfile / CVProfNode declarations in vprof.h are the old (Source 2018)
// layout and DO NOT match the shipping Source2 tier0 (different ctors, sizes
// and offsets). These structs mirror the real in-memory layout and are meant
// for *reading* the live VProf tree (g_VProfCurrentProfile) at runtime.
//
// They live in their own namespace to avoid clashing with the legacy classes
// in vprof.h. Hooking scopes does NOT need these - use VProfScopeHelper /
// VPROF_BUDGET (see vprof.h). These are only for walking the node tree.
//
// Layout verified for the 28.05.2026 build; offsets can shift on CS2 updates.
// Unknown gaps are kept as explicit padding; the static_asserts below lock the
// confirmed field offsets and sizes.
//===========================================================================

#ifndef VPROF_SOURCE2_LAYOUT_H
#define VPROF_SOURCE2_LAYOUT_H

#ifdef _WIN32
#pragma once
#endif

#include <cstdint>
#include <cstddef>

namespace vprof_source2
{

// Call-site location embedded into every node (also a trailing arg of
// EnterScopeInternal[BudgetFlags]). Mangled as "CUtlSourceLocation".
struct CUtlSourceLocation
{
	const char *m_pFileName;
	int m_nLine;
	const char *m_pFunctionName;
};

// Describes a budget group at a call site. Matches vprof.h.
struct VProfBudgetGroupCallSite
{
	const char *m_pGroupName;
	int m_nFlags;
	int m_nGroupId; // index into the global call-site table
};

// CVProfNode (Source2). Verified via ctor @0x2d22b0, EnterScope @0x2d4cf0,
// MarkFrame @0x2d4000. Timing fields are raw 64-bit cycle counters.
struct CVProfNode
{
	const char *m_pszName;
	uint64_t m_CurScopeEnter; // rdtsc captured on EnterScope
	int32_t m_nRecursions; // reset to 0 on first (non-recursive) enter
	int32_t _pad0;
	int32_t m_nCurFrameCallsEnter; // ++ on EnterScope
	int32_t m_nCurFrameCalls; // consumed by MarkFrame
	uint64_t m_CurFrameTime;
	uint64_t m_CurFrameTimeLessChildren;
	int32_t m_nPrevFrameCalls;
	int32_t _pad1;
	uint64_t m_PrevFrameTime;
	uint64_t m_PrevFrameTimeLessChildren;
	int32_t m_nTotalCalls;
	int32_t _pad2;
	uint64_t m_TotalTime;
	uint64_t m_TotalTimeLessChildren;
	uint64_t m_PeakTime;
	uint64_t _unk0; // cleared in ctor (peak-less-children?)
	CVProfNode *m_pParent;
	CVProfNode *m_pChild;
	CVProfNode *m_pSibling;
	int32_t m_iBudgetGroupID; // index into the global call-site table (stride 4104)
	int32_t m_iClientData; // initialised to -1
	int32_t m_iUniqueNodeID; // s_iCurrentUniqueNodeID++
	int32_t _pad3;
	CUtlSourceLocation m_SourceLocation;
};

// CVProfile (Source2) - large (~27 KB). Verified via ctor @0x2e1480,
// EnterScopeNode @0x2d57e0, ExitScope @0x2d58f0. Only fields with confirmed
// semantics are named; the rest is explicit padding. g_VProfCurrentProfile is
// an instance of this.
struct CVProfile
{
	int32_t _hdr[2];
	int32_t m_GroupIDStack[1024]; // budget-group id stack (pushed in EnterScope)
	int32_t m_GroupIDStackDepth; // init 1
	int32_t m_GateDepth; // checked in ExitScope
	uint8_t m_fAtRoot; // true while at root (perf gate)
	uint8_t _pad0[7];
	CVProfNode *m_pCurNode; // init &m_Root
	CVProfNode m_Root; // embedded root node
	uint8_t _gap0[4400]; // internal: detail level, frame counters, budget-group table, ...
	uint64_t m_PeakDefault; // init 0x00C8000000000000
	uint8_t _counters[18440]; // 3 x 256-entry counter arrays (24 B each) + counts
	uint8_t _gap1[24];
	uint32_t m_TargetThreadId; // = ThreadGetCurrentId() of ctor thread
	uint32_t _pad1;
	void (*m_pOutputStream)(const char *, ...); // default = Msg
};

// Global budget-group call-site table referenced by CVProfNode::m_iBudgetGroupID.
// base = qword_511C68, count = dword_511C60, stride = 4104 bytes/entry.
struct VProfBudgetGroupEntry
{
	const char *m_pGroupName; // matched against VProfBudgetGroupCallSite.m_pGroupName
	int32_t m_nFlags; // OR'd with call-site flags
	uint8_t _rest[4092]; // per-thread accumulators / budget data (TBD)
};

// Self-validation of the reverse-engineered offsets/sizes (clangd shows the
// offsets in the editor; these enforce them at compile time).
static_assert( sizeof(CUtlSourceLocation) == 24, "CUtlSourceLocation layout" );
static_assert( sizeof(VProfBudgetGroupCallSite) == 16, "VProfBudgetGroupCallSite layout" );

static_assert( sizeof(CVProfNode) == 176, "CVProfNode size" );
static_assert( offsetof(CVProfNode, m_CurFrameTime) == 32, "node m_CurFrameTime" );
static_assert( offsetof(CVProfNode, m_TotalTime) == 80, "node m_TotalTime" );
static_assert( offsetof(CVProfNode, m_PeakTime) == 96, "node m_PeakTime" );
static_assert( offsetof(CVProfNode, m_pParent) == 112, "node m_pParent" );
static_assert( offsetof(CVProfNode, m_pChild) == 120, "node m_pChild" );
static_assert( offsetof(CVProfNode, m_pSibling) == 128, "node m_pSibling" );
static_assert( offsetof(CVProfNode, m_iBudgetGroupID) == 136, "node m_iBudgetGroupID" );
static_assert( offsetof(CVProfNode, m_iUniqueNodeID) == 144, "node m_iUniqueNodeID" );
static_assert( offsetof(CVProfNode, m_SourceLocation) == 152, "node m_SourceLocation" );

static_assert( offsetof(CVProfile, m_GroupIDStack) == 8, "profile group stack" );
static_assert( offsetof(CVProfile, m_GroupIDStackDepth) == 4104, "profile stack depth" );
static_assert( offsetof(CVProfile, m_fAtRoot) == 4112, "profile m_fAtRoot" );
static_assert( offsetof(CVProfile, m_pCurNode) == 4120, "profile m_pCurNode" );
static_assert( offsetof(CVProfile, m_Root) == 4128, "profile m_Root" );
static_assert( offsetof(CVProfile, m_PeakDefault) == 8704, "profile peak default" );
static_assert( offsetof(CVProfile, m_TargetThreadId) == 27176, "profile m_TargetThreadId" );
static_assert( offsetof(CVProfile, m_pOutputStream) == 27184, "profile m_pOutputStream" );

} // namespace vprof_source2

#endif // VPROF_SOURCE2_LAYOUT_H
