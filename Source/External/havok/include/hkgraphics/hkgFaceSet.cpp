/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphics/hkGraphics.h> // precompiled
#include <hkgraphics/hkgFaceSet.h>
#include <hkgraphics/hkgFaceSetPrimitive.h>
#include <hkgraphics/hkgVertexSet.h> 
#include <hkgraphics/hkgRay.h> 
#include <hkgraphics/hkgPickData.h>
#include <hkgraphics/nvstripper/nvTriStrip.h>

hkgFaceSet* (*hkgFaceSet::create)(hkgDisplayContext* state);

hkgFaceSet::hkgFaceSet( hkgDisplayContext* state) 
: m_flags(0)
{ 
	m_vertices = hkgVertexSet::create(this);
}

hkgFaceSet::~hkgFaceSet()
{
	int np = m_primitives.getSize();	
	for (int i=0; i<np; ++i)
		delete m_primitives[i]; // owns them.

	delete m_vertices;
}


void hkgFaceSet::strip(HKG_FACE_SET_STRIP_OPTIONS opts, bool stitch, int minStripLen)
{
	if (m_flags & HKG_FACE_SET_STRIPPED)
		return;

	nvSetStitchStrips(stitch);
	nvSetMinStripSize(minStripLen);

	switch (opts)
	{
	case HKG_FACE_SET_STRIP_CACHE:
		nvSetCacheSize(16);
		break;
	case HKG_FACE_SET_STRIP_CACHE24:
		nvSetCacheSize(24);
		break;
	case HKG_FACE_SET_STRIP_DEFAULT:
	default:
		nvSetCacheSize(0);
		break;	
	}

	//for each prim that is not a tri strip, run it through the tri stripper
	//remove the original prim and add the new ones.

	int np = m_primitives.getSize();

	hkArray<int> toRemove;

	int i;
	for ( i = 0; i < np; ++i)
	{
		hkgFaceSetPrimitive* prim = m_primitives[i];

		if (prim->getType() == HKG_TRI_LIST )
		{
			nvPrimitiveGroup* nvPrims;
			unsigned short nvNumPrims;

			unsigned short* conn = prim->getIndices();
			bool haveToAllocateConn = (conn == HK_NULL);
			
			if (haveToAllocateConn )
			{
				conn = hkAllocate<unsigned short>( prim->getLength(), HK_MEMORY_CLASS_DISPLAY);
				for (int j = 0; j < (int)prim->getLength(); ++j)
					conn[j] = j + prim->getVertexBufferStartIndex();
			}

			nvGenerateStrips( conn, prim->getLength(), &nvPrims, &nvNumPrims);
	
			if (nvNumPrims > 0)
			{
				// remove orginal	
				toRemove.pushBack(i);

				// add new ones to the end
				for (int j =0; j < nvNumPrims; ++j)
				{
					nvPrimitiveGroup& p = nvPrims[j];
					
					hkgFaceSetPrimitive* hp = new hkgFaceSetPrimitive(
									 p.type == NV_PT_STRIP? HKG_STRIP : 
									/*(p.type == NV_PT_FAN? HKG_FAN : HKG_TRI_LIST)*/  HKG_TRI_LIST );
					
					p.ownIndices = false; // so that we can keep them.

					hp->setIndices( p.indices, p.numIndices );

					m_primitives.pushBack( hp );
				}
			}

			if (haveToAllocateConn)
				hkDeallocate<unsigned short>( conn );

			delete [] nvPrims;
		}
	}

	// this preserves order.
	int nr = toRemove.getSize();
	for (i = 0; i < nr; ++i)
	{
		int r = toRemove[i] - i;
		delete m_primitives[r];
		m_primitives.removeAtAndCopy(r); // -i so that we adjust for the accumulated slide
	}

	initialize();

	m_flags |= HKG_FACE_SET_STRIPPED;
	
}

void hkgFaceSet::reorderVertexSet()
{
	if (m_flags & HKG_FACE_SET_REORDERED)
		return; // already done




	m_flags |= HKG_FACE_SET_REORDERED;
}

void hkgFaceSet::tesselate( unsigned int iterations )
{

}

void hkgFaceSet::initialize()
{

}

void hkgFaceSet::render(hkgDisplayContext* context)
{

}


//
// given ray from pos in direction dir
// returns (t,u,v) where t is distance to the plane and u,v are 
// the coords within the triangle
// so the point of intersection will be v0 + u*(v1-v0) + v*(v2-v0)
//
// (from:	Fast,Minimum Storage Ray-Triangle Intersection
//			Journal of Graphics Tools, 2(1):21-28, 1997 )
#define HKG_TRIANGLE_EPSILON 0.0001f

