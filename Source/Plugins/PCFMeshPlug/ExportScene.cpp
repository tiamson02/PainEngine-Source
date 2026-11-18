/*
History:
25.6.04
	-export selected fixed because very slow
	-export can be stopped after any mesh
	-Maya warning if mesh limit exceeded


*/

#define _BOOL
#define TRUE_AND_FALSE_DEFINED

#include <stdio.h>
#include <string.h>

#include <maya/MStatus.h>
#include <maya/MPxFileTranslator.h>
#include <maya/MComputation.h>
#include <maya/MDagPath.h>
#include <maya/MItDag.h>
#include <maya/MGlobal.h>
#include <maya/MItSelectionList.h>
#include <maya/MSelectionList.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnMesh.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MColor.h>
#include <maya/MMatrix.h>
#include <maya/MItDependencyGraph.h>


#if defined( TRANSGAMING_PSP )
# include "TNG_stdio_PSP.h"
#elif defined( TRANSGAMING_PS2 )
# include "TNG_stdio_PS2.h"
#elif defined( TRANSGAMING_WINDOWS )
# include "../TNG_WIN/TNG_stdio_WIN.h"
#endif

static FILE* of;
static FILE* f;
static void ProcessMesh(MDagPath& dagPath);
static bool ProcessGeometry(MComputation& computation, MPxFileTranslator::FileAccessMode mode);

#define MAX_MESH_COUNT          (4096)
#define MAX_POLY_VERTEX_COUNT   (256)
#define MAX_VERTEX_COUNT	    (65536)
#define MAX_INDEX_COUNT		    (65536)
#define MAX_GROUP_COUNT		    (128)
#define MAX_UVSET_COUNT		    (2)

typedef unsigned char   BYTE;
typedef unsigned short	WORD;
typedef unsigned long   DWORD;

typedef float   Vec3[3];
typedef float	Mat4[4*4];

struct TextureDef
{
	float	Pan[2];
	float	Repeat[2];
	char	FileName[256];

	TextureDef()
	{
		FileName[0] = 0;
		Pan[0] = Pan[1] = 0;
		Repeat[0] = Repeat[1] = 1;
	}
};

struct ShadingGroup
{
	int		Start;
	int		Count;

	TextureDef	ColorMap;
	TextureDef	LightMap;
	TextureDef	BlendMap;
	TextureDef	AlphaMap;
};

struct Vertex
{
	int	vidx;
	int	nidx;
	int tidx[MAX_UVSET_COUNT];	
};

struct UVSet
{
	MString		Name;
	MFloatArray U,V;
};

struct Mesh
{
	MString		Name;
	Mat4		TransformMatrix;

	// mesh data
	MFloatPointArray	Points;
	MFloatVectorArray	Normals;

	UVSet	UVSets[MAX_UVSET_COUNT];
	int		UVSetCount;

	Vertex          Verts[MAX_VERTEX_COUNT];
	WORD			Indices[MAX_INDEX_COUNT];
	ShadingGroup	Groups[MAX_GROUP_COUNT];

	int     VertCount;
	int		TriCount;
	int		IndexCount;
	int		GroupCount;

	Vec3    Mins;
	Vec3    Maxs;
};

static Mesh mesh;
static int MeshCount;
static DWORD MeshOffsets[MAX_MESH_COUNT];

static void SaveMesh();

// =================================================================

static void StripFileName(const char* in, char* fileName)
{
	const char* p,*p2;
	size_t len;

	p = (p = strrchr(in,'/')) ? p+1 : in;
	len = strlen(p);

	p2 = strrchr(p,'.');
	if (p2)
		len -= strlen(p2);

	strncpy(fileName,p,len);
	fileName[len] = 0;
}

static inline void BoxClear(Vec3 mins, Vec3 maxs)
{
	mins[0] = mins[1] = mins[2] = +99999999.0f;
	maxs[0] = maxs[1] = maxs[2] = -99999999.0f;
}

static inline void BoxAdd(Vec3 mins, Vec3 maxs, const Vec3 pt) 
{
	int i;

	for (i=0; i<3; ++i) {
		const float x = pt[i];
		if (x < mins[i])
			mins[i] = x;
		if (x > maxs[i])
			maxs[i] = x;
	}
}

// =================================================================

static inline void Write(const int x)
{
#ifdef TRANSGAMING
	TNG_fwrite(&x,sizeof(int),1,f);
#else
	fwrite(&x,sizeof(int),1,f);
#endif
}

