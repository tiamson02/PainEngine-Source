/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkmath2/hkMath.h>
#include <hkcollide2/shape/hkConvexVerticesShape.h>
#include <hkgeometry2/hkGeometry.h>
#include <hkgeometryutil2/hkGeometryChecker.h>
#include <hkgeometryutil2/hkGeometryUtil.h>
#include <hkdynamics2/entity/hkRigidBodyConstructionInfo.h>
#include <hkdynamics2/entity/hkRigidBody.h>
#include <hkutilities2/inertia/hkInertiaTensorComputer.h>
#include <hkcollide2/shape/hkListShape.h>
#include <hkutilities2/fractureutil/hkFractureUtil.h>
/////////////////////////////////////////////////

hkFractureUtil::hkFractureUtil()
{

}

/////////////////////////////////////////////////
	   
hkFractureUtil::~hkFractureUtil()
{

}

/////////////////////////////////////////////////

void hkFractureUtil::createFragments(const hkGeometry* originalGeom, hkArray<hkRigidBody*>& theBodies,
	                                 const hkReal& percent,const hkReal& totalMass,const hkVector4& linVel,
									 const hkVector4& offSet)
{
  int currentDepth = 0;
  
  float tolerance = .01f;

  hkReal totalVolume = getVolume(originalGeom);
  
  m_ithNormalUsed = 0;
  m_normalArray.setSize(1000);
  
  createRandomNormals();   
  
  hkArray<hkGeometry*> geomArray;

  sliceGeometryIteratively(originalGeom,percent,geomArray);
   
  geometryToBodies(theBodies,geomArray,totalVolume,totalMass,linVel,offSet);
}

//////////////////////////////////////////////////////////

// This function returns a listShape from which a compound
// rigid body can be made. Using other utilities this body
// can be fractured upon a collision event. Prior to that
// it acts as one body

hkListShape*  hkFractureUtil::createFragments(const hkGeometry* originalGeom, 
                                              hkArray<hkMassProperties>* massProperties,
											  const hkReal& percent,const hkReal& totalMass)
{
  int currentDepth = 0;
  float tolerance = .01f;

  hkReal totalVolume = getVolume(originalGeom);
  
  m_ithNormalUsed = 0;
  m_normalArray.setSize(1000);
  
  createRandomNormals();   
  
  hkArray<hkGeometry*> geomArray;

  sliceGeometryIteratively(originalGeom,percent,geomArray);

  hkShape** theShapes = hkAllocate<hkShape*>(geomArray.getSize(),HK_MEMORY_CLASS_TOOLS);

  massProperties->setSize(geomArray.getSize());

  geometryToShapes(theShapes,geomArray,totalVolume,totalMass,massProperties);

  hkListShape* listShape = new hkListShape(theShapes,geomArray.getSize());

  hkDeallocate<hkShape*>(theShapes);

  return listShape;
}

//////////////////////////////////////////////////

hkConvexVerticesShape* hkFractureUtil::createConvexShape(hkGeometry* geometry)
{
	int numVert;
	const hkVector4* vertexData;
    float* vertexArray;

	hkConvexVerticesShape* cShape;

	numVert = geometry->getNumVertices();
    vertexData = geometry->getVertices();
    vertexArray = hkAllocate<float>(numVert*3,HK_MEMORY_CLASS_TOOLS);
 
	for(int j = 0; j < numVert; j++)
	{
       vertexArray[j * 3] = vertexData[j](0);
       vertexArray[j * 3 + 1] = vertexData[j](1);
       vertexArray[j * 3 + 2] = vertexData[j](2);
	}

	cShape = new hkConvexVerticesShape(vertexArray,sizeof(float)*3,numVert);
 	hkDeallocate<float>(vertexArray);

	return cShape;
}

//////////////////////////////////////////////////
 
