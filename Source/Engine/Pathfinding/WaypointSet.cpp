// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/07 Krystian Galaj
//----------------------------------------------------------------------------

#include "Engine.h"

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "WaypointSet.h"
#include "PathfindingNode.h"
#include "Pathfinder.h"
#include "PointerHeap.h"
#include "PathfinderPortalNode.h"
#include "Render/Viewport.h"
#include "Engine/Portal.h"
#include "Engine/Mesh.h"
#include "Engine/World.h"

#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;

WaypointSet::WaypointSet(void)
{
	mDistances = 0;					// distance information not prepared at the start
	mPortalConnections = 0;			// portal connections not generated
	mFloors = 0;					// no floors exist in this waypoint set

	mHowManyDistances = 0;
	mPackedIndices = 0;
	mPackedBase = 0;
	mPackedCount = 0;
}

WaypointSet::~WaypointSet(void)
{
	ClearContents();
}

void WaypointSet::ClearContents( void )
{
	ClearConnections();
	ClearDistances();
	ClearFloors();
	mWaypoints.ClearArray();
}

int WaypointSet::AddWaypoint( Waypoint waypoint )
{
	int size = mWaypoints.Size();
	int i, j;
	if( mPackedBase )
	{
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		int* newPackedBase = new int[ size+1 ];
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		int* newPackedCount = new int[ size+1 ];
		for( int i = 0; i < size; i++ )
		{
			newPackedBase[i] = mPackedBase[i];
			newPackedCount[i] = mPackedCount[i];
		};
		newPackedBase[size] = mHowManyDistances;
		newPackedCount[size] = 0;

		// Adjust selected waypoints
		BitSet newSelected;
		newSelected.Resize( mWaypoints.Size()+1 );
		newSelected.CopyFrom( mSelected );
		mWaypoints.Add( waypoint );
		size = mWaypoints.Size();
		mSelected.Resize( size );
		mSelected.CopyFrom( newSelected );

		// Adjust big monsters allowance
		BitSet newBigMonsters;
		newBigMonsters.Resize( size );
		newBigMonsters.CopyFrom( mBigMonsters );
		mBigMonsters.Resize( size );
		mBigMonsters.CopyFrom( newBigMonsters );

		// Adjust disabled waypoints allowance
		mDisabledWaypoints.Resize( size );

		delete[] mPackedBase;
		mPackedBase = newPackedBase;
		delete[] mPackedCount;
		mPackedCount = newPackedCount;
	}
	else
	{
		// Adjust selected waypoints
		BitSet newSelected;
		newSelected.Resize( mWaypoints.Size()+1 );
		newSelected.CopyFrom( mSelected );
		mWaypoints.Add( waypoint );
		size = mWaypoints.Size();
		mSelected.Resize( size );
		mSelected.CopyFrom( newSelected );

		// Adjust big monsters allowance
		BitSet newBigMonsters;
		newBigMonsters.Resize( size );
		newBigMonsters.CopyFrom( mBigMonsters );
		mBigMonsters.Resize( size );
		mBigMonsters.CopyFrom( newBigMonsters );

		mDisabledWaypoints.Resize( size );

		if( mDistances )
		{
#ifdef TRANSGAMING_PSP
            m_setTag("AI");
#endif
			float* newDist = new float[size*size];

			// Fill all previous distances
			for( i = 0; i < size-1; i++ )
			{
				for( j = 0; j < size-1; j++ )
				{
					newDist[i*size+j] = mDistances[i*(size-1)+j];
				}
			};

			// Fill the rest
			for( i = 0; i < size; i++ )
			{
				newDist[i*size+size-1] = newDist[(size-1)*size+i] = -1;
			};

			// Switch tables
			delete[] mDistances;
			mDistances = newDist;
		};
	};

	return size-1;
}

int WaypointSet::GetWaypointIndex( Waypoint& waypoint )
{
	for( int i = 0; i < mWaypoints.Size(); i++ )
	{
		if( mWaypoints[i] == waypoint )
		{
			return i;
		};
	};
	return -1;
}

void WaypointSet::RemoveWaypoint( Waypoint& waypoint )
{
	int i = GetWaypointIndex( waypoint );
	if( i == -1 )
	{
		return;	// not found
	};
	RemoveWaypoint(i);
}

void WaypointSet::RemoveWaypoint( int index )
{
	UnselectUnique( index );
	int size = mWaypoints.Size();
	for( int i = index; i < size-1; i++ )
	{
		mWaypoints[i] = mWaypoints[i+1];
		if( mSelected.Test(i+1) )
		{
			mSelected.Set(i);
		}
		else
		{
			mSelected.Clear(i);
		};

		if( mBigMonsters.Test(i+1) )
		{
			mBigMonsters.Set(i);
		}
		else
		{
			mBigMonsters.Clear(i);
		};
	};

	mWaypoints.RemoveLast();
	size = mWaypoints.Size();

	if( mDistances )
	{
		if( mPackedBase )
		{
			int newHowManyDistances = mHowManyDistances - mPackedCount[index];
			for( int i = 0; i < mHowManyDistances; i++ )
			{
				if( (mPackedIndices[i] == index) &&
					( ( i < mPackedBase[index] ) ||
					( i - mPackedBase[index] >= mPackedCount[index] ) ) )
				{
					newHowManyDistances--;
				};
			};

			if( newHowManyDistances == 0 )
			{
				mHowManyDistances = 0;
				delete[] mDistances;
				mDistances = 0;
				delete[] mPackedIndices;
				mPackedIndices = 0;
				delete[] mPackedBase;
				mPackedBase = 0;
				delete[] mPackedCount;
				mPackedCount = 0;
			}
			else
			// The lines below should have been faster in some cases than what is left of
			// the algorithm; however, they introduced a volatile, hard-to-find bug,
			// and speed in this algorithm is not needed.
/*			if( newHowManyDistances == mHowManyDistances )
			{
				int* newPackedBase = new int[size];
				int* newPackedCount = new int[size];

				for( int i = 0; i < index; i++ )
				{
					newPackedBase[i] = mPackedBase[i];
					newPackedCount[i] = mPackedCount[i];
				};
				for( int i = index+1; i < size+1; i++ )
				{
					newPackedBase[i-1] = mPackedBase[i];
					newPackedCount[i-1] = mPackedCount[i];
				};

				delete[] mPackedBase;
				mPackedBase = newPackedBase;
				delete[] mPackedCount;
				mPackedCount = newPackedCount;
			}
			else*/
			{
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
				float* newDist = new float[ newHowManyDistances ];
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
				int* newPackedIndices = new int[ newHowManyDistances ];
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
				int* newPackedBase = new int[size];
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
				int* newPackedCount = new int[size];

				int newDistPtr = 0;

				for( int i = 0; i < index; i++ )
				{
					newPackedBase[i] = newDistPtr;
					int base = mPackedBase[i];
					for( int j = 0; j < mPackedCount[i]; j++ )
					{
						int dest = mPackedIndices[base+j];
						if( dest != index )
						{
							newDist[newDistPtr] = mDistances[base+j];
							newPackedIndices[newDistPtr] = (dest>index)?dest-1:dest;
							newDistPtr++;
						};
					};
					newPackedCount[i] = newDistPtr-newPackedBase[i];
				};

				for( int i = index+1; i < size+1; i++ )
				{
					newPackedBase[i-1] = newDistPtr;
					int base = mPackedBase[i];
					for( int j = 0; j < mPackedCount[i]; j++ )
					{
						int dest = mPackedIndices[base+j];
						if( dest != index )
						{
							newDist[newDistPtr] = mDistances[base+j];
							newPackedIndices[newDistPtr] = (dest>index)?dest-1:dest;
							newDistPtr++;
						};
					};
					newPackedCount[i-1] = newDistPtr-newPackedBase[i-1];
				};

				if( newDistPtr != newHowManyDistances )
				{
					GLog.Print("Alert! Bug in RemoveWaypoint()!");
				};

				mHowManyDistances = newHowManyDistances;
				delete[] mDistances;
				mDistances = newDist;
				delete[] mPackedIndices;
				mPackedIndices = newPackedIndices;
				delete[] mPackedBase;
				mPackedBase = newPackedBase;
				delete[] mPackedCount;
				mPackedCount = newPackedCount;
			};
		}
		else
		{
#ifdef TRANSGAMING_PSP
            m_setTag("AI");
#endif
			float* newDist = new float[size*size];

			// Fill all remaining distances
			for( int i = 0; i < index; i++ )
			{
				for( int j = 0; j < index; j++ )
				{
					newDist[i*size+j] = mDistances[i*(size+1)+j];
				};
				for( int j = index+1; j <= size; j++ )
				{
					newDist[i*size+j-1] = mDistances[i*(size+1)+j];
				};
			};
			for( int i = index+1; i <= size; i++ )
			{
				for( int j = 0; j < index; j++ )
				{
					newDist[(i-1)*size+j] = mDistances[i*(size+1)+j];
				};
				for( int j = index+1; j <= size; j++ )
				{
					newDist[(i-1)*size+j-1] = mDistances[i*(size+1)+j];
				};
			};

			// Switch tables
			delete[] mDistances;
			mDistances = newDist;
		};
	};
}

void WaypointSet::ClearDistances( void )
{
	if( mDistances )
	{
		delete[] mDistances;
		mDistances = 0;
	}

	if( mPackedBase )
	{
		mHowManyDistances = 0;
		delete[] mPackedIndices;
		mPackedIndices = 0;
		delete[] mPackedBase;
		mPackedBase = 0;
		delete[] mPackedCount;
		mPackedCount = 0;
	};
}

