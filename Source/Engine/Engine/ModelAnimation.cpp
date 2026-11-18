#include "Engine.h"
#include "Engine/ModelAnimation.h"
#include "Engine/AnimatedMesh.h"
#include "Engine/Model.h"


//define this to allow the use of non-uniform keyframes in skeletal animations.
#define TRANSGAMING_USE_NONUNIFORM_KEYS

/*=====================================================================================*/

void ModelAnimation::ComputeFrameMatrix(DynamicArray<Matrix>& frameMatrixArr, float, const Model* , const Matrix &world) const
{
    frameMatrixArr = MyAnimatedMesh->ComputeBindMatrix(world);
}

ModelAnimation::~ModelAnimation()
{
}

/*=====================================================================================*/

SkeletalAnimation::~SkeletalAnimation()
{
	if (MovementCurve)
		delete MovementCurve;
}

#ifdef TRANSGAMING
void SkeletalAnimation::SetMovementCurve(const char *name, DWORD MovementCurves)
#else
void SkeletalAnimation::SetMovementCurve(const String& name, DWORD MovementCurves)
#endif
{
	if (!MovementCurve)
	{
		AnimationCurve *curve  = FindCurve(name);

		if (curve)
	{	
            m_setTag("Mesh");
			MovementCurve = new AnimationCurve(*curve);
			MovementFlags = MovementCurves;
			MovementDelta.Set(0,0,0);
			RotationDelta=IdentityQuaternion();

			DynamicArray<Key>   &keys = curve->Keys;
			DynamicArray<Key>   &mkeys = MovementCurve->Keys;

			int nKeys = keys.Size();

			// reset movement

			for (int i=0; i<nKeys; i++)
			{
				if (MovementCurves & ETransX)
					keys[i].Pos.X = keys[0].Pos.X;
				else
					mkeys[i].Pos.X = mkeys[0].Pos.X;

				if (MovementCurves & ETransY)
					keys[i].Pos.Y = keys[0].Pos.Y;
				else
					mkeys[i].Pos.Y = mkeys[0].Pos.Y;

				if (MovementCurves & ETransZ)
					keys[i].Pos.Z = keys[0].Pos.Z;
				else
					mkeys[i].Pos.Z = mkeys[0].Pos.Z;

				if (MovementCurves & ERot)
					keys[i].Rot = IdentityQuaternion();
				else
					mkeys[i].Rot = IdentityQuaternion();
			}

			if (MovementCurves & (ETransX|ETransY|ETransZ))
			{
				int k;

				if (Looped)
				{
					k = nKeys-1;
					mkeys[k].Pos = mkeys[0].Pos;
				}
				else
				{
					k = nKeys;
				}

				MovementDelta = (mkeys[k-1].Pos - mkeys[0].Pos) / Time;

				for (int i=0; i<k; i++)
				{
					mkeys[i].Pos -= MovementDelta*mkeys[i].Frame;
				}
			}

			if (MovementCurves & ERot)
			{
				int k;

				if (Looped)
				{
					k = nKeys-1;
					mkeys[k].Rot = mkeys[0].Rot;
				}
				else
				{
					k = nKeys;
				}

				//RotationDelta = (mkeys[k-1].Rot - mkeys[0].Rot) / Time;

				//for (int i=0; i<k; i++)
				//{
				//	mkeys[i].Rot -= RotationDelta*mkeys[i].Frame;
				//}
			}
		}
	}
}

Vector SkeletalAnimation::GetMovement(const float time) const
{
	if (MovementCurve)
    {
		int 
			pKey,
			nKey;
		float 
			t;

		ComputeKeysTime(fmodf(time, Time), t, pKey, nKey);

		const DynamicArray<Key>& keys = MovementCurve->Keys;

		return (keys[pKey].Pos*(1-t) + keys[nKey].Pos*t) + MovementDelta*time;
    }

	return Vector(0,0,0);
}

