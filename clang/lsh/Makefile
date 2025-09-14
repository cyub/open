CMAKE ?= cmake
BUILD_DIR := build
CMAKE_FLAGS ?= -DCMAKE_BUILD_TYPE=Release


.PHONY: configure
configure:
	$(CMAKE) $(CMAKE_FLAGS) -S . -B $(BUILD_DIR)

.PHONY: build
build:
	$(CMAKE) --build $(BUILD_DIR)

.PHONY: run
run:
	./build/lsh

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

rebuild: clean configure build