int WaypointSet::LoadContents( FileObject* f, DynamicArray<int>* portalTranslator, float scale )
{
	// Read waypoint number
	int size;
	if( !f->Read( &size, sizeof(int) ) )
	{
		return 0;
	};

//	GLog.Print( "Waypoint set: %d waypoints\n", size );

	// Read waypoints
	Waypoint wp;
	for( int i = 0; i < size; i++ )
	{
		if( !wp.ReadFrom(f) )
		{
			return 0;
		};
		wp.mPosition *= scale;
		mWaypoints.Add( wp );
	};
	mSelected.Resize(mWaypoints.Size());
	mBigMonsters.Resize(mWaypoints.Size());
	mDisabledWaypoints.Resize(mWaypoints.Size());

	// Read big monsters bitset
	int bmBits;
	if( !f->Read( &bmBits, sizeof(int) ) )
	{
		return 0;
	};
	if( bmBits )
	{
		mBigMonsters.Resize( (bmBits-1)*32 );
		if( !f->Read( mBigMonsters.GetPtrToData(), sizeof(DWORD)*bmBits ) )
		{
			return 0;
		};
	}
	else
	{
		mBigMonsters.Release();
	};

	// Read marker - distances here?
	char are;
	if( !f->Read( &are, 1 ) )
	{
		return 0;
	};
	ClearDistances();

	// Read distances if needed - packed representation
	if( are )
	{
        size = mWaypoints.Size();
		if( !f->Read( &mHowManyDistances, sizeof(int) ) )
		{
			mHowManyDistances = 0;
			return 0;
		};

//		GLog.Print( "Waypoint set: %d distances\n", mHowManyDistances );

#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		mDistances = new float[mHowManyDistances];
		if( !f->Read( mDistances, sizeof(int)*mHowManyDistances ) )
		{
			mHowManyDistances = 0;
			delete[] mDistances;
			mDistances = 0;
			return 0;
		};

		for( int i = 0; i < mHowManyDistances; i++ )
		{
			mDistances[i] *= scale;
		};

#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		mPackedIndices = new int[mHowManyDistances];
		if( !f->Read( mPackedIndices, sizeof(int)*mHowManyDistances ) )
		{
			mHowManyDistances = 0;
			delete[] mDistances; mDistances = 0;
			delete[] mPackedIndices; mPackedIndices = 0;
			return 0;
		};

#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		mPackedBase = new int[size];
		if( !f->Read( mPackedBase, sizeof(int)*size ) )
		{
			mHowManyDistances = 0;
			delete[] mDistances; mDistances = 0;
			delete[] mPackedIndices; mPackedIndices = 0;
			delete[] mPackedBase; mPackedBase = 0;
			return 0;
		};

#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		mPackedCount = new int[size];
		if( !f->Read( mPackedCount, sizeof(int)*size ) )
		{
			mHowManyDistances = 0;
			delete[] mDistances; mDistances = 0;
			delete[] mPackedIndices; mPackedIndices = 0;
			delete[] mPackedBase; mPackedBase = 0;
			delete[] mPackedCount; mPackedCount = 0;
			return 0;
		};
	};

	ClearConnections();
	// Read marker if portal connections are here
	if( !f->Read( &are, 1 ) )
	{
		return 0;
	};

	// Read number of portals
	int numPortals;
	if( !f->Read( &numPortals, sizeof(int) ) )
	{
		return 0;
	};

	// Read portal indices and convert them to portal pointers
	int missedPortals = 0;
	float* oldPortalNumbers = 0;
    if( numPortals ){
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		oldPortalNumbers = new float[numPortals];
    }

	for( int i = 0; i < numPortals; i++ )
	{
		int nodeIndex;
		if( !f->Read( &nodeIndex, sizeof(int) ) )
		{
			return 0; 
		};
		if( portalTranslator )	// temporarily; to be recalculated later
		{
			oldPortalNumbers[i] = nodeIndex;
			if( (*portalTranslator)[nodeIndex] != -1 )
				mPortalNodes.Add( (PathfinderPortalNode*)((*portalTranslator)[nodeIndex]) );
			else
				missedPortals++;
		}
		else
			mPortalNodes.Add( (PathfinderPortalNode*)nodeIndex );
	};

	// Read portal connections
	if( numPortals )
	{
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		mPortalConnections = new float[numPortals*numPortals];
		if( !f->Read( mPortalConnections, sizeof(float)*numPortals*numPortals ) )
		{
			if( oldPortalNumbers )
				delete[] oldPortalNumbers;
			return 0; 
		};

		for( int i = 0; i < numPortals*numPortals; i++ )
		{
			mPortalConnections[i] *= scale;
		};
		mPortalConnectionsForBigMonsters.Resize( numPortals*numPortals );

		// Read portal connections for big monsters
		int t3;
		if( !f->Read( &t3, sizeof(int) ) )
		{
			if( oldPortalNumbers )
				delete[] oldPortalNumbers;
			return 0;
		};
		if( bmBits )
		{
			mPortalConnectionsForBigMonsters.Resize( (t3-1)*32 );
			if( !f->Read( mPortalConnectionsForBigMonsters.GetPtrToData(), sizeof(DWORD)*t3 ) )
			{
				if( oldPortalNumbers )
					delete[] oldPortalNumbers;
				return 0;
			};
		}
		else
		{
			mPortalConnectionsForBigMonsters.Release();
		};

		if( missedPortals )	// we have to move connections excluding missed portals
		{
			if( missedPortals == numPortals )
			{
				GLog.Print( "Warning: missed all portals!!!\n" );
				mPortalConnectionsForBigMonsters.Resize( 0 );
				delete[] mPortalConnections;
				mPortalConnections = 0;
				if( oldPortalNumbers )
					delete[] oldPortalNumbers;
				return 1;
			}
			else
			{
				int realPortals = numPortals - missedPortals;
				GLog.Print( "Warning: missed %d portals!!!\n", missedPortals );
#ifdef TRANSGAMING_PSP
                m_setTag("AI");
#endif
				float* portalConnections = new float[realPortals*realPortals];
				BitSet pcfbm;
				pcfbm.Resize( realPortals*realPortals );

				int missedA = 0;
				for( int a = 0; a < numPortals; ++a )
				{
					if( (*portalTranslator)[(int)(oldPortalNumbers[a])] == -1 )
					{
						missedA++;
						continue;
					}
					else
					{
						int missedB = 0;
						for( int b = 0; b < numPortals; ++b )
						{
							if( (*portalTranslator)[(int)(oldPortalNumbers[b])] == -1 )
							{
								missedB++;
								continue;
							}
							else
							{
								portalConnections[ (a-missedA)*realPortals+(b-missedB) ] =
									mPortalConnections[ a*numPortals + b ];
								if( mPortalConnectionsForBigMonsters.Test( a*numPortals + b ) )
									pcfbm.Set( (a-missedA)*realPortals+(b-missedB) );
								else
									pcfbm.Clear( (a-missedA)*realPortals+(b-missedB) );
							}
						}
					}
				}

				// Switch new structures for old
				delete[] mPortalConnections;
				mPortalConnections = portalConnections;
				mPortalConnectionsForBigMonsters.MakeCompleteCopy( pcfbm );
			}
		}
	};

	if( oldPortalNumbers )
		delete[] oldPortalNumbers;
	return 1;
}
/*
int WaypointSet::Save( Pathfinder* pf, const char* filename, float scale )
{
	if( !strlen( filename ) )
	{
		return 0;
	};

	FILE* f = fopen(filename, "wb" );
	if( !f )
	{
		return 0;
	};

	if( !SaveContents( pf, f, scale ) ) 
	{
		fclose(f);
		return 0;
	}
	else
	{
		return 1;
	};
}

int WaypointSet::SaveContents( Pathfinder* pf, FILE* f, float scale )
{
	// Write waypoint number
	int size = mWaypoints.Size();
	if( fwrite( &size, sizeof(int), 1, f ) != 1 )
	{
		return 0;
	};

	// Write waypoints
	for( int i = 0; i < mWaypoints.Size(); i++ )
	{
		if( !mWaypoints[i].WriteTo(f, scale ) )
		{
			return 0;
		};
	};

	// Write big monsters bitset
	int bmBits = mBigMonsters.GetSize();
	if( fwrite( &bmBits, sizeof(int), 1, f ) != 1 )
	{
		return 0;
	};

	if( bmBits )
	{
		if( fwrite( mBigMonsters.GetPtrToData(), sizeof(DWORD), bmBits, f ) != (unsigned int)bmBits )
		{
			return 0;
		};
	};

	// Write marker if distances are here
	char are = (mDistances != 0);
	if( fwrite( &are,1,1,f ) != 1 )
	{
		return 0; 
	};

	// Write distances
	if( mDistances )
	{
		// Pack distances, if unpacked
		PackDistances();

		if( fwrite( &mHowManyDistances,sizeof(int),1,f ) != 1 )
		{
			return 0; 
		};

		float* t = new float[mHowManyDistances];
		::memcpy(t, mDistances, mHowManyDistances*sizeof(float) );
		for( int i = 0; i < mHowManyDistances; i++ )
		{
			t[i] *= scale;
		};

		if( fwrite( mDistances, sizeof(float), mHowManyDistances, f ) != (unsigned int)(mHowManyDistances) )
		{
			delete[] t;
			return 0;
		};
		delete[] t;

		if( fwrite( mPackedIndices, sizeof(int), mHowManyDistances, f ) != (unsigned int)(mHowManyDistances) )
		{
			return 0;
		};

		if( fwrite( mPackedBase, sizeof(float), size, f ) != (unsigned int)(size) )
		{
			return 0;
		};

		if( fwrite( mPackedCount, sizeof(float), size, f ) != (unsigned int)(size) )
		{
			return 0;
		};
	};

	// Write marker if portal connections are here
	are = (mPortalConnections != 0);
	if( fwrite( &are,1,1,f ) != 1 )
	{
		return 0; 
	};

	// Write number of portals
	int numPortals = mPortalNodes.Size();
	if( fwrite( &numPortals,sizeof(int),1,f ) != 1 )
	{
		return 0; 
	};

	// Write portal indices
	PathfinderPortalNode* base = &pf->GetNodeForPortal(0);
	for( int i = 0; i < numPortals; i++ )
	{
		int nodeIndex = mPortalNodes[i]-base;
		if( fwrite( &nodeIndex, sizeof(int), 1, f ) != 1 )
		{
			return 0; 
		};
	};

	// Write portal connections
	if( numPortals )
	{
		float* t2 = new float[numPortals*numPortals];
		::memcpy(t2, mPortalConnections, numPortals*numPortals*sizeof(float) );
		for( int i = 0; i < numPortals*numPortals; i++ )
		{
			t2[i] *= scale;
		};
		if( fwrite( t2, sizeof(float), numPortals*numPortals, f ) != (unsigned int)(numPortals*numPortals) )
		{
			return 0; 
		};
		delete[] t2;

		// Write portal connections for big monsters
		int t3 = mPortalConnectionsForBigMonsters.GetSize();
		if( fwrite( &t3, sizeof(int), 1, f ) != 1 )
		{
			return 0;
		};
		if( bmBits )
		{
			if( fwrite( mPortalConnectionsForBigMonsters.GetPtrToData(), sizeof(DWORD), t3, f ) != (unsigned int)t3 )
			{
				return 0;
			};
		};
	};

	return 1;
}
*/

