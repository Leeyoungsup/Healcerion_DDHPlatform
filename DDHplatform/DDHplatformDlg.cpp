
// DDHplatformDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "DDHplatform.h"
#include "DDHplatformDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDDHplatformDlg 대화 상자



CDDHplatformDlg::CDDHplatformDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DDHPLATFORM_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDDHplatformDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_GRAF_SEG, m_comboGrefSeg);
	DDX_Control(pDX, IDC_SLIDER_SEG_THRESHOLD, m_ThresholdSlide);
	DDX_Control(pDX, IDC_EDIT_ALPHA, m_editAlpha);
	DDX_Control(pDX, IDC_EDIT_BETA, m_editBeta);
}

BEGIN_MESSAGE_MAP(CDDHplatformDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_IMGLOAD, &CDDHplatformDlg::OnBnClickedBtnImgload)
	ON_BN_CLICKED(IDC_BTN_IMGPRE, &CDDHplatformDlg::OnBnClickedBtnImgpre)
	ON_BN_CLICKED(IDC_BTN_STD_PLANE_AI, &CDDHplatformDlg::OnBnClickedBtnStdPlaneAi)
	ON_BN_CLICKED(IDC_BTN_GRAF_SEG, &CDDHplatformDlg::OnBnClickedBtnGrafSeg)
	ON_BN_CLICKED(IDC_BTN_GRAF_ALGORITHM, &CDDHplatformDlg::OnBnClickedBtnGrafAlgorithm)
	ON_CBN_SELCHANGE(IDC_COMBO_GRAF_SEG, &CDDHplatformDlg::OnCbnSelchangeComboGrafSeg)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_SEG_THRESHOLD, &CDDHplatformDlg::OnNMCustomdrawSliderSegThreshold)
END_MESSAGE_MAP()


// CDDHplatformDlg 메시지 처리기

BOOL CDDHplatformDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	SetDlgItemText(IDC_EDIT_STDPLANE_PROB, L"0.000");
	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.
	m_pTFLiteAI.init();
	m_bSegStatus = FALSE;
	m_comboGrefSeg.AddString(_T("lower limb"));
	m_comboGrefSeg.AddString(_T("bony rim"));
	m_comboGrefSeg.AddString(_T("labrum"));
	m_comboGrefSeg.AddString(_T("ilium"));
	m_comboGrefSeg.AddString(_T("Total"));
	m_comboGrefSeg.SetCurSel(0);

	m_ThresholdSlide.SetRange(0, 1000);
	m_ThresholdSlide.SetPos(500);
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CDDHplatformDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CDDHplatformDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CDDHplatformDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDDHplatformDlg::CreateBitmapInfo(int w, int h, int bpp)
{
	if (m_pBitmapInfo != NULL)
	{
		delete m_pBitmapInfo;
		m_pBitmapInfo = NULL;
	}

	if (bpp == 8)
		m_pBitmapInfo = (BITMAPINFO *) new BYTE[sizeof(BITMAPINFO) + 255 * sizeof(RGBQUAD)];
	else // 24 or 32bit
		m_pBitmapInfo = (BITMAPINFO *) new BYTE[sizeof(BITMAPINFO)];

	m_pBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_pBitmapInfo->bmiHeader.biPlanes = 1;
	m_pBitmapInfo->bmiHeader.biBitCount = bpp;
	m_pBitmapInfo->bmiHeader.biCompression = BI_RGB;
	m_pBitmapInfo->bmiHeader.biSizeImage = 0;
	m_pBitmapInfo->bmiHeader.biXPelsPerMeter = 0;
	m_pBitmapInfo->bmiHeader.biYPelsPerMeter = 0;
	m_pBitmapInfo->bmiHeader.biClrUsed = 0;
	m_pBitmapInfo->bmiHeader.biClrImportant = 0;

	if (bpp == 8)
	{
		for (int i = 0; i < 256; i++)
		{
			m_pBitmapInfo->bmiColors[i].rgbBlue = (BYTE)i;
			m_pBitmapInfo->bmiColors[i].rgbGreen = (BYTE)i;
			m_pBitmapInfo->bmiColors[i].rgbRed = (BYTE)i;
			m_pBitmapInfo->bmiColors[i].rgbReserved = 0;
		}
	}

	m_pBitmapInfo->bmiHeader.biWidth = w;
	m_pBitmapInfo->bmiHeader.biHeight = -h;
}

