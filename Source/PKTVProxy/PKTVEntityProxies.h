#ifndef __PKTVENTITYPROXIES_H__
#define __PKTVENTITYPROXIES_H__

#include "LeString.h"
#include "LeMath.h"

// This class represents the data which is used to create the entity.
class EntityConstantData
{
	friend class EntityProxy;

	ulong32	mRefs;

	unsigned int	mIndex;	// the only way to access entity is by its index. Think - it may not exist yet we have its state sometimes.

	void AddReference( void ) { ++mRefs; };
	void RemoveReference( void ) { --mRefs; if( !mRefs ) delete this; };

	// Size calculations
	ulong32 GetReferences( void ) { return mRefs; }
	ulong32 MemoryTaken( void ) { return sizeof(EntityConstantData)+ FileName.MemoryTaken() + Name.MemoryTaken() - 2*sizeof(String); }
public:
	EntityConstantData( void ) : mRefs(0), FileName(""), Name("") {};
	virtual ~EntityConstantData( void ) {};

	String		FileName;
	String		Name;
};

class UdpMessage;

// This proxy stores the full recorded data for the entity
class EntityProxy
{
public:

	enum
    {
		ETypeNone       = 0,
        ETypeMesh       = 1,
        ETypeLight      = 2,
        ETypeParticleFx = 3,
		ETypeModel		= 4,
		ETypeRegion     = 5,
		ETypeDecal		= 6,
		ETypeVolume		= 7,
		ETypeBillboard	= 8,
		ETypeEnv		= 9, // defines environment settings overwriting those in level
		ETypeTrail		= 10,
        ETypeSound		= 11,
    };

	// already optimized for most often used flags at the beginning
	enum
	{
		EProxyWriteTranslationDiff		= ( 1 << 0 ),
		// 1 left for subclasses
		// 2 left for subclasses
		EProxyWriteRotation				= ( 1 << 3 ),
		EProxyWriteTranslation			= ( 1 << 4 ),
		EProxyWriteParentGIndex			= ( 1 << 5 ),
		EProxyWriteTimeToDelete			= ( 1 << 6 ),
		EProxyWriteSort					= ( 1 << 7 ),
		// 8 left for subclasses
		// 9 left for subclesses
		// 10 left for subclasses
		EProxyWriteScale				= ( 1 << 11 ),
		EProxyWriteFadeTime				= ( 1 << 12 ),
		EProxyWriteBoolFlags			= ( 1 << 13 ),
		EProxyWriteParent				= ( 1 << 14 ),
		EProxyWriteFlags				= ( 1 << 15 ),
		EProxyWriteParentJoint			= ( 1 << 16 ),

		// Billboard
		EProxyWriteBBDistance			= ( 1 << 1 ),
		EProxyWriteBBAlpha				= ( 1 << 2 ),

		EProxyWriteBBColor				= ( 1 << 8 ),
		EProxyWriteBBFadeInTime			= ( 1 << 9 ),
		EProxyWriteBBBillboardType		= ( 1 << 10 ),

		EProxyWriteBBTimer				= ( 1 << 17 ),
		EProxyWriteBBFadeOutTime		= ( 1 << 18 ),
		EProxyWriteBBAlphaMax			= ( 1 << 19 ),
		EProxyWriteBBBlendMode			= ( 1 << 20 ),
		EProxyWriteBBMaxDistance		= ( 1 << 21 ),
		EProxyWriteBBMinDistance		= ( 1 << 22 ),
		EProxyWriteBBMaxRadius			= ( 1 << 23 ),
		EProxyWriteBBMinRadius			= ( 1 << 24 ),
		EProxyWriteBBOffDistance		= ( 1 << 25 ),
		EProxyWriteBBTraceMargin		= ( 1 << 26 ),
		EProxyWriteBBIsVisible			= ( 1 << 27 ),
		EProxyWriteBBHitStatus			= ( 1 << 28 ),
		EProxyWriteBBTraceInterval		= ( 1 << 39 ),
		EProxyWriteBBTexName			= ( 1 << 30 ),

		// Light
		EProxyWriteLightLightType		= ( 1 << 1 ),
		EProxyWriteLightColor			= ( 1 << 2 ),

		EProxyWriteLightIntensity		= ( 1 << 8 ),
		EProxyWriteLightConeAngleCos	= ( 1 << 9 ),
		EProxyWriteLightRadius			= ( 1 << 10 ),

		EProxyWriteLightDirection		= ( 1 << 17 ),
		EProxyWriteLightFalloffAngleCos	= ( 1 << 18 ),
		EProxyWriteLightStartFalloff	= ( 1 << 19 ),
		EProxyWriteLightProjectorName	= ( 1 << 20 ),

		// Model
		EProxyWriteModelActiveAnims		= ( 1 << 1 ),
		EProxyWriteModelRagdoll			= ( 1 << 2 ),

