#include <UnitConverter/BinaryPrefixConversion.hpp>

ConversionResult BinaryPrefixConversion::Convert(double value) const
{
    double originalValue = value;
    int index = 0;
    while (value >= 1024 && index < 6)
    {
        value /= 1024;
        ++index;
    }
    return {originalValue, value, Prefixes[index]};
}

const std::vector<std::string> BinaryPrefixConversion::Prefixes = {"", "Ki", "Mi", "Gi", "Ti", "Pi", "Ei"};
