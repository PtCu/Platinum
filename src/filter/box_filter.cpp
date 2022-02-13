#include <filter/box_filter.h>

namespace platinum
{
    REGISTER_CLASS(BoxFilter, "Box");

    BoxFilter(const PropertyNode &node) : Filter(node) {}
}