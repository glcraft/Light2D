#include <nlohmann/json.hpp>
#include <memory>
#include <glm/glm.hpp>
namespace jsonexpr
{
    void add_global_variable(std::string name, float& value);
    class AbstractValue
    {
    public:
        virtual void set(const nlohmann::json& jsValue) = 0;
        virtual void update() = 0;
    };
    
    template<typename ValueType>
    class StaticValue : public AbstractValue
    {
    public:
        using value_type = ValueType;
        StaticValue(ValueType& v) : m_value(v)
        {}
        StaticValue(ValueType& v, const nlohmann::json& jsValue) : m_value(v)
        {
            set(jsValue);
        }
        void set(const nlohmann::json& jsValue) override
        {
            m_constantValue = jsValue;
        }
        void update() override
        { m_value = m_constantValue; }
    protected:
        ValueType& m_value;
        ValueType m_constantValue;
    };
    template<typename ValueType>
    class Value : public AbstractValue
    {
    public:
        using value_type = ValueType;
        Value(ValueType& v) : m_value(v)
        {}
        Value(ValueType& v, const nlohmann::json& jsValue) : m_value(v)
        {
            set(jsValue);
        }
        void set(const nlohmann::json& jsValue) override;
        virtual void update()
        {
            for(auto& v : m_subvalue)
                v->update();
        }
    protected:
        ValueType& m_value;
        std::vector<std::unique_ptr<AbstractValue>> m_subvalue;
    };
}