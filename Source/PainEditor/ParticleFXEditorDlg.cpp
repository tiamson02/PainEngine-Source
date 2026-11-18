// ParticleFXEditorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MainFrm.h"
#include "PainEditor.h"
#include "ParticleFXEditorDlg.h"
#include "Game/Main.h"
#include "Render/Viewport.h"
#include "Render/View.h"
#include "Render/Video.h"
#include "Engine/World.h"

#ifdef EDITOR

#include "Scripting/Scripting.h"
#include ".\particlefxeditordlg.h"
extern ENGINE_API Script	GScript;

#include "FileSystem/GFileManager.h"
extern ENGINE_API GFileManager  	GFileMan;

Vector FXPos(-99999,0,0);
// CParticleFXEditorDlg dialog

IMPLEMENT_DYNAMIC(CParticleFXEditorDlg, CScrolledInnerDlg)
CParticleFXEditorDlg::CParticleFXEditorDlg(CWnd* pParent /*=NULL*/)
	: CScrolledInnerDlg()
	, m_PosX(0)
	, m_PosY(0)
	, m_PosZ(0)
	, m_AngX(0)
	, m_AngY(0)
	, m_AngZ(0)
	, m_Scale(1)
	, m_Preview(FALSE)
	, FixedTransform(FALSE)
{
	Effect = NULL;
}

CParticleFXEditorDlg::~CParticleFXEditorDlg()
{
	if (Effect)
	{
#ifdef TRANSGAMING
		GEngine->pcfWorld->DeleteEntity(Effect);
#else
		GEngine->World->DeleteEntity(Effect);
#endif
		Effect = NULL;
	}
}

void CParticleFXEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_AEMITTERS, m_lstAEmitters);
	DDX_Control(pDX, IDC_SEMITTERS, m_lstSEmitters);
	DDX_Text(pDX, IDC_POSX, m_PosX);
	DDX_Text(pDX, IDC_POSY, m_PosY);
	DDX_Text(pDX, IDC_POSZ, m_PosZ);
	DDX_Text(pDX, IDC_ANGX, m_AngX);
	DDX_Text(pDX, IDC_ANGY, m_AngY);
	DDX_Text(pDX, IDC_ANGZ, m_AngZ);
	DDX_Text(pDX, IDC_SCALE, m_Scale);
	DDX_Control(pDX, IDC_EFFECTS, m_cmbEffects);
	DDX_Control(pDX, IDC_SCALE_SLIDER, m_sldScale);
	DDX_Check(pDX, IDC_PREVIEW, m_Preview);
	DDX_Check(pDX, IDC_FIXEDTRANSFORM, FixedTransform);
}



