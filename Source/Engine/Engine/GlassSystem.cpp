#include "Engine.h"

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Engine/ConvexPolygon.h"
#include "Engine/Mesh.h"
#include "Physics/PhysicsEngine.h"
#include "Physics/PhysicsObject.h"
#include "Render/Video.h"
#include "Engine/GlassSystem.h"
#include "Engine/World.h"
#include "Render/Material.h"
#include "Physics/PhysicsWorld.h"

// for havokCollisionListener
#ifndef TRANSGAMING_NO_HAVOK
#include "Physics/Havok/HavokPhysics.h"
#endif

#include "limits.h"

#define GLASS_FRAGMENTS_COUNT 10
//#define GLASS_GROUP HCGNormal
#define GLASS_GROUP HCGNormalNCWithSelf

float Glass::ActivationDelay = 1;
float Glass::ActivationSpeed = 1;
float Glass::NonColliderPerc = 3;
float Glass::TimeToDelete    = 30;
float Glass::TimeToDeleteRnd = 10;
float Glass::ExplosionRangeModifier = 0.6f;

Glass::Glass( WorldMesh* glassMesh )
:	mGlassMesh(glassMesh), mActivationState(0), mStarted(false)
,	BreakupDelayed(false), mMainMeshBody(0), mFinished( false ), mCollisionListener(0)
{

	ScriptObject GlassScript = GScript.Globals()["Tweak"]["Glass"];

	ActivationDelay = GlassScript["ActivationDelay"].GetFloat();
	ActivationSpeed = GlassScript["ActivationSpeed"].GetFloat();
	NonColliderPerc = GlassScript["NonColliderPerc"].GetFloat()/10;
	TimeToDelete    = GlassScript["TimeToDelete"].GetFloat();
	TimeToDeleteRnd = GlassScript["TimeToDeleteRnd"].GetFloat();
	ExplosionRangeModifier = GlassScript["ExplosionRangeModifier"].GetFloat();

}

Glass::~Glass()
{
	Clear();
	if( mCollisionListener )
	{
#ifndef TRANSGAMING_NO_HAVOK
		delete (havokCollisionListener*)mCollisionListener;
#endif
		mCollisionListener = 0;
	}
}

void Glass::Clear( void )
{
	while( mPreloadedGlassFragments.Size() )
	{
		delete mPreloadedGlassFragments[0];
		mPreloadedGlassFragments.FastRemove(0);
	}

	while( mGlassFragments.Size() )
	{
		delete mGlassFragments[0];
		mGlassFragments.FastRemove(0);
	}

	CreationNormals.ClearArray();
	CreationMeshIndices.ClearArray();

	if( !mStarted && !mFinished )
		for( int i = 0; i < mGlassMeshes.Size(); ++i )
			if( mGlassMeshes[i] )
				delete mGlassMeshes[i];
	mGlassMeshes.ClearArray();

	mStarted = false;
}

void Glass::Tick( const float timeDelta )
{
	if( BreakupDelayed )
	{
		BreakupDelayed = false;
		StartGlass( DelayedHitPoint, DelayedActivateRadius, DelayedVelocity );
	}

	if( !mStarted || mFinished )
		return;

	mActivationState += timeDelta;
	// Activate glass fragments with the new activationState and remove them from glass
	for( int i = mGlassFragments.Size()-1; i >= 0; --i )
	{
		if( mGlassFragments[i]->ActivationTime >= mActivationState )
			break;

		Assert( mGlassFragments[i]->FragmentMesh && mGlassFragments[i]->FragmentMesh->PO );

		mGlassFragments[i]->FragmentMesh->TimeToDelete = TimeToDelete + TimeToDeleteRnd*rand()/RAND_MAX;
#ifndef TRANSGAMING_NO_HAVOK
		mGlassFragments[i]->FragmentMesh->PO->SetPinned( false );
		mGlassFragments[i]->FragmentMesh->PO->Activate( true );
#endif
		delete mGlassFragments[i];
		mGlassFragments.RemoveLast();
	}

	if( !mGlassFragments.Size() )
		mFinished = true;
}

