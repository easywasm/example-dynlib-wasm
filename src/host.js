// this is a nodejs-based host tester

import { readFile } from 'node:fs/promises'

import MemoryManager from './MemoryManager.js'

export const env = {
  memory: new WebAssembly.Memory({ initial: 2 }),
  __indirect_function_table: new WebAssembly.Table({
    initial: 10,
    element: 'anyfunc'
  }),
  __stack_pointer: new WebAssembly.Global({ value: 'i32', mutable: true }, 65536),
  __memory_base: new WebAssembly.Global({ value: 'i32', mutable: false }, 8192),
  __table_base: new WebAssembly.Global({ value: 'i32', mutable: false }, 0),
  malloc: (size) => memoryManager.malloc(size),
  free: (ptr) => memoryManager.free(ptr),
  console_log: (ptr) => {
    console.log(copyStringFromMemory(ptr))
  }
}
const memoryManager = new MemoryManager(8192, env)

export async function loadModules(...names) {
  const bytes = await Promise.all(names.map((f) => readFile(f)))
  return Promise.all(
    bytes.map(async (b) => {
      const i = await WebAssembly.instantiate(b, { env })
      return i.instance.exports
    })
  )
}

export const malloc = env.malloc
export const free = env.free
export const memory = env.memory

const decoder = new TextDecoder()
const encoder = new TextEncoder()

export function copyStringToMemory(str) {
  const bytes = encoder.encode(str)
  const ptr = env.malloc(bytes.length + 1)
  const mem = new Uint8Array(env.memory.buffer)
  mem.set(bytes, ptr)
  mem[ptr + bytes.length] = 0
  return ptr
}

export function copyStringFromMemory(ptr) {
  const mem = new Uint8Array(env.memory.buffer)
  const bytes = mem.slice(ptr)
  const end = bytes.indexOf(0)
  return decoder.decode(bytes.slice(0, end))
}