		EProxyWriteModelMaterialName	= ( 1 << 8 ),
		EProxyWriteModelMeshVisible		= ( 1 << 9 ),
		EProxyWriteModelBlendAlpha		= ( 1 << 10 ),

		EProxyWriteModelShadowEnabled	= ( 1 << 17 ),
		EProxyWriteModelMeshAnims		= ( 1 << 18 ),

		// Particle effect
		EProxyWritePFXParentOffset			= ( 1 << 1 ),
		EProxyWritePFXParentRotation		= ( 1 << 2 ),

		EProxyWritePFXParentRotationOffset	= ( 1 << 8 ),
		EProxyWritePFXActive				= ( 1 << 9),
		EProxyWritePFXEmitterCount			= ( 1 << 10 ),

		EProxyWritePFXCameraOffset			= ( 1 << 17 ),
		EProxyWritePFXIsImmortal			= ( 1 << 18 ),
		EProxyWritePFXEmitters				= ( 1 << 19 ),

		// Trail
		EProxyWriteTrailBonesNum		= ( 1 << 1 )
	};

	EntityProxy( void );
	EntityProxy( UdpMessage* creationData );			// make a constructor like this for every child class. It should fill all contents from creation data.
	virtual ~EntityProxy( void );

	virtual int GetType( void ) { return mType; }		// each subtype should return different number

	virtual void StoreFullState( UdpMessage* mesg );					// store our full state in mesg
	virtual unsigned int StoreDiff( UdpMessage* mesg, EntityProxy* diffFrom );	// this should return 0 if it doesn't write any data
	virtual EntityProxy* CreateFromDiffAndMyself( UdpMessage* mesg, DWORD frameTimeDelta );	// make an update of myself for the new frame, using info from mesg
	virtual EntityProxy* CloneMyself( void );							// make a copy of myself for the new frame ( no diffs )

	inline unsigned int GetIndex( void ) { return mConstantData->mIndex; };

	virtual ulong32 MemoryTaken( void ) { OverallMemoryTaken += sizeof(EntityProxy); return sizeof(EntityProxy); }
	static ulong32 OverallMemoryTaken;

protected:

	virtual void ReadFullState( UdpMessage *mesg );
	virtual void CopyData( EntityProxy *target );
	virtual unsigned int ReadDiff( EntityProxy *proxy, UdpMessage *mesg, DWORD frameTimeDelta ); // returns contents
	virtual unsigned int CalcDiff( EntityProxy *diffFrom );

	int					mType;

	EntityConstantData* mConstantData;

	// Entity data - common for all types
	unsigned int	Flags;
	Vector			Translation;
	Quaternion		Rotation;
	float			Scale;
	bool			Demonic;
	float			TimeToDelete;
	float			FadeTime;
	int				Sort;
	unsigned int	ParentGIndex;
	char			ParentJoint;
	bool			TransformByParent;
};


class BillboardEntityProxy : public EntityProxy
{
public:

	BillboardEntityProxy( void );
	BillboardEntityProxy( UdpMessage *creationData );
	virtual ~BillboardEntityProxy( void );

	void StoreFullState( UdpMessage* mesg );					// store our full state in mesg
	unsigned int StoreDiff( UdpMessage* mesg, EntityProxy* diffFrom );	// this should return 0 if it doesn't write any data
	EntityProxy* CloneMyself( void );							// make a copy of myself for the new frame ( no diffs )

	virtual ulong32 MemoryTaken( void )
	{
		ulong32 mt = sizeof(BillboardEntityProxy)+TexName.MemoryTaken()-sizeof(String);
		OverallMemoryTaken += mt;
		return mt;
	}
	static ulong32 OverallMemoryTaken;

protected:

	void ReadFullState( UdpMessage *mesg );
	void CopyData( EntityProxy *target );
	unsigned int ReadDiff( EntityProxy *proxy, UdpMessage *mesg, DWORD frameTimeDelta ); // returns contents
	unsigned int CalcDiff( EntityProxy *diffFrom );

	// Entity data
	int			BillboardType;
	int			BlendMode;
	String		TexName;
	DWORD		Color;
	float		Timer;
	float		FadeInTime;
	float		FadeOutTime;
	float		Alpha;
	float		AlphaMax;
	float       Distance;
	float       MaxDistance;
	float       MinDistance;
	float       MaxRadius;
	float       MinRadius;
	float       OffDistance;
	float       TraceMargin;
	bool		IsVisible;
	bool		HitStatus;
	float		TimeDelta;
	float		TraceInterval;
};

class DecalEntityProxy : public EntityProxy
{
public:

	DecalEntityProxy( void );
	DecalEntityProxy( UdpMessage *creationData );
	virtual ~DecalEntityProxy( void );

