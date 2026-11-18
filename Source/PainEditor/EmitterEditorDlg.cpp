// EmitterEditorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PainEditor.h"
#include "EmitterEditorDlg.h"
#include "Game/Main.h"
#include "MainFrm.h"
#include ".\emittereditordlg.h"
#include "Render/Viewport.h"
#include "Render/View.h"
#include "Render/ParticleSystem.h"
#include "Render/Video.h"
#include "Render/Material.h"
#include "Engine/World.h"

#ifdef TRANSGAMING
#include <string.h>
#endif

#include "FileSystem/GFileManager.h"
extern ENGINE_API GFileManager  	GFileMan;

extern Vector FXPos;

// CEmitterEditorDlg dialog

#ifdef EDITOR


IMPLEMENT_DYNAMIC(CEmitterEditorDlg, CScrolledInnerDlg)
CEmitterEditorDlg::CEmitterEditorDlg(CWnd* pParent /*=NULL*/)
	: CScrolledInnerDlg()
	, m_Evolve(FALSE)
	, m_Warp(FALSE)
	, m_MaxParticles(0)
	, m_SpawnRate(0)
	, m_LifeTimeMin(0)
	, m_LifeTimeMax(0)
	, m_DepthTest(FALSE)
	, m_UseRandomNormal(FALSE)
	, m_Preview(FALSE)
	, m_AlphaStart(0)
	, m_AlphaEnd(0)
	, m_Texture(_T(""))
	, m_WarpTexture(_T(""))
	, m_TexFPS(0)
	, m_PosXMin(0)
	, m_PosXMax(0)
	, m_PosYMin(0)
	, m_PosYMax(0)
	, m_PosZMin(0)
	, m_PosZMax(0)
	, m_RotMin(0)
	, m_RotMax(0)
	, m_VelXMin(0)
	, m_VelXMax(0)
	, m_VelYMin(0)
	, m_VelYMax(0)
	, m_VelZMin(0)
	, m_VelZMax(0)
	, m_VelEndXMin(0)
	, m_VelEndXMax(0)
	, m_VelEndYMin(0)
	, m_VelEndYMax(0)
	, m_VelEndZMin(0)
	, m_VelEndZMax(0)
	, m_AccelXMin(0)
	, m_AccelXMax(0)
	, m_AccelYMin(0)
	, m_AccelYMax(0)
	, m_AccelZMin(0)
	, m_AccelZMax(0)
	, m_SizeStartMin(0)
	, m_SizeStartMax(0)
	, m_SizeEndMin(0)
	, m_SizeEndMax(0)
	, m_LengthMin(0)
	, m_LengthMax(0)
	, m_ThicknessMin(0)
	, m_ThicknessMax(0)
	, m_Immortal(FALSE)
	, m_AlphaMid(0)
	, m_FadeIn(0)
	, m_FadeOut(0)
	, m_VelBlendIn(0)
	, m_VelBlendOut(0)
{
    PFX = NULL;
    PE = NULL;

}

CEmitterEditorDlg::~CEmitterEditorDlg()
{
}


void CEmitterEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);		 
	DDX_Control(pDX, IDC_LIFETIME_RANGE, m_rngLifeTime);
	DDX_Control(pDX, IDC_FADE_RANGE, m_rngFade);
	DDX_Control(pDX, IDC_VELBLEND_RANGE, m_rngVelBlend);
	DDX_Control(pDX, IDC_POSX_RANGE, m_rngPosX);
	DDX_Control(pDX, IDC_POSY_RANGE, m_rngPosY);
	DDX_Control(pDX, IDC_POSZ_RANGE, m_rngPosZ);	
	DDX_Control(pDX, IDC_ROT_RANGE, m_rngRot);
	DDX_Control(pDX, IDC_VELX_RANGE, m_rngVelX);
	DDX_Control(pDX, IDC_VELY_RANGE, m_rngVelY);
	DDX_Control(pDX, IDC_VELZ_RANGE, m_rngVelZ);	
	DDX_Control(pDX, IDC_VELX_RANGE_END, m_rngVelEndX);
	DDX_Control(pDX, IDC_VELY_RANGE_END, m_rngVelEndY);
	DDX_Control(pDX, IDC_VELZ_RANGE_END, m_rngVelEndZ);	
	DDX_Control(pDX, IDC_ACCELX_RANGE, m_rngAccelX);
	DDX_Control(pDX, IDC_ACCELY_RANGE, m_rngAccelY);
	DDX_Control(pDX, IDC_ACCELZ_RANGE, m_rngAccelZ);	
	DDX_Control(pDX, IDC_SIZESTART_RANGE, m_rngSizeStart);	
	DDX_Control(pDX, IDC_SIZEEND_RANGE, m_rngSizeEnd);	
	DDX_Control(pDX, IDC_LENGTH_RANGE, m_rngLength);	
	DDX_Control(pDX, IDC_THICKNESS_RANGE, m_rngThickness);	
	DDX_Control(pDX, IDC_EMITTERS, m_cmbEmitters);
	DDX_Check(pDX, IDC_EVOLVE, m_Evolve);
	DDX_Check(pDX, IDC_WARP, m_Warp);
	DDX_Text(pDX, IDC_MAXPARTICLES_EDIT, m_MaxParticles);
	DDX_Text(pDX, IDC_SPAWNRATE_EDIT, m_SpawnRate);
	DDX_Control(pDX, IDC_MAXPARTICLES_SLIDER, m_sldMaxParticles);
	DDX_Control(pDX, IDC_SPAWNRATE_SLIDER, m_sldSpawnRate);
	DDX_Text(pDX, IDC_LIFETIME_MIN_EDIT, m_LifeTimeMin);
	DDX_Text(pDX, IDC_LIFETIME_MAX_EDIT, m_LifeTimeMax);
	DDX_Check(pDX, IDC_DEPTHTEST, m_DepthTest);
	DDX_Check(pDX, IDC_USERANDOMNORMAL, m_UseRandomNormal);
	DDX_Check(pDX, IDC_PREVIEW, m_Preview);
	DDX_Control(pDX, IDC_TYPES, m_cmbTypes);
	DDX_Control(pDX, IDC_BLENDMODES, m_cmbBlendModes);
	DDX_Control(pDX, IDC_MATERIALS, m_cmbMaterials);
	DDX_Control(pDX, IDC_COLORSTART, m_picColorStart);
	DDX_Control(pDX, IDC_COLOREND, m_picColorEnd);
	DDX_Text(pDX, IDC_ALPHASTART_EDIT, m_AlphaStart);
	DDX_Text(pDX, IDC_ALPHAEND_EDIT, m_AlphaEnd);
	DDX_Control(pDX, IDC_ALPHASTART_SLIDER, m_sldAlphaStart);
	DDX_Control(pDX, IDC_ALPHAEND_SLIDER, m_sldAlphaEnd);
	DDX_Check(pDX, IDC_COLORINTERPOLATION, m_ColorInterpolation);
	DDX_Text(pDX, IDC_TEXTURE_EDIT, m_Texture);
	DDX_Text(pDX, IDC_WARPTEXTURE_EDIT, m_WarpTexture);
	DDX_Control(pDX, IDC_FPS_SLIDER, m_sldTexFPS);
	DDX_Text(pDX, IDC_FPS_EDIT, m_TexFPS);

	DDV_RangeFloat(pDX, IDC_POSX_MIN_EDIT, m_PosXMin,"(-inf,inf)");
	DDV_RangeFloat(pDX, IDC_POSX_MAX_EDIT, m_PosXMax,"(-inf,inf)");
	DDV_RangeFloat(pDX, IDC_POSY_MIN_EDIT, m_PosYMin,"(-inf,inf)");
	DDV_RangeFloat(pDX, IDC_POSY_MAX_EDIT, m_PosYMax,"(-inf,inf)");
	DDV_RangeFloat(pDX, IDC_POSZ_MIN_EDIT, m_PosZMin,"(-inf,inf)");
	DDV_RangeFloat(pDX, IDC_POSZ_MAX_EDIT, m_PosZMax,"(-inf,inf)");

	DDV_RangeFloat(pDX, IDC_ROT_MIN_EDIT, m_RotMin,"(-inf,inf)");
	DDV_RangeFloat(pDX, IDC_ROT_MAX_EDIT, m_RotMax,"(-inf,inf)");

	DDV_RangeFloat(pDX, IDC_VELX_MIN_EDIT, m_VelXMin,"(-inf,inf)");
	DDV_RangeFloat(pDX, IDC_VELX_MAX_EDIT, m_VelXMax,"(-inf,inf)");
	DDV_RangeFloat(pDX, IDC_VELY_MIN_EDIT, m_VelYMin,"(-inf,inf)");
	DDV_RangeFloat(pDX, IDC_VELY_MAX_EDIT, m_VelYMax,"(-inf,inf)");
	DDV_RangeFloat(pDX, IDC_VELZ_MIN_EDIT, m_VelZMin,"(-inf,inf)");
	DDV_RangeFloat(pDX, IDC_VELZ_MAX_EDIT, m_VelZMax,"(-inf,inf)");

	DDV_RangeFloat(pDX, IDC_VELX_MIN_EDIT_END, m_VelEndXMin,"(-inf,inf)");
	DDV_RangeFloat(pDX, IDC_VELX_MAX_EDIT_END, m_VelEndXMax,"(-inf,inf)");
	DDV_RangeFloat(pDX, IDC_VELY_MIN_EDIT_END, m_VelEndYMin,"(-inf,inf)");
	DDV_RangeFloat(pDX, IDC_VELY_MAX_EDIT_END, m_VelEndYMax,"(-inf,inf)");
	DDV_RangeFloat(pDX, IDC_VELZ_MIN_EDIT_END, m_VelEndZMin,"(-inf,inf)");
	DDV_RangeFloat(pDX, IDC_VELZ_MAX_EDIT_END, m_VelEndZMax,"(-inf,inf)");

	DDV_RangeFloat(pDX, IDC_ACCELX_MIN_EDIT, m_AccelXMin,"(-inf,inf)");
	DDV_RangeFloat(pDX, IDC_ACCELX_MAX_EDIT, m_AccelXMax,"(-inf,inf)");
	DDV_RangeFloat(pDX, IDC_ACCELY_MIN_EDIT, m_AccelYMin,"(-inf,inf)");
	DDV_RangeFloat(pDX, IDC_ACCELY_MAX_EDIT, m_AccelYMax,"(-inf,inf)");
	DDV_RangeFloat(pDX, IDC_ACCELZ_MIN_EDIT, m_AccelZMin,"(-inf,inf)");
	DDV_RangeFloat(pDX, IDC_ACCELZ_MAX_EDIT, m_AccelZMax,"(-inf,inf)");

	DDV_RangeFloat(pDX, IDC_SIZESTART_MIN_EDIT, m_SizeStartMin,"(-inf,inf)");
	DDV_RangeFloat(pDX, IDC_SIZESTART_MAX_EDIT, m_SizeStartMax,"(-inf,inf)");
	DDV_RangeFloat(pDX, IDC_SIZEEND_MIN_EDIT, m_SizeEndMin,"(-inf,inf)");
	DDV_RangeFloat(pDX, IDC_SIZEEND_MAX_EDIT, m_SizeEndMax,"(-inf,inf)");
	DDV_RangeFloat(pDX, IDC_LENGTH_MIN_EDIT, m_LengthMin,"(-inf,inf)");
	DDV_RangeFloat(pDX, IDC_LENGTH_MAX_EDIT, m_LengthMax,"(-inf,inf)");
	DDV_RangeFloat(pDX, IDC_THICKNESS_MIN_EDIT, m_ThicknessMin,"(-inf,inf)");
	DDV_RangeFloat(pDX, IDC_THICKNESS_MAX_EDIT, m_ThicknessMax,"(-inf,inf)");

	// Examples: "Cohesion [0,inf)", "Friction Angle [0,90)", "(-inf,0)"
	//


	DDX_Check(pDX, IDC_IMMORTAL, m_Immortal);
	DDX_Text(pDX, IDC_ALPHAMID_EDIT, m_AlphaMid);
	DDX_Control(pDX, IDC_ALPHAMID_SLIDER, m_sldAlphaMid);
	DDX_Text(pDX, IDC_FADE_IN_EDIT, m_FadeIn);
	DDX_Text(pDX, IDC_FADE_OUT_EDIT, m_FadeOut);
	DDX_Text(pDX, IDC_VELBLEND_IN_EDIT, m_VelBlendIn);
	DDX_Text(pDX, IDC_VELBLEND_OUT_EDIT, m_VelBlendOut);
}


