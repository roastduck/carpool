build:
	npm install
	./node_modules/.bin/electron-rebuild

rebuild:
	./node_modules/.bin/electron-rebuild

run:
	./node_modules/.bin/electron .

clean:
	rm -r node_modules

.PHONY: build rebuild run clean

