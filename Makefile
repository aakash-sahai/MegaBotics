all:
	mkdir -p target
	rm -f target/MegaBotics.zip
	(cd arduino/libraries; zip -r ../../target/MegaBotics.zip MegaBotics)
