cflags  := -std=gnu++17 -c -Wall
ldflags :=
libs    :=





# lz77
build: lz77

run: lz77
	lz77 


re: clean build

rerun: clean run


source_dirs     := src
header_dirs     := lib
search_wildcard := $(wildcard $(addsuffix /*.cpp,$(source_dirs)))
object_files    := $(patsubst %.cpp,%.o,$(search_wildcard))
object_files    := $(notdir $(object_files))
object_files    := $(addprefix target/,$(object_files))
VPATH           := $(source_dirs)

lz77: ./target $(object_files)
	g++ $(ldflags) -o $@ $(object_files) $(libs)

./target:
	mkdir -p target

target/%.o: %.cpp
	g++ $(cflags) -o $@ -MD $(addprefix -I,$(header_dirs)) $<

include $(wildcard target/*.d)



# clean
clean:
	-rm -r target lz77





# end
