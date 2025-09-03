include $(MAKE)files/Gitconfig.mk
include $(MAKE)files/PREFAB.mk
include $(MAKE)files/Export.mk

build: header proc

all:
	@echo "--- start Clock ---"
	@time $(MAKE) build -j
	@echo "-------------------"

TARGET = build/ALF
exec:
	@echo "Wechsel in: $(dir $(TARGET))"
	cd $(dir $(TARGET)) && ./$(notdir $(TARGET)$(SUFFIX))

demo:
	@$(MAKE) launch TARGET=build/3dRenderingDemo

execDemo:
	@$(MAKE) exec TARGET=build/3dRenderingDemo

formula:
	@$(MAKE) launch TARGET=build/FormulaEdit

temp:
	@$(MAKE) launch TARGET=build/temp

flush:
	rm build/*.ini

clearCaches:
	find Import -type f -name '*.RESULTCACHE' -delete

launch: all exec

relaunch: clear launch

ping:
	@echo "Pong"

api:
	@$(MAKE) launch TARGET=build/ALFAPI

.DEFAULT_GOAL := all