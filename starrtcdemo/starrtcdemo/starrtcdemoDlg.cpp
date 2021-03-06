
// startrtcdemoDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "starrtcdemo.h"
#include "starrtcdemoDlg.h"
#include "afxdialogex.h"
#include "HttpClient.h"
#include "json.h"
#include "CProcessInfo.h"
#include "CSelectShowWndDlg.h"
#include "CPicControl.h"

#include "CFaceFeatureRegisterDlg.h"
#include "CLogin.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif



enum LIST_REPORT_NAME
{
	LIVE_NAME = 0,
	LIVE_ID,
	LIVE_STATUS,
	LIVE_CREATER
};

#define USER_DEF_RESOURCE_ID 1000


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CstartrtcdemoDlg 对话框



CstartrtcdemoDlg::CstartrtcdemoDlg(CUserManager* pUserManager, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_STARTRTCDEMO_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hRecHinstance = NULL;
	m_pUserManager = pUserManager;
	m_nShowLiveSize = 4;
	memset(mShowPicControlVector, 0, sizeof(CStatic*) * 4);
}

CstartrtcdemoDlg::~CstartrtcdemoDlg()
{
	clear();
}

void CstartrtcdemoDlg::clear()
{
	for (int i = 0; i < (int)mProcessVector.size(); i++)
	{
		CProcessInfo* pProcessInfo = mProcessVector[i];
		if (pProcessInfo != NULL)
		{
			pProcessInfo->removeAllUpUser();
			if (pProcessInfo->m_pPi != NULL)
			{
				TerminateProcess(pProcessInfo->m_pPi->hProcess, 0);
				CloseHandle(pProcessInfo->m_pPi->hThread);
				pProcessInfo->m_pPi->hThread = NULL;
				CloseHandle(pProcessInfo->m_pPi->hProcess);
				pProcessInfo->m_pPi->hProcess = NULL;
				delete pProcessInfo->m_pPi;
				pProcessInfo->m_pPi = NULL;
			}
			delete pProcessInfo;
			pProcessInfo = NULL;
		}
	}
	mProcessVector.clear();
}

void CstartrtcdemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_liveListBox);
	DDX_Control(pDX, IDC_STATIC_SHOW_AREA, m_ShowArea);
}

BEGIN_MESSAGE_MAP(CstartrtcdemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_BRUSH, &CstartrtcdemoDlg::OnBnClickedButtonBrush)
	ON_WM_COPYDATA()
	ON_LBN_SELCHANGE(IDC_LIST1, &CstartrtcdemoDlg::OnLbnSelchangeList1)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_USER_REGISTER, &CstartrtcdemoDlg::OnBnClickedButtonUserRegister)
END_MESSAGE_MAP()


// CstartrtcdemoDlg 消息处理程序