static bool _triangleRayIntersect(const float pos[3], const float dir[3],
                   const float v0[3],
				   const float v1[3],
				   const float v2[3],
				   float &t, float &u, float &v)
{

	float det,inv_det;

	/* find vectors for two edges sharing vert0 */   
	float edge1[3];
	hkgVec3Sub( edge1, v1, v0);
	float edge2[3];
	hkgVec3Sub( edge2, v2, v0);
	
	/* begin calculating determinant - also used to calculate U parameter */
	float pvec[3];
	hkgVec3Cross( pvec, dir, edge2);

	/* if determinant is near zero, ray lies in plane of triangle */
	det = hkgVec3Dot( edge1, pvec );

	if (det < HKG_TRIANGLE_EPSILON)      
		return false;

	/* calculate distance from vert0 to ray origin */   
	float tvec[3];
	hkgVec3Sub(tvec, pos, v0);

	/* calculate U parameter and test bounds */   
	u = hkgVec3Dot( tvec, pvec );
   
	if (u < 0.0f || u > det)      
	   return false;   
   
	/* prepare to test V parameter */
	float qvec[3];
	hkgVec3Cross( qvec, tvec, edge1 );    
   
	/* calculate V parameter and test bounds */
	v = hkgVec3Dot( dir, qvec );   

	if (v < 0.0f || (u + v) > det)      
	   return false;

	/* calculate t, scale parameters, ray intersects triangle */
	t = hkgVec3Dot( edge2, qvec );   

	inv_det = 1.0f / det;   
	t *= inv_det;
	u *= inv_det; 
	v *= inv_det;

	return true;
}

unsigned int hkgFaceSet::intersect( const hkgRay& ray, bool ignoreBackfaces, hkgFaceSetPickData& data) const
{
	int numPrim = m_primitives.getSize();
	unsigned int numHits = 0;
	if (numPrim > 0)
	{
		float minDist = FLT_MAX;
		unsigned int stride = m_vertices->getStrideInBytes();

		// XXX: There may be no need to lock on a lot of platforms
		// just to get read access to buffers. To be sorted out:

		unsigned char* buf = m_vertices->lock();
	
		float t,u,v;

		for (int i = 0; i < numPrim; ++i)
		{
			hkgFaceSetPrimitive* prim = m_primitives[i];
			unsigned short* conn = prim->getIndices();
			
			unsigned int index = 0;

			if (prim->getType() == HKG_TRI_LIST)
			{
				int numTri = prim->getLength() / 3;
				
				for (int it=0; it<numTri ; ++it)
				{
					int i0 = conn? conn[index] : index;
					index++;
					int i1 = conn? conn[index] : index;
					index++;
					int i2 = conn? conn[index] : index;
					index++;

					float* v0 = (float*)( buf + stride*(i0) );
					float* v1 = (float*)( buf + stride*(i1) );
					float* v2 = (float*)( buf + stride*(i2) );
					
					if (_triangleRayIntersect( ray.getFromPtr(), ray.getDirPtr(), v0, v1, v2, t, u, v ))
					{
						if ((t > 0) && (t < minDist))
						{
							numHits++;
						
							data.m_dist = t;
							data.m_primitiveIndex = i;
							data.m_verts[0] = i0;
							data.m_verts[1] = i1;
							data.m_verts[2] = i2;
							data.m_primitiveTriIndex = it;
							minDist = t;
						}
					}
				}
			}
			else 
			{
				HK_ASSERT( prim->getType() == HKG_STRIP);

				int numTri = prim->getLength() - 2;
				
				int i1 = conn? conn[0] : 0;
				int i2 = conn? conn[1] : 1;

				int i0;
				int index = 2;

				bool ccw = true;

				for (int it=0; it<numTri ; ++it)
				{
					i0 = i1;
					i1 = i2;
					i2 = conn? conn[index] : index;
				
					index++;

					if ((i0 == i1) || ( i1 == i2 ) || ( i2 == i0 ) ) // degenerate
					{
						ccw = !ccw;
						continue;
					}

					float* v0; 
					float* v1; 
					float* v2; 
					if (ccw)
					{
						v0 = (float*)( buf + stride*(i0) );
						v1 = (float*)( buf + stride*(i1) );
						v2 = (float*)( buf + stride*(i2) );
					}
					else
					{
						v0 = (float*)( buf + stride*(i0) );
						v2 = (float*)( buf + stride*(i1) );
						v1 = (float*)( buf + stride*(i2) );
					}

					if (_triangleRayIntersect( ray.getFromPtr(), ray.getDirPtr(), v0, v1, v2, t, u, v ) )
					{						
						if ( (t > 0) && (t < minDist))
						{
							numHits++;

							data.m_dist = t;
							data.m_primitiveIndex = i;

							if (ccw)
							{
								data.m_verts[0] = i0;
								data.m_verts[1] = i1;
								data.m_verts[2] = i2;
							}
							else
							{
								data.m_verts[0] = i0;
								data.m_verts[1] = i2;
								data.m_verts[2] = i1;
							}

							data.m_primitiveTriIndex = it;
							minDist = t;
						}
					}

					ccw = !ccw;
				}
			}
		}
	}
	
	return numHits;
}

