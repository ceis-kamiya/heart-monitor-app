// RRD1SampleDlg.cpp : 実装ファイル
//

#include "pch.h"
#include "framework.h"
#include "RRD1Sample.h"
#include "RRD1SampleDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CRRD1SampleDlg ダイアログ

CRRD1SampleDlg::CRRD1SampleDlg(CWnd* pParent /*=nullptr*/)
  : hRRD_(0),
  event_(nullptr),
  timerID_(0),
  thread_(),
  stop_thread_(nullptr),
  wndECG_(),
  wndTemp_(),
  wndAccX_(),
  wndAccY_(),
  wndAccZ_(),
  data_(),
  radioTimer_(),
  radioEvent_(),
  radioCallBack_(),
  editOpenStatus_(),
  editCloseStatus_(),
  editAddress_(),
  staticHeader_(),
  sendID_(),
  tempID_(),
  staticECG1_(),
  staticECG2_(),
  staticECG3_(),
  staticECG4_(),
  staticECG5_(),
  staticTemp1_(),
  staticTemp2_(),
  staticTemp3_(),
  staticTemp4_(),
  staticTemp5_(),
  staticAccX1_(),
  staticAccX2_(),
  staticAccX3_(),
  staticAccX4_(),
  staticAccX5_(),
  staticAccY1_(),
  staticAccY2_(),
  staticAccY3_(),
  staticAccY4_(),
  staticAccY5_(),
  staticAccZ1_(),
  staticAccZ2_(),
  staticAccZ3_(),
  staticAccZ4_(),
  staticAccZ5_(),
  staticEcgMode_(),
  CDialog(IDD_RRD1SAMPLE_DIALOG, pParent)
{
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRRD1SampleDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_RADIO_TIMER, radioTimer_);
  DDX_Control(pDX, IDC_RADIO_EVENT, radioEvent_);
  DDX_Control(pDX, IDC_RADIO_CALLBACK, radioCallBack_);
  DDX_Control(pDX, IDC_STATIC_OPEN_STATUS, editOpenStatus_);
  DDX_Control(pDX, IDC_STATIC_CLOSE_STATUS, editCloseStatus_);
  DDX_Control(pDX, IDC_EDIT_ADDRESS, editAddress_);
  DDX_Control(pDX, IDC_STATIC_HEADER, staticHeader_);
  DDX_Control(pDX, IDC_STATIC_SENDID, sendID_);
  DDX_Control(pDX, IDC_STATIC_TEMP_ID, tempID_);
  DDX_Control(pDX, IDC_STATIC_ECG1, staticECG1_);
  DDX_Control(pDX, IDC_STATIC_ECG2, staticECG2_);
  DDX_Control(pDX, IDC_STATIC_ECG3, staticECG3_);
  DDX_Control(pDX, IDC_STATIC_ECG4, staticECG4_);
  DDX_Control(pDX, IDC_STATIC_ECG5, staticECG5_);
  DDX_Control(pDX, IDC_STATIC_TEMP1, staticTemp1_);
  DDX_Control(pDX, IDC_STATIC_TEMP2, staticTemp2_);
  DDX_Control(pDX, IDC_STATIC_TEMP3, staticTemp3_);
  DDX_Control(pDX, IDC_STATIC_TEMP4, staticTemp4_);
  DDX_Control(pDX, IDC_STATIC_TEMP5, staticTemp5_);
  DDX_Control(pDX, IDC_STATIC_ACCX1, staticAccX1_);
  DDX_Control(pDX, IDC_STATIC_ACCX2, staticAccX2_);
  DDX_Control(pDX, IDC_STATIC_ACCX3, staticAccX3_);
  DDX_Control(pDX, IDC_STATIC_ACCX4, staticAccX4_);
  DDX_Control(pDX, IDC_STATIC_ACCX5, staticAccX5_);
  DDX_Control(pDX, IDC_STATIC_ACCY1, staticAccY1_);
  DDX_Control(pDX, IDC_STATIC_ACCY2, staticAccY2_);
  DDX_Control(pDX, IDC_STATIC_ACCY3, staticAccY3_);
  DDX_Control(pDX, IDC_STATIC_ACCY4, staticAccY4_);
  DDX_Control(pDX, IDC_STATIC_ACCY5, staticAccY5_);
  DDX_Control(pDX, IDC_STATIC_ACCZ1, staticAccZ1_);
  DDX_Control(pDX, IDC_STATIC_ACCZ2, staticAccZ2_);
  DDX_Control(pDX, IDC_STATIC_ACCZ3, staticAccZ3_);
  DDX_Control(pDX, IDC_STATIC_ACCZ4, staticAccZ4_);
  DDX_Control(pDX, IDC_STATIC_ACCZ5, staticAccZ5_);
  DDX_Control(pDX, IDC_STATIC_ECG_MODE, staticEcgMode_);
}

