// (C) PROTEI protei.com

#pragma once

#include <memory>

namespace calc
{

class IHistory;

class ICalculator
{
public:
    virtual ~ICalculator() = default;

    virtual int Add(int a, int b) = 0;
    virtual int Subtract(int a, int b) = 0;
    virtual int Multiply(int a, int b) = 0;
    virtual int Divide(int a, int b) = 0;

    virtual void SetHistory(std::shared_ptr<IHistory> history) = 0;
};

} // namespace calc