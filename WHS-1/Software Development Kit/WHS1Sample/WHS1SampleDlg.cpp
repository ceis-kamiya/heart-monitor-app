// WHS1SampleDlg.cpp : 実装ファイル
//

#include "pch.h"
#include "framework.h"
#include "WHS1Sample.h"
#include "WHS1SampleDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//時計の更新
CEvent timeUpdate_mutex(TRUE, TRUE);
std::thread timeUpdate_thread;
void timeUpdate(CStatic* _wnd)
{
  using namespace std;

  while (WAIT_OBJECT_0 != WaitForSingleObject(timeUpdate_mutex ,10)) {
    auto tp = chrono::system_clock::now();
    time_t tt = chrono::system_clock::to_time_t(tp);
    tm t;
    localtime_s(&t, &tt);

    stringstream ss;
    ss << t.tm_year + 1900 << '/' << setw(2) << setfill('0') << t.tm_mon + 1 << '/'
      << setw(2) << setfill('0') << t.tm_mday << ' '
      << setw(2) << setfill('0') << t.tm_hour << ':'
      << setw(2) << setfill('0') << t.tm_min << ':'
      << setw(2) << setfill('0') << t.tm_sec;
    _wnd->SetWindowText(ss.str().c_str());
    _wnd->Invalidate(0);
  }
}

//ErrorMessage
void errorMessage(HANDLE _h, const std::string* _comment = 0)
{
  using namespace std;

  char msg[1024];
  UTWSGetErrorMessage(_h, msg, 1024);
  string smsg(msg);
  if (_comment)
    smsg += string(" : ") + *_comment;
  MessageBox(0, smsg.c_str(), "Error", MB_ICONINFORMATION);
}

//WHS-1が引き抜かれたときに呼び出されるコールバック関数
void disconnected(void* _p)
{
  CWHS1SampleDlg* dlg = reinterpret_cast<CWHS1SampleDlg*>(_p);
  dlg->staticStatus_.SetWindowText("WHS-1が抜き取られました");
  UTWSCloseDevice(dlg->hwhs_);
  dlg->hwhs_ = 0;
}

//以下の関数を実行するスレッドオブジェクト
std::thread download_thread;
//非同期フラッシュ読み出し時の、読み出しブロック数表示
void monitor(CWHS1SampleDlg* _p)
{
  using namespace std;

  this_thread::sleep_for(chrono::microseconds(100));
  while (true) {
    UINT n, d;
    if (UTWSWHS1ReadSize(_p->hwhs_, &n, &d)) {
      stringstream s;
      s << n << " / " << d;
      _p->staticStatus_.SetWindowText(s.str().c_str());
    }
    if (WAIT_OBJECT_0 == WaitForSingleObject(_p->download_event_, 20)) {
      break;
    }
  }
}

//ファイルを保存したときtrue
bool saveData(CWHS1SampleDlg& _w)
{
  using namespace std;

  UINT buf_size;
  if (!UTWSWHS1GetMemoryBinarySize(_w.hwhs_, &buf_size)) {
    string msg("読み取ったメモリのサイズを取得できません");
    errorMessage(_w.hwhs_, &msg);
    _w.staticStatus_.SetWindowText(msg.c_str());
    return false;
  }

  vector<BYTE> bd(buf_size);
  if (!UTWSWHS1GetMemoryBinary(_w.hwhs_, &bd[0], bd.size())) {
    string msg("読み取ったメモリをメイン側へコピーできませんでした");
    errorMessage(_w.hwhs_, &msg);
    _w.staticStatus_.SetWindowText(msg.c_str());
    return false;
  }

  //時刻補完を有効にする
  BOOL time_adjust = TRUE;

  //バイナリデータを操作するため、ハンドルを生成する
  HANDLE hbin;
  if (!(hbin = UTWSOpenDevice(UTWS_WHS_1_BINARYDATA, 0))) {
    string msg("バイナリデータを操作するためのハンドルを生成できません。");
    errorMessage(0, &msg);
    _w.staticStatus_.SetWindowText(msg.c_str());
    return false;
  }
  if (-1 == UTWSWHS1BinaryToData(hbin, &bd[0], bd.size(), time_adjust, FALSE)) {
    string msg("バイナリデータを生体情報に変換できません。");
    errorMessage(hbin, &msg);
    _w.staticStatus_.SetWindowText(msg.c_str());
    return false;
  }

  CFileDialog fdg(FALSE);
  INT_PTR res = fdg.DoModal();
  if (IDOK == res) {
    string path = static_cast<const char*>(fdg.GetPathName());
    //生体情報に変換されたバイナリデータをファイルへCSVファイルとして保存する.
    if (!UTWSWHS1SaveDataFromBinary(hbin, path.c_str())) {
      string msg("CSVファイルを保存できませんでした。");
      errorMessage(hbin, &msg);
      _w.staticStatus_.SetWindowText(msg.c_str());
      return false;
    }
    //バイナリファイルとして保存する
    path += ".bin";
    if (!UTWSWHS1SaveMemoryBinary(_w.hwhs_, path.c_str())) {
      string msg("バイナリファイルを保存できませんでした。");
      errorMessage(_w.hwhs_, &msg);
      _w.staticStatus_.SetWindowText(msg.c_str());
      return false;
    }
  }
  
  return true;
}

