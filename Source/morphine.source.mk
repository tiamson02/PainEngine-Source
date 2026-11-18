ifeq "$(OS)" "PS2DEVKIT"
TARGETWORD=_ps2
else
ifeq "$(OS)" "PSPDEVKIT"
TARGETWORD=_psp
else
TARGETWORD=_linux
endif
endif

ifeq "$(BUILDGOLD)" "true"
GOLDWORD=gold
endif

ifeq "$(BUILDDEMO)" "true"
EXE_NAME=painkiller$(GOLDWORD)demo
else
EXE_NAME=painkiller$(GOLDWORD)$(TARGETWORD)
endif

EXE_DEDICATED_NAME=$(EXE_NAME)d

all: $(DIR.OBJ)/$(EXE_NAME)$(EXT.EXE)
#includes by subgroup

INCLUDES.LOADER=\
	-ILoader

INCLUDES.PAINKILLER=\
	-IPainKiller/Include -IBase -IEngine

#INCLUDES.BLACKEDITIONLOADER=\
#	-IBlackEditionLoader

INCLUDES.D3DEV=\
	-ID3Dev\
	-IEngine

INCLUDES.ENGINE=\
	-IEngine\
	-IEngine/Core\
	-IEngine/Scripting/LUA

INCLUDES.ENGINE.AUDIO=\
	-IEngine\
	-IEngine/Audio\
	-IExternal/miles/include

			
INCLUDES.ENGINE.CORE=\
	-IEngine/Core\

INCLUDES.ENGINE.CORE.FILESYSTEM=\
	-IEngine/Core/FileSystem

INCLUDES.ENGINE.CORE.MATH=\
	-IEngine/Core/Math

INCLUDES.ENGINE.CORE.ZLIB=\
	-IEngine/Core/zlib

INCLUDES.ENGINE.ENGINE=\
	-IEngine/Engine\
        -IEngine/Scripting/LUA\
	-IExternal/miles/include\
	-IExternal/Bink
	#-IExternal/freetype2/include\

INCLUDES.ENGINE.ENGINE.MENU=\
	-IEngine/Engine\
	-IExternal/Bink
	#-IExternal/freetype2/include\

INCLUDES.ENGINE.FILESYSTEM=\
	-IEngine/FileSystem

INCLUDES.ENGINE.GAME=\
	-IEngine/Game

INCLUDES.ENGINE.NET=\
        -IEngine/Net\
	-IExternal/miles/include\
	-IEngine/Ase
	#-IExternal/freetype2/include\

INCLUDES.ENGINE.NET.PKTV=\
        -IEngine/Net

INCLUDES.ENGINE.PATHFINDING=\
	-IPathfinding
	
INCLUDES.ENGINE.PHYSICS=\
	-IPhysics

INCLUDES.ENGINE.PHYSICS.HAVOK=\
	-IPhysics/Havok

INCLUDES.ENGINE.PHYSICS.HAVOK.TNGPHYSFSSTREAM=\
	-IPhysics/Havok/TNGPhysFSStream

INCLUDES.ENGINE.RENDER=\
	-IRender\
	-IExternal/miles/include\
	-IExternal/Bink
	#-IExternal/iconv\
	#-IExternal/freetype2/include\

INCLUDES.ENGINE.ENTITIES=\
	-IEngine/Engine\
	-IRender\
	-IExternal/miles/include\
	-IExternal/Bink
	#-IExternal/freetype2/include\
	
INCLUDES.ENGINE.SCRIPTING=\
	-IScripting\
	-IScripting/scripts\
	-IExternal/miles/include
	#-IExternal/freetype2/include

INCLUDES.ENGINE.SCRIPTING.LUA=\
	-ILUA

#sources by subgroup

SOURCE.LOADER=\
#	Loader/Loader.cpp

SOURCE.PAINKILLER=\
	PainKiller/Source/Main.cpp\
	PainKiller/Source/PlayerController.cpp
#	PainKiller/Source/SimpleGame.cpp\
#	PainKiller/Source/SimpleGame.cpp\
#	PainKiller/Source/TestGame.cpp\
#	PainKiller/Source/PlayerController.cpp


#SOURCE.BLACKEDITIONLOADER=\
#	BlackEditionLoader/BlackEditionLoader.cpp\
#	BlackEditionLoader/stdafx.cpp


SOURCE.D3DEV=\
	D3Dev/D3DEffect.cpp\
	D3Dev/D3Dev.cpp\
	D3Dev/D3DTexture.cpp


SOURCE.ENGINE=\
	Engine/Engine.cpp


ifeq "$(OS)" "PS2DEVKIT"
SOURCE.ENGINE.AUDIO=\
	Engine/Audio/AudioRecording.cpp\
	Engine/Audio/Miles2DSound.cpp\
	Engine/Audio/Miles3DSound.cpp\
	Engine/Audio/MilesEngine.cpp
else
ifeq "$(OS)" "PSPDEVKIT"
SOURCE.ENGINE.AUDIO=
endif
endif

SOURCE.ENGINE.CORE=\
	Engine/Core/CRC32.cpp\
	Engine/Core/Debug.cpp\
	Engine/Core/LeString.cpp\
	Engine/Core/MemoryManager.cpp\
	Engine/Core/mmgr.cpp\
	Engine/Core/Object.cpp\
	Engine/Core/RegistryManager.cpp\
	Engine/Core/SimpleProfiler.cpp\
	Engine/Core/Window.cpp


ifeq "$(OS)" "PS2DEVKIT"
SOURCE.ENGINE.CORE += Engine/Core/SystemDriver_PS2.cpp
else
ifeq "$(OS)" "PSPDEVKIT"
SOURCE.ENGINE.CORE += Engine/Core/SystemDriver_PSP.cpp
else
SOURCE.ENGINE.CORE += Engine/Core/SystemDriver.cpp
endif
endif


