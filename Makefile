EXEC	 = WE2010

DEBUG    = Debug
RELEASE  = Release

first: debug

all: debug release

debug:
	cd ${DEBUG}; make -j3 all

release:
	cd ${RELEASE}; make -j3 all

clean:
	rm -f ${EXEC}
	cd ${DEBUG}; make clean
	cd ${RELEASE}; make clean
	rm -f core*
