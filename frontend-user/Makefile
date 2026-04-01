.PHONY: build test clean docker

BUILD_DIR := build

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    NPROC := $(shell nproc)
endif
ifeq ($(UNAME_S),Darwin)
    NPROC := $(shell sysctl -n hw.ncpu)
endif
ifndef NPROC
    NPROC := 1
endif

build:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake ..
	cd $(BUILD_DIR) && make -j$(NPROC)

test: build
	cd $(BUILD_DIR) && make test

clean:
	rm -rf $(BUILD_DIR)
	rm -rf test_output

docker:
	docker build -t xml-to-docx-converter .