//非同期読み出し完了時のコールバック
void asynchronousReadCallback(void* _p)
{
  using namespace std;

  CWHS1SampleDlg* p = reinterpret_cast<CWHS1SampleDlg*>(_p);
  if (download_thread.joinable())
    download_thread.join();

  string msg;
  switch (UTWSWHS1ReadMemoryBinaryResult(p->hwhs_)) {
  case UTWS_NO_ERR:
    //データのセーブ
    if (saveData(*p))
      p->staticStatus_.SetWindowText("ファイルを保存しました");
    else
      p->staticStatus_.SetWindowText("ファイルは保存されていません");
    return;
    break;
  case UTWS_ERR_FAILED:
    msg = "失敗しました";
    break;
  case UTWS_ERR_TIMEOUT:
    msg = "タイムアウトです";
    break;
  case UTWS_ERR_INVALID_HANDLE:
    msg = "ハンドルが無効です";
    break;
  default:
    msg = "原因不明のエラーです";
    break;
  }
  errorMessage(p->hwhs_, &msg);
  p->staticStatus_.SetWindowText(msg.c_str());
}

// CWHS1SampleDlg ダイアログ

CWHS1SampleDlg::CWHS1SampleDlg(CWnd* pParent /*=nullptr*/)
  : hwhs_(0)
  , download_event_(TRUE, TRUE)
  , items_(0)
  , editTempID_()
  , radioRF_()
  , radioMemory_()
  , radioWave_()
  , radioRRI_()
  , radioHR_()
  , chkUpdateWithBeat_()
  , chkMonitor_()
  , radioAccPeakHold_()
  , radioACCAverage_()
  , radioOverWrite_()
  , radioMemoryStop_()
  , editFWVersion_()
  , staticTempOffset_()
  , editFlushWriteCount_()
  , editSerialNo_()
  , editCPUID_()
  , editBlockSize_()
  , staticStatus_()
  , groupACC_()
  , groupMemory_()
  , staticTime_()
  , editDestinationAddress_()
  , editTempOffset_()
  , CDialog(IDD_WHS1SAMPLE_DIALOG, pParent)
{
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
  //全ダイアログアイテムの登録
  items_.push_back(&editTempID_);
  items_.push_back(&radioRF_);
  items_.push_back(&radioMemory_);
  items_.push_back(&radioWave_);
  items_.push_back(&radioRRI_);
  items_.push_back(&radioHR_);
  items_.push_back(&chkUpdateWithBeat_);
  items_.push_back(&chkMonitor_);
  items_.push_back(&radioAccPeakHold_);
  items_.push_back(&radioACCAverage_);
  items_.push_back(&radioOverWrite_);
  items_.push_back(&radioMemoryStop_);
  items_.push_back(&editFWVersion_);
  items_.push_back(&staticTempOffset_);
  items_.push_back(&editFlushWriteCount_);
  items_.push_back(&editSerialNo_);
  items_.push_back(&editCPUID_);
  items_.push_back(&editBlockSize_);
  items_.push_back(&staticStatus_);
  items_.push_back(&groupACC_);
  items_.push_back(&groupMemory_);
  items_.push_back(&staticTime_);
  items_.push_back(&editDestinationAddress_);
  items_.push_back(&editTempOffset_);
}