int WaypointSet::SaveContents( Pathfinder* pf, FileObject* f, float scale )
{
	// Write waypoint number
	int size = mWaypoints.Size();
	if( !f->Write( &size, sizeof(int) ) )
	{
		return 0;
	};

	// Write waypoints
	for( int i = 0; i < mWaypoints.Size(); i++ )
	{
		if( !mWaypoints[i].WriteTo(f, scale ) )
		{
			return 0;
		};
	};

	// Write big monsters bitset
	int bmBits = mBigMonsters.GetSize();
	if( !f->Write( &bmBits, sizeof(int) ) )
	{
		return 0;
	};

	if( bmBits )
	{
		if( !f->Write( mBigMonsters.GetPtrToData(), sizeof(DWORD)*bmBits ) )
		{
			return 0;
		};
	};

	// Write marker if distances are here
	char are = (mDistances != 0);
	if( !f->Write( &are, 1 ) )
	{
		return 0; 
	};

	// Write distances
	if( mDistances )
	{
		// Pack distances, if unpacked
		PackDistances();

		if( !f->Write( &mHowManyDistances, sizeof(int) ) )
		{
			return 0; 
		};

#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		float* t = new float[mHowManyDistances];
		::memcpy(t, mDistances, mHowManyDistances*sizeof(float) );
		for( int i = 0; i < mHowManyDistances; i++ )
		{
			t[i] *= scale;
		};

		if( !f->Write( mDistances, sizeof(float)*mHowManyDistances ) )
		{
			delete[] t;
			return 0;
		};
		delete[] t;

		if( !f->Write( mPackedIndices, sizeof(int)*mHowManyDistances ) )
		{
			return 0;
		};

		if( !f->Write( mPackedBase, sizeof(float)*size ) )
		{
			return 0;
		};

		if( !f->Write( mPackedCount, sizeof(float)*size ) )
		{
			return 0;
		};
	};

	// Write marker if portal connections are here
	are = (mPortalConnections != 0);
	if( !f->Write( &are, 1 ) )
	{
		return 0; 
	};

	// Write number of portals
	int numPortals = mPortalNodes.Size();
	if( !f->Write( &numPortals, sizeof(int) ) )
	{
		return 0; 
	};

	// Write portal indices
	PathfinderPortalNode* base = &pf->GetNodeForPortal(0);
	for( int i = 0; i < numPortals; i++ )
	{
		int nodeIndex = mPortalNodes[i]-base;
		if( !f->Write( &nodeIndex, sizeof(int) ) )
		{
			return 0; 
		};
	};

	// Write portal connections
	if( numPortals )
	{
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		float* t2 = new float[numPortals*numPortals];
		::memcpy(t2, mPortalConnections, numPortals*numPortals*sizeof(float) );
		for( int i = 0; i < numPortals*numPortals; i++ )
		{
			t2[i] *= scale;
		};
		if( !f->Write( t2, sizeof(float)*numPortals*numPortals ) )
		{
			return 0; 
		};
		delete[] t2;

		// Write portal connections for big monsters
		int t3 = mPortalConnectionsForBigMonsters.GetSize();
		if( !f->Write( &t3, sizeof(int) ) )
		{
			return 0;
		};
		if( bmBits )
		{
			if( !f->Write( mPortalConnectionsForBigMonsters.GetPtrToData(), sizeof(DWORD)*t3 ) )
			{
				return 0;
			};
		};
	};

	return 1;
}

int WaypointSet::LoadFloors( FILE* f, float scale )
{
	if( !mFloors )
	{
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		mFloors = new PathfinderFloorSet;
	};
	return mFloors->Load( f, scale );
}

int WaypointSet::SaveFloors( FILE* f, float scale )
{
	if( !mFloors )
	{
		return 0;
	};
	return mFloors->Save( f, scale );
}

// Assuming distances table valid.
// Begin with searchPtr == 0, iterate, ends when dist == -1 and returns 0 then
// searchPtr-1 is an index of last returned waypoint.

Waypoint* WaypointSet::GetNextAvailableFrom( int from, int& searchPtr, float& dist )
{
	if( !mDistances )
	{
		return 0;
	};
	if( !mPackedBase )	// Search in unpacked data
	{
		int distPtr = from*mWaypoints.Size()+searchPtr;
		do
		{
			dist = mDistances[distPtr];
			searchPtr++;
			distPtr++;
			if( searchPtr > mWaypoints.Size() )
			{
				dist = -1;
				return 0;
			};
		}
		while( dist <= 0 );

		return &mWaypoints[searchPtr-1];
	}
	else	// Search in packed data
	{
		int index = mPackedBase[from];
		int count = mPackedCount[from];
		while( count )
		{
			if( mPackedIndices[index] >= searchPtr )
			{
				searchPtr = mPackedIndices[index]+1;
				dist = mDistances[index];
				return &mWaypoints[searchPtr-1];
			};
			index++;
			count--;
		};
		dist = -1;
		return 0;
	};
}

float WaypointSet::GetShortestPath( int from, int* to, int toHowMany, AStarHeuristicFunction heuristic, WaypointPath& path, int bigMonsters, Vector* targetForHeur, float maxLenToTry )
{
	// Safeguards
	if( !mWaypoints.Size() || !mDistances || !toHowMany )
	{
		return FLT_MAX;
	};

	Waypoint targetWaypointForHeur;
	targetWaypointForHeur.mPosition = ( targetForHeur == 0 )?mWaypoints[to[0]].mPosition:*targetForHeur;

	BitSet inClosed;
	inClosed.Resize( mWaypoints.Size() );
	BitSet inOpen;
	inOpen.Resize( mWaypoints.Size() );

	PointerHeap<PathfindingNode> openNodes;
#ifdef TRANSGAMING_PSP
    m_setTag("AI");
#endif
	PathfindingNode* nodes = new PathfindingNode[ mWaypoints.Size() ];
	PathfindingNode* current;

	// Insert the starting node
	current = nodes + from;
	current->mCost = 0;
	current->mFromIndex = -1;
	current->mHeuristic = (*heuristic)( mWaypoints[from], targetWaypointForHeur, mWaypoints[from] );
	openNodes.Insert( current );
	inOpen.Set( from );

	char endReached = 0;
	// While there are still nodes to visit
	while( openNodes.Size() )
	{
		current = openNodes.Remove();
//		SelectUnique( current-nodes );
		inOpen.Clear( current-nodes );
		inClosed.Set( current-nodes );

		// End reached?
		for( int i = 0; i < toHowMany; i++ )
		{
			if( current-nodes == to[i] )
			{
				endReached = 1;
			};
		};

		if( endReached || ( current->mCost > 1.2f*maxLenToTry ) )	// BIG WARNING: Depends on GAStarDistanceHeuristic!!!
		{
			break;
		};

		// For each neighbour of our node
		int searchPtr = 0;
		float dist;
		while( GetNextAvailableFrom( current-nodes, searchPtr, dist ) != 0 )
		{
			if( mDisabledWaypoints.Test( searchPtr-1 ) || ( bigMonsters && (!mBigMonsters.Test(searchPtr-1))) )
			{
				continue;	// This one can't be accessed by a big monster or it may be disabled
			};

			if( ( inClosed.Test( searchPtr-1 ) ||
				inOpen.Test( searchPtr-1 ) ) )
			{
				if( nodes[searchPtr-1].mCost > current->mCost + DistanceBetween(current-nodes, searchPtr-1) )
				{
					nodes[searchPtr-1].mFromIndex = current - nodes;
					nodes[searchPtr-1].mCost = current->mCost + DistanceBetween(current-nodes, searchPtr-1);
					inClosed.Clear( searchPtr-1 );
					if( inOpen.Test( searchPtr - 1 ) )
					{
						// Adjust open node in heap
						openNodes.Remove( nodes + (searchPtr - 1) );
						openNodes.Insert( nodes + (searchPtr - 1) );
					}
					else	// Add node to heap
					{
						openNodes.Insert( nodes + (searchPtr - 1) );
						inOpen.Set( searchPtr-1 );
					};
				};		// else node does not need changes
			}
			else	// new node; not in open or close
			{
				nodes[searchPtr-1].mFromIndex = current - nodes;
				nodes[searchPtr-1].mCost = current->mCost + DistanceBetween(current-nodes, searchPtr-1);
				nodes[searchPtr-1].mHeuristic = (*heuristic)( mWaypoints[from], targetWaypointForHeur, mWaypoints[searchPtr-1] );
				openNodes.Insert( nodes + (searchPtr - 1) );
				inOpen.Set( searchPtr-1 );
			};
		};
	};

	if( endReached )	// create path list(backward, like stack) and return it
	{
		float cost = current->mCost;
		while( current->mFromIndex != -1 )
		{
			path.AddAtEnd( &(mWaypoints[current-nodes]) );
			current = nodes + current->mFromIndex;
		};
		path.AddAtEnd( &(mWaypoints[current-nodes]) );
		delete[] nodes;
		return cost;
	}
	else
	{
		delete[] nodes;
		return FLT_MAX;
	};
}

