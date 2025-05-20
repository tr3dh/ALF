release:
	@echo "CWD: $(CURDIR)"
	rm -rf tmp
	mkdir tmp

	cp -r bin tmp/

	mkdir -p tmp/build
	find build -type f \( -name '*.exe' -o -name '*.dll' \) -exec cp --parents {} tmp/ \;

	cp -r Recc tmp/
	cp -r Import tmp/
	cp -r shader tmp/
	cp -r Markdown tmp/
	cp -f Credits.md tmp/
	cp -f License.txt tmp/
	cp -f README.md tmp/

	rm -f tmp/build/3dRenderingDemo.exe

	rm -f FemPROC.zip
	zip -r FemPROC.zip tmp/*
	rm -rf tmp