void Glass::StartGlass( Vector hitPoint, float activateRadius, Vector velocityToGive )
{
	bool makeSound = false;

	if( !mStarted )
	{
		// Disable the original glass ( we'll need it for save data )
		Assert( mGlassMesh );
#ifdef	TRANSGAMING
#ifndef TRANSGAMING_NO_HAVOK
		GEngine->pcfWorld->Physics->RemoveStaticMesh( mMainMeshBody );
		mGlassMesh->KillAllChildren();
#endif // TRANSGAMING_NO_HAVOK
		GEngine->pcfWorld->RemoveEntity( mGlassMesh );
#else
		GEngine->World->Physics->RemoveStaticMesh( mMainMeshBody );
		mGlassMesh->KillAllChildren();
		GEngine->World->RemoveEntity( mGlassMesh );
#endif

		Breakup( hitPoint, activateRadius, velocityToGive );
		mStarted = true;
		makeSound = true;
	}
	else
		makeSound = BreakupAfterStart( hitPoint, activateRadius, velocityToGive );

	if( makeSound )
#ifdef	TRANSGAMING
		GScript.QueueMessage("BROKEN_GLASS","%f,%f,%f,%d",hitPoint.X,hitPoint.Y,hitPoint.Z, GEngine->pcfWorld->Frame );
#else
		GScript.QueueMessage("BROKEN_GLASS","%f,%f,%f,%d",hitPoint.X,hitPoint.Y,hitPoint.Z, GEngine->World->Frame );
#endif
}

void Glass::StartGlassDelayed( Vector hitPoint, float activateRadius, Vector velocityToGive )
{
	BreakupDelayed = true;
	DelayedHitPoint = hitPoint;
	DelayedActivateRadius = activateRadius;
	DelayedVelocity = velocityToGive;
}

