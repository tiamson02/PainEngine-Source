#include "Engine.h"

#include "Stripify.h"

void Stripify(SimpleMesh* mesh, bool listsOnly)
{
    if (mesh->Elems.Size())
        mesh->Elems.Clear();

    if (!mesh->Triangles.Size())
    {
        printf("Stripify: ERROR - mesh has no tris!\n");
        return;
    }

    // Stripping stuff
	SetCacheSize(CACHESIZE_GEFORCE3);
	SetStitchStrips(true);
	SetMinStripSize(0);
	SetListsOnly(listsOnly);            

    int totalIndices=0;

    //DynamicArray<WorldMesh::Group> newGroups;

    for (int i=0; i<mesh->Groups.Size(); ++i) 
    {
        SimpleMesh::Group& group=mesh->Groups[i];
                
        const int baseIndex  = group.RenderBase;
        const int indexCount = group.RenderCount*3;

	    //stripify/optimize
	    PrimitiveGroup* primGroups=0;
        unsigned short numGroups=0;
	    GenerateStrips(&mesh->Triangles[baseIndex], indexCount, &primGroups, (unsigned short*)&numGroups);

        if (primGroups && numGroups==1) 
        {
            //o<<"section["<<i<<"]:"<<indexCount<<"->"<<primGroups[0].numIndices<<std::endl;                    

            /*
            WorldMesh::Group* g = newGroups.Create();
            g->Material     = 0;
            g->MaterialName = group.MaterialName;
            g->RenderBase   = totalIndices;
            g->RenderCount  = primGroups[0].numIndices-2; // number of strips
            */

            group.RenderBase  = totalIndices;
			if (listsOnly)
				group.RenderCount = primGroups[0].numIndices/3; 
			else 
				group.RenderCount = primGroups[0].numIndices-2; 

            for (unsigned int j=0; j<primGroups[0].numIndices; ++j) 
                mesh->Elems.Add(primGroups[0].indices[j]);

            totalIndices+=primGroups[0].numIndices;
        }
                    
        if(primGroups) delete [] primGroups;
    }

    // replace shading groups
    //mesh->Groups = newGroups;

    // ugly hack
    if (listsOnly)
    {
        mesh->Triangles = mesh->Elems;
        mesh->Elems.Clear();
    }
}