// if you want to use other function here, change behaviour of maxLenToTry in GetShortestPath
float ENGINE_API GAStarDistanceHeuristic( Waypoint& from, Waypoint& to, Waypoint& current )
{
	from;
	return to.DistanceFrom( current )*1.1f;
}
/*
float GAStarDistanceHeuristicBow( Waypoint& from, Waypoint& to, Waypoint& current )
{
	Vector vFrom = from.mPosition;
	Vector vTo = to.mPosition;
	Vector vCurrent = current.mPosition;

	// Calculate modifier
	Vector dir = vTo-vFrom;
	float len = dir.Magnitude();
	dir.Normalize();

	Vector toCur = vCurrent-vFrom;
	float percent = dir.Dot(toCur);
	Vector thruDir = dir*percent;
	percent /= len;
	Vector acrossDir = toCur - thruDir;
	float acrossPercent = acrossDir.Magnitude()/len;

	if( percent > 0.5f ) percent = 1.0f - percent;
	percent = sqrt(percent);

	acrossPercent *= 2;

//	float modifier = ( percent > acrossPercent )?(acrossPercent/percent):(percent/acrossPercent);
	float modifier = ( fabs( percent - acrossPercent )*len < 4.0f )?0.0f:10000.0f;
//	modifier = 1.0f-fabs( modifier );

	GLog.Print("modifier: %f\n", modifier );

	return to.DistanceFrom( current )*1.1f+modifier;
}
*/

// Test method; no longer needed, but left for future use
void WaypointSet::Test_InitWithWaypointGrid( int howMany )
{
	// Prepare structures
	ClearDistances();
	mWaypoints.ClearArray();

	// Make waypoint set - rectangular grid of points
	Waypoint wp;
	wp.mPosition.Y = (float)1;
	wp.mPosition.X = (float)0;
	wp.mPosition.Z = (float)0;

	AddWaypoint( wp );

	int j = 1;
	int i = 0;
	
	wp.mPosition.Y = (float)1;
	for( i = 0; i < howMany; i++ )
	{
		wp.mPosition.X = (float)i;
		for( ; j < howMany; j++ )
		{
			wp.mPosition.Z = (float)j;
			AddWaypoint( wp );
		};
		j = 0;
	};

	SelectedSetClearAll( 1 );
	ConnectDisconnectSelected( 0.5f, 1.5f, 1, 1 );
	SelectedSetClearAll( 0 );
	PackDistances();
}
/*
static void AddBoxOfTriangles( Vector*& v, const BoundingBox& box )
{
    const Vector P1=Vector(box.Min.X,box.Min.Y,box.Min.Z);
    const Vector P2=Vector(box.Max.X,box.Min.Y,box.Min.Z);                       
    const Vector P3=Vector(box.Min.X,box.Max.Y,box.Min.Z);                       
    const Vector P4=Vector(box.Max.X,box.Max.Y,box.Min.Z);                       
    const Vector P5=Vector(box.Min.X,box.Min.Y,box.Max.Z);                       
    const Vector P6=Vector(box.Max.X,box.Min.Y,box.Max.Z);                       
    const Vector P7=Vector(box.Min.X,box.Max.Y,box.Max.Z);                       
    const Vector P8=Vector(box.Max.X,box.Max.Y,box.Max.Z);

	v[0] = P1; v[1] = P3; v[2] = P2;
	v[3] = P2; v[4] = P3; v[5] = P4;
	v[6] = P2; v[7] = P4; v[8] = P6;
	v[9] = P6; v[10] = P4; v[11] = P8;
	v[12] = P6; v[13] = P8; v[14] = P5;
	v[15] = P5; v[16] = P8; v[17] = P7;
	v[18] = P5; v[19] = P7; v[20] = P1;
	v[21] = P1; v[22] = P7; v[23] = P3;
	v[24] = P5; v[25] = P1; v[26] = P6;
	v[27] = P6; v[28] = P1; v[29] = P2;
	v[30] = P3; v[31] = P7; v[32] = P4;
	v[33] = P4; v[34] = P7; v[35] = P8;
}
*/
/*
void WaypointSet::Render( RenderDevice* dev, LineBuffer& VBuf )
{
    Assert(dev);

    RenderState state;
    state.Flags=RenderState::ESetObjectTransform
               |RenderState::ESetConstColor;
    state.ObjectTransform=GIdentityMatrix;
    state.ConstColor=0xff;

    dev->SetState(state);
    dev->SetColorMode();
    dev->SetStream(VBuf.GetStream());

    // render unselected waypoints for all monsters
    for(int index = 0; index < mWaypoints.Size(); index++ )
	{
		if( mSelected.Test( index ) || mBigMonsters.Test( index ) )
		{
			continue;
		};

		Waypoint& wp = mWaypoints[index];
		BoundingBox b;
		wp.MakeBox( b );

		Vector* v = VBuf.Lock(3*6*2);
		AddBoxOfTriangles(v,b);

		dev->Draw(RenderDevice::EPrimTriList, VBuf.Unlock(), 6*2 );
	};

	// render selected waypoints for all monsters
    dev->SetConstColor(0xff00);

    for(int index = 0; index < mWaypoints.Size(); index++ )
	{
		if( !mSelected.Test( index ) || mBigMonsters.Test( index ) )
		{
			continue;
		};

		Waypoint& wp = mWaypoints[index];
		BoundingBox b;
		wp.MakeBox( b );

		Vector* v = VBuf.Lock(3*6*2);
		AddBoxOfTriangles(v,b);

		dev->Draw(RenderDevice::EPrimTriList, VBuf.Unlock(), 6*2 );
	};

    // render unselected waypoints for big monsters
    dev->SetConstColor(0xa00000);

    for(int index = 0; index < mWaypoints.Size(); index++ )
	{
		if( mSelected.Test( index ) || (!mBigMonsters.Test( index )) )
		{
			continue;
		};

		Waypoint& wp = mWaypoints[index];
		BoundingBox b;
		wp.MakeBox( b );

		Vector* v = VBuf.Lock(3*6*2);
		AddBoxOfTriangles(v,b);

		dev->Draw(RenderDevice::EPrimTriList, VBuf.Unlock(), 6*2 );
	};

	// render selected waypoints for big monsters
    dev->SetConstColor(0xff0000);

    for(int index = 0; index < mWaypoints.Size(); index++ )
	{
		if( !mSelected.Test( index ) || (!mBigMonsters.Test( index )) )
		{
			continue;
		};

		Waypoint& wp = mWaypoints[index];
		BoundingBox b;
		wp.MakeBox( b );

		Vector* v = VBuf.Lock(3*6*2);
		AddBoxOfTriangles(v,b);

		dev->Draw(RenderDevice::EPrimTriList, VBuf.Unlock(), 6*2 );
	};

	int size = mWaypoints.Size();

	// render all connections
    dev->SetConstColor(0xff0000);

	if( mDistances )
	{
		if( !mPackedBase )		// Operating on unpacked connections
		{
			int howManyLinePoints = 0;
			for( int i = 0; i < size*size; i++ )
			{
				if( mDistances[i] > 0 )
				{
					howManyLinePoints += 2;
				};
			};

			int ind = 0;
			i = 0;
			int j = 0;
			while( howManyLinePoints )
			{
				int currentLinePoints = (howManyLinePoints > 1024)?1024:howManyLinePoints;
				howManyLinePoints -= 1024;
				if( howManyLinePoints < 0 )
				{
					howManyLinePoints = 0;
				};

				Vector* v = VBuf.Lock( currentLinePoints );
				currentLinePoints = 0;

				for( ;i < size; i++ )
				{
					for( ; j < size; j++, ind++ )
					{
						if( mDistances[ind] > 0 )
						{
							v[currentLinePoints++] = mWaypoints[i].mPosition;
							v[currentLinePoints++] = mWaypoints[j].mPosition;
							if( currentLinePoints >= 1024 )
							{
								j++;
								ind++;
								break;
							};
						};
					};
					if( currentLinePoints >= 1024 )
					{
						break;
					};
					j = 0;
				};

				dev->Draw(RenderDevice::EPrimLineList, VBuf.Unlock(), currentLinePoints/2 );
			};
		}
		else	// Operating on packed connections
		{
			int i = 0;
			int j = 0;

			int howManyLeft = mHowManyDistances*2;
			while( howManyLeft )
			{
				int toDrawThisTime = (howManyLeft>1024)?1024:howManyLeft;
				howManyLeft -= toDrawThisTime;

				Vector* v = VBuf.Lock( toDrawThisTime );
				int drawThisTime = 0;

				while( drawThisTime < toDrawThisTime )
				{
					while( j >= mPackedCount[i] )
					{
						j = 0;
						i++;
					};
					int index = mPackedBase[i] + j;
					j++;
					v[drawThisTime++] = mWaypoints[i].mPosition;
					v[drawThisTime++] = mWaypoints[mPackedIndices[index]].mPosition;
				};

				dev->Draw(RenderDevice::EPrimLineList, VBuf.Unlock(), toDrawThisTime/2 );
			};
		};
	};
}
*/
// This returns -1 if no waypoints are in the tolerance distance from mouse pointer.
// Otherwise it returns waypoint closest to mouse pointer.