BOOL CstartrtcdemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标


	CRect showRect;
	::GetWindowRect(m_ShowArea, showRect);
	CRect dlgRect;
	::GetWindowRect(this->m_hWnd, dlgRect);
	int left = showRect.left - dlgRect.left - 8;
	int top = showRect.top - dlgRect.top - 23;

	CRect rectArr[4];
	CRect rect1(left + 1, top + 1, left + showRect.Width() / 2 - 1, top + showRect.Height() / 2 - 1);
	CRect rect2(rect1.right + 1, top + 1, rect1.right + showRect.Width() / 2 - 2, top + showRect.Height() / 2 - 1);
	CRect rect3(left + 1, rect1.bottom + 1, left + showRect.Width() / 2 - 1, rect1.bottom + showRect.Height() / 2 - 2);
	CRect rect4(rect1.right + 1, rect1.bottom + 1, rect1.right + showRect.Width() / 2 - 1, rect1.bottom + showRect.Height() / 2 - 2);

	rectArr[0] = rect1;
	rectArr[1] = rect2;
	rectArr[2] = rect3;
	rectArr[3] = rect4;
	CString strWinName = "";
	for (int i = 0; i < 4; i++)
	{
		CPicControl *pPicControl = new CPicControl();
		CRect rect1(left, top, left + showRect.Width() / 2, top + showRect.Height() / 2);
		pPicControl->Create(_T(""), WS_CHILD | WS_VISIBLE | SS_BITMAP, rectArr[i], this, USER_DEF_RESOURCE_ID + i * 10);
		//mShowPicControlVector[i] = pPicControl;
		pPicControl->ShowWindow(SW_SHOW);
		DWORD dwStyle = ::GetWindowLong(pPicControl->GetSafeHwnd(), GWL_STYLE);
		::SetWindowLong(pPicControl->GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);

		CProcessInfo *pProcessInfo = NULL;
		strWinName.Format("starrtc%d", i + 1);
		PROCESS_INFORMATION* pPi = openProcess(strWinName);
		if (pPi != NULL)
		{
			CRect rect1(left, top, left + showRect.Width() / 2, top + showRect.Height() / 2);
			pProcessInfo = new CProcessInfo(this, rect1);
			pProcessInfo->m_strWindowName = strWinName;
			pProcessInfo->m_pPi = pPi;
			pProcessInfo->m_pPictureControl = pPicControl;
			pProcessInfo->m_pPictureControlArr[0] = pPicControl;
			pProcessInfo->m_DrawRect = rectArr[i];
			pPicControl->setInfo(this, pProcessInfo);
			CRect rectClient = rectArr[i];
			CRect rectChild(rectClient.right - (int)(rectClient.Width()*0.25), rectClient.top, rectClient.right, rectClient.bottom);
			for (int n = 0; n < 6; n++)
			{
				CPicControl *pPictureControl = new CPicControl();
				pPictureControl->setInfo(this, pProcessInfo);
				rectChild.top = rectClient.top + n * (long)(rectClient.Height()*0.25);
				rectChild.bottom = rectClient.top + (n + 1) * (long)(rectClient.Height()*0.25);
				pPictureControl->Create(_T(""), WS_CHILD | WS_VISIBLE | SS_BITMAP, rectChild, this, USER_DEF_RESOURCE_ID + i * 10 + n + 1);
				pProcessInfo->m_pPictureControlArr[n + 1] = pPictureControl;
				DWORD dwStyle1 = ::GetWindowLong(pPictureControl->GetSafeHwnd(), GWL_STYLE);
				::SetWindowLong(pPictureControl->GetSafeHwnd(), GWL_STYLE, dwStyle1 | SS_NOTIFY);
			}
			mProcessVector.push_back(pProcessInfo);
		}
		else
		{
			CString strErr = "";
			strErr.Format("open process failed, wnd name:%s", strWinName);
			AfxMessageBox(strErr);
		}
	}

	Sleep(10);

	getLiveList();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CstartrtcdemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CstartrtcdemoDlg::OnPaint()
{
	CPaintDC dc1(this); // 用于绘制的设备上下文
	for (int i = 0; i < (int)mProcessVector.size(); i++)
	{
		dc1.SetStretchBltMode(COLORONCOLOR);
		FillRect(dc1.m_hDC, &mProcessVector[i]->m_DrawRect, CBrush(RGB(0, 0, 0)));
		mProcessVector[i]->drawBackground(mProcessVector[i]->m_pPictureControlArr[0]);
	}
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
HCURSOR CstartrtcdemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CstartrtcdemoDlg::getLiveList()
{
	mVLivePrograms.clear();

	CString stUrl = "";
	stUrl.Format(_T("https://api.starrtc.com/public/live/list?appid=%s"), m_pUserManager->m_ServiceParam.m_strAgentId.c_str());
	int port = 9904;
	char* data = "";

	CString strPara = _T("");
	CString strContent;

	CHttpClient httpClient;
	int nRet = httpClient.HttpPost(stUrl, strPara, strContent);

	string str_json = strContent.GetBuffer(0);
	Json::Reader reader;
	Json::Value root;
	if (nRet == 0 && str_json != "" && reader.parse(str_json, root))  // reader将Json字符串解析到root，root将包含Json里所有子元素   
	{
		std::cout << "========================[Dispatch]========================" << std::endl;
		if (root.isMember("status") && root["status"].asInt() == 1)
		{
			if (root.isMember("data"))
			{
				Json::Value data = root.get("data", "");
				int nSize = data.size();
				for (int i = 0; i < nSize; i++)
				{
					CLiveProgram livePro;

					if (data[i].isMember("ID"))
					{
						livePro.m_strId = data[i]["ID"].asCString();
					}

					if (data[i].isMember("Name"))
					{
						livePro.m_strName = data[i]["Name"].asCString();
					}

					if (data[i].isMember("Creator"))
					{
						livePro.m_strCreator = data[i]["Creator"].asCString();
					}
					livePro.m_liveState = FALSE;
					if (data[i].isMember("liveState"))
					{
						CString status = data[i]["liveState"].asCString();
						if (status == "1")
						{
							livePro.m_liveState = TRUE;
						}

					}
					mVLivePrograms.push_back(livePro);

				}
			}

		}

	}

	m_liveListBox.ResetContent();
	int nRowIndex = 0;

	for (int i = 0; i < (int)mVLivePrograms.size(); i++)
	{
		m_liveListBox.AddString(mVLivePrograms[i].m_strName);
	}
}

void CstartrtcdemoDlg::OnBnClickedButtonBrush()
{

	/*for (int i = 0; i < 4; i++)
	{
		CImage image;  // CBitmap对象，用于加载位图
		CString str;
		str.Format("%d.jpg", i + 1);
		image.Load(str);
		CRect rect;
		CDC* pDC = mShowPicControlVector[i]->GetWindowDC();    //获得显示控件的DC
		mShowPicControlVector[i]->GetClientRect(rect);
		pDC->SetStretchBltMode(COLORONCOLOR);
		image.Draw(pDC->m_hDC, rect);      //图片类的图片绘制Draw函数
		//ReleaseDC(pDC);
	}*/

	getLiveList();
}




CLiveProgram* CstartrtcdemoDlg::getLiveProgram(CString strName)
{
	CLiveProgram* pRet = NULL;
	for (int i = 0; i < (int)mVLivePrograms.size(); i++)
	{
		if (mVLivePrograms[i].m_strName == strName)
		{
			pRet = new CLiveProgram();
			pRet->m_strId = mVLivePrograms[i].m_strId;
			pRet->m_strName = mVLivePrograms[i].m_strName;
			pRet->m_strCreator = mVLivePrograms[i].m_strCreator;
			pRet->m_liveState = mVLivePrograms[i].m_liveState;
			break;
		}
	}
	return pRet;
}

bool CstartrtcdemoDlg::sendMessage(CString winName, CString strData)
{
	bool bRet = false;
	LRESULT copyDataResult;  //copyDataResult has value returned by other app 
	CWnd *pOtherWnd = CWnd::FindWindow(NULL, winName);
	if (pOtherWnd)
	{
		COPYDATASTRUCT cpd;
		cpd.dwData = 0;
		cpd.cbData = strData.GetLength() + sizeof(wchar_t);             //data length
		cpd.lpData = (void*)strData.GetBuffer(cpd.cbData); //data buffer
		copyDataResult = pOtherWnd->SendMessage(WM_COPYDATA, (WPARAM)AfxGetApp()->m_pMainWnd->GetSafeHwnd(), (LPARAM)&cpd);
		strData.ReleaseBuffer();
		bRet = true;
	}
	return bRet;
}


PROCESS_INFORMATION* CstartrtcdemoDlg::openProcess(CString strWindowName)
{
	STARTUPINFO si;
	memset(&si, 0, sizeof(STARTUPINFO));//初始化si在内存块中的值（详见memset函数）
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;
	string str = " " + strWindowName;
	CString strPath;
	strPath.Format("%s.exe", "starrtc");

	PROCESS_INFORMATION* pPi = new PROCESS_INFORMATION();//必备参数设置结束
	if (!CreateProcess(TEXT(strPath),
		(LPSTR)str.c_str(),
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&si,
		pPi
	)) {
		delete pPi;
		pPi = NULL;
	}
	return pPi;
}

BOOL CstartrtcdemoDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	char strWinName[128] = { 0 };
	::GetWindowText(pWnd->m_hWnd, strWinName, sizeof(strWinName) / sizeof(char));
	//CString strRecievedText = (LPCSTR)(pCopyDataStruct->lpData);
	//string str(strRecievedText.GetBuffer(pCopyDataStruct->cbData));
	CProcessInfo* pProcessInfo = findProcessInfo(strWinName);
	if (pProcessInfo != NULL)
	{
		pProcessInfo->setData((uint8_t*)pCopyDataStruct->lpData, pCopyDataStruct->cbData);
	}
	return CDialogEx::OnCopyData(pWnd, pCopyDataStruct);
}

