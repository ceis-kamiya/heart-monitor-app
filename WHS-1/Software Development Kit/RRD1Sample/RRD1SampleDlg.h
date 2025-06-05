// RRD1SampleDlg.h : ヘッダー ファイル
//

#pragma once

// CRRD1SampleDlg ダイアログ
class CRRD1SampleDlg : public CDialog
{
  // コンストラクション
public:
  CRRD1SampleDlg(CWnd* pParent = nullptr);	// 標準コンストラクター

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_RRD1SAMPLE_DIALOG };
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
  //RDD-1のハンドル
  HANDLE hRRD_;
  //受信イベント
  std::shared_ptr<CEvent> event_;
  //受信タイマー
  UINT_PTR timerID_;
  //イベント受信のための受信スレッド
  std::thread thread_;
  //受信スレッド停止イベント
  std::shared_ptr<CEvent> stop_thread_;
  //受信データ表示のstaticをvectorでまとめる
  std::vector<CStatic*> wndECG_;
  std::vector<CStatic*> wndTemp_;
  std::vector<CStatic*> wndAccX_;
  std::vector<CStatic*> wndAccY_;
  std::vector<CStatic*> wndAccZ_;
  //受信データ
  RRD1DataEx data_;

  CButton radioTimer_;
  CButton radioEvent_;
  CButton radioCallBack_;
  CStatic editOpenStatus_;
  CStatic editCloseStatus_;
  CEdit editAddress_;
  CStatic staticHeader_;
  CStatic sendID_;
  CStatic tempID_;
  CStatic staticECG1_;
  CStatic staticECG2_;
  CStatic staticECG3_;
  CStatic staticECG4_;
  CStatic staticECG5_;
  CStatic staticTemp1_;
  CStatic staticTemp2_;
  CStatic staticTemp3_;
  CStatic staticTemp4_;
  CStatic staticTemp5_;
  CStatic staticAccX1_;
  CStatic staticAccX2_;
  CStatic staticAccX3_;
  CStatic staticAccX4_;
  CStatic staticAccX5_;
  CStatic staticAccY1_;
  CStatic staticAccY2_;
  CStatic staticAccY3_;
  CStatic staticAccY4_;
  CStatic staticAccY5_;
  CStatic staticAccZ1_;
  CStatic staticAccZ2_;
  CStatic staticAccZ3_;
  CStatic staticAccZ4_;
  CStatic staticAccZ5_;
  CStatic staticEcgMode_;
  afx_msg void OnBnClickedButtonOpen();
  afx_msg void OnBnClickedButtonClose();
  afx_msg void OnClose();
  afx_msg void OnBnClickedButtonGetAddress();
  afx_msg void OnBnClickedButtonSetAddress();
  afx_msg void OnBnClickedButtonStart();
  afx_msg void OnBnClickedButtonStop();
  afx_msg void OnTimer(UINT_PTR nIDEvent);
};
