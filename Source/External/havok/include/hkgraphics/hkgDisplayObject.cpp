/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphics/hkGraphics.h> // precompiled
#include <hkgraphics/hkgDisplayObject.h>
#include <hkgraphics/hkgViewport.h>

hkgDisplayObject* (*hkgDisplayObject::create)();

void hkgDisplayObject::optimize()
{
	int i;
	int ng;
	ng = m_solidGeoms.getSize();
	for (i =0; i < ng; ++i)
		m_solidGeoms[i]->optimize();
	
	ng = m_alphaGeoms.getSize();
	for (i =0; i < ng; ++i)
		m_alphaGeoms[i]->optimize();

	ng = m_mixedGeoms.getSize();
	for (i =0; i < ng; ++i)
		m_mixedGeoms[i]->optimize();


}

void hkgDisplayObject::render(hkgDisplayContext* context) const
{
	// do nothing
}

void hkgDisplayObject::setMaterialOnAll( hkgMaterial* mat )
{
	hkArray<hkgGeometry*>* ga;
	int i;
	int ng;
	if (mat->isTransparent())
	{
		m_geomTransp = HKG_GEOMETRY_TRANS_TRANSPARENT;
		ga = &m_alphaGeoms;

		// place all geoms in the m_alphaList and then set all to this material
		ng = m_solidGeoms.getSize();
		for (i =0; i < ng; ++i)
			m_alphaGeoms.pushBack( m_solidGeoms[i] );
		
		ng = m_mixedGeoms.getSize();
		for (i =0; i < ng; ++i)
			m_alphaGeoms.pushBack( m_mixedGeoms[i] );

		m_mixedGeoms.setSize(0);
		m_solidGeoms.setSize(0);
	}
	else
	{
		m_geomTransp = HKG_GEOMETRY_TRANS_OPAQUE;		
		ga = &m_solidGeoms; 

		// place all geoms in the m_solidList and then set all to this material
		ng = m_alphaGeoms.getSize();
		for (i =0; i < ng; ++i)
			m_solidGeoms.pushBack( m_alphaGeoms[i] );
		
		ng = m_mixedGeoms.getSize();
		for (i =0; i < ng; ++i)
			m_solidGeoms.pushBack( m_mixedGeoms[i] );

		m_mixedGeoms.setSize(0);
		m_alphaGeoms.setSize(0);
	}

	// set the material
	ng = ga->getSize();
	for (i =0; i < ng; ++i)
	{
		hkgGeometry* geom = (*ga)[i];
		geom->setMaterialOnAll(mat);
	}
}

void hkgDisplayObject::renderGeomsInOrder(hkgDisplayContext* context) const
{
	hkArray<hkgGeometry*>* ga;
	
	switch (m_geomTransp)
	{
		case HKG_GEOMETRY_TRANS_OPAQUE:
		{
			ga = const_cast<hkArray<hkgGeometry*>*>( &m_solidGeoms );
			break;
		}

		case HKG_GEOMETRY_TRANS_TRANSPARENT:
		{
			ga = const_cast<hkArray<hkgGeometry*>*>( &m_alphaGeoms );
			break;
		}

		default: // mixed or unknown:
		{
			HKG_ENABLED_STATE cs = (HKG_ENABLED_STATE)( context->getEnabledState() & HKG_ENABLED_ZWRITE );
			HKG_ENABLED_STATE ds = (HKG_ENABLED_STATE)( context->getCurrentViewport()->getDesiredState() & HKG_ENABLED_ZWRITE );

			int ng;
			int i;
			// do we have any solid geoms?
			if (m_geomTransp & HKG_GEOMETRY_TRANS_OPAQUE)
			{
				if (cs != ds)
					context->setDepthWriteState( ds != 0 );

				ng = m_solidGeoms.getSize();
				for (i =0; i < ng; ++i)
					m_solidGeoms[i]->render(context);
			}
			
			// do we have any mixed geoms?
			// they set thier own state
			ng = m_mixedGeoms.getSize();
			for (i =0; i < ng; ++i)
				m_mixedGeoms[i]->render(context);
		
			// any pure alpha geoms
			if (m_geomTransp & HKG_GEOMETRY_TRANS_TRANSPARENT)
			{
				context->setDepthWriteState( false );

				ng = m_alphaGeoms.getSize();
				for (i =0; i < ng; ++i)
					m_alphaGeoms[i]->render(context);

				context->setDepthWriteState( true );
			}

			return;
		}
	}

	// normal 
	int ng = ga->getSize();
	for (int i =0; i < ng; ++i)
		(*ga)[i]->render(context);

}