BEGIN_MESSAGE_MAP(CRRD1SampleDlg, CDialog)
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_BN_CLICKED(IDC_BUTTON_OPEN, &CRRD1SampleDlg::OnBnClickedButtonOpen)
  ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CRRD1SampleDlg::OnBnClickedButtonClose)
  ON_WM_CLOSE()
  ON_BN_CLICKED(IDC_BUTTON_GET_ADDRESS, &CRRD1SampleDlg::OnBnClickedButtonGetAddress)
  ON_BN_CLICKED(IDC_BUTTON_SET_ADDRESS, &CRRD1SampleDlg::OnBnClickedButtonSetAddress)
  ON_BN_CLICKED(IDC_BUTTON_START, &CRRD1SampleDlg::OnBnClickedButtonStart)
  ON_BN_CLICKED(IDC_BUTTON_STOP, &CRRD1SampleDlg::OnBnClickedButtonStop)
  ON_WM_TIMER()
END_MESSAGE_MAP()

// CRRD1SampleDlg メッセージ ハンドラー

BOOL CRRD1SampleDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  // このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
  //  Framework は、この設定を自動的に行います。
  SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
  SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

  // TODO: 初期化をここに追加します。
  //受信検出はデフォルトでタイマーを使う
  radioTimer_.SetCheck(BST_CHECKED);
  //受信表示staticのvectorへの登録
  wndECG_.push_back(&staticECG1_);
  wndECG_.push_back(&staticECG2_);
  wndECG_.push_back(&staticECG3_);
  wndECG_.push_back(&staticECG4_);
  wndECG_.push_back(&staticECG5_);
  wndTemp_.push_back(&staticTemp1_);
  wndTemp_.push_back(&staticTemp2_);
  wndTemp_.push_back(&staticTemp3_);
  wndTemp_.push_back(&staticTemp4_);
  wndTemp_.push_back(&staticTemp5_);
  wndAccX_.push_back(&staticAccX1_);
  wndAccX_.push_back(&staticAccX2_);
  wndAccX_.push_back(&staticAccX3_);
  wndAccX_.push_back(&staticAccX4_);
  wndAccX_.push_back(&staticAccX5_);
  wndAccY_.push_back(&staticAccY1_);
  wndAccY_.push_back(&staticAccY2_);
  wndAccY_.push_back(&staticAccY3_);
  wndAccY_.push_back(&staticAccY4_);
  wndAccY_.push_back(&staticAccY5_);
  wndAccZ_.push_back(&staticAccZ1_);
  wndAccZ_.push_back(&staticAccZ2_);
  wndAccZ_.push_back(&staticAccZ3_);
  wndAccZ_.push_back(&staticAccZ4_);
  wndAccZ_.push_back(&staticAccZ5_);

  return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

//ヘッダ部の文字列を生成し表示する
void displayHeader(CWnd* _wnd, const RRD1DataEx& _data)
{
  using namespace std;

  stringstream ss;
  ss << setw(2) << setfill('0') << _data.hour << ':'
    << setw(2) << setfill('0') << _data.min << ':'
    << setw(2) << setfill('0') << _data.sec << '.'
    << setw(3) << setfill('0') << _data.msec;

  switch (_data.ecg_mode) {
  case 0:
    ss << " 心拍波形";
    break;
  case 1:
    ss << " 心拍周期";
    break;
  case 2:
    ss << " 心拍数";
    break;
  default:
    ss << " 加速度";
  }
  if (1 == _data.ecg_mode || 2 == _data.ecg_mode) {
    ss << " 加速度センサモード: " << (0 == _data.acc_mode ? "移動平均" : "ピークホールド");
  }
  if (0 != _data.ecg_mode && 0 == _data.sampling_freq)
    ss << " 1拍1回送信";
  if (_data.lowbattery)
    ss << " ローバッテリー";
  if (_data.saturation)
    ss << " 入力信号飽和";
  _wnd->SetWindowText(ss.str().c_str());
}

//BYTE値を数値としてウィンドウに書き込む
void displayByteValue(CWnd* _wnd, BYTE _b)
{
  std::stringstream ss;
  ss << static_cast<int>(_b);

  _wnd->SetWindowText(ss.str().c_str());
}

//WORD値をウィンドウに書き込む
void displayWordValue(CWnd* _wnd, WORD _w)
{
  std::stringstream ss;
  ss << static_cast<int>(_w);

  _wnd->SetWindowText(ss.str().c_str());
}

