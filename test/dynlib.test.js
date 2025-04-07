import { test } from 'node:test'

import { loadModules, copyStringToMemory, copyStringFromMemory, malloc, free } from '../src/host.js'

// build these with `make`
const [lib1, lib2] = await loadModules('test/lib1.wasm', 'test/lib2.wasm')

// lib1.example_log_function();

test('should be able to create a struct/string in lib1', ({ assert }) => {
  const namePtr = copyStringToMemory('test')
  const structPtr = lib1.create_struct(42, namePtr)

  assert.equal(namePtr, 8192)
  assert.equal(structPtr, 8200) // 4 bytes is aligned to 8
  assert.equal(copyStringFromMemory(namePtr), 'test')

  free(namePtr)
  free(structPtr)
})

test('should be able to call a function in lib2 that uses struct from lib1', ({ assert }) => {
  const namePtr = copyStringToMemory('test')
  const structPtr = lib1.create_struct(42, namePtr)
  lib2.process_struct(structPtr)

  assert.equal(lib1.get_x(structPtr), 84)

  free(namePtr)
  free(structPtr)
})
