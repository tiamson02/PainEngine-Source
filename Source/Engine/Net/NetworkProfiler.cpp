
#include "Engine.h"
#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;
#include "Net/NetworkProfiler.h"

#ifdef NETWORK_PROFILER

char *NetworkProfilerVariableNames[] = {
	"IsZombie", "EntityType", "SynchronizerNumber", "AnimLooped", "AnimHasMoveCurve",	// 5
	"OtherCommandTypeMarker", "POUpdateSynchroMarker","Script: WriteByte","Script: WriteChar","Script: WriteString",
	"Script: WriteULong", "Script: WriteUShort", "Script: WriteLong", "Script: WriteFloat", "AnimNumber",
	"AnimName","AnimBlendTime", "AnimTimeScale","POFreedomOfRotation", "IPChannelMsgTypeMarker",
	"Script: MessageSize", "AnimsCount", "ActiveAnimsCount", "ActiveAnimIndex", "ActiveAnimWeight",
	"ActiveAnimTimeScale", "ActiveAnimFrameBlendTime", "ActiveAnimBlendTime", "PORotation", "POCollisionGroupIndex",
	"POBodyType", "IPChannelTextMessageString", "ActiveMeshIndex", "CreateEntitiesCount", "DeleteEntitiesCount",
	"UpdateEntitiesCount", "ScriptMessagesCount", "OtherMessagesCount", "POState", "StringHash: String",
	"IPDriver: Basic Out-Of-Band Message", "EntityFlags", "EntityScale", "POVelocity", "POAngularVelocity",
	"EntityRotation", "EntityPosition", "POPosition", "RagdollLimbStates", "RagdollLimbPosition",
	"RagdollLimbOrientation", "RagdollLimbCount", "RagdollLimbVelocity", "RagdollLimbAngularVelocity", "RagdollLimbPinned",
	"RagdollLinearDamping", "RagdollAngularDamping", "RagdollMass", "RagdollCollisionGroup", "POActionCount",
	"POActionData", "ModelHasAnim", "POIsClamping", "UpdateClass", "CRCChecksum",
	"POFriction", "PORestitution", "POMass", "POCreationData",
};

short NetworkProfilerVariableSizes[] = {
	1, 1, 1, 1, 1,
	1, 1, 1, 1, 1,
	4, 2, 4, 4, 1,
	1, 4, 4, 1,	1,
	2, 1, 1, 1, 4,
	4, 4, 4, 1, 1,
	1, 1, 2, 2, 2,
	2, 2, 2, 2, 1,
	1, 4, 4, 4,12,
	6, 8, 8, 4, 8,
	6, 1, 8, 8, 1,
	4, 4, 4, 1, 1,
	1, 1, 1, 1, 4,
	4, 4, 4, 1,
};

LogBuffer NetworkProfilerLog( "netProfiler.log" );

void NetworkProfiler::LogReport( void )
{
	// Calculate summaric bytes sent
	int i = 0;
	DWORD sum = 0;
	for( ; i < VariableTimesSent.Size(); ++i )
		sum += VariableTimesSent[i]*NetworkProfilerVariableSizes[i];

	// Print report
	NetworkProfilerLog.Print( "\nBUILT-IN NETWORK PROFILER REPORT\n\n" );
	NetworkProfilerLog.Print( "Bytes taken by various sent variables\n" );
	NetworkProfilerLog.Print( "-----------------------------------\n" );

	for( i = 0; i < VariableTimesSent.Size(); ++i )
		NetworkProfilerLog.Print( "%40s: sent %7d times ( %9d bytes ) ( %9.4f%% of bandwidth taken )\n",
			NetworkProfilerVariableNames[i],
			VariableTimesSent[i],
			VariableTimesSent[i]*NetworkProfilerVariableSizes[i],
			(((double)VariableTimesSent[i]*NetworkProfilerVariableSizes[i])/sum)*100.0 );

	for( ; i < NP_LAST; ++i )
		NetworkProfilerLog.Print( "%40s: sent %7d times ( %9d bytes ) ( %9.4f%% of bandwidth taken )\n",
			NetworkProfilerVariableNames[i],
			0, 0, 0.0f );

	NetworkProfilerLog.Print( "-----------------------------------\n" );
}

#endif // NETWORK_PROFILER