void hkgFaceSet::computeAABB(float minE[3], float maxE[3]) const
{
	int numPrim = m_primitives.getSize();
	
	if (numPrim > 0)
	{
		int stride = m_vertices->getStrideInBytes();

		const unsigned char* buf = m_vertices->lock();
		
		int nv = m_vertices->getNumVerts();

		for (int j=0; j < nv; ++j)
		{
			const float* vert = (const float*)( &(buf[j*stride]) );

			for (int k =0; k < 3; ++k)
			{
				const float vk = vert[k];
				if ( maxE[k] < vk )
					maxE[k] = vk;

				if ( minE[k] > vk )
					minE[k] = vk;
			}
		}

		m_vertices->unlock();
	}
}


void hkgFaceSet::print(hkOstream& stream) const
{
	stream << "\t\t\t\t\t[ " << m_primitives.getSize() << "] Primitives.\n";
	stream << "\t\t\t\t\t[ " << m_vertices->getNumVerts() << "] Vertices.\n";
}

void hkgFaceSet::transform( const float t[16] )
{
	unsigned char* buf = m_vertices->lock();
	int stride = m_vertices->getStrideInBytes();

	int numVerts = m_vertices->getNumVerts();

	float tempPos[3];
	for (int i = 0; i < numVerts; ++i)
	{
		// transform pos
		float* pos = (float*)( &buf[i*stride] );
		hkgVec3Transform( tempPos, pos, t );
		hkgVec3Copy( pos, tempPos );

		// rotate normal (slow, but cross platform to ask for it)
		float* norm = m_vertices->getNormalInfo(i);
		hkgVec3Rotate( norm, t );
	}

	m_vertices->unlock();

}

void hkgFaceSet::removeIndexArrays()
{
	bool haveConn = false;

	int totalVerts = 0;
	int numPrim = m_primitives.getSize();

	int m;
	for ( m = 0; m < numPrim; ++m)
	{
		const hkgFaceSetPrimitive* prim = getPrimitive(m);	

		if (prim->getIndices())
			haveConn = true;
			
		totalVerts += prim->getLength();		
	}
	
	if (!haveConn) 
		return; // nothing to do.
		
	hkgVertexSet* newVerts = hkgVertexSet::create(this);
	bool needUV = (m_vertices->getVertexFormat() & HKG_VERTEXSET_UV) != 0;
	bool needColor = (m_vertices->getVertexFormat() & HKG_VERTEXSET_COLOR) != 0;
	
	newVerts->setNumVerts(totalVerts, needUV, needColor);
	newVerts->lock();
	m_vertices->lock();
	
		
	for ( m = 0; m < numPrim; ++m)
	{
		hkgFaceSetPrimitive* prim = m_primitives[m];	
		unsigned short* conn = prim->getIndices();	
		int offset = prim->getVertexBufferStartIndex();
	
		int nv = prim->getLength();
		
		int newOffset = newVerts->getPosAddCounter();
		
		for (int v = 0; v < nv; ++v)
		{
			int vindex = conn? conn[v] : v;
			vindex += offset;
			
			newVerts->addPosInfo( m_vertices->getPosInfo( vindex ), 1 );
			newVerts->addNormalInfo( m_vertices->getNormalInfo( vindex ), 1 );
			
			if (needUV)	
				newVerts->addUVInfo( m_vertices->getUVInfo( vindex ), 1);

			if (needColor)	
				newVerts->addUVInfo( m_vertices->getColorInfo( vindex ), 1);
		}
		
		prim->setIndices( HK_NULL, nv );
		prim->setVertexBufferStartIndex( newOffset );
	}	
	
	newVerts->unlock();
	m_vertices->unlock();
	
	delete m_vertices;
	m_vertices = newVerts;
}