void hkFractureUtil::createRandomNormals()
{
  // Code ripped from JGT article on Halton sample points

  float p, t, st, phi, phirad, ip;
  int k, kk, pos, a,n,p1,p2;

  p1 = 2;
  p2 = 3;
  n = 10000;
  m_normalArray.setSize(n);
  
  for (k = 0, pos = 0  ; k < n ; k++)
  {
    t = 0;
    ip = 1.0f / p1;                    
	
	// kk = (int)(kk/p1)
    for (p = ip, kk = k ; kk ; p *= ip, kk /= p1) 
	{
      if ((a = kk % p1))
	  {
	    t += a * p;
	  }
	}

    // map from [0,1] to [-1,1]
    t = 2.0f * t  - 1.0f;                    
    st = hkMath::sqrt(1.0f - t * t);

    phi = 0;
    ip = 1.0f / p2;       

	// kk = (int)(kk/p2)
    for (p = ip, kk = k ; kk ; p *= ip, kk /= p2) 
	{
      if ((a = kk % p2))
	  {
		  phi += a * p;
	  }
	}

    // map from [0,0.5] to [0, 2 pi)
	phirad =  phi * 4.0f * hkMath::HK_REAL_PI;            
    
	m_normalArray[k](0) = st * hkMath::cos(phirad);
    m_normalArray[k](1) = st * hkMath::sin(phirad);
    m_normalArray[k](2) = t;
    //m_normalArray[k].normalize3();
  }
}

///////////////////////////////////////////////////
// Ripped from 1.6 Newton Branch
hkReal hkFractureUtil::getVolume(const hkGeometry* g)
{
	// Apply random rotation to avoid "aligned triangles" numerical error
	hkVector4 axis(.3252f,-.82424f, 0.53453f);
    axis.normalize3();
	hkQuaternion q(axis,0.453f);
	hkReal volume = 0.0f;
	
	int numt= g->getNumTriangles();

	for(int i = 0;i < numt;i++)
	{
		hkVector4 p1 = g->getVertex(g->getTriangle(i).getVertex(0));
		hkVector4 p2 = g->getVertex(g->getTriangle(i).getVertex(1));
		hkVector4 p3 = g->getVertex(g->getTriangle(i).getVertex(2));

	// Apply random rotation to avoid "aligned triangles" numerical error
		p1.setRotatedDir(q,p1);
		p2.setRotatedDir(q,p2);
		p3.setRotatedDir(q,p3);

		const hkReal height = (p1(2) + p2(2) + p3(2) ) * hkReal(1.f/3.f);
		p1(2) = 0;
		p2(2) = 0;
		p3(2) = 0;

		hkVector4 sub1,sub2;
	    sub1.setSub4(p2,p1); 
        sub2.setSub4(p3,p1); 

		hkVector4 crossResult;
		crossResult.setCross(sub1,sub2);

		hkVector4 area; area.setMul4(0.5f, crossResult);
		if (area(2) > 0)
			volume += height * (hkReal)area.length3();
		else			
			volume -= height * (hkReal)area.length3();

	}

	return volume;
}

//////////////////////////////////////////////////

