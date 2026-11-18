#include "CArea.h"
#include "Engine/PCFSystem.h"
#include "Engine/World.h"
#include "Scripting/Scripting.h"


extern PCFSystem*   GEngine;
extern Script       GScript;

//**********************************************************************************************//
//***************************** CubicSpline implementation *************************************//
//**********************************************************************************************//
CArea::CubicSpline::CubicSpline(){
    id = 0;
    area = NULL;
    spline = NULL;
    splineCount = 0;
}

CArea::CubicSpline::CubicSpline(const CArea *area){
    id = 0;
    area = NULL;
    spline = NULL;
    splineCount = 0;

    create(area);
}

CArea::CubicSpline::~CubicSpline(){
    clear();
}


void CArea::CubicSpline::clear(){
    if (spline)
        delete []spline;

    spline = NULL;
    splineCount = 0;
}

void CArea::CubicSpline::create(const CArea *_area){
    if (_area == NULL)
        return;

    area = _area;


    m_setTag("CArea::Spline");
    int     n = area->pointCount - 1;
    float * buf = (float *)malloc(sizeof(float) * (n + 1) * 5);
    float * gamma = &buf[0];
    float * delta = &buf[n + 1];
    float * DX = &buf[2 * (n + 1)];
    float * DY = &buf[3 * (n + 1)];
    float * DZ = &buf[4 * (n + 1)];
    

    gamma[0] = 0.5f;

    for (int i = 1; i < n; i++)
        gamma[i] = 1.0f / (4.0f - gamma[i - 1]);
    
    gamma[n] = 1/(2-gamma[n-1]);



    //calculate the deltas for X
    delta[0] = 3 * (area->points[1].X - area->points[0].X) * gamma[0];
    
    for (int i = 1; i < n; i++)
        delta[i] = (3 * (area->points[i + 1].X - area->points[i - 1].X) - delta[i - 1]) * gamma[i];

    delta[n] = (3 * (area->points[n].X - area->points[n - 1].X) - delta[n - 1]) * gamma[n];
    DX[n] = delta[n];

    for (int i = n - 1; i >= 0; i--)
        DX[i] = delta[i] - gamma[i] * DX[i + 1];


    //calculate the deltas for Y
    delta[0] = 3 * (area->points[1].Y - area->points[0].Y) * gamma[0];

    for (int i = 1; i < n; i++)
        delta[i] = (3 * (area->points[i + 1].Y - area->points[i - 1].Y) - delta[i - 1]) * gamma[i];

    delta[n] = (3 * (area->points[n].Y - area->points[n - 1].Y) - delta[n - 1]) * gamma[n];
    DY[n] = delta[n];

    for (int i = n - 1; i >= 0; i--)
        DY[i] = delta[i] - gamma[i] * DY[i + 1];


    //calculate the deltas for Z
    delta[0] = 3 * (area->points[1].Z - area->points[0].Z) * gamma[0];

    for (int i = 1; i < n; i++)
        delta[i] = (3 * (area->points[i + 1].Z - area->points[i - 1].Z) - delta[i - 1]) * gamma[i];

    delta[n] = (3 * (area->points[n].Z - area->points[n - 1].Z) - delta[n - 1]) * gamma[n];
    DZ[n] = delta[n];

    for (int i = n - 1; i >= 0; i--)
        DZ[i] = delta[i] - gamma[i] * DZ[i + 1];



    //compute the coefficients of the cubics
    clear();
    spline = new Curve[n];
    splineCount = n;


    for (int i = 0; i < n; i++){
        spline[i].x.a = area->points[i].X;
        spline[i].x.b = DX[i];
        spline[i].x.c = 3 * (area->points[i+1].X - area->points[i].X) - 2 * DX[i] - DX[i + 1];
        spline[i].x.d = 2 * (area->points[i].X - area->points[i + 1].X) + DX[i] + DX[i + 1];

        spline[i].y.a = area->points[i].Y;
        spline[i].y.b = DY[i];
        spline[i].y.c = 3 * (area->points[i+1].Y - area->points[i].Y) - 2 * DY[i] - DY[i + 1];
        spline[i].y.d = 2 * (area->points[i].Y - area->points[i + 1].Y) + DY[i] + DY[i + 1];

        spline[i].z.a = area->points[i].Z;
        spline[i].z.b = DZ[i];
        spline[i].z.c = 3 * (area->points[i+1].Z - area->points[i].Z) - 2 * DZ[i] - DZ[i + 1];
        spline[i].z.d = 2 * (area->points[i].Z - area->points[i + 1].Z) + DZ[i] + DZ[i + 1];
    }
    
    free(buf);
}