int hkgDisplayObject::intersect( const hkgRay& ray, bool ignoreBackfaces, hkgDisplayObjectPickData& data) const
{
	// have to transform ray of we have a non identity transform
	hkgRay localRay( ray );

	if (!m_identityTrans)
	{
		float inv[16];
		hkgMat4QuickInvert( inv, m_trans );
		localRay.transform( inv );
	}

	hkgGeometryPickData gdata;

	int ng;
	int i;
	float minDist = FLT_MAX;
	int numHits = 0;

	ng = m_solidGeoms.getSize();
	for (i =0; i < ng; ++i)
	{
		int h = m_solidGeoms[i]->intersect( localRay, ignoreBackfaces, gdata);
		if (h >0)
		{
			if ((gdata.m_facePickData.m_dist > 0) && (gdata.m_facePickData.m_dist < minDist))
			{
				numHits += h;
			
				minDist = gdata.m_facePickData.m_dist;
				data.m_geomPickData = gdata;
				data.m_geomIndex = i;
			}
		}
	}	
	
	ng = m_alphaGeoms.getSize();
	for (i =0; i < ng; ++i)
	{
		int h = m_alphaGeoms[i]->intersect( localRay, ignoreBackfaces, gdata);
		if (h > 0)
		{
			if ((gdata.m_facePickData.m_dist > 0) && (gdata.m_facePickData.m_dist < minDist))
			{
				numHits += h;
			
				minDist = gdata.m_facePickData.m_dist;
				data.m_geomPickData = gdata;
				data.m_geomIndex = i + m_solidGeoms.getSize();
			}
		}
	}

	
	ng = m_mixedGeoms.getSize();
	for (i =0; i < ng; ++i)
	{
		int h = m_mixedGeoms[i]->intersect( localRay, ignoreBackfaces, gdata);
		if (h > 0)
		{
			if ((gdata.m_facePickData.m_dist > 0) && (gdata.m_facePickData.m_dist < minDist))
			{
				numHits += h;
		
				minDist = gdata.m_facePickData.m_dist;
				data.m_geomPickData = gdata;
				data.m_geomIndex = i + m_solidGeoms.getSize() + m_alphaGeoms.getSize();
			}
		}
	}

	return numHits;
}

static inline void AABB_UPDATE(  float maxInOutA[3], const float maxInB[3],
							float minInOutA[3], const float minInB[3])
{
	for (int j =0; j < 3; ++j)
	{
		if (maxInOutA[j] < maxInB[j])
			maxInOutA[j] = maxInB[j];
	
		if (minInOutA[j] > minInB[j])
			minInOutA[j] = minInB[j];
	}
}

void hkgDisplayObject::computeAABB()
{
	float minE[3];
	float maxE[3];

	minE[0] = minE[1] = minE[2] = 3e+38f;
	maxE[0] = maxE[1] = maxE[2] = -3e+38f;
	
	int ng = m_solidGeoms.getSize();
	int i;
	for ( i = 0; i < ng; ++i)
	{
		m_solidGeoms[i]->computeAABB();
		const float* gmax = m_solidGeoms[i]->getAABBMaxPtr();
		const float* gmin = m_solidGeoms[i]->getAABBMinPtr();
		AABB_UPDATE( maxE, gmax, minE, gmin );
	}

	ng = m_alphaGeoms.getSize();
	for ( i = 0; i < ng; ++i)	
	{
		m_alphaGeoms[i]->computeAABB();
		const float* gmax = m_alphaGeoms[i]->getAABBMaxPtr();
		const float* gmin = m_alphaGeoms[i]->getAABBMinPtr();
		AABB_UPDATE( maxE, gmax, minE, gmin );
	}

	ng = m_mixedGeoms.getSize();
	for ( i = 0; i < ng; ++i)	
	{
		m_mixedGeoms[i]->computeAABB();
		const float* gmax = m_mixedGeoms[i]->getAABBMaxPtr();
		const float* gmin = m_mixedGeoms[i]->getAABBMinPtr();
		AABB_UPDATE( maxE, gmax, minE, gmin );
	}

	hkgVec3Sub( m_aabbExtent, maxE, minE );
	hkgVec3Scale( m_aabbExtent, 0.5f );
	m_computedRadius = hkgVec3Length(m_aabbExtent);
	hkgVec3Add( m_aabbCent, minE, m_aabbExtent );
}