void hkFractureUtil::geometryToShapes(hkShape** theShapes,const hkArray<hkGeometry*>& geomArray,
           	 	                      const hkReal& totalVolume,const hkReal& totalMass,
			   				          hkArray<hkMassProperties>* massProperties)
{
    // This method is inefficient due to the required allocation and deallocation of memory
 	// However this is done once before the simulation. 

	int numBits = geomArray.getSize();
	

	int numVert;
	const hkVector4* vertexData;
    float* vertexArray;

	hkConvexVerticesShape* cShape;

    hkReal partialVolume;
 
    for(int i = 0; i < numBits; i++)
	{
		numVert = geomArray[i]->getNumVertices();
        vertexData = geomArray[i]->getVertices();
        vertexArray = hkAllocate<float>(numVert*3,HK_MEMORY_CLASS_TOOLS);
 
        partialVolume = getVolume(geomArray[i]);

		for(int j = 0; j < numVert; j++)
		{
            vertexArray[j * 3] = vertexData[j](0);
            vertexArray[j * 3 + 1] = vertexData[j](1);
            vertexArray[j * 3 + 2] = vertexData[j](2);
		}

		cShape = new hkConvexVerticesShape(vertexArray,sizeof(float)*3,numVert);
 		theShapes[i] = cShape;

		// Create mass properties for this shape
		// Assign proportion of total mass to fragment
	    (*massProperties)[i].m_mass = totalMass * (partialVolume/totalVolume);

		/// Compute the inertia tensor and the center of mass given a total mass for a hkShape (using "volume" methods)
		hkInertiaTensorComputer::computeShapeVolumeMassProperties(cShape, (*massProperties)[i].m_mass, (*massProperties)[i]);

     	/////////////////////////////////////////////////////
		hkDeallocate<float>(vertexArray);
	}	
}

///////////////////////////////////////////////////

void hkFractureUtil::geometryToBodies(hkArray<hkRigidBody*>& theBodies,hkArray<hkGeometry*>& geomArray,
        	          const hkReal& totalVolume,const hkReal& totalMass,const hkVector4& linVel,
					  const hkVector4& offSet)
{
    // This method is inefficient due to the required allocation and deallocation of memory
 	// However this is done once before the simulation. 

	int numBits = geomArray.getSize();
	theBodies.setSize(numBits);

	int numVert;
	const hkVector4* vertexData;
    float* vertexArray;

	hkRigidBody* fragment;
	hkConvexVerticesShape* cShape;
	hkRigidBodyConstructionInfo info;
    hkReal partialVolume;
 
    for(int i = 0; i < numBits; i++)
	{
		numVert = geomArray[i]->getNumVertices();
        vertexData = geomArray[i]->getVertices();
        vertexArray = hkAllocate<float>(numVert*3,HK_MEMORY_CLASS_TOOLS);
        info.m_position.set(0,0,0);
        partialVolume = getVolume(geomArray[i]);

		for(int j = 0; j < numVert; j++)
		{
            vertexArray[j * 3] = vertexData[j](0);
            vertexArray[j * 3 + 1] = vertexData[j](1);
            vertexArray[j * 3 + 2] = vertexData[j](2);
		}

		cShape = new hkConvexVerticesShape(vertexArray,sizeof(float)*3,numVert);
 
		// Now create the rigid body and its properties

		info.m_shape = cShape;
		info.m_motionType = HK_SPHERE_INERTIA_MOTION;

		hkMassProperties massProperties;

		// Assign proportion of total mass to fragment
	    info.m_mass = totalMass * (partialVolume/totalVolume);

		// Compute the inertia tensor and the center of mass given a total mass for a hkShape (using "volume" methods)
		hkInertiaTensorComputer::computeShapeVolumeMassProperties(info.m_shape, info.m_mass, massProperties);

		info.m_centerOfMass = massProperties.m_centerOfMass;
		info.m_inertiaTensor = massProperties.m_inertiaTensor;

		//  Rotate and raise the pieces so they can drop
		
		hkVector4 axis(1,1,1);
		
		axis.normalize3();

		hkQuaternion qRot(axis,.707f);
		info.m_position.setRotatedDir(qRot,info.m_position);
		info.m_rotation = qRot;
		info.m_position.add4(offSet);

		fragment = new hkRigidBody(info);

		fragment->setLinearVelocity(linVel);
	    
		info.m_shape->removeReference();

		/////////////////////////////////////////////////////
		hkDeallocate<float>(vertexArray);

		theBodies[i] = fragment;
	}	
}

////////////////////////////////////////////////////

