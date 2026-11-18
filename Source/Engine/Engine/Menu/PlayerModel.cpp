#include "Engine.h"
#include "Render/HUD.h"
#include "Render/View.h"
#include "Engine/InputSystem.h"
#include "Render/Video.h"

#include "Engine/PainMenu.h"

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;


MenuItemPlayerModel::MenuItemPlayerModel() : MenuItem()
{
	mType = EMenuItemPlayerModel;
}


MenuItemPlayerModel::~MenuItemPlayerModel()
{
	GScript.DoString( "PainMenu:ReleasePlayerModel('%s')", mName.AsChar() );
}


void MenuItemPlayerModel::Render()
{
	GScript.DoString( "PainMenu:RenderPlayerModel('%s')", mName.AsChar() );
}
