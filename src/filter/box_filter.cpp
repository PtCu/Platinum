#include <filter/box_filter.h>

namespace platinum
{
    REGISTER_CLASS(BoxFilter, "BoxFilter");

    BoxFilter(const PropertyNode &node) : Filter(node) {}
}