BEGIN_MESSAGE_MAP(CParticleFXEditorDlg, CScrolledInnerDlg)
	ON_BN_CLICKED(IDC_ADD, OnBnClickedAdd)
	ON_LBN_DBLCLK(IDC_AEMITTERS, OnLbnDblclkAemitters)
	ON_LBN_SELCHANGE(IDC_SEMITTERS, OnLbnSelchangeSemitters)
	ON_BN_CLICKED(IDC_DEC_Y, OnBnClickedDecY)
	ON_BN_CLICKED(IDC_INC_Y, OnBnClickedIncY)
	ON_BN_CLICKED(IDC_DEC_X, OnBnClickedDecX)
	ON_BN_CLICKED(IDC_INC_X, OnBnClickedIncX)
	ON_BN_CLICKED(IDC_INC_Z, OnBnClickedIncZ)
	ON_BN_CLICKED(IDC_DEC_Z, OnBnClickedDecZ)
	ON_BN_CLICKED(IDC_RESTART, OnBnClickedRestart)
	ON_BN_CLICKED(IDC_PREVIEW, OnBnClickedPreview)
	ON_CBN_SELCHANGE(IDC_EFFECTS, OnCbnSelchangeEffects)
	ON_BN_CLICKED(IDC_REMOVE, OnBnClickedRemove)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_DEC_AX, OnBnClickedDecAx)
	ON_BN_CLICKED(IDC_INC_AX, OnBnClickedIncAx)
	ON_BN_CLICKED(IDC_INC_AY, OnBnClickedIncAy)
	ON_BN_CLICKED(IDC_DEC_AY, OnBnClickedDecAy)
	ON_BN_CLICKED(IDC_DEC_AZ, OnBnClickedDecAz)
	ON_BN_CLICKED(IDC_INC_AZ, OnBnClickedIncAz)
	ON_EN_CHANGE(IDC_SCALE, OnEnChangeScale)
	ON_EN_CHANGE(IDC_POSX, OnEnChangePosx)
	ON_EN_CHANGE(IDC_POSZ, OnEnChangePosz)
	ON_EN_CHANGE(IDC_POSY, OnEnChangePosy)
	ON_EN_CHANGE(IDC_ANGX, OnEnChangeAngx)
	ON_EN_CHANGE(IDC_ANGY, OnEnChangeAngy)
	ON_EN_CHANGE(IDC_ANGZ, OnEnChangeAngz)
	ON_BN_CLICKED(IDC_EDIT, OnBnClickedEdit)
	ON_BN_CLICKED(IDC_SAVE, OnBnClickedSave)

	ON_WM_KEYDOWN()
	ON_LBN_DBLCLK(IDC_SEMITTERS, OnLbnDblclkSemitters)
	ON_BN_CLICKED(IDC_FRONTTOCAM, OnBnClickedFronttocam)
	ON_BN_CLICKED(IDC_SAVEALLEMITTERS, OnBnClickedSaveallemitters)
	ON_BN_CLICKED(IDC_FIXEDTRANSFORM, OnBnClickedFixedtransform)
END_MESSAGE_MAP()
// CEmitterEditorDlg message handlers

BOOL CParticleFXEditorDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	FillEffectsCombo();
	
	FillAvailableEmittersCombo();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CParticleFXEditorDlg::FillAvailableEmittersCombo()
{
	DynamicArray<String> files;
	m_lstAEmitters.ResetContent();

	GFileMan.FindFiles("../Data/Scripts/Emitters/*.ini",files,EFindFiles);	
	for (int i=0;i<files.Size();i++)
		m_lstAEmitters.AddString(String::StripExtension(files[i]).AsChar());
}


void CParticleFXEditorDlg::OnBnClickedAdd()
{
	int i = m_lstAEmitters.GetCurSel();
	if (i>-1)
	{		
		bool added = true;
		CString txt;
		m_lstAEmitters.GetText(i,txt);
		if (!Effect)
		{
#ifdef TRANSGAMING
			Effect = (ParticleEffect*) GEngine->pcfWorld->CreateEntity(Entity::ETypeParticleFx,txt,"Effect_TEST",1);
#else
			Effect = (ParticleEffect*) GEngine->World->CreateEntity(Entity::ETypeParticleFx,txt,"Effect_TEST",1);
#endif
			Effect->IsImmortal = true;
			Effect->Flags &= ~Entity::EEntDestroyOnReload;
			m_Preview = true;
		}
		else
		{
			int n = Effect->AddEmitter(GEngine->Video->ParticleSys.CreateEmitter((LPCSTR)(txt+".ini")));
			if (n==-1) added = false;
		}
		
		Vector cv = GEngine->Video->GetActiveView()->mCamera.GetForwardVector();
        Vector cp = GEngine->Video->GetActiveView()->mCamera.GetPosition();

		if (FXPos.X == -99999)
			FXPos.Set(cp.X+cv.X*7,cp.Y+cv.Y*7,cp.Z+cv.Z*7);
		Effect->SetPosition(FXPos);

#ifdef TRANSGAMING
		GEngine->pcfWorld->AddEntity(Effect);
#else
		GEngine->World->AddEntity(Effect);
#endif
		//Effect->RenderCleanup(GEngine->Video->RenderDev,GEngine->Video->MaterialSys);
		//Effect->RenderInitialize(GEngine->Video->RenderDev,GEngine->Video->MaterialSys);
		
		if (added)
		{
			int i = m_lstSEmitters.AddString(txt);				
			m_lstSEmitters.SetCurSel(i);
			OnLbnSelchangeSemitters();
		}
		else
			MessageBox("Max. 16 emitters in Effect");

		//m_Preview = false;
		//OnBnClickedPreview();
	}
}

