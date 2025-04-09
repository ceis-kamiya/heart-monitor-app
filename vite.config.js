// vite.config.js
import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'

// 👇 GitHub Pages で使うための「公開先パス」を設定
export default defineConfig({
  base: '/heart-monitor-app/',
  plugins: [react()],
})
