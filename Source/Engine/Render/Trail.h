// #pragma once
#ifndef _TRAIL_
#define _TRAIL_


#include "Engine/Entity.h"

class TrailEffect;

class ENGINE_API Trail : public Entity
{
	static const int MAX_BONES = 16;
#ifdef TRANSGAMING
    static const int MAX_POSITIONS = 32;
#else
    static const int MAX_POSITIONS = 2000;
#endif
#ifdef TRANSGAMING
    static const float ADDNODE_PERIOD_SCALE;

    float       currentNodeTime;
#endif

    int			MAX_ELEMS;
#ifdef PKTV_ENABLED
    int			Vpn;
#endif

	Vector		*Positions;
	float		*Times;

	float		ScaleFactor;
	int			HeadNode;
	int			TailNode;

	int			TrailBones[MAX_BONES];
	Vector		LastPosition[MAX_BONES];
	Vector		LastNodePosition[MAX_BONES];
	int			BonesNum;

	DWORD			ConstColor;
	TrailEffect*	Effect;
	String			TextureName;
	Texture*		TexturePtr;
	float			Time;
	bool			PositionChanged;

public:
	Trail(TrailEffect *effect);
	~Trail();

	void Draw(RenderDevice* dev, int mode);
	void SetTexture(const char *texName);
	void SetTexture(Texture* texturePtr) { TexturePtr = texturePtr; }
	void SetState(RenderDevice* dev);
	void SetScale(float scale);

	void Tick(const float timeDelta);
	void UpdateTransform(bool updateInWorld=true);
	void AttachToBones(Entity* parent, const DynamicArray<String>& bones);
	void Init(int elems, int vpn);
	void Free();

	// Load/save
	virtual bool LoadEntity( FileObject* f );
	virtual bool SaveEntity( FileObject* f );

#ifndef TRANSGAMING
    // Demo recording
    virtual bool DemoCalcSize( int &size, DWORD &contents );
	virtual bool DemoReadEntity( FileObject *f );
	virtual bool DemoWriteEntity( FileObject *f );
	virtual bool DemoReadFull( FileObject *f );
	virtual bool DemoWriteFull( FileObject *f );
#endif

#ifdef PKTV_ENABLED
    char *GetEffectName() const;
	const char *GetTextureName() const { return TextureName.AsChar(); }
	float GetScaleFactor() const { return ScaleFactor; }
	DWORD GetConstColor() const { return ConstColor; }
	float GetTime() const { return Time; }
	bool GetPositionChanged() const { return PositionChanged; }
	int GetMaxElems() const { return MAX_ELEMS; }
	int GetHeadNode() const { return HeadNode; }
	int GetTailNode() const { return TailNode; }
	const int *GetTrailBones() const { return &TrailBones[0]; }
	const Vector *GetLastPosition() const { return &LastPosition[0]; }
	const Vector *GetLastNodePosition() const { return &LastNodePosition[0]; }
	float *GetTimes() const { return Times; }
	Vector *GetPositions() const { return Positions; }
    int GetBonesNum() const { return BonesNum; }
    int GetVpn() const { return Vpn; }

    // These functions are *ONLY* for playing demos/pktv
    void SetConstColor( DWORD color ) { ConstColor = color; }
	void SetScaleFactor( float scale ) { ScaleFactor = scale; }
	void SetTime( float time ) { Time = time; }
	void SetPositionChanged( bool val ) { PositionChanged = val; }
	void SetMaxElems( int count ) { MAX_ELEMS = count; }
	void SetNodes( int head, int tail ) { HeadNode = head; TailNode = tail; }
    void SetBonesNum( int count ) { BonesNum = count; }
    void SetVpn( int vpn ) { Vpn = vpn; }
#endif

protected:
	void AddNode();
	void CutTail();

#ifndef TRANSGAMING
	// Last frame state for demo recording
	int		mLastBonesNum;
#endif
};


#endif //_TRAIL_