void CWHS1SampleDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_EDIT_TEMPID, editTempID_);
  DDX_Control(pDX, IDC_RADIO_RF, radioRF_);
  DDX_Control(pDX, IDC_RADIO_MEMORY, radioMemory_);
  DDX_Control(pDX, IDC_RADIO_WAVE, radioWave_);
  DDX_Control(pDX, IDC_RADIO_RRI, radioRRI_);
  DDX_Control(pDX, IDC_RADIO_HR, radioHR_);
  DDX_Control(pDX, IDC_CHECK_UPDATE_WITH_BEAT, chkUpdateWithBeat_);
  DDX_Control(pDX, IDC_CHECK_MONITOR, chkMonitor_);
  DDX_Control(pDX, IDC_RADIO_ACC_PEAK_HOLD, radioAccPeakHold_);
  DDX_Control(pDX, IDC_RADIO_ACC_AVERAGE, radioACCAverage_);
  DDX_Control(pDX, IDC_RADIO_OVER_WRITE, radioOverWrite_);
  DDX_Control(pDX, IDC_RADIO_MEMORY_STOP, radioMemoryStop_);
  DDX_Control(pDX, IDC_EDIT_FWVERSION, editFWVersion_);
  DDX_Control(pDX, IDC_STATIC_TEMP_OFFSET, staticTempOffset_);
  DDX_Control(pDX, IDC_EDIT_FLUSH_WRITE_COUNT, editFlushWriteCount_);
  DDX_Control(pDX, IDC_EDIT_SERIAL_NO, editSerialNo_);
  DDX_Control(pDX, IDC_EDIT_CPU_ID, editCPUID_);
  DDX_Control(pDX, IDC_EDIT_BLOCK_SIZE, editBlockSize_);
  DDX_Control(pDX, IDC_STATIC_STATUS, staticStatus_);
  DDX_Control(pDX, IDC_STATIC_ACC_GROUP, groupACC_);
  DDX_Control(pDX, IDC_STATIC_MEMORY_GROUP, groupMemory_);
  DDX_Control(pDX, IDC_STATIC_TIME, staticTime_);
  DDX_Control(pDX, IDC_EDIT_SEND_ADDRESS, editDestinationAddress_);
  DDX_Control(pDX, IDC_EDIT_TEMP_OFFSET, editTempOffset_);
}

BEGIN_MESSAGE_MAP(CWHS1SampleDlg, CDialog)
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_WM_CLOSE()
  ON_BN_CLICKED(IDC_BUTTON_READ, &CWHS1SampleDlg::OnBnClickedButtonRead)
  ON_BN_CLICKED(IDC_BUTTON_WRITE, &CWHS1SampleDlg::OnBnClickedButtonWrite)
  ON_BN_CLICKED(IDC_BUTTON_MEMORY, &CWHS1SampleDlg::OnBnClickedButtonMemory)
  ON_BN_CLICKED(IDC_BUTTON_ASYNC_READ, &CWHS1SampleDlg::OnBnClickedButtonAsyncRead)
  ON_BN_CLICKED(IDC_BUTTON_BINTOCSV, &CWHS1SampleDlg::OnBnClickedButtonBintocsv)
  ON_BN_CLICKED(IDC_BUTTON_SEND_ADDRES, &CWHS1SampleDlg::OnBnClickedButtonSendAddres)
END_MESSAGE_MAP()

// CWHS1SampleDlg メッセージ ハンドラー