Quaternion SkeletalAnimation::GetRotation(const float time) const
{
	if (MovementCurve)
    {
		int 
			pKey,
			nKey;
		float 
			t;

		ComputeKeysTime(fmodf(time, Time), t, pKey, nKey);

		const DynamicArray<Key>& keys = MovementCurve->Keys;

		return Quaternion::Slerp(t, keys[pKey].Rot, keys[nKey].Rot);
    }

	return IdentityQuaternion();
}

void SkeletalAnimation::ComputeFrameMatrix(DynamicArray<Matrix>& frameMatrixArr, float time, const Model* model, const Matrix &world) const
{
	int 
		pKey,
		nKey,
		index = 0;
	float 
		t;

	ComputeKeysTime(time, t, pKey, nKey);

    for (int i=0; i<MyAnimatedMesh->Skeleton.Hierarchies; i++)
    {
        ComputeFrameMatrix(world, frameMatrixArr, index, model, t, pKey, nKey);
    }
}

void SkeletalAnimation::ComputeKeysTime(float time, float &t, int &pKey, int &nKey) const
{
	pKey  = 0,
	nKey  = 0,
	t = 0;

	if (time<=0)
	{
		return;
	}

	const DynamicArray<Key>& keys = AnimationCurves[0].Keys;
	int keyNum = keys.Size();

	if (keyNum <= 0)
		return;

	if (time>=Time)
	{
		pKey = nKey = keyNum-1;
		return;
	}
	else if (AnimationCurves.Size()>=2)
	{
#ifndef TRANSGAMING_USE_NONUNIFORM_KEYS
        float keysPerSec = 1.0f / (keys[1].Frame - keys[0].Frame);
#endif

		pKey = keyNum-1;

        //search backward if requesting a key in the upper half of the animation set
        if (time > Time * 0.5f){
		    for (int iKey = keyNum - 2; iKey >= 1; iKey--){
			    if (time > keys[iKey].Frame){
				    nKey = iKey + 1;
				    pKey = iKey;

				    break;
			    }
		    }
        }

        //search forward if requesting a key in the lower half of the animation set
        else{
		    for (int iKey=1; iKey<keyNum; iKey++)
		    {
			    if (time <= keys[iKey].Frame)
			    {
				    nKey = iKey;
				    pKey = iKey-1;

				    break;
			    }
		    }
        }

#ifdef TRANSGAMING_USE_NONUNIFORM_KEYS
		t = (time - keys[pKey].Frame) / (keys[nKey].Frame - keys[pKey].Frame);
#else
		t = (time-keys[pKey].Frame)*keysPerSec;
#endif
	}
}

