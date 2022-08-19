# ugly convenience makefile

CONFIG = Release
# CONFIG = Debug

all:
	echo "commands: clean; config; build; run"

config::
	cd build && cmake .. -DCMAKE_BUILD_TYPE=$(CONFIG)

build::
	cd build && MSBuild.exe ./DOCALC.sln /property:Configuration=$(CONFIG)

run::
	cd build/$(CONFIG) && ./main

clean::
	rm -rf ./build/*