Vector CArea::CubicSpline::getPoint(float mu) const{
    int     i = (int)(splineCount * mu);
    float   u = 1 - ((i + 1) - (splineCount * mu));


    if (i >= 0 && i < splineCount){
        Curve & c = spline[i];
        Vector  result;


        result.X = (((c.x.d * u) + c.x.c) * u + c.x.b) * u + c.x.a;
        result.Y = (((c.y.d * u) + c.y.c) * u + c.y.b) * u + c.y.a;
        result.Z = (((c.z.d * u) + c.z.c) * u + c.z.b) * u + c.z.a;

        return result;
    }

    return Vector(0, 0, 0);
}

Vector CArea::CubicSpline::getDerivative(float mu) const{
    int i = (int)(splineCount * mu);
    float u = 1 - ((i + 1) - (splineCount * mu));
    
    
    if (i >= 0 && i < splineCount){
        Curve & c = spline[i];
        Vector  result;


        result.X = ((c.x.d * 3 * u) + c.x.c * 2) * u + c.x.b;
        result.Y = ((c.y.d * 3 * u) + c.y.c * 2) * u + c.y.b;
        result.Z = ((c.z.d * 3 * u) + c.z.c * 2) * u + c.z.b;

        return result;
    }

    return Vector(0, 0, 0);
}


//**********************************************************************************************//
//***************************** CArea implementation *******************************************//
//**********************************************************************************************//
DynamicArray<CArea::CubicSpline *>  CArea::splines;
DynamicArray<CArea *>               CArea::globalAreas;


//***************************** static member functions ****************************************//
void CArea::removeSpline(CubicSpline *spline){
    for (int i = 0; i < splines.Size(); i++){
        if (splines[i] && splines[i] == spline)
            splines[i] = NULL;
    }
}

//***************************** constructor & destructor *************************************//
CArea::CArea(){
    region =        NULL;
    id =            0;
    points =        NULL;
    pointCount =    0;
}

/*CArea::CArea(const DynamicArray<Vector> &p){
    registerScript();

    region =        NULL;
    id =            0;
    points =        NULL;
    pointCount =    0;

    setPoints(p);
}//*/


CArea::~CArea(){
    if (region)
        GEngine->pcfWorld->DeleteEntityDelayed(region);

    if (points)
        delete []points;

    //delete the splines that were owned by this area
    for (int i = 0; i < splines.Size(); i++){
        if (splines[i] && splines[i]->area == this){
            delete splines[i];

            splines[i] = NULL;
        }
    }

    //remove this area from the global ID table
    int index = globalAreas.Find(this);
    if (index >= 0)
        globalAreas[index] = NULL;

    //clear the memory used by the global ID tables if they're empty
    if (globalAreas.Size() == 0){
        globalAreas.ClearArray();

        Assert(splines.Size() == 0);
        splines.ClearArray();
    }
}

//***************************** member access functions **************************************//
void CArea::setPointCount(int count){
    if (points){

        //requested size is larger than what we currently have => reallocate
        if (count > pointCount)
            delete []points;

        //requested size is smaller than what we currently have => shrink the count and keep the allocation
        else{
            pointCount = count;

            return;
        }
    }


    points = new VectorA[count];
    pointCount = count;
}

void CArea::setPoint(int i, float x, float y, float z, float angle = 0.0f){
    points[i].X = x;
    points[i].Y = y;
    points[i].Z = z;
    points[i].A = angle;
}

