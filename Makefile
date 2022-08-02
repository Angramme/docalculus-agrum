# ugly convenience makefile

all:
	echo "commands: clean; config; build; run"

config::
	cd build && cmake .. -DCMAKE_BUILD_TYPE=Release

build::
	cd build && MSBuild.exe ./DOCALC.sln /property:Configuration=Release

run::
	cd build/Release && ./main

clean::
	rm -rf ./build/*