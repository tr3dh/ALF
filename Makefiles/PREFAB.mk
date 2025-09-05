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

raylibCpp:
	@if [ -d "thirdParty/raylib-cpp" ]; then \
		echo "Info: 'thirdParty/raylib' existiert bereits. Überspringe Build."; \
	else \
		cd thirdParty && git clone https://github.com/RobLoach/raylib-cpp.git; \
	fi

# Quell- und Objektdateien
RLIMGUI_DIR = thirdParty/rlImgui
RLIMGUI_SRC = $(wildcard $(RLIMGUI_DIR)/*.cpp)
RLIMGUI_OBJ = $(patsubst $(RLIMGUI_DIR)/%.cpp,thirdParty/rlImgui/bin/%.o,$(RLIMGUI_SRC))

# ImGui Kompilieren
thirdParty/rlImgui/bin%.o: $(RLIMGUI_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) -c -o $@ $< -I./thirdParty/imgui

librlimgui: $(RLIMGUI_OBJ)
	ar rcs thirdParty/rlImgui/bin/librlimgui.a $^

raylibImgui:
	@if [ -d "thirdParty/rlImGui" ]; then \
		echo "Info: 'thirdParty/rlImGui' existiert bereits. Überspringe Build."; \
	else \
		cd thirdParty && git clone https://github.com/raylib-extras/rlImGui.git; \
	fi

	@if [ -d "thirdParty/rlImGui/bin" ]; then \
		echo "Info: 'thirdParty/rlImGui/bin' existiert bereits. Überspringe Build."; \
	else \
		cd thirdParty && cd rlImgui && mkdir bin; \
		cd ../../; \
		make librlimgui; \
	fi

# Quell- und Objektdateien
IMGUI_DIR = thirdParty/imgui
IMGUI_SRC = $(wildcard $(IMGUI_DIR)/*.cpp)
IMGUI_OBJ = $(patsubst $(IMGUI_DIR)/%.cpp,thirdParty/imgui/bin/%.o,$(IMGUI_SRC))

# ImGui Kompilieren
thirdParty/imgui/bin%.o: $(IMGUI_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) -c -o $@ $<

libimgui: $(IMGUI_OBJ)
	ar rcs thirdParty/imgui/bin/libimgui.a $^

imgui:
	@if [ -d "thirdParty/imGui" ]; then \
		echo "Info: 'thirdParty/imGui' existiert bereits. Überspringe Build."; \
	else \
		cd thirdParty && git clone https://github.com/ocornut/imgui.git; \
	fi

	@if [ -d "thirdParty/imGui/bin" ]; then \
		echo "Info: 'thirdParty/imGui/bin' existiert bereits. Überspringe Build."; \
	else \
		cd thirdParty && cd imgui && mkdir bin; \
		cd ../../; \
		make libimgui; \
	fi

filebrowser:
	@if [ -d "thirdParty/imgui-filebrowser" ]; then \
		echo "Info: 'thirdParty/imgui-filebrowser' existiert bereits. Überspringe Build."; \
	else \
		cd thirdParty && git clone https://github.com/AirGuanZ/imgui-filebrowser.git; \
	fi

# Quell- und Objektdateien
IMPLOT_DIR = thirdParty/implot
IMPLOT_SRC = $(wildcard $(IMPLOT_DIR)/*.cpp)
IMPLOT_OBJ = $(patsubst $(IMPLOT_DIR)/%.cpp,thirdParty/implot/bin/%.o,$(IMPLOT_SRC))

# ImGui Kompilieren
thirdParty/implot/bin%.o: $(IMPLOT_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) -c -o $@ $< -I./thirdParty/imgui/

libimplot: $(IMPLOT_OBJ)
	ar rcs thirdParty/implot/bin/libimplot.a $^

implot:
	@if [ -d "thirdParty/implot" ]; then \
		echo "Info: 'thirdParty/implot' existiert bereits. Überspringe Build."; \
	else \
		cd thirdParty && git clone https://github.com/epezent/implot.git; \
	fi

	@if [ -d "thirdParty/implot/bin" ]; then \
		echo "Info: 'thirdParty/implot/bin' existiert bereits. Überspringe Build."; \
	else \
		cd thirdParty && cd implot && mkdir bin; \
		cd ../../; \
		make libimplot; \
	fi

# bislang habe ich die symengine nur mit ninja gebaut bekommen
# deshalb ist auc die installation erforderlich
r3d:
	@if [ -d "thirdParty/r3d" ]; then \
		echo "Info: 'thirdParty/r3d' existiert bereits. Überspringe Build.";\
	else \
		cd thirdParty && git clone --recurse-submodules https://github.com/Bigfoot71/r3d; \
		cd r3d; \
		mkdir build; \
		cd build; \
		cmake ..; \
		cmake --build .; \
	fi \
	
	@make dllCopy COPYTARGET=thirdParty/r3d/build/

raylib:

	@if [ -d "thirdParty/raylib" ]; then \
		echo "Info: 'thirdParty/raylib' existiert bereits. Überspringe Build.";\
	else \
		cd thirdParty && git clone https://github.com/raysan5/raylib.git; \
		cd raylib/; \
		cmake -B build -G "MinGW Makefiles" \
			-DGRAPHICS=GRAPHICS_API_OPENGL_43 \
			-DSUPPORT_FILEFORMAT_GLTF=ON \
			-DSUPPORT_FILEFORMAT_GLTF_DRACO=ON \
			-DSUPPORT_FILEFORMAT_ASSIMP=ON \
			-DUSE_EXTERNAL_GLFW=ON \
			-DBUILD_SHARED_LIBS=OFF \
			-DCMAKE_BUILD_TYPE=Release; \
		cmake --build build; \
		cmake -B dynamicBuild -G "MinGW Makefiles" \
			-DGRAPHICS=GRAPHICS_API_OPENGL_43 \
			-DSUPPORT_FILEFORMAT_GLTF=ON \
			-DSUPPORT_FILEFORMAT_GLTF_DRACO=ON \
			-DSUPPORT_FILEFORMAT_ASSIMP=ON \
			-DUSE_EXTERNAL_GLFW=ON \
			-DBUILD_SHARED_LIBS=ON \
			-DCMAKE_BUILD_TYPE=Release; \
		cmake --build dynamicBuild; \
	fi
	
#	@make dllCopy COPYTARGET=thirdParty/r3d/build/

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

enet:
	@if [ -d "thirdParty/enet" ]; then \
			echo "Info: 'thirdParty/enet' existiert bereits. Überspringe Build.";\
		else \
			echo "Info: builde enet."; \
			cd thirdParty && git clone https://github.com/lsalzman/enet.git; \
			cd enet;\
			cmake -B build -DENET_STATIC=1 -DCMAKE_BUILD_TYPE=Release;\
			cmake --build build;\
		fi

# funktioniert noch nicht einwandfrei !!
microTex:
	@if [ -d "thirdParty/MicroTeX" ]; then \
			rm -rf thirdParty/MicroTeX; \
			echo "Info: 'thirdParty/MicroTeX' existiert bereits. Überspringe Build.";\
		else \
			echo "Info: builde MicroTeX."; \
			cd thirdParty && git clone https://github.com/NanoMichael/MicroTeX; \
			cd MicroTeX;\
			mkdir build && cd build;\
			cmake .. -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_STANDARD_REQUIRED=ON -DCMAKE_CXX_EXTENSIONS=OFF -DCMAKE_CXX_FLAGS="-DWIN32_LEAN_AND_MEAN -DNOMINMAX";\
			cmake --build .;\
		fi

COPYTARGET ?= build/
dllCopy:
	mkdir -p $(COPYTARGET);
	cp -n /mingw64/bin/libgcc_s_seh-1.dll $(COPYTARGET);
	cp -n /mingw64/bin/libstdc++-6.dll $(COPYTARGET);
	cp -n /mingw64/bin/libwinpthread-1.dll $(COPYTARGET);
	cp -n /mingw64/bin/libgomp-1.dll $(COPYTARGET);
	cp -n /mingw64/bin/libgmp-10.dll $(COPYTARGET);
	cp -n /mingw64/bin/libmpfr-6.dll $(COPYTARGET);
	cp -n thirdParty/raylib/dynamicBuild/raylib/libraylib.dll $(COPYTARGET);
	cp -n /mingw64/bin/glfw3.dll $(COPYTARGET);
# 	cp -n /mingw64/bin/vulkan-1.dll $(COPYTARGET);
# 	cp -n /mingw64/bin/libsfml-graphics-2.dll $(COPYTARGET);
# 	cp -n /mingw64/bin/libsfml-system-2.dll $(COPYTARGET);
# 	cp -n /mingw64/bin/libsfml-audio-2.dll $(COPYTARGET);
# 	cp -n /mingw64/bin/libsfml-window-2.dll $(COPYTARGET);
	cp -n /mingw64/bin/libfreetype-6.dll $(COPYTARGET);
	cp -n /mingw64/bin/libbrotlidec.dll $(COPYTARGET);
	cp -n /mingw64/bin/libbrotlicommon.dll $(COPYTARGET);
	cp -n /mingw64/bin/libbz2-1.dll $(COPYTARGET);
	cp -n /mingw64/bin/libharfbuzz-0.dll $(COPYTARGET);
	cp -n /mingw64/bin/libpng16-16.dll $(COPYTARGET);
	cp -n /mingw64/bin/zlib1.dll $(COPYTARGET);
	cp -n /mingw64/bin/libglib-2.0-0.dll $(COPYTARGET);
	cp -n /mingw64/bin/libgraphite2.dll $(COPYTARGET);
	cp -n /mingw64/bin/libintl-8.dll $(COPYTARGET);
	cp -n /mingw64/bin/libiconv-2.dll $(COPYTARGET);
	cp -n /mingw64/bin/libpcre2-8-0.dll $(COPYTARGET);
	cp -n /mingw64/bin/libLLVM-20.dll $(COPYTARGET);
	cp -n /mingw64/bin/libxml2-16.dll $(COPYTARGET);

	echo "DLLs kopiert nach $(COPYTARGET)";

prefab:

	@mkdir -p build
	@mkdir -p thirdParty

	@echo "Installiere Toolchain..."
	$(PACMAN) $(MINGW_PREFIX)-toolchain base-devel

	@echo "Installiere Build-Tools..."
	$(PACMAN) $(MINGW_PREFIX)-cmake $(MINGW_PREFIX)-make $(MINGW_PREFIX)-ninja
	$(PACMAN) unzip curl

	@echo "Installiere Vulkan-Tools..."
	$(PACMAN) $(MINGW_PREFIX)-spirv-tools $(MINGW_PREFIX)-glslang 
	$(PACMAN) $(MINGW_PREFIX)-assimp $(MINGW_PREFIX)-glm

# 	$(MINGW_PREFIX)-vulkan-devel
#	$(MINGW_PREFIX)-vulkanscenegraph
# 	$(PACMAN) mingw-w64-x86_64-vulkan-loader
# 	$(PACMAN) mingw-w64-x86_64-vulkan-validation-layers
# 	$(PACMAN) mingw-w64-x86_64-vulkan-utility-libraries
	$(PACMAN) mingw-w64-x86_64-glfw
# 	$(PACMAN) mingw-w64-x86_64-shaderc

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

	@echo "Installiere zip..."
	$(PACMAN) zip

	@echo "Installiere SFML..."
	$(PACMAN) mingw-w64-x86_64-sfml

	@echo "Installiere NLohmnann JSON..."
	$(PACMAN) mingw-w64-x86_64-nlohmann-json

	$(PACMAN) mingw-w64-x86_64-imagemagick
	$(PACMAN) mingw-w64-x86_64-poppler
	$(PACMAN) mingw-w64-x86_64-tinyxml2

# 	$(PACMAN) mingw-w64-x86_64-raylib

# 	@make r3d
# 	@make raylibCpp

	@echo "Installiere Magic Enum..."
	@make magic_enum

	@make symengine

	@make raylib
	@make imgui
	@make filebrowser
	@make raylibImgui
	@make implot