void hkgFaceSet::collapse()
{
	int numPrims = m_primitives.getSize();

	if ( (numPrims == 1) || ((numPrims == 2) && (m_primitives[0]->getType() != m_primitives[1]->getType())) )
		return; // nothing to do.

	hkArray<hkgFaceSetPrimitive*> indexedTriList;
	hkArray<hkgFaceSetPrimitive*> indexedStrip;

	int indexArraySize =0;
	bool contigOnly = true;
	{
		for (int i=0; i < m_primitives.getSize(); ++i)
		{
			hkgFaceSetPrimitive* p = m_primitives[i];

			if (p->getIndices())
			{
				contigOnly = false;
			}
			else
			{
				// we want to be able to remap indexs. To do this we must convert all contigous into indexed versions. 
				// If we only have contigous versions, we will remap back again afterwards to all contigous again, 
				// but if we have some index versions already then we are not reasonably expected to do so.

				int startIndex = p->getVertexBufferStartIndex();
				int len = p->getLength();

				unsigned short *conn = hkAllocate<unsigned short>( len, HK_MEMORY_CLASS_DISPLAY );
				{
					for (int j=0; j < len;++j)
					{
						conn[j] = j + startIndex;
					}
				}

				p->setIndices( conn, len ); // takes ownership of conn
			}
			
			if (p->getType() == HKG_TRI_LIST)
				indexedTriList.pushBack( p );
			else
				indexedStrip.pushBack( p );

			indexArraySize += p->getLength();
		}
	}

	hkgFaceSetPrimitive* stripPrim = HK_NULL;
	hkgFaceSetPrimitive* tlistPrim = HK_NULL;

	// Make one new strip prim
	int numStrips = indexedStrip.getSize();
	if (numStrips > 0)
	{
		if (numStrips == 1)
		{
			stripPrim = indexedStrip[0];
		}
		else
		{
			stripPrim = new hkgFaceSetPrimitive( HKG_STRIP );

			// count index size
			int stripIndexSize = 0;
			{
				for (int i =0; i < numStrips; ++i)
				{
					hkgFaceSetPrimitive* p = indexedStrip[i];
					stripIndexSize += p->getLength();
				}
			}
			// stiches
			stripIndexSize += (numStrips-1) *2;

			unsigned short* conn = hkAllocate<unsigned short>( stripIndexSize, HK_MEMORY_CLASS_DISPLAY );
			stripPrim->setIndices( conn, stripIndexSize ); // takes ownership

			int index = 0;
			{
				for (int i =0; i < numStrips; ++i)
				{
					hkgFaceSetPrimitive* p = indexedStrip[i];
					unsigned short* pconn = p->getIndices();
					{
						for (int j=0; j <p->getLength(); ++j)
						{
							conn[index++] = pconn[j];
						}
					}
				
					if ( i < (numStrips-1) ) // stich needed (by means of two degenerates) with next strip
					{
						conn[index] = conn[index-1]; // repeat last;
						conn[index+1] = indexedStrip[i+1]->getIndices()[0]; // repeat next
						index += 2;
					}
				}
			}
			HK_ASSERT( index == stripIndexSize );
		}
	}

	// Make one new trilist prim
	int numTriLists = indexedTriList.getSize();
	if (numTriLists > 0)
	{
		if (numTriLists == 1)
		{
			tlistPrim = indexedTriList[0];
		}
		else
		{
			tlistPrim = new hkgFaceSetPrimitive( HKG_TRI_LIST );

			// count index size
			int tlistIndexSize = 0;
			{
				for (int i =0; i < numTriLists; ++i)
				{
					hkgFaceSetPrimitive* p = indexedTriList[i];
					tlistIndexSize += p->getLength();
				}
			}
			unsigned short* conn = hkAllocate<unsigned short>( tlistIndexSize, HK_MEMORY_CLASS_DISPLAY );
			tlistPrim->setIndices( conn, tlistIndexSize ); // takes ownership

			int index = 0;
			{
				for (int i =0; i < numTriLists; ++i)
				{
					hkgFaceSetPrimitive* p = indexedTriList[i];
					unsigned short* pconn = p->getIndices();

					{
						for (int j=0; j <p->getLength(); ++j)
						{
							conn[index++] = pconn[j];
						}
					}
				}
			}
			HK_ASSERT( index == tlistIndexSize );
		}
	}

	// We now have at most two prims, indexed.
	// Delete all other prims;

	{
		for (int i =0; i < m_primitives.getSize(); ++i)
		{
			hkgFaceSetPrimitive* p = m_primitives[i];
			if ( (p != tlistPrim) && ( p != stripPrim))
				delete p;
		}
	}
	m_primitives.setSize(0);
	m_flags  = 0;

	if (tlistPrim)
		addPrimitive(tlistPrim);
	if (stripPrim)
		addPrimitive(stripPrim);


	// 
	// as we don't want to leave the face set in a totally different state to before
	// we should set back to contigous if it was like that to start 
	// as some platforms may require it (PS2GL)
	//
	if (contigOnly)
		removeIndexArrays();
}

/*
* Havok Game Dynamics SDK - PUBLIC RELEASE, BUILD(#20030622)
*
* (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. The Telekinesys
* Logo, Havok and the Havok buzzsaw logo are trademarks of Telekinesys
* Research Limited.  Title, ownership
* rights, and intellectual property rights in the Software remain in
* Telekinesys Research Limited and/or its suppliers.
*
* Use of this software is subject to and indicates acceptance of the End User licence
* Agreement for this product. A copy of the license is included with this software and
* is also available from info@havok.com.
*
*/ 
