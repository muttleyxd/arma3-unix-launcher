#include "mod.hpp"

#include <vector>

namespace FilesystemUtils
{
    std::vector<std::string> Ls(std::filesystem::path const &, bool)
    {
        return {"addons", "test.cpp"};
    }
}

static std::string return_value{};
namespace StdUtils
{
    std::string FileReadAllText(std::filesystem::path const &)
    {
        return return_value;
    }
}

extern "C" int LLVMFuzzerTestOneInput(uint8_t const *data, size_t size)
{
    std::string_view view(reinterpret_cast<char const *>(data), size);
    return_value = view;
    try
    {
        Mod m("/mod_path");
    }
    catch (std::exception const &e)
    {
    }

    return 0;
}