int WaypointSet::PickWaypoint( Viewport& viewport, int xm, int ym, float toleranceDistance )
{
	POINT pt;
	pt.x = xm; pt.y = ym;

	float x=float(pt.x);
	float y=float(pt.y);
	int i;
	
	// Compute the vector of the pick ray in screen space
	Vector v;
	v.X =  ( ( ( 2.0f * x ) / viewport.Width  ) - 1 ) / viewport.CameraToScreen.A00;
	v.Y = -( ( ( 2.0f * y ) / viewport.Height ) - 1 ) / viewport.CameraToScreen.A11;
	v.Z =  1.0f;

	// Transform the screen space pick ray into 3D space
	RaySegment ray;
	Matrix CameraToWorld = viewport.WorldToCamera.Inverse();
	ray.Dir    = CameraToWorld.TransformNormal(v);
	ray.Origin = CameraToWorld.GetTranslation();

	float dist = toleranceDistance;
	int waypointIndex = -1;
	float newDist;

	for( i = 0; i < mWaypoints.Size(); i++ )
	{
		newDist = ray.DistanceFromPoint( mWaypoints[i].mPosition );
//			/ (ray.Origin-mWaypoints[i].mPosition).Magnitude();
		if( newDist < dist )
		{
			dist = newDist;
			waypointIndex = i;
		};
	};

	return waypointIndex;
}

void WaypointSet::MoveWaypoint( int waypointIndex, Vector newPosition, int recalculateDistances )
{
	mWaypoints[waypointIndex].mPosition = newPosition;

	if( recalculateDistances )
	{
		RecalculateDistances( waypointIndex );
	};
}

void WaypointSet::RecalculateDistances( int waypointIndex )
{
	int i, j;
	if( ! mDistances )
	{
		return;
	};

	if( !mPackedBase )
	{
		int size = mWaypoints.Size();
		for( i = 0; i < size; i++ )
		{
			float curDist = mWaypoints[i].DistanceFrom( mWaypoints[waypointIndex] );
			if( mDistances[i*size+waypointIndex] != -1 )
			{
				mDistances[i*size+waypointIndex] = curDist;
			};

			if( mDistances[waypointIndex*size+i] != -1 )
			{
				mDistances[waypointIndex*size+i] = curDist;
			};
		};
	}
	else
	{
		int size = mWaypoints.Size();
		for( i = 0; i < size; i++ )
		{
			int base = mPackedBase[i];
			if( i == waypointIndex )
			{
				for( j = 0; j < mPackedCount[waypointIndex]; j++ )
				{
					mDistances[base+j] = mWaypoints[waypointIndex].DistanceFrom( mWaypoints[mPackedIndices[base+j]] );
				};
				continue;
			};

			for( j = 0; j < mPackedCount[i]; j++ )
			{
				if( mPackedIndices[base+j] == waypointIndex )
				{
					mDistances[base+j] = mWaypoints[i].DistanceFrom( mWaypoints[waypointIndex] );
				};
			};
		};
	};
}

void WaypointSet::GetWaypointIndicesInBox( BoundingBox box, DynamicArray<int>& list )
{
	int size = mWaypoints.Size();
	for( int i = 0; i < size; i++ )
	{
		if( box.Intersect( mWaypoints[i].mPosition ) )
		{
			list.Add( i );
		};
	};
}

void WaypointSet::ClearConnections( void )
{
	if( mPortalConnections )
	{
		delete[] mPortalConnections;
		mPortalConnections = 0;
		mPortalConnectionsForBigMonsters.Release();
	};
	mPortalNodes.ClearArray();
}

void WaypointSet::GeneratePortalConnections( DynamicArray<PathfinderPortalNode*>& portals, DynamicArray<int>& zoneIndices )
{
	ClearConnections();
	mPortalNodes = portals;
	int size = mPortalNodes.Size();
#ifdef TRANSGAMING_PSP
    m_setTag("AI");
#endif
	mPortalConnections = new float[size*size];
	mPortalConnectionsForBigMonsters.Resize(size*size);

	for( int i = 0; i < size; i++ )
	{
		mPortalConnections[i*size+i] = FLT_MAX;	// cost from portal to same portal
		for( int j = 0; j < size; j++ )
		{
			if( i == j )
			{
				continue;
			};
			if( ( mPortalNodes[i]->NumberOfWaypoints() == 0 ) ||
				( mPortalNodes[j]->NumberOfWaypoints() == 0 ) )
			{
				mPortalConnections[i*size+j] = FLT_MAX;
				continue;
			};

			int* from = mPortalNodes[i]->GetWaypointsForZone( zoneIndices[i] );
			int* to = mPortalNodes[j]->GetWaypointsForZone( zoneIndices[j] );
			WaypointPath path;
			float t = GetShortestPath( *from, to, portals[j]->NumberOfWaypoints(),
				GAStarDistanceHeuristic , path, 0 );
			mPortalConnections[i*size+j] = t;
			t = GetShortestPath( *from, to, portals[j]->NumberOfWaypoints(),
				GAStarDistanceHeuristic , path, 1 );
			if( t != FLT_MAX )
			{
				mPortalConnectionsForBigMonsters.Set(i*size+j);
			}
			else
			{
				mPortalConnectionsForBigMonsters.Clear(i*size+j);
			};
		};
	};
}

int WaypointSet::GetIndexOfWaypointClosestTo( const Vector& point, float distanceTolerance )
{
	int size = mWaypoints.Size();
	float distance = FLT_MAX;
	int wpIndex = -1;
	for( int i = 0; i < size; i++ )
	{
		float t = (mWaypoints[i].mPosition - point).Magnitude();
		if( t < distance )
		{
			distance = t;
			wpIndex = i;
		};
	};

	if( distance < distanceTolerance )
	{
		return wpIndex;
	}
	else
	{
		return -1;
	};
}

// This method does not return portals with no waypoints in them.

void WaypointSet::GetPortalNodesOrderedByDistanceFromPoint( const Vector& point, DynamicArray<PathfinderPortalNode*>& nodes, int firstToZone )
{
	DynamicArray<PathfinderPortalNode*> in;
	in = mPortalNodes;

	for( int i = 0; i < in.Size(); i++ )
	{
		if( !in[i]->NumberOfWaypoints() )
		{
			in.FastRemove(i);
			i--;
		};
	};

	if( firstToZone == -1 )
	{
		while( in.Size() )
		{
			float distance = FLT_MAX;
			PathfinderPortalNode* portal;
			int index = 0;
			for( int i = 0; i < in.Size(); i++ )
			{
				float t = ( in[i]->GetPortal()->Box.GetCenter() - point ).Magnitude();
				if( t < distance )
				{
					distance = t;
					portal = in[i];
					index = i;
				};
			};
			in.FastRemove( index );
			nodes.Add( portal );
		};
	}
	else
	{
		DynamicArray<PathfinderPortalNode*> restOfNodes;

		while( in.Size() )
		{
			float distance = FLT_MAX;
			PathfinderPortalNode* portal = 0;
			int index = 0;
			for( int i = 0; i < in.Size(); i++ )
			{
				float t = ( in[i]->GetPortal()->Box.GetCenter() - point ).Magnitude();
				if( t < distance )
				{
					distance = t;
					portal = in[i];
					index = i;
				};
			};
			in.FastRemove( index );

			bool toZone = false;
			int s = portal->GetPortal()->ZoneRefs.Size();
			for( int i = 0; i < s; ++i )
				if( portal->GetPortal()->ZoneRefs[i] == firstToZone )
				{
					toZone = true;
					break;
				}

			if( toZone )
				nodes.Add( portal );
			else
				restOfNodes.Add( portal );
		};

		for( int i = 0; i < restOfNodes.Size(); ++i )
			nodes.Add( restOfNodes[i] );
	}
}

