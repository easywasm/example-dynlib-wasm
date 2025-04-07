import { test } from 'node:test'

import { loadModules, copyStringToMemory, copyStringFromMemory, malloc, free } from '../src/host.js'

// build these with `make`
const [lib1, lib2, lib3, lib4] = await loadModules('test/lib1.wasm', 'test/lib2.wasm', 'test/lib3.wasm', 'test/lib4.wasm')

// this just prints "Hello"
// lib1.example_log_function()

// these tests don't even use any wasm

test('should be able to create a string with no wasm', ({ assert }) => {
  const namePtr = copyStringToMemory('test0')
  assert.equal(copyStringFromMemory(namePtr), 'test0')
  free(namePtr)
})

// here we test passing things around between wasm

test('should be able to create a struct/string in lib1', ({ assert }) => {
  const namePtr = copyStringToMemory('test1')
  const structPtr = lib1.create_struct(42, namePtr)

  assert.equal(namePtr, 8192)
  assert.equal(structPtr, 8200) // 5 bytes is aligned to 8
  assert.equal(copyStringFromMemory(namePtr), 'test1')

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
