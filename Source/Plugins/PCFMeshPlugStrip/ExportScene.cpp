/*
History:
25.6.04
	-export selected fixed because very slow
	-export can be stopped after any mesh
	-Maya warning if mesh limit exceeded


*/

#define STITCH
//#define STRIP

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

#include "DynArray.h"

#include "NvTriStrip/NvTriStrip.h"

static FILE* of;
static FILE* f;

// Blair - Adding two other file pointers, one for PSP and one for PS2
static FILE* filePtr_PSP;
static FILE* filePtr_PS2;

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

void Stripify(void);
void NvidiaStripify(void);
void Init(void);

enum EOutputMapFormat
{
	eFirst = 0,
	ePCMap = eFirst,
	ePSPMap,
	ePS2Map,
	eCount
};


enum MeshFlags{
    MESH_TRISTRIP_BIT =     0x80000000,     //set if the incoming data is in tristrip format
    MESH_DEINDEXED_BIT =    0x40000000,     //set if the incoming data is deindexed
    MESH_PSPDATA_BIT =      0x20000000,     //set if the incoming data is already in the PSP native format
    MESH_PS2DATA_BIT =      0x10000000,     //set if the incoming data is already in the PS2 native format
    MESH_BOUNDINGBOX_BIT =  0x08000000,     //set if the stream contains a bounding box
    MESH_MTFORMAT_BIT =     0x04000000,     //set if the incoming data is in MT format

    MESH_UVCOUNT_MASK =     0x00000003,     //support up to 3 UV channels

    MESH_ALL_MASK =         MESH_TRISTRIP_BIT | 
                            MESH_DEINDEXED_BIT |
                            MESH_PSPDATA_BIT |
                            MESH_PS2DATA_BIT |
                            MESH_BOUNDINGBOX_BIT |
                            MESH_MTFORMAT_BIT,
};

class TexCoord
{
public:
	union 
	{
		struct { float S, T; };
		struct { float U, V; };
		struct { float X, Y; };
		float  Array[2];
	};

    TexCoord() {}
    TexCoord(float x, float y) : X(x), Y(y) {}

    bool operator==(const TexCoord& tu) const
    {
        return U==tu.U && V==tu.V;
    }

    /*
    friend FileObject& operator>>(FileObject& file, TexCoord& tu)
	{
		return file >> tu.S >> tu.T;
	}

    friend FileObject& operator<<(FileObject& file, TexCoord& tu)
	{
		return file << tu.S << tu.T;
	}
    */
};

struct TexRange
{
   TexCoord min;
   TexCoord max;

   void updateRange(const TexCoord &t)
   {
       if (t.X < min.X) min.X = t.X;
       if (t.Y < min.Y) min.Y = t.Y;

       if (t.X > max.X) max.X = t.X;
       if (t.Y > max.Y) max.Y = t.Y;
   }
};

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
public:
	TextureDef	ColorMap;
	TextureDef	LightMap;
	TextureDef	BlendMap;
	TextureDef	AlphaMap;

	int	        RenderBase;
	int	        RenderCount;
	int			VertexMin;
	int			VertexCount;

	ShadingGroup() :  RenderBase(0), RenderCount(0),ColorMap(),LightMap(),BlendMap(),AlphaMap(),
	VertexMin(0), VertexCount(0)
	 {}
	~ShadingGroup() {}
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
public:
	
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
//	DynamicArray<ShadingGroup>		Groups;
	
	int     VertCount;
	int		TriCount;
	int		IndexCount;
	int		GroupCount;

    bool    UseTriStrips;

	Vec3    Mins;
	Vec3    Maxs;
};



static int g_total_num_indices = 0;
static int g_total_num_verts = 0;
static int g_total_num_strips = 0;

static Mesh mesh;
static int MeshCount;
static DWORD MeshOffsets[MAX_MESH_COUNT];
static EOutputMapFormat format;
static DeIndexMeshes = false;
static TriStripMeshes = false;

static void DeIndexData( Mesh &mesh );
//static void SaveNonIndexedMesh();
static void SaveMesh();
static void FixTextureCoords( Mesh mesh );
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
	fwrite(&x,sizeof(int),1,f);
}

static inline void Write(const WORD x)
{
	fwrite(&x,sizeof(WORD),1,f);
}

static inline void Write(const DWORD x)
{
	fwrite(&x,sizeof(DWORD),1,f);
}

static inline void Write(const Vec3 a)
{
	fwrite(a,sizeof(Vec3),1,f);
}

static inline void Write(const char* str)
{
	int len = (int) strlen(str)+1;
	Write(len);
	fwrite(str,len,1,f);
}

static inline void Write(const TextureDef& texDef)
{
	Write(texDef.FileName);
	fwrite(texDef.Pan,sizeof(float),2,f);
	fwrite(texDef.Repeat,sizeof(float),2,f);
}

/*
// ========================== Write WITH filePtr ===================
static inline void Write(const int x, FILE* filePtr)
{
	fwrite(&x,sizeof(int),1,filePtr);
}

static inline void Write(const WORD x, FILE* filePtr)
{
	fwrite(&x,sizeof(WORD),1,filePtr);
}

static inline void Write(const DWORD x, FILE* filePtr)
{
	fwrite(&x,sizeof(DWORD),1,filePtr);
}

static inline void Write(const Vec3 a, FILE* filePtr)
{
	fwrite(a,sizeof(Vec3),1,filePtr);
}

static inline void Write(const char* str, FILE* filePtr)
{
	int len = (int) strlen(str)+1;
	Write(len);
	fwrite(str,len,1,filePtr);
}

static inline void Write(const TextureDef& texDef, FILE* filePtr)
{
	Write(texDef.FileName);
	fwrite(texDef.Pan,sizeof(float),2,filePtr);
	fwrite(texDef.Repeat,sizeof(float),2,filePtr);
}
*/
// =================================================================

