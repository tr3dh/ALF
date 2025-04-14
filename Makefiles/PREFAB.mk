PACMAN := pacman -S --noconfirm --needed
MSYS2_ROOT ?= /c/msys64
MINGW_PREFIX := mingw-w64-x86_64

update:
	@echo "MinGw Packages updaten..."
	pacman -Syuu --noconfirm --needed

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

magic_enum:
	@if [ -d "thirdParty/magic_enum" ]; then \
			echo "Info: 'thirdParty/magic_enum' existiert bereits. Überspringe Build.";\
		else \
			cd thirdParty && git clone https://github.com/Neargye/magic_enum.git; \
		fi

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

	@echo "Installiere Vulkan-Tools..."
	$(PACMAN) $(MINGW_PREFIX)-spirv-tools $(MINGW_PREFIX)-glslang $(MINGW_PREFIX)-vulkan-devel
	$(PACMAN) $(MINGW_PREFIX)-assimp $(MINGW_PREFIX)-glm $(MINGW_PREFIX)-vulkanscenegraph

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

	@echo "Installiere Magic Enum..."
	@make magic_enum

	@make symengine