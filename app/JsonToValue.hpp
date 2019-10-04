#include <nlohmann/json.hpp>
#include <memory>
#include <exprtk.hpp>
#include <glm/glm.hpp>
namespace jsonexpr
{
    class AbstractValue
    {
    public:
        virtual void set(const nlohmann::json& jsValue) = 0;
        virtual void update() = 0;
    };
    class Expression : public AbstractValue
    {
    public:
        //Useless
        virtual void set(const nlohmann::json& jsValue)
        {}
        virtual void update() 
        { m_expr.value(); }
        // void set(exprtk::expression<float> expr)
        // { m_expr = std::move(expr); }
        exprtk::expression<float>& get()
        { return m_expr; }
    private:
        exprtk::expression<float> m_expr;
    };
    template<typename ValueType>
    class Value : public AbstractValue
    {
    public:
        Value(ValueType& v) : m_value(v)
        {}
        virtual void set(const nlohmann::json& jsValue);
        virtual void update()
        {
            for(auto& v : m_subvalue)
                v->update();
        }
    protected:
        template <int len>
        void define_vec();
        ValueType& m_value;
        std::vector<std::unique_ptr<AbstractValue>> m_subvalue;
    };
}