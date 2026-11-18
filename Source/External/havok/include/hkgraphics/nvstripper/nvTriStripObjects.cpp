/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkgraphics/hkGraphics.h> // precompiled

#include <hkgraphics/nvstripper/nvTriStripObjects.h>
#include <hkgraphics/nvstripper/nvVertexCache.h>

#define CACHE_INEFFICIENCY 6

#define nv_MAX(x,y) ((x)>(y) ? (x) : (y))

#if defined(HK_COMPILER_MSVC)
	// conversion from 'type1' to 'type2', possible loss of data
#	pragma warning(disable: 4244)
#endif

nvStripifier::nvStripifier()
{
	
}

nvStripifier::~nvStripifier()
{
}

///////////////////////////////////////////////////////////////////////////////////////////
// FindEdgeInfo()
//
// find the edge info for these two indices
//
nvEdgeInfo * nvStripifier::FindEdgeInfo(nvEdgeInfoVec &edgeInfos, int v0, int v1){
	
	// we can get to it through either array
	// because the edge infos have a v0 and v1
	// and there is no order except how it was
	// first created.
	nvEdgeInfo *infoIter = edgeInfos[v0];
	while (infoIter != HK_NULL){
		if (infoIter->m_v0 == v0){
			if (infoIter->m_v1 == v1)
				return infoIter;
			else
				infoIter = infoIter->m_nextV0;
		}
		else {
			HK_ASSERT(infoIter->m_v1 == v0);
			if (infoIter->m_v0 == v1)
				return infoIter;
			else
				infoIter = infoIter->m_nextV1;
		}
	}
	return HK_NULL;
}


///////////////////////////////////////////////////////////////////////////////////////////
// FindOtherFace
//
// find the other face sharing these vertices
// exactly like the edge info above
//
nvFaceInfo * nvStripifier::FindOtherFace(nvEdgeInfoVec &edgeInfos, int v0, int v1, nvFaceInfo *faceInfo){
	nvEdgeInfo *edgeInfo = FindEdgeInfo(edgeInfos, v0, v1);

	if( (edgeInfo == HK_NULL) && (v0 == v1))
	{
		//we've hit a degenerate
		return HK_NULL;
	}

	HK_ASSERT(edgeInfo != HK_NULL);
	return (edgeInfo->m_face0 == faceInfo ? edgeInfo->m_face1 : edgeInfo->m_face0);
}


