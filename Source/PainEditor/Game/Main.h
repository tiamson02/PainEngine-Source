#ifndef __GAME__H
#define __GAME__H

#include "EditorGame.h"

int Game_Init(HINSTANCE hInstance, const char* commandLine);
void  Game_Release();
void SwitchToFullScreen(bool enable);

EditorGame* Game();

#endif