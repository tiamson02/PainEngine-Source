
// #pragma once
#ifndef _MATERIALDEF_
#define _MATERIALDEF_


// ==================================================================

struct MaterialDef 
{
	String		Name;
    Texture*    TexPtr;
	TexCoord	Pan;
	TexCoord	Tile;
	float		Rot;

	MaterialDef() : TexPtr(0), Pan(0,0), Tile(1,1), Rot(0) {}

	friend FileObject& operator>>(FileObject& file, MaterialDef& mat)
	{
		return file >> mat.Name >> mat.Pan >> mat.Tile;
	}

	friend FileObject& operator<<(FileObject& file, MaterialDef& mat)
	{
		return file << mat.Name << mat.Pan << mat.Tile;
	}
};




#endif //_MATERIALDEF_

