#include <core/object.h>
#include <core/utilities.h>
namespace platinum
{

    std::map<std::string, ObjectFactory::Constructor> &ObjectFactory::GetMap()
    {
        static std::map<std::string, Constructor> _ctors;
        return _ctors;
    }

    void ObjectFactory::RegisterClass(const std::string &type, const Constructor &construtor)
    {
        auto &_ctors = GetMap();
        _ctors.insert({type, construtor});
        LOG(INFO)
            << "Register " << type << "\n";
    }

    Object *ObjectFactory::CreateInstance(const std::string &type, const PropertyNode &node)
    {
        auto &_ctors = GetMap();
        if (_ctors.find(type) == _ctors.end())
        {
            LOG(ERROR) << "A constructor for class \"" << type << "\" could not be found!";
            return nullptr;
        }
        return _ctors[type](node);
    }
}