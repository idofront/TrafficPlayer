#ifndef UNIT_CONVERTER__UNIT_CONVERSION_STRATEGY_HPP
#define UNIT_CONVERTER__UNIT_CONVERSION_STRATEGY_HPP

#include <UnitConverter/ConversionResult.hpp>

class UnitConversionStrategy
{
  public:
    virtual ~UnitConversionStrategy() = default;
    virtual ConversionResult Convert(double value) const = 0;
};

#endif
