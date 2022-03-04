#ifndef CORE_OBJECT_H_
#define CORE_OBJECT_H_

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <optional>
#include <map>
#include <core/utilities.h>

namespace platinum
{

    using PropertyNode = boost::property_tree::ptree;

    class PropertyTree
    {

    public:
        PropertyTree() = default;

        PropertyTree(const boost::property_tree::ptree &node) : _root(node) {}

        template <typename T>
        T Get(const std::string &name) const
        {
            return _root.get<T>(name);
        }

        template <typename T>
        boost::optional<T> GetOptional(const std::string &name) const
        {
            return _root.get_optional<T>(name);
        }

        template <typename T>
        T Get(const std::string &name, const T &defaultValue) const
        {
            return _root.get<T>(name, defaultValue);
        }

        template <>
        Vector2f Get<Vector2f>(const std::string &name) const
        {
            Vector2f ans;
            auto iter = _root.get_child(name).begin();
            ans.x = (iter++)->second.get_value<float>();
            ans.y = iter->second.get_value<float>();
            return ans;
        }

        template <>
        Vector3f Get<Vector3f>(const std::string &name) const
        {
            Vector3f ans;
            auto iter = _root.get_child(name).begin();
            ans.x = (iter++)->second.get_value<float>();
            ans.y = (iter++)->second.get_value<float>();
            ans.z = iter->second.get_value<float>();
            return ans;
        }

        PropertyTree GetChild(const std::string &name) const
        {
            auto child = _root.get_child(name);
            return PropertyTree(child);
        }

        boost::optional<PropertyTree> GetChildOptional(const std::string &name) const
        {
            auto child = _root.get_child_optional(name);
            if (!child)
                return boost::none;
            return PropertyTree(child.get());
        }

        void ReadJson(const std::string &path)
        {
            boost::property_tree::read_json(path, _root);
        }

        boost::property_tree::ptree GetNode() const
        {
            return _root;
        }

        boost::property_tree::ptree::iterator begin()
        {
            return _root.begin();
        }

        boost::property_tree::ptree::iterator end()
        {
            return _root.end();
        }

        boost::property_tree::ptree _root;
    };

    // class PropertyIterator
    // {
    // public:
    //     explicit PropertyIterator(const PropertyTree &p) : _tree(&p) {}

    //     PropertyIterator operator++()
    //     {
    //         _tree->_root;
    //     }

    //     bool operator!=(const PropertyIterator &other) const
    //     {
    //         return other._tree->_root != this->_tree->_root;
    //     }

    // private:
    //     const PropertyTree *_tree;
       
    // };

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
        using Constructor = std::function<Object *(const PropertyTree &)>;

        static void RegisterClass(const std::string &type, const Constructor &construtor);

        static Object *CreateInstance(const std::string &type, const PropertyTree &node);

    private:
        static std::map<std::string, Constructor> &GetMap();
    };

    //  Macro for registering an object constructor with the \ref AObjectFactory
    //  在CPP文件中注册具体的类。
    //  生成 xxx_类，并创建静态对象。类的构造函数负责调用RegisterClass，将xxx_create()函数注册。
    //  xxx_create()函数调用xxx的构造函数并返回构造对象的指针
    //  运行时注册。

#define REGISTER_CLASS(cls, name)                             \
    inline cls *cls##_create(const PropertyTree &node)        \
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