void hkFractureUtil::sliceGeometryIteratively(const hkGeometry* inputGeom,const hkReal& minVolumePercent, 
											  hkArray<hkGeometry*>& geomArray)
{
  bool done = false;
  hkGeometry *g1, *g2;
  long i,size;
  hkArray<long> largeVolIndices; 
  hkReal volume1,volume2,minVolume,tol;
   
  hkVector4 normal;
  hkReal distance;
  tol = .00001f;

  getRandomSlicePlane(inputGeom, normal, distance);
  sliceGeometry(inputGeom, normal, distance, tol, g1,g2); 
  
  geomArray.pushBack(g1);
  geomArray.pushBack(g2);
 
  
  for(i = 0 ; i < geomArray.getSize(); i++)
  {
    largeVolIndices.pushBack(i);
  }

  // largeVolIndices contains the indices into the geomArray of pieces
  // with volume > minVolume
  
  volume1 = getVolume(inputGeom);
  minVolume = volume1 * minVolumePercent; // minVolumePercent (0,1];

  while(!done)
  {
     size = largeVolIndices.getSize();
     
	 for(i = 0 ; i < largeVolIndices.getSize(); i++)
	 {
        getRandomSlicePlane(geomArray[largeVolIndices[i]], normal, distance);
        sliceGeometry(geomArray[largeVolIndices[i]], normal, distance, tol, g1,g2);

		if((g1 != HK_NULL) && (g2 != HK_NULL))
		{
		   hkDeallocate<hkGeometry>(geomArray[largeVolIndices[i]]);
           geomArray[largeVolIndices[i]] = g1;
		   geomArray.pushBack(g2);
		   volume1 = getVolume(g1);
		   volume2 = getVolume(g2);

           if(volume1 <= minVolume && volume2 <= minVolume)
		   {
             largeVolIndices.removeAt(i);
		   }
		   else if(volume2 > minVolume && volume1 <= minVolume)
		   {
             largeVolIndices[i] = geomArray.getSize() - 1; 
		   }
		   else if(volume2 > minVolume && volume1 > minVolume)
		   {
             largeVolIndices.pushBack(geomArray.getSize() - 1);
		   }
		}
		else
		{
          g1 = HK_NULL;
		}
	 }
      
	 if(!largeVolIndices.getSize() )
	 {
       done = true;
	 }
  }

}

/////////////////////////////////////////////////////

void hkFractureUtil::getRandomSlicePlane(const hkGeometry* inputGeom, hkVector4& planeNormal, 
			                             hkReal& planeDistance)
{
	// Find the AABB, centroid. Use center as plane center, pick random normal, 
	// and cal distance using this
    
	hkVector4 centroid(0,0,0,1);
	
	int i,numVert;

	for(i = 0; i < inputGeom->getNumVertices(); i++)
	{
		centroid.add4(inputGeom->getVertex(i));
	}

    numVert = inputGeom->getNumVertices();
	// Dumb .. but where does the /= operator exist or function??
	centroid(0) = centroid(0) /= numVert; 
	centroid(1) = centroid(1) /= numVert;
	centroid(2) = centroid(2) /= numVert;

	// Using Halton points on sphere..better distribution of cutting directions
	// in theory anyway

	planeNormal = m_normalArray[m_ithNormalUsed];
	planeNormal.normalize3();
	m_ithNormalUsed++;

	if(m_ithNormalUsed >= m_normalArray.getSize())
	{
      m_ithNormalUsed = 0;
	}

	planeDistance = centroid.dot3(planeNormal);

}

//////////////////////////////////////////////////////

