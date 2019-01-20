/*
MIT License

Copyright (c) 2018 Aurelien Regat-Barrel

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef CPP_STATIC_TODO_HPP
#define CPP_STATIC_TODO_HPP

// Display errors only when testing
#ifdef DOCTEST_CONFIG_DISABLE

#define TODO_BEFORE(month, year, msg)
#define FIXME_BEFORE(month, year, msg)

#else

// Returns the year from the current build date
constexpr int current_build_year() {
    // exemple: "Nov 27 2018"
    const char *year = __DATE__;

    // skip the 2 first spaces
    int nbSpaces = 0;
    while (nbSpaces < 2) {
        if (*year == ' ') {
            nbSpaces++;
        }
        year++;
    }

    return (year[0] - '0') * 1000 + (year[1] - '0') * 100 + (year[2] - '0') * 10 + (year[3] - '0');
}

// Returns the month number (1..12) from the current build date
constexpr int current_build_month() {
    constexpr const char months[12][4]{
        "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    // exemple: "Nov 27 2018"
    constexpr const char date[] = __DATE__;

    // find matching month
    for (int i = 0; i < 12; i++) {
        const char *m = months[i];
        if (m[0] == date[0] && m[1] == date[1] && m[2] == date[2]) {
            return i + 1;
        }
    }

    return 0; // will be caught by the static_assert() check on month value
}

/// TODO_BEFORE() inserts a compilation "time bomb" that will trigger a "TODO" build error a soon as
/// the given date is reached.
///
/// This is useful to force attention on a specific piece of code that should not been forgotten
/// among a growing list of many other "TODO" comments...
///
/// Example:
///     TODO_BEFORE(01, 2019, "refactor to use std::optional<> once we compile in C++17 mode");
#define TODO_BEFORE(month, year, msg)                                                              \
    static_assert((year >= 2018 && month > 0 && month <= 12) &&                                    \
                      (current_build_year() < year ||                                              \
                          (current_build_year() == year && current_build_month() < month)),        \
        "TODO: " msg)

/// FIXME_BEFORE() works the same way than TODO_BEFORE() but triggers a "FIXME" error instead
#define FIXME_BEFORE(month, year, msg)                                                             \
    static_assert((year >= 2018 && month > 0 && month <= 12) &&                                    \
                      (current_build_year() < year ||                                              \
                          (current_build_year() == year && current_build_month() < month)),        \
        "FIXME: " msg)

#endif

#endif