void CDDHplatformDlg::DrawImage(int nDevice, Mat matImage)
{
	CClientDC dc(GetDlgItem(nDevice));

	CRect rect;
	GetDlgItem(nDevice)->GetClientRect(&rect);

	SetStretchBltMode(dc.GetSafeHdc(), COLORONCOLOR);
	StretchDIBits(dc.GetSafeHdc(), 0, 0, rect.Width(), rect.Height(), 0, 0, matImage.cols, matImage.rows, matImage.data, m_pBitmapInfo, DIB_RGB_COLORS, SRCCOPY);
}

void CDDHplatformDlg::OnBnClickedBtnImgload()
{
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_READONLY, _T("image file(*.jpg;*.bmp;*.png;)|*.jpg;*.bmp;*.png;|All Files(*.*)|*.*||"));
	if (fileDlg.DoModal() == IDOK)
	{
		CString path = fileDlg.GetPathName();
		CEdit *p = (CEdit *)GetDlgItem(IDC_EDIT_FILEPATH);
		p->SetWindowText(path); 
		SetDlgItemText(IDC_EDIT_DATA, L"cv::Mat");
		
		CT2CA pszString(path);
		std::string strPath(pszString);
		
		m_matImage = imread(strPath, IMREAD_UNCHANGED);
		CString strMatSize;
		strMatSize.Format(_T("%d X %d X %d"), m_matImage.cols, m_matImage.rows, m_matImage.channels());
		SetDlgItemText(IDC_EDIT_MAT_SIZE, strMatSize);
		CreateBitmapInfo(m_matImage.cols, m_matImage.rows, m_matImage.channels() * 8);
		
		DrawImage(IDC_PC_RAW_VIEW, m_matImage);
	}
}

void CDDHplatformDlg::OnBnClickedBtnImgpre()
{
	m_matPreprocessingImage = m_matImage.clone();
	// preprocessing
	SetDlgItemText(IDC_EDIT_DATA2, L"cv::Mat");
	m_matPreprocessingImage=m_pPreprocessing->CreateZeroPadding(m_matPreprocessingImage);
	m_matPreprocessingImage = m_pPreprocessing->ResizeImage(m_matPreprocessingImage);
	CreateBitmapInfo(m_matPreprocessingImage.cols, m_matPreprocessingImage.rows, m_matPreprocessingImage.channels() * 8);
	CString strMatSize;
	strMatSize.Format(_T("%d X %d X %d"), m_matPreprocessingImage.cols, m_matPreprocessingImage.rows, m_matPreprocessingImage.channels());
	SetDlgItemText(IDC_EDIT_MAT_SIZE2, strMatSize);
	DrawImage(IDC_PC_PRE_VIEW, m_matPreprocessingImage);
}


void CDDHplatformDlg::OnBnClickedBtnStdPlaneAi()
{
	double dPredictStdPlane = m_pTFLiteAI.StandardPlaneClassification(m_matPreprocessingImage);

	CString strPredict;
	strPredict.Format(_T("%f"), dPredictStdPlane);
	SetDlgItemText(IDC_EDIT_STDPLANE_PROB, strPredict);
	
}


