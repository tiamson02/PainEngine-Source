/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphics/hkGraphics.h>
#include <hkgraphics/hkgGeometry.h>
#include <hkgraphics/hkgViewport.h>

hkgGeometry* (*hkgGeometry::create)();

hkgGeometry::~hkgGeometry()
{
	// solid
	int nmfs = m_matSolidFaceSets.getSize();
	int i;
	for (i =0; i < nmfs; ++i)
	{
		hkgMaterialFaceSet* fs = m_matSolidFaceSets[i];
		fs->release();
	}

	// alpha
	nmfs = m_matAlphaFaceSets.getSize();
	for ( i =0; i < nmfs; ++i)
	{
		hkgMaterialFaceSet* fs = m_matAlphaFaceSets[i];
		fs->release();
	}
}

void hkgGeometry::optimize()
{
	// solid
	int nmfs = m_matSolidFaceSets.getSize();
	int i;
	for (i =0; i < nmfs; ++i)
	{
		hkgMaterialFaceSet* fs = m_matSolidFaceSets[i];
		fs->optimize();
	}

	// alpha
	nmfs = m_matAlphaFaceSets.getSize();
	for ( i =0; i < nmfs; ++i)
	{
		hkgMaterialFaceSet* fs = m_matAlphaFaceSets[i];
		fs->optimize();
	}

}

void hkgGeometry::setMaterialOnAll( hkgMaterial* mat )
{
	// all faces are now either all solid or all transparent

	if (mat->isTransparent())
	{
		// swap over the face sets
		int nmfs = m_matSolidFaceSets.getSize();
		int i;
		for (i =0; i < nmfs; ++i)
		{
			hkgMaterialFaceSet* fs = m_matSolidFaceSets[i];
			m_matAlphaFaceSets.pushBack( fs );
		}

		m_matSolidFaceSets.setSize(0);

		// set the material
		int nmafs = m_matAlphaFaceSets.getSize();
		for ( i =0; i < nmafs; ++i)
		{
			hkgMaterialFaceSet* fs = m_matAlphaFaceSets[i];
			fs->setMaterial( mat );
		}

		m_trans = HKG_GEOMETRY_TRANS_TRANSPARENT;
	}
	else
	{
		// swap over the face sets
		int nmfs = m_matAlphaFaceSets.getSize();
		int i;
		for (i =0; i < nmfs; ++i)
		{
			hkgMaterialFaceSet* fs = m_matAlphaFaceSets[i];
			m_matSolidFaceSets.pushBack( fs );
		}

		m_matAlphaFaceSets.setSize(0);
		
		// set the material
		int nmafs = m_matSolidFaceSets.getSize();
		for ( i =0; i < nmafs; ++i)
		{
			hkgMaterialFaceSet* fs = m_matSolidFaceSets[i];
			fs->setMaterial( mat );
		}

		m_trans = HKG_GEOMETRY_TRANS_OPAQUE;
	}
}

void hkgGeometry::recomputeAlphaSets()
{
	hkArray<hkgMaterialFaceSet*> newSolid;
	hkArray<hkgMaterialFaceSet*> newAlpha;

	newSolid.reserve( m_matSolidFaceSets.getSize() );
	newAlpha.reserve( m_matAlphaFaceSets.getSize() );

	//alpha
	int nmfs = m_matAlphaFaceSets.getSize();
	int i;
	for (i =0; i < nmfs; ++i)
	{
		hkgMaterialFaceSet* fs = m_matAlphaFaceSets[i];
		if (fs->getMaterial()->isTransparent())
			newAlpha.pushBack( fs );
		else
			newSolid.pushBack( fs );
	}
	
	// solid
	nmfs = m_matAlphaFaceSets.getSize();
	for (i =0; i < nmfs; ++i)
	{
		hkgMaterialFaceSet* fs = m_matSolidFaceSets[i];
		if (fs->getMaterial()->isTransparent())
			newAlpha.pushBack( fs );
		else
			newSolid.pushBack( fs );
	}

	//
	// set the arrays
	//

	m_trans = HKG_GEOMETRY_TRANS_UNKNOWN;

	m_matAlphaFaceSets.setSize( newAlpha.getSize() );
	m_matSolidFaceSets.setSize( newSolid.getSize() );

	// alpha
	nmfs = newAlpha.getSize();
	for (i =0; i < nmfs; ++i)
		m_matAlphaFaceSets[i] = newAlpha[i];
	
	if (nmfs > 0)
		m_trans |= HKG_GEOMETRY_TRANS_TRANSPARENT;

	// solid
	nmfs = newSolid.getSize();
	for (i =0; i < nmfs; ++i)
		m_matSolidFaceSets[i] = newSolid[i];

	if (nmfs > 0)
		m_trans |= HKG_GEOMETRY_TRANS_OPAQUE;

}
	

