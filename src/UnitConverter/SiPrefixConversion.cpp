#include <UnitConverter/SiPrefixConversion.hpp>

ConversionResult SiPrefixConversion::Convert(double value) const
{
    double originalValue = value;
    int index = 0;
    while (value >= 1000 && index < 6)
    {
        value /= 1000;
        ++index;
    }
    return {originalValue, value, Prefixes[index]};
}
const std::vector<std::string> SiPrefixConversion::Prefixes = {"", "k", "M", "G", "T", "P", "E"};
