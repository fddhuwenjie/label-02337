.PHONY: build test clean docker cppcheck help

BUILD_DIR := frontend-user/build
PROJECT_DIR := frontend-user

all: build

build:
	@echo "Building project..."
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake ..
	@cd $(BUILD_DIR) && make -j$$(nproc)
	@echo "Build completed successfully!"

test: build
	@echo "Running tests..."
	@cd $(BUILD_DIR) && xvfb-run make test
	@echo "Tests completed!"

cppcheck:
	@echo "Running cppcheck static analysis..."
	@cd $(PROJECT_DIR) && cppcheck --enable=warning,style,performance \
		--std=c++17 \
		--error-exitcode=1 \
		--suppress=missingIncludeSystem \
		-I include \
		src/ include/
	@echo "Cppcheck completed successfully!"

clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR)
	@echo "Clean completed!"

docker:
	@echo "Building Docker image..."
	@cd $(PROJECT_DIR) && docker build -t xml-to-docx-converter .
	@echo "Docker image built successfully!"

docker-run: docker
	@echo "Starting Docker container..."
	@cd $(PROJECT_DIR) && docker run -p 8080:8080 xml-to-docx-converter

help:
	@echo "Available targets:"
	@echo "  make build      - Build the project using CMake"
	@echo "  make test       - Build and run tests"
	@echo "  make cppcheck   - Run cppcheck static analysis"
	@echo "  make clean      - Remove build artifacts"
	@echo "  make docker     - Build Docker image"
	@echo "  make docker-run - Build and run Docker container (port 8080)"
	@echo "  make help       - Show this help message"
