
#include <exception_tuple.h>

void example_1(bool f) {
    try {
        if (f)
	  throw NT_EXCEPTION(("onemore"_ = 1234));
	//("xmsg"_ = "this is an example"));
        else
            throw NT_EXCEPTION();

    } catch (std::exception& e) {
        std::cout << "got exception: " << e.what() << '\n';
    }
}

int main(int argc, char**) {
    example_1(argc > 1);
    return 0;
}
