
CXXFLAGS := -I. -std=c++2a -W -Wall -Wshadow -O3 -ggdb3
all:
	mkdir -p build; cd build ; cmake .. ; make -j VERBOSE=1 && ./named_tuple_example

reformat:
	@for f in *.h *.cpp ; do echo $$f ; clang-format -style="{BasedOnStyle: Google, IndentWidth: 4, SpaceAfterTemplateKeyword: false}" -i $$f ; done # # # named_tuple.h named_tuple_example.cpp


clean:
	rm -rf build *~ *.o *.a *.s 
