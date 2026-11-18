#ifndef XXX_H_ALREADY_INCLUDED
#define XXX_H_ALREADY_INCLUDED

#include "CAIState.h"

class CxxxState : public CAIState
{
public:
	CxxxState();
    virtual ~CxxxState();

    virtual void    onInit(CAIBrain *brain, Script &obj);
    virtual void    onUpdate(CAIBrain *brain, Script &obj);
    virtual void    onRelease(CAIBrain *brain, Script &obj);
    virtual float   evaluate(CAIBrain *brain, Script &obj);
};

#endif	// XXX_H_ALREADY_INCLUDED