void hkFractureUtil::sliceGeometry(const hkGeometry* inputGeom, hkVector4& planeNormal, 
  	                               hkReal planeDistance, hkReal tolerance, hkGeometry* &output1, 
   			                       hkGeometry* &output2)
{
	// Need to partition all vertices into two sets first
	hkArray<hkVector4> newVert1;
	hkArray<hkVector4> newVert2;

	planeNormal.normalize3();

	hkReal minDot = 100000.0f;
	hkReal maxDot = -100000.0f;

	int i;
	for(i = 0; i < inputGeom->getNumVertices(); i++)
	{
		hkVector4 v = inputGeom->getVertex(i);
		hkReal dot3 = planeNormal.dot3(v);
		if(dot3 <  planeDistance - tolerance * 0.25f)
		{
			newVert1.pushBack(v);
		}
		
		if(dot3 > planeDistance + tolerance * 0.25f)
		{
			newVert2.pushBack(v);
		}

		minDot = (dot3 < minDot)? dot3: minDot;
		maxDot = (dot3 > maxDot)? dot3: maxDot;
	}


	hkArray<hkGeomCheckerEdgeIndices> edges;
	hkGeometryChecker::findAllEdges(inputGeom, edges);

		// Need to do twice, once for vertices "below" plane, once for vertices "above"
	for(i = 0; i < edges.getSize(); i++)
	{
		hkGeomCheckerEdgeIndices edge = edges[i];
		hkVector4 start = inputGeom->getVertex(edge.getVertex(0));
		hkVector4 end = inputGeom->getVertex(edge.getVertex(1));

		
		// Do "below"
		{
			hkReal newPlaneDistance = planeDistance;// - tolerance * 0.5f;

			hkReal d0 = (hkReal)start.dot3(planeNormal) - newPlaneDistance;
			hkReal d1 = (hkReal)end.dot3(planeNormal) - newPlaneDistance;

				// For ease of thinking about this, we'll reorder so that start is "under" plane,
				// and end is "over"
			if(d1 < 0)
			{
				hkReal td = d1;
				d1 = d0;
				d0 = td;
				hkVector4 tv = end;
				end = start;
				start = tv;
			}

			// If opposite sides
			if(d0 * d1 < 0.0f)
			{
     			// Find intersection
				hkReal sdot = hkMath::fabs((hkReal)start.dot3(planeNormal) - newPlaneDistance);
				hkVector4 diff; diff.setSub4(end, start);
				hkReal alldot = hkMath::fabs(diff.dot3(planeNormal));
				hkReal ratio0 = sdot / alldot;

				hkVector4 v0; v0.setAddMul4( start, diff, ratio0);
				newVert1.pushBack(v0);
			}
		}

		// Do "above"
		{
			hkReal newPlaneDistance = planeDistance;// + tolerance * 0.5f;


			hkReal d0 = (hkReal)start.dot3(planeNormal) - newPlaneDistance;
			hkReal d1 = (hkReal)end.dot3(planeNormal) - newPlaneDistance;

				// For ease of thinking about this, we'll reorder so that start is "under" plane,
				// and end is "over"
			if(d1 < 0)
			{
				hkReal td = d1;
				d1 = d0;
				d0 = td;
				hkVector4 tv = end;
				end = start;
				start = tv;
			}

			// If opposite sides
			if(d0 * d1 < 0.0f)
			{
				
				// Find intersection
				hkReal sdot = hkMath::fabs((hkReal)start.dot3(planeNormal) - newPlaneDistance);
				hkVector4 diff; diff.setSub4(end, start);
				hkReal alldot = hkMath::fabs(diff.dot3(planeNormal));
				hkReal ratio1 = sdot  / alldot;

				hkVector4 v1; v1.setAddMul4( start, diff, ratio1 );
				newVert2.pushBack(v1);

			}
		}
	}

	output1 = output2 = HK_NULL;
    hkcout << "Creating Geometry" << hkendl;
	output1 = hkGeometryUtil::createConvexGeometry(&(newVert1[0]), newVert1.getSize(), false,.00001f);
	output2 = hkGeometryUtil::createConvexGeometry(&(newVert2[0]), newVert2.getSize(), false,.00001f);
    hkcout << "Geometry done "<< hkendl;
	if((output1 == HK_NULL) || (output2 == HK_NULL))
	{
	    // HK_ASSERT(0);
		output1 = output2 = HK_NULL;
	}
}

////////////////////////////////////////////////////////

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
