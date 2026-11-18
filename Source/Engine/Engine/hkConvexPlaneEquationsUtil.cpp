// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM		: ALL
// PRODUCT		: HAVOK_2
// VISIBILITY	: PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

#include <hkgeometryutil2/hkGeometryUtil2.h>
#include <hkgeometry2/hkTriangleUtil.h>

#include <hkcollide2/shape/hkConvexVerticesShape.h>


HK_FORCE_INLINE bool hkConvexPlaneEquationsUtil::tryAddPlane(hkVector4& triangleSupportingPlaneDirection,
															 hkReal triangleSupportingPlaneConstant,
															 hkConvexVerticesShape* convexShape, 
															 const Tolerances& tolerances,
															 hkArray<hkVector4>& planeEquations
															 )
{
	int vertexId = 0;
	hkVector4 supportVertex;
	// Find vertex with max distance along triangleSupportingPlaneDirection
	convexShape->getSupportingVertex( triangleSupportingPlaneDirection , supportVertex ,vertexId );

	hkReal& maxPlaneConstant = triangleSupportingPlaneDirection(3);

	maxPlaneConstant = - hkReal(triangleSupportingPlaneDirection.dot3(supportVertex));
	
	//
	// Check that no vertex is "in front of" this plane. The vertex with max dot product is
	// the support vertex, and it has distance maxPlaneConstant
	//
	if (hkMath::equal(	maxPlaneConstant,	triangleSupportingPlaneConstant,tolerances.m_hullDistanceTolerance ))
	{
		int i;

		for (i=0;i<planeEquations.getSize();i++)
		{
		
			//if (planeEquations[i].equals3( triangleSupportingPlaneDirection,tolerances.m_planeNormalEqualityTolerance ))

			const hkVector4& testPlane = planeEquations[i];
			hkReal cosangle = testPlane.dot3( triangleSupportingPlaneDirection );
			if (cosangle > (1.f-tolerances.m_planeNormalEqualityTolerance) )
			{
				// roughly equal plane is already there
				return true;
			}
		}

		planeEquations.pushBack( triangleSupportingPlaneDirection );
		return true;

	}

	return false;
}


bool hkConvexPlaneEquationsUtil::computePlanesFromVertices(const float *verticesin, int numverts, int vertexStriding, hkArray<hkVector4>& planeEquations, hkArray<hkVector4>& usedVertices, const Tolerances& tolerances )
{

	// idea is to take k evenly distributed directions, 
	// get the supporting vertex and build planeequations
	// only add vertices that contribute to planeequations to the 'usedvertices'
	// this effectively removed 'internal' vertices (internal meaning inside the volume)

	if (numverts == 3)
	{
		return generateTrianglePlanes(verticesin, numverts, vertexStriding, planeEquations, usedVertices,tolerances);
	}

	bool checkInternal = true;

	hkArray<bool> usedVertexIndices;
	usedVertexIndices.setSize( numverts , false);
	

	hkArray<hkVector4> tmpPlaneEquations;


    m_setTag("Convex");
	hkConvexVerticesShape* convexShape = new hkConvexVerticesShape(	verticesin, vertexStriding,	numverts, tmpPlaneEquations);

	int i,j,k;
	for (i=0;i<numverts;i++)
	{
		hkReal* vf0 = hkAddByteOffset<float>((float*)verticesin, vertexStriding * i );
		hkVector4 vert0 (*vf0,*(vf0+1),*(vf0+2));

		for (j=i+1;j<numverts;j++)
		{
			hkReal* vf1 = hkAddByteOffset<float>((float*)verticesin, vertexStriding * j );
			hkVector4 vert1 (*vf1,*(vf1+1),*(vf1+2));

			for (k=j+1;k<numverts;k++)
			{
				hkReal* vf2 = hkAddByteOffset<float>((float*)verticesin, vertexStriding * k );
				hkVector4 vert2 (*vf2,*(vf2+1),*(vf2+2));

				if (0)
				{
					hkprintf("vert0 = (%f %f %f)\n",vert0(0),vert0(1),vert0(2));
					hkprintf("vert1 = (%f %f %f)\n",vert1(0),vert1(1),vert1(2));
					hkprintf("vert2 = (%f %f %f)\n",vert2(0),vert2(1),vert2(2));
				}

				
				{
					hkVector4 direction;
					hkVector4 edge0,edge1;
					edge0.setSub4(vert1,vert0);
					edge1.setSub4(vert2,vert0);
					direction.setCross(edge0,edge1);
					hkReal halfarea = direction.lengthSquared3();
					
					//don't add degenerate triangles
					if (halfarea > tolerances.m_planeNormalEqualityTolerance )
					{
						//normalize direction
						direction.mul4( hkMath::sqrtInverse( halfarea ));
						hkReal planeConstant;
						planeConstant = - hkReal(direction.dot3(vert0));
						// if supportVertex is in the plane, use it, else throw it away
						if (tryAddPlane ( direction , planeConstant, convexShape,  tolerances, planeEquations))
						{
							usedVertexIndices[i]=true;
							usedVertexIndices[j]=true;
							usedVertexIndices[k]=true;
						}
						//also use opposite direction vector
						direction.setNeg4( direction );
						planeConstant = - hkReal(direction.dot3(vert0));
						if (tryAddPlane ( direction , planeConstant, convexShape,  tolerances, planeEquations))
						{
							usedVertexIndices[i]=true;
							usedVertexIndices[j]=true;
							usedVertexIndices[k]=true;
						}
						
					}
					
				} 
			}
		}
	}

	HK_ASSERT2(planeEquations.getSize() >= 4,"degenerate input detected, unable to generate at least 4 planes");

	//
	//add the used vertices
	//
	if ( planeEquations.getSize() >= 4 )
	{
		int i;
		for (i=0;i<numverts;i++)
		{
			if (usedVertexIndices[i])
			{
				hkReal* vf0 = hkAddByteOffset<float>((float*)verticesin, vertexStriding * i );
				hkVector4 vert0 (*vf0,*(vf0+1),*(vf0+2));
				usedVertices.pushBack( vert0);
			}
		}
	} else
	{
		planeEquations.clear();
	}

	convexShape->removeReference();

	return true;
}


					