SOURCE.ENGINE.CORE.FILESYSTEM=\
	Engine/Core/FileSystem/ConfigFile.cpp\
	Engine/Core/FileSystem/LogBuffer.cpp
#XX	Engine/Core/FileSystem/FileManagerStandard.cpp


SOURCE.ENGINE.CORE.MATH=\
	Engine/Core/Math/ConvexPolygon.cpp\
	Engine/Core/Math/GRandom.cpp\
	Engine/Core/Math/LeMath.cpp


SOURCE.ENGINE.ENGINE=\
	Engine/Engine/AnimatedMeshManager.cpp\
	Engine/Engine/AnimationController.cpp\
	Engine/Engine/ConvexPolygon.cpp\
	Engine/Engine/ModelAnimation.cpp\
	Engine/Engine/MPStats.cpp\
	Engine/Engine/MapSelect.cpp\
	Engine/Engine/MagicBoard.cpp\
	Engine/Engine/InputSystem.cpp\
	Engine/Engine/PainMenu.cpp\
	Engine/Engine/Console.cpp\
	Engine/Engine/SimpleMesh.cpp\
	Engine/Engine/Portal.cpp\
	Engine/Engine/AnimatedMesh.cpp\
	Engine/Engine/GlassSystem.cpp\
	Engine/Engine/BaseGame.cpp\
	Engine/Engine/World.cpp\
	Engine/Engine/PCFSystem.cpp\
	Engine/Engine/tri_stripper/tri_stripper.cpp\

ifeq "$(OS)" "PS2DEVKIT"
SOURCE.ENGINE.ENGINE += Engine/Engine/PSPInputSystem.cpp
#SOURCE.ENGINE.ENGINE += Engine/Engine/PS2InputSystem.cpp
else
ifeq "$(OS)" "PSPDEVKIT"
SOURCE.ENGINE.ENGINE += Engine/Engine/PSPInputSystem.cpp
else
SOURCE.ENGINE.ENGINE += Engine/Core/DIInputSystem.cpp
endif
endif

# #xx means the file is no use	
#XX	Engine/Engine/AudioDevice.cpp\
#XX	Engine/Engine/EngineApp.cpp\
#XX	Engine/Engine/Entity.cpp\
#XX	Engine/Engine/Environment.cpp\
#XX	Engine/Engine/hkConvexPlaneEquationsUtil.cpp\
#XX	Engine/Engine/Light.cpp\
#XX	Engine/Engine/MenuSystem.cpp\
#XX	Engine/Engine/Mesh.cpp\
#XX	Engine/Engine/Model.cpp\
#XX	Engine/Engine/NetClient.cpp\
#XX	Engine/Engine/Region.cpp\
#XX	Engine/Engine/Sound.cpp\
#XX	Engine/Engine/Win32InputSystem.cpp\

SOURCE.ENGINE.ENGINE.MENU=\
	Engine/Engine/Menu/Border.cpp\
	Engine/Engine/Menu/Checkbox.cpp\
	Engine/Engine/Menu/ColorPicker.cpp\
	Engine/Engine/Menu/ImageButton.cpp\
	Engine/Engine/Menu/KeyConf.cpp\
	Engine/Engine/Menu/List.cpp\
	Engine/Engine/Menu/LoadingScreen.cpp\
	Engine/Engine/Menu/LoadSave.cpp\
	Engine/Engine/Menu/MapTable.cpp\
	Engine/Engine/Menu/MenuItem.cpp\
	Engine/Engine/Menu/MenuScreen.cpp\
	Engine/Engine/Menu/MenuToLua.cpp\
	Engine/Engine/Menu/PlayerModel.cpp\
	Engine/Engine/Menu/Scroller.cpp\
	Engine/Engine/Menu/ServerList.cpp\
	Engine/Engine/Menu/Slider.cpp\
	Engine/Engine/Menu/TextButtonEx.cpp\
	Engine/Engine/Menu/TextEdit.cpp


SOURCE.ENGINE.FILESYSTEM=\
	Engine/FileSystem/GFile.cpp\
	Engine/FileSystem/GFileManager.cpp\
	Engine/FileSystem/GMemFile.cpp\
	Engine/FileSystem/GPack.cpp\
	Engine/FileSystem/GZipPack.cpp\
	Engine/FileSystem/WildCard.cpp
	

SOURCE.ENGINE.FILESYSTEM_C=\
	Engine/FileSystem/zip.c\
	Engine/FileSystem/unzip.c\
	Engine/FileSystem/iowin32.c\
	Engine/FileSystem/ioapi.c



ifeq "$(OS)" "PS2DEVKIT"
SOURCE.ENGINE.GAME += Engine/Game/PainMain_PS2.cpp
SOURCE.ENGINE.GAME += Engine/Game/PainGame_PS2.cpp
else
ifeq "$(OS)" "PSPDEVKIT"
SOURCE.ENGINE.GAME += Engine/Game/PainMain_PSP.cpp
SOURCE.ENGINE.GAME += Engine/Game/PainGame_PSP.cpp
endif
endif
SOURCE.ENGINE.GAME += Engine/Game/memoryDump.cpp


SOURCE.ENGINE.NET=\
	Engine/Net/DemoRecording2.cpp\
	Engine/Net/NetworkDevice2.cpp\
	Engine/Net/NetworkPeer2.cpp\
	Engine/Net/ObjectSynchronizer2.cpp\
	Engine/Net/StringHashSystem.cpp\
	Engine/Net/UdpMessage.cpp\
# #xx means the file is no use	
#xx	Engine/Net/DemoRecording.cpp\
#xx	Engine/Net/FileTransfer.cpp\
#xx	Engine/Net/IPChannel.cpp\
#xx	Engine/Net/IpDriver.cpp\
#xx	Engine/Net/NetworkClient.cpp\
#xx	Engine/Net/UdpMessageProfiler.cpp
#xx	Engine/Net/NetworkDevice.cpp\
#xx	Engine/Net/NetworkProfiler.cpp\
#xx	Engine/Net/ObjectSynchronizer.cpp\

