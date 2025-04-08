CXX := g++
CXXFLAGS := -Wextra -MMD -MP -std=c++23 -fuse-ld=lld \
	-I./src \
	-I/mingw64/include \
	-I./thirdparty/symengine -I./thirdparty/symengine/build \
	-pthread

LDFLAGS := -L/mingw64/lib \
	-L./thirdparty/symengine/build/symengine -lsymengine \
	-lgmp -lmpfr \
	-lassimp \
	-lplplotcxx -lplplot \

SRCDIR := src
OBJDIR := build
SRC = $(shell find $(SRCDIR) -name '*.cpp')
OBJ = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRC))

# Größen für precompiled Header
PCH = build/precompiledDefines.h.gch
PCH_SRC = src/defines.h

TARGET := build/proc

# Erzeugen der Dependency files
-include $(OBJ:.o=.d)

# eigentliches Proc/executable
$(TARGET): $(OBJ)
	@mkdir -p $(@D)
	$(CXX) -o $@ $^ $(LDFLAGS)

proc:
	@make $(TARGET) -j

# Objekt files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(PCH)
	@mkdir -p $(@D)
	$(CXX) -c -o $@ $< $(CXXFLAGS) -include $(PCH_SRC)

# Precompiled Header
$(PCH): $(PCH_SRC)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

header:
	@make $(PCH) -j

clean:
	rm -f $(TARGET)

.DEFAULT_GOAL := all