CProcessInfo* CstartrtcdemoDlg::findProcessInfo(string strWinName)
{
	CProcessInfo* pProcessInfo = NULL;
	for (int i = 0; i < (int)mProcessVector.size(); i++)
	{
		if (strWinName == mProcessVector[i]->m_strWindowName)
		{
			pProcessInfo = mProcessVector[i];
			break;
		}
	}

	return pProcessInfo;
}


void CstartrtcdemoDlg::OnLbnSelchangeList1()
{
	int nCurSel;

	nCurSel = m_liveListBox.GetCurSel();    // 获取当前选中列表项

	if (nCurSel >= 0)
	{
		//CString str = m_liveList.GetItemText(nIndex, LIVE_NAME);
		CString str = "";
		m_liveListBox.GetText(nCurSel, str);

		CLiveProgram* pLiveProgram = getLiveProgram(str);
		if (pLiveProgram != NULL)
		{
			string strId = pLiveProgram->m_strId.GetBuffer(0);
			if (strId.length() == 32)
			{
				CSelectShowWndDlg selectDlg;
				selectDlg.DoModal();
				if (selectDlg.m_bSure)
				{
					if (((int)mProcessVector.size()) > selectDlg.m_nSelect)
					{
						string strChannelId = strId.substr(0, 16);
						string strChatRoomId = strId.substr(16, 16);
						CString str;
						CLogin login(m_pUserManager);
						str.Format("%d", (int)PARENT_PROCESS_START_LIVE);
						string strMessage = "{\"type\":";
						strMessage += str;
						strMessage += ",\"channelId\":\"" + strChannelId;
						strMessage += "\",\"chatroomId\":\"" + strChatRoomId;
						strMessage += "\",\"config\":{" + login.getUserManagerInfo();
						strMessage += "}";
						strMessage += "}";

						bool bSend = sendMessage(mProcessVector[selectDlg.m_nSelect]->m_strWindowName.c_str(), strMessage.c_str());
						if (!bSend)
						{
							AfxMessageBox("send message failed!");
						}
					}

					/*CLiveDialog dlg(m_pUserManager, this);

					dlg.m_pStartRtcLive->setInfo(strChannelId, strChatRoomId);
					dlg.DoModal();*/
				}

			}
			else
			{
				CString strMessage = "";
				strMessage.Format("err id %s", strId.c_str());
				AfxMessageBox(strMessage);
			}
		}

	}
}