BEGIN_MESSAGE_MAP(CEmitterEditorDlg, CScrolledInnerDlg)
	ON_BN_CLICKED(IDC_SAVE, OnBnClickedSave)
	ON_BN_CLICKED(IDC_PREVIEW, OnBnClickedPreview)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_MAXPARTICLES_EDIT, OnEnChangeMaxparticlesEdit)
	ON_EN_CHANGE(IDC_SPAWNRATE_EDIT, OnEnChangeSpawnrateEdit)
	ON_BN_CLICKED(IDC_EVOLVE, OnBnClickedEvolve)
	ON_CBN_SELCHANGE(IDC_EMITTERS, OnCbnSelchangeEmitters)
	ON_REGISTERED_MESSAGE(RANGE_CHANGED, OnRangeChange)
	ON_EN_CHANGE(IDC_LIFETIME_MIN_EDIT, OnEnChangeLifetimeMinEdit)
	ON_EN_CHANGE(IDC_LIFETIME_MAX_EDIT, OnEnChangeLifetimeMaxEdit)
	ON_CBN_SELCHANGE(IDC_TYPES, OnCbnSelchangeTypes)
	ON_CBN_SELCHANGE(IDC_BLENDMODES, OnCbnSelchangeBlendmodes)
	ON_CBN_SELCHANGE(IDC_MATERIALS, OnCbnSelchangeMaterials)
	ON_BN_CLICKED(IDC_DEPTHTEST, OnBnClickedDepthtest)
	ON_BN_CLICKED(IDC_USERANDOMNORMAL, OnBnClickedUserandomnormal)
	ON_CPN_XT_SELENDOK(IDC_COLORSTART, OnSelEndColorStart)
	ON_CPN_XT_SELENDOK(IDC_COLOREND, OnSelEndColorEnd)
	ON_BN_CLICKED(IDC_COLORINTERPOLATION, OnBnClickedColorinterpolation)
	ON_EN_CHANGE(IDC_ALPHASTART_EDIT, OnEnChangeAlphastartEdit)
	ON_EN_CHANGE(IDC_ALPHAEND_EDIT, OnEnChangeAlphaendEdit)
	ON_BN_CLICKED(IDC_TEXBROWSER, OnBnClickedTexbrowser)
	ON_BN_CLICKED(IDC_WARPTEXBROWSER, OnBnClickedWarpTexbrowser)
	ON_EN_CHANGE(IDC_FPS_EDIT, OnEnChangeFpsEdit)
	ON_EN_CHANGE(IDC_POSX_MIN_EDIT, OnEnChangePosxMinEdit)
	ON_EN_CHANGE(IDC_POSX_MAX_EDIT, OnEnChangePosxMaxEdit)
	ON_EN_CHANGE(IDC_POSY_MIN_EDIT, OnEnChangePosyMinEdit)
	ON_EN_CHANGE(IDC_POSY_MAX_EDIT, OnEnChangePosyMaxEdit)
	ON_EN_CHANGE(IDC_POSZ_MIN_EDIT, OnEnChangePoszMinEdit)
	ON_EN_CHANGE(IDC_POSZ_MAX_EDIT, OnEnChangePoszMaxEdit)
	ON_BN_CLICKED(IDC_RESTART, OnBnClickedRestart)
	ON_EN_CHANGE(IDC_ROT_MAX_EDIT, OnEnChangeRotMaxEdit)
	ON_EN_CHANGE(IDC_ROT_MIN_EDIT, OnEnChangeRotMinEdit)
	ON_EN_CHANGE(IDC_VELX_MIN_EDIT, OnEnChangeVelxMinEdit)
	ON_EN_CHANGE(IDC_VELX_MAX_EDIT, OnEnChangeVelxMaxEdit)
	ON_EN_CHANGE(IDC_VELY_MIN_EDIT, OnEnChangeVelyMinEdit)
	ON_EN_CHANGE(IDC_VELY_MAX_EDIT, OnEnChangeVelyMaxEdit)
	ON_EN_CHANGE(IDC_VELZ_MIN_EDIT, OnEnChangeVelzMinEdit)
	ON_EN_CHANGE(IDC_VELZ_MAX_EDIT, OnEnChangeVelzMaxEdit)
	ON_EN_CHANGE(IDC_VELX_MIN_EDIT_END, OnEnChangeVelEndxMinEdit)
	ON_EN_CHANGE(IDC_VELX_MAX_EDIT_END, OnEnChangeVelEndxMaxEdit)
	ON_EN_CHANGE(IDC_VELY_MIN_EDIT_END, OnEnChangeVelEndyMinEdit)
	ON_EN_CHANGE(IDC_VELY_MAX_EDIT_END, OnEnChangeVelEndyMaxEdit)
	ON_EN_CHANGE(IDC_VELZ_MIN_EDIT_END, OnEnChangeVelEndzMinEdit)
	ON_EN_CHANGE(IDC_VELZ_MAX_EDIT_END, OnEnChangeVelEndzMaxEdit)
	ON_EN_CHANGE(IDC_ACCELX_MIN_EDIT, OnEnChangeAccelxMinEdit)
	ON_EN_CHANGE(IDC_ACCELX_MAX_EDIT, OnEnChangeAccelxMaxEdit)
	ON_EN_CHANGE(IDC_ACCELY_MIN_EDIT, OnEnChangeAccelyMinEdit)
	ON_EN_CHANGE(IDC_ACCELY_MAX_EDIT, OnEnChangeAccelyMaxEdit)
	ON_EN_CHANGE(IDC_ACCELZ_MIN_EDIT, OnEnChangeAccelzMinEdit)
	ON_EN_CHANGE(IDC_ACCELZ_MAX_EDIT, OnEnChangeAccelzMaxEdit)
	ON_EN_CHANGE(IDC_SIZESTART_MIN_EDIT, OnEnChangeSizestartMinEdit)
	ON_EN_CHANGE(IDC_SIZESTART_MAX_EDIT, OnEnChangeSizestartMaxEdit)
	ON_EN_CHANGE(IDC_SIZEEND_MIN_EDIT, OnEnChangeSizeendMinEdit)
	ON_EN_CHANGE(IDC_SIZEEND_MAX_EDIT, OnEnChangeSizeendMaxEdit)
	ON_EN_CHANGE(IDC_LENGTH_MIN_EDIT, OnEnChangeLengthMinEdit)
	ON_EN_CHANGE(IDC_LENGTH_MAX_EDIT, OnEnChangeLengthMaxEdit)
	ON_EN_CHANGE(IDC_THICKNESS_MIN_EDIT, OnEnChangeThicknessMinEdit)
	ON_EN_CHANGE(IDC_THICKNESS_MAX_EDIT, OnEnChangeThicknessMaxEdit)
	ON_EN_CHANGE(IDC_TEXTURE_EDIT, OnEnChangeTextureEdit)
	ON_EN_CHANGE(IDC_WARPTEXTURE_EDIT, OnEnChangeWarpTextureEdit)
	ON_BN_CLICKED(IDC_RELOAD, OnBnClickedReload)
	ON_BN_CLICKED(IDC_IMMORTAL, OnBnClickedImmortal)
	ON_EN_CHANGE(IDC_ALPHAMID_EDIT, OnEnChangeAlphamidEdit)
	ON_EN_CHANGE(IDC_FADE_IN_EDIT, OnEnChangeFadeInEdit)
	ON_EN_CHANGE(IDC_FADE_OUT_EDIT, OnEnChangeFadeOutEdit)
	ON_EN_CHANGE(IDC_VELBLEND_IN_EDIT, OnEnChangeVelBlendInEdit)
	ON_EN_CHANGE(IDC_VELBLEND_OUT_EDIT, OnEnChangeVelBlendOutEdit)
	ON_BN_CLICKED(IDC_WARP, OnBnClickedWarp)
END_MESSAGE_MAP()


// CEmitterEditorDlg message handlers

BOOL CEmitterEditorDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	FillEmittersCombo();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEmitterEditorDlg::FillEmittersCombo() 
{
	DynamicArray<String> files;

	m_cmbEmitters.ResetContent();

	GFileMan.FindFiles("../Data/Scripts/Emitters/*.ini",files,EFindFiles);	

	for (int i=0;i<files.Size();i++)
		m_cmbEmitters.AddString(String::StripExtension(files[i]).AsChar());
}

void CorrectMinMaxValue(float &vmin,float &vmax)
{
	float mi=vmin,ma=vmax;
	vmin = min(mi,ma);
	vmax = max(mi,ma);
}

void SetupRange(CRangeSlider *pRS,float &cmin,float &cmax,float rmin,float rmax,float &vmin,float &vmax)
{
	CorrectMinMaxValue(vmin,vmax);
	pRS->SetMinMax(rmin,rmax);
	pRS->SetRange(vmin,vmax);
	cmin = vmin;
	cmax = vmax;
}

