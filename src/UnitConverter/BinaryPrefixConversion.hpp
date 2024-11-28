#ifndef UNIT_CONVERTER__BINARY_PREFIX_CONVERSION_HPP
#define UNIT_CONVERTER__BINARY_PREFIX_CONVERSION_HPP

#include <UnitConverter/UnitConversionStrategy.hpp>
#include <vector>

class BinaryPrefixConversion : public UnitConversionStrategy
{
  private:
    static const std::vector<std::string> Prefixes;

  public:
    virtual ConversionResult Convert(double value) const override;
};

#endif
