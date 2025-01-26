'use client'

import React, { useRef, useMemo } from 'react'
import { Canvas, useFrame } from '@react-three/fiber'
import { Sphere, OrbitControls } from '@react-three/drei'
import * as THREE from 'three'

function GlobeMesh({ size }) {
  const meshRef = useRef(null)
  const textureRef = useRef(null)

  const canvasTexture = useMemo(() => {
    const canvas = document.createElement('canvas')
    canvas.width = 2048
    canvas.height = 1024
    const ctx = canvas.getContext('2d')
    textureRef.current = new THREE.CanvasTexture(canvas)
    textureRef.current.needsUpdate = true
    return { canvas, ctx }
  }, [])

  const updateTexture = () => {
    const { ctx, canvas } = canvasTexture

    ctx.fillStyle = 'rgba(255, 255, 255, 0.05)'
    ctx.fillRect(0, 0, canvas.width, canvas.height)
    
    const chars = 'abcdefghijklmnopqrstuvwxyz';
    for (let i = 0; i < 100; i++) {
      const char = chars.charAt(
        Math.floor(Math.random() * chars.length)
      )
      const x = Math.random() * canvas.width
      const y = Math.random() * canvas.height
      ctx.fillStyle = `rgba(0, 0, 0, ${Math.random() * 0.5 + 0.5})`
      ctx.font = '32px Geist'
      ctx.fillText(char, x, y)
    }

    textureRef.current.needsUpdate = true
  }

  useFrame(() => {
    updateTexture()
    if (meshRef.current) {
      meshRef.current.rotation.y += 0.001
    }
  })

  return (
    <Sphere args={[size, 64, 64]} ref={meshRef}>
      <meshBasicMaterial map={textureRef.current} transparent={true} />
    </Sphere>
  )
}

export default function Globe({ size=220 }) {
  return (
    <div style={{ height: `${size}px`, width: `${size}px`, backgroundColor: 'white' }}>
      <Canvas camera={{ position: [0, 0, 1.7 * size] }}>
        <GlobeMesh size={size} />
        <OrbitControls enableZoom={false} />
      </Canvas>
    </div>
  )
}
