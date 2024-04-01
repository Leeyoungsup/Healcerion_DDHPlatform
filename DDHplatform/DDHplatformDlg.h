
// DDHplatformDlg.h: 헤더 파일
//

#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include "DDH.h"
using namespace cv;

// CDDHplatformDlg 대화 상자
class CDDHplatformDlg : public CDialogEx
{
// 생성입니다.
public:
	CDDHplatformDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DDHPLATFORM_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

public:
	Mat m_matImage; // 이미지 정보를 담고 있는 객체.
	Mat m_matPreprocessingImage; // preprocessing 이미지
	BITMAPINFO *m_pBitmapInfo; // Bitmap 정보를 담고 있는 구조체.
	Preprocessing *m_pPreprocessing;
	TfLite m_pTFLiteAI;
	cv::Mat m_matOutputIlium;
	cv::Mat m_matOutputLabrum;
	cv::Mat m_matOutputBonyRim;
	cv::Mat m_matOutputLowerLimb;
	cv::Mat m_matfOutputIlium;
	cv::Mat m_matfOutputLabrum;
	cv::Mat m_matfOutputBonyRim;
	cv::Mat m_matfOutputLowerLimb;
	PCA m_pcaBaseLine;
	bool m_bSegStatus;
	void CreateBitmapInfo(int w, int h, int bpp); // Bitmap 정보를 생성하는 함수.
	void DrawImage(int nDevice, Mat matImage); // 그리는 작업을 수행하는 함수.
	double m_dAlphaAngle;
	double m_dBetaAngle;
// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnImgload();
	afx_msg void OnBnClickedBtnImgpre();
	afx_msg void OnBnClickedBtnStdPlaneAi();
	afx_msg void OnBnClickedBtnGrafSeg();
	afx_msg void OnBnClickedBtnGrafAlgorithm();
	CComboBox m_comboGrefSeg;
	afx_msg void OnCbnSelchangeComboGrafSeg();
	CSliderCtrl m_ThresholdSlide;
	afx_msg void OnNMCustomdrawSliderSegThreshold(NMHDR* pNMHDR, LRESULT* pResult);
	CEdit m_editAlpha;
	CEdit m_editBeta;
};
