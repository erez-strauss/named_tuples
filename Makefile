
CXXFLAGS := -I. -std=c++2a -W -Wall -Wshadow -O3 -ggdb3

all: gtest_nvtuple
	mkdir -p build; cd build ; cmake .. ; make -j VERBOSE=1 && ./named_tuple_example && ./gtest_nvtuple

reformat:
	@for f in *.h *.cpp ; do echo $$f ; clang-format -style="{BasedOnStyle: Google, IndentWidth: 4, SpaceAfterTemplateKeyword: false}" -i $$f ; done # # # named_tuple.h named_tuple_example.cpp


gtest_nvtuple: gtest_nvtuple.cpp named_tuple.h
	g++ -O3 -ggdb3 -W -Wall -Wshadow -std=c++17 -I . -DGTEST_HAS_PTHREAD=1 -pthread gtest_nvtuple.cpp -l gtest_main -l gtest -o gtest_nvtuple


clean:
	rm -rf gtest_nvtuple build *~ *.o *.a *.s 