void CParticleFXEditorDlg::OnLbnDblclkAemitters()
{
	OnBnClickedAdd();
}



void CParticleFXEditorDlg::OnLbnSelchangeSemitters()
{

	ParticleEffect::EmitterDef * pe = GetSelectedEmitter();
	if (pe)
	{
		m_PosX = pe->Pos.X;
		m_PosY = pe->Pos.Y;
		m_PosZ = pe->Pos.Z;
		m_AngX = pe->Rotation.X;
		m_AngY = pe->Rotation.Y;
		m_AngZ = pe->Rotation.Z;
		m_Scale = pe->Scale;
		
		m_sldScale.SetRange(0,50);
		m_sldScale.SetTicFreq(2);
		m_sldScale.SetPos(pe->Scale*10.0f);
	}
	UpdateData(false);
}


void CParticleFXEditorDlg::SetParam(float &val,float delta)
{
	ParticleEffect::EmitterDef * pe = GetSelectedEmitter();
	if (pe)
	{
		val = val + delta;
		pe->SetupTransform();
	}
	OnLbnSelchangeSemitters();
}

ParticleEffect::EmitterDef *CParticleFXEditorDlg::GetSelectedEmitter()
{
	if (!Effect) return NULL;
	int i = m_lstSEmitters.GetCurSel();
	if (i>-1)
		return &Effect->Emitters[i];	
	return NULL;
}

void CParticleFXEditorDlg::OnBnClickedDecX()
{
	ParticleEffect::EmitterDef * pe = GetSelectedEmitter();
	if (pe)
		SetParam(pe->Pos.X,-0.1f);
}

void CParticleFXEditorDlg::OnBnClickedIncX()
{
	ParticleEffect::EmitterDef * pe = GetSelectedEmitter();
	if (pe)
		SetParam(pe->Pos.X,0.1f);
}

void CParticleFXEditorDlg::OnBnClickedDecY()
{
	ParticleEffect::EmitterDef * pe = GetSelectedEmitter();
	if (pe)
		SetParam(pe->Pos.Y,-0.1f);
}

void CParticleFXEditorDlg::OnBnClickedIncY()
{
	ParticleEffect::EmitterDef * pe = GetSelectedEmitter();
	if (pe)
		SetParam(pe->Pos.Y,0.1f);
}

void CParticleFXEditorDlg::OnBnClickedDecZ()
{
	ParticleEffect::EmitterDef * pe = GetSelectedEmitter();
	if (pe)
		SetParam(pe->Pos.Z,-0.1f);
}

void CParticleFXEditorDlg::OnBnClickedIncZ()
{
	ParticleEffect::EmitterDef * pe = GetSelectedEmitter();
	if (pe)
		SetParam(pe->Pos.Z,0.1f); 
}



void CParticleFXEditorDlg::FillEffectsCombo()
{
	DynamicArray<String> files;

	m_cmbEffects.ResetContent();

	GFileMan.FindFiles("../Data/Scripts/Effects/*.pfx",files,EFindFiles);	

	for (int i=0;i<files.Size();i++)
		m_cmbEffects.AddString(String::StripExtension(files[i]).AsChar());
}



void CParticleFXEditorDlg::OnBnClickedRestart()
{
	if (!Effect) return;

#ifdef TRANSGAMING
	GEngine->pcfWorld->AddEntity(Effect);
#else
	GEngine->World->AddEntity(Effect);
#endif
	Effect->Restart();
}

