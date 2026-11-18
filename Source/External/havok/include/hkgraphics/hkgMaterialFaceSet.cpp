/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphics/hkGraphics.h> // precompiled
#include <hkgraphics/hkgMaterialFaceSet.h>
#include <hkgraphics/hkgViewport.h>
#include <hkgraphics/hkgVertexSet.h>

hkgMaterialFaceSet* (*hkgMaterialFaceSet::create)();

hkgMaterialFaceSet::~hkgMaterialFaceSet()
{
	int l = m_faceSets.getSize();
	for(int i = 0;i<l;++i)
		m_faceSets[i]->release();

	if (m_material)
		m_material->release();
}

void hkgMaterialFaceSet::optimize()
{
	int nfs = m_faceSets.getSize();
	for (int i = 0; i < nfs; ++i)
	{
		hkgFaceSet* faceSet = m_faceSets[i];
		faceSet->strip( HKG_FACE_SET_STRIP_CACHE24 );
		faceSet->reorderVertexSet();
	}
}

void hkgMaterialFaceSet::render(hkgDisplayContext* context) const
{
	// if we have disabled textures, but the transparency comes 
	// from the texture, then we should disable the zwite 
	HKG_ENABLED_STATE s = context->getEnabledState();

	bool needZWrite = (m_material == HK_NULL) || !m_material->isTransparent();

	if (needZWrite)
		needZWrite = (( s & HKG_ENABLED_TEXTURE2D ) == 0) && 
						m_material && 
					  (( m_material->getTransparentSource() & HKG_MATERIAL_TRANS_SOURCE_COLOR ) == 0) &&
					  (  context->getCurrentViewport()->getDesiredState() & HKG_ENABLED_ZWRITE);

	bool needZSwap = needZWrite && ( (s & HKG_ENABLED_ZWRITE) == 0);
	
	if (needZSwap)
		context->setDepthWriteState(true);

	// if the material has a texture, bind to it.
	if (m_material)
		m_material->bind(context);
	else if (s & HKG_ENABLED_TEXTURE2D)
	{
		hkgMaterial::unbind(context);
	}

	int nfs = m_faceSets.getSize();
	for (int i = 0; i < nfs; ++i)
	{
		hkgFaceSet* faceSet = m_faceSets[i];
		faceSet->render(context);
	}

	if (needZSwap)
		context->setDepthWriteState(false);
}

void hkgMaterialFaceSet::print(hkOstream& stream) const
{
	stream << "\t\t\t\tMaterial:\n";
	if (m_material)
		m_material->print(stream);
	else
		stream << "\t\t\t\t[NONE]\n";

	stream << "\t\t\t\t [" << m_faceSets.getSize() << "] face sets:\n";
	for (int i=0; i< m_faceSets.getSize();++i)
		m_faceSets[i]->print(stream);

}	

void hkgMaterialFaceSet::bake(hkgDisplayContext* context)
{
	// make just one face set (sum total verts etc)
	// TODO: Have to make sure that we don't bake over the number of total verts per face set (unsigned short indexes..)


	// Find all face sets that have the same vertex format
	int numFaceSets = m_faceSets.getSize();

	hkArray<hkgFaceSet*> unique;
	hkArray<int> uniqueSizes;

	hkArray<int> sameAsArray;
	sameAsArray.setSize( numFaceSets );

	int i,j,p,v;
	
	for (i=0; i < numFaceSets; ++i)
	{
		hkgFaceSet* fs = m_faceSets[i]; 
		for (j=0; j < unique.getSize(); ++j)
		{
			hkgFaceSet* ufs = m_faceSets[i];
			if (fs->getVertices()->getVertexFormat() == ufs->getVertices()->getVertexFormat())
			{
				sameAsArray[i] = j;
				uniqueSizes[j] += fs->getVertices()->getNumVerts();
				break;
			}
		}

		if (j == unique.getSize())
		{
			unique.pushBack( fs );
			uniqueSizes.pushBack( fs->getVertices()->getNumVerts() );
			sameAsArray[i] = -1;
		}
	}

	int numUniqueFS = unique.getSize();
	if (numUniqueFS != numFaceSets) // do we need to search?
	{
		for (i=0; i<numUniqueFS; ++i)
		{
			hkgFaceSet* ufs = unique[i];
			hkgVertexSet* uverts = ufs->getVertices();

			if (uniqueSizes[i] == uverts->getNumVerts() )
				continue; // then no change

			// make a holder for the new set
			hkgFaceSet* nufs = hkgFaceSet::create(context);
			hkgVertexSet* newVerts = nufs->getVertices();

			bool color = (uverts->getVertexFormat() & HKG_VERTEXSET_COLOR) != 0;
			bool uvs = (uverts->getVertexFormat() & HKG_VERTEXSET_UV) != 0;

			newVerts->setNumVerts( uniqueSizes[i], uvs, color);
			newVerts->lock();
			uverts->lock();

			unsigned int numAddedCheck = 0;
			// copy the verts and prims from the orig across
			//Verts:
			for (v =0; v < uverts->getNumVerts(); ++v)
			{
				newVerts->addPosInfo( uverts->getPosInfo(v),1 );
				newVerts->addNormalInfo( uverts->getNormalInfo(v),1 );
				if ( color )
					newVerts->addColorInfo( uverts->getColorInfo(v),1 );
				if ( uvs )
					newVerts->addUVInfo( uverts->getUVInfo(v),1 );
			
				++numAddedCheck;
			}
			
			uverts->unlock();

			//Prims
			for (p=0; p < ufs->getNumPrimitives(); ++p)
			{
				const hkgFaceSetPrimitive* prim = ufs->getPrimitive(p);
				hkgFaceSetPrimitive* newPrim = new hkgFaceSetPrimitive( (*prim) );
				nufs->addPrimitive( newPrim );
			}


			// find all alike to this one and append
			for (j=0;j<sameAsArray.getSize(); ++j)
			{	
				if (sameAsArray[j] == i)
				{
					hkgFaceSet* fs = m_faceSets[j];
					hkgVertexSet* ivs = fs->getVertices();
					ivs->lock();

					// Verts
					int indexoffset = newVerts->getPosAddCounter();

					for ( v =0; v < ivs->getNumVerts(); ++v)
					{
						newVerts->addPosInfo( ivs->getPosInfo(v),1 );
						newVerts->addNormalInfo( ivs->getNormalInfo(v),1 );
						if ( color )
							newVerts->addColorInfo( ivs->getColorInfo(v),1 );
						if ( uvs )
							newVerts->addUVInfo( ivs->getUVInfo(v),1 );

						++numAddedCheck;
					}
					
					ivs->unlock();

					//Prims
					for (p=0; p < fs->getNumPrimitives(); ++p)
					{
						const hkgFaceSetPrimitive* prim = fs->getPrimitive(p);
						hkgFaceSetPrimitive* newPrim = new hkgFaceSetPrimitive( (*prim), indexoffset );
						nufs->addPrimitive( newPrim );
					}		
					
					fs->release();
				}
			}

			HK_ASSERT( newVerts->getNumVerts() == (int)numAddedCheck );

			newVerts->unlock();
		
			//
			// cleanup
			//

			ufs->release(); // delete old.
			unique[i] = nufs; // store new
		}

		// update faceset list
		m_faceSets.setSize( numUniqueFS );
		for (i=0; i<numUniqueFS ;++i)
		{
			m_faceSets[i] = unique[i];
			m_faceSets[i]->collapse();
		}
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
