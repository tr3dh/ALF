CXX := g++

#	-I./thirdParty/enet/include 
CXXFLAGS := -Wextra -MMD -MP -std=c++23 -fuse-ld=lld \
	-g \
	-Wno-deprecated-literal-operator \
	-Wno-cpp \
	-I./src \
	-I./thirdParty \
	-I/mingw64/include \
	-I./thirdparty/symengine -I./thirdparty/symengine/build \
	-I./thirdparty/magic_enum/include \
	-I./thirdParty/r3d/include \
	-I./thirdParty/imgui \
	-I./thirdParty/raylibImgui \
	-I./thirdParty/imgui-filebrowser \
	-I./thirdParty/implot \
	-pthread

# -lr3d vor lraylib einfügen falls nötig
# -lbase nach -L...vulkan/build/base einfügen
# -L./thirdParty/enet/build -lenet vor winsocks
LDFLAGS := -L/mingw64/lib \
	-L./thirdparty/symengine/build/symengine -lsymengine \
	-L./thirdParty/rlImGui/bin -lrlimgui \
	-L./thirdParty/implot/bin -limplot \
	-L./thirdParty/imgui/bin -limgui \
	-L./thirdParty/r3d/build \
	-lr3d -lraylib \
	-lopengl32 -lgdi32 -lwinmm -lws2_32\
	-lgmp -lmpfr \
	-lsfml-graphics -lsfml-window -lsfml-system -lsfml-network -lsfml-audio\
	-lpthread\

SRCDIR := src
OBJDIR := build
SRC = $(shell find $(SRCDIR) -name '*.cpp')
OBJ = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRC))

# Größen für precompiled Header
PCH = build/precompiledDefines.h.gch
PCH_SRC = src/defines.h

#TARGET := build/FEMProcUI__

# Erzeugen der Dependency files
-include $(OBJ:.o=.d)

# eigentliches Proc/executable
#$(TARGET): $(OBJ)
#	@mkdir -p $(@D)
#	$(CXX) -o $@ $^ Recc/Compilation/proc.res $(LDFLAGS)

proc:
	make targets

# Objekt files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(PCH)
	@mkdir -p $(@D)
	$(CXX) -c -o $@ $< $(CXXFLAGS) -include $(PCH_SRC)

PROC_DIR = Procs
PROCS_CPP = $(shell find $(PROC_DIR) -name '*.cpp')
PROCS_OBJ = $(patsubst $(PROC_DIR)/%.cpp, build/%.o, $(PROCS_CPP))
PROCS_TARGETS = $(patsubst $(PROC_DIR)/%.cpp, build/%, $(PROCS_CPP))

build/%.o: $(PROC_DIR)/%.cpp $(PCH)
	@mkdir -p $(@D)
	$(CXX) -c -o $@ $< $(CXXFLAGS) -include $(PCH_SRC)

build/% : build/%.o $(OBJ)
	@mkdir -p $(@D)
	$(CXX) -o $@ $^ Recc/Compilation/proc.res $(LDFLAGS)

targets: $(PROCS_OBJ) $(PROCS_TARGETS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(PCH)
	@mkdir -p $(@D)
	$(CXX) -c -o $@ $< $(CXXFLAGS) -include $(PCH_SRC)

# Precompiled Header
$(PCH): $(PCH_SRC)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

header:
	@make $(PCH) -j

ICON ?= Recc/Compilation/icon
icon:
	magick "$(ICON).png" -define icon:auto-resize=16,32,48,64,128,256 "$(ICON).ico"; \

res:
	windres Recc/Compilation/proc.rc -O coff -o Recc/Compilation/proc.res

# nach ausführung muss exit unter Umständen selbst nochmak in die cmd eingetippt werden damit man zur msys shell zurückkommt
clearIconCache:
	@cmd /C "Batch//clearIconCache.bat"

clean:
#rm -f $(PROCS_TARGETS)

.DEFAULT_GOAL := all