	void StoreFullState( UdpMessage* mesg );					// store our full state in mesg
	unsigned int StoreDiff( UdpMessage* mesg, EntityProxy* diffFrom );	// this should return 0 if it doesn't write any data
	EntityProxy* CloneMyself( void );							// make a copy of myself for the new frame ( no diffs )

	virtual ulong32 MemoryTaken( void )
	{
		ulong32 mt = sizeof(DecalEntityProxy)+TextureName.MemoryTaken()-sizeof(String)+NumElems*sizeof(DecalTable);
		OverallMemoryTaken += mt;
		return mt;
	}
	static ulong32 OverallMemoryTaken;

protected:

	void ReadFullState( UdpMessage *mesg );
	void CopyData( EntityProxy *target );
	unsigned int ReadDiff( EntityProxy *proxy, UdpMessage *mesg, DWORD frameTimeDelta ); // returns contents
	unsigned int CalcDiff( EntityProxy *diffFrom );

	struct DecalTable
	{
		Vector	Pos;
		DWORD	Color;
		float	U, V;
	};

	// Entity data
	String		TextureName;
	float		ScaleFactor;
	DWORD		ConstColor;
	float		TimeToDie;
	bool		NeverDie;
	int			NumElems;
	DecalTable*	Verts;
};

class LightEntityProxy : public EntityProxy
{
public:

	LightEntityProxy( void );
	LightEntityProxy( UdpMessage *creationData );
	virtual ~LightEntityProxy( void );

	void StoreFullState( UdpMessage* mesg );					// store our full state in mesg
	unsigned int StoreDiff( UdpMessage* mesg, EntityProxy* diffFrom );	// this should return 0 if it doesn't write any data
	EntityProxy* CloneMyself( void );							// make a copy of myself for the new frame ( no diffs )

	virtual ulong32 MemoryTaken( void )
	{
		ulong32 mt = sizeof(LightEntityProxy)+ProjectorName.MemoryTaken()-sizeof(String);
		OverallMemoryTaken += mt;
		return mt;
	}
	static ulong32 OverallMemoryTaken;

protected:

	void ReadFullState( UdpMessage *mesg );
	void CopyData( EntityProxy *target );
	unsigned int ReadDiff( EntityProxy *proxy, UdpMessage *mesg, DWORD frameTimeDelta ); // returns contents
	unsigned int CalcDiff( EntityProxy *diffFrom );

	// Entity data
	int			LightType;
	DWORD		Color;
	float		Intensity;
	Vector		Direction;
	float		ConeAngleCos;
	float		FalloffAngleCos;
	float		Radius;
	float		StartFalloff;
	String		ProjectorName;
};


class ModelEntityProxy : public EntityProxy
{
	struct Animation
	{
		String	name;
		bool	looped;
		BYTE	movementCurve;
	};

	struct AnimParams
	{
		char	index;
		float	frameTime;
		float	weight;
		float	timeScale;
		bool	looped;
		bool	startAfterBlend;

		float	blendTime;

		bool operator==( const AnimParams& p ) const
		{
			return ( ( index == p.index ) &&
				( frameTime == p.frameTime ) &&
				( weight == p.weight ) &&
				( timeScale == p.timeScale ) &&
				( looped == p.looped ) &&
				( startAfterBlend == p.startAfterBlend ) &&
				( blendTime == p.blendTime ) );
		}
	};
public:

	ModelEntityProxy( void );
	ModelEntityProxy( UdpMessage *creationData );
	virtual ~ModelEntityProxy( void );

	void StoreFullState( UdpMessage* mesg );					// store our full state in mesg
	unsigned int StoreDiff( UdpMessage* mesg, EntityProxy* diffFrom );	// this should return 0 if it doesn't write any data
	EntityProxy* CloneMyself( void );							// make a copy of myself for the new frame ( no diffs )

	virtual ulong32 MemoryTaken( void )
	{
		ulong32 s = 0;
		for( int i = 0; i < Animations.Size(); ++i )
			s += Animations[i].name.MemoryTaken()-sizeof(String);

		s += sizeof(ModelEntityProxy) +
			MaterialName.MemoryTaken()-sizeof(String) +
			MeshVisible.MemoryTaken() +
			Animations.MemoryTaken() +
			Params.MemoryTaken();

		OverallMemoryTaken += s;
		return s;
	}
	static ulong32 OverallMemoryTaken;

protected:

	void ReadFullState( UdpMessage *mesg );
	void CopyData( EntityProxy *target );
	unsigned int ReadDiff( EntityProxy *proxy, UdpMessage *mesg, DWORD frameTimeDelta ); // returns contents
	unsigned int CalcDiff( EntityProxy *diffFrom );

	// Entity data
	float						BlendAlpha;
	String						MaterialName;
	bool						ShadowEnabled;

	DynamicArray<bool>			MeshVisible;
	DynamicArray<Animation>		Animations;
	DynamicArray<AnimParams>	Params;