SOURCE.ENGINE.NET.PKTV=\
	Engine/Net/PKTVEntityProxies.cpp\
	Engine/Net/PKTVManager.cpp\
	Engine/Net/PKTVProxyConnection.cpp\
	Engine/Net/PKTVStringDict.cpp\
	Engine/Net/PKTVTransport.cpp\
	Engine/Net/PKTVTransportFile.cpp\
	Engine/Net/PKTVTransportTCP.cpp\
	Engine/Net/PKTVTransportUDP.cpp\
	Engine/Net/PKTVUnpackedFrame.cpp\
	Engine/Net/SocketClosingModule.cpp\

SOURCE.ENGINE.PATHFINDING=\
	Engine/Pathfinding/Pathfinder2.cpp\
	Engine/Pathfinding/Pathfinder2Floors.cpp\
	Engine/Pathfinding/Pathfinder2Waypoints.cpp\
	Engine/Pathfinding/Pathfinder.cpp\
	Engine/Pathfinding/PathfinderFloor.cpp\
	Engine/Pathfinding/PathfinderFloorEdge.cpp\
	Engine/Pathfinding/PathfinderFloorProducer.cpp\
	Engine/Pathfinding/PathfinderFloorSet.cpp\
	Engine/Pathfinding/PathfinderPortalNode.cpp\
	Engine/Pathfinding/WaypointGPath.cpp\
	Engine/Pathfinding/WaypointPath.cpp\
	Engine/Pathfinding/WaypointSet.cpp


SOURCE.ENGINE.PHYSICS=\
	Engine/Physics/PhysicsObject.cpp\
	Engine/Physics/PhysicsEngine.cpp\
	Engine/Physics/PhysicsWorld.cpp\
	Engine/Physics/Ragdoll.cpp\
#XX	Engine/Physics/PlayerMove.inl
#XX	Engine/Physics/Physics.cpp\

SOURCE.ENGINE.PHYSICS.HAVOK=\
	Engine/Physics/Havok/HavokMissileController.cpp\
	Engine/Physics/Havok/HavokCharacterController.cpp\
	Engine/Physics/Havok/HavokPhysics.cpp\
	Engine/Physics/Havok/HavokRagdoll.cpp\
	Engine/Physics/Havok/HavokWorld.cpp\
	Engine/Physics/Havok/POHavokPart.cpp


SOURCE.ENGINE.PHYSICS.HAVOK.TNGPHYSFSSTREAM=\
	Engine/Physics/Havok/TNGPhysFSStream/TNGPhysFSStreamReader.cpp\
	Engine/Physics/Havok/TNGPhysFSStream/TNGPhysFSStreamWriter.cpp\


SOURCE.ENGINE.RENDER=\
	Engine/Render/HUD.cpp\
	Engine/Render/DecalSystem.cpp\
	Engine/Render/Movie.cpp\
	Engine/Render/PostProcess.cpp\
	Engine/Render/RenderDevice.cpp\
	Engine/Render/RenderWindow.cpp\
	Engine/Render/SceneRender.cpp\
	Engine/Render/Sky.cpp\
	Engine/Render/TrailSystem.cpp\
	Engine/Render/View.cpp\
	Engine/Render/Viewport.cpp\
	Engine/Render/LineRender.cpp\
	Engine/Render/Material.cpp\
	Engine/Render/ParticleSystem.cpp\
	Engine/Render/VertexList.cpp
	#Engine/Render/ParticleRender.cpp

ifeq "$(OS)" "PS2DEVKIT"
SOURCE.ENGINE.RENDER += Engine/Render/Video_PS2.cpp
SOURCE.ENGINE.RENDER += Engine/Render/VideoSystem_PS2.cpp
else
ifeq "$(OS)" "PSPDEVKIT"
SOURCE.ENGINE.RENDER += Engine/Render/Video_PSP.cpp
SOURCE.ENGINE.RENDER += Engine/Render/VideoSystem_PSP.cpp
else
SOURCE.ENGINE.RENDER += Engine/Render/Video.cpp
endif
endif


SOURCE.ENGINE.ENTITIES=\
	Engine/Render/Billboard.cpp\
	Engine/Render/Decal.cpp\
	Engine/Engine/Entity.cpp\
	Engine/Engine/Environment.cpp\
	Engine/Render/FogVolume.cpp\
	Engine/Render/Light.cpp\
	Engine/Engine/Mesh.cpp\
	Engine/Engine/Model.cpp\
	Engine/Render/ParticleEffect.cpp\
	Engine/Engine/Region.cpp\
	Engine/Engine/Sound.cpp\
	Engine/Render/Trail.cpp\


SOURCE.ENGINE.SCRIPTING=\
	Engine/Scripting/EngineToLua.cpp\
	Engine/Scripting/Scripting.cpp

SOURCE.ENGINE.SCRIPTING_C=\
	Engine/Scripting/LMalloc.c

SOURCE.ENGINE.SCRIPTING.LUA=\
	Engine/Scripting/LUA/lapi.c\
	Engine/Scripting/LUA/lauxlib.c\
	Engine/Scripting/LUA/lbaselib.c\
	Engine/Scripting/LUA/lcode.c\
	Engine/Scripting/LUA/ldblib.c\
	Engine/Scripting/LUA/ldebug.c\
	Engine/Scripting/LUA/ldo.c\
	Engine/Scripting/LUA/ldump.c\
	Engine/Scripting/LUA/lfunc.c\
	Engine/Scripting/LUA/lgc.c\
	Engine/Scripting/LUA/linit.c\
	Engine/Scripting/LUA/liolib.c\
	Engine/Scripting/LUA/llex.c\
	Engine/Scripting/LUA/lmathlib.c\
	Engine/Scripting/LUA/lmem.c\
	Engine/Scripting/LUA/loadlib.c\
	Engine/Scripting/LUA/lobject.c\
	Engine/Scripting/LUA/lopcodes.c\
	Engine/Scripting/LUA/loslib.c\
	Engine/Scripting/LUA/lparser.c\
	Engine/Scripting/LUA/lstate.c\
	Engine/Scripting/LUA/lstring.c\
	Engine/Scripting/LUA/lstrlib.c\
	Engine/Scripting/LUA/ltable.c\
	Engine/Scripting/LUA/ltablib.c\
	Engine/Scripting/LUA/ltm.c\
	Engine/Scripting/LUA/lundump.c\
	Engine/Scripting/LUA/lvm.c\
	Engine/Scripting/LUA/lzio.c
