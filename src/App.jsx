import React, { useEffect, useState, useRef } from 'react';
import {
  LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer
} from 'recharts';

// 表示期間（秒）
const DISPLAY_SECONDS = 60;

function App() {
  const [data, setData] = useState([]);           // 全データ保持（最大300件）
  
  // アラート条件
  const [hrThreshold, setHrThreshold] = useState(100);
  const [accThreshold, setAccThreshold] = useState(1.0);
  const [duration, setDuration] = useState(3);      // 条件継続秒数
  
  // 表示開始時刻（Unix秒：ライブ/手動モードで利用）
  const [startTime, setStartTime] = useState(0);
  const [liveMode, setLiveMode] = useState(true);   // ライブモードなら常に最新60秒を表示
  
  // アラート履歴（連続するアラートは 1 件にまとめる）
  // 各要素は { start: globalIdx, end: globalIdx }
  const [alertRanges, setAlertRanges] = useState([]);
  const [alertActive, setAlertActive] = useState(false);
  
  const audioRef = useRef(null);
  
  // リアルタイムデータ生成用の参照（滑らかな変動用）
  const hrRef = useRef(90);
  const accRef = useRef(1.0);
  
  // 新しいデータを1秒ごとに追加（timeValue はUnix秒）
  useEffect(() => {
    const interval = setInterval(() => {
      let hr = hrRef.current + (Math.random() * 10 - 5);
      hr = Math.max(60, Math.min(130, hr));
      hrRef.current = hr;
      
      let acc = accRef.current + (Math.random() * 0.6 - 0.3);
      if (Math.random() < 0.1) acc = 0;
      acc = Math.max(0, Math.min(2.0, acc));
      accRef.current = acc;
      
      const now = new Date();
      const timeValue = now.getTime() / 1000;
      const displayTime = now.toLocaleTimeString();
      
      setData(prev => {
        const newData = [...prev, {
          globalIdx: prev.length,
          timeValue,
          displayTime,
          heartRate: Math.round(hr),
          acceleration: parseFloat(acc.toFixed(2))
        }];
        return newData.slice(-300);
      });
    }, 1000);
    
    return () => clearInterval(interval);
  }, []);
  
  // アラート判定＆履歴更新（直近duration秒間のデータが条件を満たすか）
  useEffect(() => {
    if (data.length < duration) return;
    
    const i = data.length - 1;
    const slice = data.slice(i - duration + 1, i + 1);
    const hrHigh = slice.every(d => d.heartRate > hrThreshold);
    const accLow = slice.every(d => d.acceleration <= accThreshold);
    const nowAlert = hrHigh && accLow;
    
    if (nowAlert && !alertActive) {
      setAlertRanges(prev => [...prev, { start: i, end: i }]);
      if (audioRef.current) audioRef.current.play();
    } else if (nowAlert && alertActive) {
      setAlertRanges(prev => {
        const updated = [...prev];
        updated[updated.length - 1].end = i;
        return updated;
      });
    }
    
    setAlertActive(nowAlert);
  }, [data, hrThreshold, accThreshold, duration]);
  
  // ライブモードの場合は表示ウィンドウの開始時刻を自動更新
  useEffect(() => {
    if (liveMode && data.length > 0) {
      const currentTime = data[data.length - 1].timeValue;
      setStartTime(currentTime - DISPLAY_SECONDS);
    }
  }, [data, liveMode]);
  
  // 表示ウィンドウのドメイン
  const xDomain = [startTime, startTime + DISPLAY_SECONDS];
  
  // 指定された時刻範囲に含まれるデータを抽出
  const displayData = data.filter(d => d.timeValue >= xDomain[0] && d.timeValue <= xDomain[1]);
  
  // スライダー用：最小はデータ最初のtimeValue、最大は (最新のtimeValue - DISPLAY_SECONDS)
  const sliderMin = data.length > 0 ? data[0].timeValue : 0;
  const sliderMax = data.length > 0 ? data[data.length - 1].timeValue - DISPLAY_SECONDS : 0;
  
  // 時刻表示変換（Unix秒 → hh:mm:ss）
  const formatTime = (timeValue) => {
    return new Date(timeValue * 1000).toLocaleTimeString();
  };
  
  // アラート部分の赤色オーバーレイ（heartRate）
  const alertLinesHR = alertRanges.map((range, idx) => {
    const startT = data[range.start]?.timeValue;
    const endT = data[range.end]?.timeValue;
    if (!startT || endT < xDomain[0] || startT > xDomain[1]) return null;
    const alertData = data.filter(d => d.timeValue >= startT && d.timeValue <= endT && d.timeValue >= xDomain[0] && d.timeValue <= xDomain[1]);
    return alertData.length > 0 ? (
      <Line
        key={`alert-hr-${idx}`}
        yAxisId="left"
        type="monotone"
        data={alertData}
        dataKey="heartRate"
        stroke="red"
        strokeWidth={4}
        dot={false}
        isAnimationActive={false}
        legendType="none"
      />
    ) : null;
  }).filter(line => line !== null);
  
  // アラート部分の赤色オーバーレイ（acceleration）
  const alertLinesAcc = alertRanges.map((range, idx) => {
    const startT = data[range.start]?.timeValue;
    const endT = data[range.end]?.timeValue;
    if (!startT || endT < xDomain[0] || startT > xDomain[1]) return null;
    const alertData = data.filter(d => d.timeValue >= startT && d.timeValue <= endT && d.timeValue >= xDomain[0] && d.timeValue <= xDomain[1]);
    return alertData.length > 0 ? (
      <Line
        key={`alert-acc-${idx}`}
        yAxisId="right"
        type="monotone"
        data={alertData}
        dataKey="acceleration"
        stroke="red"
        strokeWidth={4}
        dot={false}
        isAnimationActive={false}
        legendType="none"
      />
    ) : null;
  }).filter(line => line !== null);
  
  return (
    <div style={{ padding: '20px', fontFamily: 'sans-serif' }}>
      <h2>💓 心拍 & 加速度モニタリング</h2>
      
      {/* アラート条件設定 */}
      <div style={{ marginBottom: '10px' }}>
        <label>心拍しきい値：
          <input type="number" value={hrThreshold} onChange={e => setHrThreshold(Number(e.target.value))} />
        </label>
        <label style={{ marginLeft: '20px' }}>加速度しきい値：
          <input type="number" value={accThreshold} step="0.1" onChange={e => setAccThreshold(Number(e.target.value))} />
        </label>
        <label style={{ marginLeft: '20px' }}>継続秒数：
          <input type="number" value={duration} onChange={e => setDuration(Number(e.target.value))} />
        </label>
      </div>
      
      {/* ライブモード切替 */}
      <div style={{ marginBottom: '10px' }}>
        <label>
          <input type="checkbox" checked={liveMode} onChange={e => setLiveMode(e.target.checked)} />
          常に最新の{DISPLAY_SECONDS}秒を表示（ライブモード）
        </label>
      </div>
      
      {/* 手動モードの場合、スライダーで表示開始時刻を選択 */}
      {!liveMode && (
        <div style={{ marginBottom: '10px' }}>
          表示開始時刻：
          <input
            type="range"
            min={sliderMin}
            max={sliderMax}
            value={startTime}
            onChange={e => setStartTime(Number(e.target.value))}
            style={{ width: '60%' }}
          />
          <span style={{ marginLeft: '10px' }}>{formatTime(startTime)}</span>
        </div>
      )}
      
      {/* グラフ表示 */}
      <ResponsiveContainer width="100%" height={400}>
        <LineChart
          data={displayData}
          margin={{ top: 5, right: 20, bottom: 5, left: 0 }}
        >
          <CartesianGrid strokeDasharray="3 3" />
          <XAxis
            dataKey="timeValue"
            type="number"
            domain={xDomain}
            tickFormatter={formatTime}
            tick={{ fontSize: 10 }}
          />
          <YAxis yAxisId="left" domain={[30, 220]} />
          <YAxis yAxisId="right" orientation="right" domain={[0, 3]} />
          <Tooltip formatter={(value, name) => [value, name === 'heartRate' ? '心拍数' : '加速度']}
                   labelFormatter={timeVal => `時刻: ${formatTime(timeVal)}`} />
          <Legend />
          {/* 通常のライン */}
          <Line
            yAxisId="left"
            type="monotone"
            dataKey="heartRate"
            stroke="#8884d8"
            dot={false}
            isAnimationActive={false}
          />
          <Line
            yAxisId="right"
            type="monotone"
            dataKey="acceleration"
            stroke="#82ca9d"
            dot={false}
            isAnimationActive={false}
          />
          {/* アラート部分の赤色オーバーレイ（心拍・加速度） */}
          {alertLinesHR}
          {alertLinesAcc}
        </LineChart>
      </ResponsiveContainer>
      
      {/* アラート履歴表示 */}
      <div style={{ marginTop: '30px' }}>
        <h4>📜 アラート履歴</h4>
        {alertRanges.length === 0 ? (
          <p>アラートはまだ発生していません。</p>
        ) : (
          alertRanges.map((range, i) => (
            <div key={i} style={{
              backgroundColor: '#ffeaea',
              padding: '8px 12px',
              borderRadius: '8px',
              marginBottom: '6px',
              borderLeft: '5px solid red'
            }}>
              <strong>🚨 {data[range.start]?.displayTime} 〜 {data[range.end]?.displayTime}</strong>
              <br />
              心拍 > {hrThreshold} &amp; 加速度 ≤ {accThreshold} が {range.end - range.start + 1}秒継続
            </div>
          ))
        )}
      </div>
      
      <audio ref={audioRef} src="https://actions.google.com/sounds/v1/alarms/beep_short.ogg" preload="auto" />
    </div>
  );
}

export default App;