void SkeletalAnimation::ComputeFrameMatrix(Matrix m, const DynamicArray<Matrix>& matrix, int& index, const Model* model,
										   float t, int pKey, int nKey) const
{
    AnimationCurve* curve = FindCurve(index);
    Joint& joint = MyAnimatedMesh->Skeleton.Joints[index];

	if (curve)
    {
		const DynamicArray<Key>& keys = curve->Keys;
		
		Matrix mm;

		Quaternion qslerp = Quaternion::Slerp(t, keys[pKey].Rot, keys[nKey].Rot);

		// apply additional per-model transformation on joint
		const DynamicArray<AnimatedJoint>	&AnimatedJoints = model->AnimatedJoints;

		for (int i=0; i<AnimatedJoints.Size(); i++)
		{
			if (AnimatedJoints[i].Joint==index)
			{
				qslerp = AnimatedJoints[i].Angle*qslerp;
				break;
			}
		}

		mm = qslerp.ToMatrix();

		mm.SetTranslation(keys[pKey].Pos*(1-t) + keys[nKey].Pos*t);

		Matrix rhs = m;

		m.A00 = mm.A00 * rhs.A00 + mm.A01 * rhs.A10 + mm.A02 * rhs.A20;
		m.A01 = mm.A00 * rhs.A01 + mm.A01 * rhs.A11 + mm.A02 * rhs.A21;
		m.A02 = mm.A00 * rhs.A02 + mm.A01 * rhs.A12 + mm.A02 * rhs.A22;
		m.A03 = 0;

		m.A10 = mm.A10 * rhs.A00 + mm.A11 * rhs.A10 + mm.A12 * rhs.A20;
		m.A11 = mm.A10 * rhs.A01 + mm.A11 * rhs.A11 + mm.A12 * rhs.A21;
		m.A12 = mm.A10 * rhs.A02 + mm.A11 * rhs.A12 + mm.A12 * rhs.A22;
		m.A13 = 0;

		m.A20 = mm.A20 * rhs.A00 + mm.A21 * rhs.A10 + mm.A22 * rhs.A20;
		m.A21 = mm.A20 * rhs.A01 + mm.A21 * rhs.A11 + mm.A22 * rhs.A21;
		m.A22 = mm.A20 * rhs.A02 + mm.A21 * rhs.A12 + mm.A22 * rhs.A22;
		m.A23 = 0;

		m.A30 = mm.A30 * rhs.A00 + mm.A31 * rhs.A10 + mm.A32 * rhs.A20 + rhs.A30;
		m.A31 = mm.A30 * rhs.A01 + mm.A31 * rhs.A11 + mm.A32 * rhs.A21 + rhs.A31;
		m.A32 = mm.A30 * rhs.A02 + mm.A31 * rhs.A12 + mm.A32 * rhs.A22 + rhs.A32;
		m.A33 = 1;

    }
    else
    {
        m = joint.Mat*m;
    }

    matrix[index++] = m;

    for (int iChild=0; iChild<joint.Children; iChild++)
    {
        ComputeFrameMatrix(m, matrix, index, model, t, pKey, nKey);
    }
}

DynamicArray<Matrix> SkeletalAnimation::ComputeKeyMatrix(int key, float scale) const
{
    int index = 0;
    DynamicArray<Joint> &joints = MyAnimatedMesh->Skeleton.Joints;

    DynamicArray<Matrix> frameMatrixArr;
    frameMatrixArr.ResizeArray(joints.Size());
	Matrix world = ScaleMatrix(scale);

    for (int i=0; i<MyAnimatedMesh->Skeleton.Hierarchies; i++)
    {
        ComputeKeyMatrix(key, world, frameMatrixArr, index);
    }

    return frameMatrixArr;
}

void SkeletalAnimation::ComputeKeyMatrix(int key, Matrix m, const DynamicArray<Matrix>& matrix, int& index) const
{
    Joint& joint = MyAnimatedMesh->Skeleton.Joints[index];

    AnimationCurve* curve = FindCurve(index);

	if (curve)
    {
        DynamicArray<Key>& keys = curve->Keys;
        m = keys[key].Mat*m;
    }
    else
    {
        m = joint.Mat*m;
    }

    matrix[index++] = m;

    for (int iChild=0; iChild<joint.Children; iChild++)
    {
        ComputeKeyMatrix(key, m, matrix, index);
    }
}