void hkgDisplayObject::getAABB(float minE[3], float maxE[3]) const
{		
	if (!m_identityTrans)
	{
		float et[3];
		float wc[3];
		
		// world cent
		hkgVec3Add( wc, &(m_trans[12]), m_aabbCent);

		// world half extents
		hkgVec3Copy( et, m_aabbExtent);
		hkgVec3Rotate( et, m_trans );

		hkgVec3Add( maxE, wc, et);
		hkgVec3Sub( minE, wc, et);
	}
	else
	{
		hkgVec3Add( maxE, m_aabbCent, m_aabbExtent);
		hkgVec3Sub( minE, m_aabbCent, m_aabbExtent);
	}
}

void hkgDisplayObject::computeTransparency()
{
	if (m_mixedGeoms.getSize() > 0)
	{
		m_geomTransp = HKG_GEOMETRY_TRANS_MIXED;
		return; // no need to look any further..
	}

	m_geomTransp = HKG_GEOMETRY_TRANS_UNKNOWN;

	if (m_solidGeoms.getSize() > 0)
		m_geomTransp |= HKG_GEOMETRY_TRANS_OPAQUE;

	if (m_alphaGeoms.getSize() > 0)
		m_geomTransp |= HKG_GEOMETRY_TRANS_TRANSPARENT;

}

static void _getGeomStats( const hkgGeometry* geom, hkArray<const hkgTexture*>& textures, int& numMatFaces, int& numTexMatFaceSets)
{
	int j,k;
	for (j=0; j<geom->getNumMaterialFaceSets();++j)
	{
		const hkgMaterialFaceSet* fs = geom->getMaterialFaceSet(j);
		numMatFaces++;
		if (fs->getMaterial() && fs->getMaterial()->getTexture())
		{
			const hkgTexture* t = fs->getMaterial()->getTexture();
			for (k=0; k <textures.getSize();++k)
			{
				if (t == textures[k])
					break; // seen it before
			}

			if ( k == textures.getSize())
				textures.pushBack( t );
		
			numTexMatFaceSets++;
		}
	}
}

void hkgDisplayObject::getTextureStats(int& numMatFaces, int& numTexMatFaceSets, int& numTextures ) const
{
	int i;
	
	hkArray<const hkgTexture*> textures;
	
	numTexMatFaceSets = 0;
	numMatFaces = 0;

	for (i = 0; i < m_solidGeoms.getSize(); ++i)
	{
		hkgGeometry* geom = m_solidGeoms[i];
		_getGeomStats( geom, textures, numMatFaces, numTexMatFaceSets );
	}
	
	for (i = 0; i < m_alphaGeoms.getSize(); ++i)
	{
		hkgGeometry* geom = m_alphaGeoms[i];
		_getGeomStats( geom, textures, numMatFaces, numTexMatFaceSets );
	}
	
	for (i = 0; i < m_mixedGeoms.getSize(); ++i)
	{
		hkgGeometry* geom = m_mixedGeoms[i];
		_getGeomStats( geom, textures, numMatFaces, numTexMatFaceSets );
	}

	numTextures = textures.getSize();
}