void CDDHplatformDlg::OnBnClickedBtnGrafSeg()
{
	m_pTFLiteAI.GrafSegmentation(m_matPreprocessingImage,m_matfOutputIlium, m_matfOutputLabrum,m_matfOutputBonyRim, m_matfOutputLowerLimb);
	m_bSegStatus = TRUE;
	m_ThresholdSlide.SetPos(500);
	int nComboIndex= m_comboGrefSeg.GetCurSel();
	int nIndexGraf;
	nIndexGraf = m_comboGrefSeg.GetCurSel();
	Mat matPredictStdPlane;
	cv::compare(m_matfOutputIlium, 0.5, m_matOutputIlium, cv::CMP_GT);
	cv::compare(m_matfOutputLabrum, 0.5, m_matOutputLabrum, cv::CMP_GT);
	cv::compare(m_matfOutputBonyRim, 0.5, m_matOutputBonyRim, cv::CMP_GT);
	cv::compare(m_matfOutputLowerLimb, 0.5, m_matOutputLowerLimb, cv::CMP_GT);
	switch (nIndexGraf)
	{
	case 0:
		matPredictStdPlane = m_matOutputBonyRim.clone();
		break;
	case 1:
		matPredictStdPlane = m_matOutputIlium.clone();
		break;
	case 2:
		matPredictStdPlane = m_matOutputLabrum.clone();
		break;
	case 3:
		matPredictStdPlane = m_matOutputLowerLimb.clone();
		break;
	}
	std::vector<cv::Mat> planes; // 3개 영상의 벡터 생성
	Mat result;
	if(nIndexGraf!=4)
	{
	
		cv::split(m_matPreprocessingImage, planes); // 3채널인 단일 영상을 3개의 1채널 영상으로 분리

		planes[1] += matPredictStdPlane; // 초록 채널에 더하기

		cv::merge(planes, result); // 3개의 1채널 영상을 3채널인 단일 영상으로 병합
	}
	else
	{
		cv::split(m_matPreprocessingImage, planes); // 3채널인 단일 영상을 3개의 1채널 영상으로 분리

		planes[0] += m_matOutputBonyRim;
		planes[1] += m_matOutputIlium;
		planes[2] += m_matOutputLabrum;
		planes[2] += m_matOutputLowerLimb;
		planes[1] += m_matOutputLowerLimb;

		cv::merge(planes, result); // 3개의 1채널 영상을 3채널인 단일 영상으로 병합
	}
	CreateBitmapInfo(result.cols, result.rows, result.channels() * 8);
	DrawImage(IDC_PC_VIEW3, result);

}

void CDDHplatformDlg::OnCbnSelchangeComboGrafSeg()
{
	if (m_bSegStatus)
	{
		int nComboIndex = m_comboGrefSeg.GetCurSel();
		int nIndexGraf;
		nIndexGraf = m_comboGrefSeg.GetCurSel();
		Mat matPredictStdPlane;
		switch (nIndexGraf)
		{
		case 0:
			matPredictStdPlane = m_matOutputBonyRim.clone();
			break;
		case 1:
			matPredictStdPlane = m_matOutputIlium.clone();
			break;
		case 2:
			matPredictStdPlane = m_matOutputLabrum.clone();
			break;
		case 3:
			matPredictStdPlane = m_matOutputLowerLimb.clone();
			break;
		}
		std::vector<cv::Mat> planes; // 3개 영상의 벡터 생성
		Mat result;
		if (nIndexGraf != 4)
		{

			cv::split(m_matPreprocessingImage, planes); // 3채널인 단일 영상을 3개의 1채널 영상으로 분리

			planes[1] += matPredictStdPlane; // 초록 채널에 더하기

			cv::merge(planes, result); // 3개의 1채널 영상을 3채널인 단일 영상으로 병합
		}
		else
		{
			cv::split(m_matPreprocessingImage, planes); // 3채널인 단일 영상을 3개의 1채널 영상으로 분리

			planes[0] += m_matOutputBonyRim;
			planes[1] += m_matOutputIlium;
			planes[2] += m_matOutputLabrum;
			planes[2] += m_matOutputLowerLimb;
			planes[1] += m_matOutputLowerLimb;

			cv::merge(planes, result); // 3개의 1채널 영상을 3채널인 단일 영상으로 병합
		}
		CreateBitmapInfo(result.cols, result.rows, result.channels() * 8);
		DrawImage(IDC_PC_VIEW3, result);
	}
}