#	Engine/Scripting/LUA/ltests.c\

	
#compiler flags by subgroup

CFLAGS.LOADER = $(CFLAGS) $(INCLUDES.LOADER)
CFLAGS.PAINKILLER = $(CFLAGS) $(INCLUDES.PAINKILLER)
#CFLAGS.BLACKEDITIONLOADER = $(CFLAGS) $(INCLUDES.BLACKEDITIONLOADER)
CFLAGS.D3DEV = $(CFLAGS) $(INCLUDES.D3DEV)
CFLAGS.ENGINE = $(CFLAGS) $(INCLUDES.ENGINE) -DENGINE_API=
CFLAGS.ENGINE.AUDIO = $(CFLAGS) $(INCLUDES.ENGINE.AUDIO)
CFLAGS.ENGINE.CORE = $(CFLAGS) $(INCLUDES.ENGINE) $(INCLUDES.ENGINE.CORE) $(INCLUDES.ENGINE.CORE.ZLIB)
CFLAGS.ENGINE.CORE.FILESYSTEM = $(CFLAGS) $(INCLUDES.ENGINE) $(INCLUDES.ENGINE.CORE) $(INCLUDES.ENGINE.CORE.FILESYSTEM)
CFLAGS.ENGINE.CORE.MATH = $(CFLAGS) $(INCLUDES.ENGINE) $(INCLUDES.ENGINE.CORE.MATH)
CFLAGS.ENGINE.ENGINE = $(CFLAGS) $(INCLUDES.ENGINE) $(INCLUDES.ENGINE.ENGINE)
CFLAGS.ENGINE.ENGINE.MENU = $(CFLAGS) $(INCLUDES.ENGINE) $(INCLUDES.ENGINE.ENGINE.MENU)
CFLAGS.ENGINE.FILESYSTEM = $(CFLAGS) $(INCLUDES.ENGINE) $(INCLUDES.ENGINE.FILESYSTEM)
CFLAGS.ENGINE.GAME = $(CFLAGS) $(INCLUDES.ENGINE) $(INCLUDES.ENGINE.GAME)
CFLAGS.ENGINE.NET = $(CFLAGS) $(INCLUDES.ENGINE) $(INCLUDES.ENGINE.NET)
CFLAGS.ENGINE.NET.PKTV = $(CFLAGS) $(INCLUDES.ENGINE) $(INCLUDES.ENGINE.NET.PKTV)
CFLAGS.ENGINE.PATHFINDING = $(CFLAGS) $(INCLUDES.ENGINE) $(INCLUDES.ENGINE.PATHFINDING)
CFLAGS.ENGINE.RENDER = $(CFLAGS) $(INCLUDES.ENGINE) $(INCLUDES.ENGINE.RENDER)
CFLAGS.ENGINE.ENTITIES = $(CFLAGS) $(INCLUDES.ENGINE) $(INCLUDES.ENGINE.ENTITIES)
CFLAGS.ENGINE.SCRIPTING = $(CFLAGS) $(INCLUDES.ENGINE) $(INCLUDES.ENGINE.SCRIPTING)
CFLAGS.ENGINE.SCRIPTING.LUA = $(CFLAGS) $(INCLUDES.ENGINE) $(INCLUDES.ENGINE.SCRIPTING.LUA)


ifeq "$(OS)" "PS2DEVKIT"
CFLAGS.ENGINE.PHYSICS = $(CFLAGS) -fno-rtti -mvu0-use-vf0-vf15 $(INCLUDES.ENGINE) $(INCLUDES.ENGINE.PHYSICS)
CFLAGS.ENGINE.PHYSICS.HAVOK = $(CFLAGS) -fno-rtti -mvu0-use-vf0-vf15 $(INCLUDES.ENGINE) $(INCLUDES.ENGINE.PHYSICS.HAVOK)
CFLAGS.ENGINE.PHYSICS.HAVOK.TNGPHYSFSSTREAM = $(CFLAGS) -fno-rtti -mvu0-use-vf0-vf15 $(INCLUDES.ENGINE) $(INCLUDES.ENGINE.PHYSICS.HAVOK.TNGPHYSFSSTREAM)
else
ifeq "$(OS)" "PSPDEVKIT"
CFLAGS.ENGINE.PHYSICS = $(CFLAGS) -fno-rtti $(INCLUDES.ENGINE) $(INCLUDES.ENGINE.PHYSICS)
CFLAGS.ENGINE.PHYSICS.HAVOK = $(CFLAGS) -fno-rtti $(INCLUDES.ENGINE) $(INCLUDES.ENGINE.PHYSICS.HAVOK)
CFLAGS.ENGINE.PHYSICS.HAVOK.TNGPHYSFSSTREAM = $(CFLAGS) -fno-rtti $(INCLUDES.ENGINE) $(INCLUDES.ENGINE.PHYSICS.HAVOK.TNGPHYSFSSTREAM)
else
CFLAGS.ENGINE.PHYSICS = $(CFLAGS) $(INCLUDES.ENGINE) $(INCLUDES.ENGINE.PHYSICS)
CFLAGS.ENGINE.PHYSICS.HAVOK = $(CFLAGS) $(INCLUDES.ENGINE) $(INCLUDES.ENGINE.PHYSICS.HAVOK)
CFLAGS.ENGINE.PHYSICS.HAVOK.TNGPHYSFSSTREAM = $(CFLAGS) $(INCLUDES.ENGINE) $(INCLUDES.ENGINE.PHYSICS.HAVOK.TNGPHYSFSSTREAM)
endif
endif


