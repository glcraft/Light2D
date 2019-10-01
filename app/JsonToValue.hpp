#include <nlohmann/json.hpp>
#include <exprtk.hpp>
#include <glm/glm.hpp>
template<typename ValueType>
class JSONToValue
{
public:
    JSONToValue() : m_value()
    {}
    template <typename ...Args>
    JSONToValue(Args... args) : m_value(args...)
    {}
    void set(nlohmann::json jsValue);
    void update()
    {
        m_expr.value();
    }
    operator ValueType()
    {
        return m_value;
    }
private:
    // nlohmann::json m_jsValue;
    exprtk::expression<float> m_expr;
    exprtk::symbol_table<float> m_symbol;
    ValueType m_value;
};
