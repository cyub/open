GO=go
BIN=silero-vad

# ==================== 自动检测架构 ====================
ARCH    := $(shell uname -m)
ifeq ($(ARCH),x86_64)
    ONNX_ARCH := x64
else ifeq ($(ARCH),aarch64)
    ONNX_ARCH := aarch64
else ifeq ($(ARCH),arm64)          # macOS 上 uname -m 返回 arm64
    ONNX_ARCH := aarch64
else ifeq ($(ARCH),ppc64le)
    ONNX_ARCH := ppc64le
else
    $(error Unsupported architecture: $(ARCH))
endif


# ==================== 版本和路径 ====================
ONNX_VERSION := 1.23.2
ONNX_TGZ     := onnxruntime-linux-$(ONNX_ARCH)-$(ONNX_VERSION).tgz
ONNX_URL     := https://github.com/microsoft/onnxruntime/releases/download/v$(ONNX_VERSION)/$(ONNX_TGZ)
ONNX_DIR     := onnxruntime-linux-$(ONNX_ARCH)-$(ONNX_VERSION)

.PHONY: install-onnxruntime clean show-arch


build:export CGO_ENABLED=1
build:export CGO_CFLAGS=-I$(PWD)/onnxruntime-linux-x64-1.23.2/include
build:export CGO_LDFLAGS=-L$(PWD)/onnxruntime-linux-x64-1.23.2/lib -lonnxruntime
build:
	$(GO) build -o $(BIN)

run:export LD_LIBRARY_PATH=$(PWD)/onnxruntime-linux-x64-1.23.2/lib
run:
	./$(BIN) --input ../../dataset/english.wav

install-onnxruntime: $(ONNX_DIR)/lib/libonnxruntime.so
	@echo "ONNX Runtime $(ONNX_VERSION) for $(ARCH) ($(ONNX_ARCH)) 已下载安装好！"

reinstall-onnxruntime: clean-install install-onnxruntime

# 显示当前检测到的架构（调试用）
show-arch:
	@echo "Detected ARCH     = $(ARCH)"
	@echo "ONNX package arch = $(ONNX_ARCH)"
	@echo "Will download     = $(ONNX_URL)"

$(ONNX_TGZ):
	@echo "Downloading ONNX Runtime for $(ARCH) ..."
	curl -fSL --progress-bar -o $@ $(ONNX_URL)

# 解压
$(ONNX_DIR)/lib/libonnxruntime.so: $(ONNX_TGZ)
	@echo "Extracting $(ONNX_TGZ) ..."
	tar -xzf $<
	@touch $@   # 防止重复解压

clean:
	rm -rf $(ONNX_TGZ)
	rm -rf $(ONNX_DIR)
	rm -rf $(BIN)
	rm -rf segments
