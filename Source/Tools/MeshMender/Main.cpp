
#include "NVMeshMender.h"

#include "Core/AbstractFactory.h"
#include "Engine/SimpleMesh.h"
#include "Engine/Mesh.h"
#include "Engine/AnimatedMesh.h"
#include "Engine/Entity.h"
#include "Engine/World.h"
#include "Stripify.h"

// options

static bool BuildTangents=false;
static bool BuildStrips  =false;
static bool TreatElemsAsTris=false; // to convert old format data into new
static bool RemoveDist=false;

static void ProcessMesh(WorldMesh* mesh)
{
    // retrive positions and texcoords
    std::vector<float> posVec;
    std::vector<float> norVec;
    std::vector<float> tuVec;

    for (int i=0; i<mesh->Verts.Size(); ++i)
    {
        posVec.push_back(mesh->Verts[i].Pos.X);
        posVec.push_back(mesh->Verts[i].Pos.Y);
        posVec.push_back(mesh->Verts[i].Pos.Z);

        norVec.push_back(mesh->Verts[i].Normal.X);
        norVec.push_back(mesh->Verts[i].Normal.Y);
        norVec.push_back(mesh->Verts[i].Normal.Z);

        tuVec.push_back(mesh->Verts[i].TU.X);
        tuVec.push_back(mesh->Verts[i].TU.Y);
        tuVec.push_back(0);
    }

    // retrive triangle indices
    std::vector<int> indices;
    indices.resize(mesh->Triangles.Size());

    for (int i=0; i<mesh->Triangles.Size(); ++i)
        indices[i] = int(mesh->Triangles[i]);


    std::vector<NVMeshMender::VertexAttribute> inputAtts;	// What you have
    std::vector<NVMeshMender::VertexAttribute> outputAtts;	// What you want.

    // inputs
    NVMeshMender::VertexAttribute posAtt;
    posAtt.Name_ = "position";
    posAtt.floatVector_ = posVec;

    NVMeshMender::VertexAttribute norAtt;
    norAtt.Name_ = "normal";
    norAtt.floatVector_ = norVec;

    NVMeshMender::VertexAttribute texCoordAtt;
    texCoordAtt.Name_ = "tex0";
    texCoordAtt.floatVector_ = tuVec;

    NVMeshMender::VertexAttribute triIndAtt;
    triIndAtt.Name_ = "indices";
    triIndAtt.intVector_ = indices;

    inputAtts.push_back(posAtt);
    inputAtts.push_back(norAtt);
    inputAtts.push_back(texCoordAtt);
    inputAtts.push_back(triIndAtt);

    // outputs
    NVMeshMender::VertexAttribute tgtSpaceAtt;
    tgtSpaceAtt.Name_ = "tangent";

    NVMeshMender::VertexAttribute binormalAtt;
    binormalAtt.Name_ = "binormal";

    outputAtts.push_back(posAtt);
    outputAtts.push_back(norAtt);
    outputAtts.push_back(texCoordAtt);
    outputAtts.push_back(triIndAtt);
    outputAtts.push_back(tgtSpaceAtt);
    outputAtts.push_back(binormalAtt);

    printf("pos     : %d\n",outputAtts[0].floatVector_.size()/3);
    printf("normal  : %d\n",outputAtts[1].floatVector_.size()/3);
    printf("tu      : %d\n",outputAtts[2].floatVector_.size()/3);
    printf("tris    : %d\n",outputAtts[3].intVector_.size()/3);

    printf("munging ...\n");

	// do it
	NVMeshMender mender;
	if (!mender.Munge(
					inputAtts,								// input attributes
					outputAtts,								// outputs attributes
					3.141592654f / 3.0f,					// tangent space smooth angle
					0,										// no texture matrix applied to my texture coordinates
					NVMeshMender::FixTangents,				// fix degenerate bases & texture mirroring
					NVMeshMender::DontFixCylindricalTexGen,		// handle cylindrically mapped textures via vertex duplication
					NVMeshMender::DontWeightNormalsByFaceSize	// weigh vertex normals by the triangle's size
					))
	{
		printf("NVMeshMender failed\n");
		exit(-1);
	}

    printf("pos     : %d\n",outputAtts[0].floatVector_.size()/3);
    printf("normal  : %d\n",outputAtts[1].floatVector_.size()/3);
    printf("tu      : %d\n",outputAtts[2].floatVector_.size()/3);
    printf("tris    : %d\n",outputAtts[3].intVector_.size()/3);
    printf("tangent : %d\n",outputAtts[4].floatVector_.size()/3);
    printf("binormal: %d\n",outputAtts[5].floatVector_.size()/3);

    // reinit mesh
    mesh->Verts.Resize(outputAtts[0].floatVector_.size()/3);
    //mesh->Tangents.Resize(mesh->Verts.Size());
	//printf("Mesh.Tangents: %d\n",mesh->Tangents.Size());


	if (!mesh->Tangents.Size())
	{
		mesh->Tangents.Resize(mesh->Verts.Size());
		for (int i=0; i<mesh->Tangents.Size(); ++i)
			mesh->Tangents[i].TU = TexCoord(mesh->Verts[i].TU.X,mesh->Verts[i].TU.Y);
	}

    for (int i=0; i<mesh->Verts.Size(); ++i)
    {
        mesh->Verts[i].Pos.X = outputAtts[0].floatVector_[i*3+0];
        mesh->Verts[i].Pos.Y = outputAtts[0].floatVector_[i*3+1];
        mesh->Verts[i].Pos.Z = outputAtts[0].floatVector_[i*3+2];

        mesh->Verts[i].Normal.X = outputAtts[1].floatVector_[i*3+0];
        mesh->Verts[i].Normal.Y = outputAtts[1].floatVector_[i*3+1];
        mesh->Verts[i].Normal.Z = outputAtts[1].floatVector_[i*3+2];

        mesh->Verts[i].TU.X = outputAtts[2].floatVector_[i*3+0];
        mesh->Verts[i].TU.Y = outputAtts[2].floatVector_[i*3+1];

        mesh->Tangents[i].Tangent.X = outputAtts[4].floatVector_[i*3+0];
        mesh->Tangents[i].Tangent.Y = outputAtts[4].floatVector_[i*3+1];
        mesh->Tangents[i].Tangent.Z = outputAtts[4].floatVector_[i*3+2];

        mesh->Tangents[i].Binormal.X = outputAtts[5].floatVector_[i*3+0];
        mesh->Tangents[i].Binormal.Y = outputAtts[5].floatVector_[i*3+1];
        mesh->Tangents[i].Binormal.Z = outputAtts[5].floatVector_[i*3+2];

		//printf("TU (%.2f %.2f)\n",mesh->Tangents[i].TU.X,mesh->Tangents[i].TU.Y);
        // TEST
        //mesh->Tangents[i].TU.X = mesh->Verts[i].TU.X;
        //mesh->Tangents[i].TU.Y = mesh->Verts[i].TU.Y;
    }

    // set flags to indicate we've got tangent space stream
    mesh->Flags |= WorldMesh::ENormTangent;
}

