#include "mod_search.hpp"

#include <algorithm>
#include <string>

namespace
{
    constexpr std::string_view whitespace = " \t\n\r\f\v";

    // Only ASCII is case folded: mod names are mostly ASCII and this never splits UTF-8 sequences
    char ascii_lowercase(char const c)
    {
        if (c >= 'A' && c <= 'Z')
            return static_cast<char>(c - 'A' + 'a');
        return c;
    }

    bool contains_case_insensitive(std::string_view const text, std::string_view const lowercase_word)
    {
        auto const it = std::search(text.begin(), text.end(), lowercase_word.begin(), lowercase_word.end(),
                                    [](char const text_char, char const word_char)
        {
            return ascii_lowercase(text_char) == word_char;
        });
        return it != text.end();
    }
}

namespace ModSearch
{
    bool Matches(std::string_view const name, std::string_view const workshop_id, std::string_view const query)
    {
        std::string word;
        size_t position = query.find_first_not_of(whitespace);
        while (position != std::string_view::npos)
        {
            size_t const end = std::min(query.find_first_of(whitespace, position), query.size());
            word.assign(query.substr(position, end - position));
            std::transform(word.begin(), word.end(), word.begin(), ascii_lowercase);

            if (!contains_case_insensitive(name, word) && !contains_case_insensitive(workshop_id, word))
                return false;

            position = query.find_first_not_of(whitespace, end);
        }
        return true;
    }
}
