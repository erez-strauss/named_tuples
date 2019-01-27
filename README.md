# named_tuples
C++ named tuple

# motivation

The need for simple on the fly creation of named tuple with out much of overhead and remove the magic behind the first and second fields of std::pair<> or the index of the members in std::tuple<>

With named tuples one can do much better, for example, create named fields pair:

 auto t = std::make_tuple ( ("addr"_ = p), ("size"_ = n) );
 return t;

 and the caller of this function will get a tuple with pointer and size, with their names
 access to fields is using [] operator:  for example:
 p = t["arrd"_];
 or setting using t["addr"_] = p;
 the operator [] is compile time functions, without any runtime overhead.
 the type names are all external to the data, so now data overhead enccured by using this header.

 To simplify the usage of the named tuple, the << operator was defined for named fields and named tuples

 so one can do

 std::cout << "t: " << t << std::endl;

 the implementation is goals are:

 1. no runtime overhead
 2. no additional data in side the tuple - same memory layout for named_tuple<> and std::tuple<>
 3. access tuple members using [] operator that does not add overhead, see example#2
 4. the named tuple enable printing of the fields with their names
 

# next tasks:

1. namespace - for named tuples
2. do two different namespaces for the ""_ and ""_t   literals
3. 

# external refrences:
In order to complete this header and examples file, I read the following refrences:

1. https://stackoverflow.com/questions/6245735/pretty-print-stdtuple, by AndyG and others

2. http://blogs.microsoft.co.il/sasha/2015/01/28/implementing-tuple-part-4/

3. https://alexpolt.github.io/named-tuple.html