static inline void Write(const WORD x)
{
#ifdef TRANSGAMING
	TNG_fwrite(&x,sizeof(WORD),1,f);
#else
	fwrite(&x,sizeof(WORD),1,f);
#endif
}

static inline void Write(const DWORD x)
{
#ifdef TRANSGAMING
	TNG_fwrite(&x,sizeof(DWORD),1,f);
#else
	fwrite(&x,sizeof(DWORD),1,f);
#endif
}

static inline void Write(const Vec3 a)
{
#ifdef TRANSGAMING
	TNG_fwrite(a,sizeof(Vec3),1,f);
#else
	fwrite(a,sizeof(Vec3),1,f);
#endif
}

static inline void Write(const char* str)
{
	int len = (int) strlen(str)+1;
	Write(len);
#ifdef TRANSGAMING
	TNG_fwrite(str,len,1,f);
#else
	fwrite(str,len,1,f);
#endif
}

static inline void Write(const TextureDef& texDef)
{
	Write(texDef.FileName);
#ifdef TRANSGAMING
	TNG_fwrite(texDef.Pan,sizeof(float),2,f);
	TNG_fwrite(texDef.Repeat,sizeof(float),2,f);
#else
	fwrite(texDef.Pan,sizeof(float),2,f);
	fwrite(texDef.Repeat,sizeof(float),2,f);
#endif
}

// =================================================================

MStatus ExportScene(const MFileObject& file, 
			  const MString& options, 
			  MPxFileTranslator::FileAccessMode mode)
{
	const char* fileName = file.fullName().asChar();

	char tmp[1024];
	strcpy(tmp,fileName);
	strcpy(tmp+strlen(tmp)-4,".log");

#ifdef TRANSGAMING
	of = TNG_fopen(tmp,"w");

	TNG_fprintf(of,"FILE: %s\n",fileName);
#else
	of = fopen(tmp,"w");
	fprintf(of,"FILE: %s\n",fileName);
#endif

#ifdef TRANSGAMING
	f = TNG_fopen(fileName,"wb");
#else
	f = fopen(fileName,"wb");
#endif


	if (f) {

		MComputation computation;
		computation.beginComputation();

		MeshCount = 0;
		if (!ProcessGeometry(computation,mode)) {

		}

		computation.endComputation();

		// save offset table
		for (int i=0; i<MeshCount; ++i)
			Write(MeshOffsets[i]);

		Write(MeshCount);
		Write((DWORD)0xdeadbeef);

#ifdef TRANSGAMING
		TNG_fclose(f);
#else
		fclose(f);
#endif
	}

#ifdef TRANSGAMING
	TNG_fclose(of);
#else
	fclose(of);
#endif
	return MStatus::kSuccess;
}

static bool IsObjectSelected(MItSelectionList& iter, const MDagPath& path)
{
	while (!iter.isDone()) {
		MDagPath sDagPath;
		if (iter.getDagPath(sDagPath) && (sDagPath == path))
			return true;

		iter.next();
	}

	return false;
}