void CEmitterEditorDlg::UpdateView()
{
	if (!PE) return;
	m_Evolve = PE->Evolve;
	m_Warp = PE->Warp;
	m_Immortal = PE->Immortal;

	
	m_MaxParticles = PE->MaxParticles;
	m_sldMaxParticles.SetRange(0,200);
	m_sldMaxParticles.SetTicFreq(10);
	m_sldMaxParticles.SetPos(m_MaxParticles);
	
	if (PE->SpawnRate>0)
		m_SpawnRate = 1.0f/PE->SpawnRate;
	else
		m_SpawnRate = 0;
	
	m_sldSpawnRate.SetRange(0,400);
	m_sldSpawnRate.SetTicFreq(10);
	m_sldSpawnRate.SetPos(m_SpawnRate);

	m_DepthTest = PE->DepthTest;
	m_UseRandomNormal = PE->UseRandomNormal;

	SetupRange(&m_rngLifeTime,m_LifeTimeMin,m_LifeTimeMax,0,10,PE->LifeTime.Min,PE->LifeTime.Max);

	m_cmbTypes.SetCurSel(PE->Type-1);

	switch (PE->BlendMode)
	{
	case RenderState::EBlendFilter:      m_cmbBlendModes.SetCurSel(0); break;
	case RenderState::EBlendAdd:         m_cmbBlendModes.SetCurSel(1); break;
	case RenderState::EBlendTranslucent: m_cmbBlendModes.SetCurSel(2); break;
	case RenderState::EBlendAlpha:       m_cmbBlendModes.SetCurSel(3); break;
	}

	if (PE->MaterialName.Len() > 0)
		m_cmbMaterials.SetCurSel(m_cmbMaterials.FindString(-1,PE->MaterialName.AsChar()));
	else
		m_cmbMaterials.SetCurSel(0);
		

	m_DepthTest = PE->DepthTest;
	m_UseRandomNormal = PE->UseRandomNormal;

	m_ColorInterpolation = PE->LerpColor;

	Vector cmin = PE->ColorRange.Min;
	Vector cmax = PE->ColorRange.Max;
	m_picColorStart.SetColor(RGB(cmin.X*255,cmin.Y*255,cmin.Z*255));
	m_picColorEnd.SetColor(RGB(cmax.X*255,cmax.Y*255,cmax.Z*255));	

	m_AlphaStart = PE->Alpha.Min*255;
	m_AlphaEnd = PE->Alpha.Max*255;

	m_sldAlphaStart.SetRange(0,255);
	m_sldAlphaStart.SetTicFreq(10);
	m_sldAlphaStart.SetPos(m_AlphaStart);

	m_AlphaMid = PE->AlphaMid*255;
	m_sldAlphaMid.SetRange(0,255);
	m_sldAlphaMid.SetTicFreq(10);
	m_sldAlphaMid.SetPos(m_AlphaMid);

	m_sldAlphaEnd.SetRange(0,255);
	m_sldAlphaEnd.SetTicFreq(10);
	m_sldAlphaEnd.SetPos(m_AlphaEnd);

	m_FadeIn = PE->FadeTime.Min;
	m_FadeOut = PE->FadeTime.Max;
	m_rngFade.SetMinMax(0,100);
	m_rngFade.SetRange(m_FadeIn,m_FadeOut);

	m_VelBlendIn = PE->VelBlend.Min;
	m_VelBlendOut = PE->VelBlend.Max;
	m_rngVelBlend.SetMinMax(0,100);
	m_rngVelBlend.SetRange(m_VelBlendIn,m_VelBlendOut);

	m_Texture = PE->TextureName.AsChar();
	m_WarpTexture = PE->WarpTexName.AsChar();

	m_sldTexFPS.SetRange(0,99);
	m_sldTexFPS.SetTicFreq(5);
	m_sldTexFPS.SetPos(0);
	m_TexFPS = 0;

	m_TexFPS = PE->TexAnimFPS;
	m_sldTexFPS.SetPos(m_TexFPS);

	// XXX - powymieniac ten shit na:
	// SetupRange(&m_rngLifeTime,m_LifeTimeMin,m_LifeTimeMax,0,10,PE->LifeTime.Min,PE->LifeTime.Max);

	CorrectMinMaxValue(PE->PosRange.Min.X,PE->PosRange.Max.X);
	m_PosXMin = PE->PosRange.Min.X;
	m_PosXMax = PE->PosRange.Max.X;
	m_rngPosX.SetMinMax(-100,100);
	m_rngPosX.SetRange(PE->PosRange.Min.X,PE->PosRange.Max.X);

	CorrectMinMaxValue(PE->PosRange.Min.Y,PE->PosRange.Max.Y);
	m_PosYMin = PE->PosRange.Min.Y;
	m_PosYMax = PE->PosRange.Max.Y;
	m_rngPosY.SetMinMax(-100,100);
	m_rngPosY.SetRange(PE->PosRange.Min.Y,PE->PosRange.Max.Y);

	CorrectMinMaxValue(PE->PosRange.Min.Z,PE->PosRange.Max.Z);
	m_PosZMin = PE->PosRange.Min.Z;
	m_PosZMax = PE->PosRange.Max.Z;
	m_rngPosZ.SetMinMax(-100,100);
	m_rngPosZ.SetRange(PE->PosRange.Min.Z,PE->PosRange.Max.Z);

	CorrectMinMaxValue(PE->RotRange.Min,PE->RotRange.Max);
	m_RotMin = PE->RotRange.Min;
	m_RotMax = PE->RotRange.Max;
	m_rngRot.SetMinMax(-100,100);
	m_rngRot.SetRange(PE->RotRange.Min,PE->RotRange.Max);


	CorrectMinMaxValue(PE->VelStart.Min.X,PE->VelStart.Max.X);
	m_VelXMin = PE->VelStart.Min.X;
	m_VelXMax = PE->VelStart.Max.X;
	m_rngVelX.SetMinMax(-500,500);
	m_rngVelX.SetRange(PE->VelStart.Min.X,PE->VelStart.Max.X);

	CorrectMinMaxValue(PE->VelStart.Min.Y,PE->VelStart.Max.Y);
	m_VelYMin = PE->VelStart.Min.Y;
	m_VelYMax = PE->VelStart.Max.Y;
	m_rngVelY.SetMinMax(-500,500);
	m_rngVelY.SetRange(PE->VelStart.Min.Y,PE->VelStart.Max.Y);

	CorrectMinMaxValue(PE->VelStart.Min.Z,PE->VelStart.Max.Z);
	m_VelZMin = PE->VelStart.Min.Z;
	m_VelZMax = PE->VelStart.Max.Z;
	m_rngVelZ.SetMinMax(-500,500);
	m_rngVelZ.SetRange(PE->VelStart.Min.Z,PE->VelStart.Max.Z);


	CorrectMinMaxValue(PE->VelEnd.Min.X,PE->VelEnd.Max.X);
	m_VelEndXMin = PE->VelEnd.Min.X;
	m_VelEndXMax = PE->VelEnd.Max.X;
	m_rngVelEndX.SetMinMax(-500,500);
	m_rngVelEndX.SetRange(PE->VelEnd.Min.X,PE->VelEnd.Max.X);

	CorrectMinMaxValue(PE->VelEnd.Min.Y,PE->VelEnd.Max.Y);
	m_VelEndYMin = PE->VelEnd.Min.Y;
	m_VelEndYMax = PE->VelEnd.Max.Y;
	m_rngVelEndY.SetMinMax(-500,500);
	m_rngVelEndY.SetRange(PE->VelEnd.Min.Y,PE->VelEnd.Max.Y);

	CorrectMinMaxValue(PE->VelEnd.Min.Z,PE->VelEnd.Max.Z);
	m_VelEndZMin = PE->VelEnd.Min.Z;
	m_VelEndZMax = PE->VelEnd.Max.Z;
	m_rngVelEndZ.SetMinMax(-500,500);
	m_rngVelEndZ.SetRange(PE->VelEnd.Min.Z,PE->VelEnd.Max.Z);

	m_AccelXMin = PE->AccelRange.Min.X;
	m_AccelXMax = PE->AccelRange.Max.X;
	m_rngAccelX.SetMinMax(-500,500);
	m_rngAccelX.SetRange(PE->AccelRange.Min.X,PE->AccelRange.Min.X);
	
	m_AccelYMin = PE->AccelRange.Min.Y;
	m_AccelYMax = PE->AccelRange.Max.Y;
	m_rngAccelY.SetMinMax(-500,500);
	m_rngAccelY.SetRange(PE->AccelRange.Min.Y,PE->AccelRange.Min.Y);

	m_AccelZMin = PE->AccelRange.Min.Z;
	m_AccelZMax = PE->AccelRange.Max.Z;
	m_rngAccelZ.SetMinMax(-500,500);
	m_rngAccelZ.SetRange(PE->AccelRange.Min.Z,PE->AccelRange.Min.Z);

	CorrectMinMaxValue(PE->StartSize.Min,PE->StartSize.Max);
	m_SizeStartMin = PE->StartSize.Min;
	m_SizeStartMax = PE->StartSize.Max;
	m_rngSizeStart.SetMinMax(0,15);
	m_rngSizeStart.SetRange(PE->StartSize.Min,PE->StartSize.Max);

	CorrectMinMaxValue(PE->EndSize.Min,PE->EndSize.Max);
	m_SizeEndMin = PE->EndSize.Min;
	m_SizeEndMax = PE->EndSize.Max;
	m_rngSizeEnd.SetMinMax(0,15);
	m_rngSizeEnd.SetRange(PE->EndSize.Min,PE->EndSize.Max);

	CorrectMinMaxValue(PE->Length.Min,PE->Length.Max);
	m_LengthMin = PE->Length.Min;
	m_LengthMax = PE->Length.Max;
	m_rngLength.SetMinMax(0,2*100);
	m_rngLength.SetRange(PE->Length.Min,PE->Length.Max);

	CorrectMinMaxValue(PE->Thickness.Min,PE->Thickness.Max);
	m_ThicknessMin = PE->Thickness.Min;
	m_ThicknessMax = PE->Thickness.Max;
	m_rngThickness.SetMinMax(0,5*100);
	m_rngThickness.SetRange(PE->Thickness.Min,PE->Thickness.Max);


	UpdateData(false);
}