void Glass::PreloadBreakup( void )
{
	mGlassMesh->RecomputeBox();
#ifdef	TRANSGAMING
#ifndef TRANSGAMING_NO_HAVOK
	mMainMeshBody = GEngine->pcfWorld->Physics->GetStaticMesh( mGlassMesh );
#endif //TRANSGAMING_NO_HAVOK
#else
	mMainMeshBody = GEngine->World->Physics->GetStaticMesh( mGlassMesh );
#endif
	
	DynamicArray<PolygonalMesh> meshes;
	meshes.Add(PolygonalMesh(*mGlassMesh));
	Vector extent = meshes[0].Box.GetExtent();
	Vector normal = extent.MinimalAxis();
/*	
	// Cut glass
	float maxSize = extent.X;
	if( maxSize < extent.Y ) maxSize = extent.Y;
	if( maxSize < extent.Z ) maxSize = extent.Z;
	for( int i = 0; i < 4*maxSize; ++i )
*/
	for( int i = 0; i < GLASS_FRAGMENTS_COUNT; ++i )
	{
		Vector randn, n;
		n.RandomNormalized();
		randn = n^normal;
		randn.Normalize();

		int m = rand()%meshes.Size();
		Plane randomPlane(randn, meshes[m].Box.GetCenter());

		PolygonalMesh front, back;
		int res = meshes[m].Split(randomPlane, front, back);

		if( res == 0 )
		{
			Assert(front.Box.Good());
			Assert(back.Box.Good());
			meshes[m] = front;
			meshes.Add(back);

			CreationNormals.Add( randn );
			CreationMeshIndices.Add( m );
		}
	}

	// Texture assignment
	String subPath;
#ifdef	TRANSGAMING
	if (GEngine->pcfWorld->Name.Size())
		subPath = String("Levels/")+GEngine->pcfWorld->Name+"/";
#else
	if (GEngine->World->Name.Size())
		subPath = String("Levels/")+GEngine->World->Name+"/";
#endif
	String newName = mGlassMesh->Data->Groups[0].TexName + "_broken";

	if( !GEngine->Video->MaterialSys->IsTextureOnDisk((subPath+newName).AsChar()) )
		newName = "";

#ifndef FINAL_BUILD
	float Time1 = GEngine->SysDriver.GetCurrentTime();
#endif

	// Make glass meshes
	DynamicArray<WorldMesh*> newMeshes;

	for( int i = 0; i < meshes.Size(); ++i )
	{
		// Mesh naming
        m_setTag("MeshGlass");
		newMeshes.Add( new WorldMesh(*mGlassMesh) );
		newMeshes[i]->Name += String::Sprintf("phys%.2d",i);
		Assert(meshes[i].Box.Good());

		meshes[i].CreateMesh( *newMeshes[i] );

		if( newMeshes[i] && newName.Len() )
			newMeshes[i]->Data->Groups[0].TexName = newName;

		newMeshes[i]->RecomputeBox();
		Vector ctr = newMeshes[i]->Box.GetCenter();
		newMeshes[i]->CenterGeometry();
		newMeshes[i]->SetPosition( newMeshes[i]->GetPosition() + ctr );
		newMeshes[i]->Flags &= ~Entity::EEntDontDeleteOnTimer;
	}

#ifndef FINAL_BUILD
	float Time2 = GEngine->SysDriver.GetCurrentTime();
#endif


#ifndef FINAL_BUILD
	float Time3 = GEngine->SysDriver.GetCurrentTime();
#endif


	for( int i = 0; i < meshes.Size(); ++i )
	{

#ifndef	TRANSGAMING_NO_HAVOK
#ifdef	TRANSGAMING
		newMeshes[i]->PO = GEngine->pcfWorld->Physics->CreatePhysicsObject( 
#else
		newMeshes[i]->PO = GEngine->World->Physics->CreatePhysicsObject( 
#endif
		newMeshes[i]->GetPosition(), newMeshes[i]->GetRotation(), EPawnFromMeshGlassSweep, newMeshes[i], 0.1f, GLASS_GROUP, true );
#endif // TRANSGAMING_NO_HAVOK
		if( !newMeshes[i]->PO )
		{
//			GLog.Print("Warning: Glass fragment not created!\n");
//			GEngine->World->AddEntity( newMeshes[i] );
			delete newMeshes[i];
			mGlassMeshes.Add(0);
			continue;
		}
		else
		{
#ifndef	TRANSGAMING_NO_HAVOK
			newMeshes[i]->PO->EnableLineTraceCollision( true );
			newMeshes[i]->PO->SetAsGlass( this );
#endif
            m_setTag("MeshGlass");
			GlassFragment* gf = new GlassFragment;
			gf->FragmentMesh = newMeshes[i];
			mPreloadedGlassFragments.Add( gf );
			mGlassMeshes.Add( newMeshes[i] );
#ifdef	TRANSGAMING
			GEngine->pcfWorld->PtrMap[newMeshes[i]] = 0;
#else
			GEngine->World->PtrMap[newMeshes[i]] = 0;
#endif
		}
	}

#if defined(TRANSGAMING) && defined(FIND_HASH_PEAK)
    if (GEngine->pcfWorld->PtrMap.Size() > GEngine->pcfWorld->mMaxPtrMapUsage){
        GEngine->pcfWorld->mMaxPtrMapUsage = GEngine->pcfWorld->PtrMap.Size();
        TRACE("@@@ Glass::PreloadBreakup(): peak PtrMap hash usage = %d\n", GEngine->pcfWorld->mMaxPtrMapUsage);
    }
#endif

	// Prepare meshes for display
	for( int i = 0; i < mGlassMeshes.Size(); ++i ) {
		if (mGlassMeshes[i]) {
			mGlassMeshes[i]->Init();

			BoundingBox lBox = mGlassMeshes[i]->LocalBox;

			mGlassMeshes[i]->RenderInitialize(GEngine->Video->RenderDev,GEngine->Video->MaterialSys, subPath.AsChar());

			mGlassMeshes[i]->LocalBox = lBox;
		}
	}

#ifndef FINAL_BUILD
	float Time4 = GEngine->SysDriver.GetCurrentTime();
	GLog.Print( "Glass preload time: %f ( RenderInitialize(): %f, other: %f )\n", Time4-Time1, Time3-Time2, Time2-Time1+Time4-Time3 );
#endif
}

void Glass::Breakup(const Vector &hitPoint, float activationRadius, Vector velocityToGive )
{
	Vector center = hitPoint;
	
	Vector extent = mGlassMesh->Box.GetExtent();
	Vector normal = extent.MinimalAxis();
	
	float velToGiveMagnitude = velocityToGive.Magnitude();
	if( velToGiveMagnitude < 0.01f ) velToGiveMagnitude = 0.01f;

	// Add fragments to world
	for( int i = 0; i < mPreloadedGlassFragments.Size(); ++i )
#ifdef	TRANSGAMING
		GEngine->pcfWorld->AddEntity( mPreloadedGlassFragments[i]->FragmentMesh );
#else
		GEngine->World->AddEntity( mPreloadedGlassFragments[i]->FragmentMesh );
#endif

	while( mPreloadedGlassFragments.Size() )
	{
		// Find the vertex with closest distance to hitPoint
		Vector closestPoint;// = mPreloadedGlassFragments[0]->FragmentMesh->Box.GetCenter();
		float closestDist;// = (closestPoint-hitPoint).MagnitudeSquared();

		//DynamicArray<MeshVertex>& vertices = mPreloadedGlassFragments[0]->FragmentMesh->Verts;
        //VertexList vertices = mPreloadedGlassFragments[0]->FragmentMesh->Data->VertList;


        mPreloadedGlassFragments[0]->FragmentMesh->Data->VertList.getClosestPoint(hitPoint, mPreloadedGlassFragments[0]->FragmentMesh->Box.GetCenter(), closestDist, closestPoint);
		/*for( int i = 0; i < vertices.Size(); ++i )
		{
            Vector &pos = vertices.getVertPos(i);
			float   magn = (pos - hitPoint).MagnitudeSquared();

			if( magn < closestDist )
			{
				closestDist = magn;
				closestPoint = pos;
			}
		}//*/

		if( closestDist > 0.0001f )
		{
            mPreloadedGlassFragments[0]->FragmentMesh->Data->VertList.getClosestPoint2(hitPoint, closestDist, closestPoint);
			/*Vector closestFromLine;

			for( int i = 0; i < vertices.Size(); ++i )
			{
                Vector &pos = vertices.getVertPos(i);
				Vector toHP2 = hitPoint - pos;


				for( int j = i+1; j < vertices.Size(); ++j )
				{
                    Vector &    pos2 = vertices.getVertPos(j);
					Vector      line = pos - pos2;
					Vector      toHP1 = hitPoint - pos2;


					if( ( line.Dot(toHP1) > 0 ) && ( line.Dot(toHP2) < 0 ) )
					{
						closestFromLine.SetToClosestFromLine( hitPoint, pos, pos2 );
						float magn = (closestFromLine-hitPoint).MagnitudeSquared();
						if( magn < closestDist )
						{
							closestDist = magn;
							closestPoint = closestFromLine;
						}
					}
				}
			}//*/
		}

		float hitRange = (closestPoint-hitPoint).Magnitude();

		// Set activation time
		if( activationRadius > 0 )
			mPreloadedGlassFragments[0]->ActivationTime = (hitRange-activationRadius)/ActivationSpeed;
		else
			mPreloadedGlassFragments[0]->ActivationTime = ActivationDelay+hitRange/ActivationSpeed;

#ifndef	TRANSGAMING_NO_HAVOK
		mPreloadedGlassFragments[0]->FragmentMesh->PO->SetAngularDamping( 0.3f );
		if( mPreloadedGlassFragments[0]->ActivationTime < 0 )
		{
			mPreloadedGlassFragments[0]->FragmentMesh->TimeToDelete = TimeToDelete + TimeToDeleteRnd*rand()/RAND_MAX;
			Vector vel = mPreloadedGlassFragments[0]->FragmentMesh->PO->GetCenterOfMass() - hitPoint;
			float velMagnitude = vel.Magnitude();
			if( velMagnitude < 0.01f ) velMagnitude = 0.01f;
			if( hitRange < 0.01f ) hitRange = 0.01f;
			float velToSet = velToGiveMagnitude/hitRange;
			vel *= ( velToSet/velMagnitude );
			vel += velocityToGive;

			mPreloadedGlassFragments[0]->FragmentMesh->PO->Enable( true );
			mPreloadedGlassFragments[0]->FragmentMesh->PO->Activate( true );
			mPreloadedGlassFragments[0]->FragmentMesh->PO->SetVel( vel );
			mPreloadedGlassFragments[0]->FragmentMesh->PO->Hit( hitPoint, velocityToGive*velToSet/velToGiveMagnitude );
			delete mPreloadedGlassFragments[0];
		}
		else
		{
			mPreloadedGlassFragments[0]->FragmentMesh->PO->Enable( true );
			mPreloadedGlassFragments[0]->FragmentMesh->PO->SetPinned( true );
			mGlassFragments.Add( mPreloadedGlassFragments[0] );
		}
#endif
		mPreloadedGlassFragments.FastRemove(0);
	}

	// Lowest last
	for( int i = 0; i < mGlassFragments.Size(); ++i )
	{
		float highestTime = mGlassFragments[i]->ActivationTime;
		int index = i;

		for( int j = i+1; j < mGlassFragments.Size(); ++j )
			if( mGlassFragments[j]->ActivationTime > highestTime )
			{
				highestTime = mGlassFragments[j]->ActivationTime;
				index = j;
			}

		GlassFragment* temp = mGlassFragments[i];
		mGlassFragments[i] = mGlassFragments[index];
		mGlassFragments[index] = temp;
	}
}

bool Glass::BreakupAfterStart(const Vector &hitPoint, float activationRadius, Vector velocityToGive )
{
	bool makeSound = false;

	Vector center = hitPoint;
	
	Vector extent = mGlassMesh->Box.GetExtent();
	Vector normal = extent.MinimalAxis();
	
	float velToGiveMagnitude = velocityToGive.Magnitude();
	if( velToGiveMagnitude < 0.01f ) velToGiveMagnitude = 0.01f;

	for( int i = 0; i < mGlassFragments.Size(); ++i )
	{
		// Find the vertex with closest distance to hitPoint
		Vector closestPoint;// = mGlassFragments[i]->FragmentMesh->Box.GetCenter();
		float closestDist;// = (closestPoint-hitPoint).MagnitudeSquared();

		//DynamicArray<MeshVertex>& vertices = mGlassFragments[i]->FragmentMesh->Verts;
        //VertexList vertices = mGlassFragments[i]->FragmentMesh->Data->VertList;


        mGlassFragments[i]->FragmentMesh->Data->VertList.getClosestPoint(hitPoint, mGlassFragments[i]->FragmentMesh->Box.GetCenter(), closestDist, closestPoint);
		/*for( int j = 0; j < vertices.Size(); ++j )
		{
            Vector &pos = vertices.getVertPos(j);
			float   magn = (pos - hitPoint).MagnitudeSquared();


			if( magn < closestDist )
			{
				closestDist = magn;
				closestPoint = pos;
			}
		}//*/

		if( closestDist > 0.0001f )
		{
            mGlassFragments[i]->FragmentMesh->Data->VertList.getClosestPoint2(hitPoint, closestDist, closestPoint);
			/*Vector closestFromLine;

			for( int j = 0; j < vertices.Size(); ++j )
			{
                Vector &    pos = vertices.getVertPos(j);
				Vector      toHP2 = hitPoint - pos;


				for( int k = j+1; k < vertices.Size(); ++k )
				{
                    Vector &    pos2 = vertices.getVertPos(k);
					Vector      line = pos - pos2;
					Vector      toHP1 = hitPoint - pos2;


					if( ( line.Dot(toHP1) > 0 ) && ( line.Dot(toHP2) < 0 ) )
					{
						closestFromLine.SetToClosestFromLine( hitPoint, pos, pos2 );
						float magn = (closestFromLine-hitPoint).MagnitudeSquared();
						if( magn < closestDist )
						{
							closestDist = magn;
							closestPoint = closestFromLine;
						}
					}
				}
			}//*/
		}

		float hitRange = (closestPoint-hitPoint).Magnitude();

		// Set activation time
		float time;
		if( activationRadius > 0 )
			time = (hitRange-activationRadius)/ActivationSpeed;
		else
			time = ActivationDelay+hitRange/ActivationSpeed;
		if( time < mGlassFragments[i]->ActivationTime )
			mGlassFragments[i]->ActivationTime = time;

#ifndef TRANSGAMING_NO_HAVOK
		if( mGlassFragments[i]->ActivationTime < 0 )
		{
			makeSound = true;
			mGlassFragments[i]->FragmentMesh->TimeToDelete = TimeToDelete + TimeToDeleteRnd*rand()/RAND_MAX;
			Vector vel = mGlassFragments[i]->FragmentMesh->PO->GetCenterOfMass() - hitPoint;
			float velMagnitude = vel.Magnitude();
			if( velMagnitude < 0.01f ) velMagnitude = 0.01f;
			if( hitRange < 0.01f ) hitRange = 0.01f;
			float velToSet = velToGiveMagnitude/hitRange;
			vel *= ( velToSet/velMagnitude );
			vel += velocityToGive;

			mGlassFragments[i]->FragmentMesh->PO->Enable( true );
			mGlassFragments[i]->FragmentMesh->PO->SetPinned( false );
			mGlassFragments[i]->FragmentMesh->PO->Activate( true );
			mGlassFragments[i]->FragmentMesh->PO->SetVel( vel );
			mGlassFragments[i]->FragmentMesh->PO->Hit( hitPoint, velocityToGive*velToSet/velToGiveMagnitude );
			delete mGlassFragments[i];
			mGlassFragments.FastRemove(i);
			i--;
		}
		else
		{
			mGlassFragments[i]->FragmentMesh->PO->Enable( true );
			mGlassFragments[i]->FragmentMesh->PO->SetPinned( true );
		}
#endif
	}

	// Lowest last
	for( int i = 0; i < mGlassFragments.Size(); ++i )
	{
		float highestTime = mGlassFragments[i]->ActivationTime;
		int index = i;

		for( int j = i+1; j < mGlassFragments.Size(); ++j )
			if( mGlassFragments[j]->ActivationTime > highestTime )
			{
				highestTime = mGlassFragments[j]->ActivationTime;
				index = j;
			}

		GlassFragment* temp = mGlassFragments[i];
		mGlassFragments[i] = mGlassFragments[index];
		mGlassFragments[index] = temp;
	}

	return makeSound;
}

void Glass::ActivateFragment( void* rigidBody, Vector hitPoint, Vector velocityToSet )
{
	for( int i = mGlassFragments.Size()-1; i >= 0; --i )
	{
#ifndef TRANSGAMING_NO_HAVOK
		Assert( mGlassFragments[i]->FragmentMesh && mGlassFragments[i]->FragmentMesh->PO );
		if( mGlassFragments[i]->FragmentMesh->PO->GetHavokBody() == rigidBody )
		{
			mGlassFragments[i]->FragmentMesh->TimeToDelete = TimeToDelete + TimeToDeleteRnd*rand()/RAND_MAX;
			mGlassFragments[i]->FragmentMesh->PO->SetPinned( false );
			mGlassFragments[i]->FragmentMesh->PO->Activate( true );
			mGlassFragments[i]->FragmentMesh->PO->Hit( hitPoint, velocityToSet );
			delete mGlassFragments[i];
			mGlassFragments.Remove(i);
			if( !mGlassFragments.Size() )
				mFinished = true;
			return;
		}
#endif
	}
}

#define GLASS_LOAD_SAVE_TOKEN 0x5e70

bool Glass::LoadGlass( FileObject* f )
{
	if( !f->TestToken( GLASS_LOAD_SAVE_TOKEN ) )
	{
		GLog.Print( "Token test failure: Glass::LoadGlass()\n" );
		return false;
	}

	Clear();

	// Load internal state
	Assert( mGlassMesh );

	char temp;
	String name;

	*f >> name; Assert( name == mGlassMesh->Name );
	*f >> temp; mStarted = (temp != 0);
	*f >> temp; mFinished = (temp != 0);
	*f >> mActivationState;
	*f >> temp; BreakupDelayed = (temp != 0);
	*f >> DelayedHitPoint;
	*f >> DelayedActivateRadius;
	*f >> DelayedVelocity;

	// Create glass meshes ( from loaded data )
	if( !BreakupFromFile( f ) ) return false;

	// Texture assignment
	String subPath;
#ifdef	TRANSGAMING
	if (GEngine->pcfWorld->Name.Size())
		subPath = String("Levels/")+GEngine->pcfWorld->Name+"/";
#else
	if (GEngine->World->Name.Size())
		subPath = String("Levels/")+GEngine->World->Name+"/";
#endif

	// Load created meshes, and put them in appropriate tables ( or destroy them )
	for( int i = 0; i < mGlassMeshes.Size(); ++i )
	{
		*f >> temp;
		bool haveIt = ( temp != 0 );
		if( haveIt )
		{
			Assert( mGlassMeshes[i] != 0 );

			// Prepare meshes for display
			mGlassMeshes[i]->Init();
			mGlassMeshes[i]->RenderInitialize( GEngine->Video->RenderDev,GEngine->Video->MaterialSys, subPath.AsChar() );
			mGlassMeshes[i]->RecomputeBox();
			mGlassMeshes[i]->UpdateTransform();
			Assert( mGlassMeshes[i]->LocalBox.Good() );

			// Pass the unneeded data
			int type;
			String filename, name;
			float scale;
			char centered;
			*f >> type >> filename >> name >> scale >> centered;

			Assert( !mGlassMeshes[i]->PO );

			if( !mGlassMeshes[i]->LoadEntity( f ) )
				return false;

			char shouldBeInWorld;
			*f >> shouldBeInWorld;

#ifdef	TRANSGAMING
			if( shouldBeInWorld )
				GEngine->pcfWorld->AddEntity( mGlassMeshes[i] );

			GEngine->pcfWorld->PtrMap[mGlassMeshes[i]] = 0;
#else
			if( shouldBeInWorld )
				GEngine->World->AddEntity( mGlassMeshes[i] );

			GEngine->World->PtrMap[mGlassMeshes[i]] = 0;
#endif
#ifndef TRANSGAMING_NO_HAVOK
			if( mGlassMeshes[i]->PO )
				mGlassMeshes[i]->PO->SetAsGlass( this );
			else
			{
				GLog.Print( "Warning: creation of PO for glass fragment failed. Destroying fragment in next tick.\n" );
				mGlassMeshes[i]->TimeToDelete = 0;
			}
#endif
			int index;
			*f >> index;
			if( index != -1 )
			{
				while( mPreloadedGlassFragments.Size() < index+1 )
					mPreloadedGlassFragments.Add(0);
				Assert( !mPreloadedGlassFragments[index] );
                m_setTag("MeshGlass");
                mPreloadedGlassFragments[index] = new GlassFragment;
				mPreloadedGlassFragments[index]->FragmentMesh = mGlassMeshes[i];
				mPreloadedGlassFragments[index]->ActivationTime = 0;
			}

			*f >> index;
			if( index != -1 )
			{
				while( mGlassFragments.Size() < index+1 )
					mGlassFragments.Add(0);
				Assert( !mGlassFragments[index] );
                m_setTag("MeshGlass");
				mGlassFragments[index] = new GlassFragment;
				mGlassFragments[index]->FragmentMesh = mGlassMeshes[i];
				*f >> mGlassFragments[index]->ActivationTime;
			}
		}
		else
		{
			Assert( mGlassMeshes[i] != 0 );
			delete mGlassMeshes[i];
			mGlassMeshes[i] = 0;
		}
	}

#if defined(TRANSGAMING) && defined(FIND_HASH_PEAK)
    if (GEngine->pcfWorld->PtrMap.Size() > GEngine->pcfWorld->mMaxPtrMapUsage){
        GEngine->pcfWorld->mMaxPtrMapUsage = GEngine->pcfWorld->PtrMap.Size();
        TRACE("@@@ Glass::LoadGlass(): peak PtrMap hash usage = %d\n", GEngine->pcfWorld->mMaxPtrMapUsage);
    }
#endif


#ifdef _DEBUG
	for( int i = 0; i < mPreloadedGlassFragments.Size(); ++i ) { ; Assert( mPreloadedGlassFragments[i] ); }
	for( int i = 0; i < mGlassFragments.Size(); ++i ) { ; Assert( mGlassFragments[i] ); }
#endif

	if( mStarted || mFinished )
	{
		// Disable the original glass ( we'll need it for save data )
		Assert( mGlassMesh );
		if( mGlassMesh->PO )
			mGlassMesh->PO->Enable(false);
#ifndef	TRANSGAMING_NO_HAVOK
		mGlassMesh->KillAllChildren();
#endif

#ifdef	TRANSGAMING
		GEngine->pcfWorld->RemoveEntity( mGlassMesh );
#else
		GEngine->World->RemoveEntity( mGlassMesh );
#endif
	}

	return true;
}

bool Glass::SaveGlass( FileObject* f )
{
	if( !f->PutToken( GLASS_LOAD_SAVE_TOKEN ) ) return false;

	// Save internal state
	Assert( mGlassMesh );

	char temp;

	*f << mGlassMesh->Name;
	temp = mStarted?1:0; *f << temp;
	temp = mFinished?1:0; *f << temp;
	*f << mActivationState;
	temp = BreakupDelayed?1:0; *f << temp;
	*f << DelayedHitPoint;
	*f << DelayedActivateRadius;
	*f << DelayedVelocity;

	// Save data to make all glass meshes
	int count = CreationNormals.Size();
	*f << count;
	for( int i = 0; i < count; ++i )
	{
		*f << CreationNormals[i];
		*f << CreationMeshIndices[i];
	}

	// Save all glass meshes we have ( what tables they're in, and their data )
	for( int i = 0; i < count+1; ++i )
	{
		temp = mGlassMeshes[i]?1:0;
		*f << temp;
		if( temp )
		{
			if( !mGlassMeshes[i]->SaveEntity( f ) )
				return false;

			char inWorld = mGlassMeshes[i]->WorldLink?1:0;
			*f << inWorld;

			int index = -1;
			for( int j = 0; j < mPreloadedGlassFragments.Size(); ++j )
				if( mPreloadedGlassFragments[j]->FragmentMesh == mGlassMeshes[i] )
				{
					index = j;
					break;
				}
			*f << index;
			index = -1;
			for( int j = 0; j < mGlassFragments.Size(); ++j )
				if( mGlassFragments[j]->FragmentMesh == mGlassMeshes[i] )
				{
					index = j;
					break;
				}
			*f << index;
			if( index != -1 )
				*f << mGlassFragments[index]->ActivationTime;
		}
	}

	return true;
}

void Glass::ForgetFragment( WorldMesh* fragment )
{
	for( int i = 0; i < mGlassMeshes.Size(); ++i )
		if( mGlassMeshes[i] && (mGlassMeshes[i] == fragment ) )
		{
			mGlassMeshes[i] = 0;
			return;
		}
}

bool Glass::BreakupFromFile( FileObject* f )
{
	CreationNormals.ClearArray();
	CreationMeshIndices.ClearArray();
	mGlassMeshes.ClearArray();

	mGlassMesh->RecomputeBox();
#ifdef	TRANSGAMING
#ifndef	TRANSGAMING_NO_HAVOK
	mMainMeshBody = GEngine->pcfWorld->Physics->GetStaticMesh( mGlassMesh );
#else
	mMainMeshBody = NULL;
#endif //TRANSGAMING_NO_HAVOK
#else
	mMainMeshBody = GEngine->World->Physics->GetStaticMesh( mGlassMesh );
#endif
	
	DynamicArray<PolygonalMesh> meshes;
	meshes.Add(PolygonalMesh(*mGlassMesh));
	Vector extent = meshes[0].Box.GetExtent();
	Vector normal = extent.MinimalAxis();
	
	int count;
	*f >> count;
	for( int i = 0; i < count; ++i )
	{
		Vector randn;
		*f >> randn;
		CreationNormals.Add( randn );
		int m;
		*f >> m;
        CreationMeshIndices.Add( m );

		Plane randomPlane(randn, meshes[m].Box.GetCenter());

		PolygonalMesh front, back;
		int res = meshes[m].Split(randomPlane, front, back);

		// This is emergency - when engine split differently before save than from save.
		// It will produce different results,
		// but it will not crash the engine.
		while( res != 0 )
		{
			Vector n;
			n.RandomNormalized();
			randn = n^normal;
			randn.Normalize();
			Plane randomPlane(randn, meshes[m].Box.GetCenter());
			res = meshes[m].Split(randomPlane, front, back);
		}

		Assert(front.Box.Good());
		Assert(back.Box.Good());

		meshes[m] = front;
		meshes.Add(back);
	}
		
	// Texture assignment
	String subPath;
#ifdef	TRANSGAMING
	if (GEngine->pcfWorld->Name.Size())
		subPath = String("Levels/")+GEngine->pcfWorld->Name+"/";
#else
	if (GEngine->World->Name.Size())
		subPath = String("Levels/")+GEngine->World->Name+"/";
#endif
	String newName = mGlassMesh->Data->Groups[0].TexName + "_broken";

	if( !GEngine->Video->MaterialSys->IsTextureOnDisk((subPath+newName).AsChar()) )
		newName = "";

	// Make glass meshes
	for( int i = 0; i < meshes.Size(); ++i )
	{
		// Mesh naming
        m_setTag("MeshGlass");
		mGlassMeshes.Add( new WorldMesh(*mGlassMesh) );
		mGlassMeshes[i]->Name += String::Sprintf("phys%.2d",i);
		Assert(meshes[i].Box.Good());

		meshes[i].CreateMesh( *mGlassMeshes[i] );

		if( mGlassMeshes[i] && newName.Len() )
			mGlassMeshes[i]->Data->Groups[0].TexName = newName;

		mGlassMeshes[i]->RecomputeBox();
		Vector ctr = mGlassMeshes[i]->Box.GetCenter();
		mGlassMeshes[i]->CenterGeometry();
		mGlassMeshes[i]->SetPosition( mGlassMeshes[i]->GetPosition() + ctr );
		mGlassMeshes[i]->Flags &= ~Entity::EEntDontDeleteOnTimer;
	}

	return true;
}
