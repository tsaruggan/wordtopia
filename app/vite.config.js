import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'
import vercel from 'vite-plugin-vercel';

// https://vite.dev/config/
export default defineConfig({
  plugins: [react(), vercel()],
  server: {
    port: process.env.PORT,
    // port: 3000,
    // proxy: { // Proxy requests to local server
    //   '/suggest': 'http://localhost:8080', 
    //   '/search': 'http://localhost:8080',
    // },
    // proxy: {
    //   "/api": {
    //     target: "/api",
    //     changeOrigin: true,
    //     secure: false,
    //   },
    // },
  },
})