static inline void GetObjectMatrix(MDagPath& dagPath, Mat4& outMatrix)
{
	MMatrix matrix=dagPath.inclusiveMatrix();

	for(int i=0; i<4; ++i) {
		for(int j=0; j<4; ++j) {
			outMatrix[i*4+j]=(float)matrix(i,j);
		}
	}
}
/*
old bart orginal
static bool ProcessGeometry(MComputation& computation, MPxFileTranslator::FileAccessMode mode)
{
	MStatus stat = MS::kFailure;
	MItDag dagIterator(MItDag::kDepthFirst, MFn::kInvalid, &stat);
	if (MS::kSuccess != stat)
		return false;

	MSelectionList activeList;
	MGlobal::getActiveSelectionList(activeList);

	for (; !dagIterator.isDone(); dagIterator.next()) {

		MDagPath currentPath;
		MObject currentNode = dagIterator.item(&stat);
		stat = dagIterator.getPath(currentPath);

		if (!stat)
			continue;

		if (mode == MPxFileTranslator::kExportActiveAccessMode) {

			bool result = false;
			MDagPath searchPath(currentPath);
			while (true) {

				if (IsObjectSelected(MItSelectionList(activeList,MFn::kDagNode),searchPath)) {
					result = true;
					break;
				}    
				if (searchPath.length() <= 1)
					break;
				searchPath.pop();
			}   

			if (!result)
				continue;
		}

		MFnDagNode fnDagNode(currentPath, &stat);
		if (!stat)
			continue;

		if (fnDagNode.isIntermediateObject())
			continue;

		if (currentPath.hasFn (MFn::kMesh)) {

			// skip transforms
			if (currentPath.hasFn(MFn::kTransform))
				continue;

			ProcessMesh(currentPath);
		}
	}

	return true;
}

*/
static bool ProcessGeometry(MComputation& computation, MPxFileTranslator::FileAccessMode mode)
{
	MStatus stat = MS::kFailure;

	int MeshCount =0;
	
	// trzeba rozdzielic export selection od export all bo bardzo dlugo trwa
	
	if (mode == MPxFileTranslator::kExportActiveAccessMode)// export selected meshes
	{

		
		MSelectionList sList;
		MDagPath dagPath;

		MGlobal::getActiveSelectionList( sList );
		if (sList.length() == 0)
		{
			MGlobal::displayError("nothing selected!!!");
			return false;
		}
		for ( unsigned int index = 0; index < sList.length(); index++ )
		{
			
			if (computation.isInterruptRequested())
			{
				return true;
			}	
			
			if (MeshCount >= MAX_MESH_COUNT)
			{
				MGlobal::displayError("Too many meshes. Mesh limit = 4096, skipping rest!!!");
				return true;
			}

			stat = sList.getDagPath(index, dagPath);

			if (!stat)
			{
				continue;
			}

			//extend to shape
			//     
			// skip over intermediate objects
			if (! dagPath.node().hasFn(MFn::kMesh) 
				&& ! (dagPath.node().hasFn(MFn::kTransform) )
				&&  dagPath.hasFn(MFn::kMesh)
				&& dagPath.hasFn(MFn::kLight) )		
			{
				continue;
			}
			if (dagPath.node().hasFn(MFn::kTransform)  &&  dagPath.node().hasFn(MFn::kTransform))
			{
				dagPath.extendToShape();
			}
			
			MFnDagNode dagNode( dagPath, &stat );

			if (dagPath.hasFn(MFn::kMesh) &&  !dagNode.isIntermediateObject())
			{
							
				ProcessMesh(dagPath);
				MeshCount++;
			}

				
		}

	}
	else // all meshes
	{

		MItDag dagIterator(MItDag::kDepthFirst, MFn::kInvalid, &stat);
		if (MS::kSuccess != stat)
			return false;
		
		MDagPath currentPath;

		for (; !dagIterator.isDone(); dagIterator.next()) 
		{

			if (computation.isInterruptRequested())
			{
				return true;
			}
			if (MeshCount >= MAX_MESH_COUNT)
			{
				MGlobal::displayError("Too many meshes. Mesh limit = 4096, skipping rest!!!");
				return true;
			}
			
			
			MObject currentNode = dagIterator.item(&stat);
			stat = dagIterator.getPath(currentPath);

			if (!stat)
				continue;

			MFnDagNode fnDagNode(currentPath, &stat);
			if (!stat)
				continue;

			if (fnDagNode.isIntermediateObject())
				continue;

			if (currentPath.hasFn (MFn::kMesh)) {

				// skip transforms
				if (currentPath.hasFn(MFn::kTransform))
					continue;

				ProcessMesh(currentPath);
				MeshCount++;
			}
		}
	}

	return true;
}

static void ExtractTextureDef(MPlug& plug, TextureDef& texDef)
{
	MStatus stat;

	MItDependencyGraph dgIt(plug, 
		MFn::kFileTexture, 
		MItDependencyGraph::kUpstream, 
		MItDependencyGraph::kBreadthFirst, 
		MItDependencyGraph::kNodeLevel, 
		&stat);
	if (!stat)
		return;

	dgIt.disablePruningOnFilter();

	if (dgIt.isDone())
		return;	// no texture file node was found

	// get the texture file name
	MString tmp;
	MFnDependencyNode(dgIt.thisNode()).findPlug("fileTextureName").getValue(tmp);
	StripFileName(tmp.asChar(),texDef.FileName);

	float repeatU,repeatV;
	MFnDependencyNode(dgIt.thisNode()).findPlug("repeatU").getValue(repeatU);
	MFnDependencyNode(dgIt.thisNode()).findPlug("repeatV").getValue(repeatV);

	if (repeatU)
		texDef.Repeat[0] = repeatU;
	if (repeatV)
		texDef.Repeat[1] = repeatV;

	float panU,panV;
	MFnDependencyNode(dgIt.thisNode()).findPlug("offsetU").getValue(panU);
	MFnDependencyNode(dgIt.thisNode()).findPlug("offsetV").getValue(panV);

	texDef.Pan[0] = panU;
	texDef.Pan[1] = panV;
}