static int NumInstrCached(int cacheSize, const DynamicArray<WORD>& Elems)
{
	DynamicArray<WORD> fifo;
	fifo.Resize(cacheSize);
	for (int i=0; i<cacheSize; i++)
	{
		fifo[i] = -1;
	}

	int instr = 0;
	int last = 0;

	for (int e=0; e<Elems.Size(); e++)
	{
		bool inCache = false;

		for (int i=0; i<cacheSize; i++)
		{
			if (fifo[i]==Elems[e])
			{
				inCache = true;
				break;
			}
		}

		if (!inCache)
		{
			fifo[last] = Elems[e];
			last = (last+1)%cacheSize;
			instr ++;
		}
	}

	return instr;
}

static int DistinctVerts(const DynamicArray<MeshVertex>& Verts)
{
	int dist = 0;

	for (int i=0; i<Verts.Size(); i++)
	{
		const MeshVertex &vi = Verts[i];
		dist ++;

		for (int j=0; j<i; j++)
		{
			const MeshVertex &vj = Verts[j];

			if (vi.Pos==vj.Pos && vi.TU==vj.TU && vi.Normal==vj.Normal)
			{
				dist --;
				break;
			}
		}
	}

	return dist;
}

static int DistinctVerts(const DynamicArray<MeshVertex>& Verts, const DynamicArray<Vector>& Normals)
{
	int dist = 0;

	for (int i=0; i<Verts.Size(); i++)
	{
		const MeshVertex &vi = Verts[i];
		const Vector &ni = Normals[i];
		dist ++;

		for (int j=0; j<i; j++)
		{
			const MeshVertex &vj = Verts[j];
			const Vector &nj = Normals[j];

			if (vi.Pos==vj.Pos && vi.TU1==vj.TU1 && vi.TU2==vj.TU2 && ni==nj)
			{
				dist --;
				break;
			}
		}
	}

	return dist;
}

