# named_tuples
C++ named tuple

# motivation

The need for simple on the fly creation of named tuple without much of overhead and remove the magic behind the first and second fields of std::pair<> or the index of the members in std::tuple<>

With named tuples one can do much better, for example, create named fields pair:

 auto t = std::make_tuple ( ("addr"_ = p), ("size"_ = n) );
 return t;

 and the caller of this function will get a tuple with pointer and size, with their names
 access to fields is using [] operator:  for example:
 p = t[ "adrr"\_];
 or setting using t["addr"\_] = p;
 the operator [] is compile time functions, without any runtime overhead.
 the type names are all external to the data, so now data overhead enccured by using this header.

 To simplify the usage of the named tuple, the << operator was defined for named fields and named tuples

 so one can do

 std::cout << "t: " << t << std::endl;

 the implementation is goals are:

 1. no runtime overhead
 2. no additional data in side the tuple - same memory layout for named_tuple<> and std::tuple<>
 3. access tuple members using [] operator that does not add overhead, see examples
 4. the named tuple enable printing of the fields with their names
 5. field names verification at compile time
 6. no MACROS, implement everything as C++ 17 and beyond.
 

# examples:

    auto t1 = std::make_tuple(("fieldA"_ = 123), ("B"_ = 100.111), ("lastone"_ = 999));
    std::cout << "example 1:\n  " << t1 << '\n';
example 1:
  (fieldA: 123, B: 100.111, lastone: 999)

    auto t2 = named_tuple {("fieldA"_ = 123), ("B"_ = 100.111), ("lastone"_ = 999) };
    std::cout << "example 2:\n  " << t2 << '\n';
    t2["fieldA"_] = 321;
    std::cout << "  changed fieldA: " << t2 << '\n';
example 2:
  (fieldA: 123, B: 100.111, lastone: 999)
  changed fieldA: (fieldA: 321, B: 100.111, lastone: 999)
example 3:
  Data: (hello, 555)
example 4:
  abc: Namedtype: abc
   xyz: 42
deep tuple: (Hello, 0.1, (1, 2, 3, four, 5.5), Z)
named tuple: (ax: 5, by: 3.4, str1: hello world, str2: blabla2)
deep deep 1: (a1: 33, deeptuple: (ax: 5, by: 3.4, str1: hello world, str2: blabla2))
tx two int: (r: 5, i: 1) sizeof(tx): 8
tx : r: 5 sizeof(tx): 8
tx two int: (r: 3, i: 1) sizeof(tx): 8
one value named tuple: (field1: 2.1)
named tuple: (fieldX: 123, fieldY: test me)
nt1.find_field_index : 0
n1.get_index<>(): 0
fieldX: 123
fieldX: 123
fieldY: test me


# next tasks:

1. namespace - for named tuples
2. do two different namespaces for the ""_ and ""_t   literals
4. copy tuples by field types.

# external refrences:

In order to complete this header and examples file, I read the following refrences:

1. https://stackoverflow.com/questions/6245735/pretty-print-stdtuple, by AndyG and others

2. http://blogs.microsoft.co.il/sasha/2015/01/28/implementing-tuple-part-4/

3. https://alexpolt.github.io/named-tuple.html