static void ProcessShader(MObject& shader, ShadingGroup& group)
{
	// init shading group
	group = ShadingGroup();

	// get surfaceShader
	MStatus stat;
	MPlug mpShader = MFnDependencyNode(shader).findPlug("surfaceShader", &stat);

	if (!stat || mpShader.isNull())
		return;

	// get connected plugs
	MPlugArray aPlugs;
	mpShader.connectedTo(aPlugs, true, false, &stat);

	if (!stat || aPlugs.length() != 1) 
		return;

	MFnDependencyNode fnShaderNode;
	stat = fnShaderNode.setObject(mpShader.node());

	if (!stat)
		return;

	/*
	switch (aPlugs[0].node().apiType()) { 
	case MFn::kLambert:
	case MFn::kBlinn:
	case MFn::kPhong:
	break;
	}
	*/

	// extract lightmap: always on ambient color
	MPlug mpAmbientColor = MFnDependencyNode(aPlugs[0].node()).findPlug("ambientColor", &stat);
	if (!stat)
		return;

	ExtractTextureDef(mpAmbientColor,group.LightMap);

	// get color plug
	MPlug mpColor = MFnDependencyNode(aPlugs[0].node()).findPlug("color", &stat);
	if (!stat)
		return;

	MItDependencyGraph dgIt(mpColor, 
		MFn::kInvalid,
		MItDependencyGraph::kUpstream,
		MItDependencyGraph::kBreadthFirst,
		MItDependencyGraph::kNodeLevel,
		&stat);

	if (!stat)
		return;

	dgIt.next();
	MObject node = dgIt.thisNode();

	// parse textures either directly from file or from layered texture
	if (node.hasFn(MFn::kFileTexture)) {
		ExtractTextureDef(mpColor,group.ColorMap);
	}
	else if (node.hasFn(MFn::kLayeredTexture)) {
		//fprintf(of,"kLayeredTexture\n");

		MPlug inputs = MFnDependencyNode(node).findPlug("inputs", &stat);
		if (!stat)
			return;

		//fprintf(of,"parsing layer0\n");
		MPlug mpLayer0 = inputs.elementByPhysicalIndex(0, &stat);
		if (!stat)
			return;

		//fprintf(of,"parsing layer1\n");
		MPlug mpLayer1 = inputs.elementByPhysicalIndex(1, &stat);
		if (!stat)
			return;

		//fprintf(of,"parsing color0\n");
		MPlug mpColor0 = mpLayer0.child(0, &stat);	
		if (!stat)
			return;

		//fprintf(of,"parsing alpha\n");
		MPlug mpAlpha  = mpLayer0.child(1, &stat);
		if (!stat)
			return;

		//fprintf(of,"parsing color1\n");
		MPlug mpColor1 = mpLayer1.child(0, &stat);
		if (!stat)
			return;

		ExtractTextureDef(mpColor0,group.ColorMap);
		ExtractTextureDef(mpColor1,group.BlendMap);
		ExtractTextureDef(mpAlpha,group.AlphaMap);

#ifdef TRANSGAMING
		TNG_fprintf(of,"ColorMap: \"%s\"\n",group.ColorMap.FileName);
		TNG_fprintf(of,"BlendMap: \"%s\"\n",group.BlendMap.FileName);
		TNG_fprintf(of,"AlphaMap: \"%s\"\n",group.AlphaMap.FileName);
#else
		fprintf(of,"ColorMap: \"%s\"\n",group.ColorMap.FileName);
		fprintf(of,"BlendMap: \"%s\"\n",group.BlendMap.FileName);
		fprintf(of,"AlphaMap: \"%s\"\n",group.AlphaMap.FileName);
#endif
    }
}

static inline int AddUniqueVertex(const Vertex& vertex)
{    
	int index;

	for (index=0; index<mesh.VertCount; ++index) {
		const Vertex& v = mesh.Verts[index];

		// pos & normal
		if (v.vidx == vertex.vidx) {
			if (v.nidx == vertex.nidx) {

				// check uvs
				int k;
				for (k=0; k<mesh.UVSetCount; ++k) {
					if (v.tidx[k] != vertex.tidx[k])
						break;
				}

				if (k == mesh.UVSetCount) // found it            
					return index;        
			}
		}
	}

	// gotta add a new one
	index = mesh.VertCount++;
	mesh.Verts[index] = vertex;

	return index;
}