//字数を下 _p 桁の制度で表示する
void displayFloatValue(CWnd* _wnd, double _v, int _p)
{
  std::stringstream ss;
  ss.setf(std::ios_base::fixed, std::ios_base::floatfield);
  ss << std::setprecision(_p) << _v;

  _wnd->SetWindowText(ss.str().c_str());
}

//受信文字を更新する
void displayReceivedData(CRRD1SampleDlg& _p, const RRD1DataEx& _data)
{
  displayHeader(&_p.staticHeader_, _data);
  displayByteValue(&_p.sendID_, _data.sendedID);
  displayByteValue(&_p.tempID_, _data.tempID);

  switch (_data.ecg_mode)
  {
  case 0:
    _p.staticEcgMode_.SetWindowText("心拍波形");
    for (int i = 0; i != _data.data_count; ++i)
      displayWordValue(_p.wndECG_[i], _data.data[i].ecg);
    break;
  case 1:
    _p.staticEcgMode_.SetWindowText("心拍間隔");
    for (int i = 0; i != _data.data_count; ++i)
      displayWordValue(_p.wndECG_[i], _data.data[i].ecg);
    break;
  case 2:
    _p.staticEcgMode_.SetWindowText("心拍数");
    for (int i = 0; i != _data.data_count; ++i)
      displayFloatValue(_p.wndECG_[i], 60.0 * 1000.0 / double(_data.data[i].ecg), 1);
    break;
  case 3:
    for (auto wnd : _p.wndECG_) {
      wnd->SetWindowText("");
    }
    for (int i = 0; i != _data.data_count; ++i) {
      displayFloatValue(_p.wndECG_[i], std::hypot(_data.data[i].acc_x, _data.data[i].acc_y, _data.data[i].acc_z), 4);
    }
    _p.staticEcgMode_.SetWindowText("|加速度|");
  default:
    break;
  }

  for (int i = 0; i != _data.data_count; ++i) {
    displayFloatValue(_p.wndTemp_[i], _data.data[i].temp, 1);
    displayFloatValue(_p.wndAccX_[i], _data.data[i].acc_x, 4);
    displayFloatValue(_p.wndAccY_[i], _data.data[i].acc_y, 4);
    displayFloatValue(_p.wndAccZ_[i], _data.data[i].acc_z, 4);
  }
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CRRD1SampleDlg::OnPaint()
{
  if (IsIconic())
  {
    CPaintDC dc(this); // 描画のデバイス コンテキスト

    SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

    // クライアントの四角形領域内の中央
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    // アイコンの描画
    dc.DrawIcon(x, y, m_hIcon);
  }
  else
  {
    displayReceivedData(*this, data_);
    CDialog::OnPaint();
  }
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CRRD1SampleDlg::OnQueryDragIcon()
{
  return static_cast<HCURSOR>(m_hIcon);
}

//オブジェクトの後始末
void closeHandles(CRRD1SampleDlg& p)
{
  if (p.timerID_) {
    p.KillTimer(p.timerID_);
    p.timerID_ = 0;
  }
  if (p.thread_.joinable()) {
    p.stop_thread_->SetEvent();
    p.thread_.join();
  }
  if (p.event_) {
    p.event_ = nullptr;
  }
  if (p.stop_thread_) {
    p.stop_thread_ = nullptr;
  }
}

//接続が解除されたときのコールバック関数
VOID WINAPI callbackDisconnect(VOID* _p, VOID*)
{
  CRRD1SampleDlg* p = reinterpret_cast<CRRD1SampleDlg*>(_p);

  UTWSCloseDevice(p->hRRD_);
  p->hRRD_ = 0;
  p->MessageBox("ドングルが抜けました．ハンドルは閉じられます．");
  CloseHandle(*p);
}

void CRRD1SampleDlg::OnBnClickedButtonOpen()
{
  hRRD_ = UTWSOpenDevice(UTWS_RRD_1, 0);
  char buffer[256];
  UTWSGetErrorMessage(hRRD_, buffer, 256);
  editOpenStatus_.SetWindowText(buffer);
  if (hRRD_)
    UTWSRRD1SetCallBackDisconnect(hRRD_, callbackDisconnect, this);
}

void CRRD1SampleDlg::OnBnClickedButtonClose()
{
  closeHandles(*this);
  UTWSCloseDevice(hRRD_);
  hRRD_ = 0;
  char buffer[256];
  UTWSGetErrorMessage(0, buffer, 256);
  editCloseStatus_.SetWindowText(buffer);
}

void CRRD1SampleDlg::OnClose()
{
  OnBnClickedButtonClose();
  CDialog::OnClose();
}

void CRRD1SampleDlg::OnBnClickedButtonGetAddress()
{
  CHAR address[11];
  if (UTWSRRD1GetLocalAddress(hRRD_, address)) {
    editAddress_.SetWindowText(address);
  }
  else {
    editAddress_.SetWindowText("失敗しました");
  }
}

void CRRD1SampleDlg::OnBnClickedButtonSetAddress()
{
  using namespace std;

  size_t n = editAddress_.GetWindowTextLength();
  vector<CHAR> buf(n + 1);
  editAddress_.GetWindowText(&buf[0], buf.size());

  if (UTWSRRD1SetLocalAddress(hRRD_, &buf[0])) {
    MessageBox("設定しました．");
  }
  else {
    UINT err_id = UTWSGetErrorMessage(hRRD_, 0, 0);
    if (UTWS_ERR_BAD_DESTINATION_ADDRESS == err_id) {
      MessageBox("宛先アドレスが間違っています．");
    }
    else {
      MessageBox("失敗しました．");
    }
  }
}

int recievingMethod(CRRD1SampleDlg& p)
{
  if (p.radioTimer_.GetCheck())
    return 0;
  if (p.radioEvent_.GetCheck())
    return 1;
  return 2;
}

//受信をイベントで検知するスレッド
void thread_event(CRRD1SampleDlg* p)
{
  while (true) {
    if (WAIT_OBJECT_0 == WaitForSingleObject(*p->stop_thread_, 0)) {
      return;
    }
    if (WAIT_OBJECT_0 == WaitForSingleObject(*p->event_, 1)) {
      p->event_->ResetEvent();
      if (WAIT_OBJECT_0 == WaitForSingleObject(*p->stop_thread_, 0)) {
        return;
      }
      if (UTWSRRD1GetDataEx(p->hRRD_, &p->data_)) {
        p->SendMessage(WM_PAINT);
      }
    }
  }
}

//コールバック関数
VOID CALLBACK callbackRecieved(VOID* _p, VOID* _h)
{
  CRRD1SampleDlg* p = reinterpret_cast<CRRD1SampleDlg*>(_p);

  UINT count = 0;
  while (-1 != (count = UTWSRRD1DataCount(p->hRRD_)) && count) {
    UTWSRRD1GetDataEx(p->hRRD_, &p->data_);
    p->SendMessage(WM_PAINT);
  }
}

void CRRD1SampleDlg::OnBnClickedButtonStart()
{
  using namespace std;

  for_each(wndECG_.begin(), wndECG_.end(), [](auto* p) {p->SetWindowText(""); });
  for_each(wndTemp_.begin(), wndTemp_.end(), [](auto* p) {p->SetWindowText(""); });
  for_each(wndAccX_.begin(), wndAccX_.end(), [](auto* p) {p->SetWindowText(""); });
  for_each(wndAccY_.begin(), wndAccY_.end(), [](auto* p) {p->SetWindowText(""); });
  for_each(wndAccZ_.begin(), wndAccZ_.end(), [](auto* p) {p->SetWindowText(""); });

  if (!hRRD_) {
    MessageBox("RDD-1をオープンしていません");
    return;
  }

  //受信停止させ、ハンドル類をクローズしておく
  UTWSRRD1StopReceiving(hRRD_);
  closeHandles(*this);

  switch (recievingMethod(*this)) {
  case 0:
    SetTimer(timerID_ = 100, 1, 0);
    UTWSRRD1StartReceiving(hRRD_, 0, 0, 0);
    break;

  case 1:
    event_ = make_shared<CEvent>(FALSE, TRUE);
    stop_thread_ = make_shared<CEvent>(FALSE, TRUE);
    UTWSRRD1StartReceiving(hRRD_, *event_, 0, 0);
    thread_ = thread(thread_event, this);
    break;

  case 2:
    UTWSRRD1StartReceiving(hRRD_, 0, callbackRecieved, this);
    break;

  default:
    return;
  }
}

void CRRD1SampleDlg::OnBnClickedButtonStop()
{
  if (!hRRD_)
    return;
  UTWSRRD1StopReceiving(hRRD_);
  closeHandles(*this);
}

void CRRD1SampleDlg::OnTimer(UINT_PTR nIDEvent)
{
  UINT count = 0;
  if (-1 != (count = UTWSRRD1DataCount(hRRD_)) && count)
  {
    UTWSRRD1GetDataEx(hRRD_, &data_);
    SendMessage(WM_PAINT);
  }

  CDialog::OnTimer(nIDEvent);
}