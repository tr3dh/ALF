CXX := g++
BUILD_MODE ?= DEBUG

CXXFLAGS :=\

LDFLAGS :=\

NATIVEWIN_FLAGS =\
	-Wl,--subsystem,windows\
	-mwindows

SUFFIX := \

ifeq ($(BUILD_MODE),DEBUG)
    CXXFLAGS += -g -DDEBUG
	SUFFIX = _d
else
    CXXFLAGS += -O3 -DNDEBUG -march=native -funroll-loops -flto -ffunction-sections -fdata-sections
	LDFLAGS += $(NATIVEWIN_FLAGS) -flto -fuse-linker-plugin -Wl,-O2 -Wl,--gc-sections -Wl,--as-needed
endif

# -Wno-missing-designated-field-initializers
CXXFLAGS += -Wextra -MMD -MP -std=c++23 -fuse-ld=lld -fexceptions \
	-Wno-deprecated-literal-operator \
	-Wno-missing-field-initializers -Wno-sign-compare \
	-Wno-cpp \
	-I./src \
	-I./thirdParty \
	-I./thirdParty/raylib/src \
	-I/mingw64/include \
	-I./thirdparty/symengine -I./thirdparty/symengine/build \
	-I./thirdparty/magic_enum/include \
	-I./thirdParty/r3d/include \
	-I./thirdParty/imgui \
	-I./thirdParty/raylibImgui \
	-I./thirdParty/imgui-filebrowser \
	-I./thirdParty/implot \
	-pthread

LINKING ?= STATIC

ifeq ($(LINKING), STATIC)
	CXXFLAGS += -static-libgcc -static-libstdc++
endif

LDFLAGS += -L./src \
	-L./thirdParty/raylib/build/raylib -L./thirdParty/raylib/dynamicBuild/raylib -lraylib \
	-L/mingw64/lib \
	-L./thirdparty/symengine/build/symengine -lsymengine \
	-L./thirdParty/rlImGui/bin -lrlimgui \
	-L./thirdParty/implot/bin -limplot \
	-L./thirdParty/imgui/bin -limgui \
	-lgdi32 -lopengl32 -ld3d11 -ld3d9 -ldxgi \
	-lwinmm -lws2_32\
	-lglfw3 \
	-lgmp -lmpfr \
	-lpthread\

# CXXFLAGS += $(shell llvm-config --cxxflags)
# LDFLAGS  += -lLLVM-20 $(shell llvm-config --system-libs)

# notwendig da llvm-config --cxxflags den sprachstandard auf c++17 setzt und -fno-exeptions
CXXFLAGS += -std=c++23 -fexceptions

SRCDIR := src
OBJDIR := build
SRC = $(shell find $(SRCDIR) -name '*.cpp')
OBJ = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRC))

# Größen für precompiled Header
PCH = build/precompiledDefines.h.gch
PCH_SRC = src/defines.h

# Erzeugen der Dependency files
-include $(OBJ:.o=.d)

# eigentliches Proc/executable
#$(TARGET): $(OBJ)
#	@mkdir -p $(@D)
#	$(CXX) -o $@ $^ Recc/Compilation/proc.res $(LDFLAGS)

.PHONY: proc debug release

proc:
	$(MAKE) -j targets 

debug:
	$(MAKE) BUILD_MODE=DEBUG proc 

release:
	$(MAKE) BUILD_MODE=RELEASE proc

# Objekt files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(PCH)
	@mkdir -p $(@D)
	$(CXX) -c -o $@ $< $(CXXFLAGS) -include $(PCH_SRC)

SRCLIB := build/libalf$(SUFFIX).a
$(SRCLIB): $(OBJ) $(PCH)
	ar rcs $(SRCLIB) $^

lib: $(SRCLIB)

PROC_DIR = Procs
PROCS_CPP = $(shell find $(PROC_DIR) -name '*.cpp')
PROCS_OBJ = $(patsubst $(PROC_DIR)/%.cpp, build/%.o, $(PROCS_CPP))
PROCS_TARGETS = $(patsubst $(PROC_DIR)/%.cpp, build/%, $(PROCS_CPP))

PROCSRC ?= $(OBJ)

build/%.o: $(PROC_DIR)/%.cpp $(PCH)
	@mkdir -p $(@D)
	$(CXX) -c -o $@ $< $(CXXFLAGS) -include $(PCH_SRC)

build/% : build/%.o $(PROCSRC)
	@mkdir -p $(@D)
	$(CXX) -o $@$(SUFFIX) $^ Recc/Compilation/proc.res $(LDFLAGS)

targets: $(PROCS_OBJ) $(PROCS_TARGETS)

targetsFromObj:
	@$(MAKE) targets PROC_SRC=$(OBJ)

targetsFromLib:
	@$(MAKE) targets PROC_SRC=$(SRCLIB)

# Precompiled Header
$(PCH): $(PCH_SRC)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

header:
	@$(MAKE) V=1 $(PCH) -j CXXFLAGS="$(CXXFLAGS)"

ICON ?= Recc/Compilation/icon
icon:
	magick "$(ICON).png" -define icon:auto-resize=16,32,48,64,128,256 "$(ICON).ico"; \

res:
	windres Recc/Compilation/proc.rc -O coff -o Recc/Compilation/proc.res

# nach ausführung muss exit unter Umständen selbst nochmak in die cmd eingetippt werden damit man zur msys shell zurückkommt
clearIconCache:
	@cmd /C "Batch//clearIconCache.bat"

clean:
	rm -f $(PROCS_TARGETS) $(PROCS_OBJ)

clear:
	rm -r build

clearBuild:
	@echo "Cleaning build directory (except .exe files)..."
	@find build -type f ! \( -name "*.exe" -o -name "*.a" \) -delete
	@find build -type d -empty -delete

remComp:
	@find build -type f \( -name '*.o' -o -name '*.d' \) -delete

.DEFAULT_GOAL := all