#object version of the sources
SOURCE.LOADER.OBJ:=$(addprefix $(DIR.OBJ)/, $(addsuffix $(O), $(basename $(SOURCE.LOADER))) )
SOURCE.PAINKILLER.OBJ:=$(addprefix $(DIR.OBJ)/, $(addsuffix $(O), $(basename $(SOURCE.PAINKILLER))) )
#SOURCE.BLACKEDITIONLOADER.OBJ:=$(addprefix $(DIR.OBJ)/, $(addsuffix $(O), $(basename $(SOURCE.BLACKEDITIONLOADER))) )
SOURCE.D3DEV.OBJ:=$(addprefix $(DIR.OBJ)/, $(addsuffix $(O), $(basename $(SOURCE.D3DEV))) )
SOURCE.ENGINE.OBJ:=$(addprefix $(DIR.OBJ)/, $(addsuffix $(O), $(basename $(SOURCE.ENGINE))) )
SOURCE.ENGINE.AUDIO.OBJ:=$(addprefix $(DIR.OBJ)/, $(addsuffix $(O), $(basename $(SOURCE.ENGINE.AUDIO))) )
SOURCE.ENGINE.CORE.OBJ:=$(addprefix $(DIR.OBJ)/, $(addsuffix $(O), $(basename $(SOURCE.ENGINE.CORE))) )
SOURCE.ENGINE.CORE.FILESYSTEM.OBJ:=$(addprefix $(DIR.OBJ)/, $(addsuffix $(O), $(basename $(SOURCE.ENGINE.CORE.FILESYSTEM))) )
SOURCE.ENGINE.CORE.MATH.OBJ:=$(addprefix $(DIR.OBJ)/, $(addsuffix $(O), $(basename $(SOURCE.ENGINE.CORE.MATH))) )
SOURCE.ENGINE.ENGINE.OBJ:=$(addprefix $(DIR.OBJ)/, $(addsuffix $(O), $(basename $(SOURCE.ENGINE.ENGINE))) )
SOURCE.ENGINE.ENGINE.MENU.OBJ:=$(addprefix $(DIR.OBJ)/, $(addsuffix $(O), $(basename $(SOURCE.ENGINE.ENGINE.MENU))) )
SOURCE.ENGINE.FILESYSTEM.OBJ:=$(addprefix $(DIR.OBJ)/, $(addsuffix $(O), $(basename $(SOURCE.ENGINE.FILESYSTEM))) )
SOURCE.ENGINE.FILESYSTEM_C.OBJ:=$(addprefix $(DIR.OBJ)/, $(addsuffix $(O), $(basename $(SOURCE.ENGINE.FILESYSTEM_C))) )
SOURCE.ENGINE.GAME.OBJ:=$(addprefix $(DIR.OBJ)/, $(addsuffix $(O), $(basename $(SOURCE.ENGINE.GAME))) )
SOURCE.ENGINE.NET.OBJ:=$(addprefix $(DIR.OBJ)/, $(addsuffix $(O), $(basename $(SOURCE.ENGINE.NET))) )
SOURCE.ENGINE.NET.PKTV.OBJ:=$(addprefix $(DIR.OBJ)/, $(addsuffix $(O), $(basename $(SOURCE.ENGINE.NET.PKTV))) )
SOURCE.ENGINE.PATHFINDING.OBJ:=$(addprefix $(DIR.OBJ)/, $(addsuffix $(O), $(basename $(SOURCE.ENGINE.PATHFINDING))) )
SOURCE.ENGINE.PHYSICS.OBJ:=$(addprefix $(DIR.OBJ)/, $(addsuffix $(O), $(basename $(SOURCE.ENGINE.PHYSICS))) )
SOURCE.ENGINE.PHYSICS.HAVOK.OBJ:=$(addprefix $(DIR.OBJ)/, $(addsuffix $(O), $(basename $(SOURCE.ENGINE.PHYSICS.HAVOK))) )
SOURCE.ENGINE.PHYSICS.HAVOK.TNGPHYSFSSTREAM.OBJ:=$(addprefix $(DIR.OBJ)/, $(addsuffix $(O), $(basename $(SOURCE.ENGINE.PHYSICS.HAVOK.TNGPHYSFSSTREAM))) )
SOURCE.ENGINE.RENDER.OBJ:=$(addprefix $(DIR.OBJ)/, $(addsuffix $(O), $(basename $(SOURCE.ENGINE.RENDER))) )
SOURCE.ENGINE.ENTITIES.OBJ:=$(addprefix $(DIR.OBJ)/, $(addsuffix $(O), $(basename $(SOURCE.ENGINE.ENTITIES))) )
SOURCE.ENGINE.SCRIPTING.OBJ:=$(addprefix $(DIR.OBJ)/, $(addsuffix $(O), $(basename $(SOURCE.ENGINE.SCRIPTING))) )
SOURCE.ENGINE.SCRIPTING_C.OBJ:=$(addprefix $(DIR.OBJ)/, $(addsuffix $(O), $(basename $(SOURCE.ENGINE.SCRIPTING_C))) )
SOURCE.ENGINE.SCRIPTING.LUA.OBJ:=$(addprefix $(DIR.OBJ)/, $(addsuffix $(O), $(basename $(SOURCE.ENGINE.SCRIPTING.LUA))) )

#compilation rules by subgroup
${SOURCE.LOADER.OBJ}: $(DIR.OBJ)/%$(O) : %.cpp
	$(COMPILER.cc) $(CFLAGS.LOADER) -I. $(PATH.H.SYS) -o $@ -c $< 