BOOL CWHS1SampleDlg::OnInitDialog()
{
  using namespace std;

  CDialog::OnInitDialog();

  // このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
  //  Framework は、この設定を自動的に行います。
  SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
  SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

  // 時計の開始
  timeUpdate_thread = thread(timeUpdate, &staticTime_);

  //接続数を調べる タイムアウト1秒
  UINT count = UTWSWHS1CountConnected(1, 1000);
  if (!count) {
    staticStatus_.SetWindowText("WHS-1が接続されていません．");
    return TRUE;
  }
  else if (-1 == count) {
    string msg("接続時のエラー．");
    errorMessage(0, &msg);
    return TRUE;
  }

  //1台目を開く
  hwhs_ = UTWSOpenDevice(UTWS_WHS_1, 0);
  if (!hwhs_) {
    string msg("開けませんでした.");
    errorMessage(0, &msg);
    staticStatus_.SetWindowText(msg.c_str());
    return TRUE;
  }
  else {
    //設定情報の表示
    updateConfig();
    //WHS-1抜き取りを検出するコールバック関数登録
    if (!UTWSWHS1SetCallBackDisconnect(hwhs_, disconnected, this)) {
      string msg("コールバック関数を登録できませんでした．");
      errorMessage(hwhs_, &msg);
      return TRUE;
    }
  }

  return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CWHS1SampleDlg::OnPaint()
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
    CDialog::OnPaint();
  }
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CWHS1SampleDlg::OnQueryDragIcon()
{
  return static_cast<HCURSOR>(m_hIcon);
}

void CWHS1SampleDlg::OnClose()
{
  //非同期読み込み中ならばこの完了を待ちます
  // (ここでは記述しませんがしていませんが）

  //時計表示の終了
  if (timeUpdate_thread.joinable()) {
    timeUpdate_mutex.SetEvent();
    timeUpdate_thread.join();
  }

  //非同期読み込みモニターが動作していれば完了を待つ
  if (download_thread.joinable())
    download_thread.join();
  //WHS-1をクローズする
  UTWSCloseDevice(hwhs_);

  CDialog::OnClose();
}

