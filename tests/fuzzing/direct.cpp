#include <filesystem>
#include <fstream>
#include <string>

#include <fmt/format.h>

std::string FileReadAllText(std::filesystem::path const &path)
{
    std::ifstream file(path);
    std::stringstream str;
    str << file.rdbuf();
    return str.str();
}

extern "C" int LLVMFuzzerTestOneInput(uint8_t const *data, size_t size);

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fmt::print("usage: {} INPUT_FILE\n", argv[0]);
        return 2;
    }

    auto file = FileReadAllText(argv[1]);
    uint8_t const *data = reinterpret_cast<uint8_t const *>(file.c_str());
    size_t size = file.size();
    LLVMFuzzerTestOneInput(data, size);
    return 0;
}
