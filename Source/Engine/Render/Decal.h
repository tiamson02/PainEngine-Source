// #pragma once
#ifndef _DECAL_
#define _DECAL_

#include "Render/Vertex.h"
#include "Engine/Entity.h"

class DecalEffect;

class ENGINE_API Decal : public Entity
{
#ifdef TRANSGAMING_MEMORY_CONSTRAINTS
	static const int MAX_ELEMS = 64;
#else
	static const int MAX_ELEMS = 2048;
#endif
	SimpleVertex VertexArray[MAX_ELEMS];
	int			 NumElems;

	float			ScaleFactor;
	DWORD			ConstColor;
	DecalEffect*	Effect;
	float			TimeToDie;
	bool			NeverDie;
	String			TextureName;
	Texture*		TexturePtr;
	Matrix			SpawnMatrix;

public:
	Entity*			SpawnedOn;
	int				SpawnedOnId;


	Decal(DecalEffect *effect);
	~Decal();

	void SetState(RenderDevice* dev);
	void Draw(RenderDevice* dev, int mode);
	void SetScale(float scale);
	void SetTexture(const char *texName);
	void SetTexture(Texture* texturePtr) { TexturePtr = texturePtr; }
	const Matrix& GetSpawnMatrix() const { return SpawnMatrix; }

	void Tick(const float timeDelta);
	bool Spawn(Entity* e, const Vector& pos, const Vector& dir);
	bool Spawn(Entity* e, const Vector& pos, const Vector& nor, const Vector& tan, const Vector& bin);
	bool Spawn(Entity* e, const Matrix& m);

	void SetAlpha(int alpha) {ConstColor = alpha;};
	// Load/save
	virtual bool LoadEntity( FileObject* f );
	virtual bool SaveEntity( FileObject* f );

#ifndef TRANSGAMING
    // Demo recording
	virtual bool DemoReadEntity( FileObject *f );
	virtual bool DemoWriteEntity( FileObject *f );
	virtual bool DemoReadFull( FileObject *f );
	virtual bool DemoWriteFull( FileObject *f );
#endif

#ifdef PKTV_ENABLED
	char *GetEffectName() const;
	char *GetTextureName() const { return ( char *)TextureName.AsChar(); }
	DWORD GetConstColor() const { return ConstColor; }
	float GetTimeToDie() const { return TimeToDie; }
	bool GetNeverDie() const { return NeverDie; }
	int GetNumElems() const { return NumElems; }
	Vector GetVertPos( int i ) const { return VertexArray[i].Pos; }
	DWORD GetVertColor( int i ) const { return VertexArray[i].Color; }
	void GetVertUV( int i, float &u, float &v ) const { u = VertexArray[i].TU.U; v = VertexArray[i].TU.V; }

	void SetConstColor( DWORD color ) { ConstColor = color; }
	void SetTimeToDie( float time ) { TimeToDie = time; }
	void SetNeverDie( bool die ) { NeverDie = die; }
	void SetSpawnMatrix( Matrix &m ) { SpawnMatrix = m; }
	void SetNumElems( int num ) { NumElems = num; }
	void SetVertex( int i, Vector pos, DWORD color, float u, float v ) {
		VertexArray[i].Pos = pos; VertexArray[i].Color = color; VertexArray[i].TU.U = u; VertexArray[i].TU.V = v;
	}
#endif
};


#endif //_DECAL_

