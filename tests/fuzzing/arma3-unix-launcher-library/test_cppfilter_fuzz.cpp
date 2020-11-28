#include "cppfilter.hpp"

#include <stdexcept>
#include <vector>

#include "exceptions/syntax_error.hpp"

extern "C" int LLVMFuzzerTestOneInput(uint8_t const *data, size_t size)
{
    std::string_view view(reinterpret_cast<char const *>(data), size);
    try
    {
        CppFilter cpp{std::string(view)};
        cpp.RemoveClass("class any");
    }
    catch (SyntaxErrorException const &e)
    {
    }
    catch (std::out_of_range const &e)
    {
    }
    return 0;
}