//設定情報の更新
void CWHS1SampleDlg::updateConfig()
{
  using namespace std;

  //WHS-1のバージョンチェック
  //バージョンに応じてダイアログアイテムをEnableする．
  UINT ver;
  if (!UTWSWHS1Version(hwhs_, &ver)) {
    string msg("WHS-1の設定情報を取得できませんでした");
    errorMessage(hwhs_, &msg);
    staticStatus_.SetWindowText(msg.c_str());
    return;
  }
  editFWVersion_.SetWindowText(to_string(ver).c_str());
  //いったん全てのダイアログアイテムをEnableにする
  for_each(items_.begin(), items_.end(), [](CWnd* p) {p->EnableWindow(); });
  if (ver < 500 /*旧WHS-1*/) {
    chkMonitor_.EnableWindow(FALSE);
  }
  else {//新WHS-1
    radioHR_.EnableWindow(FALSE);
    groupACC_.EnableWindow(FALSE);
    radioACCAverage_.EnableWindow(FALSE);
    radioAccPeakHold_.EnableWindow(FALSE);
    groupMemory_.EnableWindow(FALSE);
    radioOverWrite_.EnableWindow(FALSE);
    radioMemoryStop_.EnableWindow(FALSE);
    staticTempOffset_.EnableWindow(FALSE);
    editTempID_.EnableWindow(FALSE);
  }

  //情報取得
  WHS1Config cnf;
  if (!UTWSWHS1ReadConfig(hwhs_, &cnf)) {
    string msg("WHS-1の設定情報を取得できませんでした");
    errorMessage(hwhs_, &msg);
    staticStatus_.SetWindowText(msg.c_str());
    return;
  }

  //情報の表示
  editTempID_.SetWindowText(to_string(cnf.temp_id).c_str());
  //0 : 無線モード 1: メモリモード
  if (cnf.mode == 0) {
    radioRF_.SetCheck(BST_CHECKED);
    radioMemory_.SetCheck(BST_UNCHECKED);
  }
  else {
    radioRF_.SetCheck(BST_UNCHECKED);
    radioMemory_.SetCheck(BST_CHECKED);
  }
  //モニタモード
  chkMonitor_.SetCheck(cnf.monitor_mode == 1 ? BST_CHECKED : BST_UNCHECKED);

  //1 : 心拍波形 2: 心拍間隔 1000Hz 3: 心拍間隔 (加速度1秒更新) 4: 心拍数データ
  radioWave_.SetCheck(BST_UNCHECKED);
  radioRRI_.SetCheck(BST_UNCHECKED);
  radioHR_.SetCheck(BST_UNCHECKED);
  chkUpdateWithBeat_.SetCheck(BST_UNCHECKED);
  switch (cnf.ecg_mode) {
  case 0x01:
    radioWave_.SetCheck(BST_CHECKED);
    break;
  case 0x02:
    radioRRI_.SetCheck(BST_CHECKED);
    break;
  case 0x03:
    radioRRI_.SetCheck(BST_CHECKED);
    chkUpdateWithBeat_.SetCheck(BST_CHECKED);
    break;
  case 0x04:
    radioHR_.SetCheck(BST_CHECKED);
    break;
  default:
    break;
  }

  //加速度モード  0:移動平均 1: ピークホールド
  if (cnf.acc_mode == 0) {
    radioACCAverage_.SetCheck(BST_CHECKED);
    radioAccPeakHold_.SetCheck(BST_UNCHECKED);
  }
  else {
    radioACCAverage_.SetCheck(BST_UNCHECKED);
    radioAccPeakHold_.SetCheck(BST_CHECKED);
  }
  //メモリ書き込み方式 0:上書き 1:停止
  if (cnf.mem_mode == 0) {
    radioOverWrite_.SetCheck(BST_CHECKED);
    radioMemoryStop_.SetCheck(BST_UNCHECKED);
  }
  else {
    radioOverWrite_.SetCheck(BST_UNCHECKED);
    radioMemoryStop_.SetCheck(BST_CHECKED);
  }
  editTempOffset_.SetWindowText(to_string(cnf.tmp_offset).c_str());
  editFlushWriteCount_.SetWindowTextA(to_string(cnf.flush_write_count).c_str());
  editSerialNo_.SetWindowText(to_string(cnf.set_serial_id).c_str());
  editCPUID_.SetWindowText(to_string(cnf.cpu_id).c_str());

  //フラッシュメモリに記録されたブロック数
  UINT block_size;
  if (!UTWSWHS1GetRecordedSize(hwhs_, &block_size)) {
    string msg("フラッシュメモリに記録されたブロック数を取得できませんでした");
    errorMessage(hwhs_, &msg);
    staticStatus_.SetWindowText(msg.c_str());
    return;
  }
  editBlockSize_.SetWindowText(to_string(block_size).c_str());

  CHAR address[11];
  if (!UTWSWHS1GetDestinationAddress(hwhs_, address)) {
    string msg("WHS-1の宛先アドレスを取得できませんでした");
    errorMessage(hwhs_, &msg);
    staticStatus_.SetWindowText(msg.c_str());
    return;
  }
  editDestinationAddress_.SetWindowText(address);

  staticStatus_.SetWindowText("");
}

void CWHS1SampleDlg::OnBnClickedButtonRead()
{
  updateConfig();
  staticStatus_.SetWindowText("設定を読み出しました");
}

//仮IDが正しいかどうかチェックする
bool examineTempID(CWnd& _wnd, int* _val)
{
  using namespace std;

  auto n = _wnd.GetWindowTextLength();
  vector<char> buf(n + 1);
  _wnd.GetWindowText(&buf[0], buf.size());

  for (auto p = buf.begin(), ep = buf.end(); p != ep - 1; ++p) {
    switch (*p) {
    case '0': case '1': case '2': case '3': case '4': case '5':
    case '6': case '7': case '8': case '9':
      continue;
    default:
      return false;
    }
  }

  *_val = atoi(&buf[0]);
  return  (0 <= *_val && 255 >= *_val);
}