${SOURCE.PAINKILLER.OBJ}: $(DIR.OBJ)/%$(O) : %.cpp
	$(COMPILER.cc) ${CFLAGS.PAINKILLER} -I. $(PATH.H.SYS) -o $@ -c $< 
	    
#${SOURCE.BLACKEDITIONLOADER.OBJ}: $(DIR.OBJ)/%$(O) : %.cpp
#	$(COMPILER.cc) ${CFLAGS.BLACKEDITIONLOADER} -I. $(PATH.H.SYS) -o $@ -c $< 

${SOURCE.D3DEV.OBJ}: $(DIR.OBJ)/%$(O) : %.cpp
	$(COMPILER.cc) ${CFLAGS.D3DEV} -I. $(PATH.H.SYS) -o $@ -c $< 

${SOURCE.ENGINE.OBJ}: $(DIR.OBJ)/%$(O) : %.cpp
	$(COMPILER.cc) ${CFLAGS.ENGINE} -I. $(PATH.H.SYS) -o $@ -c $< 

${SOURCE.ENGINE.AUDIO.OBJ}: $(DIR.OBJ)/%$(O) : %.cpp
	$(COMPILER.cc) ${CFLAGS.ENGINE.AUDIO} -I. $(PATH.H.SYS) -o $@ -c $< 

${SOURCE.ENGINE.CORE.OBJ}: $(DIR.OBJ)/%$(O) : %.cpp
	$(COMPILER.cc) ${CFLAGS.ENGINE.CORE} -I. $(PATH.H.SYS) -o $@ -c $< 

${SOURCE.ENGINE.CORE.FILESYSTEM.OBJ}: $(DIR.OBJ)/%$(O) : %.cpp
	$(COMPILER.cc) ${CFLAGS.ENGINE.CORE.FILESYSTEM} -I. $(PATH.H.SYS) -o $@ -c $< 

${SOURCE.ENGINE.CORE.MATH.OBJ}: $(DIR.OBJ)/%$(O) : %.cpp
	$(COMPILER.cc) ${CFLAGS.ENGINE.CORE.MATH} -I. $(PATH.H.SYS) -o $@ -c $< 

${SOURCE.ENGINE.ENGINE.OBJ}: $(DIR.OBJ)/%$(O) : %.cpp
	$(COMPILER.cc) ${CFLAGS.ENGINE.ENGINE} -I. $(PATH.H.SYS) -o $@ -c $< 

${SOURCE.ENGINE.ENGINE.MENU.OBJ}: $(DIR.OBJ)/%$(O) : %.cpp
	$(COMPILER.cc) ${CFLAGS.ENGINE.ENGINE.MENU} -I. $(PATH.H.SYS) -o $@ -c $< 

${SOURCE.ENGINE.FILESYSTEM.OBJ}: $(DIR.OBJ)/%$(O) : %.cpp
	$(COMPILER.cc) ${CFLAGS.ENGINE.FILESYSTEM} -I. $(PATH.H.SYS) -o $@ -c $< 

${SOURCE.ENGINE.FILESYSTEM_C.OBJ}: $(DIR.OBJ)/%$(O) : %.c
	$(COMPILER.cc) ${CFLAGS.ENGINE.FILESYSTEM} -I. $(PATH.H.SYS) -o $@ -c $< 

${SOURCE.ENGINE.GAME.OBJ}: $(DIR.OBJ)/%$(O) : %.cpp
	$(COMPILER.cc) ${CFLAGS.ENGINE.GAME} -I. $(PATH.H.SYS) -o $@ -c $< 

${SOURCE.ENGINE.NET.OBJ}: $(DIR.OBJ)/%$(O) : %.cpp
	$(COMPILER.cc) ${CFLAGS.ENGINE.NET} -I. $(PATH.H.SYS) -o $@ -c $< 

${SOURCE.ENGINE.NET.PKTV.OBJ}: $(DIR.OBJ)/%$(O) : %.cpp
	$(COMPILER.cc) ${CFLAGS.ENGINE.NET.PKTV} -I. $(PATH.H.SYS) -o $@ -c $< 

${SOURCE.ENGINE.PATHFINDING.OBJ}: $(DIR.OBJ)/%$(O) : %.cpp
	$(COMPILER.cc) ${CFLAGS.ENGINE.PATHFINDING} -I. $(PATH.H.SYS) -o $@ -c $< 

${SOURCE.ENGINE.PHYSICS.OBJ}: $(DIR.OBJ)/%$(O) : %.cpp
	$(COMPILER.cc) ${CFLAGS.ENGINE.PHYSICS} -I. $(PATH.H.SYS) -o $@ -c $< 

${SOURCE.ENGINE.PHYSICS.HAVOK.OBJ}: $(DIR.OBJ)/%$(O) : %.cpp
	$(COMPILER.cc) ${CFLAGS.ENGINE.PHYSICS.HAVOK} -I. $(PATH.H.SYS) -o $@ -c $< 

${SOURCE.ENGINE.PHYSICS.HAVOK.TNGPHYSFSSTREAM.OBJ}: $(DIR.OBJ)/%$(O) : %.cpp
	$(COMPILER.cc) ${CFLAGS.ENGINE.PHYSICS.HAVOK.TNGPHYSFSSTREAM} -I. $(PATH.H.SYS) -o $@ -c $< 

${SOURCE.ENGINE.RENDER.OBJ}: $(DIR.OBJ)/%$(O) : %.cpp
	$(COMPILER.cc) ${CFLAGS.ENGINE.RENDER} -I. $(PATH.H.SYS) -o $@ -c $< 

${SOURCE.ENGINE.ENTITIES.OBJ}: $(DIR.OBJ)/%$(O) : %.cpp
	$(COMPILER.cc) ${CFLAGS.ENGINE.ENTITIES} -I. $(PATH.H.SYS) -o $@ -c $< 

