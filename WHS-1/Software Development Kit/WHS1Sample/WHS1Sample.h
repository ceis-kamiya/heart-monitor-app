
// WHS1Sample.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'pch.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CWHS1SampleApp:
// このクラスの実装については、WHS1Sample.cpp を参照してください
//

class CWHS1SampleApp : public CWinApp
{
public:
	CWHS1SampleApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CWHS1SampleApp theApp;