void CWHS1SampleDlg::OnBnClickedButtonWrite()
{
  using namespace std;

  UINT ver;
  if (!UTWSWHS1Version(hwhs_, &ver)) {
    string msg("WHS-1のF/W Verを取得できません");
    errorMessage(hwhs_, &msg);
    staticStatus_.SetWindowText(msg.c_str());
    return;
  }

  WHS1Config cnf;
  ZeroMemory(&cnf, sizeof cnf);

  cnf.mode = (BST_CHECKED == radioRF_.GetCheck()) ? 0 : 1;
  if (BST_CHECKED == radioWave_.GetCheck()) {
    cnf.ecg_mode = 1;
  }
  else if (BST_CHECKED == radioRRI_.GetCheck()) {
    cnf.ecg_mode = (BST_CHECKED == chkUpdateWithBeat_.GetCheck()) ? 3 : 2;
  }
  else if (BST_CHECKED == radioHR_.GetCheck()) {
    cnf.ecg_mode = 4;
  }
  else {
    cnf.ecg_mode = 5;
  }
  if (ver >= 500 && BST_CHECKED == chkMonitor_.GetCheck()) {
    cnf.monitor_mode = 1;
  }
  if (ver < 500) {
    cnf.acc_mode = (BST_CHECKED == radioACCAverage_.GetCheck()) ? 0 : 1;
    cnf.mem_mode = (BST_CHECKED == radioOverWrite_.GetCheck()) ? 0 : 1;
  }
  else {
    cnf.acc_mode = 1;
    cnf.mem_mode = 1;
  }

  int tempID;
  if (!examineTempID(editTempID_, &tempID)) {
    string msg("仮IDが間違っています");
    MessageBox(msg.c_str());
    staticStatus_.SetWindowText(msg.c_str());
    return;
  }
  cnf.temp_id = tempID;

  auto tp = chrono::system_clock::now();
  time_t tt = chrono::system_clock::to_time_t(tp);
  tm t;
  localtime_s(&t, &tt);

  if (!UTWSWHS1WriteConfig(hwhs_, &cnf, cnf.mem_mode,
    t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec)) {
    string msg("設定できませんでした");
    errorMessage(hwhs_, &msg);
    staticStatus_.SetWindowText(msg.c_str());
    return;
  }

  staticStatus_.SetWindowText("設定を書き込みました");

  return;
}

void CWHS1SampleDlg::OnBnClickedButtonMemory()
{
  using namespace std;

  //フラッシュメモリに記録されたブロック数
  UINT block_size;
  if (!UTWSWHS1GetRecordedSize(hwhs_, &block_size)) {
    string msg("フラッシュメモリに記録されたブロック数を取得できませんでした");
    errorMessage(hwhs_, &msg);
    staticStatus_.SetWindowText(msg.c_str());
    return;
  }
  if (block_size == 0) {
    string msg("WHS-1のフラッシュは空です");
    errorMessage(hwhs_, &msg);
    staticStatus_.SetWindowText(msg.c_str());
    return;
  }

  //時刻補完を有効にする
  BOOL time_adjust = TRUE;

  WHS1MemDataHeader header;

  if (FALSE == UTWSWHS1ReadMemoryEx(hwhs_, &header)) {
    string msg("WHS-1からデータを読み出せません");
    errorMessage(hwhs_, &msg);
    staticStatus_.SetWindowText(msg.c_str());
    return;
  }
  else if (0 == header.data_count) {
    string msg("保存されたデータはありませんでした");
    errorMessage(hwhs_, &msg);
    staticStatus_.SetWindowText(msg.c_str());
    return;
  }

  //呼び出し側メモリへデータをコピーする例
  if (3 == header.ecg_mode && 0 == header.acc_mode) {
    //加速度1秒更新かつピークホールド
    vector<WHS1EcgDataEx> data(header.data_count);
    if (!UTWSWHS1GetReceivedMemoryEx(hwhs_, &header, &data[0], data.size(), time_adjust)) {
      string msg("DDL内ストアデータコピーに失敗しました．");
      errorMessage(hwhs_, &msg);
      staticStatus_.SetWindowText(msg.c_str());
      return;
    }
  }
  else {
    vector<WHS1EcgData> data(header.data_count);
    if (!UTWSWHS1GetReceivedMemory(hwhs_, &header, &data[0], data.size(), time_adjust)) {
      string msg("DDL内ストアデータコピーに失敗しました．");
      errorMessage(hwhs_, &msg);
      staticStatus_.SetWindowText(msg.c_str());
      return;
    }
  }

  //ファイルへ保存
  CFileDialog fdg(FALSE);
  if (IDOK == fdg.DoModal()) {
    if (!UTWSWHS1SaveReceivedMemory(hwhs_, static_cast<const char*>(fdg.GetPathName()), time_adjust)) {
      string msg("ファイルの保存に失敗しました．");
      errorMessage(hwhs_, &msg);
      staticStatus_.SetWindowText(msg.c_str());
      return;
    }
    else {
      staticStatus_.SetWindowText("保存しました");
    }
  }
  else {
    staticStatus_.SetWindowText("");
  }
}

