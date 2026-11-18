#pragma once

class ParticleEmitter;
class ParticleEffect;

#include "ScrolledInnerDlg.h"
#include "afxwin.h"
#include "afxcmn.h"

#ifdef EDITOR

// CEmitterEditorDlg dialog

class CEmitterEditorDlg : public CScrolledInnerDlg
{
	DECLARE_DYNAMIC(CEmitterEditorDlg)

public:
	ParticleEffect *PFX;
	ParticleEmitter *PE;
	CRangeSlider m_rngLifeTime;
	CRangeSlider m_rngPosX;
	CRangeSlider m_rngPosY;
	CRangeSlider m_rngPosZ;
	CRangeSlider m_rngRot;
	CRangeSlider m_rngVelX;
	CRangeSlider m_rngVelY;
	CRangeSlider m_rngVelZ;
	CRangeSlider m_rngVelEndX;
	CRangeSlider m_rngVelEndY;
	CRangeSlider m_rngVelEndZ;
	CRangeSlider m_rngAccelX;
	CRangeSlider m_rngAccelY;
	CRangeSlider m_rngAccelZ;
	CRangeSlider m_rngSizeStart;
	CRangeSlider m_rngSizeEnd;
	CRangeSlider m_rngLength;
	CRangeSlider m_rngThickness;
	CRangeSlider m_rngFade;
	CRangeSlider m_rngVelBlend;


	CEmitterEditorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEmitterEditorDlg();

// Dialog Data
	enum { IDD = IDD_EMITTEREDITOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();
	void UpdateView();

	DECLARE_MESSAGE_MAP()
public:
	void FillEmittersCombo();
	afx_msg void OnBnClickedSave();
	CComboBox m_cmbEmitters;
	BOOL m_Evolve;
	BOOL m_Warp;
	UINT m_MaxParticles;
	UINT m_SpawnRate;
	CSliderCtrl m_sldMaxParticles;
	CSliderCtrl m_sldSpawnRate;
	float m_LifeTimeMin;
	float m_LifeTimeMax;
	BOOL m_DepthTest;
	BOOL m_UseRandomNormal;
	BOOL m_Preview;
	afx_msg void OnBnClickedPreview();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEnChangeMaxparticlesEdit();
	afx_msg void OnEnChangeSpawnrateEdit();
	afx_msg void OnBnClickedEvolve();
	afx_msg void OnCbnSelchangeEmitters();
	afx_msg void LoadEmitter(bool preview,bool reload);
	afx_msg LRESULT OnRangeChange(WPARAM wParam, LPARAM  lParam );
	afx_msg void OnEnChangeLifetimeMinEdit();
	afx_msg void OnEnChangeLifetimeMaxEdit();
	CComboBox m_cmbTypes;
	CComboBox m_cmbBlendModes;
	CComboBox m_cmbMaterials;
	afx_msg void OnCbnSelchangeTypes();
	afx_msg void OnCbnSelchangeBlendmodes();
	afx_msg void OnCbnSelchangeMaterials();
	afx_msg void OnBnClickedDepthtest();
	afx_msg void OnBnClickedUserandomnormal();
	CXTColorPicker m_picColorStart;
	CXTColorPicker m_picColorEnd;

	afx_msg void OnSelEndColorStart();
	afx_msg void OnSelEndColorEnd();
	BYTE m_AlphaStart;
	BYTE m_AlphaMid;
	BYTE m_AlphaEnd;
	CSliderCtrl m_sldAlphaStart;
	CSliderCtrl m_sldAlphaMid;
	CSliderCtrl m_sldAlphaEnd;
	BOOL m_ColorInterpolation;
	afx_msg void OnBnClickedColorinterpolation();
	afx_msg void OnEnChangeAlphastartEdit();
	afx_msg void OnEnChangeAlphaendEdit();
	CString m_Texture;
	CString m_WarpTexture;
	afx_msg void OnBnClickedTexbrowser();
	afx_msg void OnBnClickedWarpTexbrowser();
	CSliderCtrl m_sldTexFPS;
	BYTE m_TexFPS;
	afx_msg void OnEnChangeFpsEdit();
	void EditToRange(CRangeSlider *pRS,float &cmin,float &cmax,float &vmin,float &vmax);

	
	float m_PosXMin;
	float m_PosXMax;
	float m_PosYMin;
	float m_PosYMax;
	float m_PosZMin;
	float m_PosZMax;

