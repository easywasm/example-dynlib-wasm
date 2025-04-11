.PHONY: clean test

test: test/lib1.wasm test/lib2.wasm test/lib3.wasm test/lib4.wasm
	node --test

clean:
	rm -f test/*.wasm

# these modules use wasm_shared

# copy of lib3
test/lib4.wasm: wasm/lib3.c
	/opt/wasi-sdk/bin/clang  -Wl,--import-memory -o $@ $<

test/%.wasm: wasm/%.c
	/opt/wasi-sdk/bin/clang   -Wl,--import-memory -o $@ $<
