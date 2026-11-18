#ifndef  __VECTOR_H__
# define __VECTOR_H__

# include "stdTypes.h"


//*********************************** Vertex Component Types *****************************************//
class Vector{
public:
    float32 x, y, z;

    Vector(){
        x = y = z = 0;
    }

    Vector(const Vector &v){
        x = v.x;
        y = v.y;
        z = v.z;
    }

    Vector(float32 xx, float32 yy, float32 zz){
        x = xx;
        y = yy;
        z = zz;
    }

    Vector &operator = (const Vector &v){
        x = v.x;
        y = v.y;
        z = v.z;

        return *this;
    }

    Vector &set(float32 xx, float32 yy, float32 zz){
        x = xx;
        y = yy;
        z = zz;

        return *this;
    }

    bool operator == (const Vector &v) const{
        return x == v.x && y == v.y && z == v.z;
    }

    bool operator != (const Vector &v) const{
        return x != v.x || y != v.y || z != v.z;
    }

    Vector operator * (float32 f) const{
        return Vector(x * f, y * f, z * f);
    }
    Vector operator / (float32 f) const{
        return Vector(x / f, y / f, z / f);
    }

    Vector operator + (const Vector &v) const{
        return Vector(x + v.x, y + v.y, z + v.z);
    }
    Vector operator - (const Vector &v) const{
        return Vector(x - v.x, y - v.y, z - v.z);
    }
    Vector operator * (const Vector &v) const{
        return Vector(x * v.x, y * v.y, z * v.z);
    }
    Vector operator / (const Vector &v) const{
        return Vector(x / v.x, y / v.y, z / v.z);
    }

    Vector &operator *= (float32 f){
        x *= f;
        y *= f;
        z *= f;

        return *this;
    }
    Vector &operator /= (float32 f){
        x /= f;
        y /= f;
        z /= f;

        return *this;
    }

    Vector &operator += (const Vector &v){
        x += v.x;
        y += v.y;
        z += v.z;

        return *this;
    }
    Vector &operator -= (const Vector &v){
        x -= v.x;
        y -= v.y;
        z -= v.z;

        return *this;
    }
    Vector &operator *= (const Vector &v){
        x *= v.x;
        y *= v.y;
        z *= v.z;

        return *this;
    }
    Vector &operator /= (const Vector &v){
        x /= v.x;
        y /= v.y;
        z /= v.z;

        return *this;
    }
};


class Vector4{
public:
    float32 x, y, z, w;

    Vector4(){
        x = y = z = w = 0;
    }

    Vector4(const Vector4 &v){
        x = v.x;
        y = v.y;
        z = v.z;
        w = v.w;
    }

    Vector4(float32 xx, float32 yy, float32 zz, float32 ww){
        x = xx;
        y = yy;
        z = zz;
        w = ww;
    }

    Vector4 &operator = (const Vector4 &v){
        x = v.x;
        y = v.y;
        z = v.z;
        w = v.w;

        return *this;
    }

    Vector4 &set(float32 xx, float32 yy, float32 zz, float32 ww){
        x = xx;
        y = yy;
        z = zz;
        w = ww;

        return *this;
    }

    bool operator == (const Vector4 &v) const{
        return x == v.x && y == v.y && z == v.z && w == v.w;
    }

    bool operator != (const Vector4 &v) const{
        return x != v.x || y != v.y || z != v.z || w != v.w;
    }


    Vector4 operator * (float32 f) const{
        return Vector4(x * f, y * f, z * f, w * f);
    }
    Vector4 operator / (float32 f) const{
        return Vector4(x / f, y / f, z / f, w / f);
    }

    Vector4 operator + (const Vector4 &v) const{
        return Vector4(x + v.x, y + v.y, z + v.z, w + v.w);
    }
    Vector4 operator - (const Vector4 &v) const{
        return Vector4(x - v.x, y - v.y, z - v.z, w - v.w);
    }
    Vector4 operator * (const Vector4 &v) const{
        return Vector4(x * v.x, y * v.y, z * v.z, w * v.w);
    }
    Vector4 operator / (const Vector4 &v) const{
        return Vector4(x / v.x, y / v.y, z / v.z, w / v.w);
    }

    Vector4 &operator *= (float32 f){
        x *= f;
        y *= f;
        z *= f;
        w *= f;

        return *this;
    }
    Vector4 &operator /= (float32 f){
        x /= f;
        y /= f;
        z /= f;
        w /= f;

        return *this;
    }

