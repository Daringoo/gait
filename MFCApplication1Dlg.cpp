
// MFCApplication1Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCApplication1.h"
#include "MFCApplication1Dlg.h"
#include "afxdialogex.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCApplication1Dlg 对话框

CMFCApplication1Dlg::CMFCApplication1Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMFCApplication1Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	

}

void CMFCApplication1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	////自己添加
//	DDX_Control(pDX, IDC_EDIT1, m_studyGaitName);
	DDX_Control(pDX, VideoPathLoaded, m_editVideoPath);  //videopathloaded 视频路径显示对话框
	DDX_Control(pDX, IDC_EDIT2, m_editRecogName);  //最后识别结果与编辑框2绑定，传递变量为m_editRecogName
//	DDX_Control(pDX, IDC_CHECK1, m_btShowVideo);

}

BEGIN_MESSAGE_MAP(CMFCApplication1Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMFCApplication1Dlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CMFCApplication1Dlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON2, &CMFCApplication1Dlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_CHECK1, &CMFCApplication1Dlg::OnBnClickedCheck1)
//	ON_BN_CLICKED(IDC_BUTTON4, &CMFCApplication1Dlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CMFCApplication1Dlg::OnBnClickedButton5)
	ON_BN_CLICKED(OpenVideoButton, &CMFCApplication1Dlg::OnBnClickedOpenvideobutton)
//	ON_EN_CHANGE(VideoPathLoaded, &CMFCApplication1Dlg::OnEnChangeVideopathloaded)
END_MESSAGE_MAP()


// CMFCApplication1Dlg 消息处理程序

BOOL CMFCApplication1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	ShowWindow(SW_MAXIMIZE);

	// TODO:  在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCApplication1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCApplication1Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCApplication1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/*打开视频路径按钮功能实现*/
void CMFCApplication1Dlg::OnBnClickedOpenvideobutton()   
{
	CString pathGaitVideo;
	if (!m_glb.OpenFileDig(pathGaitVideo))//openFiledig函数用来获取pathGaitvideo的值
	{
		return;
	}
	m_editVideoPath.SetWindowTextW(pathGaitVideo);//m_editvideopath = videopathloaded编辑框,将pathGaitVideo赋给m_editVideoPath
	m_editVideoPath.UpdateWindow();
}
/*识别按钮功能实现*/

void CMFCApplication1Dlg::OnBnClickedButton5()        
{
	//视频分帧
	CString pathGaitVideo;
	m_editVideoPath.GetWindowTextW(pathGaitVideo);
	int len = WideCharToMultiByte(CP_ACP, 0, pathGaitVideo, -1, NULL, 0, NULL, NULL);//Cstring To char * in Unicode character set
	char *path = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, pathGaitVideo, -1, path, len, NULL, NULL);
	char* bgPath = "E:\\MFCApplication1\\MFCApplication1\\001-nm.jpg";//背景图像
	vector<Mat> frameVec=m_glb.videoToFrames(path,bgPath);//二值化图像
	m_glb.areaCompare(frameVec);
	//面积比较
	
//	m_glb.preProcess(path);

	//单帧二值化
//	imshow("ff", frameVec[5]);
//	waitKey(0);
	
	//m_glb.findBodyContour(frameVec,initial_frame);

}


void CMFCApplication1Dlg::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码

	

	    CDialogEx::OnOK();

}


void CMFCApplication1Dlg::OnBnClickedCancel()
{
	// TODO:  在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}


void CMFCApplication1Dlg::OnBnClickedButton2()
{
	// TODO:  在此添加控件通知处理程序代码
}


void CMFCApplication1Dlg::OnBnClickedCheck1()
{
	// TODO:  在此添加控件通知处理程序代码
	BOOL bshow = m_btShowVideo.GetCheck(); //检索单选按钮或复选框的复选状态
	m_glb.SetShowVideo(bshow); //void SetShowVideo(BOOL bshow){ m_bShowVideo = bshow; }
}








