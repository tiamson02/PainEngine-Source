#ifndef HEAR_H_ALREADY_INCLUDED
#define HEAR_H_ALREADY_INCLUDED

#include "CAIState.h"
#include "Core/Math/Vector.h"


class CHearState : public CAIState
{
private:
    float   lastTimeHEAR;
    bool    active;
    Vector  dest;
    int     state;
    bool    rot;

public:
	CHearState();
    virtual ~CHearState();

    virtual void    onInit(CAIBrain *brain, Script &obj);
    virtual void    onUpdate(CAIBrain *brain, Script &obj);
    virtual void    onRelease(CAIBrain *brain, Script &obj);
    virtual float   evaluate(CAIBrain *brain, Script &obj);
};

#endif	// HEAR_H_ALREADY_INCLUDED