${SOURCE.ENGINE.SCRIPTING.OBJ}: $(DIR.OBJ)/%$(O) : %.cpp
	$(COMPILER.cc) ${CFLAGS.ENGINE.SCRIPTING} -I. $(PATH.H.SYS) -o $@ -c $< 

${SOURCE.ENGINE.SCRIPTING_C.OBJ}: $(DIR.OBJ)/%$(O) : %.c
	$(COMPILER.cc) ${CFLAGS.ENGINE.SCRIPTING} -I. $(PATH.H.SYS) -o $@ -c $< 

${SOURCE.ENGINE.SCRIPTING.LUA.OBJ}: $(DIR.OBJ)/%$(O) : %.c
	$(COMPILER.c) ${CFLAGS.ENGINE.SCRIPTING.LUA} -I. $(PATH.H.SYS) -o $@ -c $< 

#all objects
SOURCE.MORPHINE.OBJ =\
		$(SOURCE.ENGINE.AUDIO.OBJ) \
		$(SOURCE.ENGINE.ENTITIES.OBJ) \
		$(SOURCE.ENGINE.CORE.OBJ) \
		$(SOURCE.ENGINE.CORE.FILESYSTEM.OBJ) \
		$(SOURCE.ENGINE.CORE.MATH.OBJ) \
		$(SOURCE.ENGINE.ENGINE.OBJ) \
		$(SOURCE.ENGINE.GAME.OBJ) \
		$(SOURCE.ENGINE.ENGINE.MENU.OBJ) \
		$(SOURCE.ENGINE.FILESYSTEM.OBJ) \
		$(SOURCE.ENGINE.FILESYSTEM_C.OBJ) \
		$(SOURCE.ENGINE.PATHFINDING.OBJ) \
		$(SOURCE.ENGINE.RENDER.OBJ) \
		$(SOURCE.ENGINE.SCRIPTING.OBJ) \
		$(SOURCE.ENGINE.SCRIPTING_C.OBJ) \
		$(SOURCE.ENGINE.SCRIPTING.LUA.OBJ) \
		$(SOURCE.ENGINE.NET.OBJ) 

SOURCE.MORPHINE.OBJ += $(SOURCE.ENGINE.PHYSICS.OBJ)
SOURCE.MORPHINE.OBJ += $(SOURCE.ENGINE.PHYSICS.HAVOK.OBJ)
SOURCE.MORPHINE.OBJ += $(SOURCE.ENGINE.PHYSICS.HAVOK.TNGPHYSFSSTREAM.OBJ)

#		$(SOURCE.ENGINE.NET.PKTV.OBJ) \
#		$(SOURCE.ENGINE.OBJ) \

#		$(SOURCE.ENGINE.OBJ) \
#		$(SOURCE.ENGINE.AUDIO.OBJ) \
#		$(SOURCE.ENGINE.CORE.OBJ) \
#		$(SOURCE.ENGINE.CORE.FILESYSTEM.OBJ) \
#		$(SOURCE.ENGINE.CORE.MATH.OBJ) \
#		$(SOURCE.ENGINE.ENGINE.OBJ) \
#		$(SOURCE.ENGINE.ENGINE.MENU.OBJ) \
#		$(SOURCE.ENGINE.FILESYSTEM.OBJ) \
# #?? means it seems no use , and a lot of compile problem inside
#??		$(SOURCE.ENGINE.GAME.OBJ) \
#??		$(SOURCE.LOADER.OBJ) \
#		$(SOURCE.ENGINE.NET.OBJ) \
#		$(SOURCE.ENGINE.NET.PKTV.OBJ) \
#		$(SOURCE.ENGINE.PATHFINDING.OBJ) \
#		$(SOURCE.ENGINE.PHYSICS.OBJ) \
#		$(SOURCE.ENGINE.PHYSICS.HAVOK.OBJ) \
#		$(SOURCE.ENGINE.RENDER.OBJ) \
#		$(SOURCE.ENGINE.ENTITIES.OBJ) \
#		$(SOURCE.ENGINE.SCRIPTING.OBJ) \
#		$(SOURCE.ENGINE.SCRIPTING.LUA.OBJ) \

#		$(SOURCE.PAINKILLER.OBJ) \
#		$(SOURCE.D3DEV.OBJ) \

#		$(SOURCE.EMPTY)
		
#SOURCE.MORPHINE.OBJ:=$(addprefix $(DIR.OBJ)/, $(addsuffix $O, $(basename $(SOURCE.MORPHINE))) )

#SOURCE.ALL += $(DIR.LIST)

#DIR.LIST=$(DIR.OBJ)/ $(addprefix $(DIR.OBJ)/, $(sort $(dir $(SOURCE.ALL))))

targetsclean += torqueClean

#---------------------------------------
# Set up include variables here.
#INCLUDES_BASE = -I../lib/zlib -I../lib/lungif -I../lib/lpng -I../lib/ljpeg -I../lib/directx8 -I../lib/oggvorbis/include
INCLUDES_LINUX = $(INCLUDES_BASE) 
INCLUDES_PS2DEVKIT = $(INCLUDES_BASE) -I../ps2libs/tg_ps2/include -I../crosslibs/include -I../physfs -I../ps2libs/ps2sdk/ps2shell -I./External/havok3/include/common -I./External/havok3/include/physics -I./External/havok3/include/plugins/common
INCLUDES_PSPDEVKIT = $(INCLUDES_BASE) -I../psplibs/tg_psp/include -I../crosslibs/include -I../psplibs/tgCommon/include -I./External/havok3/include/common -I./External/havok3/include/physics -I./External/havok3/include/plugins/common -I../physfs


#----------------------------------------
# normal binary
$(EXE_NAME): $(DIR.OBJ)/$(EXE_NAME)$(EXT.EXE)

