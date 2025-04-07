.PHONY: clean test

test: test/lib1.wasm test/lib2.wasm
	node --test

clean:
	rm -f test/*.wasm

test/%.wasm: wasm/%.c
	/opt/wasi-sdk/bin/clang -nostdlib -fPIC -shared -Wl,--experimental-pic -Wl,--no-entry -Wl,--export-all -Wl,--import-memory -o $@ $<
