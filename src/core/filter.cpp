#include "filter.h"

PLATINUM_BEGIN

Filter::Filter(const PropertyList &props) : m_radius(props.getVector2f("Radius", Vector2f(0.5f))),
                                            m_invRadius(Vector2f(1 / m_radius.x, 1 / m_radius.y)) {}

PLATINUM_END