include Makefiles/Gitconfig.mk
include Makefiles/PREFAB.mk
include Makefiles/PROC.mk

build: clean header proc

all:
	@echo "--- start Clock ---"
	@time make build -j
	@echo "-------------------"

exec:
	@echo "Wechsel in: $(dir $(TARGET))"
	cd $(dir $(TARGET)) && ./$(notdir $(TARGET))

launch: all exec

ping:
	@echo "Pong"

.DEFAULT_GOAL := all