void hkgDisplayObject::print(hkOstream& stream) const
{
	int i,j;
	
	stream << "\tName : [" << m_name.cString() << "].\n";
	stream << "\tTransparency : [" <<
		(m_geomTransp == HKG_GEOMETRY_TRANS_OPAQUE?
			"opaque" : 
		(m_geomTransp == HKG_GEOMETRY_TRANS_TRANSPARENT? 
			"fully transparent" : 
		(m_geomTransp == HKG_GEOMETRY_TRANS_MIXED? 
			"mixed" : "unknown"))) << "]\n";


	if (m_identityTrans)
	{
		stream <<"\tMatrix : [Identity]\n";
	}
	else
	{
		stream <<"\tMatrix : \n\t\t";
		for (i=0; i < 4; ++i)
		{
			for (j=0;j < 4; ++j)
				stream << m_trans[j*4 + i] << " ";
			stream << "\n\t\t";
		}
		stream << "\n";
	}
		

	stream << "\t[" << m_solidGeoms.getSize() << "] Solid Geoms:\n\n";
	for (i=0; i < m_solidGeoms.getSize(); ++i)
		m_solidGeoms[i]->print( stream );
	
	stream << "\t[" << m_alphaGeoms.getSize() << "] Alpha Geoms:\n\n";
	for (i=0; i < m_alphaGeoms.getSize(); ++i)
		m_alphaGeoms[i]->print( stream );
	
	stream << "\t[" << m_mixedGeoms.getSize() << "] Mixed Geoms:\n\n";
	for (i=0; i < m_mixedGeoms.getSize(); ++i)
		m_mixedGeoms[i]->print( stream );
}

void hkgDisplayObject::bake(hkgDisplayContext* context)
{
	// replace all solid geoms with one solid geom and bake it.
	// By moving them down we get a full solid material face set list
	// for this body that we can then try and fix up.

	// Solid
	int i,j;
	for (i=1; i < m_solidGeoms.getSize();++i)
	{
		hkgGeometry* dest_geom = m_solidGeoms[0];
		
		hkgGeometry* geom = m_solidGeoms[i];
		int nfs = geom->getNumMaterialFaceSets();
		for (j=0; j<nfs;++j)
		{
			dest_geom->addMaterialFaceSet( geom->getMaterialFaceSet(j) );
		}

		geom->release();
	}

	// Alpha
	for (i=1; i < m_alphaGeoms.getSize();++i)
	{
		hkgGeometry* dest_geom = m_alphaGeoms[0];
		
		hkgGeometry* geom = m_alphaGeoms[i];
		int nfs = geom->getNumMaterialFaceSets();
		for (j=0; j<nfs;++j)
		{
			dest_geom->addMaterialFaceSet( geom->getMaterialFaceSet(j) );
		}

		geom->release();
	}

	// Distribute mixed between alpha and solid geoms
	for (i=0; i < m_mixedGeoms.getSize();++i)
	{
		hkgGeometry* geom = m_mixedGeoms[i];
		int nfs = geom->getNumMaterialFaceSets();
		for (j=0; j<nfs;++j)
		{
			hkgGeometry* dest_geom;
			hkgMaterialFaceSet* matFaceSet = geom->getMaterialFaceSet(j);
			if (matFaceSet->getMaterial() && matFaceSet->getMaterial()->isTransparent())
			{
				if (m_alphaGeoms.getSize() == 0)
				{
					hkgGeometry* g = hkgGeometry::create(); 
					m_alphaGeoms.pushBack( g ); // have the reference
				}
					
				dest_geom= m_alphaGeoms[0];
			}
			else
			{
				if (m_solidGeoms.getSize() == 0)
				{
					hkgGeometry* g = hkgGeometry::create(); 
					m_solidGeoms.pushBack( g ); // have the reference
				}
									
					
				dest_geom= m_solidGeoms[0];
			}

			dest_geom->addMaterialFaceSet( matFaceSet );
		}

		geom->release();
	}

	// resize results
	// and proceed to bake the geoms
	if (m_solidGeoms.getSize() > 1)
	{
		hkgGeometry* g = m_solidGeoms[0];
		m_solidGeoms.setSize(1);
		m_solidGeoms[0] = g;

		g->bake(context);
	}

	if (m_alphaGeoms.getSize() > 1)
	{
		hkgGeometry* g = m_alphaGeoms[0];
		m_alphaGeoms.setSize(1);
		m_alphaGeoms[0] = g;

		g->bake(context);
	}

	m_mixedGeoms.setSize(0);
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
