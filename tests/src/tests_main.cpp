#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include "arma3client.hpp"
#include "mod.hpp"
#include "steam.hpp"
#include "vdf.hpp"

// dirty hack to avoid classes being stripped out from binary
__attribute__((unused)) void do_not_strip_these()
{
    ARMA3::Client c("", "");
    Mod m;
    Steam s;
    VDF v;
}
