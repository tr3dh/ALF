PACMAN := pacman -S --noconfirm --needed
MSYS2_ROOT ?= /c/msys64
MINGW_PREFIX := mingw-w64-x86_64

update:
	@echo "MinGw Packages updaten..."
	pacman -Syuu --noconfirm --needed

DXC_RELEASE_URL := https://api.github.com/repos/microsoft/DirectXShaderCompiler/releases/latest
DXC_DIR := thirdparty/dxc

dxc:
	@if [ -d "thirdParty/dxc" ]; then \
		echo "Info: 'thirdParty/dxc' existiert bereits. Überspringe Build.";\
	else \
		make setup-dxc;\
	fi\

setup-dxc:
	@echo "Suche neueste dxc-Version..."
	@mkdir -p $(DXC_DIR)
	@curl -s $(DXC_RELEASE_URL) | grep -oP '"browser_download_url": "\K[^"]+dxc[^"]+\.zip' | head -1 > $(DXC_DIR)/download_url.txt
	@DXC_DOWNLOAD_URL=$$(cat $(DXC_DIR)/download_url.txt); \
	if [ -z "$$DXC_DOWNLOAD_URL" ]; then \
		echo "Fehler: Download-URL nicht gefunden!"; \
		exit 1; \
	fi; \
	echo "Lade dxc herunter: $$DXC_DOWNLOAD_URL"; \
	curl -L $$DXC_DOWNLOAD_URL -o $(DXC_DIR)/dxc.zip || (echo "Download fehlgeschlagen"; exit 1)
	@echo "Entpacke dxc..."
	@unzip -q $(DXC_DIR)/dxc.zip -d $(DXC_DIR) || (echo "Entpacken fehlgeschlagen"; exit 1)
	@chmod +x $(DXC_DIR)/bin/x64/dxc.exe
	@echo "Füge dxc zum PATH hinzu..."
	@export PATH="$$(pwd)/$(DXC_DIR)/bin:$$PATH"

# bislang habe ich die symengine nur mit ninja gebaut bekommen
# deshalb ist auc die installation erforderlich
symengine:
	@if [ -d "thirdParty/symengine" ]; then \
		echo "Info: 'thirdParty/symengine' existiert bereits. Überspringe Build.";\
	else \
		cd thirdParty && git clone https://github.com/symengine/symengine.git && \
		cd symengine && mkdir -p build && \
		cd build && cmake -G "Ninja" \
			-DCMAKE_C_COMPILER=gcc \
			-DCMAKE_CXX_COMPILER=g++ \
			-DCMAKE_INSTALL_PREFIX=/mingw64 \
			-DCMAKE_BUILD_TYPE=Release \
			-DWITH_SYMENGINE_THREAD_SAFE=ON \
			-DBUILD_BENCHMARKS=OFF \
			-DWITH_MPFR=ON \
			-DWITH_BOOST=OFF \
			-DWITH_LLVM=off \
			.. && \
		ninja; \
	fi

# bislang habe ich die symengine nur mit ninja gebaut bekommen
# deshalb ist auc die installation erforderlich
r3d:
	@if [ -d "thirdParty/r3d" ]; then \
		echo "Info: 'thirdParty/r3d' existiert bereits. Überspringe Build.";\
	else \
		cd thirdParty && it clone --recurse-submodules https://github.com/Bigfoot71/r3d; \
		cd r3d && mkdir -p build; \
		cmake .. -DCMAKE_BUILD_TYPE=Release; \
		cmake --build . --config Release; \
		cmake --build .; \
	fi \
	
	@make dllCopy COPYTARGET=thirdParty/r3d/build/

magic_enum:
	@if [ -d "thirdParty/magic_enum" ]; then \
			echo "Info: 'thirdParty/magic_enum' existiert bereits. Überspringe Build.";\
		else \
			cd thirdParty && git clone https://github.com/Neargye/magic_enum.git; \
		fi

