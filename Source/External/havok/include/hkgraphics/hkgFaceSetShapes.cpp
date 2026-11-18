/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphics/hkGraphics.h> // precomp

#include <hkgraphics/hkgGeometry.h>
#include <hkgraphics/hkgMaterialFaceSet.h>
#include <hkgraphics/hkgFaceSet.h>
#include <hkgraphics/hkgVertexSet.h>

void hkgFaceSet::makeSphere( const float center[3], float radius, int numMajor, int numMinor, bool uv, bool vcolor)
{
	int rows = numMajor + 1;
	int cols = numMinor + 1;

	int numVerts = rows * cols;
	int numIndices = (2*numMajor) * cols;

	m_vertices->setNumVerts( numVerts, uv, vcolor ); 
	m_vertices->lock();
	
	float majorStep = (HKG_PI / (float)numMajor); // 180 degrees
    float minorStep = (2.0f*HKG_PI / (float)numMinor); // 360 degrees

	// first and last vertex are the two apexs

	float tempPos[3];
	float tempNorm[3];
	float tempTex[2];
	float tempColor[3];

	tempColor[0] = tempColor[1] = tempColor[2] = 1.0f;
	
	int i, j;

	for (i=0; i < rows; ++i) 
	{
		const float a = i * majorStep;
		
		const float sa = hkg_sin(a);
		const float r0 = radius * sa;

		const float ca = hkg_cos(a);
		const float z0 = radius * ca;
		
		for (j=0; j < cols; ++j) 
		{ 
			const float c = j * minorStep;
			const float x = hkg_cos(c);
			const float y = hkg_sin(c);

			tempPos[0] = x*r0 + center[0];
			tempPos[1] = y*r0 + center[1];
			tempPos[2] = z0 + center[2];
		
			tempNorm[0] = x*sa;
			tempNorm[1] = y*sa;
			tempNorm[2] = ca;
				
			m_vertices->addPosInfo(tempPos, 1);
			m_vertices->addNormalInfo(tempNorm, 1);

			if (uv)
			{
				tempTex[0] = j/(float)numMinor; 
				tempTex[1] = i/(float)numMajor; 
				m_vertices->addUVInfo(tempTex, 1);	
			}

			if (vcolor)
				m_vertices->addColorInfo(tempColor, 1);
		}
	}
	
	m_vertices->unlock();
	

	//
	// fill index buffer
	//
	hkgFaceSetPrimitive* prim = new hkgFaceSetPrimitive(HKG_STRIP);
	prim->setLength(numIndices);
	
	addPrimitive( prim ); // owns it

	unsigned short* conn = prim->getIndices();

	bool dirUp = true;
	int c = 0;

	// fill strip info
	for (i = 0; i < numMajor; i++) // res not cols
	{
		int start0 = cols*i;
		int start1 = cols*(i+1);

		if (dirUp)
		{
			for (j = 0; j< cols; j++) // rows not res
			{
				conn[c++] = start0 + j;
				conn[c++] = start1 + j;
			}
		}
		else 
		{
			for (j = cols-1; j >= 0; j--) // rows not res
			{
				conn[c++] = start1 + j;
				conn[c++] = start0 + j;
			}
		}	

		dirUp = !dirUp;
	}

	HK_ASSERT( c == numIndices );
}


void hkgFaceSet::makeGrid( const float corner[3], const float sideA[3], const float sideB[3], int resA, int resB, bool uv, bool vcolor)
{
	int rowsX = resA +1;
	int colsY = resB +1;

	int numVertices = rowsX * colsY; 
	int numIndices = (2*rowsX) * resB;
		
	float normal[3];
	hkgVec3Cross( normal, sideB, sideA );

	m_vertices->setNumVerts( numVertices, uv, vcolor );
	m_vertices->lock();

	// fill vertices

	float tempTex[2];
	float tempColor[3];

	tempColor[0] = tempColor[1] = tempColor[2] = 1.0f;

	int i, j;
	for (i = 0; i < colsY; i++)
	{
		float vy[3];
		hkgVec3Mult( vy, sideB, i/(float)(colsY-1) );
		hkgVec3Add( vy, corner );
		
		for (j = 0; j < rowsX; j++)
		{
			float vxy[3];
			hkgVec3Mult( vxy, sideA, j/(float)(rowsX-1) );
			hkgVec3Add( vxy, vy );

			m_vertices->addPosInfo(vxy, 1);
			m_vertices->addNormalInfo(normal, 1);

			if (uv)
			{
				tempTex[0] = j/(float)rowsX; 
				tempTex[1] = i/(float)colsY; 
				m_vertices->addUVInfo(tempTex, 1);	
			}

			if (vcolor)
				m_vertices->addColorInfo(tempColor, 1);
		}
	}

	m_vertices->unlock();

	hkgFaceSetPrimitive* prim = new hkgFaceSetPrimitive(HKG_STRIP);
	prim->setLength(numIndices);
	
	addPrimitive( prim ); // owns it

	unsigned short* conn = prim->getIndices();

	bool dirUp = true;
	int c = 0;

	// fill strip info
	for (i = 0; i < resB; i++) // res not cols
	{
		int start0 = rowsX*i;
		int start1 = rowsX*(i+1);

		if (dirUp)
		{
			for (j = 0; j< rowsX; j++) // rows not res
			{
				conn[c++] = start0 + j;
				conn[c++] = start1 + j;
			}
		}
		else 
		{
			for (j = rowsX -1; j>= 0; j--) // rows not res
			{
				conn[c++] = start1 + j;
				conn[c++] = start0 + j;
			}
		}	

		dirUp = !dirUp;
	}

	HK_ASSERT( c == numIndices );
}