static inline void DumpTexDef(const char* name, const TextureDef& texDef)
{
#ifdef TRANSGAMING
	TNG_fprintf(of,"\t\t\t%s: [%.2f %.2f][%.2f %.2f] \"%s\"\n",name,
		texDef.Pan[0],texDef.Pan[1],texDef.Repeat[0],texDef.Repeat[1],texDef.FileName);
#else
	fprintf(of,"\t\t\t%s: [%.2f %.2f][%.2f %.2f] \"%s\"\n",name,
		texDef.Pan[0],texDef.Pan[1],texDef.Repeat[0],texDef.Repeat[1],texDef.FileName);
#endif
}

static void ProcessMesh(MDagPath& dagPath)
{
	int i;
	MStatus stat;

	// get mesh from dagPath
	MFnMesh fnMesh(dagPath, &stat);
	if (!stat)
		return;

	if (MeshCount >= MAX_MESH_COUNT)
	{
		MGlobal::displayWarning ("Too may meshes !!!"); 
		return;
	}

	// get mesh name
	mesh.Name = fnMesh.partialPathName().asChar();
	GetObjectMatrix(dagPath,mesh.TransformMatrix);

	// bound check
	if (fnMesh.numVertices() > MAX_VERTEX_COUNT) {
#ifdef TRANSGAMING
		TNG_fprintf(of,"ERROR: MAX_VERTEX_COUNT\n");
#else
		fprintf(of,"ERROR: MAX_VERTEX_COUNT\n");
#endif
		return;
	}

	if (fnMesh.numNormals() > MAX_VERTEX_COUNT) {
#ifdef TRANSGAMING
		TNG_fprintf(of,"ERROR: MAX_VERTEX_COUNT (normals)\n");
#else
		fprintf(of,"ERROR: MAX_VERTEX_COUNT (normals)\n");
#endif
		return;
	}

	// get vertices and normals
	fnMesh.getPoints (mesh.Points,MSpace::kWorld);
	fnMesh.getNormals(mesh.Normals,MSpace::kWorld);

#ifdef TRANSGAMING
	TNG_fprintf(of,"mesh [%d]: %s (%d points, %d normals)\n",MeshCount,
		mesh.Name.asChar(),mesh.Points.length(),mesh.Normals.length());
#else
	fprintf(of,"mesh [%d]: %s (%d points, %d normals)\n",MeshCount,
		mesh.Name.asChar(),mesh.Points.length(),mesh.Normals.length());
#endif

	// get and write UVs
	mesh.UVSetCount = fnMesh.numUVSets();
	if (!mesh.UVSetCount) {
#ifdef TRANSGAMING
		TNG_fprintf(of,"ERROR: no UV for mesh, skipping\n");
#else
		fprintf(of,"ERROR: no UV for mesh, skipping\n");
#endif
		return;
	}

	if (mesh.UVSetCount > MAX_UVSET_COUNT) {
#ifdef TRANSGAMING
		TNG_fprintf(of,"WARNING: too many UV sets for mesh (%d), truncating\n",mesh.UVSetCount);
#else
		fprintf(of,"WARNING: too many UV sets for mesh (%d), truncating\n",mesh.UVSetCount);
#endif
		mesh.UVSetCount = MAX_UVSET_COUNT;
	}

	MStringArray uvSetNames;
	fnMesh.getUVSetNames(uvSetNames);

	for (i=0; i<mesh.UVSetCount; ++i) {
		if (fnMesh.numUVs(uvSetNames[i]) > MAX_VERTEX_COUNT) {
#ifdef TRANSGAMING
			TNG_fprintf(of,"ERROR: MAX_VERTEX_COUNT (uvSet: %s)\n",uvSetNames[i].asChar());
#else
			fprintf(of,"ERROR: MAX_VERTEX_COUNT (uvSet: %s)\n",uvSetNames[i].asChar());
#endif
			return;
		}

		mesh.UVSets[i].Name = uvSetNames[i];
		fnMesh.getUVs(mesh.UVSets[i].U,mesh.UVSets[i].V,&mesh.UVSets[i].Name);

#ifdef TRANSGAMING
		TNG_fprintf(of,"\tUVSet[%d]: %s (%d,%d)\n",i,
			mesh.UVSets[i].Name.asChar(),mesh.UVSets[i].U.length(),mesh.UVSets[i].V.length());		
#else
		fprintf(of,"\tUVSet[%d]: %s (%d,%d)\n",i,
			mesh.UVSets[i].Name.asChar(),mesh.UVSets[i].U.length(),mesh.UVSets[i].V.length());		
#endif
	}

	// shaders
	MObjectArray shaders;
	MIntArray shaderIndices;
	fnMesh.getConnectedShaders(0, shaders, shaderIndices);

	if (shaders.length() > MAX_GROUP_COUNT) {
#ifdef TRANSGAMING
		TNG_fprintf(of,"ERROR: MAX_GROUP_COUNT\n");
#else
		fprintf(of,"ERROR: MAX_GROUP_COUNT\n");
#endif
		return;
	}

	// poly iterator
	MItMeshPolygon poly(fnMesh.object());

	// walk shading groups
	int base = 0;

	mesh.VertCount = 0;
	mesh.TriCount = 0;
	mesh.GroupCount = 0;
	mesh.IndexCount = 0;

	for (unsigned int shaderIndex=0; shaderIndex<shaders.length(); ++shaderIndex) {

		for (unsigned int index=0; index<shaderIndices.length(); ++index) {
			if (shaderIndices[index] == shaderIndex) {

				int junk;
				stat = poly.setIndex(index,junk);
				if (!stat)
					continue;

				int vertNum = poly.polygonVertexCount();
				if (vertNum > MAX_POLY_VERTEX_COUNT) {
#ifdef TRANSGAMING
					TNG_fprintf(of,"WARNING: polygon too big, skipping\n");
#else
					fprintf(of,"WARNING: polygon too big, skipping\n");
#endif
					continue;
				}

				Vertex faceIdx[MAX_POLY_VERTEX_COUNT];
				for (int i=0; i<vertNum; ++i) {
					faceIdx[i].vidx = poly.vertexIndex(i);
					faceIdx[i].nidx = poly.normalIndex(i);

					for (int j=0; j<mesh.UVSetCount; ++j) {
						faceIdx[i].tidx[j] = -1;
						poly.getUVIndex(i, faceIdx[i].tidx[j], &mesh.UVSets[j].Name);
					}
				}

				// process triangles
				int triNum;
				poly.numTriangles(triNum);

				// loop over triangles
				for (int i=0; i<triNum; ++i) {

					// get triangle
					MPointArray pts;
					MIntArray indices;

					poly.getTriangle(i, pts, indices);

					//for (int idx=0; idx<3; ++idx) {
					for (int idx=2; idx>=0; --idx) {
						Vertex vertex;
						// get indices for this vertex
						vertex.vidx = indices[idx];
						vertex.nidx = -1;

						for (int k=0; k<mesh.UVSetCount; ++k)
							vertex.tidx[k] = -1;

						// find matching index
						for (int j=0; j<vertNum; ++j) {
							if (faceIdx[j].vidx == vertex.vidx) {
								vertex.nidx = faceIdx[j].nidx;

								for (int k=0; k<mesh.UVSetCount; ++k)
									vertex.tidx[k] = faceIdx[j].tidx[k];

								break;
							}
						}

						/*
						const int index = mesh.VertCount++;
						mesh.Verts[index] = vertex;
						mesh.Indices[mesh.IndexCount++] = index;
						*/

						mesh.Indices[mesh.IndexCount++] = (WORD) AddUniqueVertex(vertex);
					}

					++mesh.TriCount;
				}
			}
		}

		int count = mesh.TriCount - base;
		if (count) {

			ShadingGroup& group = mesh.Groups[mesh.GroupCount++];
			ProcessShader(shaders[shaderIndex], group);

			group.Start = base*3;
			group.Count = count;

			/*
#ifdef TRANSGAMING
			TNG_fprintf(of,"\t\tgroup[%d]: C:\"%s\" L:\"%s\" (%d %d)\n",
			mesh.GroupCount,
			mesh.Groups[mesh.GroupCount].ColorMap,
			mesh.Groups[mesh.GroupCount].LightMap,
			mesh.Groups[mesh.GroupCount].Start, 
			mesh.Groups[mesh.GroupCount].Count);
#else
			fprintf(of,"\t\tgroup[%d]: C:\"%s\" L:\"%s\" (%d %d)\n",
			mesh.GroupCount,
			mesh.Groups[mesh.GroupCount].ColorMap,
			mesh.Groups[mesh.GroupCount].LightMap,
			mesh.Groups[mesh.GroupCount].Start, 
			mesh.Groups[mesh.GroupCount].Count);
#endif
			*/

			/*
			// dump
#ifdef TRANSGAMING
			TNG_fprintf(of,"\t\tshader[%d]: '%s' (%d %d)\n",
			shaderIndex,MFnDependencyNode(shaders[shaderIndex]).name().asChar(),
			mesh.Groups[shaderIndex].Start, mesh.Groups[shaderIndex].Count);
#else
			fprintf(of,"\t\tshader[%d]: '%s' (%d %d)\n",
			shaderIndex,MFnDependencyNode(shaders[shaderIndex]).name().asChar(),
			mesh.Groups[shaderIndex].Start, mesh.Groups[shaderIndex].Count);
#endif
			*/
		}

		base = mesh.TriCount;
	}

	for (int i=0; i<mesh.GroupCount; ++i) {
		const ShadingGroup& group = mesh.Groups[i];

#ifdef TRANSGAMING
		TNG_fprintf(of,"\t\tgroup[%d]: (%d %d)\n",i,group.Start,group.Count);
#else
		fprintf(of,"\t\tgroup[%d]: (%d %d)\n",i,group.Start,group.Count);
#endif
		DumpTexDef("ColorMap",group.ColorMap);
		DumpTexDef("LightMap",group.LightMap);
		DumpTexDef("BlendMap",group.BlendMap);
		DumpTexDef("AlphaMap",group.AlphaMap);
	}

	/*
	int err_vertex = 0;
	int err_normal_missing = 0;
	int err_normal = 0;
	int err_tidx_missing[] = { 0, 0 };

	for (int i=0; i<mesh.VertCount; ++i) {
	const Vertex& vertex = mesh.Verts[i];

	if ((vertex.vidx == -1) || vertex.vidx >= mesh.Points.length())
	err_vertex++;

	if (vertex.nidx == -1) {
	err_normal_missing++;
	} else if (vertex.nidx >= mesh.Normals.length()) {
	vertex.nidx = -1;
	err_normal++;
	}

	for (int k=0; k<mesh.UVSetCount; ++k) {
	if (vertex.tidx[k] == -1) {
	err_tidx_missing[k]++;
	} else if ((vertex.tidx[k] >= mesh.UVSets[k].U.length())
	|| (vertex.tidx[k] >= mesh.UVSets[k].V.length())
	{
	vertex.tidx[k] = -1;
	err_tidx[k]++;
	}
	}
	}
	*/

	if (!mesh.GroupCount) {
#ifdef TRANSGAMING
		TNG_fprintf(of,"WARNING: mesh '%s' has no shading groups, skipping\n",mesh.Name);
#else
		fprintf(of,"WARNING: mesh '%s' has no shading groups, skipping\n",mesh.Name);
#endif
		return;
	}

#ifdef TRANSGAMING
	MeshOffsets[MeshCount++] = TNG_ftell(f);
#else
	MeshOffsets[MeshCount++] = ftell(f);
#endif

	SaveMesh();
}

