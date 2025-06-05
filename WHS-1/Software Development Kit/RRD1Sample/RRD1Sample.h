// RRD1Sample.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです
//

#pragma once

#ifndef __AFXWIN_H__
#error "PCH に対してこのファイルをインクルードする前に 'pch.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル

// CRRD1SampleApp:
// このクラスの実装については、RRD1Sample.cpp を参照してください
//

class CRRD1SampleApp : public CWinApp
{
public:
  CRRD1SampleApp();

  // オーバーライド
public:
  virtual BOOL InitInstance();

  // 実装

  DECLARE_MESSAGE_MAP()
};

extern CRRD1SampleApp theApp;