MStatus ExportScene(const MFileObject& file, 
			  const MString& options, 
			  MPxFileTranslator::FileAccessMode mode)
{
	//const char* fileName = file.fullName().asChar();
    char fileName[64];
    strcpy(fileName, "coffeeffee.mpk");
	//char* fileName = (char*)file.fullName().asChar();

	char tmp[1024];
	strcpy(tmp,fileName);
	strcpy(tmp+strlen(tmp)-4,".log");

	of = fopen(tmp,"w");
	fprintf(of,"FILE: %s\n",fileName);

// ================================= PC format map ===============================
	//EOutputMapFormat format;

	for (int i=eFirst; i<eCount; ++i)
	{

        format = (EOutputMapFormat)i;

		// Choose the correct filename...
		if ( format == ePSPMap )		// PSP
		{
			char* buffer;
			buffer = strchr(fileName,'.');
			strncpy (buffer+1,"mpp",3);
			fprintf(of, "ePSPMap! fileName = %s\n", fileName);

            // Platform Parameters...
            DeIndexMeshes = false;
            TriStripMeshes = false;
		}
		else if ( format == ePS2Map )	// PS2
		{
			char* buffer;
			buffer = strchr(fileName,'.');
			strncpy (buffer+1,"mp2",3);
			fprintf(of, "ePS2Map!  fileName = %s\n", fileName);

            // Platform Parameters...
            DeIndexMeshes = false;
            TriStripMeshes = true;
		}
		else
		{
			fprintf(of, "ePCMap! fileName = %s\n", fileName);

            // Platform Parameters...
            DeIndexMeshes = true;
            TriStripMeshes = true;
		}

		f = fopen(fileName,"wb");	// PC

		if ( f ) // Were we successful? 
		{
			MComputation computation;
			computation.beginComputation();

			MeshCount = 0;
			if (!ProcessGeometry(computation,mode)) 
			{
			}
			computation.endComputation();

			// save offset table
			for (int i=0; i<MeshCount; ++i)
				Write(MeshOffsets[i]);

			Write(MeshCount);

			if ( format == ePSPMap )
				Write((DWORD)0xdeafbaad); // Our magic word for PSP
			else if ( format == ePS2Map )
				Write((DWORD)0xbadddadd); // ...and the PS2
			else
				Write((DWORD)0xdeadbeef);

			fclose(f);
		}
	}
	//	fclose(of);

// ================================= end ===============================

	fclose(of);
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

			// extend to shape
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

		fprintf(of,"ColorMap: \"%s\"\n",group.ColorMap.FileName);
		fprintf(of,"BlendMap: \"%s\"\n",group.BlendMap.FileName);
		fprintf(of,"AlphaMap: \"%s\"\n",group.AlphaMap.FileName);
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
	fprintf(of,"\t\t\t%s: [%.2f %.2f][%.2f %.2f] \"%s\"\n",name,
		texDef.Pan[0],texDef.Pan[1],texDef.Repeat[0],texDef.Repeat[1],texDef.FileName);
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

    // blair - Check the name of the mesh to determine if we want tri-stripage.  On portals and zones, we want to keep them as
    // tri-lists, since they're just squares.
    if ( !strstr( mesh.Name.toLowerCase().asChar(), "portal") && !strstr( mesh.Name.toLowerCase().asChar(), "zone") && !strstr( mesh.Name.toLowerCase().asChar(), "antyp") )
        mesh.UseTriStrips = TriStripMeshes;
    else
        mesh.UseTriStrips = false;

	GetObjectMatrix(dagPath,mesh.TransformMatrix);

	// bound check
	if (fnMesh.numVertices() > MAX_VERTEX_COUNT) {
		fprintf(of,"ERROR: MAX_VERTEX_COUNT\n");
		return;
	}

	if (fnMesh.numNormals() > MAX_VERTEX_COUNT) {
		fprintf(of,"ERROR: MAX_VERTEX_COUNT (normals)\n");
		return;
	}

	// get vertices and normals
	fnMesh.getPoints (mesh.Points,MSpace::kWorld);
	fnMesh.getNormals(mesh.Normals,MSpace::kWorld);

	fprintf(of,"mesh [%d]: %s (%d points, %d normals)\n",MeshCount,
		mesh.Name.asChar(),mesh.Points.length(),mesh.Normals.length());

	// get and write UVs
	mesh.UVSetCount = fnMesh.numUVSets();
	if (!mesh.UVSetCount) {
		fprintf(of,"ERROR: no UV for mesh, skipping\n");
		return;
	}

	if (mesh.UVSetCount > MAX_UVSET_COUNT) {
		fprintf(of,"WARNING: too many UV sets for mesh (%d), truncating\n",mesh.UVSetCount);
		mesh.UVSetCount = MAX_UVSET_COUNT;
	}

	MStringArray uvSetNames;
	fnMesh.getUVSetNames(uvSetNames);

	for (i=0; i<mesh.UVSetCount; ++i) {
		if (fnMesh.numUVs(uvSetNames[i]) > MAX_VERTEX_COUNT) {
			fprintf(of,"ERROR: MAX_VERTEX_COUNT (uvSet: %s)\n",uvSetNames[i].asChar());
			return;
		}

		mesh.UVSets[i].Name = uvSetNames[i];
		fnMesh.getUVs(mesh.UVSets[i].U,mesh.UVSets[i].V,&mesh.UVSets[i].Name);

		fprintf(of,"\tUVSet[%d]: %s (%d,%d)\n",i,
			mesh.UVSets[i].Name.asChar(),mesh.UVSets[i].U.length(),mesh.UVSets[i].V.length());		
	}

	// shaders
	MObjectArray shaders;
	MIntArray shaderIndices;
	fnMesh.getConnectedShaders(0, shaders, shaderIndices);

	if (shaders.length() > MAX_GROUP_COUNT) {
		fprintf(of,"ERROR: MAX_GROUP_COUNT\n");
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

	for (unsigned int shaderIndex=0; shaderIndex<shaders.length(); ++shaderIndex) 
    {
		for (unsigned int index=0; index<shaderIndices.length(); ++index) 
        {
			if (shaderIndices[index] == shaderIndex) 
            {

				int junk;
				stat = poly.setIndex(index,junk);
				if (!stat)
					continue;

				int vertNum = poly.polygonVertexCount();
				if (vertNum > MAX_POLY_VERTEX_COUNT) {
					fprintf(of,"WARNING: polygon too big, skipping\n");
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
		fprintf(of,"FILE: count=%d %TriCount=%d base=%d\n",count,mesh.TriCount,base);
		if (count) 
        {

		
			ShadingGroup& group=mesh.Groups[mesh.GroupCount++];

			ProcessShader(shaders[shaderIndex], group);

            group.RenderBase = base*3;
            group.RenderCount = count;
			
			fprintf(of,"\t\tgroup[%d]: C:\"%s\" L:\"%s\" (%d %d)\n",
			mesh.GroupCount,
			mesh.Groups[mesh.GroupCount].ColorMap,
			mesh.Groups[mesh.GroupCount].LightMap,
			mesh.Groups[mesh.GroupCount].RenderBase, 
			mesh.Groups[mesh.GroupCount].RenderCount);
			

			fprintf(of,"\n");

			
			// dump
			fprintf(of,"\t\tshader[%d]: '%s' (%d %d)\n",
			shaderIndex,MFnDependencyNode(shaders[shaderIndex]).name().asChar(),
			mesh.Groups[shaderIndex].RenderBase, mesh.Groups[shaderIndex].RenderCount);
			
		}

		base = mesh.TriCount;
	}

	for (int i=0; i<mesh.GroupCount; ++i) {
		const ShadingGroup& group = mesh.Groups[i];

		fprintf(of,"\t\tgroup[%d]: (%d %d)\n",i,group.RenderBase,group.RenderCount);
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
		fprintf(of,"WARNING: mesh '%s' has no shading groups, skipping\n",mesh.Name);
		return;
	}

	MeshOffsets[MeshCount++] = ftell(f);

	float dpos[3];
	memset(dpos,0,sizeof(dpos));

	fprintf( of, "Before Stripify() - VertCount = %d | TriCount = %d | IndexCount = %d | GroupCount = %d\n", mesh.VertCount, mesh.TriCount, mesh.IndexCount, mesh.GroupCount);

    if ( mesh.UseTriStrips )
    {
        //Stripify();
	    NvidiaStripify();
    }
    else
    {
        fprintf(of, "NOSTRIP");
    }

    //if ( format == ePSPMap )
    //    SavePSPMesh();
    //else if ( format == ePS2Map )
    //    SaveMesh();
    //else
	SaveMesh();
	//SaveNonIndexedMesh();


    fprintf( of, "=================================================\n");
    fprintf( of, "== Results:\n");
    fprintf( of, "==\n");
    fprintf( of, "==  g_total_num_verts   = %d\n", g_total_num_verts);
    fprintf( of, "==  g_total_num_indices = %d\n", g_total_num_indices);
    fprintf( of, "==  g_total_num_strips  = %d\n", g_total_num_strips);
    //fprintf( of, "==  g_total_num_prestrips  = %d\n", GetTotalNumPreStrips());
    fprintf( of, "==\n");
    fprintf( of, "=================================================\n");
	fprintf( of, "DONE PRINTING\n");
	//SavePSPMesh();
}


static void DeIndexData( Mesh &mesh )
{
	Vertex*			NonIndexedVerts;

	if ( mesh.IndexCount > MAX_VERTEX_COUNT )
	{
		MGlobal::displayError("Too many verts when de-indexed!\n");
		return;
	}
	else
	{
		// Create the buffer...
		NonIndexedVerts = (Vertex*)malloc( mesh.IndexCount * sizeof( Vertex ) );

		// Loop through the groups...
		int limit = 0;
		int count = 0;
		for ( int gi=0; gi < mesh.GroupCount; ++gi )
		{
			// Update the group's RenderBase...
			mesh.Groups[gi].RenderBase = count;

			limit = mesh.Groups[gi].RenderBase + (mesh.UseTriStrips ? (mesh.Groups[gi].RenderCount+2) : (mesh.Groups[gi].RenderCount*3));
			Assert( limit < mesh.IndexCount );
			Assert( count < mesh.IndexCount );
			for ( int i=mesh.Groups[gi].RenderBase; i < limit; ++i )
			{
				NonIndexedVerts[count++] =  mesh.Verts[mesh.Indices[i]];
			}
		}
		Assert( count == mesh.IndexCount );

		// Fill the mesh's vert buffer with goodness...
		memcpy( mesh.Verts, NonIndexedVerts, sizeof( Vertex ) * mesh.IndexCount );
		free( NonIndexedVerts );

		// ...square off the number of vertices...
		mesh.VertCount = mesh.IndexCount;
		mesh.IndexCount = 0;
	}

	fprintf(of, "Number of Verts AFTER De-Indexing = %d\n", mesh.VertCount);
}

static void SaveMesh()
{
	char name[256];

	strcpy(name,mesh.Name.asChar());
	strlwr(name);

	fprintf(of,"\tSAVE: \"%s\": [%d] V:%d E:%d T:%d G:%d\n\n",name,
		mesh.UVSetCount,mesh.VertCount,mesh.IndexCount,mesh.TriCount,mesh.GroupCount);

	// sanity magic and mesh name
	Write((DWORD) 0xdeafbabe);
	Write(name);

	// mesh transform
	fwrite(mesh.TransformMatrix,sizeof(Mat4),1,f);

    // If we are dealing with Tri-strips, save the information
    int numUVSets = mesh.UVSetCount;
    if ( mesh.UseTriStrips )
    {
        numUVSets |= MESH_TRISTRIP_BIT;
    }

	// Send the message that this mesh is de-indexed.
	numUVSets |= MESH_DEINDEXED_BIT;

	// Send the message that this mesh includes the normals stitched in...
	//numUVSets |= MESH_NORMAL_INCLUSIVE_BIT;

    // We're writing out the numUVSets variable, because it's altered, and will
    // not jive with the the if-statement check down below, so we'll keep UVSetCount
    // pure.
    Write(numUVSets);

    if ( DeIndexMeshes )
    {
	    // Do our deindexing...
	    DeIndexData( mesh );
    }

	// Save number of de-indexed vertices
	Write(mesh.VertCount);

    //g_total_num_verts += NonIndexedVertCount;

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

			//fprintf(of, "POS = %f %f %f\n", d[0], d[1], d[2]);
			//fprintf(of, "NML = %f %f %f\n", d[3], d[4], d[5]);
			//fprintf(of, "TEX = %f %f\n", d[6], d[7]);

			// save 
			fwrite(&d,sizeof(float),8,f);
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

			float d[10];
			memset(d,0,sizeof(d));

			// pos
			for (int j=0; j<3; ++j)
				d[j] = (float) mesh.Points[vertex.vidx][j];

			// normals
			if (vertex.nidx != -1)
				for (int j=0; j<3; ++j)
					d[3+j] = (float) mesh.Normals[vertex.nidx][j];

			// uv 1
			if (vertex.tidx[0] != -1) {
				//if (vertex.tidx[0] >= mesh.UVSets[0].U.length())
				//    fprintf(of,"BOGUS: vertex[%d]: tidx[0] = %d (%d)\n",
				//        i,vertex.tidx[0],mesh.UVSets[0].U.length());

				d[6] = mesh.UVSets[0].U[vertex.tidx[0]];
				d[7] = 1.0f - mesh.UVSets[0].V[vertex.tidx[0]];
			}

			// uv 2
			if (vertex.tidx[1] != -1) {
				//if (vertex.tidx[1] >= mesh.UVSets[1].U.length())
				//    fprintf(of,"BOGUS: vertex[%d]: tidx[1] = %d\n",
				//        i,vertex.tidx[1],mesh.UVSets[1].U.length());
				d[8] = mesh.UVSets[1].U[vertex.tidx[1]];
				d[9] = 1.0f - mesh.UVSets[1].V[vertex.tidx[1]];
			}

			//fprintf(of, "POS = %f %f %f\n", d[0], d[1], d[2]);
			//fprintf(of, "TEX1 = %f %f\n", d[4], d[5]);
			//fprintf(of, "TEX2 = %f %f\n", d[6], d[7]);

			// save 
			fwrite(&d,sizeof(float),8,f);
			BoxAdd(mesh.Mins,mesh.Maxs,d);
		}


        /*
		// save normals separately
		Write(mesh.VertCount);
		for (int i=0; i<mesh.VertCount; ++i) 
        {
			const Vertex& vertex = mesh.Verts[i];
			float n[3] = {0,0,0};

			if (vertex.nidx != -1)
				for (int j=0; j<3; ++j)
					n[j] = (float) mesh.Normals[vertex.nidx][j];

			fwrite(&n,sizeof(float),3,f);
		}
        */
	}

	// save box
	Write(mesh.Mins);
	Write(mesh.Maxs);

    // Since we're de-indexed, we will write the index count 
	// as zero...
	Write(mesh.IndexCount);

    //g_total_num_indices += mesh.IndexCount;

	if ( mesh.IndexCount )
		fwrite(&mesh.Indices[0],sizeof(WORD),mesh.IndexCount,f);

	// save groups
	Write(mesh.GroupCount);
	fprintf(of, "GroupCount = %d\n", mesh.GroupCount );
	for (int i=0; i<mesh.GroupCount; ++i) {
		const ShadingGroup& group = mesh.Groups[i];

        // Write out the starting point, and the number of primitives for the group
        Write((WORD) group.RenderBase);
		Write((WORD) group.RenderCount);

		fprintf(of, "Group[%d].RenderBase = %d\n", i, group.RenderBase );
		fprintf(of, "Group[%d].RenderCount = %d\n", i, group.RenderCount );
		fprintf(of, "Group[%d].VertexMin = %d\n", i, group.VertexMin );
		fprintf(of, "Group[%d].VertexCount = %d\n", i, group.VertexCount );

		Write(group.ColorMap);
		Write(group.LightMap);
		Write(group.BlendMap);
		Write(group.AlphaMap);
	}
}

/*
static void SaveMesh()
{
	char name[256];

	strcpy(name,mesh.Name.asChar());
	strlwr(name);

	fprintf(of,"\tSAVE: \"%s\": [%d] V:%d E:%d T:%d G:%d\n\n",name,
		mesh.UVSetCount,mesh.VertCount,mesh.IndexCount,mesh.TriCount,mesh.GroupCount);

	// sanity magic and mesh name
	Write((DWORD) 0xdeafbabe);
	Write(name);

	// mesh transform
	fwrite(mesh.TransformMatrix,sizeof(Mat4),1,f);

	// number of uv sets
    // If we are dealing with Tri-strips, save the information
    int numUVSets = mesh.UVSetCount;
    if ( mesh.UseTriStrips )
    {
        numUVSets |= 0x80000000;
    }

	// Send the message that this mesh includes the normals stitched in...
	numUVSets |= 0x20000000;

    // We're writing out the numUVSets variable, because it's altered, and will
    // not jive with the the if-statement check down below, so we'll keep UVSetCount
    // pure.
    Write(numUVSets);
	//Write(mesh.UVSetCount);

	// create and save vertices
	Write(mesh.VertCount);

    g_total_num_verts += mesh.VertCount;

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


			//fprintf(of, "POS = %f %f %f\n", d[0], d[1], d[2]);
			//fprintf(of, "NML = %f %f %f\n", d[3], d[4], d[5]);
			//fprintf(of, "TEX = %f %f\n", d[6], d[7]);


			// save 
			fwrite(&d,sizeof(float),8,f);
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

			float d[10];
			memset(d,0,sizeof(d));

			// pos
			for (int j=0; j<3; ++j)
				d[j] = (float) mesh.Points[vertex.vidx][j];

			// normals
			if (vertex.nidx != -1)
				for (int j=0; j<3; ++j)
					d[3+j] = (float) mesh.Normals[vertex.nidx][j];

			// uv 1
			if (vertex.tidx[0] != -1) {
				//if (vertex.tidx[0] >= mesh.UVSets[0].U.length())
				//    fprintf(of,"BOGUS: vertex[%d]: tidx[0] = %d (%d)\n",
				//        i,vertex.tidx[0],mesh.UVSets[0].U.length());

				d[6] = mesh.UVSets[0].U[vertex.tidx[0]];
				d[7] = 1.0f - mesh.UVSets[0].V[vertex.tidx[0]];
			}

			// uv 2
			if (vertex.tidx[1] != -1) {
				//if (vertex.tidx[1] >= mesh.UVSets[1].U.length())
				//    fprintf(of,"BOGUS: vertex[%d]: tidx[1] = %d\n",
				//        i,vertex.tidx[1],mesh.UVSets[1].U.length());
				d[8] = mesh.UVSets[1].U[vertex.tidx[1]];
				d[9] = 1.0f - mesh.UVSets[1].V[vertex.tidx[1]];
			}

			//fprintf(of, "POS = %f %f %f\n", d[0], d[1], d[2]);
			//fprintf(of, "TEX1 = %f %f\n", d[4], d[5]);
			//fprintf(of, "TEX2 = %f %f\n", d[6], d[7]);

			// save 
			fwrite(&d,sizeof(float),8,f);
			BoxAdd(mesh.Mins,mesh.Maxs,d);
		}
	}

	// save box
	Write(mesh.Mins);
	Write(mesh.Maxs);

	// save indices
	
	//for (int i=0; i<mesh.TriCount; ++i) {
	//mesh.Indices[i*3+0] = mesh.Indices[i*3+2];
	//mesh.Indices[i*3+1] = mesh.Indices[i*3+1];
	//mesh.Indices[i*3+2] = mesh.Indices[i*3+0];
	//}
	

    // Write out the 
	Write(mesh.IndexCount);

    g_total_num_indices += mesh.IndexCount;

	fwrite(&mesh.Indices[0],sizeof(WORD),mesh.IndexCount,f);

	// save groups
	Write(mesh.GroupCount);
	fprintf(of, "GroupCount = %d\n", mesh.GroupCount );
	for (int i=0; i<mesh.GroupCount; ++i) {
		const ShadingGroup& group = mesh.Groups[i];

        // Write out the starting point, and the number of primitives for the group
        Write((WORD) group.RenderBase);
		Write((WORD) group.RenderCount);

		fprintf(of, "Group[%d].RenderBase = %d\n", i, group.RenderBase );
		fprintf(of, "Group[%d].RenderCount = %d\n", i, group.RenderCount );
		fprintf(of, "Group[%d].VertexMin = %d\n", i, group.VertexMin );
		fprintf(of, "Group[%d].VertexCount = %d\n", i, group.VertexCount );

		Write(group.ColorMap);
		Write(group.LightMap);
		Write(group.BlendMap);
		Write(group.AlphaMap);
	}
}
*/

#include "tri_stripper/stdafx.h"
#include "tri_stripper/tri_stripper.h"

class tri_stripper;

static DynamicArray<WORD>	Elems;

//#include "NvTriStrip/NvTriStrip.h"


void NvidiaStripify(void)
{
	DynamicArray<ShadingGroup> ResultGroups;
    DynamicArray<WORD> ResultElems;

	for (int i=0; i<mesh.GroupCount; ++i) 
    {
		ShadingGroup& group = mesh.Groups[i];

        // Blair - unsigned ibse = group.Start;
        // Blair - unsigned icnt = group.Count * 3;
        unsigned ibse = group.RenderBase;
        unsigned icnt = group.RenderCount * 3;

		// USING NVIDIA STRIPPER
		unsigned short NbGroups;
		PrimitiveGroup* pPrimitiveGroup = NULL;

		const int numIndices = icnt;
		unsigned short* indices;

		indices = (unsigned short*)malloc( icnt * sizeof(short));
		for (unsigned e=0; e<icnt; e++) 
		{
            indices[e] = (short)mesh.Indices[ibse+e];
		}

		SetCacheSize(0);
		SetStitchStrips(true);
		SetMinStripSize(0);

		GenerateStrips( indices, icnt, &pPrimitiveGroup, &NbGroups);

		for (size_t i = 0; i < NbGroups; ++i) 
		{
            g_total_num_strips++;

            fprintf(of, "#### Number of Groups = %d | %uh \n", NbGroups, NbGroups );
            unsigned ecnt = pPrimitiveGroup[i].numIndices;
            if ( ecnt > 0 )
            {
                ShadingGroup ngrp(group);
                ngrp.RenderBase = ResultElems.Size();
                ngrp.RenderCount = ecnt-2;
                ResultGroups.Add(ngrp);
				//submesh::indices( pPrimitiveGroup[i].indices + 0, pPrimitiveGroup[i].indices + pPrimitiveGroup[i].numIndices ),
			    for ( int j=0; j < pPrimitiveGroup[i].numIndices; ++j )
			    {
				    fprintf( of, "pPrimitiveGroup[%d].index[%d] = %d\n", i, j, pPrimitiveGroup[i].indices[j] );
                    ResultElems.Add(pPrimitiveGroup[i].indices[j]);
			    }
            }
		}
        //ResultElems.Add(pPrimitiveGroup[i].indices[j])
        free( indices );
	}

    for(int i=0; i<ResultGroups.Size(); i++)
	{		
		mesh.Groups[i] = ResultGroups[i];
	}

    Elems = ResultElems;

    fprintf(of, "#*#*#*#*# ResultElems SIZE = %d\n", ResultElems.Size() );
    mesh.IndexCount = ResultElems.Size();
    for ( int i=0; i<ResultElems.Size(); ++i )
	{
		fprintf(of, "ResultElems[%d] = %d\n", i, ResultElems[i]);
		mesh.Indices[i] = ResultElems[i];
	}

    Init();
}


void Stripify(void)
{
	DynamicArray<ShadingGroup> ResultGroups;
    DynamicArray<WORD> ResultElems;

	for (int i=0; i<mesh.GroupCount; ++i) 
    {

		ShadingGroup& group = mesh.Groups[i];

        //unsigned ibse = group.Start;
        //unsigned icnt = group.Count * 3;
        unsigned ibse = group.RenderBase;
        unsigned icnt = group.RenderCount * 3;

		fprintf(of, "STRIPIFY - ibse = %u\n", ibse);
		fprintf(of, "STRIPIFY - icnt = %u\n", icnt);
		
		  // Build index list for group
        triangle_stripper::tri_stripper::indices indices(icnt);
        
		for (unsigned e=0; e<icnt; e++) {

            indices[e] = mesh.Indices[ibse+e];

		}

		// Initialize stripper
        triangle_stripper::tri_stripper stripper(indices);

        stripper.SetCacheSize(0);

        stripper.SetMinStripSize(2);

		  // Perform stripping
        triangle_stripper::tri_stripper::primitives_vector prims;

		fprintf(of, "tri_stripper::Stripify() ENTER!\n");
		stripper.Strip(&prims);
		fprintf(of, "tri_stripper::Stripify() EXIT!\n");


		fprintf(of, "## prims.size = %d \n", prims.size());

#ifdef STITCH
		ShadingGroup ngrp(group);

		ngrp.RenderBase = ResultElems.Size();

		int stitch_c = 0, stitch_i = -1;

#endif

        //g_total_num_strips += prims.size();

        // Process each resulting tristrip/trilist
        for (unsigned p=0; p<prims.size(); p++)
        {

             triangle_stripper::tri_stripper::primitives& prim = prims[p];
             unsigned ecnt = prim.m_Indices.size();

			 fprintf( of, "-- prim[%d].m_Indices.size() = %d", p, prim.m_Indices.size());

             if (prim.m_Type == triangle_stripper::tri_stripper::PT_Triangle_Strip) 
             {
                 g_total_num_strips++;
                 // It's a tristrip, yay
#ifndef STITCH
                 ShadingGroup ngrp(group);
                 ngrp.RenderBase = ResultElems.Size();
                ngrp.RenderCount = ecnt-2;
                 ResultGroups.Add(ngrp);
#else
                if (stitch_c) 
                {
					//fprintf( of, "@@ START FIRST STITCH\n");
					//fprintf( of, "@@ if ( stitch_c ) // stitch_c --> = %d\n", stitch_c);
                     for (unsigned e=0; e<stitch_c; e++)
					 {//blair
						//fprintf( of, "@@ for (unsigned e=0; %d<%d; %d++)\n", e, stitch_c, e);
						//fprintf( of, "@@ ResultElems.Add(%d) // <--- stitch_i\n", stitch_i);
                         ResultElems.Add(stitch_i);
					 }//blair
					 //fprintf( of, "@@ ResultElems.Add(%d)\n", prim.m_Indices[0]);
                     ResultElems.Add(prim.m_Indices[0]);
				}
					//fprintf( of, "@@ END FIRST STITCH\n");
#endif
				  for (unsigned e=0; e<ecnt; e++)
                     ResultElems.Add(prim.m_Indices[e]);
#ifdef STITCH
                 stitch_c = 1 + (ecnt & 1);
                 stitch_i = prim.m_Indices[ecnt-1];
#endif
             } 
			 else {


 // It's a list of stray triangles
                 // Since we can't mix tristrips and trilists,
                 // we must convert each triangle in the list to its
                 // own single-triangle tristrip, unless we stitch them
                 unsigned tcnt = ecnt/3;
				for (unsigned t=0; t<tcnt; t++) {
                    g_total_num_strips++;
#ifndef STITCH
                     ShadingGroup ngrp(group);
                     ngrp.RenderBase = ResultElems.Size();
                     ngrp.RenderCount = 1;
                     ResultGroups.Add(ngrp);
#else

                     if (stitch_c) {

                         for (unsigned e=0; e<stitch_c; e++)
						 {
                             ResultElems.Add(stitch_i);
						 }
                         ResultElems.Add(prim.m_Indices[t*3+0]);

                     }
#endif
                     ResultElems.Add(prim.m_Indices[t*3+0]);

                     ResultElems.Add(prim.m_Indices[t*3+1]);

                     ResultElems.Add(prim.m_Indices[t*3+2]);

#ifdef STITCH
                     // it'd probably be great if we could stitch
                     // some of these triangles with less than
                     // 3 dummy vertices, but it's not trivial...

                     stitch_c = 2;

                     stitch_i = prim.m_Indices[t*3+2];
#endif
                 
					 }
				}
		  }
#ifdef STITCH


        ngrp.RenderCount = ResultElems.Size() - ngrp.RenderBase - 2;

        if (ngrp.RenderCount > 0)
		{
            ResultGroups.Add(ngrp);
		}
#endif

	}  
			// Install stripified mesh
				 //Triangles.Clear();

    for(int i=0; i<ResultGroups.Size(); i++)
	{		
		mesh.Groups[i] = ResultGroups[i];
	}

    Elems = ResultElems;

	mesh.IndexCount = ResultElems.Size();
	
	for ( int i=0; i<ResultElems.Size(); ++i )
	{
		fprintf(of, "ResultElems[%d] = %d\n", i, ResultElems[i]);
		mesh.Indices[i] = ResultElems[i];
	}

	 // Recalculate vertex ranges
    Init();


}


void Init()
{
	//fprintf(of, "## Init - Enter\n");
	DynamicArray<WORD> *IndexArr; 

	if (Elems.Size()) {
		IndexArr = &Elems;
	}
//	else if (Triangles.Size()) {
	//	IndexArr = &Triangles;
//	}
	else 
		return;

	for (int i=0; i<mesh.GroupCount; i++)
	{
		int emin = mesh.Groups[i].RenderBase;
        int emax = (i+1==mesh.GroupCount) ? (*IndexArr).Size() : mesh.Groups[i+1].RenderBase;
		int vmin = mesh.VertCount;
		int vmax = 0;

		for (int e=emin; e<emax; e++)
		{
			if (vmin > (*IndexArr)[e])
				vmin = (*IndexArr)[e];
			if (vmax < (*IndexArr)[e])
				vmax = (*IndexArr)[e];
		}

		//fprintf(of, "## Init - mesh.Groups[%d].VertexMin = %d -> ", i, mesh.Groups[i].VertexMin);
		mesh.Groups[i].VertexMin   = vmin;
		//fprintf(of, "%d\n", mesh.Groups[i].VertexMin);
		//fprintf(of, "## Init - mesh.Groups[%d].VertexCount = %d -> ", i, mesh.Groups[i].VertexCount);
		mesh.Groups[i].VertexCount = vmax-vmin+1;
		//fprintf(of, "%d\n", mesh.Groups[i].VertexCount);
	}
	//fprintf(of, "## Init - Exit\n");
}

static void FixTextureCoords( Mesh mesh )
{
/*
   TexCoord    min, max;
   TexCoord    range;
   int         x;
   int         y;

   min.X = min.Y = 2048.0f;
   max.X = max.Y = -2048.0f;

   //int uvSetCount = mesh.UVSetCount;

   if (mesh.UVSetCount == 1)
   {
       for (int i = 0; i < mesh.VertCount; i++)
       {
           const Vertex& vertex = mesh.Verts[i];
           float tex_X = mesh.UVSets[0].U[vertex.tidx[0]];
           float tex_Y = 1.0f - mesh.UVSets[0].V[vertex.tidx[0]];
           if (tex_X < min.X)  min.X = tex_X;
           if (tex_Y < min.Y)  min.Y = tex_Y;
           if (tex_X > max.X)  max.X = tex_X;
           if (tex_Y > max.Y)  max.Y = tex_Y;
       }

       range.X = max.X - min.X;
       range.Y = max.Y - min.Y;

       if (range.X < 16.0f && range.Y < 16.0f)
       {
           x = (int)((range.X * 0.5f) + min.X);
           y = (int)((range.Y * 0.5f) + min.Y);

           for (int i = 0; i < mesh.VertCount; i++)
           {
               const Vertex& vertex = mesh.Verts[i];
               //float tex_X = mesh.UVSets[0].U[vertex.tidx[0]];
               //float tex_Y = 1.0f - mesh.UVSets[0].V[vertex.tidx[0]];
               mesh.UVSets[0].U[vertex.tidx[0]] -= x;
               mesh.UVSets[0].V[vertex.tidx[0]] -= y; // ** BLAIR ** - should this be tidx[1]?
           }
       }

       else
       {
           //TRACE("WorldMesh::FixTextureCoords(): WARNING-> the texture coordinate range on '%s' is too large!  Attempting to rescale... (fmt = TU1, range = <%g, %g>)\n", mesh.Name.AsChar(), range.X, range.Y);

           // BLAIR - if (PrimType == RenderDevice::EPrimTriList)
           if ( !mesh.UseTriStrips )
           {
               DynamicArray<TexRange> tc(mesh.VertCount);

               //initialize the neighbour ranges
               for (int i = 0; i < mesh.VertCount; i++)
               {
                   tc[i].min.X = tc[i].min.Y = 2048.0f;
                   tc[i].max.X = tc[i].max.Y = -2048.0f;
               }

               //loop through the index and find the range for each vertex
               for (int i = 0; i < mesh.IndexCount; i += 3)
               {
                   //tc[Triangles[i + 0]].updateRange(Verts[Triangles[i + 0]].TU);
                   tc[mesh.Indices[i + 0]].updateRange( TexCoord(mesh.UVSets[0].U[ mesh.Verts[mesh.Indices[i + 0]].tidx[0]));
                   tc[Triangles[i + 0]].updateRange(Verts[Triangles[i + 1]].TU);
                   tc[Triangles[i + 0]].updateRange(Verts[Triangles[i + 2]].TU);

                   tc[Triangles[i + 1]].updateRange(Verts[Triangles[i + 0]].TU);
                   tc[Triangles[i + 1]].updateRange(Verts[Triangles[i + 1]].TU);
                   tc[Triangles[i + 1]].updateRange(Verts[Triangles[i + 2]].TU);

                   tc[Triangles[i + 2]].updateRange(Verts[Triangles[i + 0]].TU);
                   tc[Triangles[i + 2]].updateRange(Verts[Triangles[i + 1]].TU);
                   tc[Triangles[i + 2]].updateRange(Verts[Triangles[i + 2]].TU);
               }

               //loop through the verticies and update the
               for (int i = 0; i < mesh.VertCount; i++)
               {
                   x = (int)(((tc[i].max.X - tc[i].min.X) * 0.5f) + tc[i].min.X);
                   y = (int)(((tc[i].max.Y - tc[i].min.Y) * 0.5f) + tc[i].min.Y);

                   Verts[i].TU.X -= x;
                   Verts[i].TU.Y -= y;
               }
           }

           //else if (PrimType == RenderDevice::EPrimTriStrip)
           //{
           //    TRACE("WorldMesh::FixTextureCoords(): WARNING-> texcoords fixup isn't implemented for tristrips! (name = '%s', fmt = TU1)\n", Name.AsChar());
           //}
       }
   }

   else
   {
       for (int i = 0; i < mesh.VertCount; i++)
       {
           if (Verts[i].TU1.X < min.X)  min.X = Verts[i].TU1.X;
           if (Verts[i].TU1.Y < min.Y)  min.Y = Verts[i].TU1.Y;
           if (Verts[i].TU1.X > max.X)  max.X = Verts[i].TU1.X;
           if (Verts[i].TU1.Y > max.Y)  max.Y = Verts[i].TU1.Y;
       }

       range.X = max.X - min.X;
       range.Y = max.Y - min.Y;

       if (range.X < 16.0f && range.Y < 16.0f)
       {
           x = (int)((range.X * 0.5f) + min.X);
           y = (int)((range.Y * 0.5f) + min.Y);

           for (int i = 0; i < mesh.VertCount; i++)
           {
               Verts[i].TU1.X -= x;
               Verts[i].TU1.Y -= y;
           }
       }

       else
       {
           //TRACE("WorldMesh::FixTextureCoords(): ERROR-> the texture coordinate range on '%s' is too large!  Attempting to rescale... (fmt = TU2, range = <%g, %g>)\n", Name.AsChar(), range.X, range.Y);

           if (PrimType == RenderDevice::EPrimTriList)
           {
               DynamicArray<TexRange> tc(mesh.);


               //TRACE("WorldMesh::FixTextureCoords(): verts.size() = %d, tc.size() = %d\n", Verts.Size(), tc.Size());

               //initialize the neighbour ranges
               for (int i = 0; i < Verts.Size(); i++)
               {
                   tc[i].min.X = tc[i].min.Y = 2048.0f;
                   tc[i].max.X = tc[i].max.Y = -2048.0f;
               }


               TRACE("WorldMesh::FixTextureCoords(): finding coordinate range for %d triangles\n", Triangles.Size());
               //loop through the index and find the range for each vertex
               for (int i = 0; i < Triangles.Size(); i += 3)
               {
                   tc[Triangles[i + 0]].updateRange(Verts[Triangles[i + 0]].TU1);
                   tc[Triangles[i + 0]].updateRange(Verts[Triangles[i + 1]].TU1);
                   tc[Triangles[i + 0]].updateRange(Verts[Triangles[i + 2]].TU1);

                   tc[Triangles[i + 1]].updateRange(Verts[Triangles[i + 0]].TU1);
                   tc[Triangles[i + 1]].updateRange(Verts[Triangles[i + 1]].TU1);
                   tc[Triangles[i + 1]].updateRange(Verts[Triangles[i + 2]].TU1);

                   tc[Triangles[i + 2]].updateRange(Verts[Triangles[i + 0]].TU1);
                   tc[Triangles[i + 2]].updateRange(Verts[Triangles[i + 1]].TU1);
                   tc[Triangles[i + 2]].updateRange(Verts[Triangles[i + 2]].TU1);
               }

               TRACE("WorldMesh::FixTextureCoords(): updating coordinates for %d verts\n", Verts.Size());
               //loop through the verticies and update the
               for (int i = 0; i < Verts.Size(); i++)
               {
                   x = (int)(((tc[i].max.X - tc[i].min.X) * 0.5f) + tc[i].min.X);
                   y = (int)(((tc[i].max.Y - tc[i].min.Y) * 0.5f) + tc[i].min.Y);

                   TRACE(" %4d) <x, y> = <%d, %d>, min = <%g, %g>, max = <%g, %g>, dT = <%g, %g> -> <%g, %g>\n", i, x, y, tc[i].min.X, tc[i].min.Y, tc[i].max.X, tc[i].max.Y, Verts[i].TU1.X, Verts[i].TU1.Y, Verts[i].TU1.X - x, Verts[i].TU1.Y - y);
                   Verts[i].TU1.X -= x;
                   Verts[i].TU1.Y -= y;
               }
           }

           else if (PrimType == RenderDevice::EPrimTriStrip)
               TRACE("WorldMesh::FixTextureCoords(): ERROR-> texcoords fixup isn't implemented for tristrips! (name = '%s', fmt = TU2)\n", Name.AsChar());
       }
   }
*/
}