    Vector4 &operator += (const Vector4 &v){
        x += v.x;
        y += v.y;
        z += v.z;
        w += v.w;

        return *this;
    }
    Vector4 &operator -= (const Vector4 &v){
        x -= v.x;
        y -= v.y;
        z -= v.z;
        w -= v.w;

        return *this;
    }
    Vector4 &operator *= (const Vector4 &v){
        x *= v.x;
        y *= v.y;
        z *= v.z;
        w *= v.w;

        return *this;
    }
    Vector4 &operator /= (const Vector4 &v){
        x /= v.x;
        y /= v.y;
        z /= v.z;
        w /= v.w;

        return *this;
    }
};


class Color{
public:
    union{
        struct{
            uint8 r;
            uint8 g;
            uint8 b;
            uint8 a;
        };

        uint32 c;
    };


    Color(){
        c = 0;
    }

    Color(const Color &cc){
        c = cc.c;
    }

    Color(uint32 cc){
        c = cc;
    }

    Color(uint8 rr, uint8 gg, uint8 bb, uint8 aa){
        r = rr;
        g = gg;
        b = bb;
        a = aa;
    }

    Color &operator = (const Color &cc){
        c = cc.c;

        return *this;
    }

    Color &operator = (const uint32 cc){
        c = cc;

        return *this;
    }

    Color &set(uint32 cc){
        c = cc;

        return *this;
    }

    Color &set(uint8 rr, uint8 gg, uint8 bb, uint8 aa){
        r = rr;
        g = gg;
        b = bb;
        a = aa;

        return *this;
    }
};


class TexCoord{
public:
    float32 u, v;

    TexCoord(){
        u = v = 0;
    }

    TexCoord(const TexCoord &t){
        u = t.u;
        v = t.v;
    }

    TexCoord(float32 _u, float32 _v){
        u = _u;
        v = _v;
    }


    TexCoord &operator = (const TexCoord &t){
        u = t.u;
        v = t.v;

        return *this;
    }

    TexCoord &set(float32 _u, float32 _v){
        u = _u;
        v = _v;

        return *this;
    }
};

class TexCoordUS{
public:
    uint16 u, v;

    TexCoordUS(){
        u = v = 0;
    }
    TexCoordUS(const TexCoordUS &t){
        u = t.u;
        v = t.v;
    }
    TexCoordUS(uint16 _u, uint16 _v){
        u = _u;
        v = _v;
    }
    TexCoordUS(float32 _u, float32 _v){
        u = (uint16)(_u * 0x8000);
        v = (uint16)(_v * 0x8000);
    }

    TexCoordUS &operator = (const TexCoordUS &t){
        u = t.u;
        v = t.v;

        return *this;
    }
    TexCoordUS &operator = (const TexCoord &t){
        u = (uint16)(t.u * 0x8000);
        v = (uint16)(t.v * 0x8000);

        return *this;
    }

    TexCoordUS &set(uint16 _u, uint16 _v){
        u = _u;
        v = _v;

        return *this;
    }
    TexCoordUS &set(float32 _u, float32 _v){
        u = (uint16)(_u * 0x8000);
        v = (uint16)(_v * 0x8000);

        return *this;
    }
};

//*********************************************** Bounding Box ***********************************//
class Box{
    static const int32 initValue = 50000;

public:
    enum{
        AXIS_X = 0x01,
        AXIS_Y = 0x02,
        AXIS_Z = 0x04,
    };

    Vector min;
    Vector max;

    Box(){
        reset();
    }

    Box(const Box &b){
        min = b.min;
        max = b.max;
    }

    Box(Vector _min, Vector _max){
        min = _min;
        max = _max;
    }

    Box &operator = (const Box &b){
        min = b.min;
        max = b.max;

        return *this;
    }

    Box &operator += (const Vector &v){
        if (v.x < min.x)
            min.x = v.x;

        if (v.x > max.x)
            max.x = v.x;

        if (v.y < min.y)
            min.y = v.y;

        if (v.y > max.y)
            max.y = v.y;

        if (v.z < min.z)
            min.z = v.z;

        if (v.z > max.z)
            max.z = v.z;

        return *this;
    }

    Box &operator += (const Box &b){
        if (!b.isValid())
            return *this;


        if (b.min.x < min.x)
            min.x = b.min.x;

        if (b.max.x > max.x)
            max.x = b.max.x;

        if (b.min.y < min.y)
            min.y = b.min.y;

        if (b.max.y > max.y)
            max.y = b.max.y;

        if (b.min.z < min.z)
            min.z = b.min.z;

        if (b.max.z > max.z)
            max.z = b.max.z;

        return *this;
    }

