// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/07 Krystian Galaj
//----------------------------------------------------------------------------

#include "Engine.h"
#include "WaypointPath.h"
#include "WaypointSet.h"
#include "Pathfinder.h"
#include "WaypointGPath.h"

/*
void WaypointPath::Render( RenderDevice* dev, LineBuffer& VBuf )
{
	Assert(dev);

	int size = mPath.Size();
	if( size < 2 ) return;

    RenderState state;
    state.Flags=RenderState::ESetConstColor
               |RenderState::ESetObjectTransform;
	state.ConstColor = 0xff00;
	state.ObjectTransform=GIdentityMatrix;

	dev->SetState(state);
    dev->SetColorMode();
    dev->SetStream(VBuf.GetStream());

	// render all connections
	Vector* v = VBuf.Lock( size );
	for( int i = 0; i < size; i++ )
	{
		v[i] = mPath[i]->mPosition;
		v[i].Y += 0.1f;
	};
	dev->Draw(RenderDevice::EPrimLineStrip, VBuf.Unlock(), size-1 );
}
*/

int WaypointPath::PeekNextIndex( Pathfinder* pf, int zoneIndex )
{
	if( !mPath.Size() )
		return 0;

	WaypointSet& ws = pf->GetWaypointsInZone( zoneIndex );
	return ws.FastGetWaypointIndex( mPath[mPath.Size()-1] );
}

int WaypointPath::PeekIndex( Pathfinder* pf, int zoneIndex, int index )
{
	WaypointSet& ws = pf->GetWaypointsInZone( zoneIndex );
	return ws.FastGetWaypointIndex( mPath[mPath.Size()-1-index] );
}