	float m_RotMin;
	float m_RotMax;

	float m_VelXMin;
	float m_VelXMax;
	float m_VelYMin;
	float m_VelYMax;
	float m_VelZMin;
	float m_VelZMax;

	float m_VelEndXMin;
	float m_VelEndXMax;
	float m_VelEndYMin;
	float m_VelEndYMax;
	float m_VelEndZMin;
	float m_VelEndZMax;

	float m_AccelXMin;
	float m_AccelXMax;
	float m_AccelYMin;
	float m_AccelYMax;
	float m_AccelZMin;
	float m_AccelZMax;
	float m_SizeStartMin;
	float m_SizeStartMax;
	float m_SizeEndMin;
	float m_SizeEndMax;
	float m_LengthMin;
	float m_LengthMax;
	float m_ThicknessMin;
	float m_ThicknessMax;
	afx_msg void OnEnChangePosxMinEdit();
	afx_msg void OnEnChangePosxMaxEdit();
	afx_msg void OnEnChangePosyMinEdit();
	afx_msg void OnEnChangePosyMaxEdit();
	afx_msg void OnEnChangePoszMinEdit();
	afx_msg void OnEnChangePoszMaxEdit();
	afx_msg void OnBnClickedRestart();
	afx_msg void OnEnChangeRotMaxEdit();
	afx_msg void OnEnChangeRotMinEdit();
	afx_msg void OnEnChangeVelxMaxEdit();
	afx_msg void OnEnChangeVelxMinEdit();
	afx_msg void OnEnChangeVelyMinEdit();
	afx_msg void OnEnChangeVelyMaxEdit();
	afx_msg void OnEnChangeVelzMinEdit();
	afx_msg void OnEnChangeVelzMaxEdit();
	
	afx_msg void OnEnChangeVelEndxMaxEdit();
	afx_msg void OnEnChangeVelEndxMinEdit();
	afx_msg void OnEnChangeVelEndyMinEdit();
	afx_msg void OnEnChangeVelEndyMaxEdit();
	afx_msg void OnEnChangeVelEndzMinEdit();
	afx_msg void OnEnChangeVelEndzMaxEdit();

	afx_msg void OnEnChangeAccelxMinEdit();
	afx_msg void OnEnChangeAccelxMaxEdit();
	afx_msg void OnEnChangeAccelyMinEdit();
	afx_msg void OnEnChangeAccelyMaxEdit();
	afx_msg void OnEnChangeAccelzMinEdit();
	afx_msg void OnEnChangeAccelzMaxEdit();
	afx_msg void OnEnChangeSizestartMinEdit();
	afx_msg void OnEnChangeSizestartMaxEdit();
	afx_msg void OnEnChangeSizeendMinEdit();
	afx_msg void OnEnChangeSizeendMaxEdit();
	afx_msg void OnEnChangeLengthMinEdit();
	afx_msg void OnEnChangeLengthMaxEdit();
	afx_msg void OnEnChangeThicknessMinEdit();
	afx_msg void OnEnChangeThicknessMaxEdit();
	afx_msg void OnEnChangeTextureEdit();
	afx_msg void OnEnChangeWarpTextureEdit();
	virtual void OnOK() {};
	virtual void OnCancel() {};

	afx_msg void OnBnClickedReload();
	afx_msg void OnBnClickedImmortal();
	BOOL m_Immortal;
	afx_msg void OnEnChangeAlphamidEdit();
	float m_FadeIn;
	float m_FadeOut;
	afx_msg void OnEnChangeFadeInEdit();
	afx_msg void OnEnChangeFadeOutEdit();
	afx_msg void OnBnClickedWarp();
	
	float m_VelBlendIn;
	float m_VelBlendOut;
	afx_msg void OnEnChangeVelBlendInEdit();
	afx_msg void OnEnChangeVelBlendOutEdit();
};


#endif