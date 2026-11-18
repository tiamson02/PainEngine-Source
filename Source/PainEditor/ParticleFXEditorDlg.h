#pragma once


#include "ScrolledInnerDlg.h"
#include "afxwin.h"
#include "afxcmn.h"

#include "Render/ParticleEffect.h"

// CParticleFXEditorDlg dialog

class CParticleFXEditorDlg : public CScrolledInnerDlg
{
	DECLARE_DYNAMIC(CParticleFXEditorDlg)

	ParticleEffect *Effect;

public:
	CParticleFXEditorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CParticleFXEditorDlg();

// Dialog Data
	enum { IDD = IDD_PARTICLEFXEDITOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();


	DECLARE_MESSAGE_MAP()
public:
	void EditEmitter(CString name);
	void FillEffectsCombo();
	void FillAvailableEmittersCombo();
	CListBox m_lstAEmitters;
	CListBox m_lstSEmitters;
	afx_msg void OnBnClickedAdd();
	afx_msg void OnLbnDblclkAemitters();
	afx_msg void OnLbnSelchangeSemitters();
	float m_PosX;
	float m_PosY;
	float m_PosZ;
	float m_AngX;
	float m_AngY;
	float m_AngZ;
	float m_Scale;
	afx_msg void OnBnClickedDecY();
	afx_msg void OnBnClickedIncY();
	afx_msg void OnBnClickedDecX();
	afx_msg void OnBnClickedIncX();
	afx_msg void OnBnClickedIncZ();
	afx_msg void OnBnClickedDecZ();

	CComboBox m_cmbEffects;
	afx_msg void OnBnClickedRestart();
	CSliderCtrl m_sldScale;
	BOOL m_Preview;
	afx_msg void OnBnClickedPreview();
	afx_msg void OnCbnSelchangeEffects();
	afx_msg void OnBnClickedRemove();
	void SetParam(float &val,float delta);
	ParticleEffect::EmitterDef *GetSelectedEmitter();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedDecAx();
	afx_msg void OnBnClickedIncAx();
	afx_msg void OnBnClickedIncAy();
	afx_msg void OnBnClickedDecAy();
	afx_msg void OnBnClickedDecAz();
	afx_msg void OnBnClickedIncAz();
	afx_msg void OnEnChangeScale();
	afx_msg void OnEnChangePosx();
	afx_msg void OnEnChangePosz();
	afx_msg void OnEnChangePosy();
	afx_msg void OnEnChangeAngx();
	afx_msg void OnEnChangeAngy();
	afx_msg void OnEnChangeAngz();
	afx_msg void OnBnClickedEdit();
	afx_msg void OnBnClickedSave();
	virtual void OnOK() {};
	virtual void OnCancel() {};
	afx_msg void OnLbnDblclkSemitters();
	afx_msg void OnBnClickedFronttocam();
	afx_msg void OnBnClickedSaveallemitters();
	afx_msg void OnBnClickedFixedtransform();
	BOOL FixedTransform;
};
