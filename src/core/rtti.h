#ifndef CORE_RTTI_H_
#define CORE_RTTI_H_

#include "platinum.h"
#include "math_utils.h"
#include "transform.h"

#include <functional>
#include <map>
#include <array>
#include <sstream>

namespace platinum
{

    class PropertyList final
    {
    public:
        PropertyList() = default;

        void set(const std::string &name, const std::string &value);
        void set(const std::string &name, const std::vector<std::string> &values);

        bool has(const std::string &name) const;

        bool getBoolean(const std::string &name) const;
        bool getBoolean(const std::string &name, const bool &defaultValue) const;
        Float getFloat(const std::string &name) const;
        Float getFloat(const std::string &name, const Float &defaultValue) const;
        int getInteger(const std::string &name) const;
        int getInteger(const std::string &name, const int &defaultValue) const;
        std::string getString(const std::string &name) const;
        std::string getString(const std::string &name, const std::string &defaultValue) const;
        Vector2f getVector2f(const std::string &name) const;
        Vector2f getVector2f(const std::string &name, const Vector2f &defaultValue) const;
        Vector3f getVector3f(const std::string &name) const;
        Vector3f getVector3f(const std::string &name, const Vector3f &defaultValue) const;
        std::vector<Float> getVectorNf(const std::string &name) const;
        std::vector<Float> getVectorNf(const std::string &name, const std::vector<Float> &defaultValue) const;

    private:
        /* Custom variant data type */
        class Property final
        {
        public:
            Property() : values({}) {}
            bool empty() const { return values.empty(); }
            size_t size() const { return values.size(); }
            void addValue(const std::string &value) { values.push_back(value); }
            void setValue(const std::vector<std::string> &value_list) { values = value_list; }
            std::string &operator[](const size_t &index)
            {
                CHECK_LT(index, values.size());
                return values[index];
            }
            const std::string &operator[](const size_t &index) const
            {
                CHECK_LT(index, values.size());
                return values[index];
            }

        private:
            std::vector<std::string> values;
        };

        //每个Property实际上有许多string类型的values
        std::map<std::string, Property> m_properties;

        template <typename Type>
        Type get(const std::string &name, const size_t &index) const
        {
            auto it = m_properties.find(name);
            if (it == m_properties.end())
                LOG(ERROR) << "Property \"" << name << "\" is missing!";

            //Note: only support float, int, bool and string
            auto valueStr = it->second[index];
            std::istringstream iss(valueStr);
            Type value;
            iss >> value;
            return value;
        }

        //特化
        template <>
        bool get<bool>(const std::string &name, const size_t &index) const
        {
            auto it = m_properties.find(name);
            if (it == m_properties.end())
                LOG(ERROR) << "Property \"" << name << "\" is missing!";

            bool value = it->second[index] == "true" ? true : false;
            return value;
        }

        template <typename Type>
        Type get(const std::string &name, const size_t &index, const Type &defaulValue) const
        {
            auto it = m_properties.find(name);
            if (it == m_properties.end())
                return defaulValue;
            //Note: only support float, int, bool and string
            auto valueStr = it->second[index];
            std::istringstream iss(valueStr);
            Type value;
            iss >> value;
            return value;
        }

        template <>
        bool get<bool>(const std::string &name, const size_t &index, const bool &defaulValue) const
        {
            auto it = m_properties.find(name);
            if (it == m_properties.end())
                return defaulValue;
            bool value = it->second[index] == "true" ? true : false;
            return value;
        }

        template <size_t N>
        std::array<Float, N> getVector(const Property &prop) const
        {
            std::array<Float, N> values;
            auto get_func = [&](const size_t &index) -> Float {
                auto valueStr = prop[index];
                std::istringstream iss(valueStr);
                Float value;
                iss >> value;
                return value;
            };

            for (size_t i = 0; i < N; ++i)
                values[i] = get_func(i);

            return values;
        }
    };

    //For parser
    class PropertyTreeNode final
    {
    public:
        PropertyTreeNode(const std::string &nodeName) : m_nodeName(nodeName) {}

        std::string getTypeName() const;
        const PropertyList &getPropertyList() const;
        const std::string &getNodeName() const { return m_nodeName; }
        const PropertyTreeNode &getPropertyChild(const std::string &name) const;

        bool hasProperty(const std::string &name) const;
        bool hasPropertyChild(const std::string &name) const;

        void addProperty(const std::string &name, const std::string &value);
        void addProperty(const std::string &name, const std::vector<std::string> &values);
        void addChild(const PropertyTreeNode &child);

        static std::string m_directory;

    private:
        std::string m_nodeName;
        PropertyList m_property;
        std::vector<PropertyTreeNode> m_children;
    };
    /**
	 * \brief Base class of all objects
	 *
	 * A Nori object represents an instance that is part of
	 * a scene description, e.g. a camera or intergrator.
	 */
    class Object
    {
    public:
        enum ClassType
        {
            Hitable = 0,
            Shape,
            Material,
            Light,
            Camera,
            Integrator,
            Sampler,
            Filter,
            Film,
            Entity,
            ClassTypeCount
        };

        virtual ~Object() = default;

        virtual ClassType getClassType() const = 0;

        /**
		 * \brief Add a child object to the current instance
		 *
		 * The default implementation does not support children and
		 * simply throws an exception
		 */
        virtual void addChild(Object *child);

        /**
		 * \brief Set the parent object
		 *
		 * Subclasses may choose to override this method to be
		 * notified when they are added to a parent object. The
		 * default implementation does nothing.
		 */
        virtual void setParent(Object *parent);

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
        virtual void activate();

        // Return a brief string summary of the instance (for debugging purposes)
        virtual std::string toString() const = 0;

        // Turn a class type into a human-readable string
        static std::string getClassTypeName(ClassType type)
        {
            switch (type)
            {
            case Material:
                return "Material";
            case Hitable:
                return "Hitable";
            case Shape:
                return "Shape";
            case Light:
                return "Light";
            case Camera:
                return "Camera";
            case Integrator:
                return "Integrator";
            case Sampler:
                return "Sampler";
            case Filter:
                return "Filter";
            case Film:
                return "Film";
            case Entity:
                return "Entity";
            default:
                return "Unknown";
            }
        }
    };

    /**
	 * \brief Factory for Objects
	 *
	 * This utility class is part of a mini-RTTI framework and can
	 * instantiate arbitrary Objects by their name.
	 */
    class ObjectFactory
    {
    public:
        typedef std::function<Object *(const PropertyTreeNode &)> Constructor;

        /**
		 * \brief Register an object constructor with the object factory
		 *
		 * This function is called by the macro \ref PLT_REGISTER_CLASS
		 *
		 * \param name
		 *     An internal name that is associated with this class. This is the
		 *     'type' field found in the scene description XML files
		 *
		 * \param constr
		 *     A function pointer to an anonymous function that is
		 *     able to call the constructor of the class.
		 */
        static void registerClass(const std::string &type, const Constructor &constr);

        /**
		 * \brief Construct an instance from the class of the given name
		 *
		 * \param name
		 *     An internal name that is associated with this class. This is the
		 *     'type' field found in the scene description XML files
		 *
		 * \param propList
		 *     A list of properties that will be passed to the constructor
		 *     of the class.
		 */
        static Object *createInstance(const std::string &type, const PropertyTreeNode &node);

    private:
        //静态变量，记录string类型的type和对于的构造函数
        static std::map<std::string, Constructor> &getConstrMap();
    };

}

#endif