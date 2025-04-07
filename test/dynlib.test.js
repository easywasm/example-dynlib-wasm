import { test } from 'node:test'

import { loadModules, copyStringToMemory, copyStringFromMemory, malloc, free, copyToMemory, copyFromMemory } from '../src/host.js'

// build these with `make`
const [lib1, lib2, lib3, lib4] = await loadModules('test/lib1.wasm', 'test/lib2.wasm', 'test/lib3.wasm', 'test/lib4.wasm')

// this just prints "Hello"
// lib1.example_log_function()

test('should be able to create some memory with no wasm', ({ assert }) => {
  const buffer = new Uint8Array(5)
  buffer[1] = 1
  buffer[2] = 2
  buffer[3] = 3
  buffer[4] = 4
  const memPtr = copyToMemory(buffer)
  assert.deepEqual(copyFromMemory(memPtr, 5), buffer)
  free(memPtr)
})

test('should be able to create a string with no wasm', ({ assert }) => {
  const strPtr = copyStringToMemory('test0')
  assert.equal(copyStringFromMemory(strPtr), 'test0')
  free(strPtr)
})

test('lib1 should be able to use wasi functions', ({ assert }) => {
  const r = lib1.example_random()
  const d = lib1.example_time()
  assert.notEqual(r, 0)
  assert.equal(typeof d, 'bigint')
  assert.notEqual(d, 0n)
})

test('should be able to create a struct/string in lib1', ({ assert }) => {
  const namePtr = copyStringToMemory('test1')
  const structPtr = lib1.create_struct(42, namePtr)

  assert.equal(namePtr, 8192)
  assert.equal(structPtr, 8200) // 5 bytes is aligned to 8
  assert.equal(copyStringFromMemory(namePtr), 'test1')

  // 42/"test1\0"
  assert.deepEqual(copyFromMemory(structPtr, 10), new Uint8Array([42, 0, 0, 0, 116, 101, 115, 116, 49, 0]))

  free(namePtr)
  free(structPtr)
})

test('should be able to call a function in lib2 that uses struct from lib1', ({ assert }) => {
  const namePtr = copyStringToMemory('test2')
  const structPtr = lib1.create_struct(42, namePtr)
  lib2.process_struct(structPtr)

  assert.equal(lib1.get_x(structPtr), 84)

  free(namePtr)
  free(structPtr)
})

test('should be able create a struct and modify from lib3 (wasi)', ({ assert }) => {
  const namePtr = copyStringToMemory('test3')
  const structPtr = lib1.create_struct(42, namePtr)
  const outPtr = malloc(100)
  lib3.example_mod_string(structPtr, outPtr)

  assert.equal(copyStringFromMemory(outPtr), 'Hello test3')

  free(namePtr)
  free(structPtr)
  free(outPtr)
})

test('should be able create a struct and modify from lib4 (wasi)', ({ assert }) => {
  const namePtr = copyStringToMemory('test4')
  const structPtr = lib1.create_struct(42, namePtr)
  const outPtr = malloc(100)
  lib4.example_mod_string(structPtr, outPtr)

  assert.equal(copyStringFromMemory(outPtr), 'Hello test4')

  free(namePtr)
  free(structPtr)
  free(outPtr)
})
