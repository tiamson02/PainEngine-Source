#define _BOOL
#define TRUE_AND_FALSE_DEFINED

#if _MSC_VER < 1300
#define for if(0) {} else for
#endif // _MSC_VER

#include <cstdio>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>


#include <maya/MStatus.h>
#include <maya/MIntArray.h>
#include <maya/MFloatPointArray.h>
#include <maya/MPointArray.h>
#include <maya/MObject.h>
#include <maya/MGlobal.h>
#include <maya/MTime.h>
#include <maya/MColor.h>
#include <maya/MMatrix.h>
#include <maya/MObjectArray.h>
#include <maya/MAnimControl.h>
#include <maya/MAnimUtil.h>
#include <maya/MSelectionList.h>
#include <maya/MComputation.h>
#include <maya/MPxFileTranslator.h>

#include <maya/MItDag.h>
#include <maya/MDagPath.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnMesh.h>
#include <maya/MFnLight.h>
#include <maya/MFnAnimCurve.h>
#include <maya/MFnTransform.h>

#include <maya/MItDependencyGraph.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MItDependencyNodes.h>

#include <maya/MItMeshPolygon.h>

#include "WorldExporter.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <math.h>

#include "Base/Base.h"
#include "Engine/Engine.h"

// ================================================================

class WorldExporter
{
public:
	WorldExporter();
	~WorldExporter();

	bool Open(const char* FileName);
	void Close();
	MStatus Export(const MString& options,MPxFileTranslator::FileAccessMode mode);

private:
	MStatus ProcessAll();
	MStatus ProcessSelected();
	void	Triangulate();

	MStatus OutputFile();
	MStatus ProcessMesh(MFnMesh& mesh);
	MStatus ProcessShader(MObject& shader, String& colorTex, String& lmapTex, String& nmapTex);

    MStatus ProcessZone(MFnMesh& mesh);
    MStatus ProcessPortal(MFnMesh& mesh);
    MStatus ProcessNode(MDagPath& dagPath);

private:
    ObjectPackage*          Package;

	std::vector<MDagPath>	MeshNodes;
    std::vector<MDagPath>   LightNodes;

    BOOL    Stripify;
	MStringArray  nodesArrayForTri;

    Matrix  ObjectToWorld;

    // stats
	int TotalMeshes;
	int TotalPortals;
	int TotalZones;
	int TotalVerts;
	int TotalTris;
	int TotalElems;

};

static std::ofstream o("PCFWorldExport.log");

// =======================================================

MStatus ExportWorld(const MFileObject& file,
                    const MString& options,
                    MPxFileTranslator::FileAccessMode mode)
{
	WorldExporter WorldExporter;
	if(WorldExporter.Open(file.fullName().asChar())) {
		return WorldExporter.Export(options,mode);
	}

	std::cerr<<"ERROR: could not open output file:"<<file.fullName().asChar()<<std::endl;
	return MS::kFailure;
}

// =========================================================

WorldExporter::WorldExporter()
:	Package(NULL), Stripify(FALSE)
{
}

WorldExporter::~WorldExporter()
{
	Close();
}

bool WorldExporter::Open(const char* FileName)
{
	Package = new ObjectPackage(FileName);
    return Package != NULL;
}

void WorldExporter::Close()
{
	if (Package)
	{
		delete Package;
		Package = NULL;
	}
}

MStatus WorldExporter::Export(const MString& options,
							   MPxFileTranslator::FileAccessMode mode)
{
	if(options.length() > 0) {
		// parse options
	}

	TotalMeshes=0;
	TotalPortals=0;
	TotalZones=0;
	TotalVerts=0;
	TotalTris=0;
	TotalElems=0;

	std::cerr<<std::endl;
	std::cerr<<"***********************************************"<<std::endl;
	std::cerr<<"                      START"<<std::endl;
	std::cerr<<"***********************************************"<<std::endl;
	std::cerr<<std::endl;
	//std::cerr<<"PREPROCESS ... => triangulating...."<<std::endl;

	MStatus status=MS::kFailure;
    if((mode == MPxFileTranslator::kExportAccessMode) ||
       (mode == MPxFileTranslator::kSaveAccessMode))
    {
        try {
            status=ProcessAll();
        }
        catch(...) {
            std::cerr<<"SHIT HIT THE FAN!!!"<<std::endl;
            return MS::kFailure;
        }
    }
    else 
	if(mode == MPxFileTranslator::kExportActiveAccessMode)
    {
        status=ProcessSelected();
    }

	if(MS::kSuccess == status)
		status=OutputFile();

#if 0
	// untriangulate
	if (nodesArrayForTri.length() != 0)
	{
		MString command = "delete";
		for (unsigned int i = 0; i < nodesArrayForTri.length(); i++)
		{
			command += " " + nodesArrayForTri[i];

		}

		MGlobal::executeCommand(command, 1 , 1);
	}
#endif

	return status;
}

