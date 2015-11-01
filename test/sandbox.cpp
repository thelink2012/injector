#include <injector/injector.hpp>
#include <string>

int main()
{
    int i = 400;
    std::printf("%d\n", i);
    if(!rand())
    {
        injector::write(std::nullptr_t(), 1337);
        injector::write((void*)(1), 1337);
        //injector::write(0, std::string());
    }
    std::printf("%d\n", i);
    //injector::write(injector::aslr_ptr(0x4F107E), 1337);
    //injector::write(injector::raw_ptr(0x4F107E), 1337);
    //injector::write(raw_ptr(0x4F107E), 1337);
    std::getchar();
}
