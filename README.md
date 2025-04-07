This is a simple example that follows [DynamicLinking](https://github.com/WebAssembly/tool-conventions/blob/main/DynamicLinking.md).

Essentially, we have a few wasm modules that can pass structs and strings to each other, via shared mem & host-implemented malloc/free.

- I have implemented the host in js but other hosts could follow similar pattern.
- I have implemented the wasm in C but other languages could follow similar pattern.