void CEmitterEditorDlg::OnBnClickedSave()
{	
	if (!PE) return;
	CString txt;
	m_cmbEmitters.UpdateData(true);
	m_cmbEmitters.GetWindowText(txt);
	txt.Trim();
	if (txt.GetLength()>1)
	{
#ifdef TRANSGAMING
		char buffer[64];
		snprintf(buffer, 64, "../Data/Scripts/Emitters/%s.ini", txt );
		if (!GEngine->Video->ParticleSys.SaveEmitter(*PE,buffer))
#else
		if (!GEngine->Video->ParticleSys.SaveEmitter(*PE,String("../Data/Scripts/Emitters/")+txt+String(".ini")))
#endif
		{
			MessageBox("Cannot save emitter!!! ReadOnly?");
			return;
		}

		FillEmittersCombo();
		m_cmbEmitters.SetWindowText(txt);
		int i = m_cmbEmitters.FindString(-1,txt);
		m_cmbEmitters.SetCurSel(i);
	
		// laduje go na nowo aby stworzyc nowego mastera
		LoadEmitter( (m_Preview!=0), true);

		CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
		CParticleFXEditorDlg *dlg = (CParticleFXEditorDlg *) pWnd->m_wndParticleFXDlg.m_InnerDlg;
		dlg->FillAvailableEmittersCombo();
		i = dlg->m_lstAEmitters.FindString(-1,txt);
		dlg->m_lstAEmitters.SetCurSel(i);
	}
	else
		MessageBox("Invalid file name!!!");
}


void CEmitterEditorDlg::OnBnClickedPreview()
{
	if (!PE) return;

	m_Preview = !m_Preview;

	PFX->EnableDraw(m_Preview?true:false);

	Vector cv = GEngine->Video->GetActiveView()->mCamera.GetForwardVector();
	Vector cp = GEngine->Video->GetActiveView()->mCamera.GetPosition();

	FXPos.Set(cp.X+cv.X*7,cp.Y+cv.Y*7,cp.Z+cv.Z*7);
	PFX->SetPosition(FXPos);

	OnBnClickedRestart();
};

void CEmitterEditorDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{	
	if (!PE) return;
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	if ((CSliderCtrl *)pScrollBar ==  &m_sldMaxParticles)
	{
		PE->MaxParticles = m_sldMaxParticles.GetPos();
		m_MaxParticles = m_sldMaxParticles.GetPos();			
	}
	if ((CSliderCtrl *)pScrollBar ==  &m_sldSpawnRate)
	{
		m_SpawnRate = (float)m_sldSpawnRate.GetPos();
		PE->SpawnRate = 1.0f/m_SpawnRate;
	}
	if ((CSliderCtrl *)pScrollBar ==  &m_sldAlphaStart)
	{
		PE->Alpha.Min = (float)m_sldAlphaStart.GetPos()/255.0f;
		m_AlphaStart = m_sldAlphaStart.GetPos();
	}
	if ((CSliderCtrl *)pScrollBar ==  &m_sldAlphaEnd)
	{
		PE->Alpha.Max = (float)m_sldAlphaEnd.GetPos()/255.0f;
		m_AlphaEnd = m_sldAlphaEnd.GetPos();
	}
	if ((CSliderCtrl *)pScrollBar ==  &m_sldAlphaMid)
	{
		PE->AlphaMid = (float)m_sldAlphaMid.GetPos()/255.0f;
		m_AlphaMid = m_sldAlphaMid.GetPos();
	}
	if ((CSliderCtrl *)pScrollBar ==  &m_sldTexFPS)
	{
		PE->TexAnimFPS  = m_sldTexFPS.GetPos();
		m_TexFPS = m_sldTexFPS.GetPos();
	}
	//GEngine->TempClient->EdParticle->Restart();
	//GEngine->TempClient->EdParticle->Active = true;
	
	UpdateData(false);
	GEngine->Video->ParticleSys.UpdateAllEmittersWithMaster(PE);
}

void CEmitterEditorDlg::OnEnChangeMaxparticlesEdit()
{
	if (!PE) return;
	UpdateData(true);
	m_sldMaxParticles.SetPos(m_MaxParticles);
	PE->MaxParticles = m_MaxParticles;
	GEngine->Video->ParticleSys.UpdateAllEmittersWithMaster(PE);
}

void CEmitterEditorDlg::OnEnChangeSpawnrateEdit()
{
	if (!PE) return;
	UpdateData(true);
	m_sldSpawnRate.SetPos(m_SpawnRate);
	PE->SpawnRate = 1.0f/m_SpawnRate;
	GEngine->Video->ParticleSys.UpdateAllEmittersWithMaster(PE);
}

void CEmitterEditorDlg::OnBnClickedEvolve()
{
	if (!PE) return;
	m_Evolve = !m_Evolve;
	PE->Evolve = m_Evolve;
	PFX->Restart();
	GEngine->Video->ParticleSys.UpdateAllEmittersWithMaster(PE);
}

void CEmitterEditorDlg::OnBnClickedWarp()
{
	if (!PE) return;
	m_Warp = !m_Warp;
	PE->Warp = m_Warp;
	PFX->Restart();
	GEngine->Video->ParticleSys.UpdateAllEmittersWithMaster(PE);
}

void CEmitterEditorDlg::LoadEmitter(bool preview, bool reload)
{
	if (PFX)
	{
#ifdef TRANSGAMING
		GEngine->pcfWorld->DeleteEntity(PFX);
#else
		GEngine->World->DeleteEntity(PFX);
#endif
		PFX = NULL;
		PE = NULL;
	}


	int i = m_cmbEmitters.GetCurSel();
	if (i>=0)
	{
		CString txt;
		m_cmbEmitters.GetLBText(i,txt);
#ifdef TRANSGAMING
		PFX = (ParticleEffect*) GEngine->pcfWorld->CreateEntity(Entity::ETypeParticleFx,"editor",NULL,1);
#else
		PFX = (ParticleEffect*) GEngine->World->CreateEntity(Entity::ETypeParticleFx,"editor",NULL,1);
#endif
		PFX ->AddEmitter(GEngine->Video->ParticleSys.CreateEmitter(txt+".ini",reload));
		PFX->IsImmortal = true;
		PFX->Flags &= ~Entity::EEntDestroyOnReload;
        Vector cv = GEngine->Video->GetActiveView()->mCamera.GetForwardVector();
		Vector cp = GEngine->Video->GetActiveView()->mCamera.GetPosition();
		if (FXPos.X == -99999)
			FXPos.Set(cp.X+cv.X*7,cp.Y+cv.Y*7,cp.Z+cv.Z*7);
		PFX->SetPosition(FXPos);
#ifdef TRANSGAMING
		GEngine->pcfWorld->AddEntity(PFX);
#else
		GEngine->World->AddEntity(PFX);
#endif
		if (preview)
		{
			PFX->Restart();
			PFX->Flags &= ~Entity::EEntNoDraw;
			m_Preview = true;
		}
		else
		{
			PFX->Flags |= Entity::EEntNoDraw;
			m_Preview = false;
		}
		UpdateData(false);

		PE = GEngine->Video->ParticleSys.GetMasterEmitter(txt+".ini");
	}

	UpdateView();
	UpdateView();
}