void CParticleFXEditorDlg::OnBnClickedPreview()
{
	if (!Effect) return;

	m_Preview = !m_Preview;

	if (m_Preview)
		Effect->Flags &= ~Entity::EEntNoDraw;
	else
		Effect->Flags |= Entity::EEntNoDraw;

	UpdateData(false);
	OnBnClickedRestart();
}

void CParticleFXEditorDlg::OnCbnSelchangeEffects()
{
	m_lstSEmitters.ResetContent();
	if (Effect)
	{
#ifdef TRANSGAMING
		GEngine->pcfWorld->DeleteEntity(Effect);
#else
		GEngine->World->DeleteEntity(Effect);
#endif
		Effect = NULL;
	}

	int i = m_cmbEffects.GetCurSel();
	if (i>=0)
	{
#ifdef TRANSGAMING
		Effect = (ParticleEffect*) GEngine->pcfWorld->CreateEntity(Entity::ETypeParticleFx,"","Effect_TEST",1);
#else
		Effect = (ParticleEffect*) GEngine->World->CreateEntity(Entity::ETypeParticleFx,"","Effect_TEST",1);
#endif
		Effect->IsImmortal = true;
		Effect->Flags &= ~Entity::EEntDestroyOnReload;

		CString txt;
		m_cmbEffects.GetLBText(i,txt);
		GScript.DoString("ParticleFXArray['%s'] = nil",(LPCSTR)txt);
		GScript.Call("LoadParticleFX",0,"%e,%s",Effect,(LPCSTR)txt);

        Vector cv = GEngine->Video->GetActiveView()->mCamera.GetForwardVector();
		Vector cp = GEngine->Video->GetActiveView()->mCamera.GetPosition();
		if (FXPos.X == -99999)
			FXPos.Set(cp.X+cv.X*7,cp.Y+cv.Y*7,cp.Z+cv.Z*7);
		Effect->SetPosition(FXPos);		
#ifdef TRANSGAMING
		GEngine->pcfWorld->AddEntity(Effect);
#else
		GEngine->World->AddEntity(Effect);
#endif
		Effect->RenderCleanup(GEngine->Video->RenderDev,GEngine->Video->MaterialSys);
		Effect->RenderInitialize(GEngine->Video->RenderDev,GEngine->Video->MaterialSys);
		
		for (int i=0;i<Effect->EmitterCount;i++)
			m_lstSEmitters.AddString(String::StripExtension(Effect->Emitters[i].Emitter->Name).AsChar());

		FixedTransform = Effect->FixedTransform;
	}

	m_Preview = false;
	OnBnClickedPreview();
    OnBnClickedFronttocam();
	AfxGetMainWnd()->SetFocus();
}

void CParticleFXEditorDlg::OnBnClickedRemove()
{
	if (!Effect) return;
	int i = m_lstSEmitters.GetCurSel();
	if (i>-1)
	{
		Effect->RemoveEmitter(i);
		m_lstSEmitters.DeleteString(i);
		if (i >= m_lstSEmitters.GetCount())
			i = m_lstSEmitters.GetCount() - 1;
		m_lstSEmitters.SetCurSel(i);
	}
}


void CParticleFXEditorDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CScrolledInnerDlg::OnHScroll(nSBCode, nPos, pScrollBar);

	ParticleEffect::EmitterDef * pe = GetSelectedEmitter();
	if (pe)
	{
		if ((CSliderCtrl *)pScrollBar ==  &m_sldScale)
			pe->Scale = m_sldScale.GetPos()/10.f;

		pe->SetupTransform();
		OnLbnSelchangeSemitters();
	}

}

void CParticleFXEditorDlg::OnBnClickedDecAx()
{
	ParticleEffect::EmitterDef * pe = GetSelectedEmitter();
	if (pe)
	{
		SetParam(pe->Rotation.X,-0.1f);
		pe->SetupTransform();
	}
}

void CParticleFXEditorDlg::OnBnClickedIncAx()
{
	ParticleEffect::EmitterDef * pe = GetSelectedEmitter();
	if (pe)
	{
		SetParam(pe->Rotation.X,0.1f);
		pe->SetupTransform();
	}
}