static void _fillBoxSideVerts(hkgVertexSet* vs, const float from[3], const float sideA[3], const float sideB[3], int ra, int rb, bool uv, bool vcolor)
{
	float temp0[3];
	float temp1[3];
	float tempTex[2];
	float tempColor[3] = {1.0f, 1.0f, 1.0f};
	float norm0[3];

	hkgVec3Cross( norm0, sideB, sideA);
	hkgVec3Normalize( norm0 );
	
	int resA = ra -1;
	int resB = rb -1;

	for (int i=0; i < ra; ++i)
	{
		float ia = i/(float)resA; //0-->1
		hkgVec3Mult( temp0, sideA, ia);
		hkgVec3Add( temp0, from );

		for (int j=0; j < rb; ++j)
		{
			float jb = j/(float)resB; 
			hkgVec3Mult( temp1, sideB, jb);
			hkgVec3Add( temp1, temp0 );
						
			vs->addPosInfo(temp1, 1);
			vs->addNormalInfo(norm0, 1);

			if (uv)
			{
				tempTex[0] = ia; 
				tempTex[1] = jb; 
				vs->addUVInfo(tempTex, 1);	
			}

			if (vcolor)
				vs->addColorInfo(tempColor, 1);
		}
	}
}

static int _fillBoxSideIndices(unsigned short* conn, int coffset, int voffset, int ra, int rb, bool ccw )
{
	int index = coffset;
	int resA = ra -1;
	int resB = rb -1;

	for (int i = 0; i < resA ;++i)
	{
		int rowOffset = i*rb + voffset;

		for (int j = 0; j < resB ;++j)
		{
			int i0 = rowOffset + j;
			int i1 = i0 + 1;
			int i2 = i0 + rb;
			int i3 = i2 + 1;

			if (ccw)
			{
				conn[index++] = i0;
				conn[index++] = i2;
				conn[index++] = i3;

				conn[index++] = i0;
				conn[index++] = i3;
				conn[index++] = i1;
			}
			else
			{
				conn[index++] = i0;
				conn[index++] = i3;
				conn[index++] = i2;
				
				conn[index++] = i0;
				conn[index++] = i1;
				conn[index++] = i3;
			}
		}
	}

	return index;
}

