#include "SimpleCalculator.h"
#include "InMemoryHistory.h"
#include <gtest/gtest.h>
#include <iostream>
#include <climits>

using namespace calc;

struct CalculatorTest : public ::testing::Test
{
    static void SetUpTestSuite() 
    {
        std::cout << "Prepairing the test environment for Google Tests..." << std::endl;
    }

    void SetUp() override 
    {
        auto calculator_history = std::make_shared<InMemoryHistory>();
        calculator = std::make_unique<SimpleCalculator>(calculator_history);
    }

    void TearDown() override 
    {
        calculator.reset();
    }

    static void TearDownTestSuite()
    {
        std::cout << "Cleaning up testing zone..." << std::endl;
    }

    std::unique_ptr<SimpleCalculator> calculator;
};

struct TestCase 
{
    int a;
    int b;
    int result;
    std::string exception_explanation;
};

class Parametrized_Add_Overflow_Test : public CalculatorTest, public ::testing::WithParamInterface<TestCase> {};
class Parametrized_Sub_Overflow_Test : public CalculatorTest, public ::testing::WithParamInterface<TestCase> {};
class Parametrized_Mul_Overflow_Test : public CalculatorTest, public ::testing::WithParamInterface<TestCase> {};

INSTANTIATE_TEST_SUITE_P(
    Default,
    Parametrized_Add_Overflow_Test,
    ::testing::Values(
        TestCase{INT_MAX, 1, INT_MIN, "Fix implementation to throw std::overflow_error"},
        TestCase{INT_MIN,-1, INT_MAX, "Fix implementation to throw std::overflow_error"}
    )
);

INSTANTIATE_TEST_SUITE_P(
    Default,
    Parametrized_Sub_Overflow_Test,
    ::testing::Values(
        TestCase{INT_MAX,-1, INT_MIN, "Fix implementation to throw std::overflow_error"},
        TestCase{INT_MAX, INT_MAX, 0, "Fix implementation to throw std::overflow_error"}
    )
);

INSTANTIATE_TEST_SUITE_P(
    Default,
    Parametrized_Mul_Overflow_Test,
    ::testing::Values(
        TestCase{INT_MAX, 2, -2, "Fix implementation to throw std::overflow_error"},
        TestCase{INT_MIN, 2,  0, "Fix implementation to throw std::overflow_error"}
    )
);

TEST_F(CalculatorTest, Add_Check) 
{
    ASSERT_EQ(calculator->Add(0, 0), 0);
    EXPECT_EQ(calculator->Add(3, 5), 8);
    EXPECT_EQ(calculator->Add(10, -5), 5);
    EXPECT_EQ(calculator->Add(100, 0), 100);
    EXPECT_EQ(calculator->Add(INT_MIN, 0), INT_MIN);
}

TEST_F(CalculatorTest, Sub_Check) 
{
    ASSERT_EQ(calculator->Subtract(0, 0), 0);
    EXPECT_EQ(calculator->Subtract(20, 3), 17);
    EXPECT_EQ(calculator->Subtract(2121, 0), 2121);
    EXPECT_EQ(calculator->Subtract(100, -100), 200);
    EXPECT_EQ(calculator->Subtract(INT_MAX, INT_MAX), 0);
}

TEST_F(CalculatorTest, Mul_Check) 
{
    ASSERT_EQ(calculator->Multiply(0, 0), 0);
    EXPECT_EQ(calculator->Multiply(10, 90), 900);
    EXPECT_EQ(calculator->Multiply(-20, -4), 80);
    EXPECT_EQ(calculator->Multiply(90, -2), -180);
    EXPECT_EQ(calculator->Multiply(INT_MIN, 0), 0);
}

TEST_F(CalculatorTest, Div_Check) 
{
    ASSERT_EQ(calculator->Divide(0, 100), 0);
    EXPECT_EQ(calculator->Divide(100, 199), 0);
    EXPECT_EQ(calculator->Divide(-231, 230), -1);
    EXPECT_EQ(calculator->Divide(222, 222), 1);
    EXPECT_EQ(calculator->Divide(INT_MAX, 1), INT_MAX);
}

TEST_F(CalculatorTest, Division_By_Zero) 
{
    ASSERT_THROW(calculator->Divide(0, 0), std::invalid_argument) << "Division by zero";
}

TEST_F(CalculatorTest, Watch_History_Logs)
{
    std::shared_ptr<InMemoryHistory> calculator_history = std::make_shared<InMemoryHistory>();
    calculator->SetHistory(calculator_history);

    calculator->Add(20, 30);
    calculator->Subtract(22, 12);
    calculator->Multiply(100, 50);
    calculator->Divide(23, 7);

    std::vector<std::string> logs = calculator_history->GetLastOperations(4);

    ASSERT_EQ(logs[0], "20 + 30 = 50");
    EXPECT_EQ(logs[1], "22 - 12 = 10");
    EXPECT_EQ(logs[2], "100 * 50 = 5000");
    EXPECT_EQ(logs[3], "23 / 7 = 3");
}

TEST_F(CalculatorTest, Watch_Not_Existed_Logs) 
{
    std::shared_ptr<InMemoryHistory> calculator_history = std::make_shared<InMemoryHistory>();
    calculator->SetHistory(calculator_history);

    calculator->Add(-10, 10);
    calculator->Subtract(10, 20);
    calculator->Multiply(25, 5);
    calculator->Divide(20, 5);

    EXPECT_ANY_THROW(calculator_history->GetLastOperations(5)) << "Fix implementation to throw std::out_of_range";
    EXPECT_ANY_THROW(calculator_history->GetLastOperations(5)[10]) << "Fix implementation to throw std::out_of_range";
}

TEST_P(Parametrized_Add_Overflow_Test, Add_With_Overflow) 
{
    auto params = GetParam();
    EXPECT_THROW(calculator->Add(params.a, params.b), std::overflow_error) << params.exception_explanation;
}

TEST_P(Parametrized_Sub_Overflow_Test, Sub_With_Overflow) 
{
    auto params = GetParam();
    EXPECT_THROW(calculator->Subtract(params.a, params.b), std::overflow_error) << params.exception_explanation;
}

TEST_P(Parametrized_Mul_Overflow_Test, Mul_With_Overflow) 
{
    auto params = GetParam();
    EXPECT_THROW(calculator->Multiply(params.a, params.b), std::overflow_error) << params.exception_explanation;
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}