PathfinderPortalNode* WaypointSet::GetConnectionFromPortal( PathfinderPortalNode* node1, int number, float& distance, int bigMonster )
{
	int startIndex = -1;
	int size = mPortalNodes.Size();
	for( int i = 0; i < size; i++ )
	{
		if( node1 == mPortalNodes[i] )
		{
			startIndex = i;
			break;
		};
	};

	Assert( startIndex != -1 );

	if( bigMonster && (!mPortalConnectionsForBigMonsters.Test(startIndex*size+number)) )
	{
		distance = FLT_MAX;
	}
	else
	{
		distance = mPortalConnections[startIndex*size+number];
	};

	return mPortalNodes[number];
}

void WaypointSet::ClearFloors( void )
{
	if( mFloors )
	{
		delete mFloors;
		mFloors = 0;
	};
}

void WaypointSet::PackDistances( void )
{
	if( !mDistances || mPackedBase || !mWaypoints.Size() )
	{
		return;
	};

	int i;

	int size = mWaypoints.Size();

#ifdef TRANSGAMING_PSP
    m_setTag("AI");
#endif
	mPackedCount = new int[size];
#ifdef TRANSGAMING_PSP
    m_setTag("AI");
#endif
	mPackedBase = new int[size];

	// Count valid connections
	int validCon = 0;
	for( i = 0; i < size*size; ++i )
	{
		if( mDistances[i] >= 0 )
		{
			validCon++;
		};
	};

	mHowManyDistances = validCon;
#ifdef TRANSGAMING_PSP
    m_setTag("AI");
#endif
	float* tempDistances = new float[validCon];
	int tempPtr = 0;
#ifdef TRANSGAMING_PSP
    m_setTag("AI");
#endif
	mPackedIndices = new int[validCon];

	for( i = 0; i < size; ++i )
	{
		mPackedBase[i] = tempPtr;

		int validCon2 = 0;
		for( int j = 0; j < size; ++j )
		{
			if( mDistances[i*size+j] >= 0 )
			{
				validCon2++;
				tempDistances[tempPtr] = mDistances[i*size+j];
				mPackedIndices[tempPtr++] = j;
			};
		};
		mPackedCount[i] = validCon2;
	};

	delete[] mDistances;
	mDistances = tempDistances;
}

void WaypointSet::UnpackDistances( void )
{
	if( !mDistances || !mPackedBase || !mWaypoints.Size() )
	{
		return;
	};

	int size = mWaypoints.Size();
#ifdef TRANSGAMING_PSP
    m_setTag("AI");
#endif
	float* tempDistances = new float[size*size];

	for( int i = 0; i < size; ++i )
	{
		int index = mPackedBase[i];
		int count = mPackedCount[i];

		for( int j = 0; j < size; ++j )
		{
			if( count && ( mPackedIndices[index] == j ) )
			{
				count--;
				tempDistances[i*size+j] = mDistances[index];
				index++;
			}
			else
			{
				tempDistances[i*size+j] = -1.f;
			};
		};
	};

	mHowManyDistances = 0;
	delete[] mPackedBase; mPackedBase = 0;
	delete[] mPackedCount; mPackedCount = 0;
	delete[] mPackedIndices; mPackedIndices = 0;
	delete[] mDistances; mDistances = tempDistances;
}

int WaypointSet::Selection( Viewport& viewport, float toleranceDistance, RECT* rect, int select )
{
	if( rect == NULL )
	{
		SelectedSetClearAll( select );
		return select?0:-1;
	}
	else if( ( rect->left == rect->right ) && ( rect->top == rect->bottom ) )
	{
		int wp = PickWaypoint( viewport, rect->left, rect->top, toleranceDistance );
		if( select )
		{
			SelectUnique( wp );
			return wp;
		}
		else
		{
			UnselectUnique( wp );
			return -1;
		};
	}
	else
	{
		Matrix WTC = viewport.WorldToCamera;

		int sel = -1;

		for( int i = 0; i < mWaypoints.Size(); i++ )
		{
			Vector newVec = viewport.WorldToScreen.TransformCoord( mWaypoints[i].mPosition );
			if( newVec.Z <= 0 )
			{
				continue;
			};
			newVec = viewport.WorldToScreen.Transform( mWaypoints[i].mPosition );

			newVec.X = ( newVec.X + 1 ) * viewport.Width / 2;
			newVec.Y = ( -newVec.Y + 1 ) * viewport.Height / 2;


			int x = (int)(newVec.X);
			int y = (int)(newVec.Y);

			if( ( x >= rect->left ) && ( x < rect->right ) &&
				( y >= rect->top ) && ( y < rect->bottom ) )
			{
				if( select )
				{
					SelectUnique( i );
					sel = i;
				}
				else
				{
					UnselectUnique( i );
				};
			};
		};

		return sel;
	};
}

void WaypointSet::SelectUnique( int waypointIndex )
{
	if( waypointIndex < 0 )
	{
		return;
	};

	mSelected.Set(waypointIndex);
}

void WaypointSet::UnselectUnique( int waypointIndex )
{
	if( waypointIndex < 0 )
	{
		return;
	};

	mSelected.Clear(waypointIndex);
}

void WaypointSet::DisplaceSelected( const Vector& displacement )
{
	for( int i = 0; i < mWaypoints.Size(); i++ )
	{
		if( mSelected.Test(i) )
		{
			mWaypoints[i].mPosition += displacement;
		};
	};
}

void WaypointSet::RemoveSelected( void )
{
	int sel;
	do
	{
		sel = 0;
		for( int i = mWaypoints.Size()-1; i >= 0; i-- )
			if( mSelected.Test(i) )
			{
				RemoveWaypoint(i);
				sel = 1;
				break;
			};
	} while( sel );
}

void WaypointSet::MoveSelectedToTheZoneTheyreIn( void )
{
	int sel;
	int i;
	int zoneIndex;
	do
	{
		sel = 0;
		for( i = mWaypoints.Size()-1; i >= 0; i-- )
			if( mSelected.Test(i) )
			{
#ifdef	TRANSGAMING
				zoneIndex = GEngine->pcfWorld->FindZone( mWaypoints[i].mPosition );
				if( zoneIndex != -1 )
				{
					WaypointSet& wpset = GEngine->pcfWorld->Paths.GetWaypointsInZone( zoneIndex );
					if( &wpset != this )
					{
						// create new waypoint in the zone in coords
						wpset.AddWaypoint( mWaypoints[i] );

						// remove it from here
						RemoveWaypoint(i);
						sel = 1;
						break;
					}
				}
#else
				zoneIndex = GEngine->World->FindZone( mWaypoints[i].mPosition );
				if( zoneIndex != -1 )
				{
					WaypointSet& wpset = GEngine->World->Paths.GetWaypointsInZone( zoneIndex );
					if( &wpset != this )
					{
						// create new waypoint in the zone in coords
						wpset.AddWaypoint( mWaypoints[i] );

						// remove it from here
						RemoveWaypoint(i);
						sel = 1;
						break;
					}
				}
#endif
			}
	}
	while( sel );
}

void WaypointSet::ScaleSelected( const Vector& center, float scale )
{
	for( int i = 0; i < mWaypoints.Size(); i++ )
	{
		if( mSelected.Test( i ) )
		{
			Vector diff = mWaypoints[i].mPosition - center;
			diff *= scale;
			mWaypoints[i].mPosition = center + diff;
		};
	};
}

void WaypointSet::RecalculateSelected( void )
{
	for( int i = 0; i < mWaypoints.Size(); i++ )
	{
		if( mSelected.Test(i) )
		{
			RecalculateDistances( i );
		};
	};
}

void WaypointSet::ConnectDisconnectSelected( float minDistance, float maxDistance, int connect, int disconnect )
{
	int size = mWaypoints.Size();

	if( !size )
	{
		return;
	};

	if( !mDistances )
	{
		mDistances = 0;
		mPackedIndices = 0;

#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		mPackedBase = new int[size];
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		mPackedCount = new int[size];

		for( int i = 0; i < size; i++ )
		{
			mPackedBase[i] = 0;
			mPackedCount[i] = 0;
		};
	};

	if( !mPackedBase )
	{
		for( int i = 0; i < mWaypoints.Size(); i++ )
		{
			if( mSelected.Test(i) )
			{
				for( int j = 0; j < mWaypoints.Size(); j++ )
				{
					if( mSelected.Test(j) )
					{
						float curDist = mWaypoints[i].DistanceFrom( mWaypoints[j] );

						if( !connect && ( mDistances[i*size+j] == -1 ) )
						{
							continue;
						};

						if( !disconnect && ( mDistances[i*size+j] != -1 ) )
						{
							continue;
						};

						if( ( curDist >= minDistance ) && ( curDist <= maxDistance ) )
						{
							mDistances[i*size+j] = curDist;
						}
						else
						{
							mDistances[i*size+j] = -1;
						};
					};
				};
			};
		};
	}
	else
	{
		int size = mWaypoints.Size();
		DynamicArray<float> newDist;
		DynamicArray<int> newInd;
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		int* newBase = new int[ size ];
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		int* newCount = new int[ size ];

		int newDistPtr = 0;

		for( int i = 0; i < size; i++ )
		{
			if( !mSelected.Test(i) )
			{
				int base = mPackedBase[i];
				newBase[i] = newDistPtr;
				for( int j = 0; j < mPackedCount[i]; j++ )
				{
					newDist.Add( mDistances[base+j] );
					newInd.Add( mPackedIndices[base+j] );
					newDistPtr++;
				};
				newCount[i] = newDistPtr - newBase[i];
			}
			else
			{
				newBase[i] = newDistPtr;
				for( int j = 0; j < size; j++ )
				{
					float curDist = mWaypoints[i].DistanceFrom( mWaypoints[j] );

					int connected = 0;
					for( int l = 0; l < mPackedCount[j]; l++ )
					{
						if( mPackedIndices[mPackedBase[j]+l] == i )
						{
							connected = 1;
							break;
						};
					};

					if( !mSelected.Test(j) )
					{
						if( connected )
						{
							newDist.Add( curDist );
							newInd.Add( j );
							newDistPtr++;
						};
						continue;
					};

					if( ( !connect && !connected ) || ( !disconnect && connected ) )
					{
						continue;
					};

					if( ( curDist < minDistance ) || ( curDist > maxDistance ) )
					{
						continue;
					};

					newDist.Add( curDist );
					newInd.Add( j );
					newDistPtr++;
				};
				newCount[i] = newDistPtr - newBase[i];
			};
		};

		if( mDistances )
		{
			delete[] mDistances;
		};
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		mDistances = new float[newDist.Size()];
		for( int i = 0; i < newDist.Size(); i++ )
		{
			mDistances[i] = newDist[i];
		};
		mHowManyDistances = newDist.Size();

		if( mPackedIndices )
		{
			delete[] mPackedIndices;
		};
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		mPackedIndices = new int[newInd.Size()];
		for( int i = 0; i < newInd.Size(); i++ )
		{
			mPackedIndices[i] = newInd[i];
		};
		delete[] mPackedBase;
		mPackedBase = newBase;
		delete[] mPackedCount;
		mPackedCount = newCount;

	};
}

