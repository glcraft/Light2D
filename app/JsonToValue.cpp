#include "JsonToValue.hpp"
#include <sstream>
inline std::string get_value(const nlohmann::json& jV)
{
    return (jV.is_number() ? std::to_string(jV.get<float>()) : jV.get<std::string>());
}
template<>
void JSONToValue<glm::vec2>::set(nlohmann::json jsValue)
{
    exprtk::parser<float> ExprParser;
    if (jsValue.is_null())
        return;
    std::ostringstream expr;
    m_symbol.add_variable("x",m_value.x);
    m_symbol.add_variable("y",m_value.y);
    for (const char* str : {"x", "y"})
        expr << str<< ":=" << get_value(jsValue[str])<<";";
    m_expr.register_symbol_table(m_symbol);
    bool test = ExprParser.compile(expr.str(), m_expr);
    update();
}
template<>
void JSONToValue<glm::vec3>::set(nlohmann::json jsValue)
{
    exprtk::parser<float> ExprParser;
    if (jsValue.is_null())
        return;
    std::ostringstream expr;
    m_symbol.add_variable("r",m_value.r);
    m_symbol.add_variable("g",m_value.g);
    m_symbol.add_variable("b",m_value.b);
    for (const char* str : {"r", "g", "b"})
        expr << str<< ":=" << get_value(jsValue[str])<<";";
    m_expr.register_symbol_table(m_symbol);
    bool test = ExprParser.compile(expr.str(), m_expr);
    update();
}
template<>
void JSONToValue<float>::set(nlohmann::json jsValue)
{
    exprtk::parser<float> ExprParser;
    if (jsValue.is_null())
        return;
    std::ostringstream expr;
    m_symbol.add_variable("x",m_value);
    expr << "x"<< ":=" << get_value(jsValue)<<";";
    m_expr.register_symbol_table(m_symbol);
    bool test = ExprParser.compile(expr.str(), m_expr);
    update();
}