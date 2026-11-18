
// #pragma once
#ifndef _NETWORKPROFILER_
#define _NETWORKPROFILER_


#include "Engine.h"

#ifndef FINAL_BUILD
#define NETWORK_PROFILER
#endif

#include "Core/DynArray.h"

#define NPROF(x) GEngine->Net->Profiler.Sent(x)
#define NPROF2(x,y) GEngine->Net->Profiler.Sent(x,y)

enum NetworkProfilerVariables
{
	NPIsZombie,					NPEntityType,					NPSynchronizerNumber,	NPAnimLooped,					NPAnimHasMoveCurve,
	NPOtherTypeMarker,			NPPOUpdateSynchroMarker,		NPScriptWriteByte,		NPScriptWriteChar,				NPScriptWriteString,
	NPScriptWriteULong,			NPScriptWriteUShort,			NPScriptWriteLong,		NPScriptWriteFloat,				NPAnimNumber,
	NPAnimName,					NPAnimBlendTime,				NPAnimTimeScale,		NPPOFreedomOfRot,				NPIPChannelMsgTypeMarker,
	NPScriptMessageSize,		NPAnimsCount,					NPActiveAnimsCount,		NPActiveAnimIndex,				NPActiveAnimWeight,
	NPActiveAnimTimeScale,		NPActiveAnimFrameBlendTime,		NPActiveAnimBlendTime,	NPPORotation,					NPCollGroupIndex,
	NPPOBodyType,				NPIPChannelTextMessageString,	NPActiveMeshIndex,		NPCreateEntitiesCount,			NPDeleteEntitiesCount,
	NPUpdateEntitiesCount,		NPScriptMessagesCount,			NPOtherMessagesCount,	NPPOState,						NPStringHashString,
	NPBasicOOBString,			NPEntityFlags,					NPEntityScale,			NPPOVelocity,					NPPOAngularVelocity,
	NPEntityRotation,			NPEntityPosition,				NPPOPosition,			NPRagdollLimbState,				NPRagdollLimbPosition,
	NPRagdollLimbOrientation,	NPRagdollLimbCount,				NPRagdollLimbVelocity,	NPRagdollLimbAngularVelocity,	NPRagdollLimbPinned,
	NPRagdollLinearDamping,		NPRagdollAngularDamping,		NPRagdollMass,			NPRagdollCollisionGroup,		NPPOActionCount,
	NPPOActionData,				NPModelHasAnim,					NPPOClampingObject,		NPUpdateClass,					NPCRCChecksum,
	NPPOFriction,				NPPORestitution,				NPPOMass,				NPPOCreationData,
	NP_LAST,	// This one must be the last
};

#ifdef NETWORK_PROFILER

class ENGINE_API NetworkProfiler
{
public:
	NetworkProfiler( void ) { Clear(); };
	~NetworkProfiler( void ) { LogReport(); };

	inline void Sent( unsigned short variable, int count = 1 ) { VariableTimesSent[variable] += count; };
	inline unsigned long Remember( unsigned short variable ) { return VariableTimesSent[variable]; };
	inline void Remind( unsigned short variable, unsigned long count ) { VariableTimesSent[variable] = count; };
	void Clear( void ) { VariableTimesSent.ClearArray(); while( VariableTimesSent.Size() < NP_LAST ) VariableTimesSent.Add(0); };
	void LogReport( void );

protected:
	DynamicArray<DWORD> VariableTimesSent;
};

#else
// not defined NETWORK_PROFILER

class ENGINE_API NetworkProfiler
{
public:
	NetworkProfiler( void ) {};
	~NetworkProfiler( void ) {};

	inline void Sent( unsigned short variable, int count = 1 ) {};
	inline unsigned long Remember( unsigned short variable ) { return 0; };
	inline void Remind( unsigned short variable, unsigned long count ) {};
	void Clear( void ) {};
	void LogReport( void ) {};
};

#endif


#endif //_NETWORKPROFILER_


