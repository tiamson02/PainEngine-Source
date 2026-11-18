inline void *VertexList::getVerts(int *_stride) const{
    switch (format){
        default:
        case VTXFMT_NONE:
            *_stride = sizeof(verts[0]);
            return verts;

#ifdef TRANSGAMING_PS2
        case VTXFMT_MT:
            *_stride = sizeof(MT.positions[0]);
            return MT.positions;

        case VTXFMT_MESH:
            *_stride = sizeof(RN1.positions[0]);
            return RN1.positions;

        case VTXFMT_BONE:
            *_stride = sizeof(bone.positions[0]);
            return bone.positions;
#else
        case VTXFMT_MT:
            *_stride = sizeof(MTVerts[0]);
            return MTVerts;

        case VTXFMT_MESH:
            *_stride = sizeof(RN1Verts[0]);
            return RN1Verts;

        case VTXFMT_BONE:
            *_stride = sizeof(boneVerts[0]);
            return boneVerts;
#endif
    }

    *_stride = 0;
    return NULL;
}



inline Vector VertexList::getVertPos(int i) const{
    switch (format){
        default:
        case VTXFMT_NONE:
            return verts[i].Pos;

#ifdef TRANSGAMING_PS2
        case VTXFMT_MT:
            return MT.positions[i];

        case VTXFMT_MESH:
            return RN1.positions[i];

        case VTXFMT_BONE:
            return bone.positions[i];
#else
        case VTXFMT_MT:
            return MTVerts[i].Pos;

        case VTXFMT_MESH:
            return RN1Verts[i].Pos;

        case VTXFMT_BONE:
            return boneVerts[i].Pos;
#endif
    }
}

inline void *VertexList::getVertPos(int i, int *_stride) const{
    switch (format){
        default:
        case VTXFMT_NONE:
            *_stride = sizeof(verts[0]);
            return &verts[i].Pos;

#ifdef TRANSGAMING_PS2
        case VTXFMT_MT:
            *_stride = sizeof(MT.positions[0]);
            return &MT.positions[i];

        case VTXFMT_MESH:
            *_stride = sizeof(RN1.positions[0]);
            return &RN1.positions[i];

        case VTXFMT_BONE:
            *_stride = sizeof(bone.positions[0]);
            return &bone.positions[i];
#else
        case VTXFMT_MT:
            *_stride = sizeof(MTVerts[0]);
            return &MTVerts[i].Pos;

        case VTXFMT_MESH:
            *_stride = sizeof(RN1Verts[0]);
            return &RN1Verts[i].Pos;

        case VTXFMT_BONE:
            *_stride = sizeof(boneVerts[0]);
            return &boneVerts[i].Pos;
#endif
    }

    *_stride = 0;
    return NULL;
}


inline void VertexList::setVertPos(int i, const Vector &v){
    switch (format){
        case VTXFMT_NONE:
            verts[i].Pos = v;
            break;

#ifdef TRANSGAMING_PS2
        case VTXFMT_MT:
            MT.positions[i] = v;
            break;

        case VTXFMT_MESH:
            RN1.positions[i] = v;
            break;

        case VTXFMT_BONE:
            bone.positions[i] = v;
            break;
#else
        case VTXFMT_MT:
            MTVerts[i].Pos = v;
            break;

        case VTXFMT_MESH:
            RN1Verts[i].Pos = v;
            break;

        case VTXFMT_BONE:
            boneVerts[i].Pos = v;
            break;
#endif
    }
}


inline void VertexList::setVert(int i, const MeshVertex &vtx){
    Assert(i >= 0 && i < count);

    switch (format){
        default:
        case VTXFMT_NONE:
            verts[i] = vtx;
            break;

#ifdef TRANSGAMING_PS2
        case VTXFMT_MT:
            MT.positions[i] = vtx.Pos;
# ifdef TRANSGAMING_KEEP_NORMALS
            MT.normals[i] = vtx.Normal;
# endif
            MT.tex1[i] = vtx.TU1;
            MT.tex2[i] = vtx.TU2;
            break;

        case VTXFMT_MESH:
            RN1.positions[i] = vtx.Pos;
            RN1.normals[i] = vtx.Normal;
            RN1.tex1[i] = vtx.TU;
            break;

        case VTXFMT_BONE:
            bone.positions[i] = vtx.Pos;
            bone.normals[i] = vtx.Normal;
            bone.tex1[i] = vtx.TU;
            bone.weights[i * WEIGHT_COUNT] = 1.0f;

            for (int j = 1; j < WEIGHT_COUNT; j++)
                bone.weights[i * WEIGHT_COUNT + j] = 0.0f;

            break;
#else
        case VTXFMT_MT:
            MTVerts[i] = vtx;
            break;

        case VTXFMT_MESH:
            RN1Verts[i] = vtx;
            break;

        case VTXFMT_BONE:
            boneVerts[i] = vtx;
            break;
#endif
    }
}


#ifdef TRANSGAMING
/*inline void setVert(int i, const RenderVertexN1 &vtx){
    Assert(i >= 0 && i < count);

    switch (format){
        default:
        case VTXFMT_NONE:
            verts[i] = vtx;
            break;

        case VTXFMT_MT:
            MTVerts[i] = vtx;
            MTVerts[i].Pos = vtx.Pos;
# ifdef TRANSGAMING_KEEP_NORMALS
            MTVerts[i].Normal = vtx.Normal;
# endif
            MTVerts[i].TU1 = vtx.TU;
            MTVerts[i].TU2.X = 0;
            MTVerts[i].TU2.Y = 0;
            break;

        case VTXFMT_MESH:
            RN1Verts[i].Pos = vtx;
            break;

        case VTXFMT_BONE:
            boneVerts[i].Pos = vtx.Pos;
            boneVerts[i].Normal = vtx.Normal;
            boneVerts[i].TU = vtx.TU;
            boneVerts[i].Weights[0] = 1.0f;
            break;
    }
}

inline void setVert(int i, const BoneVertex4 &vtx){
    Assert(i >= 0 && i < count);

    switch (format){
        default:
        case VTXFMT_NONE:
            verts[i] = vtx;
            break;

        case VTXFMT_MT:
            MTVerts[i] = vtx;
            MTVerts[i].Pos = vtx.Pos;
# ifdef TRANSGAMING_KEEP_NORMALS
            MTVerts[i].Normal = vtx.Normal;
# endif
            MTVerts[i].TU1 = vtx.TU;
            MTVerts[i].TU2.X = 0;
            MTVerts[i].TU2.Y = 0;
            break;

        case VTXFMT_MESH:
            RN1Verts[i].Pos = vtx.Pos;
            RN1Verts[i].Normal = vtx.Normal;
            RN1Verts[i].TU = vtx.TU;
            break;

        case VTXFMT_BONE:
            boneVerts[i] = vtx;
            break;
    }
}

inline void setVert(int i, const MTVertex &vtx){
    Assert(i >= 0 && i < count);

    switch (format){
        default:
        case VTXFMT_NONE:
            verts[i] = vtx;
            break;

        case VTXFMT_MT:
            MTVerts[i] = vtx;
            break;

        case VTXFMT_MESH:
            RN1Verts[i].Pos = vtx.Pos;
            RN1Verts[i].Normal = vtx.Normal;
            RN1Verts[i].TU = vtx.TU;
            break;

        case VTXFMT_BONE:
            boneVerts[i].Pos = vtx.Pos;
            boneVerts[i].Normal = vtx.Normal;
            boneVerts[i].TU = vtx.TU;
            boneVerts[i].Weights[0] = 1.0f;
            break;
    }
}//*/
#endif