bool hkConvexPlaneEquationsUtil::approximatePlanesFromVerticesWithKDop(const float *verticesin, int numverts, int vertexStriding, hkArray<hkVector4>& planeEquations, hkArray<hkVector4>& usedVertices)
{

	// idea is to take k evenly distributed directions, 

	hkArray<hkVector4> directions;
	directions.setSize(13);

	hkReal tolerance = 1e-3f;

	int i;

	//x,y, and z axes
	directions[0].set( 1.0f, 0.0f, 0.0f); 
	directions[1].set( 0.0f, 1.0f, 0.0f); 
	directions[2].set( 0.0f, 0.0f, 1.0f); 

	directions[3].set( 0.0f, 1.0f, 1.0f);
	directions[4].set( 1.0f, 0.0f, 1.0f);
	directions[5].set( 1.0f, 1.0f, 0.0f);

	directions[6].set( 0.0f, 1.0f,-1.0f);
	directions[7].set( 1.0f, 0.0f,-1.0f);
	directions[8].set( 1.0f,-1.0f, 0.0f);

	directions[9].set( 1.0f, 1.0f, 1.0f);
	directions[10].set( 1.0f, 1.0f,-1.0f);
	directions[11].set( 1.0f,-1.0f,-1.0f);
	directions[12].set( 1.0f,-1.0f,1.0f);

	// normalize directions
	for (i = 0; i < 13; i++)
	{
		directions[i].normalize3();
	}

	bool checkInternal = false;

	hkArray<hkVector4> tmpPlaneEquations;
    m_setTag("Convex");
    hkConvexVerticesShape* convexShape = new hkConvexVerticesShape(
		verticesin, 
		vertexStriding,
		numverts,
		tmpPlaneEquations);


	for (i=0;i<directions.getSize();i++)
	{
		hkVector4 planeEquation = directions[i];

		int vertexId = 0;
		hkVector4 supportVertex;
		// Find vertex with max distance along triangleSupportingPlaneDirection
		convexShape->getSupportingVertex( planeEquation , supportVertex ,vertexId );

		usedVertices.pushBack( supportVertex );
		planeEquation (3) = - hkReal(planeEquation .dot3(supportVertex));
		planeEquations.pushBack( planeEquation );

		//also use the opposite direction to generate a plane
		planeEquation.setNeg4(directions[i]);

		convexShape->getSupportingVertex( planeEquation , supportVertex ,vertexId );
		usedVertices.pushBack( supportVertex );
		planeEquation (3) = - hkReal(planeEquation .dot3(supportVertex));
		
		planeEquations.pushBack( planeEquation );
		
	}

	convexShape->removeReference();

	return true;
}

bool hkConvexPlaneEquationsUtil::generateTrianglePlanes(const float *verticesin, int numverts, int vertexStriding, hkArray<hkVector4>& planeEquations, hkArray<hkVector4>& usedVertices, const Tolerances& tolerances)
{


	int vecid = 0;
		
	// add the six bevel planes from the aabb relative to this triangle
	// add the 2 support planes for the triangle
	// add the edge-edge bevel planes
	hkReal* vf0 = hkAddByteOffset<float>((float*)verticesin, vertexStriding * 0 );
	hkReal* vf1 = hkAddByteOffset<float>((float*)verticesin, vertexStriding * 1 );
	hkReal* vf2 = hkAddByteOffset<float>((float*)verticesin, vertexStriding * 2 );

	hkVector4 vertices[3] = {
		hkVector4(*vf0,*(vf0+1),*(vf0+2)),
		hkVector4(*vf1,*(vf1+1),*(vf1+2)),
		hkVector4(*vf2,*(vf2+1),*(vf2+2))
	};
	
	//add the 2 support planes
	hkVector4 trianglePlane;
	hkVector4 v10; v10.setSub4( vertices[1], vertices[0]);
	hkVector4 v20; v20.setSub4( vertices[2], vertices[0]);
	trianglePlane.setCross(v10, v20);
	hkReal area = trianglePlane.lengthSquared3();
	if (area >= tolerances.m_planeNormalEqualityTolerance )
	{
		trianglePlane.mul4( hkMath::sqrtInverse( area));

		trianglePlane(3) = - hkReal(vertices[0].dot3(trianglePlane));
		planeEquations.pushBack(trianglePlane);
		trianglePlane.setNeg4(trianglePlane);
		trianglePlane(3) = - hkReal(vertices[0].dot3(trianglePlane));
		planeEquations.pushBack(trianglePlane);
		
		//3 edge-planes...
		hkVector4 prevvert,edge;
		prevvert = vertices[2];
		for (int i=0;i<3;i++)
		{
			edge.setSub4( prevvert, vertices[i]);
			prevvert= vertices[i];
			hkVector4 edgePlane;
			edgePlane.setCross(edge,trianglePlane);
			edgePlane.normalize3();
			edgePlane(3) = - hkReal(prevvert.dot3(edgePlane));
			planeEquations.pushBack(edgePlane);
		};

		usedVertices.pushBack(vertices[0]);
		usedVertices.pushBack(vertices[1]);
		usedVertices.pushBack(vertices[2]);

		return true;
	} else
	{
		HK_ASSERT2(0,"degenerate triangle doesn't return any planeequations");
	
	}
	return false;
}