void CEmitterEditorDlg::OnCbnSelchangeEmitters()
{
	LoadEmitter( (m_Preview!=0), false);
	AfxGetMainWnd()->SetFocus();
}

void Range2Edit(CRangeSlider *pRS,float &vmin,float &vmax,float &cmin,float &cmax,int nfl = 0)
{
	float l = pRS->GetLeft();
	float r = pRS->GetRight();
	nfl = nfl?pow(10,nfl):1;
	l = float((int)(l*nfl))/nfl;
	r = float((int)(r*nfl))/nfl;
	vmin = l;
	vmax = r;
	cmin = l;
	cmax = r;	
}

LRESULT CEmitterEditorDlg::OnRangeChange(WPARAM wParam, LPARAM  lParam ) {
	if (!PE) return 0;
	UpdateData();
	CRangeSlider *pRS = (CRangeSlider *)lParam;

	if (pRS == &m_rngLifeTime)  
		Range2Edit(pRS,PE->LifeTime.Min,PE->LifeTime.Max,m_LifeTimeMin,m_LifeTimeMax,1);

	if (pRS == &m_rngPosX)
		Range2Edit(pRS,PE->PosRange.Min.X,PE->PosRange.Max.X,m_PosXMin,m_PosXMax,1);

	if (pRS == &m_rngPosY)
		Range2Edit(pRS,PE->PosRange.Min.Y,PE->PosRange.Max.Y,m_PosYMin,m_PosYMax,1);

	if (pRS == &m_rngPosZ)
		Range2Edit(pRS,PE->PosRange.Min.Z,PE->PosRange.Max.Z,m_PosZMin,m_PosZMax,1);

	if (pRS == &m_rngRot)
		Range2Edit(pRS,PE->RotRange.Min,PE->RotRange.Max,m_RotMin,m_RotMax,1);

	if (pRS == &m_rngVelX)
		Range2Edit(pRS,PE->VelStart.Min.X,PE->VelStart.Max.X,m_VelXMin,m_VelXMax,1);

	if (pRS == &m_rngVelY)
		Range2Edit(pRS,PE->VelStart.Min.Y,PE->VelStart.Max.Y,m_VelYMin,m_VelYMax,1);

	if (pRS == &m_rngVelZ)
		Range2Edit(pRS,PE->VelStart.Min.Z,PE->VelStart.Max.Z,m_VelZMin,m_VelZMax,1);

	
	if (pRS == &m_rngVelEndX)
		Range2Edit(pRS,PE->VelEnd.Min.X,PE->VelEnd.Max.X,m_VelEndXMin,m_VelEndXMax,1);

	if (pRS == &m_rngVelEndY)
		Range2Edit(pRS,PE->VelEnd.Min.Y,PE->VelEnd.Max.Y,m_VelEndYMin,m_VelEndYMax,1);

	if (pRS == &m_rngVelEndZ)
		Range2Edit(pRS,PE->VelEnd.Min.Z,PE->VelEnd.Max.Z,m_VelEndZMin,m_VelEndZMax,1);

	if (pRS == &m_rngAccelX)
		Range2Edit(pRS,PE->AccelRange.Min.X,PE->AccelRange.Max.X,m_AccelXMin,m_AccelXMax,1);

	if (pRS == &m_rngAccelY)
		Range2Edit(pRS,PE->AccelRange.Min.Y,PE->AccelRange.Max.Y,m_AccelYMin,m_AccelYMax,1);

	if (pRS == &m_rngAccelZ)
		Range2Edit(pRS,PE->AccelRange.Min.Z,PE->AccelRange.Max.Z,m_AccelZMin,m_AccelZMax,1);

	if (pRS == &m_rngSizeStart)
		Range2Edit(pRS,PE->StartSize.Min,PE->StartSize.Max,m_SizeStartMin,m_SizeStartMax,1);
		
	if (pRS == &m_rngSizeEnd)
		Range2Edit(pRS,PE->EndSize.Min,PE->EndSize.Max,m_SizeEndMin,m_SizeEndMax,1);

	if (pRS == &m_rngLength)
		Range2Edit(pRS,PE->Length.Min,PE->Length.Max,m_LengthMin,m_LengthMax,1);

	if (pRS == &m_rngThickness)
		Range2Edit(pRS,PE->Thickness.Min,PE->Thickness.Max,m_ThicknessMin,m_ThicknessMax,1);

	if (pRS == &m_rngFade)
		Range2Edit(pRS,PE->FadeTime.Min,PE->FadeTime.Max,m_FadeIn,m_FadeOut,1);

	if (pRS == &m_rngVelBlend)
		Range2Edit(pRS,PE->VelBlend.Min,PE->VelBlend.Max,m_VelBlendIn,m_VelBlendOut,1);

	GEngine->Video->ParticleSys.UpdateAllEmittersWithMaster(PE);
	UpdateData(FALSE);
	return 0;
}
void CEmitterEditorDlg::OnEnChangeLifetimeMinEdit()
{
	EditToRange(&m_rngLifeTime,m_LifeTimeMin,m_LifeTimeMax,PE->LifeTime.Min,PE->LifeTime.Max);
}

void CEmitterEditorDlg::OnEnChangeLifetimeMaxEdit()
{
	EditToRange(&m_rngLifeTime,m_LifeTimeMin,m_LifeTimeMax,PE->LifeTime.Min,PE->LifeTime.Max);
}

void CEmitterEditorDlg::OnCbnSelchangeTypes()
{
	if (!PE) return;
	PE->Type = m_cmbTypes.GetCurSel()+1;
	GEngine->Video->ParticleSys.UpdateAllEmittersWithMaster(PE);
}

void CEmitterEditorDlg::OnCbnSelchangeBlendmodes()
{
	if (!PE) return;
	switch (m_cmbBlendModes.GetCurSel())
	{
	case 0: PE->BlendMode = RenderState::EBlendFilter; break;
	case 1: PE->BlendMode = RenderState::EBlendAdd; break;
	case 2: PE->BlendMode = RenderState::EBlendTranslucent; break;
	case 3: PE->BlendMode = RenderState::EBlendAlpha; break;
	}
	GEngine->Video->ParticleSys.UpdateAllEmittersWithMaster(PE);
}

void CEmitterEditorDlg::OnCbnSelchangeMaterials()
{
	if (!PE) return;
	
	int i = m_cmbMaterials.GetCurSel();
	if (i<0) return;

	CString name;
	m_cmbMaterials.GetLBText(m_cmbMaterials.GetCurSel(),name);
	if (name == "" || name == "none")
	{
		PE->MaterialName = "";
		PE->MaterialPtr = NULL;
	}
	else
	{
		PE->MaterialName = (LPCSTR)name;
		PE->MaterialPtr	= GEngine->Video->MaterialSys->FindMaterial(PE->MaterialName.AsChar());
	}

	
	GEngine->Video->ParticleSys.UpdateAllEmittersWithMaster(PE);
}

void CEmitterEditorDlg::OnBnClickedDepthtest()
{
	if (!PE) return;
	m_DepthTest = !m_DepthTest;
	PE->DepthTest = m_DepthTest?true:false;
	GEngine->Video->ParticleSys.UpdateAllEmittersWithMaster(PE);
}

void CEmitterEditorDlg::OnBnClickedUserandomnormal()
{
	if (!PE) return;
	m_UseRandomNormal = !m_UseRandomNormal;
	PE->UseRandomNormal = m_UseRandomNormal;
	GEngine->Video->ParticleSys.UpdateAllEmittersWithMaster(PE);
}

void CEmitterEditorDlg::OnSelEndColorStart()
{
	if (!PE) return;
	int col = m_picColorStart.GetColor();
	PE->ColorRange.Min.X = GetRValue(col)/255.0f;
	PE->ColorRange.Min.Y = GetGValue(col)/255.0f;
	PE->ColorRange.Min.Z = GetBValue(col)/255.0f;
	GEngine->Video->ParticleSys.UpdateAllEmittersWithMaster(PE);
}