void CWHS1SampleDlg::OnBnClickedButtonAsyncRead()
{
  using namespace std;

  download_event_.ResetEvent();
  if (!UTWSWHS1ReadMemoryBinary(hwhs_, download_event_, asynchronousReadCallback, this)) {
    string msg("非同期読み出しを開始できません");
    errorMessage(hwhs_, &msg);
    staticStatus_.SetWindowText(msg.c_str());
    return;
  }
  download_thread = thread(monitor, this);
}

void CWHS1SampleDlg::OnBnClickedButtonBintocsv()
{
  using namespace std;

  HANDLE hbin = 0;
  //時刻補完を有効にする
  BOOL time_adjust = TRUE;
  //バイナリデータへのハンドルを生成する
  if (!(hbin = UTWSOpenDevice(UTWS_WHS_1_BINARYDATA, 0))) {
    string msg("バイナリデータのハンドルを生成できませんでした");
    errorMessage(0, &msg);
    staticStatus_.SetWindowText(msg.c_str());
    return;
  }

  //バイナリファイルの読み込み
  //DLL内でデータに変換する
  CFileDialog fdgr(TRUE);
  if (IDOK == fdgr.DoModal()) {
    string path = static_cast<const char*>(fdgr.GetPathName());
    if (-1 == UTWSWHS1BinaryFileToData(hbin, path.c_str(), time_adjust, FALSE)) {
      string msg("バイナリデータを読み込めませんでした");
      errorMessage(hbin, &msg);
      staticStatus_.SetWindowText(msg.c_str());
      UTWSCloseDevice(hbin);
      return;
    }
  }
  else {
    staticStatus_.SetWindowText("キャンセルされました");
    return;
  }
  //CSVへ書き出す
  CFileDialog fdg(FALSE);
  if (IDOK == fdg.DoModal()) {
    string path = static_cast<const char*>(fdg.GetPathName());
    if (!UTWSWHS1SaveDataFromBinary(hbin, path.c_str())) {
      string msg("CSVファイルを保存できませんでした");
      errorMessage(hbin, &msg);
      staticStatus_.SetWindowText(msg.c_str());
      UTWSCloseDevice(hbin);
      return;
    }
  }
  else {
    staticStatus_.SetWindowText("キャンセルされました");
    return;
  }

  staticStatus_.SetWindowTextA("変換しました");
  UTWSCloseDevice(hbin);
}

void CWHS1SampleDlg::OnBnClickedButtonSendAddres()
{
  using namespace std;

  auto n = editDestinationAddress_.GetWindowTextLength();
  vector<CHAR> buf(n + 1);
  editDestinationAddress_.GetWindowText(&buf[0], buf.size());
  if (!UTWSWHS1SetDestinationAddress(hwhs_, &buf[0])) {
    string msg("宛先アドレスを設定できませんでした");
    errorMessage(hwhs_, &msg);
    staticStatus_.SetWindowText(msg.c_str());
    return;
  }

  CHAR address[11];
  if (!UTWSWHS1GetDestinationAddress(hwhs_, address)) {
    string msg("WHS-1の宛先アドレスを取得できませんでした");
    errorMessage(hwhs_, &msg);
    staticStatus_.SetWindowText(msg.c_str());
    return;
  }
  editDestinationAddress_.SetWindowText(address);

  staticStatus_.SetWindowText("宛先アドレスを設定しました");
}
