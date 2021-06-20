cflags  := -std=gnu++17 -c -Wall -g3
ldflags :=
libs    :=





# huff
build: huff

run: huff
	huff 


re: clean build

rerun: clean run


source_dirs     := src
headers_dirs    := lib
search_wildcard := $(wildcard $(addsuffix /*.cpp,$(source_dirs)))
object_files    := $(patsubst %.cpp,%.o,$(search_wildcard))
object_files    := $(notdir $(object_files))
object_files    := $(addprefix target/,$(object_files))
VPATH           := $(source_dirs)

huff: ./target $(object_files)
	g++ $(ldflags) -o $@ $(object_files) $(libs)

./target:
	if ! [ -d ./target ]; then mkdir target; fi

target/%.o: %.cpp
	g++ $(cflags) -o $@ -MD $(addprefix -I,$(headers_dirs)) $<

include $(wildcard target/*.d)



# clean
clean:
	-rm target/*.o huff





# end