void SkeletalAnimation::Load(FileObject* f, bool loopKeys, float scale)
{
	Looped = loopKeys;	// for later synchronization in multiplayer
    DWORD nCurves = 0;
    f->Read(&Time, sizeof(float));
    f->Read(&nCurves, sizeof(DWORD));

	BindSkeleton& skeleton = MyAnimatedMesh->Skeleton;

    AnimationCurves.ResizeArray(skeleton.Joints.Size());

    for (DWORD iCurve=0; iCurve<nCurves; iCurve++)
    {
        AnimationCurve curve;// = AnimationCurves[iCurve];
        // joint curve name
		int nameLen;
#ifdef TRANSGAMING
        char tmpBuf[256];


		f->Read(&nameLen, sizeof(int));
        Assert(nameLen < sizeof(tmpBuf) / sizeof(tmpBuf[0]));
        f->Read(tmpBuf, nameLen);
        tmpBuf[nameLen] = 0;
# ifdef DEBUG
        if (nameLen >= sizeof(tmpBuf) / sizeof(tmpBuf[0])){
            tmpBuf[sizeof(tmpBuf) / sizeof(tmpBuf[0]) - 1] = 0;
            TRACE("SkeletalAnimation::Load(): ERROR-> name buffer wasn't big enough to hold the name '%s' (len = %d)\n", tmpBuf, nameLen);
        }
# endif

        curve.Name = tmpBuf;
#else
		f->Read(&nameLen, sizeof(int));
		curve.Name.Resize(nameLen+1);
		f->Read(&curve.Name[0], nameLen);
		curve.Name[nameLen] = 0;
#endif
		if (curve.Name == "test_root")
		{
			curve.Name = "root";
		}

        // curve keys number
        DWORD nKeys = 0;
        f->Read(&nKeys, sizeof(DWORD));

		curve.Keys.ResizeArray(nKeys);

        for (DWORD iKey=0; iKey<nKeys; iKey++)
        {
            // The Key
            Key& key = curve.Keys[iKey];

            f->Read(&key.Frame, sizeof(float));
            f->Read(&key.Mat, sizeof(float) * 16);
        }

		curve.Joint = skeleton.JointIndex(curve.Name.AsChar());
		if (curve.Joint!=-1)
			AnimationCurves[curve.Joint] = curve;
    }

#define SHRINKCOUNT 3
#if defined(TRANSGAMING) && defined(SHRINKCOUNT)
	for (int i=0; i<SHRINKCOUNT; i++)
	ShrinkAnim();
#endif

	NormalizeJoints(scale);

	if (AnimationCurves.Size()>0 && AnimationCurves[0].Keys.Size()>0)
	{
		// offset time
		const AnimationCurve& fcurve = AnimationCurves[0];

		float offsetTime = fcurve.Keys[0].Frame;
		int nKeys = fcurve.Keys.Size();

		for (int iCurve=0; iCurve<AnimationCurves.Size(); iCurve++)
		{
			AnimationCurve& curve = AnimationCurves[iCurve];
			if (curve.Joint==-1)
				continue;

			for (int iKey=0; iKey<nKeys; iKey++)
			{
				curve.Keys[iKey].Frame -= offsetTime;
			}

			if (loopKeys)
			{
				Key key = curve.Keys[0];
				key.Frame = Time;
				curve.Keys.Add(key);
			}
		}

		Time = fcurve.Keys[fcurve.Keys.Size()-1].Frame;
	}
	else 
	{
		Time = 0;
	}
}

void SkeletalAnimation::NormalizeJoints(const float scale)
{
	if (AnimationCurves.Size()<1)
		return;

    AnimationCurve& curve = AnimationCurves[0];
	DynamicArray<DynamicArray<Matrix> > matrixArrays;
	matrixArrays.ResizeArray(curve.Keys.Size());

	for (int iKey=0; iKey<matrixArrays.Size(); iKey++)
	{
		matrixArrays[iKey] = ComputeKeyMatrix(iKey, scale);

		for (int iMatrix=0; iMatrix<matrixArrays[iKey].Size(); iMatrix++)
		{
			matrixArrays[iKey][iMatrix].OrthoNormalize();
		}
	}

	DynamicArray<Joint>& joints = MyAnimatedMesh->Skeleton.Joints;

	for (int iJoint=0; iJoint<joints.Size(); iJoint++)
	{
		AnimationCurve *curve = FindCurve(iJoint);

		if (!curve)
		{
			//GLog.Print("ERROR: can not find animation curve %s\n", joints[iJoint].Name.AsChar());
			continue;
		}

		for (int iKey=0; iKey<matrixArrays.Size(); iKey++)
		{
			Key &key = curve->Keys[iKey];

			int p = joints[iJoint].Parent;

			if (p==-1)
			{
				key.Mat = matrixArrays[iKey][iJoint];
			}
			else
			{
				key.Mat = matrixArrays[iKey][iJoint] * matrixArrays[iKey][p].Inverse();
			}

			key.Rot = Quaternion(key.Mat);
			key.Pos = key.Mat.GetTranslation();
		}
	}
}


