#ifndef UNIT_CONVERTER__CONVERSION_RESULT_HPP
#define UNIT_CONVERTER__CONVERSION_RESULT_HPP

#include <string>

struct ConversionResult
{
    const double OriginalValue;
    const double ConvertedValue;
    const std::string Unit;
};

#endif