void CParticleFXEditorDlg::OnBnClickedDecAy()
{
	ParticleEffect::EmitterDef * pe = GetSelectedEmitter();
	if (pe)
	{
		SetParam(pe->Rotation.Y,-0.1f);
		pe->SetupTransform();
	}
}

void CParticleFXEditorDlg::OnBnClickedIncAy()
{
	ParticleEffect::EmitterDef * pe = GetSelectedEmitter();
	if (pe)
	{
		SetParam(pe->Rotation.Y,0.1f);
		pe->SetupTransform();
	}
}

void CParticleFXEditorDlg::OnBnClickedDecAz()
{
	ParticleEffect::EmitterDef * pe = GetSelectedEmitter();
	if (pe)
	{
		SetParam(pe->Rotation.Z,-0.1f);
		pe->SetupTransform();
	}
}

void CParticleFXEditorDlg::OnBnClickedIncAz()
{
	ParticleEffect::EmitterDef * pe = GetSelectedEmitter();
	if (pe)
	{
		SetParam(pe->Rotation.Z,0.1f);
		pe->SetupTransform();
	}
}

void CParticleFXEditorDlg::OnEnChangeScale()
{
	UpdateData(true);
	ParticleEffect::EmitterDef * pe = GetSelectedEmitter();
	if (pe)
	{
		pe->Scale = m_Scale;
		pe->SetupTransform();
		m_sldScale.SetPos(pe->Scale*10.0f);
	}
}

void CParticleFXEditorDlg::OnEnChangePosx()
{
	UpdateData(true);
	ParticleEffect::EmitterDef * pe = GetSelectedEmitter();
	if (pe)
	{
		pe->Pos.X = m_PosX;
		pe->SetupTransform();
	}
}

void CParticleFXEditorDlg::OnEnChangePosy()
{
	UpdateData(true);
	ParticleEffect::EmitterDef * pe = GetSelectedEmitter();
	if (pe)
	{
		pe->Pos.Y = m_PosY;
		pe->SetupTransform();
	}
}

void CParticleFXEditorDlg::OnEnChangePosz()
{
	UpdateData(true);
	ParticleEffect::EmitterDef * pe = GetSelectedEmitter();
	if (pe)
	{
		pe->Pos.Z = m_PosZ;
		pe->SetupTransform();
	}
}


void CParticleFXEditorDlg::OnEnChangeAngx()
{
	UpdateData(true);
	ParticleEffect::EmitterDef * pe = GetSelectedEmitter();
	if (pe)
	{
		pe->Rotation.X = m_AngX;
		pe->SetupTransform();
	}
}

void CParticleFXEditorDlg::OnEnChangeAngy()
{
	UpdateData(true);
	ParticleEffect::EmitterDef * pe = GetSelectedEmitter();
	if (pe)
	{
		pe->Rotation.Y = m_AngY;
		pe->SetupTransform();
	}
}

void CParticleFXEditorDlg::OnEnChangeAngz()
{
	UpdateData(true);
	ParticleEffect::EmitterDef * pe = GetSelectedEmitter();
	if (pe)
	{
		pe->Rotation.Z = m_AngZ;
		pe->SetupTransform();
	}
}

void CParticleFXEditorDlg::OnBnClickedEdit()
{
	CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
	pWnd->m_wndEmitterDlg.ShowWindow(SW_SHOW);

	int i = m_lstAEmitters.GetCurSel();
	if (i>-1)
	{
		CString name;
		m_lstAEmitters.GetText(i,name);
		EditEmitter(name);
	};
	
}