void hkgFaceSet::makeBox( const float corner[3], const float sideA[3], const float sideB[3], const float sideC[3], int resA, int resB, int resC, bool uv, bool vcolor)
{
	// as a box does has a good few discontinual parts (edges) to be a nice stip, if we are not tesselating too much then 
	// we can just make a simple tri list.
	// TODO: the highly tesselated case.

	int ra = resA + 1;
	int rb = resB + 1;
	int rc = resC + 1;

	// for each of the 3 faces sets of two faces each, sum
	int numVerts = (ra*rb + ra*rc + rb*rc) *2;

	m_vertices->setNumVerts( numVerts, uv, vcolor ); 
	m_vertices->lock();
	
	float tpos[3];

	// AB
	_fillBoxSideVerts( m_vertices, corner, sideA, sideB, ra, rb, uv, vcolor);
	hkgVec3Add( tpos, corner, sideC);
	_fillBoxSideVerts( m_vertices, tpos, sideB, sideA, rb, ra, uv, vcolor);


	// CA
	_fillBoxSideVerts( m_vertices, corner, sideC, sideA, rc, ra, uv, vcolor);
	hkgVec3Add( tpos, corner, sideB);
	_fillBoxSideVerts( m_vertices, tpos, sideA, sideC, ra, rc, uv, vcolor);


	// BC
	_fillBoxSideVerts( m_vertices, corner, sideB, sideC, rb, rc, uv, vcolor);
	hkgVec3Add( tpos, corner, sideA);
	_fillBoxSideVerts( m_vertices, tpos, sideC, sideB, rc, rb, uv, vcolor);

	//
	// the verts are now done, we just need to triangluate them.
	//

	hkgFaceSetPrimitive* prim = new hkgFaceSetPrimitive(HKG_TRI_LIST);
	
	int numTris = (resA*resB + resA*resC + resB*resC)*4;
	int numIndices = numTris * 3;

	prim->setLength( numIndices );

	int cindex = 0;
	int voffset = 0;
	
	//AB
	cindex = _fillBoxSideIndices( prim->getIndices(), cindex, voffset, ra, rb, false);
	voffset += ra * rb;
	cindex = _fillBoxSideIndices( prim->getIndices(), cindex, voffset, rb, ra, false);
	voffset += ra * rb;

	//CA
	cindex = _fillBoxSideIndices( prim->getIndices(), cindex, voffset, rc, ra, false);
	voffset += ra * rc;
	cindex = _fillBoxSideIndices( prim->getIndices(), cindex, voffset, ra, rc, false);
	voffset += ra * rc;

	//BC
	cindex = _fillBoxSideIndices( prim->getIndices(), cindex, voffset, rb, rc, false);
	voffset += rb * rc;
	cindex = _fillBoxSideIndices( prim->getIndices(), cindex, voffset, rc, rb, false);

	HK_ASSERT ( (voffset + (rb * rc)) == numVerts );
	HK_ASSERT ( cindex == numIndices );

	m_vertices->unlock();

	addPrimitive(prim);
}

