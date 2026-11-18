/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef NV_TRISTRIP_OBJECTS_H
#define NV_TRISTRIP_OBJECTS_H

#include <hkgraphics/nvstripper/nvVertexCache.h>

/////////////////////////////////////////////////////////////////////////////////
//
// Types defined for stripification
//
/////////////////////////////////////////////////////////////////////////////////

struct nvVertex {

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_DISPLAY, nvVertex );

	float x, y, z;
	float nx, ny, nz;
};

typedef nvVertex nvVector;

struct nvFace {

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_DISPLAY, nvFace );

	int v1, v2, v3;
	float nx, ny, nz;
};


class nvFaceInfo {
public:

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_DISPLAY, nvFaceInfo );

	// vertex indices
	nvFaceInfo(int v0, int v1, int v2){
		m_v0 = v0; m_v1 = v1; m_v2 = v2;
		m_stripId      = -1;
		m_testStripId  = -1;
		m_experimentId = -1;
	}

	// data members are left public
	int   m_v0, m_v1, m_v2;
	int   m_stripId;      // real strip Id
	int   m_testStripId;  // strip Id in an experiment
	int   m_experimentId; // in what experiment was it given an experiment Id?
};

// nice and dumb edge class that points knows its
// indices, the two faces, and the next edge using
// the lesser of the indices
class nvEdgeInfo {
public:
	
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_DISPLAY, nvEdgeInfo );

	// constructor puts 1 ref on us
	nvEdgeInfo (int v0, int v1){
		m_v0       = v0;
		m_v1       = v1;
		m_face0    = HK_NULL;
		m_face1    = HK_NULL;
		m_nextV0   = HK_NULL;
		m_nextV1   = HK_NULL;
		
		// we will appear in 2 lists.  this is a good
		// way to make sure we delete it the second time
		// we hit it in the edge infos
		m_refCount = 2;    
		
	}
	
	// ref and unref
	void Unref () { if (--m_refCount == 0) delete this; }
	
	// data members are left public
	unsigned int m_refCount;
	nvFaceInfo  *m_face0, *m_face1;
	int          m_v0, m_v1;
	nvEdgeInfo  *m_nextV0, *m_nextV1;
};


// This class is a quick summary of parameters used
// to begin a triangle strip.  Some operations may
// want to create lists of such items, so they were
// pulled out into a class
class nvStripStartInfo {
public:

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_DISPLAY, nvStripStartInfo );

	nvStripStartInfo(nvFaceInfo *startFace, nvEdgeInfo *startEdge, bool toV1){
		m_startFace    = startFace;
		m_startEdge    = startEdge;
		m_toV1         = toV1;
	}
	nvFaceInfo    *m_startFace;
	nvEdgeInfo    *m_startEdge;
	bool           m_toV1;      
};


typedef hkArray<nvFaceInfo*>    nvFaceInfoVec;
typedef hkArray<nvEdgeInfo*>    nvEdgeInfoVec;

typedef hkArray<unsigned short> nvWordVec;
typedef hkArray<int>            nvIntVec;
typedef hkArray<nvVertex>       nvVertexVec;
typedef hkArray<nvFace>         nvFaceVec;

template<class T> 
inline void SWAP(T& first, T& second) 
{
	T temp = first;
	first = second;
	second = temp;
}

// This is a summary of a strip that has been built
class nvStripInfo {
public:
	
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_DISPLAY, nvStripInfo  );

	// A little information about the creation of the triangle strips
	nvStripInfo(const nvStripStartInfo &startInfo, int stripId, int experimentId = -1) :
	  m_startInfo(startInfo)
	{
		m_stripId      = stripId;
		m_experimentId = experimentId;
		visited = false;
		m_numDegenerates = 0;
	}
	  
	// This is an experiment if the experiment id is >= 0
	inline bool IsExperiment () const { return m_experimentId >= 0; }
	  
	inline bool IsInStrip (const nvFaceInfo *faceInfo) const 
	{
		if(faceInfo == HK_NULL)
			return false;
		  
		return (m_experimentId >= 0 ? faceInfo->m_testStripId == m_stripId : faceInfo->m_stripId == m_stripId);
	}
	  
	bool SharesEdge(const nvFaceInfo* faceInfo, nvEdgeInfoVec &edgeInfos);
	  
	// take the given forward and backward strips and combine them together
	void Combine(const nvFaceInfoVec &forward, const nvFaceInfoVec &backward);
	  
	//returns true if the face is "unique", i.e. has a vertex which doesn't exist in the faceVec
	bool Unique(nvFaceInfoVec& faceVec, nvFaceInfo* face);
	  
	// mark the triangle as taken by this strip
	bool IsMarked    (nvFaceInfo *faceInfo);
	void MarkTriangle(nvFaceInfo *faceInfo);
	  
	// build the strip
	void Build(nvEdgeInfoVec &edgeInfos, nvFaceInfoVec &faceInfos);
	  
	// public data members
	nvStripStartInfo m_startInfo;
	nvFaceInfoVec    m_faces;
	nvFaceInfoVec	 m_degenerateFaces;
	int              m_stripId;
	int              m_experimentId;
	  
	bool visited;

	int m_numDegenerates;
};

