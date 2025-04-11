import React, { useEffect, useState, useRef } from 'react';
import {
  LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer
} from 'recharts';

const DISPLAY_SECONDS = 60;

function App() {
  const [data, setData] = useState([]);
  const [hrThreshold, setHrThreshold] = useState(100);
  const [accThreshold, setAccThreshold] = useState(1.0);
  const [duration, setDuration] = useState(3);
  const [startTime, setStartTime] = useState(0);
  const [liveMode, setLiveMode] = useState(true);
  const [alertRanges, setAlertRanges] = useState([]);
  const [alertActive, setAlertActive] = useState(false);

  const audioRef = useRef(null);
  const hrRef = useRef(90);
  const accRef = useRef(1.0);
  // useRef を使って alertActive の即時参照を可能にする
  const alertActiveRef = useRef(false);

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

  useEffect(() => {
    if (data.length < duration) return;

    const i = data.length - 1;
    const slice = data.slice(i - duration + 1, i + 1);
    const hrHigh = slice.every(d => d.heartRate > hrThreshold);
    const accLow = slice.every(d => d.acceleration <= accThreshold);
    const nowAlert = hrHigh && accLow;

    // useRef を使って直近の alertActive 状態を即座に参照
    if (nowAlert && !alertActiveRef.current) {
      setAlertRanges(prev => [...prev, { start: i, end: i }]);
      if (audioRef.current) audioRef.current.play();
    } else if (nowAlert && alertActiveRef.current) {
      setAlertRanges(prev => {
        const updated = [...prev];
        updated[updated.length - 1].end = i;
        return updated;
      });
    }

    // useRef と state の両方を更新
    alertActiveRef.current = nowAlert;
    setAlertActive(nowAlert);
  }, [data, hrThreshold, accThreshold, duration]);

  useEffect(() => {
    if (liveMode && data.length > 0) {
      const currentTime = data[data.length - 1].timeValue;
      setStartTime(currentTime - DISPLAY_SECONDS);
    }
  }, [data, liveMode]);

  const xDomain = [startTime, startTime + DISPLAY_SECONDS];
  const displayData = data.filter(d => d.timeValue >= xDomain[0] && d.timeValue <= xDomain[1]);
  const sliderMin = data.length > 0 ? data[0].timeValue : 0;
  const sliderMax = data.length > 0 ? data[data.length - 1].timeValue - DISPLAY_SECONDS : 0;
  const formatTime = (timeValue) => new Date(timeValue * 1000).toLocaleTimeString();

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
        tooltipType="none"
      />
    ) : null;
  }).filter(line => line !== null);

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
        tooltipType="none"
      />
    ) : null;
  }).filter(line => line !== null);

  const latest = data.at(-1);

  return (
    <div style={{ padding: '20px', fontFamily: 'sans-serif' }}>
      <h2>💓 心拍 & 加速度モニタリング</h2>

      <div style={{ marginBottom: '10px' }}>
        <label>
          心拍しきい値：
          <input type="number" value={hrThreshold} onChange={e => setHrThreshold(Number(e.target.value))} />
        </label>
        <label style={{ marginLeft: '20px' }}>
          加速度しきい値：
          <input type="number" value={accThreshold} step="0.1" onChange={e => setAccThreshold(Number(e.target.value))} />
        </label>
        <label style={{ marginLeft: '20px' }}>
          継続秒数：
          <input type="number" value={duration} onChange={e => setDuration(Number(e.target.value))} />
        </label>
      </div>

      <div style={{ marginBottom: '10px' }}>
        <label>
          <input type="checkbox" checked={liveMode} onChange={e => setLiveMode(e.target.checked)} />
          常に最新の {DISPLAY_SECONDS} 秒を表示（ライブモード）
        </label>
      </div>

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
          <Tooltip content={() => null} cursor={false} />
          <Legend />
          <Line
            yAxisId="left"
            type="monotone"
            dataKey="heartRate"
            stroke="#8884d8"
            dot={{ r: 0 }}
            activeDot={false}
            isAnimationActive={false}
          />
          <Line
            yAxisId="right"
            type="monotone"
            dataKey="acceleration"
            stroke="#82ca9d"
            dot={{ r: 0 }}
            activeDot={false}
            isAnimationActive={false}
          />
          {alertLinesHR}
          {alertLinesAcc}
        </LineChart>
      </ResponsiveContainer>

      {latest && (
        <div style={{ marginTop: 10, padding: '6px 12px', background: '#f4f4f4', borderRadius: '6px' }}>
          <strong>📍 最新のデータ</strong><br />
          🕒 {latest.displayTime} &nbsp;&nbsp;
          💓 心拍数: {latest.heartRate} bpm &nbsp;&nbsp;
          🌀 加速度: {latest.acceleration} G
        </div>
      )}

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
              心拍 &gt; {hrThreshold} &amp; 加速度 ≤ {accThreshold} が {range.end - range.start + 1} 秒継続
            </div>
          ))
        )}
      </div>

      <audio ref={audioRef} src="https://actions.google.com/sounds/v1/alarms/beep_short.ogg" preload="auto" />
    </div>
  );
}

export default App;
