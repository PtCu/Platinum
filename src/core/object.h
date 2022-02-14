#ifndef CORE_OBJECT_H_
#define CORE_OBJECT_H_

#include <boost/property_tree/ptree.hpp>

#include <map>

namespace platinum
{

    using PropertyNode = boost::property_tree::ptree;

    //从配置文件中创建对象所需的基类
    class Object
    {
    public:
        enum class ClassType
        {
            Primitive = 0,
            Shape,
            Material,
            Light,
            Camera,
            Integrator,
            Sampler,
            Filter,
            Film,
            ClassTypeCount
        };

        virtual ~Object() = default;

        /**
		 * \brief Add a child object to the current instance
		 *
		 * The default implementation does not support children and
		 * simply throws an exception
		 */
        virtual void AddChild(Object *child) {}

        /**
		 * \brief Set the parent object
		 *
		 * Subclasses may choose to override this method to be
		 * notified when they are added to a parent object. The
		 * default implementation does nothing.
		 */
        virtual void SetParent(Object *parent) {}

        /**
		 * \brief Perform some action associated with the object
		 *
		 * The default implementation throws an exception. Certain objects
		 * may choose to override it, e.g. to implement initialization,
		 * testing, or rendering functionality.
		 *
		 * This function is called by the XML parser once it has
		 * constructed an object and added all of its children
		 * using \ref addChild().
		 */
        virtual void Activate() {}

        // Return a brief string summary of the instance (for debugging purposes)
        virtual std::string ToString() const = 0;

        // Turn a class type into a human-readable string
        static std::string GetClassTypeName(ClassType type)
        {
            switch (type)
            {
            case ClassType::Material:
                return "Material";
            case ClassType::Primitive:
                return "Primitive";
            case ClassType::Shape:
                return "Shape";
            case ClassType::Light:
                return "Light";
            case ClassType::Camera:
                return "Camera";
            case ClassType::Integrator:
                return "Integrator";
            case ClassType::Sampler:
                return "Sampler";
            case ClassType::Filter:
                return "Filter";
            case ClassType::Film:
                return "Film";
            default:
                return "Unknown";
            }
        }
    };

    class ObjectFactory
    {
    public:
        using Constructor = std::function<Object *(const PropertyNode &)>;

        static void RegisterClass(const std::string &type, const Constructor &construtor);

        static Object *CreateInstance(const std::string &type, const PropertyNode &node);

    private:
        static std::map<std::string, Constructor> _ctors;
    };

    //  Macro for registering an object constructor with the \ref AObjectFactory
    //  在CPP文件中注册具体的类。
    //  生成 xxx_类，并创建静态对象。类的构造函数负责调用RegisterClass，将xxx_create()函数注册。
    //  xxx_create()函数调用xxx的构造函数并返回构造对象的指针
    //  运行时注册。s
#define REGISTER_CLASS(cls, name)                             \
    inline cls *cls##_create(const PropertyNode &node)        \
    {                                                         \
        return new cls(node);                                 \
    }                                                         \
    class cls##_                                              \
    {                                                         \
    public:                                                   \
        cls##_()                                              \
        {                                                     \
            ObjectFactory::RegisterClass(name, cls##_create); \
        }                                                     \
    };                                                        \
    static cls##_ cls##_instance_;
}

#endif