bool hkgGeometry::removeMaterialFaceSet( hkgMaterialFaceSet* ms )
{
	m_trans = HKG_GEOMETRY_TRANS_UNKNOWN;

	// alpha
	int i;
	int nmfs = m_matAlphaFaceSets.getSize();
	for (i =0; i < nmfs; ++i)
	{
		if (m_matAlphaFaceSets[i] == ms)
		{
			m_matAlphaFaceSets.removeAt(i);
			ms->release();
			
			if (nmfs == 1)
				m_trans &= ~HKG_GEOMETRY_TRANS_TRANSPARENT; // no alpha parts left

			return true;
		}
	}

	
	// solid
	nmfs = m_matSolidFaceSets.getSize();
	for (i =0; i < nmfs; ++i)
	{
		if (m_matSolidFaceSets[i] == ms)
		{
			m_matSolidFaceSets.removeAt(i);
			ms->release();
			
			if (nmfs == 1)
				m_trans &= ~HKG_GEOMETRY_TRANS_OPAQUE; // no solid parts left

			return true;
		}
	}

	return false;
}


void hkgGeometry::mixedModeRender(hkgDisplayContext* context) const
{
	hkgViewport* v = context->getCurrentViewport();
	HKG_ENABLED_STATE ds = (HKG_ENABLED_STATE)( v->getDesiredState() & HKG_ENABLED_ZWRITE );
	HKG_ENABLED_STATE cs = (HKG_ENABLED_STATE)( context->getEnabledState() & HKG_ENABLED_ZWRITE );

	// render solid with depth write on (if part of desired state)
	if ( cs != ds )
		context->setDepthWriteState( ds != 0 );

	// solid
	int i;
	int nmfs = m_matSolidFaceSets.getSize();
	for (i =0; i < nmfs; ++i)
		m_matSolidFaceSets[i]->render( context );

	// alpha. Always turn z write off
	cs = (HKG_ENABLED_STATE)( context->getEnabledState() & HKG_ENABLED_ZWRITE );
	if ( cs )
		context->setDepthWriteState( false);

	nmfs = m_matAlphaFaceSets.getSize();
	for (i =0; i < nmfs; ++i)
		m_matAlphaFaceSets[i]->render( context );

	if ( cs )
		context->setDepthWriteState( true ); // leave state reasonable
}

int hkgGeometry::intersect( const hkgRay& ray, bool ignoreBackfaces, hkgGeometryPickData& data) const
{
	float minDist = FLT_MAX;
	
	hkgFaceSetPickData fdata;

	int numHits = 0;

	// solid
	int i, j;
	int nmfs = m_matSolidFaceSets.getSize();
	for (i =0; i < nmfs; ++i)
	{
		int nfs = m_matSolidFaceSets[i]->getNumFaceSets();
		for (j =0; j < nfs; ++j)
		{
			unsigned int h = m_matSolidFaceSets[i]->getFaceSet(j)->intersect( ray, ignoreBackfaces, fdata);
			if (h > 0)
			{
				if ((fdata.m_dist > 0) && (fdata.m_dist < minDist))
				{
					numHits += h;
				
					data.m_facePickData = fdata;
					data.m_matFaceSetIndex = i;
					data.m_matFaceSetSubIndex = j;
					minDist = fdata.m_dist;
				}
			}
		}
	}

	// alpha
	nmfs = m_matAlphaFaceSets.getSize();
	for (i =0; i < nmfs; ++i)
	{
		int nfs = m_matAlphaFaceSets[i]->getNumFaceSets();
		for (j =0; j < nfs; ++j)
		{
			int h = m_matAlphaFaceSets[i]->getFaceSet(j)->intersect( ray, ignoreBackfaces, fdata);
			if (h > 0)
			{
				if ((fdata.m_dist > 0) && (fdata.m_dist < minDist))
				{
					numHits += h;
				
					data.m_facePickData = fdata;
					data.m_matFaceSetIndex = i;
					data.m_matFaceSetSubIndex = j;
					minDist = fdata.m_dist;
				}
			}
		}
	}

	return numHits;
}


void hkgGeometry::computeAABB()
{
	if (m_computedAABB)
	// reset min,max
	{
		m_aabbMin[0] = m_aabbMin[1] = m_aabbMin[2] =  3e+38f;
		m_aabbMax[0] = m_aabbMax[1] = m_aabbMax[2] = -3e+38f;
	}

	// solid
	int i, j;
	int nmfs = m_matSolidFaceSets.getSize();
	for (i =0; i < nmfs; ++i)
	{
		int nfs = m_matSolidFaceSets[i]->getNumFaceSets();
		for (j =0; j < nfs; ++j)
			m_matSolidFaceSets[i]->getFaceSet(j)->computeAABB(m_aabbMin, m_aabbMax);
		
	}

	// alpha
	nmfs = m_matAlphaFaceSets.getSize();
	for (i =0; i < nmfs; ++i)
	{
		int nfs = m_matAlphaFaceSets[i]->getNumFaceSets();
		for (j =0; j < nfs; ++j)
		{
			m_matAlphaFaceSets[i]->getFaceSet(j)->computeAABB(m_aabbMin, m_aabbMax);
		}
	}

	m_computedAABB = true;
}