void WaypointSet::ConnectSelected( void )
{
	ConnectDisconnectSelected( -0.1f, FLT_MAX, 1, 0);
}

void WaypointSet::DisconnectSelected( void )
{
	ConnectDisconnectSelected( 1.0f, 0.0f, 0, 1);
}

void WaypointSet::LevelWaypointsWithFloors( float upDisplacement )
{
	if( !mFloors )
	{
		return;
	};

	// for each waypoint
	for( int i = 0; i < mWaypoints.Size(); i++ )
	{
		if( mSelected.Test(i) )
		{
			Vector pos = mWaypoints[i].mPosition;

			// for each floor
			int floorIndex = 0;
			PathfinderFloor* floor;
			while( ( floor = mFloors->GetNextSelectedFloor( floorIndex ) ) != 0 )
			{
				// if waypoint on floor
				if( floor->InsideFloor( pos ) )
				{
					// calculate desired height
					floor->SetPointHeightOnFloor( pos );
					// level waypoint
					mWaypoints[i].mPosition.Y = pos.Y + upDisplacement;
					// recalculate distances
					RecalculateDistances( i );
					break;
				};
			};
		};
	};
}

void WaypointSet::AddGridOnSelectedFloors( float step )
{
	if( !mFloors )
	{
		return;
	};

	DynamicArray<Vector> newPoints;
	mFloors->MakeGridOnSelected( step, newPoints );

	Waypoint wp;
	for( int i = 0; i < newPoints.Size(); i++ )
	{
		wp.mPosition = newPoints[i];
		SelectUnique( AddWaypoint( wp ) );
	};
}

#ifdef TRANSGAMING_USE_EDITOR
void WaypointSet::SelectUnselectWaypointsOnSelectedFloors( int select )
{
	if( !mFloors )
	{
		return;
	};

	int floor = 0;
	PathfinderFloor* fl;
	while( ( fl = mFloors->GetNextSelectedFloor( floor ) ) != 0 )
	{
		for( int j = 0; j < mWaypoints.Size(); j++ )
		{
			Vector v = mWaypoints[j].mPosition;
			if( fl->InsideFloor( v ) )
			{
				if( select )
				{
					SelectUnique(j);
				}
				else
				{
					UnselectUnique(j);
				};
			};
		};
	};
}
#endif

// Used at load time; do not touch
void WaypointSet::InLoadTransformNodeIndicesIntoPointers( Pathfinder* pf )
{
	for( int i = 0; i < mPortalNodes.Size(); i++ )
	{
		mPortalNodes[i] = &pf->GetNodeForPortal( (int)mPortalNodes[i] );
	};
}

void WaypointSet::CopySelected( const Vector displacement )
{
	DynamicArray<int> indices;
	for( int i = 0; i < mWaypoints.Size(); i++ )
	{
		if( mSelected.Test(i) )
		{
			Waypoint wp;
			wp.mPosition = mWaypoints[i].mPosition;
			wp.mPosition += displacement;
			indices.Add( AddWaypoint( wp ) );
		};
	};

	mSelected.ClearAll();

	for( int i = 0; i < indices.Size(); i++ )
	{
		SelectUnique(indices[i]);
	};
}

#ifdef TRANSGAMING_USE_EDITOR
void WaypointSet::RemoveConnectionsCollidingWithGeometryInSelected( World* world, Zone& zone )
{
	int size = mWaypoints.Size();

	for( int i = 0; i < size; i++ )
	{
		if( !mSelected.Test( i ) )
		{
			continue;
		};

		for( int j = 0; j < size; j++ )
		{
			if( ( i == j ) || ( !mSelected.Test( j ) ) )
			{
				continue;
			};

			if( !Connected( i, j ) )
			{
				continue;
			};

			if( ConnectionCollidingWithGeometry( mWaypoints[i].mPosition, mWaypoints[j].mPosition, world, zone ) )
			{
				RemoveConnection( i, j );
			};
		};
	};
}
#endif

int WaypointSet::Connected( int i, int j )
{
	if( !mDistances )
	{
		return 0;
	}
	else
	if( !mPackedBase )
	{
		return (mDistances[i*mWaypoints.Size()+j] != -1 );
	}
	else
	{
		for( int a = 0; a < mPackedCount[i]; a++ )
		{
			if( mPackedIndices[mPackedBase[i]+a] == j )
			{
				return 1;
			};
		};
		return 0;
	};
}

void WaypointSet::RemoveConnection( int i, int j )
{
	int size = mWaypoints.Size();

	if( !size || !mDistances )
	{
		return;
	};

	if( !mPackedBase )	// operating on unpacked data
	{
		mDistances[i*size+j] = -1;
	}
	else
	{
		for( int a = mPackedBase[i]; a < mPackedBase[i]+mPackedCount[i]; a++ )
		{
			if( mPackedIndices[a] == j )
			{
				mHowManyDistances--;
//	Commented, may contain a bug
//				::memmove( mDistances+a, mDistances+a+1, (mHowManyDistances-a)*sizeof(float) );
//				::memmove( mPackedIndices+a, mPackedIndices+a+1, (mHowManyDistances-a)*sizeof(int) );
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
				float* newDist = new float[mHowManyDistances];
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
				int* newInd = new int[mHowManyDistances];

				for( int k = 0; k < a; k++ )
				{
					newDist[k] = mDistances[k];
					newInd[k] = mPackedIndices[k];
				};

				for( int k = a; k < mHowManyDistances; k++ )
				{
					newDist[k] = mDistances[k+1];
					newInd[k] = mPackedIndices[k+1];
				};

				delete[] mDistances;		mDistances = newDist;
				delete[] mPackedIndices;	mPackedIndices = newInd;

				mPackedCount[i] = mPackedCount[i] - 1;
				for( int k = i+1; k < size; k++ )
				{
					mPackedBase[k] = mPackedBase[k] - 1;
				};
				return;
			};
		};
	};
}

#pragma optimize( "g", off )

void WaypointSet::AddConnection( int i, int j )
{
	int a,k;
	int size = mWaypoints.Size();

	if( !size || !mDistances )
	{
		return;
	};

	if( !mPackedBase )	// operating on unpacked data
	{
		mDistances[i*size+j] = (mWaypoints[i].mPosition-mWaypoints[j].mPosition).Magnitude();
	}
	else
	{
		for( a = mPackedBase[i]; a < mPackedBase[i]+mPackedCount[i]; a++ )
		{
			if( mPackedIndices[a] == j )
			{
				return;
			};
		};

		mHowManyDistances++;
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		float* newDist = new float[mHowManyDistances];
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		int* newInd = new int[mHowManyDistances];

		for( k = 0; k < a; ++k )
		{
			newDist[k] = mDistances[k];
			newInd[k] = mPackedIndices[k];
		};

		for( k = mHowManyDistances-1; k > a; --k )
		{
			newDist[k] = mDistances[k-1];
			newInd[k] = mPackedIndices[k-1];
		};

		newDist[k] = (mWaypoints[i].mPosition-mWaypoints[j].mPosition).Magnitude();
		newInd[k] = j;

		delete[] mDistances;		mDistances = newDist;
		delete[] mPackedIndices;	mPackedIndices = newInd;

		mPackedCount[i] = mPackedCount[i] + 1;
		for( int l = i+1; l < size; l++ )
		{
			mPackedBase[l] = mPackedBase[l] + 1;
		};
	};
}

#pragma optimize( "", on )

