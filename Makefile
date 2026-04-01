.PHONY: build test clean docker

build:
	cd frontend-user && $(MAKE) build

test:
	cd frontend-user && $(MAKE) test

clean:
	cd frontend-user && $(MAKE) clean

docker:
	cd frontend-user && $(MAKE) docker