vulkanSamples:
	@if [ -d "thirdParty/Vulkan" ]; then \
			echo "Info: 'thirdParty/Vulkan' existiert bereits. Überspringe Build.";\
		else \
			echo "Info: builde vulkan samples."; \
			cd thirdParty && git clone --recursive https://github.com/SaschaWillems/Vulkan.git; \
			cd Vulkan && cmake -Bbuild -G "MinGW Makefiles"; \
			cmake --build build; \
		fi

	@make dllCopy COPYTARGET=thirdParty/Vulkan/build/bin/

COPYTARGET ?= build/
dllCopy:
	mkdir -p $(COPYTARGET);
	cp /mingw64/bin/libgcc_s_seh-1.dll $(COPYTARGET);
	cp /mingw64/bin/libstdc++-6.dll $(COPYTARGET);
	cp /mingw64/bin/libwinpthread-1.dll $(COPYTARGET);
	cp /mingw64/bin/libgomp-1.dll $(COPYTARGET);
	cp /mingw64/bin/libgmp-10.dll $(COPYTARGET);
	cp /mingw64/bin/libmpfr-6.dll $(COPYTARGET);
	cp /mingw64/bin/libraylib.dll $(COPYTARGET);
	cp /mingw64/bin/glfw3.dll $(COPYTARGET);
	echo "DLLs kopiert nach $(COPYTARGET)";

matplot:
	cd thirdParty && git clone https://github.com/alandefreitas/matplotplusplus
	cd thirdParty && cd matplotplusplus && mkdir build
	cd thirdParty/matplotplusplus/build && cmake .. -G "MinGW Makefiles" && cmake --build .

# Batch-Datei erstellen
open_vscode.bat:
	@echo @echo off > open_vscode.bat
	@echo code . >> open_vscode.bat
	@echo exit >> open_vscode.bat

batchOpen: open_vscode.bat

prefab:

	@mkdir -p build
	@mkdir -p thirdParty

	@make batchOpen

	@echo "Installiere Toolchain..."
	$(PACMAN) $(MINGW_PREFIX)-toolchain base-devel

	@echo "Installiere Build-Tools..."
	$(PACMAN) $(MINGW_PREFIX)-cmake $(MINGW_PREFIX)-make $(MINGW_PREFIX)-ninja
	$(PACMAN) unzip curl


	@echo "Installiere Vulkan-Tools..."
	$(PACMAN) $(MINGW_PREFIX)-spirv-tools $(MINGW_PREFIX)-glslang $(MINGW_PREFIX)-vulkan-devel
	$(PACMAN) $(MINGW_PREFIX)-assimp $(MINGW_PREFIX)-glm $(MINGW_PREFIX)-vulkanscenegraph
	$(PACMAN) mingw-w64-x86_64-vulkan-loader
	$(PACMAN) mingw-w64-x86_64-vulkan-validation-layers
	$(PACMAN) mingw-w64-x86_64-vulkan-utility-libraries
	$(PACMAN) mingw-w64-x86_64-glfw
	$(PACMAN) mingw-w64-x86_64-shaderc

	@echo "Installiere mathematische Bibliotheken..."
	$(PACMAN) $(MINGW_PREFIX)-gmp $(MINGW_PREFIX)-mpfr $(MINGW_PREFIX)-mpc $(MINGW_PREFIX)-flint

	@echo "Installiere LLVM/Boost..."
	$(PACMAN) $(MINGW_PREFIX)-boost $(MINGW_PREFIX)-llvm

	@echo "Installiere Eigen3..."
	$(PACMAN) $(MINGW_PREFIX)-eigen3

	@echo "Installiere Git..."
	$(PACMAN) git

	@echo "Version Check..."
	gcc --version && g++ --version
	make --version && cmake --version && ninja --version
	git --version

	@echo "Installiere SFML..."
	$(PACMAN) mingw-w64-x86_64-sfml

	@echo "Installiere NLohmnann JSON..."
	$(PACMAN) mingw-w64-x86_64-nlohmann-json

	$(PACMAN) mingw-w64-x86_64-imagemagick
	$(PACMAN) mingw-w64-x86_64-raylib

	@echo "Installiere Magic Enum..."
	@make magic_enum

	@make symengine