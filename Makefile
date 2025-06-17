include $(MAKE)files/Gitconfig.mk
include $(MAKE)files/PREFAB.mk
include $(MAKE)files/Export.mk

build: header proc

all:
	@echo "--- start Clock ---"
	@time $(MAKE) build -j
	@echo "-------------------"

TARGET = build/FEMProcUI
exec:
	@echo "Wechsel in: $(dir $(TARGET))"
	cd $(dir $(TARGET)) && ./$(notdir $(TARGET)$(SUFFIX))

demo:
	@$(MAKE) launch TARGET=build/3dRenderingDemo

execDemo:
	@$(MAKE) exec TARGET=build/3dRenderingDemo

formula:
	@$(MAKE) launch TARGET=build/FormulaEdit

flush:
	rm build/*.ini

launch: all exec

ping:
	@echo "Pong"

.DEFAULT_GOAL := all