void CstartrtcdemoDlg::OnDestroy()
{
	clear();
	CDialogEx::OnDestroy();
}

void CstartrtcdemoDlg::liveExit(void* pParam)
{
	CProcessInfo* pProcessInfo = (CProcessInfo*)pParam;
	CString str;
	str.Format("%d", (int)PARENT_PROCESS_LIVE_EXIT);
	string strMessage = "{\"type\":";
	strMessage += str + "}";

	bool bSend = sendMessage(pProcessInfo->m_strWindowName.c_str(), strMessage.c_str());
	if (!bSend)
	{
		AfxMessageBox("send message failed!");
	}
}
void CstartrtcdemoDlg::changeStreamConfig(void* pParam, int upid)
{
	CProcessInfo* pProcessInfo = (CProcessInfo*)pParam;
	if (pProcessInfo != NULL && upid != -1 && upid < UPID_MAX_SIZE)
	{
		if (pProcessInfo->m_configArr[upid] != 2)
		{
			for (int i = 0; i < UPID_MAX_SIZE; i++)
			{
				if (pProcessInfo->m_configArr[i] == 2)
				{
					pProcessInfo->m_configArr[i] = 1;
					pProcessInfo->m_configArr[upid] = 2;
					pProcessInfo->setStreamConfig();
					pProcessInfo->setShowPictures();
					break;
				}
			}
		}

	}

}

void CstartrtcdemoDlg::closeCurrentLive(void* pParam)
{
	CProcessInfo* pProcessInfo = (CProcessInfo*)pParam;
	liveExit(pProcessInfo);
}

void CstartrtcdemoDlg::startFaceFeature(void* pParam)
{
	CProcessInfo* pProcessInfo = (CProcessInfo*)pParam;
	CString str;
	str.Format("%d", (int)PARENT_PROCESS_FACE_FEATURE);
	string strMessage = "{\"type\":";
	strMessage += str;
	strMessage += ",\"value\":";
	strMessage += "1";
	strMessage += "}";


	bool bSend = sendMessage(pProcessInfo->m_strWindowName.c_str(), strMessage.c_str());
	if (!bSend)
	{
		AfxMessageBox("send message failed!");
	}
}

void CstartrtcdemoDlg::stopFaceFeature(void* pParam)
{
	CProcessInfo* pProcessInfo = (CProcessInfo*)pParam;
	CString str;
	str.Format("%d", (int)PARENT_PROCESS_FACE_FEATURE);
	string strMessage = "{\"type\":";
	strMessage += str;
	strMessage += ",\"value\":";
	strMessage += "0";
	strMessage += "}";


	bool bSend = sendMessage(pProcessInfo->m_strWindowName.c_str(), strMessage.c_str());
	if (!bSend)
	{
		AfxMessageBox("send message failed!");
	}
}


void CstartrtcdemoDlg::OnBnClickedButtonUserRegister()
{
	CFaceFeatureRegisterDlg dlg;

	dlg.DoModal();
}