static void ProcessPackage(const char* name, const char* outFileName)
{
    ObjectPackage* package = new ObjectPackage(name);
	if (package)
	{
        printf("loaded package '%s'\n",name);

        for (ClassIterator<WorldMesh> i(package); i; ++i)
        {
            WorldMesh* mesh = i.GetInstance();
#ifdef TRANSGAMING
			printf("mesh: %s [L:%s] (%d, %d, %d)\n",
					mesh->Name.AsChar(),mesh->LightMapName.AsChar(),
					mesh->Verts.Size(),mesh->Triangles.Size()/3,mesh->Elems.Size());
#else
			printf("mesh: %s [L:%s] (%d, %d, %d) CloneOf: %s\n",
					mesh->Name.AsChar(),mesh->LightMapName.AsChar(),
					mesh->Verts.Size(),mesh->Triangles.Size()/3,mesh->Elems.Size(),
					mesh->CloneOf.AsChar());
#endif

            if (BuildTangents && (mesh->Flags & WorldMesh::EVertFormatNTU))
				//if (stricmp("polysurfaceshape210",mesh->Name.AsChar()))
					ProcessMesh(mesh);
				//else
				//	package->RemoveObject(mesh);

            if (BuildStrips)
                Stripify(mesh);
        }

		for (ClassIterator<AnimatedMesh> i(package); i; ++i)
		{
			AnimatedMesh* animmesh = i.GetInstance();

			printf("SkeletalMesh: %s \n", animmesh->Name);
			for (int m=0; m<animmesh->Meshes.Size(); m++)
			{
				MeshShape *mesh = &animmesh->Meshes[m];

				if (TreatElemsAsTris)
				{
					mesh->Triangles = mesh->Elems;
					mesh->Elems.Clear();
				}
				if (BuildStrips)
				{
					Stripify(mesh);
				}

				printf(" %s: (V:%d, T:%d, E:%d)\n",
					mesh->Name.AsChar(),
					mesh->Verts.Size(),mesh->Triangles.Size()/3,mesh->Elems.Size());

				printf("  Dist(%d)\n",
					DistinctVerts(mesh->Verts));

				printf("  GF1_2(T:%d, E:%d) GF3(T:%d, E:%d)\n",
					NumInstrCached(CACHESIZE_GEFORCE1_2, mesh->Triangles),
					NumInstrCached(CACHESIZE_GEFORCE1_2, mesh->Elems),
					NumInstrCached(CACHESIZE_GEFORCE3, mesh->Triangles),
					NumInstrCached(CACHESIZE_GEFORCE3, mesh->Elems));

				printf("\n");
			}
		}

        package->Dirty = 1;
        package->Save(outFileName);
        delete package;
    }
}

void RemoveDistVerts(SimpleMesh* Mesh)
{
	DynamicArray<MeshVertex>& Verts = Mesh->Verts;
	DynamicArray<Vector>& Normals = Mesh->Normals;
	DynamicArray<WORD>& Elems = Mesh->Elems;
	DynamicArray<WORD>& Triangles = Mesh->Triangles;

	BOOL ntu = ((WorldMesh*)Mesh)->Flags & SimpleMesh::EVertFormatNTU;

	if (ntu)
	{
		for (int i=0; i<Verts.Size(); i++)
		{
			const MeshVertex &vi = Verts[i];

			for (int j=0; j<i; j++)
			{
				const MeshVertex &vj = Verts[j];

				bool same;
				same = (vi.Pos==vj.Pos && vi.TU==vj.TU && vi.Normal==vj.Normal);

				if (same)
				{
					//same vertex
					Verts.Remove(i);

					for (int k=0; k<Triangles.Size(); k++)
					{
						if (Triangles[k] == i)
							Triangles[k] = j;
						else if (Triangles[k] > i)
							Triangles[k] --;
					}
					i --;
					break;
				}
			}
		}
	}
	else
	{
		for (int i=0; i<Verts.Size(); i++)
		{
			const MeshVertex &vi = Verts[i];
			const Vector &ni = Normals[i];

			for (int j=0; j<i; j++)
			{
				const MeshVertex &vj = Verts[j];
				const Vector &nj = Normals[j];

				bool same;
				same = (vi.Pos==vj.Pos && vi.TU1==vj.TU1 && vi.TU2==vj.TU2 && ni==nj);

				if (same)
				{
					//same vertex
					Verts.Remove(i);
					Normals.Remove(i);

					for (int k=0; k<Triangles.Size(); k++)
					{
						if (Triangles[k] == i)
							Triangles[k] = j;
						else if (Triangles[k] > i)
							Triangles[k] --;
					}
					i --;
					break;
				}
			}
		}
	}
}