AnimationCurve* SkeletalAnimation::FindCurve(int joint) const
{
	int low = 0;
	int high = AnimationCurves.Size()-1;

	if (AnimationCurves[high].Joint!=-1 && joint>AnimationCurves[high].Joint)
		return NULL;

	while (low<high)
	{
		int mid = (low+high)/2;
		int j = AnimationCurves[mid].Joint;

		for(int inc=1, i=1; inc<high-low-2; inc=-inc-i,i=-i)
		{
			if (j!=-1)	break;
			mid = mid+inc;
			j = AnimationCurves[mid].Joint;
		}

		if (joint>j)
		{
			low = mid+1;
		}
		else if (joint<j)
		{
			high = mid;
		}
		else 
		{
			return &AnimationCurves[mid];
		}
	}

	if (AnimationCurves[low].Joint==joint)
		return &AnimationCurves[low];

	return NULL;
/*
    for (int iCurve=0; iCurve<AnimationCurves.Size(); iCurve++)
    {
        if (joint==AnimationCurves[iCurve].Joint)
        {
            return &AnimationCurves[iCurve];
        }
    }
    return NULL;
*/
}

#ifdef TRANSGAMING
AnimationCurve* SkeletalAnimation::FindCurve(const char *name) const{
    for (int iCurve=0; iCurve<AnimationCurves.Size(); iCurve++)
    {
        if (!strcmp(name, AnimationCurves[iCurve].Name.AsChar()))
        {
            return &AnimationCurves[iCurve];
        }
    }
    return NULL;
}
#else
AnimationCurve* SkeletalAnimation::FindCurve(const String& name) const
{
    for (int iCurve=0; iCurve<AnimationCurves.Size(); iCurve++)
    {
        if (name==AnimationCurves[iCurve].Name)
        {
            return &AnimationCurves[iCurve];
        }
    }
    return NULL;
}
#endif

/*=====================================================================================*/

void RagdollAnimation::ComputeFrameMatrix(DynamicArray<Matrix>& frameMatrixArr, float time, const Model* model, const Matrix &world) const
{
	int index = 0;

    for (int i=0; i<MyAnimatedMesh->Skeleton.Hierarchies; i++)
    {
        ComputeFrameMatrix(world, frameMatrixArr, index, model);
    }
}

void RagdollAnimation::ComputeFrameMatrix(Matrix m, const DynamicArray<Matrix>& matrix, int& index, const Model* model) const
{
    RagdollJoint &jskel = model->Skeleton.Joints[index];
	Joint		 &jmodl = MyAnimatedMesh->Skeleton.Joints[index];

	if (jskel.Global)
    {
		m = jskel.Transform;
    }
    else
	{
        m = jskel.Transform*m;
    }

    matrix[index++] = m;

    for (int iChild=0; iChild<jmodl.Children; iChild++)
    {
        ComputeFrameMatrix(m, matrix, index, model);
    }
}

void SkeletalAnimation::ShrinkAnim(void)
{
	for (int iCurve=0; iCurve<AnimationCurves.Size(); iCurve++)
	{
		AnimationCurve& curve = AnimationCurves[iCurve];
		if (curve.Joint==-1)
		{
			if (curve.Keys.Size())
			{
				printf("too many keys\n");
				asm("break 1");
			}
			continue;
		}
		for (int iKey=1; iKey<curve.Keys.Size()-1; iKey+=2)
		{
			curve.Keys[iKey].Frame = -999.0f;
		}
	}

	for (int iCurve=0; iCurve<AnimationCurves.Size(); iCurve++)
	{
		AnimationCurve& curve = AnimationCurves[iCurve];
		if (curve.Joint==-1)
			continue;

		for (int iKey=1; iKey<curve.Keys.Size()-1; iKey++)
		{
			if (curve.Keys[iKey].Frame < 0.0f)
			{
				curve.Keys.Remove(iKey);
				iKey--;
			}
		}
		curve.Keys.Shrink(curve.Keys.Size());
	}
}

