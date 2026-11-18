// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/04 Bart Sekura
//----------------------------------------------------------------------------

#ifndef TRANSGAMING
#include "Core/Window.h"

class ENGINE_API RenderWindow : public Window
{
public:
    RenderWindow();
    ~RenderWindow();

	LRESULT HandleMessages(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

public:
    BOOL HasFocus;
};

#endif
