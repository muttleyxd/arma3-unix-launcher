#pragma once
#include <doctest/doctest.h>

struct DefaultTestReporter : public doctest::IReporter
{
    std::ostream& stdout_stream;
    doctest::ContextOptions const& opt;

    DefaultTestReporter(doctest::ContextOptions const& in)
            : stdout_stream(*in.cout)
            , opt(in) {}

    void test_case_start(doctest::TestCaseData const& in) override
    {
        stdout_stream << "[TEST STARTED] " << in.m_name << '\n';
    }

    void test_case_end(doctest::CurrentTestCaseStats const& in) override
    {
        if (in.failure_flags == 0)
            stdout_stream << "[TEST SUCCESS]\n";
        else
            stdout_stream << "[TEST FAILURE]\n";
    }

    /* useless functions because virtual interface is purely virtual :D */
    void report_query(doctest::QueryData const&) override {}
    void test_run_start() override {}
    void test_run_end(doctest::TestRunStats const&) override {}
    void test_case_reenter(doctest::TestCaseData const&) override {}
    void test_case_exception(doctest::TestCaseException const&) override {}
    void subcase_start(doctest::SubcaseSignature const&) override {}
    void subcase_end() override {}
    void log_assert(doctest::AssertData const&) override {}
    void log_message(doctest::MessageData const&) override {}
    void test_case_skipped(doctest::TestCaseData const&) override {}
};
REGISTER_LISTENER("gtest_alike_printer", 1, DefaultTestReporter);