void WorldExporter::Triangulate()
{
	MString command = "tri 1";
	MGlobal::executeCommand(command, nodesArrayForTri, 1, 0);
	std::cerr<<"triangulated " << (nodesArrayForTri.length() )<< " meshes"<<std::endl;
	std::cerr<<std::endl;
	std::cerr<<std::endl;
	std::cerr<<"Process Selected"<<std::endl;
}

MStatus WorldExporter::ProcessAll()
{
	o<<"BEGIN ProcessAll()"<<std::endl;

	MeshNodes.clear();
	MStatus status = MS::kSuccess;
	MItDag dagIterator(MItDag::kBreadthFirst, MFn::kInvalid, &status);

	if(MS::kSuccess != status) {
		std::cerr<<"ERROR: Failure in DAG iterator setup"<<std::endl;
	    return MS::kFailure;
	}

	//Triangulate();

	for(; !dagIterator.isDone(); dagIterator.next()) {
		MDagPath dagPath;
		status = dagIterator.getPath(dagPath);

        //std::cerr<<"dagPath:"<<dagPath.fullPathName().asChar()<<std::endl;

		if (!status) {
			std::cerr<<"ERROR: failure getting DAG path"<<std::endl;
			return MS::kFailure;
		}

		// skip over intermediate objects
		MFnDagNode dagNode( dagPath, &status );
		if (dagNode.isIntermediateObject()) {
			continue;
		}

		if( (dagPath.hasFn(MFn::kNurbsSurface)) &&
			(dagPath.hasFn(MFn::kTransform)))
		{
			continue;
		}
		else 
		if(	(dagPath.hasFn(MFn::kMesh)) &&
			(dagPath.hasFn(MFn::kTransform)))
		{
			// We want only the shape, 
			// not the transform-extended-to-shape.
			continue;
		}
		else 
		if (dagPath.hasFn(MFn::kMesh)) 
		{
			//std::cerr<<"mesh:"<<dagPath.fullPathName().asChar()<<std::endl;
			//MeshNodes.push_back(dagPath);
			ProcessNode(dagPath);
		}
        else
        if(dagPath.hasFn(MFn::kLight)) {
            LightNodes.push_back(dagPath);
        }
	}

	o<<"END ProcessAll()"<<std::endl;
	return status;
}