/*void CArea::setPoints(const DynamicArray<Vector> &p){
    setPointCount(p.Size());
    Assert(p.Size() == pointCount);

    for (int i = 0; i < p.Size(); i++)
        points[i] = p[i];
}//*/

Vector CArea::getPoint(int i) const{
    if (i >= pointCount || i < 0)
        return Vector(0, 0, 0);

    return Vector(points[i].X, points[i].Y, points[i].Z);
}

float CArea::getAngle(int i) const{
    if (i >= pointCount || i < 0)
        return 0.0f;

    return points[i].A;
}

int CArea::getPointCount() const{
    return pointCount;
}


//***************************** script interface functions *************************************//
void CArea::registerScript(){
    static bool initialized = false;


    if (initialized)
        return;

    luaL_reg area_lib[] = {
        {"New",                     luaArea_NewArea},  
        {"Release",                 luaArea_DeleteArea},
        {"GetPoint",                luaArea_GetPoint},
        {"GetPointCount",           luaArea_GetPointCount},
        {"CalcNaturalCubicSpline",  luaArea_CalcNaturalCubicSpline},
        {"CalcNSplinePoint",        luaArea_CalcNSplinePoint},
        {"CalcNSplineDerivative",   luaArea_CalcNSplineDerivative},
        {NULL,                      NULL}
    };

    GScript.RegisterLibrary("AREA", area_lib);
    initialized = true;
}

int CArea::luaArea_NewArea(lua_State *state){
    Script                  L(state);
    Script                  table(state, 1);
    CArea *                 area;
    DynamicArray<Vector>    points;
    int                     n = table.GetCount();
    bool                    hasRegion = L.GetInt(2, false);


    //create the new area from the set of points
    area = new CArea;

    area->setPointCount(n);

    //gather all the points from the lua table so they can be passed on to
    //the code side CArea and the physics system.
	for (int i = 1; i <= n; i++){
		Script  k = table[i];
		float   x = k["X"].GetFloat();
		float   y = k["Y"].GetFloat();
		float   z = k["Z"].GetFloat();
        float   a = k["A"].GetFloat();


        area->setPoint(i - 1, x, y, z, a);

        if (hasRegion)
		    points.Add(Vector(x, y, z));
	}


    if (hasRegion){
#ifndef TRANSGAMING
        area->region = (WorldRegion *)GEngine->World->CreateEntity(Entity::ETypeRegion, "", "", 1, false);
#else
        area->region = (WorldRegion *)GEngine->pcfWorld->CreateEntity(Entity::ETypeRegion, "", "", 1, false);
#endif
        
        area->region->CreateRegionFromPoints(points, 1.0f);
    }


    //insert the new area into the table in the first empty spot (or grow
    //the array if full) and return its new ID.
    int i = globalAreas.Find(NULL);
    
    
    if (i < 0)
        i = globalAreas.GrowArray(1);

    globalAreas[i] = area;
    area->id = i + 1;

    L.PushInt(area->id);
    L.PushInt(area->region ? area->region->GIndex : 0);

    return 2;
}

int CArea::luaArea_DeleteArea(lua_State *state){
    Script  L(state);
    int     a = L.GetInt(1, 0) - 1;
    CArea * area;



    if (a < 0 || a >= globalAreas.Size()){
        TRACE("CArea::luaArea_DeleteArea(): ERROR-> invalid area ID '%d'\n", a + 1);

        return 0;
    }

    area = globalAreas[a];

    if (area){
        delete area;

        return 0;
    }

    else{
        TRACE("CArea::luaArea_DeleteArea(): ERROR-> null area '%d'!\n", a + 1);

        return 0;
    }
}

