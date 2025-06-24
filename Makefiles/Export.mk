include $(MAKE)files/PROC.mk

fullRelease:

	$(MAKE) clear

	$(MAKE) debug
	$(MAKE) lib BUILD_MODE=DEBUG

	$(MAKE) clearBuild
	
	$(MAKE) release
	$(MAKE) lib BUILD_MODE=RELEASE

	$(MAKE) dllCopy

exportRelease: fullRelease

	@echo "CWD: $(CURDIR)"
	rm -rf tmp
	mkdir tmp

	cp -r bin tmp/

	mkdir -p tmp/build
	find build -type f \( -name '*.exe' -o -name '*.dll' -o -name '*.a' \) -exec cp --parents {} tmp/ \;

	cp -r Recc tmp/
	cp -r Import tmp/
	cp -r shader tmp/
	cp -r Markdown tmp/
	cp -f Credits.md tmp/
	cp -f License.txt tmp/
	cp -f README.md tmp/

	rm -f tmp/build/3dRenderingDemo.exe
	rm -f tmp/build/3dRenderingDemo_d.exe

	rm -f FemPROC.zip
	zip -r FemPROC.zip tmp/*
	rm -rf tmp