#ifndef UNIT_CONVERTER__UNIT_CONVERTER_HPP
#define UNIT_CONVERTER__UNIT_CONVERTER_HPP

#include <UnitConverter/UnitConversionStrategy.hpp>
#include <memory>

class UnitConverter
{
  private:
    std::shared_ptr<UnitConversionStrategy> Strategy;

  public:
    UnitConverter(std::shared_ptr<UnitConversionStrategy> strategy) : Strategy(strategy)
    {
    }

    virtual ConversionResult Convert(double value) const
    {
        if (!Strategy)
        {
            throw std::runtime_error("Conversion strategy not set");
        }
        return Strategy->Convert(value);
    }
};

#endif