void CDDHplatformDlg::OnBnClickedBtnGrafAlgorithm()
{
	
	Mat matDstImage=m_pPreprocessing->GrafAlgorithm(m_matPreprocessingImage,m_matOutputIlium, m_matOutputLabrum, m_matOutputBonyRim, m_matOutputLowerLimb, m_pcaBaseLine, m_dAlphaAngle, m_dBetaAngle);
	CreateBitmapInfo(matDstImage.cols, matDstImage.rows, matDstImage.channels() * 8);
	DrawImage(IDC_PC_VIEW4, matDstImage);
	CString strPredict;
	strPredict.Format(_T("%f"), m_dAlphaAngle);
	SetDlgItemText(IDC_EDIT_ALPHA, strPredict);
	strPredict.Format(_T("%f"), m_dBetaAngle);
	SetDlgItemText(IDC_EDIT_BETA, strPredict);
}




void CDDHplatformDlg::OnNMCustomdrawSliderSegThreshold(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	*pResult = 0;
	int nPosition = m_ThresholdSlide.GetPos();
	double dPosition = (double)nPosition / 1000.0;
	CString strPostion;
	strPostion.Format(_T("%.3f"), dPosition);
	SetDlgItemText(IDC_EDIT_THRESH, strPostion);
	if (m_bSegStatus)
	{
		cv::compare(m_matfOutputIlium, dPosition, m_matOutputIlium, cv::CMP_GT);
		cv::compare(m_matfOutputLabrum, dPosition, m_matOutputLabrum, cv::CMP_GT);
		cv::compare(m_matfOutputBonyRim, dPosition, m_matOutputBonyRim, cv::CMP_GT);
		cv::compare(m_matfOutputLowerLimb, dPosition, m_matOutputLowerLimb, cv::CMP_GT);
		int nComboIndex = m_comboGrefSeg.GetCurSel();
		int nIndexGraf;
		nIndexGraf = m_comboGrefSeg.GetCurSel();
		Mat matPredictStdPlane;
		switch (nIndexGraf)
		{
		case 0:
			matPredictStdPlane = m_matOutputBonyRim.clone();
			break;
		case 1:
			matPredictStdPlane = m_matOutputIlium.clone();
			break;
		case 2:
			matPredictStdPlane = m_matOutputLabrum.clone();
			break;
		case 3:
			matPredictStdPlane = m_matOutputLowerLimb.clone();
			break;
		}
		std::vector<cv::Mat> planes; // 3개 영상의 벡터 생성
		Mat result;
		if (nIndexGraf != 4)
		{

			cv::split(m_matPreprocessingImage, planes); // 3채널인 단일 영상을 3개의 1채널 영상으로 분리

			planes[1] += matPredictStdPlane; // 초록 채널에 더하기

			cv::merge(planes, result); // 3개의 1채널 영상을 3채널인 단일 영상으로 병합
		}
		else
		{
			cv::split(m_matPreprocessingImage, planes); // 3채널인 단일 영상을 3개의 1채널 영상으로 분리

			planes[0] += m_matOutputBonyRim;
			planes[1] += m_matOutputIlium;
			planes[2] += m_matOutputLabrum;
			planes[2] += m_matOutputLowerLimb;
			planes[1] += m_matOutputLowerLimb;

			cv::merge(planes, result); // 3개의 1채널 영상을 3채널인 단일 영상으로 병합
		}
		CreateBitmapInfo(result.cols, result.rows, result.channels() * 8);
		DrawImage(IDC_PC_VIEW3, result);
	}
}
