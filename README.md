This is a simple example that follows [DynamicLinking](https://github.com/WebAssembly/tool-conventions/blob/main/DynamicLinking.md).

Essentially, we have 2 dynamic libraries that can pass structs and strings to each other.

- I have implemented the host in js but other hosts could follow similar pattern.
- I have implemented the wasm in C but other languages could follow similar pattern.