	bool						IsRagdoll;
	DynamicArray<Vector>		JointTranslations;
	DynamicArray<Quaternion>	JointRotations;
	DynamicArray<unsigned char>	JointIndices;
};


class ParticleFXEntityProxy : public EntityProxy
{
public:

	ParticleFXEntityProxy( void );
	ParticleFXEntityProxy( UdpMessage *creationData );
	virtual ~ParticleFXEntityProxy( void );

	void StoreFullState( UdpMessage* mesg );					// store our full state in mesg
	unsigned int StoreDiff( UdpMessage* mesg, EntityProxy* diffFrom );	// this should return 0 if it doesn't write any data
	EntityProxy* CloneMyself( void );							// make a copy of myself for the new frame ( no diffs )

	virtual ulong32 MemoryTaken( void )
	{
		ulong32 mt = sizeof(ParticleFXEntityProxy);
		for( int i = 0; i < MaxEmitters; ++i )
			mt += Emitters[i].Name.MemoryTaken()-sizeof(String);
		OverallMemoryTaken += mt;
		return mt;
	}
	static ulong32 OverallMemoryTaken;

protected:

	void ReadFullState( UdpMessage *mesg );
	void CopyData( EntityProxy *target );
	unsigned int ReadDiff( EntityProxy *proxy, UdpMessage *mesg, DWORD frameTimeDelta ); // returns contents
	unsigned int CalcDiff( EntityProxy *diffFrom );

	static const int	MaxEmitters = 16;
	struct Emitter
    {
        String		Name;
		Quaternion	RotQuat;
		Vector		Position;
		Vector		LastPosition;
		bool		Active;
		bool		Respawn;
		bool		Evolve;
		bool		Immortal;
		bool		Dying;
		Vector		Pos;
		Vector		Rotation;
		float		Scale;
    };

	enum
	{
		EPFXEmitterWriteName				= ( 1 << 0 ),
		EPFXEmitterWriteRotQuat				= ( 1 << 1 ),
		EPFXEmitterWritePosition			= ( 1 << 2 ),
		EPFXEmitterWriteRotation			= ( 1 << 3 ),
		EPFXEmitterWriteFlags				= ( 1 << 4 ),
		EPFXEmitterWritePos					= ( 1 << 5 ),
		EPFXEmitterWriteScale				= ( 1 << 6 ),
		EPFXEmitterWritePositionDiffOnly    = ( 1 << 7 )
	};

	// Entity data
	Vector			ParentOffset;
    bool			ParentRotation;
    Quaternion		ParentRotationOffset;
	Vector			CameraOffset;
	bool			Active;
	bool			IsImmortal;
	int				EmitterCount;
	Emitter			Emitters[MaxEmitters];

private:

	void GetEmitterDataFromFlags( Emitter *e, unsigned char flags );
	unsigned char GetEmitterFlagsFromData( Emitter *e );
	unsigned char CalcEmitterDiff( Emitter *e, Emitter *diffFrom );
};


class TrailEntityProxy : public EntityProxy
{
public:

	TrailEntityProxy( void );
	TrailEntityProxy( UdpMessage *creationData );
	virtual ~TrailEntityProxy( void );

	void StoreFullState( UdpMessage* mesg );					// store our full state in mesg
	unsigned int StoreDiff( UdpMessage* mesg, EntityProxy* diffFrom );	// this should return 0 if it doesn't write any data
	EntityProxy* CloneMyself( void );							// make a copy of myself for the new frame ( no diffs )

	virtual ulong32 MemoryTaken( void )
	{
		int counter = ( HeadNode + MaxElems - TailNode ) % MaxElems;

		ulong32 mt = sizeof(TrailEntityProxy) +
			TextureName.MemoryTaken() - sizeof(String) +
			sizeof( Vector ) * counter * Vpn +
			sizeof( float ) * counter;

		OverallMemoryTaken += mt;
		return mt;
	}
	static ulong32 OverallMemoryTaken;

protected:

	void ReadFullState( UdpMessage *mesg );
	void CopyData( EntityProxy *target );
	unsigned int ReadDiff( EntityProxy *proxy, UdpMessage *mesg, DWORD frameTimeDelta ); // returns contents
	unsigned int CalcDiff( EntityProxy *diffFrom );

	struct TrailTable
	{
		int Bone;
		Vector LastPosition;
		Vector LastNodePosition;
	};

	// Entity data
	String		TextureName;
	float		ScaleFactor;
	DWORD		ConstColor;
	float		Time;
	bool		PositionChanged;

	int			BonesNum;
	TrailTable*	BonesData;

	int			MaxElems;
	int			HeadNode;
	int			TailNode;

	float		*Times;
	Vector		*Positions;
	int			Vpn;
};


#endif // __PKTVENTITYPROXIES_H__