void hkgFaceSet::makeCone( const float from[3], const float to[3], float radius0, float radius1, int numMajor, int numMinor, bool cap, bool uv, bool vcolor)
{

	//
	// Main body of the cone
	// 
	
	int rows = numMajor + 1;
	int cols = numMinor + 1;

	int numVerts = rows * cols; 

	bool fromCap = cap && (hkg_fabs(radius0) > 0.05f);
	bool toCap = cap && (hkg_fabs(radius1) > 0.05f);

	if (fromCap)
		numVerts += cols * 2;
	if (toCap)
		numVerts += cols * 2;
		

	int numStripIndices = (2*cols) * numMajor;

	m_vertices->setNumVerts( numVerts, uv, vcolor ); 
	m_vertices->lock();
	
	
	float minorStep = (2.0f*HKG_PI / (float)numMinor); // 360 degrees

	// first and last vertex are the two apexs

	float tempPos[3];
	float tempNorm[3];
	float tempTex[2];
	float tempColor[3];

	tempColor[0] = tempColor[1] = tempColor[2] = 1.0f;
	
	int i, j;

	float shaft[3];
	float pivot[3];
	hkgVec3Sub( shaft, to, from );
	float unitShaft[3];
	hkgVec3Copy( unitShaft, shaft );
	hkgVec3Normalize( unitShaft );

	// basis
	float XAXIS[3] = { 1,0,0 };
	float YAXIS[3] = { 0,1,0 };
	
	float basis[3];
	float normalBasis[3];

	if (hkg_fabs(hkgVec3Dot( unitShaft, XAXIS )) > 0.95f)
		hkgVec3Copy( tempPos, YAXIS );
	else
		hkgVec3Copy( tempPos, XAXIS );

	hkgVec3Cross( basis, unitShaft, tempPos);

	// normal
	//float negBasis[3];
	float slopeFrom[3];
	float slopeTo[3];
	float slope[3];
	hkgVec3Mult( slopeFrom, basis, radius0);
	hkgVec3Mult( slopeTo, basis, radius1);
	hkgVec3Sub( slope, slopeTo, slopeFrom);
	hkgVec3Normalize( slope );

	//hkgVec3Mult( negBasis, basis, -1);

	float dist = hkgVec3Dot( slope, basis );

	if (hkg_fabs(dist) > 0.05f)
	{
		hkgVec3Mult( normalBasis, slope, dist );
		hkgVec3Normalize( normalBasis );
	}
	else // basis is prep to slope, ie, is cyl
		hkgVec3Copy( normalBasis, basis);

	for (i=0; i < rows; ++i) 
	{
		float inc = i / (float)(rows-1);
		hkgVec3Mult( pivot, shaft, inc  );
		hkgVec3Add( pivot, from );

		float r0 = radius0 + ( radius1 - radius0 ) * inc;

		for (j=0; j < cols; ++j) 
		{ 
			const float c = j * -minorStep;

			float rot[4];
			hkgQuatFromAngleAxis( rot, c, unitShaft );
			hkgQuatRotateVec3( tempPos, rot, basis );
			hkgVec3Scale( tempPos, r0 );
			hkgVec3Add( tempPos, pivot);

			hkgQuatRotateVec3( tempNorm, rot, normalBasis );
				
			m_vertices->addPosInfo(tempPos, 1);
			m_vertices->addNormalInfo(tempNorm, 1);

			if (uv)
			{
				tempTex[0] = j/(float)numMinor; 
				tempTex[1] = i/(float)numMajor; 
				m_vertices->addUVInfo(tempTex, 1);	
			}

			if (vcolor)
				m_vertices->addColorInfo(tempColor, 1);
		}
	}

	//
	// fill index buffer
	//
	hkgFaceSetPrimitive* prim = new hkgFaceSetPrimitive(HKG_STRIP);
	prim->setLength(numStripIndices);
	
	addPrimitive( prim ); // owns it

	unsigned short* conn = prim->getIndices();

	bool dirUp = true;
	unsigned int c = 0;

	// fill strip info
	for (i = 0; i < numMajor; i++) // res not cols
	{
		int start0 = cols*i;
		int start1 = cols*(i+1);

		if (dirUp)
		{
			for (j = 0; j< cols; j++) // rows not res
			{
				conn[c++] = start0 + j;
				conn[c++] = start1 + j;
			}
		}
		else 
		{
			for (j = cols-1; j >= 0; j--) // rows not res
			{
				conn[c++] = start1 + j;
				conn[c++] = start0 + j;
			}
		}	

		dirUp = !dirUp;
	}

	HK_ASSERT( (int)c == (int)numStripIndices );

	//
	// fill in caps if need be
	//

	hkgVec3Mult(tempNorm, unitShaft, -1 );

	if (fromCap)
	{
		hkgFaceSetPrimitive* prim = new hkgFaceSetPrimitive(HKG_STRIP);
		prim->setLength(cols*2);

		for (j = 0; j< cols; j++) // rows not res
		{
			float* outerPos = m_vertices->getPosInfo(j); // first row

			m_vertices->addPosInfo(from, 1);
			m_vertices->addPosInfo(outerPos, 1);
					
			m_vertices->addNormalInfo(tempNorm, 1);
			m_vertices->addNormalInfo(tempNorm, 1);
			

			if (uv)
			{
				tempTex[0] = j/(float)numMinor; 
				tempTex[1] = 0.0f; 
				m_vertices->addUVInfo(tempTex, 1);	
			
				tempTex[1] = 1.0f; 
				m_vertices->addUVInfo(tempTex, 1);	
			}

			if (vcolor)
			{
				m_vertices->addColorInfo(tempColor, 1);
				m_vertices->addColorInfo(tempColor, 1);
			}
		}

		conn = prim->getIndices();
		c = 0;

		unsigned int offset = rows*cols;

		for (j = 0; j< cols; j++) // rows not res
		{
			unsigned int index = offset + j*2;
			conn[c++] = index;
			conn[c++] = index + 1;
		}

		addPrimitive( prim ); // owns it
	}

	hkgVec3Scale(tempNorm, -1 );

	if (toCap)
	{
		hkgFaceSetPrimitive* prim = new hkgFaceSetPrimitive(HKG_STRIP);
		prim->setLength(cols*2);

		int offset = (rows-1)*cols;

		for (j = 0; j< cols; j++) // rows not res
		{
			float* outerPos = m_vertices->getPosInfo( offset + j ); // last row

			m_vertices->addPosInfo(to, 1);
			m_vertices->addPosInfo(outerPos, 1);
					
			m_vertices->addNormalInfo(tempNorm, 1);
			m_vertices->addNormalInfo(tempNorm, 1);
			

			if (uv)
			{
				tempTex[0] = j/(float)numMinor; 
				tempTex[1] = 1.0f; 
				m_vertices->addUVInfo(tempTex, 1);	
			
				tempTex[1] = 0.0f; 
				m_vertices->addUVInfo(tempTex, 1);	
			}

			if (vcolor)
			{
				m_vertices->addColorInfo(tempColor, 1);
				m_vertices->addColorInfo(tempColor, 1);
			}
		}

		conn = prim->getIndices();
		c = 0;

		offset = (rows * cols);
		if (fromCap)
			offset+= cols*2;

		for (j = 0; j< cols; j++) // rows not res
		{
			unsigned int index = offset + j*2;
			conn[c++] = index + 1;
			conn[c++] = index;
			
		}

		addPrimitive( prim ); // owns it
	}
	
	m_vertices->unlock();
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
