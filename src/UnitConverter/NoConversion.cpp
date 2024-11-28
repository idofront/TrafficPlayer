#include <UnitConverter/NoConversion.hpp>

ConversionResult NoConversion::Convert(double value) const
{
    return {value, value, ""};
}