void CEmitterEditorDlg::OnSelEndColorEnd()
{
	if (!PE) return;
	int col = m_picColorEnd.GetColor();
	PE->ColorRange.Max.X = GetRValue(col)/255.0f;
	PE->ColorRange.Max.Y = GetGValue(col)/255.0f;
	PE->ColorRange.Max.Z = GetBValue(col)/255.0f;
	GEngine->Video->ParticleSys.UpdateAllEmittersWithMaster(PE);
}

void CEmitterEditorDlg::OnBnClickedColorinterpolation()
{
	if (!PE) return;
	m_ColorInterpolation = !m_ColorInterpolation;
	PE->LerpColor = m_ColorInterpolation;
	GEngine->Video->ParticleSys.UpdateAllEmittersWithMaster(PE);
}

void CEmitterEditorDlg::OnEnChangeAlphastartEdit()
{
	if (!PE) return;

	UpdateData(true);
	m_sldAlphaStart.SetPos(m_AlphaStart);
	PE->Alpha.Min = (float)m_AlphaStart/255.0f;
	GEngine->Video->ParticleSys.UpdateAllEmittersWithMaster(PE);
}

void CEmitterEditorDlg::OnEnChangeAlphaendEdit()
{
	if (!PE) return;
	UpdateData(true);
	m_sldAlphaEnd.SetPos(m_AlphaEnd);
	PE->Alpha.Max = (float)m_AlphaEnd/255.0f;
	GEngine->Video->ParticleSys.UpdateAllEmittersWithMaster(PE);
}

void CEmitterEditorDlg::OnBnClickedTexbrowser()
{
	if (!PE) return;
	CFileDialog fileDlg (TRUE, "tex", "*.*",
		OFN_FILEMUSTEXIST| OFN_HIDEREADONLY|OFN_NOCHANGEDIR, "Textures (*.*)|*.*||", this);

	fileDlg.m_pOFN->lpstrInitialDir = "..\\Data\\Textures\\Particles";
	if( fileDlg.DoModal ()==IDOK )
	{
		m_Texture = fileDlg.GetFileName();
		PE->TextureName = (LPCSTR) m_Texture;
        String texPath("Particles//");
        texPath+=PE->TextureName;
		PE->TexturePtr = GEngine->Video->MaterialSys->CreateTexture(texPath.AsChar());
		PE->Restart();
		UpdateData(false);
	}
	GEngine->Video->ParticleSys.UpdateAllEmittersWithMaster(PE);
}

void CEmitterEditorDlg::OnBnClickedWarpTexbrowser()
{
	if (!PE) return;
	CFileDialog fileDlg (TRUE, "tex", "*.*",
		OFN_FILEMUSTEXIST| OFN_HIDEREADONLY|OFN_NOCHANGEDIR, "Textures (*.*)|*.*||", this);

	fileDlg.m_pOFN->lpstrInitialDir = "..\\Data\\Textures\\Particles";
	if( fileDlg.DoModal ()==IDOK )
	{
		m_WarpTexture = fileDlg.GetFileName();
		PE->WarpTexName = (LPCSTR) m_WarpTexture;
		String texPath("Particles//");
		texPath+=PE->WarpTexName;
		PE->WarpTexPtr = GEngine->Video->MaterialSys->CreateTexture(texPath.AsChar());
		PE->Restart();
		UpdateData(false);
	}
	GEngine->Video->ParticleSys.UpdateAllEmittersWithMaster(PE);
}

void CEmitterEditorDlg::OnEnChangeFpsEdit()
{
	if (!PE) return;
	UpdateData(true);
	m_sldTexFPS.SetPos(m_TexFPS);
	PE->TexAnimFPS = m_TexFPS;
	GEngine->Video->ParticleSys.UpdateAllEmittersWithMaster(PE);
}

void CEmitterEditorDlg::EditToRange(CRangeSlider *pRS,float &cmin,float &cmax,float &vmin,float &vmax)
{
	if (!PE) return;
	UpdateData(true);
	//if (cmax<cmin) cmax = cmin;
	//if (cmin>cmax) cmin = cmax;
	pRS->SetRange(cmin,cmax);
	vmin = cmin;
	vmax = cmax;
	//UpdateData(false);
	GEngine->Video->ParticleSys.UpdateAllEmittersWithMaster(PE);
}


void CEmitterEditorDlg::OnEnChangePosxMinEdit()
{
	EditToRange(&m_rngPosX,m_PosXMin,m_PosXMax,PE->PosRange.Min.X,PE->PosRange.Max.X);
}

void CEmitterEditorDlg::OnEnChangePosxMaxEdit()
{
	EditToRange(&m_rngPosX,m_PosXMin,m_PosXMax,PE->PosRange.Min.X,PE->PosRange.Max.X);
}

void CEmitterEditorDlg::OnEnChangePosyMinEdit()
{
	EditToRange(&m_rngPosY,m_PosYMin,m_PosYMax,PE->PosRange.Min.Y,PE->PosRange.Max.Y);
}

void CEmitterEditorDlg::OnEnChangePosyMaxEdit()
{
	EditToRange(&m_rngPosY,m_PosYMin,m_PosYMax,PE->PosRange.Min.Y,PE->PosRange.Max.Y);
}

void CEmitterEditorDlg::OnEnChangePoszMinEdit()
{
	EditToRange(&m_rngPosZ,m_PosZMin,m_PosZMax,PE->PosRange.Min.Z,PE->PosRange.Max.Z);
}

void CEmitterEditorDlg::OnEnChangePoszMaxEdit()
{
	EditToRange(&m_rngPosZ,m_PosZMin,m_PosZMax,PE->PosRange.Min.Z,PE->PosRange.Max.Z);
}

void CEmitterEditorDlg::OnEnChangeRotMinEdit()
{
	EditToRange(&m_rngRot,m_RotMin,m_RotMax,PE->RotRange.Min,PE->RotRange.Max);
}

void CEmitterEditorDlg::OnEnChangeRotMaxEdit()
{
	EditToRange(&m_rngRot,m_RotMin,m_RotMax,PE->RotRange.Min,PE->RotRange.Max);
}

void CEmitterEditorDlg::OnEnChangeVelxMinEdit()
{
	EditToRange(&m_rngVelX,m_VelZMin,m_VelZMax,PE->VelStart.Min.Z,PE->VelStart.Max.Z);
}

void CEmitterEditorDlg::OnEnChangeVelxMaxEdit()
{
	EditToRange(&m_rngVelX,m_VelXMin,m_VelXMax,PE->VelStart.Min.X,PE->VelStart.Max.X);
}

void CEmitterEditorDlg::OnEnChangeVelyMinEdit()
{
	EditToRange(&m_rngVelY,m_VelYMin,m_VelYMax,PE->VelStart.Min.Y,PE->VelStart.Max.Y);
}

void CEmitterEditorDlg::OnEnChangeVelyMaxEdit()
{
	EditToRange(&m_rngVelY,m_VelYMin,m_VelYMax,PE->VelStart.Min.Y,PE->VelStart.Max.Y);
}

void CEmitterEditorDlg::OnEnChangeVelzMinEdit()
{
	EditToRange(&m_rngVelZ,m_VelZMin,m_VelZMax,PE->VelStart.Min.Z,PE->VelStart.Max.Z);
}

void CEmitterEditorDlg::OnEnChangeVelzMaxEdit()
{
	EditToRange(&m_rngVelZ,m_VelZMin,m_VelZMax,PE->VelStart.Min.Z,PE->VelStart.Max.Z);
}







void CEmitterEditorDlg::OnEnChangeVelEndxMinEdit()
{
	EditToRange(&m_rngVelEndX,m_VelEndZMin,m_VelEndZMax,PE->VelEnd.Min.Z,PE->VelEnd.Max.Z);
}

