// WHS1SampleDlg.h : ヘッダー ファイル
//

#pragma once

// CWHS1SampleDlg ダイアログ
class CWHS1SampleDlg : public CDialog
{
  //全ダイアログアイテムを登録する
  std::vector<CWnd*> items_;

  // コンストラクション
public:
  CWHS1SampleDlg(CWnd* pParent = nullptr);	// 標準コンストラクター

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_WHS1SAMPLE_DIALOG };
#endif

protected:
  virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート

// 実装
protected:
  HICON m_hIcon;

  // 生成された、メッセージ割り当て関数
  virtual BOOL OnInitDialog();
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  DECLARE_MESSAGE_MAP()
public:
  //WHS-1のハンドル
  HANDLE hwhs_;
  //非同期読み出しが完了したときにシグナル状態となるイベントオブジェクト
  CEvent download_event_;
  //設定情報の更新
  void updateConfig();

  CEdit editTempID_;
  CButton radioRF_;
  CButton radioMemory_;
  CButton radioWave_;
  CButton radioRRI_;
  CButton radioHR_;
  CButton chkUpdateWithBeat_;
  CButton chkMonitor_;
  CButton radioAccPeakHold_;
  CButton radioACCAverage_;
  CButton radioOverWrite_;
  CButton radioMemoryStop_;
  CEdit editFWVersion_;
  CStatic staticTempOffset_;
  CEdit editFlushWriteCount_;
  CEdit editSerialNo_;
  CEdit editCPUID_;
  CEdit editBlockSize_;
  CStatic staticStatus_;
  CStatic groupACC_;
  CStatic groupMemory_;
  CStatic staticTime_;
  CEdit editDestinationAddress_;
  CEdit editTempOffset_;
  afx_msg void OnClose();
  afx_msg void OnBnClickedButtonRead();
  afx_msg void OnBnClickedButtonWrite();
  afx_msg void OnBnClickedButtonMemory();
  afx_msg void OnBnClickedButtonAsyncRead();
  afx_msg void OnBnClickedButtonBintocsv();
  afx_msg void OnBnClickedButtonSendAddres();
};
