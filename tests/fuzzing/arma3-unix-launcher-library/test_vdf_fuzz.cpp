#include "vdf.hpp"

#include <cstdint>

#include "exceptions/syntax_error.hpp"

extern "C" int LLVMFuzzerTestOneInput(uint8_t const *data, size_t size)
{
    std::string_view view(reinterpret_cast<char const *>(data), size);
    try
    {
        VDF v;
        v.LoadFromText(view);
    }
    catch (SyntaxErrorException const &e)
    {
    }
    return 0;
}
