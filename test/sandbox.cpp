#include <injector/injector.hpp>
#include <string>

using namespace injector;

int main()
{
    int i = 400;
    std::printf("=%d\n", are_same_raw<raw_ptr>::value);
    if(!rand())
    {
        injector::write(std::nullptr_t(), 1337);
        injector::write((void*)(1), 1337);
        //injector::write(0, std::string());
        injector::adjust(0, 0, 0, injector::raw_ptr(nullptr));
    }
    //injector::write(injector::aslr_ptr(0x4F107E), 1337);
    //injector::write(injector::raw_ptr(0x4F107E), 1337);
    //injector::write(raw_ptr(0x4F107E), 1337);
    std::getchar();
}