MStatus WorldExporter::ProcessSelected()
{
	std::cerr<<"ProcessSelected()"<<std::endl;

	MeshNodes.clear();
	MStatus status = MS::kSuccess;

	MSelectionList sList;

	MGlobal::getActiveSelectionList( sList );
	if (sList.length() == 0)
	{
		std::cerr<<"Select sth!"<<std::endl;
		return MS::kFailure;
	}

	Triangulate();

	for ( unsigned int index = 0; index < sList.length(); index++ )
	{

		MDagPath dagPath;
		status =    sList.getDagPath(index, dagPath);

		if (!status) {
			std::cerr<<"failure getting DAG path"<<std::endl;
			return MS::kFailure;
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
		MFnDagNode dagNode( dagPath, &status );

		if (dagPath.hasFn(MFn::kMesh) &&  ! (dagNode.isIntermediateObject())) {
			//std::cerr<<"mesh : "<<dagPath.fullPathName().asChar()<<std::endl;
			//MeshNodes.push_back(dagPath);
            ProcessNode(dagPath);
		}
		else
		if(dagPath.hasFn(MFn::kLight)) {
			LightNodes.push_back(dagPath);
		}
	}

	std::cerr<<"END Selected()"<<std::endl;
	return MS::kSuccess;
}


static std::string GetObjectTypeName(const MObject& Object)
{
	if(!Object.isNull()) {
		MStatus stat = MS::kSuccess;
		MFnDependencyNode Node(Object);
		MString TypeName = Node.typeName(&stat);
		if(MS::kSuccess == stat) {
			return TypeName.asChar();
		}
	}

	return "<null>";
}

static std::string GetObjectName(const MObject& Object)
{
	if(!Object.isNull()) {
		MStatus stat = MS::kSuccess;
		MFnDependencyNode Node(Object);
		MString TypeName = Node.name(&stat);
		if(MS::kSuccess == stat) {
			return TypeName.asChar();
		}
	}

	return "<null>";
}

static size_t BaseName(const std::string& path, std::string& fileName)
{
    size_t i=path.find_last_of('\\');
    if(i==std::string::npos) {    
        i=path.find_last_of('/');
        if(i==std::string::npos) {
            fileName=path;
            return 0;
        }
    }

    fileName=path.substr(i+1);
    return i;
}

static MStatus GetTextureFileName(MPlug& plug, MString& textureFileName)
{	
    MStatus stat;

	MItDependencyGraph dgIt
    (
        plug, 
        MFn::kFileTexture, 
        MItDependencyGraph::kUpstream, 
        MItDependencyGraph::kBreadthFirst, 
        MItDependencyGraph::kNodeLevel, 
        &stat
    );

    if(MS::kSuccess == stat) {
			
	    dgIt.disablePruningOnFilter();
        if (dgIt.isDone()) {
            return MS::kFailure;	// no texture file node was found, so just continue.
        }
			  
		// get the texture file name
		
		MFnDependencyNode(dgIt.thisNode()).findPlug("fileTextureName").getValue(textureFileName);
    }

    return stat;
}

// ==================================================================================

MStatus WorldExporter::ProcessZone(MFnMesh& mesh)
{    
    MFloatPointArray points;
    if (MS::kSuccess == mesh.getPoints(points,MSpace::kWorld)) {

        Zone* zone = (Zone*) Package->CreateObject("Zone",mesh.partialPathName().asChar()+4);
        strlwr(&zone->Name[0]);

        zone->Box.Reset();
        for (unsigned int i=0; i<points.length(); ++i)           
            zone->Box+=Vector(points[i].x,points[i].y,points[i].z);

        std::cerr<<"Zone: '"<<zone->Name.AsChar()<<"'"
                 <<" ("<<zone->Box.Min.X<<","<<zone->Box.Min.Y<<","<<zone->Box.Min.Z<<")"
                 <<" ("<<zone->Box.Max.X<<","<<zone->Box.Max.Y<<","<<zone->Box.Max.Z<<")"
                 <<std::endl;       

        ++TotalZones;
        return MS::kSuccess;
    }
    return MS::kFailure;
}

// ==================================================================================

MStatus WorldExporter::ProcessPortal(MFnMesh& mesh)
{
    if (mesh.numPolygons()!=1) 
    {
        std::cerr<<"ERROR: portal has more than 1 poly, skipping"<<std::endl;
        return MS::kFailure;
    }
    
    MFloatPointArray points;
    if (MS::kSuccess == mesh.getPoints(points,MSpace::kWorld)) 
    {

        MIntArray vertexList;
        if (MS::kSuccess == mesh.getPolygonVertices(0,vertexList)) 
        {
    
            Portal* portal=(Portal*) Package->CreateObject("Portal",mesh.partialPathName().asChar());
            strlwr(&portal->Name[0]);

			portal->Box.Reset();
            for (unsigned int i=0; i<vertexList.length(); ++i) 
            {
                const MFloatPoint& pt=points[vertexList[i]];
				const Vector vertex(pt.x,pt.y,pt.z);

                portal->Poly.Verts[i] = vertex;
				portal->Box+=vertex;
            }

			portal->Poly.VertexCount=(int)vertexList.length();

            std::cerr<<"Portal: '"<<portal->Name.AsChar()<<"'"
                     <<" ("<<(int)portal->Poly.VertexCount<<" verts)"
                     <<std::endl;       

            ++TotalPortals;
            return MS::kSuccess;
        }
    }

    return MS::kFailure;
}

// ==================================================================================

static inline void GetObjectMatrix(MDagPath& dagPath, Matrix& outMatrix)
{
	MMatrix matrix=dagPath.inclusiveMatrix();

    for(int i=0; i<4; ++i) {
        for(int j=0; j<4; ++j) {
            outMatrix.mArray[i*4+j]=(float)matrix(i,j);
        }
    }
}

// ==================================================================================

static DynamicArray<WorldMesh*>	MasterMeshes;
static DynamicArray<String>		MasterMeshNames;

// ==================================================================================

struct UVSet
{
	MFloatArray	U;
	MFloatArray V;
};

struct MeshProxy
{
	DynamicArray<int>	Verts;
	DynamicArray<int>	Normals;
	DynamicArray<int>	TU1;
	DynamicArray<int>	TU2;

    DynamicArray<WorldMesh::Group>  Groups;

	WORD	Triangles[65536];
	int		TriangleNum;


	inline int Add1(int vertexIndex, int normalIndex, int uvIndex)
	{
		for (int i=0; i<Verts.Size(); ++i)
		{
			if (Verts[i] == vertexIndex && Normals[i] == normalIndex && TU1[i] == uvIndex)
				return i;
		}

		int index = Verts.Size();

		Verts.Add(vertexIndex);
		Normals.Add(normalIndex);
		TU1.Add(uvIndex);

		return index;
	}

	inline int Add2(int vertexIndex, int normalIndex, int uvIndex1, int uvIndex2)
	{
		for (int i=0; i<Verts.Size(); ++i)
		{
			if (Verts[i] == vertexIndex && Normals[i] == normalIndex && TU1[i] == uvIndex1 && TU2[i] == uvIndex2)
				return i;
		}

		int index = Verts.Size();

		Verts.Add(vertexIndex);
		Normals.Add(normalIndex);
		TU1.Add(uvIndex1);
		TU2.Add(uvIndex2);

		return index;
	}
};

// ==================================================================================

MStatus WorldExporter::ProcessShader(MObject& shader, String& colorTex, String& lmapTex, String& nmapTex)
{
	// get surfaceShader
    MStatus stat;
	MPlug mpShader = MFnDependencyNode(shader).findPlug("surfaceShader", &stat);

    if (!stat || mpShader.isNull()) 
	{
		o<<"ProcessShader(): ERROR: no surfaceShader, skippin"<<std::endl;
		return MStatus::kFailure;
	}

	// get connected plugs
	MPlugArray aPlugs;
	mpShader.connectedTo(aPlugs, true, false, &stat);

    if (!stat || aPlugs.length() != 1) 
	{
		o<<"ProcessShader(): ERROR: no plugs"<<std::endl;
		return MStatus::kFailure;     
	}

	o<<"\t\tShader type: ";
	switch (aPlugs[0].node().apiType())
	{ 
	case MFn::kLambert:
		o << "Lambert"; 
		break;
	case MFn::kBlinn:
		o << "Blinn"; 
		break;
	case MFn::kPhong:
		o << "Phong"; 
		break;
	}

	o<<std::endl;

	MPlug mpColor = MFnDependencyNode(aPlugs[0].node()).findPlug("color", &stat);
    if (!stat) 
	{
		o<<"ProcessShader(): ERROR: no color plug"<<std::endl;
		return MStatus::kFailure;     
	}

	MItDependencyGraph dgIt(mpColor, 
							MFn::kInvalid,
							MItDependencyGraph::kUpstream,
							MItDependencyGraph::kBreadthFirst,
							MItDependencyGraph::kNodeLevel,
							&stat);

	if (!stat)
	{
		o<<"ProcessShader(): ERROR: mpColor iterator failed!"<<std::endl;
		return MStatus::kFailure;     
	}

	dgIt.next();
	MObject textureNode = dgIt.thisNode();

	MString colorTextureFileName;
	MString lmapTextureFileName;
	MString nmapTextureFileName;

	MStatus gotColorTex=MS::kFailure;
	MStatus gotLmapTex=MS::kFailure;
	MStatus gotNmapTex=MS::kFailure;

	if (textureNode.hasFn(MFn::kFileTexture))
	{
		// color texture
		gotColorTex=GetTextureFileName(mpColor,colorTextureFileName);

		// lightmap
        if (!lmapTex.Size())
        {
	        MPlug mpAmbientColor = MFnDependencyNode(aPlugs[0].node()).findPlug("ambientColor", &stat);
	        if (stat)
		        gotLmapTex = GetTextureFileName(mpAmbientColor,lmapTextureFileName);
        }

        if (!nmapTex.Size())
        {
		    MPlug mpBump = MFnDependencyNode(aPlugs[0].node()).findPlug("normalCamera", &stat);
		    if (stat)
			    gotNmapTex = GetTextureFileName(mpBump,nmapTextureFileName);
        }
	}
	else 
	if (textureNode.hasFn(MFn::kLayeredTexture))
	{
		o<<"parsing from layered texture..."<<std::endl;

		//nowy plug array dla layered texture
		MPlug mpLTexColorParent = MFnDependencyNode(textureNode).findPlug("inputs", &stat);
		MPlug firstindicesText = mpLTexColorParent.elementByPhysicalIndex(0); // layer index
		MPlug mpLTexColor = firstindicesText.child(0, &stat);	// 0 = color, 1 = alpha

		if(stat)
		{
			gotColorTex=GetTextureFileName(mpLTexColor,colorTextureFileName);
			/*
			MPlug mpLLMAPColor = firstindicesText.child(0, &stat);
			if (stat)
				gotLmapTex = GetTextureFileName(mpLLMAPColor,lmapTextureFileName);
			*/
		}
	}
	else
	{
		o<<"ProcessShader(): WARNING: no kFileTexture FN!"<<std::endl;
		colorTex = "notex";
		return MStatus::kSuccess;
	}

	if(MS::kSuccess == gotColorTex)
	{
		if (colorTextureFileName.length()>0)
		{
			o<<"\t\tCOLOR texture:"<<colorTextureFileName.asChar()<<std::endl;
			String fileName=colorTextureFileName.asChar();
			fileName=String::BaseName(fileName);
			colorTex =String::StripExtension(fileName);
		}
	}

    if(MS::kSuccess == gotLmapTex)
    {
	    if (lmapTextureFileName.length()>0)
	    {
		    o<<"\t\tLMAP texture:"<<lmapTextureFileName.asChar()<<std::endl;
		    String fileName=lmapTextureFileName.asChar();
		    fileName=String::BaseName(fileName);
		    lmapTex=String::StripExtension(fileName);
		}
	}

    if(MS::kSuccess == gotNmapTex)
    {
	    if (nmapTextureFileName.length()>0)
	    {
		    o<<"\t\tNMAP texture:"<<nmapTextureFileName.asChar()<<std::endl;
		    String fileName=nmapTextureFileName.asChar();
		    fileName=String::BaseName(fileName);
		    nmapTex=String::StripExtension(fileName);
		}
	}

	return MStatus::kSuccess;
}

// ==================================================================================

MStatus WorldExporter::ProcessMesh(MFnMesh& fnMesh)
{
	// dump info
	static int index=0;
	String meshName=fnMesh.partialPathName().asChar();
    o<<"mesh["<<index++<<"] '"<<meshName.AsChar()<<"' ("                    
     <<"V:"<<fnMesh.numVertices()<<" "
     <<"N:"<<fnMesh.numNormals()<<" "
     <<"P:"<<fnMesh.numPolygons()<<")"
     <<std::endl;

	MeshProxy proxy;
	MItMeshPolygon poly(fnMesh.object());

	MFloatPointArray points;
	MFloatVectorArray normals;
	UVSet	uvSets[3];

	fnMesh.getPoints (points,MSpace::kWorld);
	fnMesh.getNormals(normals,MSpace::kWorld);

	int uvSetCount = fnMesh.numUVSets();
	if (uvSetCount > 2) 
		uvSetCount = 2;

    // the number of uv sets to use will depend on whether we find lmap and/or nmap
    int uvSetNum = -1;

	MStringArray uvSetNames;
	fnMesh.getUVSetNames(uvSetNames);

	for (int i=0; i<uvSetCount; ++i)
	{
		fnMesh.getUVs(uvSets[i].U,uvSets[i].V,&uvSetNames[i]);
		o<<"\tuvSet["<<i<<"]: '"<<uvSetNames[i].asChar()<<"' ("<<uvSets[i].U.length()<<","<<uvSets[i].V.length()<<")"<<std::endl;
	}

	/*
	for (int x=0; x<uvSets[1].U.length(); ++x)
	{
		o<<"U,V="<<uvSets[1].U[x]<<","<<uvSets[1].V[x]<<std::endl;
	}
	*/

	// process shaders
	MObjectArray shaders;
	MIntArray shaderIndices;
	fnMesh.getConnectedShaders(0, shaders, shaderIndices);

	o<<"\tshaders:"<<shaders.length()<<",shaderIndices:"<<shaderIndices.length()<<std::endl;

    // go thru shading groups and set up verts and tris
    int baseIndex=0;
    proxy.TriangleNum = 0;
    String lmapTex, nmapTex;

	for (unsigned int i=0; i<shaders.length(); ++i) 
    {
		String colorTex;

		o<<"\tshader["<<i<<"]:"<<GetObjectName(shaders[i])<<std::endl;
		if (!ProcessShader(shaders[i],colorTex,lmapTex,nmapTex))
			continue;

        // first time init
        if (uvSetNum == -1)
        {
            uvSetNum = 1;
            if (lmapTex.Size() && uvSetCount > uvSetNum)
                uvSetNum++;
			else
            if (nmapTex.Size() && uvSetCount > uvSetNum)
                uvSetNum++;

            o<<"Will use "<<uvSetNum<<" uv sets..."<<std::endl;
        }

		// find all polies for this shader
        int indexCount=0;
        for (unsigned int index=0; index<shaderIndices.length(); ++index) 
		{
			if (shaderIndices[index]==i) 
            {
				int junk;
				MStatus stat = poly.setIndex(index,junk);
						
				if (!stat) {
					o<<"ERROR: poly.setIndex()"<<std::endl;
					continue;
				}

				if (poly.polygonVertexCount() != 3)				
					continue;

				// walk vertices
				// NOTE: we reverse the winding order!
				for (int t=0; t<3; ++t)
				//for (int t=2; t>=0; --t)
				{
					int vertexIndex = poly.vertexIndex(t);
					int normalIndex = poly.normalIndex(t);

					int uvIndices[2] = { -1,-1 };	
					for (int x=0; x<uvSetNum; ++x)
						poly.getUVIndex(t,uvIndices[x],&uvSetNames[x]);

					//o<<"\t"<<vertexIndex<<","<<normalIndex<<","<<uvIndices[0]<<","<<uvIndices[1]<<","<<uvIndices[2];

					int triangle = (uvSetNum == 2) ? proxy.Add2(vertexIndex,normalIndex,uvIndices[0],uvIndices[1])
												    : proxy.Add1(vertexIndex,normalIndex,uvIndices[0]);

                    proxy.Triangles[baseIndex + indexCount++]=WORD(triangle);
					++proxy.TriangleNum;
					//o<<" -> "<<triangle<<std::endl;
				}
			}
		}

        o<<"\tSECTION: ("<<baseIndex<<","<<indexCount<<")"<<std::endl;

        // sometimes there are unreferenced shaders, skip them
        if (indexCount>0) 
        {
            WorldMesh::Group* grp = proxy.Groups.Create();
            grp->MaterialName = colorTex;
            grp->RenderBase   = baseIndex;
            grp->RenderCount  = indexCount/3;

            baseIndex+=indexCount;
        }
        else 
        {
            o<<"WARNING! bogus shader, skippin shading group"<<std::endl;
        }
	}

    o<<"========================================================"<<std::endl;
    o<<"V:"<<proxy.Verts.Size()<<" T:"<<proxy.TriangleNum<<std::endl;
    o<<"LMAP: "<<(lmapTex.Size() ? lmapTex.AsChar() : "<null>") <<std::endl;
    o<<"NMAP: "<<(nmapTex.Size() ? nmapTex.AsChar() : "<null>") <<std::endl;

    for (int i=0; i<proxy.Groups.Size(); ++i)
    {
        o<<"\tGroup["<<i<<"]:"<<proxy.Groups[i].MaterialName.AsChar()
         <<" ("<<proxy.Groups[i].RenderBase<<","<<proxy.Groups[i].RenderCount<<")"
         <<std::endl;
    }

    o<<"========================================================"<<std::endl;

    if (!proxy.Verts.Size() || !proxy.TriangleNum)
        return MStatus::kFailure;

    WorldMesh* mesh = reinterpret_cast<WorldMesh*>(Package->CreateObject("WorldMesh",meshName.AsChar()));
    if (!mesh) 
    {
        o<<"ERROR: mesh is NULL"<<std::endl;
        return MStatus::kFailure;
    }

    // init mesh
	mesh->Flags         = 0;
    mesh->Name          = meshName;
    mesh->LightMapName  = lmapTex.Size() ? lmapTex : "notex";
#ifndef TRANSGAMING
    mesh->NormalMapName = nmapTex.Size() ? nmapTex : "notex";
#endif
    mesh->ObjectToWorld = ObjectToWorld;

    // copy data over
	for (int i=0; i<proxy.TriangleNum; ++i) {
		if (proxy.Triangles[i] > proxy.Verts.Size()) {
			o<<"ERROR: mesh "<<meshName.AsChar()<<" index["<<i<<"]="
				<<proxy.Triangles[i]<<" > VertNum ("<<proxy.Verts.Size()<<")"<<std::endl;
		}
	}

	//mesh->Triangles     = proxy.Triangles;
	mesh->Triangles.Resize(proxy.TriangleNum);
	//memcpy(&mesh->Triangles[0],&proxy.Triangles[0],sizeof(WORD)*proxy.TriangleNum);
	for (int i=0; i<proxy.TriangleNum/3; ++i) {
		mesh->Triangles[i*3+0] = proxy.Triangles[i*3+2];
		mesh->Triangles[i*3+1] = proxy.Triangles[i*3+1];
		mesh->Triangles[i*3+2] = proxy.Triangles[i*3+0];
	}

    mesh->Groups        = proxy.Groups;

    // determine vertex data type
    int type = 1;

    if (uvSetNum > 1)
    {
        // NOTE: lmap takes precedence if only 2 uvSets
        if (lmapTex.Size())
            type = 2;

        // nmap will get skipped if only 2 uvSets
        if (nmapTex.Size())
        {
            if (type < 2) // there was no lightmap
                type = 3;
            else
            if (uvSetNum>2)
                type = 4;
        }
    }

    o<<"Vertex Data Type: "<<type<<std::endl;

    switch (type)
    {
    case 1: // base color only
        {
            mesh->Flags |= WorldMesh::EVertFormatNTU;
            mesh->Verts.Resize(proxy.Verts.Size());
            for (int i=0; i<proxy.Verts.Size(); ++i)
            {
                const MFloatPoint& pt   = points [proxy.Verts[i]];
                const MVector& n        = normals[proxy.Normals[i]];
                const int uvIdx         = proxy.TU1[i];

                mesh->Verts[i].Pos     = Vector(pt.x,pt.y,pt.z);
                mesh->Verts[i].Normal  = Vector(n.x,n.y,n.z);
                mesh->Verts[i].TU      = TexCoord(uvSets[0].U[uvIdx],1.0f - uvSets[0].V[uvIdx]);

                mesh->Box += mesh->Verts[i].Pos;
            }
        }
        break;
    case 2: // color + lightmap
        {
			mesh->Verts.Resize(proxy.Verts.Size());
            mesh->Normals.Resize(proxy.Verts.Size());

            for (int i=0; i<proxy.Verts.Size(); ++i)
            {
                const MFloatPoint& pt   = points [proxy.Verts[i]];
                const MVector& n        = normals[proxy.Normals[i]];
                const int uvIdx1        = proxy.TU1[i];
                const int uvIdx2        = proxy.TU2[i];

                mesh->Verts[i].Pos     = Vector(pt.x,pt.y,pt.z);
                mesh->Verts[i].Color   = (DWORD)-1;
                mesh->Verts[i].TU1     = TexCoord(uvSets[0].U[uvIdx1],1.0f - uvSets[0].V[uvIdx1]);
                mesh->Verts[i].TU2     = TexCoord(uvSets[1].U[uvIdx2],1.0f - uvSets[1].V[uvIdx2]);

                mesh->Normals[i]       = Vector(n.x,n.y,n.z);

                mesh->Box += mesh->Verts[i].Pos;
            }
        }
		break;
    case 3: // color + normal
        {
            mesh->Flags |= WorldMesh::EVertFormatNTU;
            mesh->Verts.Resize(proxy.Verts.Size());
            mesh->Tangents.Resize(proxy.Verts.Size());

            for (int i=0; i<proxy.Verts.Size(); ++i)
            {
                const MFloatPoint& pt   = points [proxy.Verts[i]];
                const MVector& n        = normals[proxy.Normals[i]];
                const int uvIdx1        = proxy.TU1[i];
                const int uvIdx2        = proxy.TU2[i];

                mesh->Verts[i].Pos     = Vector(pt.x,pt.y,pt.z);
                mesh->Verts[i].Normal  = Vector(n.x,n.y,n.z);
                mesh->Verts[i].TU      = TexCoord(uvSets[0].U[uvIdx1],1.0f - uvSets[0].V[uvIdx1]);

                mesh->Tangents[i].Tangent  = Vector(0,0,0);
                mesh->Tangents[i].Binormal = Vector(0,0,0);
                mesh->Tangents[i].TU       = TexCoord(uvSets[1].U[uvIdx2],1.0f - uvSets[1].V[uvIdx2]);

                mesh->Box += mesh->Verts[i].Pos;
            }
        }
		break;
    case 4:
        // later!!!
        break;
    }

	o<<"Mesh: "<<meshName.AsChar()<<" done"<<std::endl;

    TotalVerts += mesh->Verts.Size();
    TotalTris  += mesh->Triangles.Size();

	return MStatus::kSuccess;
}

// ==================================================================================

MStatus WorldExporter::ProcessNode(MDagPath& dagPath)
{
	MStatus stat=MS::kFailure;
	MFnMesh fnMesh(dagPath, &stat);

	if (MS::kFailure == stat) 
        return stat;

    // get world matrix
    GetObjectMatrix(dagPath,ObjectToWorld);

    // sanity check on sizes
    String meshName=fnMesh.partialPathName().asChar();
    if (fnMesh.numVertices()>65535 || fnMesh.numNormals()>65535) 
    {
	    std::cerr<<"ERROR: mesh: '"<<meshName.AsChar()<<"' TOO FUCKING BIG (verts)"<<std::endl;
        return MStatus::kFailure;
    }

    if (fnMesh.numPolygons()*3 > 65535) 
    {
	    std::cerr<<"ERROR: mesh: '"<<meshName.AsChar()<<"' TOO FUCKING BIG (polies)"<<std::endl;
        return MStatus::kFailure;
    }

    // special meshes
    if (!strnicmp(meshName.AsChar(),"zone",4)) 
    {
	    if (strlen(meshName.AsChar())<=4) 
        {
            std::cerr<<"ERROR: invalid zone name:"<<meshName.AsChar()<<std::endl;
            return MStatus::kFailure;
        }

		if (MS::kSuccess == ProcessZone(fnMesh))
	        ++TotalZones;

    }
    else
    if (!strnicmp(meshName.AsChar(),"portal",6)) 
    {
	    if (MS::kSuccess == ProcessPortal(fnMesh))
		    ++TotalPortals;
    }
    else
    {
        if (MS::kSuccess == ProcessMesh(fnMesh))
            ++TotalMeshes;
    }

    return MStatus::kSuccess;
}

// ==================================================================================

MStatus WorldExporter::OutputFile()
{
	std::cerr<<"====================================="<<std::endl;
	std::cerr<<"Zones:     "<<TotalZones<<std::endl;
	std::cerr<<"Portals:   "<<TotalPortals<<std::endl;
	std::cerr<<"Meshes:    "<<TotalMeshes<<std::endl;
	std::cerr<<"Vertices:  "<<TotalVerts<<std::endl;
	std::cerr<<"Triangles: "<<TotalTris/3<<" ("<<TotalTris<<")"<<std::endl;
	std::cerr<<"Strips:    "<<TotalElems<<std::endl;
	std::cerr<<"======================================"<<std::endl;


	std::cerr<<"Saving package ... ";
    Package->Save();
    std::cerr<<"DONE"<<std::endl;


#if 0
    // lights!
	for(std::vector<MDagPath>::size_type index=0; index<LightNodes.size(); ++index) {
		MStatus stat=MS::kFailure;
		MFnLight fnLight(LightNodes[index], &stat);
		if(MS::kSuccess == stat) {
            MColor color   =fnLight.color();
            float intensity=fnLight.intensity();

            std::cerr<<"color=("<<color.r<<","<<color.g<<","<<color.b<<") intensity="<<intensity<<std::endl;

            MObject transformNode = LightNodes[index].transform(&stat);
            MFnDagNode transform (transformNode, &stat);
            
            MTransformationMatrix matrix(transform.transformationMatrix());
            MVector translation=matrix.translation(MSpace::kWorld);

            Light* light = Lights.Create();
            light->Name = LightNodes[index].partialPathName().asChar();

            Vector position(float(translation.x),float(translation.y),-float(translation.z));
            light->Box.Reset();
            light->Box+=position;
            light->Box.Expand(16);

            light->Color=Vector(color.r,color.g,color.b);
            light->Intensity=fnLight.intensity();
        }
    }
#endif

	return MS::kSuccess;
}
