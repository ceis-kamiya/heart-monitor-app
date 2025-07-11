﻿// RRD1Sample.cpp : アプリケーションのクラス動作を定義します。
//

#include "pch.h"
#include "framework.h"
#include "RRD1Sample.h"
#include "RRD1SampleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CRRD1SampleApp

BEGIN_MESSAGE_MAP(CRRD1SampleApp, CWinApp)
  ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

// CRRD1SampleApp の構築

CRRD1SampleApp::CRRD1SampleApp()
{
}

// 唯一の CRRD1SampleApp オブジェクト

CRRD1SampleApp theApp;

// CRRD1SampleApp の初期化

BOOL CRRD1SampleApp::InitInstance()
{
  CWinApp::InitInstance();

  CRRD1SampleDlg dlg;
  m_pMainWnd = &dlg;
  INT_PTR nResponse = dlg.DoModal();
  if (nResponse == IDOK)
  {
  }
  else if (nResponse == IDCANCEL)
  {
  }
  else if (nResponse == -1)
  {
    TRACE(traceAppMsg, 0, "警告: ダイアログの作成に失敗しました。アプリケーションは予期せずに終了します。\n");
    TRACE(traceAppMsg, 0, "警告: ダイアログで MFC コントロールを使用している場合、#define _AFX_NO_MFC_CONTROLS_IN_DIALOGS を指定できません。\n");
  }

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
  ControlBarCleanUp();
#endif

  // ダイアログは閉じられました。アプリケーションのメッセージ ポンプを開始しないで
  //  アプリケーションを終了するために FALSE を返してください。
  return FALSE;
}