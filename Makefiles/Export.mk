include $(MAKE)files/PROC.mk

fullRelease:

	$(MAKE) clear

	$(MAKE) debug
	$(MAKE) lib BUILD_MODE=DEBUG

	$(MAKE) clearBuild
	
	$(MAKE) release
	$(MAKE) lib BUILD_MODE=RELEASE

	$(MAKE) dllCopy

copyTmp:
		
	@echo "CWD: $(CURDIR)"
	rm -rf tmp
	mkdir tmp

	mkdir -p tmp/build
	find build -type f \( -name '*.exe' -o -name '*.dll' -o -name '*.a' \) -exec cp --parents {} tmp/ \;

	cp -r Recc tmp/
	cp -r Import tmp/
	cp -r Batch tmp/
	cp -f .VERSION tmp/
	cp -f LICENSE tmp/
	cp -f README.md tmp/
	cp -f README.de.md tmp/
	cp -f MAINTAINING.md tmp/

exportRelease:# fullRelease

	rm -f ALF.zip
	rm -f ALFLib.zip

	@make copyTmp
	rm -f tmp/build/*.a
	zip -r ALF.zip tmp/.VERSION tmp/*

	@make copyTmp
	rm -f tmp/build/*.exe
	zip -r ALFLib.zip tmp/.VERSION tmp/*

	rm -rf tmp