int hkgGeometry::makeSingleFaceSet(hkgDisplayContext* context)
{
	hkgMaterialFaceSet* fs = hkgMaterialFaceSet::create();
	hkgFaceSet* faceSet = hkgFaceSet::create(context);
	
	fs->addFaceSet( faceSet );
	faceSet->release(); // let fs have it

	int index = addMaterialFaceSet(fs);	
	fs->release();

	return index;
}

void hkgGeometry::print(hkOstream& stream) const
{
	stream << "\t\tName :[" << m_name.cString() << "]\n";
	stream << "\t\tTransparency : [" <<
		(m_trans== HKG_GEOMETRY_TRANS_OPAQUE?
			"opaque" : 
		(m_trans == HKG_GEOMETRY_TRANS_TRANSPARENT? 
			"fully transparent" : 
			"unknown" )) << "]\n";

	stream << "\t\t[" << m_matSolidFaceSets.getSize() << "] Solid face sets:\n";
	int i;
	for (i = 0; i < m_matSolidFaceSets.getSize(); ++i)
		m_matSolidFaceSets[i]->print(stream);
	
	stream << "\t\t[" << m_matAlphaFaceSets.getSize() << "] Alpha face sets:\n";
	for (i = 0; i < m_matAlphaFaceSets.getSize(); ++i)
		m_matAlphaFaceSets[i]->print(stream);
	
}

static void mungeFaceSets(hkArray<hkgMaterialFaceSet*>& faceSetList)
{
	int i,j,k;
	
	hkArray<hkgMaterialFaceSet*> unique;
	hkgMaterialFaceSet* nomaterial = HK_NULL;

	bool munged = false;
	for (i =0; i < faceSetList.getSize(); i++)
	{
		hkgMaterialFaceSet* fs = faceSetList[i];
		hkgMaterial* m = fs->getMaterial();
		if (m)
		{
			for (j =0; j< unique.getSize(); ++j)
			{
				hkgMaterialFaceSet* ufs = unique[j];
				hkgMaterial* testm = ufs->getMaterial();
				if ( ( m == testm ) || ((*m) == (*testm)) )
				{
					// Munge fs into ufs and release fs	
					for (k=0; k<fs->getNumFaceSets(); ++k)
					{
						hkgFaceSet* faceSet = fs->getFaceSet(k);
						ufs->addFaceSet( faceSet ); 
						// no need to release as we will release the whole material face set
					}
					
					fs->release();
					munged = true;
					break;
				}
			}

			if (j == unique.getSize())
				unique.pushBack( fs );
		}
		else
		{
			if (nomaterial)
			// munge
			{
				for (k=0; k<fs->getNumFaceSets(); ++k)
				{
					hkgFaceSet* faceSet = fs->getFaceSet(k);
					nomaterial->addFaceSet( faceSet ); 
					// no need to release as we will release the whole material face set
				}
				fs->release();
				munged = true;
			}
			else
				nomaterial = fs;
		}
	}

	if (munged)
	{

		int size = unique.getSize() + ( (nomaterial == HK_NULL)? 0 : 1);
		
		faceSetList.setSize( size );

		if (nomaterial != HK_NULL)
		{
			faceSetList[0] = nomaterial;
			i = 1;
		}
		else
			i = 0;

		j = 0;

		for (; i < size; ++i, ++j)
			faceSetList[i] = unique[j];
	}
}

void hkgGeometry::bake(hkgDisplayContext* context)
{
	// match all mat face sets based on material
	// This might not have happened at load time due to texture differences that
	// maybe now don't exist etc.


	mungeFaceSets( m_matSolidFaceSets );
	mungeFaceSets( m_matAlphaFaceSets );

	// bake down

	int i;
	for (i=0; i < m_matSolidFaceSets.getSize(); ++i)
		m_matSolidFaceSets[i]->bake(context);

	for (i=0; i < m_matAlphaFaceSets.getSize(); ++i)
		m_matAlphaFaceSets[i]->bake(context);
}

void hkgGeometry::transform( const float t[16] )
{
	int i,j;
	for (i=0; i < m_matSolidFaceSets.getSize(); ++i)
	{
		hkgMaterialFaceSet* mfs = m_matSolidFaceSets[i];
		for (j=0; j<mfs->getNumFaceSets(); ++j)
			mfs->getFaceSet( j )->transform(t);
	}

	for (i=0; i < m_matAlphaFaceSets.getSize(); ++i)
	{
		hkgMaterialFaceSet* mfs = m_matAlphaFaceSets[i];
		for (j=0; j<mfs->getNumFaceSets(); ++j)
			mfs->getFaceSet( j )->transform(t);
	}
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
