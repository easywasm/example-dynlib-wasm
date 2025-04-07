// this is a nodejs-based host tester

import { readFile } from 'node:fs/promises'
import WasiPreview1 from 'easywasi'

import MemoryManager from './MemoryManager.js'

export const env = {
  memory: new WebAssembly.Memory({ initial: 2 }),
  __indirect_function_table: new WebAssembly.Table({
    initial: 2,
    element: 'anyfunc'
  }),
  __stack_pointer: new WebAssembly.Global({ value: 'i32', mutable: true }, 65536),
  __memory_base: new WebAssembly.Global({ value: 'i32', mutable: false }, 8192),
  __table_base: new WebAssembly.Global({ value: 'i32', mutable: false }, 0),

  malloc: (size) => memoryManager.malloc(size),
  free: (ptr) => memoryManager.free(ptr),

  console_log(ptr) {
    console.log(copyStringFromMemory(ptr))
  }
}
const memoryManager = new MemoryManager(8192, env)

// this is used for things that import wasi
const wasi_snapshot_preview1 = new WasiPreview1()

export async function loadModules(...names) {
  const bytes = await Promise.all(names.map((f) => readFile(f)))

  wasi_snapshot_preview1.setup({ memory: env.memory })

  return Promise.all(
    bytes.map(async (b) => {
      const i = await WebAssembly.instantiate(b, { env, wasi_snapshot_preview1 })
      if (i.instance.exports._initialize) {
        i.instance.exports._initialize()
      }
      if (i.instance.exports._start) {
        i.instance.exports._start()
      }
      return i.instance.exports
    })
  )
}

export const malloc = env.malloc
export const free = env.free
export const memory = env.memory

const decoder = new TextDecoder()
const encoder = new TextEncoder()

// send a string from host to wasm, ptr is optional
export function copyStringToMemory(str, ptr) {
  const bytes = encoder.encode(str)
  ptr ||= env.malloc(bytes.length + 1)
  const mem = new Uint8Array(env.memory.buffer)
  mem.set(bytes, ptr)
  mem[ptr + bytes.length] = 0
  return ptr
}

// get a string from wasm to host
export function copyStringFromMemory(ptr) {
  const mem = new Uint8Array(env.memory.buffer)
  const bytes = mem.slice(ptr)
  const end = bytes.indexOf(0)
  return decoder.decode(bytes.slice(0, end))
}

// copy memory (Uint8Array) from host to wasm, ptr is optional
export function copyToMemory(bytes, ptr) {
  ptr ||= env.malloc(bytes.length)
  const mem = new Uint8Array(env.memory.buffer)
  mem.set(bytes, ptr)
  return ptr
}

// copy memory (Uint8Array) to host
export function copyFromMemory(ptr, len) {
  const mem = new Uint8Array(env.memory.buffer)
  const bytes = mem.slice(ptr)
  return bytes.slice(0, len)
}