DIR.LIST=$(DIR.OBJ)/ $(sort $(dir $(SOURCE.MORPHINE.OBJ)))
#DIR.LIST=$(DIR.OBJ)/ $(addprefix $(DIR.OBJ)/, $(sort $(dir $(SOURCE.ALL))))

$(DIR.LIST): morphine.source.mk

$(DIR.OBJ)/$(EXE_NAME)$(EXT.EXE): CFLAGS += $(INCLUDES_$(OS))

$(DIR.OBJ)/$(EXE_NAME)$(EXT.EXE): LIB.PATH +=../lib/$(DIR.OBJ) \

# PS2GL_LIBNAMES is only defined for PS2 platforms
# SCESHORT_LIBS, SCELIBDIRS, PS2_CRT0 and PS2_STDLINK only defined on PS2DEVKIT
# on all other platforms, these variables should stay empty
LINKLINE=
DEPENDLINE=

ifeq "$(OS)" "PS2DEVKIT"
LINKLINE += $(addprefix -l, $(PC2PS2_SHORT_LIBS)) $(MILESPS2_LIBDIRS)/mp3ps2.a $(MILESPS2_LIBDIRS)/mssps2.a $(addprefix -l, $(BINKPS2SHORT_LIBS)) $(addprefix -l, $(ZLIBSHORT_LIBS)) $(addprefix -l, $(PS2GL_SHORT_LIBS)) $(addprefix -l, $(SCESHORT_LIBS)) $(addprefix -l, $(PHYSFS_SHORT_LIBS))   $(SCELIBDIRS) -L$(MILESPS2_LIBDIRS) -L$(BINKPS2_LIBDIRS) -L$(PC2PS2_LIBDIR) -L$(PS2GL_LIBDIR) -L$(PHYSFS_LIBDIR) -L$(ZLIB_LIBDIRS) -L$(HAVOK3_LIBDIR) $(PS2_STDLINK)
#LINKLINE += $(addprefix -l, $(PC2PS2_SHORT_LIBS)) $(MILESPS2_LIBDIRS)/mp3ps2.a $(MILESPS2_LIBDIRS)/mssps2.a $(addprefix -l, $(BINKPS2SHORT_LIBS)) $(addprefix -l, $(ZLIBSHORT_LIBS)) $(addprefix -l, $(PS2GL_SHORT_LIBS)) $(addprefix -l, $(SCESHORT_LIBS)) $(addprefix -l, $(PHYSFS_SHORT_LIBS))   $(SCELIBDIRS) -L$(MILESPS2_LIBDIRS) -L$(BINKPS2_LIBDIRS) -L$(PC2PS2_LIBDIR) -L$(PS2GL_LIBDIR) -L$(PHYSFS_LIBDIR) -L$(ZLIB_LIBDIRS) -L$(HAVOK3_LIBDIR) $(PS2_STDLINK)
DEPENDLINE += $(PHYSFS_LIBDIR)lib$(PHYSFS_SHORT_LIBS).a
DEPENDLINE += $(PC2PS2_LIBDIR)/lib$(PC2PS2_SHORT_LIBS).a
DEPENDLINE += $(ZLIB_LIBDIRS)/lib$(ZLIBSHORT_LIBS).a
DEPENDLINE += $(addsuffix .a, $(addprefix $(PS2GL_LIBDIR)/lib, $(PS2GL_SHORT_LIBS)))
endif

ifeq "$(OS)" "PSPDEVKIT"
LINKLINE += $(addprefix -l, $(PSPSHORT_LIBS)) $(addprefix -l, $(PC2PSP_SHORT_LIBS)) $(addprefix -l, $(PHYSFS_SHORT_LIBS)) $(addprefix -l, $(ZLIBSHORT_LIBS)) $(addprefix -l, $(TGCOMMON_SHORT_LIBS)) $(PSPLOADLIBES) -L$(PHYSFS_LIBDIR) -L$(PSPLIBDIRS) -L$(PC2PSP_LIBDIR) -L$(ZLIB_LIBDIRS) -L$(TGCOMMON_LIBDIR) -L$(HAVOK3_LIBDIR) $(PSP_STDLINK)
DEPENDLINE += $(PHYSFS_LIBDIR)lib$(PHYSFS_SHORT_LIBS).a
DEPENDLINE += $(PC2PSP_LIBDIR)/lib$(PC2PSP_SHORT_LIBS).a
DEPENDLINE += $(ZLIB_LIBDIRS)/lib$(ZLIBSHORT_LIBS).a
DEPENDLINE += $(PSP_EXT_LIBS_DIR)/tgCommon/lib/lib$(TGCOMMON_SHORT_LIBS).a
endif

#$(DIR.OBJ)/$(EXE_NAME)$(EXT.EXE): $(DIR.OBJ) $(DIR.LIST) $(SOURCE.MORPHINE.OBJ) $(PS2GL_LIBNAMES) $(PS2_CRT0) 
$(DIR.OBJ)/$(EXE_NAME)$(EXT.EXE): $(DIR.OBJ) $(DIR.LIST) $(SOURCE.MORPHINE.OBJ) $(PS2_CRT0) $(DEPENDLINE)
	echo linking now
	$(DO.LINK.CONSOLE.EXE) $(LINKLINE)
	$(CP) $(DIR.OBJ)/$(EXE_NAME)$(BUILD_SUFFIX).* ../example/Game/

$(DIR.OBJ)/crt0.o: $(SCEDIR)/ee/lib/crt0.s
	$ ee-gcc -c -xassembler-with-cpp -Wa,-al -o $@ $< > $*.lst


#----------------------------------------
#----------------------------------------
torqueClean:
ifneq ($(wildcard $(EXE_NAME)_DEBUG.*),)
	-$(RM)  $(EXE_NAME)$(BUILD_SUFFIX)*
endif
ifneq ($(wildcard $(EXE_NAME)_RELEASE.*),)
	-$(RM)  $(EXE_NAME)_RELEASE*
endif