void ProcessMeshes(DynamicArray<WorldMesh*> &Meshes)
{
	int TotalVerts = 0;
	int DistVerts = 0;
	int TotalTriLists = 0;
	int TotalTriStrips = 0;

	for (int i=0; i<Meshes.Size(); i++)
	{
		SimpleMesh *mesh = Meshes[i];
		
		if (RemoveDist)
		{
			RemoveDistVerts(mesh);
		}

		if (BuildStrips)
		{
			Stripify(mesh,true);
		}

		BOOL ntu = Meshes[i]->Flags & SimpleMesh::EVertFormatNTU;

		printf(" %s: (V:%d, T:%d, E:%d)\n",
			Meshes[i]->Name.AsChar(),
			mesh->Verts.Size(),
			mesh->Triangles.Size()/3,
			mesh->Elems.Size());

		TotalVerts += mesh->Verts.Size();

		if (ntu) {
			int v = DistinctVerts(mesh->Verts);
			printf("  DistNTU(%d)\n", v);
			DistVerts += v;
		}
		else {
			int v = DistinctVerts(mesh->Verts, mesh->Normals);
			printf("  DistTU2(%d)\n", v);
			DistVerts += v;
		}

		int c[] = 
		{
			NumInstrCached(CACHESIZE_GEFORCE1_2, mesh->Triangles),
			NumInstrCached(CACHESIZE_GEFORCE1_2, mesh->Elems),
			NumInstrCached(CACHESIZE_GEFORCE3, mesh->Triangles),
			NumInstrCached(CACHESIZE_GEFORCE3, mesh->Elems)
		};

		TotalTriLists += c[2];
		TotalTriStrips += c[3];

		printf("  GF1_2(T:%d, E:%d) GF3(T:%d, E:%d)\n\n",c[0],c[1],c[2],c[3]);
	}
	printf("===== Stats ======\n");
	printf("TotalVerts = %d\n",TotalVerts);
	printf("DistVerts = %d\n",DistVerts);
	printf("TotalTriLists = %d\n",TotalTriLists);
	printf("TotalTriStrips = %d\n",TotalTriStrips);
}

int main(int argc, char** argv)
{
    if (argc<2)
	{
        printf("usage: %s [-r][-s][-t][-c][-o outfile] <meshpak>\n",argv[0]);
		printf(
			"\t-r remove duplicated vertices\n"
			"\t-s build tristrips\n"
			"\t-t build tangent basis\n"
			"\t-c treat elems as tris\n"
			"example1: %s -r -s Mapa.mpk\n", argv[0]
			);
	}
    else
    {
        const char* meshPakFile = 0;
        const char* outFileName = 0;

        for (int i=1; i<argc; ++i)
        {
            if (!stricmp("-r",argv[i]))
			{
				printf("Removing Duplicated Vertices\n");
                RemoveDist = true;
			}
            else
            if (!stricmp("-s",argv[i]))
			{
				printf("Building TriStrips\n");
                BuildStrips = true;
			}
            else
            if (!stricmp("-t",argv[i]))
			{
				printf("Building Tangent Basis\n");
                BuildTangents = true;
			}
            else
            if (!stricmp("-c",argv[i]))
			{
				printf("Treat Elems As Tris\n");
                TreatElemsAsTris = true;
			}
			if (!stricmp("-o",argv[i]) && i<argc-1)
			{
				printf("Output file set to: %s\n",argv[i+1]);
				outFileName = argv[++i];
			}
	
			else
                meshPakFile = argv[i];
        }

        if (meshPakFile)
		{
			const char* s = strstr(meshPakFile,".mpk");
			if (s) {
				if (!outFileName)
					outFileName = "out.mpk";
				DynamicArray<WorldMesh*> Meshes = World::LoadMeshPakArray(meshPakFile);
                
				ProcessMeshes(Meshes);

				World::SaveMeshPakArray(outFileName,Meshes);
			}
			else  
			{
				if (!outFileName)
					outFileName = "out.dat";
				ProcessPackage(meshPakFile,outFileName);
			}
		}
    }

    return 0;
}
