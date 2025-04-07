.PHONY: clean test

test: test/lib1.wasm test/lib2.wasm test/lib3.wasm
	node --test

clean:
	rm -f test/*.wasm

test/lib3.wasm: wasm/lib3.c
	/opt/wasi-sdk/bin/clang -fPIC -Wl,--import-memory -o $@ $<

test/%.wasm: wasm/%.c
	/opt/wasi-sdk/bin/clang -fPIC -shared -Wl,--experimental-pic -Wl,--no-entry -Wl,--import-memory -o $@ $<
