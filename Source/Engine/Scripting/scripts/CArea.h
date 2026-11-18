#ifndef  __CAREA_H__
# define __CAREA_H__


# include "Engine/Region.h"
# include "Core/DynArray.h"
# include "Core/Math/Vector.h"


struct lua_State;

class CArea{
    class CubicSpline{
        friend class CArea;

    private:
        struct Point{
            float a, b, c, d;
        };

        struct Curve{
            Point x;
            Point y;
            Point z;
        };

        Curve *         spline;
        int             splineCount;
        int             id;
        const CArea *   area;

    public:
        CubicSpline();
        CubicSpline(const CArea *area);
        ~CubicSpline();

        void    clear();
        void    create(const CArea *area);

        Vector  getPoint(float mu) const;
        Vector  getDerivative(float mu) const;
    };

private:
    struct VectorA{
        float X, Y, Z, A;
    };

    WorldRegion *                       region;
    VectorA *                           points;
    int                                 pointCount;
    int                                 id;


    void        setPointCount(int count);
    void        setPoint(int i, float x, float y, float z, float angle);
    void        setPoints(const DynamicArray<Vector> &p);

    static DynamicArray<CubicSpline *>  splines;
    static DynamicArray<CArea *>        globalAreas;


    static void removeSpline(CubicSpline *spline);


    //*********************** script interface functions ****************************//
    static int  luaArea_NewArea(lua_State *state);
    static int  luaArea_DeleteArea(lua_State *state);
    static int  luaArea_GetPoint(lua_State *state);
    static int  luaArea_GetPointCount(lua_State *state);
    static int  luaArea_CalcNaturalCubicSpline(lua_State *state);
    static int  luaArea_CalcNSplinePoint(lua_State *state);
    static int  luaArea_CalcNSplineDerivative(lua_State *state);

public:
    CArea();
    ~CArea();

    Vector  getPoint(int i) const;
    float   getAngle(int i) const;
    int     getPointCount() const;

    static void registerScript();
};

#endif