void CEmitterEditorDlg::OnEnChangeVelEndxMaxEdit()
{
	EditToRange(&m_rngVelEndX,m_VelEndXMin,m_VelEndXMax,PE->VelEnd.Min.X,PE->VelEnd.Max.X);
}

void CEmitterEditorDlg::OnEnChangeVelEndyMinEdit()
{
	EditToRange(&m_rngVelEndY,m_VelEndYMin,m_VelEndYMax,PE->VelEnd.Min.Y,PE->VelEnd.Max.Y);
}

void CEmitterEditorDlg::OnEnChangeVelEndyMaxEdit()
{
	EditToRange(&m_rngVelEndY,m_VelEndYMin,m_VelEndYMax,PE->VelEnd.Min.Y,PE->VelEnd.Max.Y);
}

void CEmitterEditorDlg::OnEnChangeVelEndzMinEdit()
{
	EditToRange(&m_rngVelEndZ,m_VelEndZMin,m_VelEndZMax,PE->VelEnd.Min.Z,PE->VelEnd.Max.Z);
}

void CEmitterEditorDlg::OnEnChangeVelEndzMaxEdit()
{
	EditToRange(&m_rngVelEndZ,m_VelEndZMin,m_VelEndZMax,PE->VelEnd.Min.Z,PE->VelEnd.Max.Z);
}






void CEmitterEditorDlg::OnEnChangeAccelxMinEdit()
{
	EditToRange(&m_rngAccelX,m_AccelXMin,m_AccelXMax,PE->AccelRange.Min.X,PE->AccelRange.Max.X);
}

void CEmitterEditorDlg::OnEnChangeAccelxMaxEdit()
{
	EditToRange(&m_rngAccelX,m_AccelXMin,m_AccelXMax,PE->AccelRange.Min.X,PE->AccelRange.Max.X);
}

void CEmitterEditorDlg::OnEnChangeAccelyMinEdit()
{
	EditToRange(&m_rngAccelY,m_AccelYMin,m_AccelYMax,PE->AccelRange.Min.Y,PE->AccelRange.Max.Y);
}

void CEmitterEditorDlg::OnEnChangeAccelyMaxEdit()
{
	EditToRange(&m_rngAccelY,m_AccelYMin,m_AccelYMax,PE->AccelRange.Min.Y,PE->AccelRange.Max.Y);
}

void CEmitterEditorDlg::OnEnChangeAccelzMinEdit()
{
	EditToRange(&m_rngAccelZ,m_AccelZMin,m_AccelZMax,PE->AccelRange.Min.Z,PE->AccelRange.Max.Z);
}

void CEmitterEditorDlg::OnEnChangeAccelzMaxEdit()
{
	EditToRange(&m_rngAccelZ,m_AccelZMin,m_AccelZMax,PE->AccelRange.Min.Z,PE->AccelRange.Max.Z);
}


void CEmitterEditorDlg::OnEnChangeSizestartMinEdit()
{
	EditToRange(&m_rngSizeStart,m_SizeStartMin,m_SizeStartMax,PE->StartSize.Min,PE->StartSize.Max);
}

void CEmitterEditorDlg::OnEnChangeSizestartMaxEdit()
{
	EditToRange(&m_rngSizeStart,m_SizeStartMin,m_SizeStartMax,PE->StartSize.Min,PE->StartSize.Max);
}

void CEmitterEditorDlg::OnEnChangeSizeendMinEdit()
{
	EditToRange(&m_rngSizeEnd,m_SizeEndMin,m_SizeEndMax,PE->EndSize.Min,PE->EndSize.Max);
}

void CEmitterEditorDlg::OnEnChangeSizeendMaxEdit()
{
	EditToRange(&m_rngSizeEnd,m_SizeEndMin,m_SizeEndMax,PE->EndSize.Min,PE->EndSize.Max);
}

void CEmitterEditorDlg::OnEnChangeLengthMinEdit()
{
	EditToRange(&m_rngLength,m_LengthMin,m_LengthMax,PE->Length.Min,PE->Length.Max);
}

void CEmitterEditorDlg::OnEnChangeLengthMaxEdit()
{
	EditToRange(&m_rngLength,m_LengthMin,m_LengthMax,PE->Length.Min,PE->Length.Max);
}

void CEmitterEditorDlg::OnEnChangeThicknessMinEdit()
{
	EditToRange(&m_rngThickness,m_ThicknessMin,m_ThicknessMax,PE->Thickness.Min,PE->Thickness.Max);
}

void CEmitterEditorDlg::OnEnChangeThicknessMaxEdit()
{
	EditToRange(&m_rngThickness,m_ThicknessMin,m_ThicknessMax,PE->Thickness.Min,PE->Thickness.Max);
}

void CEmitterEditorDlg::OnBnClickedRestart()
{
	if (!PE) return;
#ifdef TRANSGAMING
	GEngine->pcfWorld->AddEntity(PFX);
#else
	GEngine->World->AddEntity(PFX);
#endif
	PFX->Restart();
	GEngine->Video->ParticleSys.UpdateAllEmittersWithMaster(PE);
}

void CEmitterEditorDlg::OnEnChangeTextureEdit()
{
	if (!PE) return;
#ifdef TRANSGAMING
	GEngine->pcfWorld->AddEntity(PFX);
#else
	GEngine->World->AddEntity(PFX);
#endif
	PFX->Restart();
	GEngine->Video->ParticleSys.UpdateAllEmittersWithMaster(PE);
}

void CEmitterEditorDlg::OnEnChangeWarpTextureEdit()
{
	if (!PE) return;
#ifdef TRANSGAMING
	GEngine->pcfWorld->AddEntity(PFX);
#else
	GEngine->World->AddEntity(PFX);
#endif
	PFX->Restart();
	GEngine->Video->ParticleSys.UpdateAllEmittersWithMaster(PE);
}

void CEmitterEditorDlg::OnBnClickedReload()
{
	if (!PE) return;
	LoadEmitter( (m_Preview!=0), true );
	GEngine->Video->ParticleSys.UpdateAllEmittersWithMaster(PE);
	PFX->Restart();
}

void CEmitterEditorDlg::OnBnClickedImmortal()
{
	if (!PE) return;
	m_Immortal = !m_Immortal;
	PE->Immortal = m_Immortal;

#ifdef TRANSGAMING
	GEngine->pcfWorld->AddEntity(PFX);
#else
	GEngine->World->AddEntity(PFX);
#endif
	PFX->Restart();
	GEngine->Video->ParticleSys.UpdateAllEmittersWithMaster(PE);
}

void CEmitterEditorDlg::OnEnChangeAlphamidEdit()
{
	if (!PE) return;

	UpdateData(true);
	m_sldAlphaMid.SetPos(m_AlphaMid);
	PE->AlphaMid = (float)m_AlphaMid/255.0f;
	GEngine->Video->ParticleSys.UpdateAllEmittersWithMaster(PE);
}

void CEmitterEditorDlg::OnEnChangeFadeInEdit()
{
	EditToRange(&m_rngFade,m_FadeIn,m_FadeOut,PE->FadeTime.Min,PE->FadeTime.Max);
}

void CEmitterEditorDlg::OnEnChangeFadeOutEdit()
{
	EditToRange(&m_rngFade,m_FadeIn,m_FadeOut,PE->FadeTime.Min,PE->FadeTime.Max);
}

void CEmitterEditorDlg::OnEnChangeVelBlendInEdit()
{
	EditToRange(&m_rngVelBlend,m_VelBlendIn,m_VelBlendOut,PE->VelBlend.Min,PE->VelBlend.Max);
}

void CEmitterEditorDlg::OnEnChangeVelBlendOutEdit()
{
	EditToRange(&m_rngVelBlend,m_VelBlendIn,m_VelBlendOut,PE->VelBlend.Min,PE->VelBlend.Max);
}

#endif