#ifdef TRANSGAMING_USE_EDITOR
int WaypointSet::ConnectionCollidingWithGeometry( const Vector& source, const Vector& destination, World* world, Zone& zone )
{
	RaySegment ray;
	ray.Dir    = destination - source;
	ray.Origin = source;
	float len = ray.Dir.Magnitude();
	ray.Dir.Normalize();

	//for ( int i = 0; i < zone.MeshRefs.Size(); i++ )
	//{
	//	Mesh& mesh = world->Meshes[zone.MeshRefs[i]];

    //for(TEntityPtr* p=zone.EntityList; p; p=p->Next)
    //WorldMesh* ptr = (*p)->AsMesh();

    for(Entity* entity = zone.EntityList; entity; entity=zone.NextEntity(entity))
    {
        WorldMesh* ptr = entity->AsMesh();
        if(!ptr)
            continue;

        WorldMesh& mesh=*ptr;

		if( ray.Intersect( mesh.Box ) )
		{
			int triCount;

#ifdef TRANSGAMING_DEINDEX
            if (!mesh.isIndexed){
                if (mesh.PrimType == RenderDevice::EPrimTriList){
                    triCount = mesh.VertList.Size()/3;

			        for( int j = 0; j < triCount; ++j )
			        {
				        const Vector v0 = mesh.VertList.getVertPos(j * 3 + 0);
				        const Vector v1 = mesh.VertList.getVertPos(j * 3 + 1);
				        const Vector v2 = mesh.VertList.getVertPos(j * 3 + 2);
				        float d;
				        if( ray.IntersectTriangle(v0,v1,v2,d ) ) 
				        {
					        if( (d > 0 ) && ( d < len ) ) 
                            {
						        return 1;
					        }
				        }
			        }
                }

                else{
                    triCount = mesh.VertList.Size() - 2;

			        for( int j = 0; j < triCount; ++j )
			        {
				        const Vector v0 = mesh.VertList.getVertPos(j + 0);
				        const Vector v1 = mesh.VertList.getVertPos(j + 1);
				        const Vector v2 = mesh.VertList.getVertPos(j + 2);
				        float d;
				        if( ray.IntersectTriangle(v0,v1,v2,d ) ) 
				        {
					        if( (d > 0 ) && ( d < len ) ) 
                            {
						        return 1;
					        }
				        }
			        }
                }
            }

            else{
#endif
                triCount = mesh.Triangles.Size() / 3;

			    for( int j = 0; j < triCount; ++j )
			    {
				    const WORD e0 = mesh.Triangles[j*3+0];
				    const WORD e1 = mesh.Triangles[j*3+1];
				    const WORD e2 = mesh.Triangles[j*3+2];

				    const Vector v0 = mesh.VertList.getVertPos(e0);
				    const Vector v1 = mesh.VertList.getVertPos(e1);
				    const Vector v2 = mesh.VertList.getVertPos(e2);
				    float d;
				    if( ray.IntersectTriangle(v0,v1,v2,d ) ) 
				    {
					    if( (d > 0 ) && ( d < len ) ) 
                        {
						    return 1;
					    }
				    }
			    }
#ifdef TRANSGAMING_DEINDEX
            }
#endif
		}
	}
	return 0;
}

void WaypointSet::SelectWaypointsOutsideOfZoneOrItsPortals( World* world, int zoneNum, float portalThickness )
{
	Zone& zone = *world->Zones[zoneNum];
	SelectedSetClearAll( 0 );	// unselect all

	DynamicArray<BoundingBox> portalBoxes;

	// Make portal boxes
	for( int i = 0; i < zone.PortalRefs.Size(); i++ )
	{
		Portal& portal = *world->Portals[zone.PortalRefs[i]];
		
		BoundingBox box = portal.Box;

		box += portal.Poly.Verts[0]+world->Planes[ portal.Poly.PlaneIndex].Normal*portalThickness;
		box += portal.Poly.Verts[0]-world->Planes[ portal.Poly.PlaneIndex].Normal*portalThickness;

		portalBoxes.Add( box );
	};

	int size = mWaypoints.Size();
	for( int i = 0; i < size; i++ )
	{
		if( zone.PointTest( mWaypoints[i].mPosition ) )
		{
			continue;
		};

		int inter = 0;
		for( int j = 0; j < portalBoxes.Size(); j++ )
		{
			if( portalBoxes[j].Intersect( mWaypoints[i].mPosition ) )
			{
				inter = 1;
				break;
			};
		};

		if( !inter )
		{
			SelectUnique(i);
		};
	};
}

void WaypointSet::MergeWaypointsAtSamePosition( float positionTolerance )
{
	if( !mPackedBase )
	{
		PackDistances();
	};

	for( int i = 0; i < mWaypoints.Size(); i++ )
	{
		for( int j = i+1; j < mWaypoints.Size(); j++ )
		{
			if( (mWaypoints[i].mPosition-mWaypoints[j].mPosition).Magnitude() < positionTolerance )
			{
				for( int k = 0; k < mWaypoints.Size(); k++ )
				{
					if( k == i || k == j )
					{
						continue;
					};
					if( Connected( k,j ) && !Connected( k,i ) )
					{
						AddConnection( k, i );
					};
					if( Connected( j,k ) && !Connected( i,k ) )
					{
						AddConnection( i,k );
					};
				};
				RemoveWaypoint( j );
				j--;
			};
		};
	};
}

void WaypointSet::RemoveWaypointsNotConnectedToAnything( void )
{
	if( !mDistances )
	{
		return;
	};

	if( !mPackedBase )
	{
		PackDistances();
	};

	for( int i = 0; i < mWaypoints.Size(); i++ )
	{
		if( mPackedCount[i] == 0 )
		{
			RemoveWaypoint(i);
			i--;
		};
	};
}

void WaypointSet::SelectWaypointsNotConnectedToAnything( void )
{
	if( !mDistances )
	{
		SelectedSetClearAll( 1 );
		return;
	};

	if( !mPackedBase )
	{
		PackDistances();
	};

	for( int i = 0; i < mWaypoints.Size(); i++ )
	{
		if( mPackedCount[i] == 0 )
		{
			SelectUnique(i);
		};
	};
}
#endif

void WaypointSet::ScaleContents( float scale )
{
	// Scale waypoints
	for( int i = 0; i < mWaypoints.Size(); i++ )
	{
		mWaypoints[i].mPosition *= scale;
	};

	// Scale distances
	if( mDistances )
	{
		if( !mPackedBase )
		{
			PackDistances();
		};

		for( int i = 0; i < mHowManyDistances; i++ )
		{
			mDistances[i] *= scale;
		};
	};
}

void WaypointSet::FlipZ( void )
{
	// Flip waypoints
	for( int i = 0; i < mWaypoints.Size(); i++ )
	{
		mWaypoints[i].mPosition.Z *= -1.0;
	};
}

void WaypointSet::SelectedForSmallMonstersOnly( void )
{
	for( int i = 0; i < mWaypoints.Size(); i++ )
	{
		if( mSelected.Test(i) )
		{
			mBigMonsters.Clear( i );
		};
	};
}

void WaypointSet::SelectedForAllMonsters( void )
{
	for( int i = 0; i < mWaypoints.Size(); i++ )
	{
		if( mSelected.Test(i) )
		{
			mBigMonsters.Set( i );
		};
	};
}

void WaypointSet::CopyTo( WaypointSet& wpset )
{
	wpset.ClearContents();

	// Copy waypoints
	for( int i = 0; i < mWaypoints.Size(); ++i )
		wpset.mWaypoints.Add( mWaypoints[i] );

	if( mHowManyDistances )
	{
		// Copy distances
		wpset.mHowManyDistances = mHowManyDistances;
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		wpset.mDistances = new float[mHowManyDistances];
		::memcpy( wpset.mDistances, mDistances, sizeof(float)*mHowManyDistances );

		// Copy mPackedIndices
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		wpset.mPackedIndices = new int[mHowManyDistances];
		::memcpy( wpset.mPackedIndices, mPackedIndices, sizeof(int)*mHowManyDistances );
	}

	if( mWaypoints.Size() )
	{
		if( mPackedBase )
		{
			// Copy mPackedBase
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
			wpset.mPackedBase = new int[mWaypoints.Size()];
			::memcpy( wpset.mPackedBase, mPackedBase, sizeof(int)*mWaypoints.Size() );
		}

		if( mPackedCount )
		{
			// Copy mPackedCount
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
			wpset.mPackedCount = new int[mWaypoints.Size()];
			::memcpy( wpset.mPackedCount, mPackedCount, sizeof(int)*mWaypoints.Size() );
		}
	}

    wpset.mSelected.MakeCompleteCopy( mSelected );
    wpset.mBigMonsters.MakeCompleteCopy( mBigMonsters );
    wpset.mPortalConnectionsForBigMonsters.MakeCompleteCopy( mPortalConnectionsForBigMonsters );
    wpset.mDisabledWaypoints.MakeCompleteCopy( mDisabledWaypoints );

	// Copy mPortalNodes - warning: copies pointers (if those are really pointers at the moment)
	for( int i = 0; i < mPortalNodes.Size(); ++i )
		wpset.mPortalNodes.Add( mPortalNodes[i] );

	// Copy mPortalConnections
	int hm = mPortalNodes.Size()*mPortalNodes.Size();
	if( hm )
	{
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		wpset.mPortalConnections = new float[hm];
		::memcpy( wpset.mPortalConnections, mPortalConnections, sizeof(float)*hm);
	}

	// Copy floors - really, don't copy floors, that's not the purpose for this method, it's a fix
	Assert( mFloors == 0 );
}