static void SaveMesh()
{
	char name[256];

	strcpy(name,mesh.Name.asChar());
	strlwr(name);

#ifdef TRANSGAMING
	TNG_fprintf(of,"\tSAVE: \"%s\": [%d] V:%d E:%d T:%d G:%d\n\n",name,
		mesh.UVSetCount,mesh.VertCount,mesh.IndexCount,mesh.TriCount,mesh.GroupCount);
#else
	fprintf(of,"\tSAVE: \"%s\": [%d] V:%d E:%d T:%d G:%d\n\n",name,
		mesh.UVSetCount,mesh.VertCount,mesh.IndexCount,mesh.TriCount,mesh.GroupCount);
#endif

	// sanity magic and mesh name
	Write((DWORD) 0xdeafbabe);
	Write(name);

	// mesh transform
#ifdef TRANSGAMING
	TNG_fwrite(mesh.TransformMatrix,sizeof(Mat4),1,f);
#else
	fwrite(mesh.TransformMatrix,sizeof(Mat4),1,f);
#endif

	// number of uv sets
	Write(mesh.UVSetCount);

	// create and save vertices
	Write(mesh.VertCount);
	BoxClear(mesh.Mins,mesh.Maxs);

	if (mesh.UVSetCount == 1) {

		// save verts in pos,normal,tu format
		for (int i=0; i<mesh.VertCount; ++i) {

			const Vertex& vertex = mesh.Verts[i];

			float d[8];
			memset(d,0,sizeof(d));

			// pos
			for (int j=0; j<3; ++j)
				d[j] = (float) mesh.Points[vertex.vidx][j];

			// normal
			if (vertex.nidx != -1)
				for (int j=0; j<3; ++j)
					d[3+j] = (float) mesh.Normals[vertex.nidx][j];

			// uvs
			if (vertex.tidx[0] != -1) {
				d[6] = mesh.UVSets[0].U[vertex.tidx[0]];
				d[7] = 1.0f - mesh.UVSets[0].V[vertex.tidx[0]];
			}

			// save 
#ifdef TRANSGAMING
			TNG_fwrite(&d,sizeof(float),8,f);
#else
			fwrite(&d,sizeof(float),8,f);
#endif
			BoxAdd(mesh.Mins,mesh.Maxs,d);
		}

		// no separate normals
		const int zero = 0;
		Write(zero);
	} 
	else if (mesh.UVSetCount == 2) {

		// save verts in pos,w,tu1,tu2 format
		for (int i=0; i<mesh.VertCount; ++i) {
			const Vertex& vertex = mesh.Verts[i];

			float d[8];
			memset(d,0,sizeof(d));

			// pos
			for (int j=0; j<3; ++j)
				d[j] = (float) mesh.Points[vertex.vidx][j];

			// pad
			d[3] = 0;

			// uv 1
			if (vertex.tidx[0] != -1) {
				//if (vertex.tidx[0] >= mesh.UVSets[0].U.length())
#ifdef TRANSGAMING
				//    TNG_fprintf(of,"BOGUS: vertex[%d]: tidx[0] = %d (%d)\n",
				//        i,vertex.tidx[0],mesh.UVSets[0].U.length());
#else
				//    fprintf(of,"BOGUS: vertex[%d]: tidx[0] = %d (%d)\n",
				//        i,vertex.tidx[0],mesh.UVSets[0].U.length());
#endif

				d[4] = mesh.UVSets[0].U[vertex.tidx[0]];
				d[5] = 1.0f - mesh.UVSets[0].V[vertex.tidx[0]];
			}

			// uv 2
			if (vertex.tidx[1] != -1) {
				//if (vertex.tidx[1] >= mesh.UVSets[1].U.length())
#ifdef TRANSGAMING
				//    TNG_fprintf(of,"BOGUS: vertex[%d]: tidx[1] = %d\n",
				//        i,vertex.tidx[1],mesh.UVSets[1].U.length());
#else
				//    fprintf(of,"BOGUS: vertex[%d]: tidx[1] = %d\n",
				//        i,vertex.tidx[1],mesh.UVSets[1].U.length());
#endif

				d[6] = mesh.UVSets[1].U[vertex.tidx[1]];
				d[7] = 1.0f - mesh.UVSets[1].V[vertex.tidx[1]];
			}

			// save
#ifdef TRANSGAMING
			TNG_fwrite(&d,sizeof(float),8,f);
#else
			fwrite(&d,sizeof(float),8,f);
#endif
			BoxAdd(mesh.Mins,mesh.Maxs,d);
		}


		// save normals separately
		Write(mesh.VertCount);
		for (int i=0; i<mesh.VertCount; ++i) {
			const Vertex& vertex = mesh.Verts[i];
			float n[3] = {0,0,0};

			if (vertex.nidx != -1)
				for (int j=0; j<3; ++j)
					n[j] = (float) mesh.Normals[vertex.nidx][j];

#ifdef TRANSGAMING
			TNG_fwrite(&n,sizeof(float),3,f);
#else
			fwrite(&n,sizeof(float),3,f);
#endif
		}
	}

	// save box
	Write(mesh.Mins);
	Write(mesh.Maxs);

	// save indices
	/*
	for (int i=0; i<mesh.TriCount; ++i) {
	mesh.Indices[i*3+0] = mesh.Indices[i*3+2];
	mesh.Indices[i*3+1] = mesh.Indices[i*3+1];
	mesh.Indices[i*3+2] = mesh.Indices[i*3+0];
	}
	*/

	Write(mesh.IndexCount);
#ifdef TRANSGAMING
	TNG_fwrite(&mesh.Indices[0],sizeof(WORD),mesh.IndexCount,f);
#else
	fwrite(&mesh.Indices[0],sizeof(WORD),mesh.IndexCount,f);
#endif

	// save groups
	Write(mesh.GroupCount);
	for (int i=0; i<mesh.GroupCount; ++i) {
		const ShadingGroup& group = mesh.Groups[i];

		Write((WORD) group.Start);
		Write((WORD) group.Count);

		Write(group.ColorMap);
		Write(group.LightMap);
		Write(group.BlendMap);
		Write(group.AlphaMap);
	}
}
