#include <core/object.h>
#include <core/utilities.h>
namespace platinum
{
    std::map<std::string, ObjectFactory::Constructor> ObjectFactory::_ctors;
    void ObjectFactory::RegisterClass(const std::string &type, const Constructor &construtor)
    {
        _ctors[type] = construtor;
    }

    Object *ObjectFactory::CreateInstance(const std::string &type, const PropertyNode &node)
    {
        if (_ctors.find(type) == _ctors.end())
        {
            LOG(ERROR) << "A constructor for class \"" << type << "\" could not be found!";
            return nullptr;
        }
        return _ctors[type](node);
    }
}