.PHONY: clean test

test: test/lib1.wasm test/lib2.wasm test/lib3.wasm test/lib4.wasm
	node --test

clean:
	rm -f test/*.wasm

# this is 2 modules that export the same stuff and use WASI

test/lib3.wasm: wasm/lib3.c
	/opt/wasi-sdk/bin/clang -Wl,--import-memory -o $@ $<

test/lib4.wasm: wasm/lib3.c
	/opt/wasi-sdk/bin/clang -Wl,--import-memory -o $@ $<


# these modules use wasm_shared

test/%.wasm: wasm/%.c
	/opt/wasi-sdk/bin/clang -fPIC -shared -Wl,--experimental-pic -Wl,--no-entry -Wl,--import-memory -o $@ $<