    Box &reset(){
        min.set((float32)initValue, (float32)initValue, (float32)initValue);
        max.set((float32)(-initValue), (float32)(-initValue), (float32)(-initValue));

        return *this;
    }

    //inside(): tests if the vector <v> is inside this box
    bool inside(const Vector &v) const{
        if (v.x < min.x || v.x > max.x)
            return false;

        if (v.y < min.y || v.y > max.y)
            return false;

        if (v.z < min.z || v.z > max.z)
            return false;

        return true;
    }

    //inside(): tests if this bounding box is inside <b>
    bool inside(const Box &b) const{
        return b.inside(min) && b.inside(b.max);
    }

    //outside(): tests if the vector <v> is outside this box
    bool outside(const Vector &v) const{
        return !inside(v);
    }

    //outside(): tests if this bounding box is outside <b>
    bool outside(const Box &b, int32 ignore = 0) const{
        int8 omin = 0;
        int8 omax = 0;


        if ((ignore & AXIS_X) == 0){
            //min x outcodes
            if (min.x < b.min.x)
                omin |= 0x01;

            if (min.x > b.max.x)
                omin |= 0x02;

            //max x outcodes
            if (max.x < b.min.x)
                omax |= 0x01;

            if (max.x > b.max.x)
                omax |= 0x02;
        }

        if ((ignore & AXIS_Y) == 0){
            //min y outcodes
            if (min.y < b.min.x)
                omin |= 0x04;

            if (min.y > b.max.x)
                omin |= 0x08;

            //max y outcodes
            if (max.y < b.min.x)
                omax |= 0x04;

            if (max.y > b.max.x)
                omax |= 0x08;
        }

        if ((ignore & AXIS_Z) == 0){
            //min z outcodes
            if (min.z < b.min.z)
                omin |= 0x10;

            if (min.z > b.max.z)
                omin |= 0x20;

            //max z outcodes
            if (max.z < b.min.z)
                omax |= 0x10;

            if (max.z > b.max.z)
                omax |= 0x20;
        }


        return (omin & omax) != 0;
    }



    bool isValid() const{
        return min.x <= max.x && min.y <= max.y && min.z <= max.z;
    }

    bool isEmpty() const{
        return min == max;
    }

    bool operator == (const Box &b) const{
        return min == b.min && max == b.max;
    }

    bool operator != (const Box &b) const{
        return min != b.min || max != b.max;
    }

    Vector getSize(){
        return Vector(max.x - min.x, max.y - min.y, max.z - min.z);
    }
};


//************************************** Interleaved Vertex Formats *************************************//
struct MeshVertex{
    Vector              pos;
    union{
        struct{
            Vector      normal;
            TexCoord    tu;
        };
        struct{
            Color       color;
            TexCoord    tu1;
            TexCoord    tu2;
        };
    };
};

struct PSPVertex{
    TexCoord    tu;
    Vector      normal;
    Vector      pos;
};

struct PS2Vertex{
    Vector      pos;
    Vector      normal;
    TexCoord    tu;
};

struct MTVertex{
    float       padding;
    TexCoord    tu1;
    TexCoordUS  tu2;
    Vector      pos;
};


//****************************************** Matrix Types ****************************************//
class Matrix{
public:
    union{
        struct{
            Vector4 x;
            Vector4 y;
            Vector4 z;
            Vector4 w;
        };
        float32 array[16];
    };

    Matrix():
        x(0, 0, 0, 0),
        y(0, 0, 0, 0),
        z(0, 0, 0, 0),
        w(0, 0, 0, 0)
    {
    }
    Matrix(const Matrix &m){
        (*this) = m;
    }

    bool operator == (const Matrix &m) const{
        return  x == m.x &&
                y == m.y &&
                z == m.z && 
                w == m.w;
    }

    bool operator != (const Matrix &m) const{
        return  x != m.x ||
                y != m.y || 
                z != m.z || 
                w != m.w;
    }

    Matrix &operator = (const Matrix &m){
        x = m.x;
        y = m.y;
        z = m.z;
        w = m.w;

        return *this;
    }

    Matrix &setIdentity(){
        x.set(1, 0, 0, 0);
        y.set(0, 1, 0, 0);
        z.set(0, 0, 1, 0);
        w.set(0, 0, 0, 1);

        return *this;
    }

    bool isIdentity() const;
};

class Identity: public Matrix{
public:
    Identity(){  
        x.set(1, 0, 0, 0);
        y.set(0, 1, 0, 0);
        z.set(0, 0, 1, 0);
        w.set(0, 0, 0, 1);
    }
};


inline bool Matrix::isIdentity() const{
    static const Identity identity;

    return (*this) == identity;
}

#endif
