#ifndef UNIT_CONVERTER__NO_CONVERSION_HPP
#define UNIT_CONVERTER__NO_CONVERSION_HPP

#include <UnitConverter/UnitConversionStrategy.hpp>

class NoConversion : public UnitConversionStrategy
{
  public:
    virtual ConversionResult Convert(double value) const override;
};

#endif