typedef hkArray<nvStripInfo*>    nvStripInfoVec;


//The actual stripifier
class nvStripifier {
public:
	
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_DISPLAY, nvStripifier  );

	// Constructor
	nvStripifier();
	~nvStripifier();
	
	//the target vertex cache size, the structure to place the strips in, and the input indices
	void Stripify(const nvWordVec &in_indices, const int in_cacheSize, const int in_minStripLength, 
				  const unsigned short maxIndex, nvStripInfoVec &allStrips, nvFaceInfoVec &allFaces, nvFaceInfoVec &discardedDegenerates);
	void CreateStrips(const nvStripInfoVec& allStrips, nvIntVec& stripIndices, const bool bStitchStrips, int& numSeparateStrips);
	
	static int GetUniqueVertexInB(nvFaceInfo *faceA, nvFaceInfo *faceB);
	//static int GetSharedVertex(nvFaceInfo *faceA, nvFaceInfo *faceB);
	static void GetSharedVertices(nvFaceInfo *faceA, nvFaceInfo *faceB, int* vertex0, int* vertex1);

	static bool IsDegenerate(const nvFaceInfo* face);
	
protected:
	
	int cacheSize;
	int minStripLength;
	float meshJump;
	bool bFirstTimeResetPoint;


	nvWordVec indices;
	
	/////////////////////////////////////////////////////////////////////////////////
	//
	// Big mess of functions called during stripification
	//
	/////////////////////////////////////////////////////////////////////////////////

	//********************
	bool IsMoneyFace(const nvFaceInfo& face);
	bool FaceContainsIndex(const nvFaceInfo& face, int index);

	bool IsCW(nvFaceInfo *faceInfo, int v0, int v1);
	bool NextIsCW(const int numIndices);
	
	bool IsDegenerate(const unsigned short v0, const unsigned short v1, const unsigned short v2);
	
	static int  GetNextIndex(const nvWordVec &indices, nvFaceInfo *face);
	static nvEdgeInfo *FindEdgeInfo(nvEdgeInfoVec &edgeInfos, int v0, int v1);
	static nvFaceInfo *FindOtherFace(nvEdgeInfoVec &edgeInfos, int v0, int v1, nvFaceInfo *faceInfo);
	nvFaceInfo *FindGoodResetPoint(nvFaceInfoVec &faceInfos, nvEdgeInfoVec &edgeInfos);
	
	void FindAllStrips(nvStripInfoVec &allStrips, nvFaceInfoVec &allFaceInfos, nvEdgeInfoVec &allEdgeInfos, int numSamples);
	void SplitUpStripsAndOptimize(nvStripInfoVec &allStrips, nvStripInfoVec &outStrips, nvEdgeInfoVec& edgeInfos, nvFaceInfoVec& outFaceList, nvFaceInfoVec& discardedDegenerates);
	void RemoveSmallStrips(nvStripInfoVec& allStrips, nvStripInfoVec& allBigStrips, nvFaceInfoVec& faceList);
	
	bool FindTraversal(nvFaceInfoVec &faceInfos, nvEdgeInfoVec &edgeInfos, nvStripInfo *strip, nvStripStartInfo &startInfo);
//	int  CountRemainingTris(std::list<nvStripInfo*>::iterator iter, std::list<nvStripInfo*>::iterator  end);
	
	void CommitStrips(nvStripInfoVec &allStrips, const nvStripInfoVec &strips);
	
	float AvgStripSize(const nvStripInfoVec &strips);
	int FindStartPoint(nvFaceInfoVec &faceInfos, nvEdgeInfoVec &edgeInfos);
	
	void UpdateCacheStrip(nvVertexCache* vcache, nvStripInfo* strip);
	void UpdateCacheFace(nvVertexCache* vcache, nvFaceInfo* face);
	float CalcNumHitsStrip(nvVertexCache* vcache, nvStripInfo* strip);
	int CalcNumHitsFace(nvVertexCache* vcache, nvFaceInfo* face);
	int NumNeighbors(nvFaceInfo* face, nvEdgeInfoVec& edgeInfoVec);
	
	void BuildStripifyInfo(nvFaceInfoVec &faceInfos, nvEdgeInfoVec &edgeInfos, const unsigned short maxIndex);
	bool AlreadyExists(nvFaceInfo* faceInfo, nvFaceInfoVec& faceInfos);
	
	// let our strip info classes and the other classes get
	// to these protected stripificaton methods if they want
	friend class nvStripInfo;
};

#endif

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