void CParticleFXEditorDlg::OnBnClickedSave()
{
	if (!Effect) return;

	CString txt;
	m_cmbEffects.GetWindowText(txt);
	txt.Trim();
	if (txt.GetLength()>1)
	{
		FILE *f = fopen("../Data/Scripts/Effects/"+txt+".pfx","wt");
		if (f)
		{
			fprintf(f,"ParticleFX =\n{\n    Emitters =\n    {");
			for (int i=0;i<Effect->EmitterCount;i++)
			{
				ParticleEffect::EmitterDef *pe = &Effect->Emitters[i];
				fprintf(f,"\n        {\n");
				fprintf(f,"            File = \"%s\",\n",pe->Emitter->Name.AsChar());
				fprintf(f,"            Scale = %.2f,\n",pe->Scale);
				fprintf(f,"            Position = {%.2f,%.2f,%.2f},\n",pe->Pos.X,pe->Pos.Y,pe->Pos.Z);
				fprintf(f,"            Rotation = {%.2f,%.2f,%.2f},\n",pe->Rotation.X,pe->Rotation.Y,pe->Rotation.Z);
				fprintf(f,"        },\n");
			}
			//fprintf(f,"    }\n}\n");
			fprintf(f,"    },\n");
			fprintf(f,"    FixedTransform = %s\n}\n", (Effect->FixedTransform ? "true" : "false"));
			fclose(f);
		}
		else
		{
			MessageBox("Cannot save emitter!!! ReadOnly?");
			return;
		}

		FillEffectsCombo();
		m_cmbEffects.SetWindowText(txt);
	}
	else
		MessageBox("Invalid file name!!!");

}

void CParticleFXEditorDlg::OnLbnDblclkSemitters()
{
	int i = m_lstSEmitters.GetCurSel();
	if (i>-1)
	{
		CString name;
		m_lstSEmitters.GetText(i,name);
		EditEmitter(name);
	};
}

void CParticleFXEditorDlg::EditEmitter(CString name)
{
	CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
	CEmitterEditorDlg *dlg = (CEmitterEditorDlg *) pWnd->m_wndEmitterDlg.m_InnerDlg;
	dlg->ShowWindow(SW_SHOW);
	int i = dlg->m_cmbEmitters.FindString(-1,name);
	dlg->m_cmbEmitters.SetCurSel(i);
	dlg->LoadEmitter(false,false);
}

void CParticleFXEditorDlg::OnBnClickedFronttocam()
{
	if (!Effect)
		return;

    Vector cv = GEngine->Video->GetActiveView()->mCamera.GetForwardVector();
	Vector cp = GEngine->Video->GetActiveView()->mCamera.GetPosition();

	FXPos.Set(cp.X+cv.X*7,cp.Y+cv.Y*7,cp.Z+cv.Z*7);
	Effect->SetPosition(FXPos);

	CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
	CEmitterEditorDlg *dlg = (CEmitterEditorDlg *) pWnd->m_wndEmitterDlg.m_InnerDlg;
	if (dlg->PFX)
		dlg->PFX->SetPosition(FXPos);

	OnBnClickedRestart();
}
void CParticleFXEditorDlg::OnBnClickedSaveallemitters()
{
	if (!Effect)
		return;

	for (int i=0; i<Effect->EmitterCount; ++i)
	{
#ifdef TRANSGAMING
		char fname[64];
		snprintf(fname, 64, "../Data/Scripts/Emitters/%s", Effect->Emitters[i].Emitter->Master->Name);
		if (!GEngine->Video->ParticleSys.SaveEmitter(*Effect->Emitters[i].Emitter->Master,fname))
			MessageBox(fname,"Cannot save emitter!!! ReadOnly?");
#else
		String fname = String("../Data/Scripts/Emitters/") + Effect->Emitters[i].Emitter->Master->Name;
		if (!GEngine->Video->ParticleSys.SaveEmitter(*Effect->Emitters[i].Emitter->Master,fname))
			MessageBox(fname.AsChar(),"Cannot save emitter!!! ReadOnly?");
#endif

	}
}

#endif
void CParticleFXEditorDlg::OnBnClickedFixedtransform()
{
	if (!Effect)
		return;

	FixedTransform = !FixedTransform;
	Effect->FixedTransform = FixedTransform ? true : false;
	Effect->Restart();
}
