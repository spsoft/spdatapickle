
#--------------------------------------------------------------------

ifeq ($(origin version), undefined)
	version = 0.2
endif

#--------------------------------------------------------------------

all:
	@( cd spxml; make )
	@( cd spjson; make )
	@( cd spdatapickle; make )

dist: clean spdatapickle-$(version).src.tar.gz

spdatapickle-$(version).src.tar.gz:
	@find . -type f | grep -v CVS | grep -v .svn | sed s:^./:spdatapickle-$(version)/: > MANIFEST
	@(cd ..; ln -s spdatapickle spdatapickle-$(version))
	(cd ..; tar cvf - `cat spdatapickle/MANIFEST` | gzip > spdatapickle/spdatapickle-$(version).src.tar.gz)
	@(cd ..; rm spdatapickle-$(version))

clean:
	@( cd spdatapickle; make clean )
	@( cd spxml;  make clean )
	@( cd spjson;  make clean )