bool nvStripifier::AlreadyExists(nvFaceInfo* faceInfo, nvFaceInfoVec& faceInfos)
{
	for(int i = 0; i < faceInfos.getSize(); ++i)
	{
		if( (faceInfos[i]->m_v0 == faceInfo->m_v0) &&
			(faceInfos[i]->m_v1 == faceInfo->m_v1) &&
			(faceInfos[i]->m_v2 == faceInfo->m_v2) )
			return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////
// BuildStripifyInfo()
//
// Builds the list of all face and edge infos
//
void nvStripifier::BuildStripifyInfo(nvFaceInfoVec &faceInfos, nvEdgeInfoVec &edgeInfos,
									 const unsigned short maxIndex)
{
	// reserve space for the face infos, but do not resize them.
	int numIndices = indices.getSize();
	faceInfos.reserve(numIndices / 3);
	
	// we actually resize the edge infos, so we must initialize to HK_NULL
	edgeInfos.setSize(maxIndex + 1);
	int i;
	for (i = 0; i < maxIndex + 1; i++)
		edgeInfos[i] = HK_NULL;
	
	// iterate through the triangles of the triangle list
	int numTriangles = numIndices / 3;
	int index        = 0;
	bool bFaceUpdated[3];

	for (i = 0; i < numTriangles; i++)
	{	
		bool bMightAlreadyExist = true;
		bFaceUpdated[0] = false;
		bFaceUpdated[1] = false;
		bFaceUpdated[2] = false;

		// grab the indices
		unsigned short v0 = indices[index++];
		unsigned short v1 = indices[index++];
		unsigned short v2 = indices[index++];

		//we disregard degenerates
		if(IsDegenerate(v0, v1, v2))
			continue;
		
		// create the face info and add it to the list of faces, but only if this exact face doesn't already 
		//  exist in the list
		nvFaceInfo *faceInfo = new nvFaceInfo(v0, v1, v2);
		
		// grab the edge infos, creating them if they do not already exist
		nvEdgeInfo *edgeInfo01 = FindEdgeInfo(edgeInfos, v0, v1);
		if (edgeInfo01 == HK_NULL)
		{
			//since one of it's edges isn't in the edge data structure, it can't already exist in the face structure
			bMightAlreadyExist = false;

			// create the info
			edgeInfo01 = new nvEdgeInfo(v0, v1);
			
			// update the linked list on both 
			edgeInfo01->m_nextV0 = edgeInfos[v0];
			edgeInfo01->m_nextV1 = edgeInfos[v1];
			edgeInfos[v0] = edgeInfo01;
			edgeInfos[v1] = edgeInfo01;
			
			// set face 0
			edgeInfo01->m_face0 = faceInfo;
		}
		else 
		{
			if (edgeInfo01->m_face1 != HK_NULL)
			{
				hkprintf("BuildStripifyInfo: > 2 triangles on an edge... uncertain consequences\n");
			}
			else
			{
				edgeInfo01->m_face1 = faceInfo;
				bFaceUpdated[0] = true;
			}
		}
		
		// grab the edge infos, creating them if they do not already exist
		nvEdgeInfo *edgeInfo12 = FindEdgeInfo(edgeInfos, v1, v2);
		if (edgeInfo12 == HK_NULL)
		{
			bMightAlreadyExist = false;
			
			// create the info
			edgeInfo12 = new nvEdgeInfo(v1, v2);
			
			// update the linked list on both 
			edgeInfo12->m_nextV0 = edgeInfos[v1];
			edgeInfo12->m_nextV1 = edgeInfos[v2];
			edgeInfos[v1] = edgeInfo12;
			edgeInfos[v2] = edgeInfo12;
			
			// set face 0
			edgeInfo12->m_face0 = faceInfo;
		}
		else 
		{
			if (edgeInfo12->m_face1 != HK_NULL)
			{
				hkprintf("BuildStripifyInfo: > 2 triangles on an edge... uncertain consequences\n");
			}
			else
			{
				edgeInfo12->m_face1 = faceInfo;
				bFaceUpdated[1] = true;
			}
		}
		
		// grab the edge infos, creating them if they do not already exist
		nvEdgeInfo *edgeInfo20 = FindEdgeInfo(edgeInfos, v2, v0);
		if (edgeInfo20 == HK_NULL)
		{
			bMightAlreadyExist = false;

			// create the info
			edgeInfo20 = new nvEdgeInfo(v2, v0);
			
			// update the linked list on both 
			edgeInfo20->m_nextV0 = edgeInfos[v2];
			edgeInfo20->m_nextV1 = edgeInfos[v0];
			edgeInfos[v2] = edgeInfo20;
			edgeInfos[v0] = edgeInfo20;
			
			// set face 0
			edgeInfo20->m_face0 = faceInfo;
		}
		else 
		{
			if (edgeInfo20->m_face1 != HK_NULL)
			{
				hkprintf("BuildStripifyInfo: > 2 triangles on an edge... uncertain consequences\n");
			}
			else
			{
				edgeInfo20->m_face1 = faceInfo;
				bFaceUpdated[2] = true;
			}
		}

		if(bMightAlreadyExist)
		{
			if(!AlreadyExists(faceInfo, faceInfos))
				faceInfos.pushBack(faceInfo);
			else
			{
				delete faceInfo;

				//cleanup pointers that point to this deleted face
				if(bFaceUpdated[0])
					edgeInfo01->m_face1 = HK_NULL;
				if(bFaceUpdated[1])
					edgeInfo12->m_face1 = HK_NULL;
				if(bFaceUpdated[2])
					edgeInfo20->m_face1 = HK_NULL;
			}
		}
		else
		{
			faceInfos.pushBack(faceInfo);
		}

	}
}


///////////////////////////////////////////////////////////////////////////////////////////
// FindStartPoint()
//
// Finds a good starting point, namely one which has only one neighbor
//
int nvStripifier::FindStartPoint(nvFaceInfoVec &faceInfos, nvEdgeInfoVec &edgeInfos)
{
	int bestCtr = -1;
	int bestIndex = -1;

	for(int i = 0; i < faceInfos.getSize(); i++)
	{
		int ctr = 0;
		
		if(FindOtherFace(edgeInfos, faceInfos[i]->m_v0, faceInfos[i]->m_v1, faceInfos[i]) == HK_NULL)
			ctr++;
		if(FindOtherFace(edgeInfos, faceInfos[i]->m_v1, faceInfos[i]->m_v2, faceInfos[i]) == HK_NULL)
			ctr++;
		if(FindOtherFace(edgeInfos, faceInfos[i]->m_v2, faceInfos[i]->m_v0, faceInfos[i]) == HK_NULL)
			ctr++;
		if(ctr > bestCtr)
		{
			bestCtr = ctr;
			bestIndex = i;
			//return i;
		}
	}
	//return -1;
	
	if(bestCtr == 0)
		return -1;
	else
		return bestIndex;
}


///////////////////////////////////////////////////////////////////////////////////////////
// FindGoodResetPoint()
//  
// A good reset point is one near other commited areas so that
// we know that when we've made the longest strips its because
// we're stripifying in the same general orientation.
//
nvFaceInfo* nvStripifier::FindGoodResetPoint(nvFaceInfoVec &faceInfos, nvEdgeInfoVec &edgeInfos){
	// we hop into different areas of the mesh to try to get
	// other large open spans done.  Areas of small strips can
	// just be left to triangle lists added at the end.
	nvFaceInfo *result = HK_NULL;
	
	if(result == HK_NULL)
	{
		int numFaces   = faceInfos.getSize();
		int startPoint;
		if(bFirstTimeResetPoint)
		{
			//first time, find a face with few neighbors (look for an edge of the mesh)
			startPoint = FindStartPoint(faceInfos, edgeInfos);
			bFirstTimeResetPoint = false;
		}
		else
			startPoint = (int)(((float) numFaces - 1) * meshJump);
		
		if(startPoint == -1)
		{
			startPoint = (int)(((float) numFaces - 1) * meshJump);
			
			//meshJump += 0.1f;
			//if (meshJump > 1.0f)
			//	meshJump = .05f;
		}

		int i = startPoint;
		do {
			
			// if this guy isn't visited, try him
			if (faceInfos[i]->m_stripId < 0){
				result = faceInfos[i];
				break;
			}
			
			// update the index and clamp to 0-(numFaces-1)
			if (++i >= numFaces)
				i = 0;
			
		} while (i != startPoint);
		
		// update the meshJump
		meshJump += 0.1f;
		if (meshJump > 1.0f)
			meshJump = .05f;
	}
	
	// return the best face we found
	return result;
}


///////////////////////////////////////////////////////////////////////////////////////////
// GetUniqueVertexInB()
//
// Returns the vertex unique to faceB
//
int nvStripifier::GetUniqueVertexInB(nvFaceInfo *faceA, nvFaceInfo *faceB){
	
	int facev0 = faceB->m_v0;
	if (facev0 != faceA->m_v0 &&
		facev0 != faceA->m_v1 &&
		facev0 != faceA->m_v2)
		return facev0;
	
	int facev1 = faceB->m_v1;
	if (facev1 != faceA->m_v0 &&
		facev1 != faceA->m_v1 &&
		facev1 != faceA->m_v2)
		return facev1;
	
	int facev2 = faceB->m_v2;
	if (facev2 != faceA->m_v0 &&
		facev2 != faceA->m_v1 &&
		facev2 != faceA->m_v2)
		return facev2;
	
	// nothing is different
	return -1;
}


///////////////////////////////////////////////////////////////////////////////////////////
// GetSharedVertices()
//
// Returns the (at most) two vertices shared between the two faces
//
void nvStripifier::GetSharedVertices(nvFaceInfo *faceA, nvFaceInfo *faceB, int* vertex0, int* vertex1)
{
	*vertex0 = -1;
	*vertex1 = -1;

	int facev0 = faceB->m_v0;
	if (facev0 == faceA->m_v0 ||
		facev0 == faceA->m_v1 ||
		facev0 == faceA->m_v2)
	{
		if(*vertex0 == -1)
			*vertex0 = facev0;
		else
		{
			*vertex1 = facev0;
			return;
		}
	}
	
	int facev1 = faceB->m_v1;
	if (facev1 == faceA->m_v0 ||
		facev1 == faceA->m_v1 ||
		facev1 == faceA->m_v2)
	{
		if(*vertex0 == -1)
			*vertex0 = facev1;
		else
		{
			*vertex1 = facev1;
			return;
		}
	}	

	int facev2 = faceB->m_v2;
	if (facev2 == faceA->m_v0 ||
		facev2 == faceA->m_v1 ||
		facev2 == faceA->m_v2)
	{
		if(*vertex0 == -1)
			*vertex0 = facev2;
		else
		{
			*vertex1 = facev2;
			return;
		}
	}

}


///////////////////////////////////////////////////////////////////////////////////////////
// GetNextIndex()
//
// Returns vertex of the input face which is "next" in the input index list
//
inline int nvStripifier::GetNextIndex(const nvWordVec &indices, nvFaceInfo *face){
	
	int numIndices = indices.getSize();
	HK_ASSERT(numIndices >= 2);
	
	int v0  = indices[numIndices-2];
	int v1  = indices[numIndices-1];
	
	int fv0 = face->m_v0;
	int fv1 = face->m_v1;
	int fv2 = face->m_v2;
	
	if (fv0 != v0 && fv0 != v1){
		if ((fv1 != v0 && fv1 != v1) || (fv2 != v0 && fv2 != v1)){
			hkprintf("GetNextIndex: Triangle doesn't have all of its vertices\n");
			hkprintf("GetNextIndex: Duplicate triangle probably got us derailed\n");
		}
		return fv0;
	}
	if (fv1 != v0 && fv1 != v1){
		if ((fv0 != v0 && fv0 != v1) || (fv2 != v0 && fv2 != v1)){
			hkprintf("GetNextIndex: Triangle doesn't have all of its vertices\n");
			hkprintf("GetNextIndex: Duplicate triangle probably got us derailed\n");
		}
		return fv1;
	}
	if (fv2 != v0 && fv2 != v1){
		if ((fv0 != v0 && fv0 != v1) || (fv1 != v0 && fv1 != v1)){
			hkprintf("GetNextIndex: Triangle doesn't have all of its vertices\n");
			hkprintf("GetNextIndex: Duplicate triangle probably got us derailed\n");
		}
		return fv2;
	}
	
	// shouldn't get here, but let's try and fail gracefully
	if( (fv0 == fv1) || (fv0 == fv2) )
		return fv0;
	else if( (fv1 == fv0) || (fv1 == fv2) )
		return fv1;
	else if( (fv2 == fv0) || (fv2 == fv1) )
		return fv2;
	else
		return -1;
}


///////////////////////////////////////////////////////////////////////////////////////////
// IsMarked()
//
// If either the faceInfo has a real strip index because it is
// already assign to a committed strip OR it is assigned in an
// experiment and the experiment index is the one we are building
// for, then it is marked and unavailable
inline bool nvStripInfo::IsMarked(nvFaceInfo *faceInfo){
	return (faceInfo->m_stripId >= 0) || (IsExperiment() && faceInfo->m_experimentId == m_experimentId);
}


///////////////////////////////////////////////////////////////////////////////////////////
// MarkTriangle()
//
// Marks the face with the current strip ID
//
inline void nvStripInfo::MarkTriangle(nvFaceInfo *faceInfo){
	HK_ASSERT(!IsMarked(faceInfo));
	if (IsExperiment()){
		faceInfo->m_experimentId = m_experimentId;
		faceInfo->m_testStripId  = m_stripId;
    }
	else{
		HK_ASSERT(faceInfo->m_stripId == -1);
		faceInfo->m_experimentId = -1;
		faceInfo->m_stripId      = m_stripId;
	}
}


bool nvStripInfo::Unique(nvFaceInfoVec& faceVec, nvFaceInfo* face)
{
	bool bv0, bv1, bv2; //bools to indicate whether a vertex is in the faceVec or not
	bv0 = bv1 = bv2 = false;

	for(int i = 0; i < faceVec.getSize(); i++)
	{
		if(!bv0)
		{
			if( (faceVec[i]->m_v0 == face->m_v0) || 
				(faceVec[i]->m_v1 == face->m_v0) ||
				(faceVec[i]->m_v2 == face->m_v0) )
				bv0 = true;
		}

		if(!bv1)
		{
			if( (faceVec[i]->m_v0 == face->m_v1) || 
				(faceVec[i]->m_v1 == face->m_v1) ||
				(faceVec[i]->m_v2 == face->m_v1) )
				bv1 = true;
		}

		if(!bv2)
		{
			if( (faceVec[i]->m_v0 == face->m_v2) || 
				(faceVec[i]->m_v1 == face->m_v2) ||
				(faceVec[i]->m_v2 == face->m_v2) )
				bv2 = true;
		}

		//the face is not unique, all it's vertices exist in the face vector
		if(bv0 && bv1 && bv2)
			return false;
	}
	
	//if we get out here, it's unique
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////
// Build()
//
// Builds a strip forward as far as we can go, then builds backwards, and joins the two lists
//
void nvStripInfo::Build(nvEdgeInfoVec &edgeInfos, nvFaceInfoVec &faceInfos)
{
	// used in building the strips forward and backward
	nvWordVec scratchIndices;
	
	// build forward... start with the initial face
	nvFaceInfoVec forwardFaces, backwardFaces;
	forwardFaces.pushBack(m_startInfo.m_startFace);

	MarkTriangle(m_startInfo.m_startFace);
	
	int v0 = (m_startInfo.m_toV1 ? m_startInfo.m_startEdge->m_v0 : m_startInfo.m_startEdge->m_v1);
	int v1 = (m_startInfo.m_toV1 ? m_startInfo.m_startEdge->m_v1 : m_startInfo.m_startEdge->m_v0);
	
	// easiest way to get v2 is to use this function which requires the
	// other indices to already be in the list.
	scratchIndices.pushBack(v0);
	scratchIndices.pushBack(v1);
	unsigned short v2 = nvStripifier::GetNextIndex(scratchIndices, m_startInfo.m_startFace);
	scratchIndices.pushBack(v2);

	//
	// build the forward list
	//
	unsigned short nv0 = v1;
	unsigned short nv1 = v2;

	nvFaceInfo *nextFace = nvStripifier::FindOtherFace(edgeInfos, nv0, nv1, m_startInfo.m_startFace);
	while (nextFace != HK_NULL && !IsMarked(nextFace))
	{
		//check to see if this next face is going to cause us to die soon
		unsigned short testnv0 = nv1;
		unsigned short testnv1 = nvStripifier::GetNextIndex(scratchIndices, nextFace);
		
		nvFaceInfo* nextNextFace = nvStripifier::FindOtherFace(edgeInfos, testnv0, testnv1, nextFace);

		if( (nextNextFace == HK_NULL) || (IsMarked(nextNextFace)) )
		{
			//uh, oh, we're following a dead end, try swapping
			nvFaceInfo* testNextFace = nvStripifier::FindOtherFace(edgeInfos, nv0, testnv1, nextFace);

			if( ((testNextFace != HK_NULL) && !IsMarked(testNextFace)) )
			{
				//we only swap if it buys us something
				
				//add a "fake" degenerate face
				nvFaceInfo* tempFace = new nvFaceInfo(nv0, nv1, nv0);
				
				m_degenerateFaces.pushBack( tempFace );

				forwardFaces.pushBack(tempFace);
				MarkTriangle(tempFace);

				scratchIndices.pushBack(nv0);
				testnv0 = nv0;

				++m_numDegenerates;
			}

		}

		// add this to the strip
		forwardFaces.pushBack(nextFace);

		MarkTriangle(nextFace);
		
		// add the index
		//nv0 = nv1;
		//nv1 = nvStripifier::GetNextIndex(scratchIndices, nextFace);
		scratchIndices.pushBack(testnv1);
		
		// and get the next face
		nv0 = testnv0;
		nv1 = testnv1;

		nextFace = nvStripifier::FindOtherFace(edgeInfos, nv0, nv1, nextFace);
	
	}
	
	// tempAllFaces is going to be forwardFaces + backwardFaces
	// it's used for Unique()
	nvFaceInfoVec tempAllFaces;
	for(int i = 0; i < forwardFaces.getSize(); i++)
		tempAllFaces.pushBack(forwardFaces[i]);

	//
	// reset the indices for building the strip backwards and do so
	//
	scratchIndices.setSize(0);
	scratchIndices.pushBack(v2);
	scratchIndices.pushBack(v1);
	scratchIndices.pushBack(v0);
	nv0 = v1;
	nv1 = v0;
	nextFace = nvStripifier::FindOtherFace(edgeInfos, nv0, nv1, m_startInfo.m_startFace);
	while (nextFace != HK_NULL && !IsMarked(nextFace))
	{
		//this tests to see if a face is "unique", meaning that its vertices aren't already in the list
		// so, strips which "wrap-around" are not allowed
		if(!Unique(tempAllFaces, nextFace))
			break;

		//check to see if this next face is going to cause us to die soon
		int testnv0 = nv1;
		int testnv1 = nvStripifier::GetNextIndex(scratchIndices, nextFace);
		
		nvFaceInfo* nextNextFace = nvStripifier::FindOtherFace(edgeInfos, testnv0, testnv1, nextFace);

		if( (nextNextFace == HK_NULL) || (IsMarked(nextNextFace)) )
		{
			//uh, oh, we're following a dead end, try swapping
			nvFaceInfo* testNextFace = nvStripifier::FindOtherFace(edgeInfos, nv0, testnv1, nextFace);
			if( ((testNextFace != HK_NULL) && !IsMarked(testNextFace)) )
			{
				//we only swap if it buys us something
				
				//add a "fake" degenerate face
				nvFaceInfo* tempFace = new nvFaceInfo(nv0, nv1, nv0);
				
				m_degenerateFaces.pushBack( tempFace );

				backwardFaces.pushBack(tempFace);
				MarkTriangle(tempFace);
				scratchIndices.pushBack(nv0);
				testnv0 = nv0;

				++m_numDegenerates;
			}
				
		}

		// add this to the strip
		backwardFaces.pushBack(nextFace);
		
		//this is just so Unique() will work
		tempAllFaces.pushBack(nextFace);

		MarkTriangle(nextFace);
		
		// add the index
		//nv0 = nv1;
		//nv1 = nvStripifier::GetNextIndex(scratchIndices, nextFace);
		scratchIndices.pushBack(testnv1);
		
		// and get the next face
		nv0 = testnv0;
		nv1 = testnv1;
		nextFace = nvStripifier::FindOtherFace(edgeInfos, nv0, nv1, nextFace);
	}
	
	// Combine the forward and backwards stripification lists and put into our own face vector
	Combine(forwardFaces, backwardFaces);
}


///////////////////////////////////////////////////////////////////////////////////////////
// Combine()
//
// Combines the two input face vectors and puts the result into m_faces
//
void nvStripInfo::Combine(const nvFaceInfoVec &forward, const nvFaceInfoVec &backward){
	
	// add backward faces
	int numFaces = backward.getSize();
	int i;
	for (i = numFaces - 1; i >= 0; i--)
		m_faces.pushBack(backward[i]);
	
	// add forward faces
	numFaces = forward.getSize();
	for (i = 0; i < numFaces; i++)
		m_faces.pushBack(forward[i]);
}


///////////////////////////////////////////////////////////////////////////////////////////
// SharesEdge()
//
// Returns true if the input face and the current strip share an edge
//
bool nvStripInfo::SharesEdge(const nvFaceInfo* faceInfo, nvEdgeInfoVec &edgeInfos)
{
	//check v0->v1 edge
	nvEdgeInfo* currEdge = nvStripifier::FindEdgeInfo(edgeInfos, faceInfo->m_v0, faceInfo->m_v1);
	
	if(IsInStrip(currEdge->m_face0) || IsInStrip(currEdge->m_face1))
		return true;
	
	//check v1->v2 edge
	currEdge = nvStripifier::FindEdgeInfo(edgeInfos, faceInfo->m_v1, faceInfo->m_v2);
	
	if(IsInStrip(currEdge->m_face0) || IsInStrip(currEdge->m_face1))
		return true;
	
	//check v2->v0 edge
	currEdge = nvStripifier::FindEdgeInfo(edgeInfos, faceInfo->m_v2, faceInfo->m_v0);
	
	if(IsInStrip(currEdge->m_face0) || IsInStrip(currEdge->m_face1))
		return true;
	
	return false;
	
}


///////////////////////////////////////////////////////////////////////////////////////////
// CommitStrips()
//
// "Commits" the input strips by setting their m_experimentId to -1 and adding to the allStrips
//  vector
//
void nvStripifier::CommitStrips(nvStripInfoVec &allStrips, const nvStripInfoVec &strips)
{	
	// Iterate through strips
	int numStrips = strips.getSize();
	for (int i = 0; i < numStrips; i++){

		// Tell the strip that it is now real
		nvStripInfo *strip = strips[i];
		strip->m_experimentId = -1;
		
		// add to the list of real strips
		allStrips.pushBack(strip);
		
		// Iterate through the faces of the strip
		// Tell the faces of the strip that they belong to a real strip now
		const nvFaceInfoVec &faces = strips[i]->m_faces;
		int numFaces = faces.getSize();

		for (int j = 0; j < numFaces; j++)
		{
			strip->MarkTriangle(faces[j]);
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////
// FindTraversal()
//
// Finds the next face to start the next strip on.
//
bool nvStripifier::FindTraversal(nvFaceInfoVec &faceInfos,
								 nvEdgeInfoVec    &edgeInfos,
								 nvStripInfo      *strip,
								 nvStripStartInfo &startInfo){
	
	// if the strip was v0->v1 on the edge, then v1 will be a vertex in the next edge.
	int v = (strip->m_startInfo.m_toV1 ? strip->m_startInfo.m_startEdge->m_v1 : strip->m_startInfo.m_startEdge->m_v0);
	
	nvFaceInfo *untouchedFace = HK_NULL;
	nvEdgeInfo *edgeIter      = edgeInfos[v];
	while (edgeIter != HK_NULL){
		nvFaceInfo *face0 = edgeIter->m_face0;
		nvFaceInfo *face1 = edgeIter->m_face1;
		if ((face0 != HK_NULL && !strip->IsInStrip(face0)) && face1 != HK_NULL && !strip->IsMarked(face1))
		{
			untouchedFace = face1;
			break;
		}
		if ((face1 != HK_NULL && !strip->IsInStrip(face1)) && face0 != HK_NULL && !strip->IsMarked(face0)){
			untouchedFace = face0;
			break;
		}
		
		// find the next edgeIter
		edgeIter = (edgeIter->m_v0 == v ? edgeIter->m_nextV0 : edgeIter->m_nextV1);
	}
	
	startInfo.m_startFace = untouchedFace;
	startInfo.m_startEdge = edgeIter;
	if (edgeIter != HK_NULL)
	{
		if(strip->SharesEdge(startInfo.m_startFace, edgeInfos))
			startInfo.m_toV1 = (edgeIter->m_v0 == v);  //note! used to be m_v1
		else
			startInfo.m_toV1 = (edgeIter->m_v1 == v);
	}
	return (startInfo.m_startFace != HK_NULL);
}


////////////////////////////////////////////////////////////////////////////////////////
// RemoveSmallStrips()
//
// allStrips is the whole strip vector...all small strips will be deleted from this list, to avoid leaking mem
// allBigStrips is an out parameter which will contain all strips above minStripLength
// faceList is an out parameter which will contain all faces which were removed from the striplist
//
void nvStripifier::RemoveSmallStrips(nvStripInfoVec& allStrips, nvStripInfoVec& allBigStrips, nvFaceInfoVec& faceList)
{
	faceList.clear();
	allBigStrips.clear();  //make sure these are empty
	nvFaceInfoVec tempFaceList;
	
	for(int i = 0; i < allStrips.getSize(); i++)
	{
		if(allStrips[i]->m_faces.getSize() < minStripLength)
		{
			//strip is too small, add faces to faceList
			for(int j = 0; j < allStrips[i]->m_faces.getSize(); j++)
				tempFaceList.pushBack(allStrips[i]->m_faces[j]);
			
			//and free memory
			delete allStrips[i];
		}
		else
		{
			allBigStrips.pushBack(allStrips[i]);
		}
	}
	
	bool *bVisitedList = hkAllocate<bool>(tempFaceList.getSize(), HK_MEMORY_CLASS_DISPLAY);
	hkString::memSet(bVisitedList, 0, tempFaceList.getSize()*sizeof(bool));
	
	nvVertexCache* vcache = new nvVertexCache(cacheSize);
	
	int bestNumHits = -1;
	int numHits;
	int bestIndex = -1;
	
	while(1)
	{
		bestNumHits = -1;
		
		//find best face to add next, given the current cache
		for(int i = 0; i < tempFaceList.getSize(); i++)
		{
			if(bVisitedList[i])
				continue;
			
			numHits = CalcNumHitsFace(vcache, tempFaceList[i]);
			if(numHits > bestNumHits)
			{
				bestNumHits = numHits;
				bestIndex = i;
			}
		}
		
		if(bestNumHits == -1.0f)
			break;
		bVisitedList[bestIndex] = true;
		UpdateCacheFace(vcache, tempFaceList[bestIndex]);
		faceList.pushBack(tempFaceList[bestIndex]);
	}
	
	delete vcache;
	hkDeallocate<bool>(bVisitedList);
}


///////////////////////////////////////////////////////////////////////////////////////////
// NextIsCW()
//
// Returns true if the next face should be ordered in CW fashion
//
bool nvStripifier::NextIsCW(const int numIndices)
{
	return ((numIndices % 2) == 0);
}


///////////////////////////////////////////////////////////////////////////////////////////
// IsCW()
//
// Returns true if the face is ordered in CW fashion
//
bool nvStripifier::IsCW(nvFaceInfo *faceInfo, int v0, int v1)
{
	if (faceInfo->m_v0 == v0)
		return (faceInfo->m_v1 == v1);
	else if (faceInfo->m_v1 == v0)
		return (faceInfo->m_v2 == v1);
	else 
		return (faceInfo->m_v0 == v1);
}

bool nvStripifier::FaceContainsIndex(const nvFaceInfo& face, int index)
{
	return ( (face.m_v0 == index) || (face.m_v1 == index) || (face.m_v2 == index) );
}

bool nvStripifier::IsMoneyFace(const nvFaceInfo& face)
{
	if(FaceContainsIndex(face, 800) &&
	   FaceContainsIndex(face, 812) &&
	   FaceContainsIndex(face, 731))
	   return true;

	return false;
}
				
////////////////////////////////////////////////////////////////////////////////////////
// CreateStrips()
//
// Generates actual strips from the list-in-strip-order.
//
void nvStripifier::CreateStrips(const nvStripInfoVec& allStrips, nvIntVec& stripIndices, 
								const bool bStitchStrips, int& numSeparateStrips)
{
	HK_ASSERT(numSeparateStrips == 0);

	nvFaceInfo tLastFace(0, 0, 0);
	nvFaceInfo tPrevStripLastFace(0, 0, 0);
	int nStripCount = allStrips.getSize();
	HK_ASSERT(nStripCount > 0);

	//we infer the cw/ccw ordering depending on the number of indices
	//this is screwed up by the fact that we insert -1s to denote changing strips
	//this is to account for that
	int accountForNegatives = 0;

	for (int i = 0; i < nStripCount; i++)
	{
		nvStripInfo *strip = allStrips[i];
		int nStripFaceCount = strip->m_faces.getSize();
		HK_ASSERT(nStripFaceCount > 0);

		// Handle the first face in the strip
		{
			nvFaceInfo tFirstFace(strip->m_faces[0]->m_v0, strip->m_faces[0]->m_v1, strip->m_faces[0]->m_v2);

			// If there is a second face, reorder vertices such that the
			// unique vertex is first
			if (nStripFaceCount > 1)
			{
				int nUnique = nvStripifier::GetUniqueVertexInB(strip->m_faces[1], &tFirstFace);
				if (nUnique == tFirstFace.m_v1)
				{
					SWAP(tFirstFace.m_v0, tFirstFace.m_v1);
				}
				else if (nUnique == tFirstFace.m_v2)
				{
					SWAP(tFirstFace.m_v0, tFirstFace.m_v2);
				}

				// If there is a third face, reorder vertices such that the
				// shared vertex is last
				if (nStripFaceCount > 2)
				{
					if(IsDegenerate(strip->m_faces[1]))
					{
						int pivot = strip->m_faces[1]->m_v1;
						if(tFirstFace.m_v1 == pivot)
						{
							SWAP(tFirstFace.m_v1, tFirstFace.m_v2);
						}
					}
					else
					{
						int nShared0, nShared1;
						GetSharedVertices(strip->m_faces[2], &tFirstFace, &nShared0, &nShared1);
						if ( (nShared0 == tFirstFace.m_v1) && (nShared1 == -1) )
						{
							SWAP(tFirstFace.m_v1, tFirstFace.m_v2);
						}
					}
				}
			}

			if( (i == 0) || !bStitchStrips)
			{
				if(!IsCW(strip->m_faces[0], tFirstFace.m_v0, tFirstFace.m_v1))
					stripIndices.pushBack(tFirstFace.m_v0);
			}
			else
			{
				// Double tap the first in the new strip
				stripIndices.pushBack(tFirstFace.m_v0);
	
				// Check CW/CCW ordering
				if (NextIsCW(stripIndices.getSize() - accountForNegatives) != IsCW(strip->m_faces[0], tFirstFace.m_v0, tFirstFace.m_v1))
				{
					stripIndices.pushBack(tFirstFace.m_v0);
				}
			}

			stripIndices.pushBack(tFirstFace.m_v0);
			stripIndices.pushBack(tFirstFace.m_v1);
			stripIndices.pushBack(tFirstFace.m_v2);

			// Update last face info
			tLastFace = tFirstFace;
		}

		for (int j = 1; j < nStripFaceCount; j++)
		{
			int nUnique = GetUniqueVertexInB(&tLastFace, strip->m_faces[j]);
			if (nUnique != -1)
			{
				stripIndices.pushBack(nUnique);

				// Update last face info
				tLastFace.m_v0 = tLastFace.m_v1;
				tLastFace.m_v1 = tLastFace.m_v2;
				tLastFace.m_v2 = nUnique;
			}
			else
			{
				//we've hit a degenerate
				stripIndices.pushBack(strip->m_faces[j]->m_v2);
				tLastFace.m_v0 = strip->m_faces[j]->m_v0;//tLastFace.m_v1;
				tLastFace.m_v1 = strip->m_faces[j]->m_v1;//tLastFace.m_v2;
				tLastFace.m_v2 = strip->m_faces[j]->m_v2;//tLastFace.m_v1;

			}
		}

		// Double tap between strips.
		if(bStitchStrips)
		{
			if(i != nStripCount - 1)
				stripIndices.pushBack(tLastFace.m_v2);
		}
		else
		{
			//-1 index indicates next strip
			stripIndices.pushBack(-1);
			accountForNegatives++;
			numSeparateStrips++;
		}

		// Update last face info
		tLastFace.m_v0 = tLastFace.m_v1;
		tLastFace.m_v1 = tLastFace.m_v2;
		tLastFace.m_v2 = tLastFace.m_v2;
	}
	
	if(bStitchStrips)
		numSeparateStrips = 1;
}


///////////////////////////////////////////////////////////////////////////////////////////
// Stripify()
//
//
// in_indices are the input indices of the mesh to stripify
// in_cacheSize is the target cache size 
//
void nvStripifier::Stripify(const nvWordVec &in_indices, const int in_cacheSize, 
							const int in_minStripLength, const unsigned short maxIndex, 
							nvStripInfoVec &outStrips, nvFaceInfoVec& outFaceList, nvFaceInfoVec& discardedDegenerates)
{
	meshJump = 0.0f;
	bFirstTimeResetPoint = true; //used in FindGoodResetPoint()

	//the number of times to run the experiments
	int numSamples = 10;
	
	//the cache size, clamped to one
	cacheSize = nv_MAX(1, in_cacheSize - CACHE_INEFFICIENCY);
	
	minStripLength = in_minStripLength;  //this is the strip size threshold below which we dump the strip into a list
	
	indices = in_indices; 
	
	// build the stripification info
	nvEdgeInfoVec allEdgeInfos;
	nvFaceInfoVec allFaceInfos;
	
	BuildStripifyInfo(allFaceInfos, allEdgeInfos, maxIndex);
	
	nvStripInfoVec allStrips;

	// stripify
	FindAllStrips(allStrips, allFaceInfos, allEdgeInfos, numSamples);
	
	//split up the strips into cache friendly pieces, optimize them, then dump these into outStrips
	SplitUpStripsAndOptimize(allStrips, outStrips, allEdgeInfos, outFaceList, discardedDegenerates);

	//clean up
	int i;
	for(i = 0; i < allStrips.getSize(); i++)
	{
		delete allStrips[i];
	}
	
	for (i = 0; i < allEdgeInfos.getSize(); i++)
	{
		nvEdgeInfo *info = allEdgeInfos[i];
		while (info != HK_NULL)
		{
			nvEdgeInfo *next = (info->m_v0 == i ? info->m_nextV0 : info->m_nextV1);
			info->Unref();
			info = next;
		}
	}
}


bool nvStripifier::IsDegenerate(const nvFaceInfo* face)
{
	if(face->m_v0 == face->m_v1)
		return true;
	else if(face->m_v0 == face->m_v2)
		return true;
	else if(face->m_v1 == face->m_v2)
		return true;
	else
		return false;
}

bool nvStripifier::IsDegenerate(const unsigned short v0, const unsigned short v1, const unsigned short v2)
{
	if(v0 == v1)
		return true;
	else if(v0 == v2)
		return true;
	else if(v1 == v2)
		return true;
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////////////////
// SplitUpStripsAndOptimize()
//
// Splits the input vector of strips (allBigStrips) into smaller, cache friendly pieces, then
//  reorders these pieces to maximize cache hits
// The final strips are output through outStrips
//
void nvStripifier::SplitUpStripsAndOptimize(nvStripInfoVec &allStrips, nvStripInfoVec &outStrips,
                                            nvEdgeInfoVec& edgeInfos, nvFaceInfoVec& outFaceList, nvFaceInfoVec& discardedDegenerates)
{
	int threshold = cacheSize;
	nvStripInfoVec tempStrips;
	
	//split up strips into threshold-sized pieces
	int i;
	for(i = 0; i < allStrips.getSize(); i++)
	{
		nvStripInfo* currentStrip;
		nvStripStartInfo startInfo(HK_NULL, HK_NULL, false);
	
		int actualStripSize = 0;
		for(int j = 0; j < allStrips[i]->m_faces.getSize(); ++j)
		{
			if( !IsDegenerate(allStrips[i]->m_faces[j]) )
				actualStripSize++;
		}
		
		if(actualStripSize /*allStrips[i]->m_faces.getSize()*/ > threshold)
		{
			
			int numTimes    = actualStripSize /*allStrips[i]->m_faces.getSize()*/ / threshold;
			int numLeftover = actualStripSize /*allStrips[i]->m_faces.getSize()*/ % threshold;

			int degenerateCount = 0;
			int j;
			for(j = 0; j < numTimes; j++)
			{
				currentStrip = new nvStripInfo(startInfo, 0, -1);
				
				int faceCtr = j*threshold + degenerateCount;
				bool bFirstTime = true;
				while(faceCtr < threshold+(j*threshold)+degenerateCount)
				{
					if(IsDegenerate(allStrips[i]->m_faces[faceCtr]))
					{
						degenerateCount++;
						
						//last time or first time through, no need for a degenerate
						if( (((faceCtr + 1) != threshold+(j*threshold)+degenerateCount) ||
							 ((j == numTimes - 1) && (numLeftover < 4) && (numLeftover > 0))) && 
							 !bFirstTime)
						{
							currentStrip->m_faces.pushBack(allStrips[i]->m_faces[faceCtr++]);
						}
						else
						{
							///XXX: Mem leak fix. Have to keep face pointer to delete it later.
							discardedDegenerates.pushBack(allStrips[i]->m_faces[faceCtr]);

							++faceCtr;
						}
					}
					else
					{
						currentStrip->m_faces.pushBack(allStrips[i]->m_faces[faceCtr++]);
						bFirstTime = false;
					}
				}
				/*
				for(int faceCtr = j*threshold; faceCtr < threshold+(j*threshold); faceCtr++)
				{
					currentStrip->m_faces.pushBack(allStrips[i]->m_faces[faceCtr]);
				}
				*/
				///*
				if(j == numTimes - 1) //last time through
				{
					if( (numLeftover < 4) && (numLeftover > 0) ) //way too small
					{
						//just add to last strip
						int ctr = 0;
						while(ctr < numLeftover)
						{
							if(!IsDegenerate(allStrips[i]->m_faces[faceCtr]))
							{	
								currentStrip->m_faces.pushBack(allStrips[i]->m_faces[faceCtr++]);
								++ctr;
							}
							else
							{
								currentStrip->m_faces.pushBack(allStrips[i]->m_faces[faceCtr++]);
								++degenerateCount;
							}
						}
						numLeftover = 0;
					}
				}
				//*/
				tempStrips.pushBack(currentStrip);
			}
			
			int leftOff = j * threshold + degenerateCount;
			
			if(numLeftover != 0)
			{
				currentStrip = new nvStripInfo(startInfo, 0, -1);   
				
				int ctr = 0;
				bool bFirstTime = true;
				while(ctr < numLeftover)
				{
					if( !IsDegenerate(allStrips[i]->m_faces[leftOff]) )
					{
						ctr++;
						bFirstTime = false;
						currentStrip->m_faces.pushBack(allStrips[i]->m_faces[leftOff++]);
					}
					else if(!bFirstTime)
						currentStrip->m_faces.pushBack(allStrips[i]->m_faces[leftOff++]);
					else
					{
						///XXX: Mem leak fix. Have to keep face pointer to delete it later.
						discardedDegenerates.pushBack(allStrips[i]->m_faces[leftOff]);

						leftOff++;
					}
				}
				/*
				for(int k = 0; k < numLeftover; k++)
				{
					currentStrip->m_faces.pushBack(allStrips[i]->m_faces[leftOff++]);
				}
				*/
				
				tempStrips.pushBack(currentStrip);
			}
		}
		else
		{
			//we're not just doing a tempStrips.pushBack(allBigStrips[i]) because
			// this way we can delete allBigStrips later to free the memory
			currentStrip = new nvStripInfo(startInfo, 0, -1);
			
			for(int j = 0; j < allStrips[i]->m_faces.getSize(); j++)
				currentStrip->m_faces.pushBack(allStrips[i]->m_faces[j]);
			
			tempStrips.pushBack(currentStrip);
		}
	}

	//add small strips to face list
	nvStripInfoVec tempStrips2;
	RemoveSmallStrips(tempStrips, tempStrips2, outFaceList);
	
	outStrips.clear();
	//screw optimization for now
//	for(i = 0; i < tempStrips.getSize(); ++i)
//    outStrips.pushBack(tempStrips[i]);
	
	if(tempStrips2.getSize() != 0)
	{
		//Optimize for the vertex cache
		nvVertexCache* vcache = new nvVertexCache(cacheSize);
		
		float bestNumHits = -1.0f;
		float numHits;
		int bestIndex = -1;
		bool done = false;
		
		int firstIndex = 0;
		float minCost = 10000.0f;
		
		for(i = 0; i < tempStrips2.getSize(); i++)
		{
			int numNeighbors = 0;
			
			//find strip with least number of neighbors per face
			for(int j = 0; j < tempStrips2[i]->m_faces.getSize(); j++)
			{
				numNeighbors += NumNeighbors(tempStrips2[i]->m_faces[j], edgeInfos);
			}
			
			float currCost = (float)numNeighbors / (float)tempStrips2[i]->m_faces.getSize();
			if(currCost < minCost)
			{
				minCost = currCost;
				firstIndex = i;
			}
		}
		
		UpdateCacheStrip(vcache, tempStrips2[firstIndex]);
		outStrips.pushBack(tempStrips2[firstIndex]);
		
		tempStrips2[firstIndex]->visited = true;
		
		bool bWantsCW = (tempStrips2[firstIndex]->m_faces.getSize() % 2) == 0;

		//this n^2 algo is what slows down stripification so much....
		// needs to be improved
		while(1)
		{
			bestNumHits = -1.0f;
			
			//find best strip to add next, given the current cache
			for(int i = 0; i < tempStrips2.getSize(); i++)
			{
				if(tempStrips2[i]->visited)
					continue;

				numHits = CalcNumHitsStrip(vcache, tempStrips2[i]);
				if(numHits > bestNumHits)
				{
					bestNumHits = numHits;
					bestIndex = i;
				}
				else if(numHits >= bestNumHits)
				{
					//check previous strip to see if this one requires it to switch polarity
					nvStripInfo *strip = tempStrips2[i];
					int nStripFaceCount = strip->m_faces.getSize();
					
					nvFaceInfo tFirstFace(strip->m_faces[0]->m_v0, strip->m_faces[0]->m_v1, strip->m_faces[0]->m_v2);
					
					// If there is a second face, reorder vertices such that the
					// unique vertex is first
					if (nStripFaceCount > 1)
					{
						int nUnique = nvStripifier::GetUniqueVertexInB(strip->m_faces[1], &tFirstFace);
						if (nUnique == tFirstFace.m_v1)
						{
							SWAP(tFirstFace.m_v0, tFirstFace.m_v1);
						}
						else if (nUnique == tFirstFace.m_v2)
						{
							SWAP(tFirstFace.m_v0, tFirstFace.m_v2);
						}
						
						// If there is a third face, reorder vertices such that the
						// shared vertex is last
						if (nStripFaceCount > 2)
						{
							int nShared0, nShared1;
							GetSharedVertices(strip->m_faces[2], &tFirstFace, &nShared0, &nShared1);
							if ( (nShared0 == tFirstFace.m_v1) && (nShared1 == -1) )
							{
								SWAP(tFirstFace.m_v1, tFirstFace.m_v2);
							}
						}
					}
						
					// Check CW/CCW ordering
					if (bWantsCW == IsCW(strip->m_faces[0], tFirstFace.m_v0, tFirstFace.m_v1))
					{
						//I like this one!
						bestIndex = i;
					}
				}
			}
			
			if(bestNumHits == -1.0f)
				break;
			tempStrips2[bestIndex]->visited = true;
			UpdateCacheStrip(vcache, tempStrips2[bestIndex]);
			outStrips.pushBack(tempStrips2[bestIndex]);
			bWantsCW = (tempStrips2[bestIndex]->m_faces.getSize() % 2 == 0) ? bWantsCW : !bWantsCW;
		}
		
		delete vcache;	
	}
}


///////////////////////////////////////////////////////////////////////////////////////////
// UpdateCacheStrip()
//
// Updates the input vertex cache with this strip's vertices
//
void nvStripifier::UpdateCacheStrip(nvVertexCache* vcache, nvStripInfo* strip)
{
	for(int i = 0; i < strip->m_faces.getSize(); ++i)
	{
		if(!vcache->InCache(strip->m_faces[i]->m_v0))
			vcache->AddEntry(strip->m_faces[i]->m_v0);
		
		if(!vcache->InCache(strip->m_faces[i]->m_v1))
			vcache->AddEntry(strip->m_faces[i]->m_v1);
		
		if(!vcache->InCache(strip->m_faces[i]->m_v2))
			vcache->AddEntry(strip->m_faces[i]->m_v2);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
// UpdateCacheFace()
//
// Updates the input vertex cache with this face's vertices
//
void nvStripifier::UpdateCacheFace(nvVertexCache* vcache, nvFaceInfo* face)
{
	if(!vcache->InCache(face->m_v0))
		vcache->AddEntry(face->m_v0);
		
	if(!vcache->InCache(face->m_v1))
		vcache->AddEntry(face->m_v1);
		
	if(!vcache->InCache(face->m_v2))
		vcache->AddEntry(face->m_v2);
}


///////////////////////////////////////////////////////////////////////////////////////////
// CalcNumHitsStrip()
//
// returns the number of cache hits per face in the strip
//
float nvStripifier::CalcNumHitsStrip(nvVertexCache* vcache, nvStripInfo* strip)
{
	int numHits = 0;
	int numFaces = 0;
	
	for(int i = 0; i < strip->m_faces.getSize(); i++)
	{
		if(vcache->InCache(strip->m_faces[i]->m_v0))
			++numHits;
		
		if(vcache->InCache(strip->m_faces[i]->m_v1))
			++numHits;
		
		if(vcache->InCache(strip->m_faces[i]->m_v2))
			++numHits;
		
		numFaces++;
	}
	
	return ((float)numHits / (float)numFaces);
}


///////////////////////////////////////////////////////////////////////////////////////////
// CalcNumHitsFace()
//
// returns the number of cache hits in the face
//
int nvStripifier::CalcNumHitsFace(nvVertexCache* vcache, nvFaceInfo* face)
{
	int numHits = 0;

	if(vcache->InCache(face->m_v0))
		numHits++;
		
	if(vcache->InCache(face->m_v1))
		numHits++;
		
	if(vcache->InCache(face->m_v2))
		numHits++;
		
	return numHits;
}


///////////////////////////////////////////////////////////////////////////////////////////
// NumNeighbors()
//
// Returns the number of neighbors that this face has
//
int nvStripifier::NumNeighbors(nvFaceInfo* face, nvEdgeInfoVec& edgeInfoVec)
{
	int numNeighbors = 0;
	
	if(FindOtherFace(edgeInfoVec, face->m_v0, face->m_v1, face) != HK_NULL)
	{
		numNeighbors++;
	}
	
	if(FindOtherFace(edgeInfoVec, face->m_v1, face->m_v2, face) != HK_NULL)
	{
		numNeighbors++;
	}
	
	if(FindOtherFace(edgeInfoVec, face->m_v2, face->m_v0, face) != HK_NULL)
	{
		numNeighbors++;
	}
	
	return numNeighbors;
}


///////////////////////////////////////////////////////////////////////////////////////////
// AvgStripSize()
//
// Finds the average strip size of the input vector of strips
//
float nvStripifier::AvgStripSize(const nvStripInfoVec &strips){
	int sizeAccum = 0;
	int numStrips = strips.getSize();
	for (int i = 0; i < numStrips; i++){
		nvStripInfo *strip = strips[i];
		sizeAccum += strip->m_faces.getSize();
		sizeAccum -= strip->m_numDegenerates;
	}
	return ((float)sizeAccum) / ((float)numStrips);
}

template <typename T>
class hkSet
{
    public:

        // bisect
        bool contains( const T& t)
        {
            unsigned lo = 0;
            unsigned hi = m_array.getSize();
            while(lo != hi)
            {
                unsigned mid = (lo + hi) >> 1;
                if(m_array[mid] > t)
                {
                    hi = mid;
                }
                else if(m_array[mid] < t)
                {
                    lo = mid;
                }
                else // equal
                {
                    return true;
                }
            }

            return false;
        }

        void insert(const T& t)
        {
            unsigned lo = 0;
            unsigned hi = m_array.getSize();
            while(lo != hi)
            {
                unsigned mid = (lo + hi) >> 1;
                if(m_array[mid] > t)
                {
                    hi = mid;
                }
                else if(m_array[mid] < t)
                {
                    lo = mid;
                }
                else // equal
                {
                    break;
                }
            }
            m_array.insertAt(lo, t);
        }

    private:
        hkArray<T> m_array;
};

///////////////////////////////////////////////////////////////////////////////////////////
// FindAllStrips()
//
// Does the stripification, puts output strips into vector allStrips
//
// Works by setting runnning a number of experiments in different areas of the mesh, and
//  accepting the one which results in the longest strips.  It then accepts this, and moves
//  on to a different area of the mesh.  We try to jump around the mesh some, to ensure that
//  large open spans of strips get generated.
//
void nvStripifier::FindAllStrips(nvStripInfoVec &allStrips,
								 nvFaceInfoVec &_allFaceInfos,
								 nvEdgeInfoVec &allEdgeInfos,
								 int numSamples)
{
	// the experiments
	int experimentId = 0;
	int stripId      = 0;
	bool done        = false;

	int loopCtr = 0;
	
	while (!done)
	{
		loopCtr++;
		
		//
		// PHASE 1: Set up numSamples * numEdges experiments
		//
		nvStripInfoVec *experiments = new nvStripInfoVec[ numSamples * 6 ]; // should use the [] havok alloc still

		int experimentIndex = 0;
		hkSet<nvFaceInfo*>  resetPoints;
		int i;
		for (i = 0; i < numSamples; i++)
		{
			
			// Try to find another good reset point.
			// If there are none to be found, we are done
			nvFaceInfo *nextFace = FindGoodResetPoint(_allFaceInfos, allEdgeInfos);
			if (nextFace == HK_NULL){
				done = true;
				break;
			}
			// If we have already evaluated starting at this face in this slew
			// of experiments, then skip going any further
			else if (resetPoints.contains(nextFace))
            {
				continue;
			}
			
			// trying it now...
			resetPoints.insert(nextFace);
			
			// otherwise, we shall now try experiments for starting on the 01,12, and 20 edges
			HK_ASSERT(nextFace->m_stripId < 0);
			
			// build the strip off of this face's 0-1 edge
			nvEdgeInfo *edge01 = FindEdgeInfo(allEdgeInfos, nextFace->m_v0, nextFace->m_v1);
			nvStripInfo *strip01 = new nvStripInfo(nvStripStartInfo(nextFace, edge01, true), stripId++, experimentId++);
			experiments[experimentIndex++].pushBack(strip01);
			
			// build the strip off of this face's 1-0 edge
			nvEdgeInfo *edge10 = FindEdgeInfo(allEdgeInfos, nextFace->m_v0, nextFace->m_v1);
			nvStripInfo *strip10 = new nvStripInfo(nvStripStartInfo(nextFace, edge10, false), stripId++, experimentId++);
			experiments[experimentIndex++].pushBack(strip10);
			
			// build the strip off of this face's 1-2 edge
			nvEdgeInfo *edge12 = FindEdgeInfo(allEdgeInfos, nextFace->m_v1, nextFace->m_v2);
			nvStripInfo *strip12 = new nvStripInfo(nvStripStartInfo(nextFace, edge12, true), stripId++, experimentId++);
			experiments[experimentIndex++].pushBack(strip12);
			
			// build the strip off of this face's 2-1 edge
			nvEdgeInfo *edge21 = FindEdgeInfo(allEdgeInfos, nextFace->m_v1, nextFace->m_v2);
			nvStripInfo *strip21 = new nvStripInfo(nvStripStartInfo(nextFace, edge21, false), stripId++, experimentId++);
			experiments[experimentIndex++].pushBack(strip21);
			
			// build the strip off of this face's 2-0 edge
			nvEdgeInfo *edge20 = FindEdgeInfo(allEdgeInfos, nextFace->m_v2, nextFace->m_v0);
			nvStripInfo *strip20 = new nvStripInfo(nvStripStartInfo(nextFace, edge20, true), stripId++, experimentId++);
			experiments[experimentIndex++].pushBack(strip20);
			
			// build the strip off of this face's 0-2 edge
			nvEdgeInfo *edge02 = FindEdgeInfo(allEdgeInfos, nextFace->m_v2, nextFace->m_v0);
			nvStripInfo *strip02 = new nvStripInfo(nvStripStartInfo(nextFace, edge02, false), stripId++, experimentId++);
			experiments[experimentIndex++].pushBack(strip02);
		}
		
		//
		// PHASE 2: Iterate through that we setup in the last phase
		// and really build each of the strips and strips that follow to see how
		// far we get
		//
		int numExperiments = experimentIndex;
		for (i = 0; i < numExperiments; i++)
        {
			
			// get the strip set
			
			// build the first strip of the list
			experiments[i][0]->Build(allEdgeInfos, _allFaceInfos);
			int experimentId = experiments[i][0]->m_experimentId;
			
			nvStripInfo *stripIter = experiments[i][0];
			nvStripStartInfo startInfo(HK_NULL, HK_NULL, false);
			while (FindTraversal(_allFaceInfos, allEdgeInfos, stripIter, startInfo)){
				
				// create the new strip info
				stripIter = new nvStripInfo(startInfo, stripId++, experimentId);
				
				// build the next strip
				stripIter->Build(allEdgeInfos, _allFaceInfos);
				
				// add it to the list
				experiments[i].pushBack(stripIter);
			}
		}
		
		//
		// Phase 3: Find the experiment that has the most promise
		//
		int bestIndex = 0;
		double bestValue = 0;
		for (i = 0; i < numExperiments; i++)
		{
			const float avgStripSizeWeight = 1.0f;
			const float numTrisWeight      = 0.0f;
			const float numStripsWeight    = 0.0f;
			float avgStripSize = AvgStripSize(experiments[i]);
			float numStrips    = (float) experiments[i].getSize();
			float value        = avgStripSize * avgStripSizeWeight + (numStrips * numStripsWeight);
			//float value = 1.f / numStrips;
			//float value = numStrips * avgStripSize;
				
			if (value > bestValue)
			{
				bestValue = value;
				bestIndex = i;
			}
		}
		
		//
		// Phase 4: commit the best experiment of the bunch
		//
		CommitStrips(allStrips, experiments[bestIndex]);
		
		// and destroy all of the others
		for (i = 0; i < numExperiments; i++)
		{
			if (i != bestIndex)
			{
				int numStrips = experiments[i].getSize();
				for (int j = 0; j < numStrips; j++)
				{
					//START FIX
					//XXX Mem leak fix:
					int dfs = experiments[i][j]->m_degenerateFaces.getSize();
					for (int k =0; k < dfs; ++k)
						delete experiments[i][j]->m_degenerateFaces[k];
					//END FIX

					delete experiments[i][j];
				}
			}
		}
		
		// delete the array that we used for all experiments
		delete [] experiments;
  }
}


/*NEVER CALLED (and uses a stl list ref)
///////////////////////////////////////////////////////////////////////////////////////////
// CountRemainingTris()
//
// This will count the number of triangles left in the
// strip list starting at iter and finishing up at end
//
int nvStripifier::CountRemainingTris(std::list<nvStripInfo*>::iterator iter,
											std::list<nvStripInfo*>::iterator  end){
	int count = 0;
	while (iter != end){
		count += (*iter)->m_faces.getSize();
		iter++;
	}
	return count;
}
*/

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