int CArea::luaArea_GetPoint(lua_State *state){
    Script  L(state);
    int     a = L.GetInt(1, 0) - 1;
    int     i = L.GetInt(2, 0) - 1;
    CArea * area;



    if (a < 0 || a >= globalAreas.Size()){
        TRACE("CArea::luaArea_GetPoint(): ERROR-> invalid area ID '%d'\n", a + 1);

        return 0;
    }

    area = globalAreas[a];

    if (area){
        if (i < 0 || i >= area->pointCount){
            TRACE("CArea::luaArea_GetPoint(): ERROR-> invalid point index '%d' for area '%d'\n", i + 1, a + 1);

            return 0;
        }

        L.PushFloat(area->points[i].X);
        L.PushFloat(area->points[i].Y);
        L.PushFloat(area->points[i].Z);
        L.PushFloat(area->points[i].A);

        return 4;
    }

    else{
        TRACE("CArea::luaArea_GetPoint(): ERROR-> null area '%d'\n", a + 1);

        return 0;
    }

    
    return 0;
}

int CArea::luaArea_GetPointCount(lua_State *state){
    Script  L(state);
    int     a = L.GetInt(1, 0) - 1;
    CArea * area;



    if (a < 0 || a >= globalAreas.Size()){
        TRACE("CArea::luaArea_GetPointCount(): ERROR-> invalid area ID '%d'\n", a + 1);

        return 0;
    }

    area = globalAreas[a];

    if (area){
        L.PushInt(area->pointCount);

        return 1;
    }

    else{
        TRACE("CArea::luaArea_GetPointCount(): ERROR-> null area '%d'\n", a + 1);

        return 0;
    }


    return 0;
}

int CArea::luaArea_CalcNaturalCubicSpline(lua_State *state){
    Script  L(state);
    int     a = L.GetInt(1, 0) - 1;
    CArea * area;



    if (a < 0 || a >= globalAreas.Size()){
        TRACE("CArea::luaArea_CalcNaturalCubicSpline(): ERROR-> invalid area ID '%d'\n", a + 1);

        return 0;
    }

    area = globalAreas[a];

    if (area){
        CubicSpline *   spline = new CubicSpline(area);
        int             index = splines.Find(NULL);
        
        
        if (index < 0)
            index = splines.GrowArray(1);


        spline->id = index + 1;

        L.PushInt(spline->id);

        return 1;
    }

    else{
        TRACE("CArea::luaArea_CalcNaturalCubicSpline(): ERROR-> null area '%d'\n", a + 1);

        return 0;
    }


    return 0;
}

int CArea::luaArea_CalcNSplinePoint(lua_State *state){
    Script          L(state);
    int             s = L.GetInt(1, 0) - 1;
    float           mu = L.GetFloat(2, 0);
    CubicSpline *   spline;



    if (s < 0 || s >= splines.Size()){
        TRACE("CArea::luaArea_CalcNSplinePoint(): ERROR-> invalid spline ID '%d'\n", s + 1);

        return 0;
    }

    spline = splines[s];

    if (spline){
        Vector result = spline->getPoint(mu);

        
        L.PushFloat(result.X);
        L.PushFloat(result.Y);
        L.PushFloat(result.Z);

        return 3;
    }

    else{
        TRACE("CArea::luaArea_CalcNSplinePoint(): ERROR-> null spline '%d'\n", s + 1);

        return 0;
    }


    return 0;
}

int CArea::luaArea_CalcNSplineDerivative(lua_State *state){
    Script          L(state);
    int             s = L.GetInt(1, 0) - 1;
    float           mu = L.GetFloat(2, 0);
    CubicSpline *   spline;



    if (s < 0 || s >= globalAreas.Size()){
        TRACE("CArea::luaArea_CalcNSplineDerivative(): ERROR-> invalid spline ID '%d'\n", s + 1);

        return 0;
    }

    spline = splines[s];

    if (spline){
        Vector result = spline->getDerivative(mu);


        L.PushFloat(result.X);
        L.PushFloat(result.Y);
        L.PushFloat(result.Z);

        return 3;
    }

    else{
        TRACE("CArea::luaArea_CalcNSplineDerivative(): ERROR-> null spline '%d'\n", s + 1);

        return 0;
    }


    return 0;
}
