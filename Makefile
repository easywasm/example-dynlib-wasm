all: test/lib1.wasm test/lib2.wasm

test/%.wasm: wasm/%.c
	/opt/wasi-sdk/bin/clang -nostdlib -fPIC -shared -Wl,--experimental-pic -Wl,--no-entry -Wl,--export-all -Wl,--import-memory -o $@ $<
