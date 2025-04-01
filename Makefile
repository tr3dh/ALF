include Makefiles/Gitconfig.mk
include Makefiles/PREFAB.mk
include Makefiles/PROC.mk

all: clean header proc

exec:
	./$(TARGET)

ping:
	@echo "Pong"

.DEFAULT_GOAL := all