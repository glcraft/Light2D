#include "JsonToValue.hpp"
#include <sstream>
#include <array>
#include <liblight.h>
#include <regex_literals.h>
inline std::string get_value(const nlohmann::json& jV)
{
    return (jV.is_number() ? std::to_string(jV.get<float>()) : jV.get<std::string>());
}
#if 0
template <typename ValueType>
struct ValueParams
{
    struct_rm<char> rg;
    const nlohmann::json& jsValue;
    exprtk::symbol_table<float>& symbol;
    ValueType& m_value;
    nlohmann::json tableName;
    std::string exec()
    {
        std::ostringstream exprStr;
        if (jsValue.is_string())
        {
            std::string s = jsValue;
            std::smatch sm;
            if (rg.match(s, sm))
            {
                std::string names[3]={sm[1], sm[2], sm[3]};
                std::string expr;
                for (int i=0;i<3;i++)
                {
                    symbol.add_variable(sm[i+1],m_value[i]);
                }
                exprStr << sm[4];
            }
        }
        else if (jsValue.is_number())
        {
            
        }
        else if (jsValue.is_object())
        {
            for (nlohmann::json arrName : tableName)
            {
                if (jsValue.find(arrName[0])!=jsValue.end())
                {
                    for (int i=0;i<m_value.len;i++)
                    {
                        symbol.add_variable(arrName[i],m_value[i]);
                        exprStr << arrName[i] << ":=" << get_value(jsValue[arrName[i].get<std::string>()]) << ";";
                    }
                    break;
                }
            }
        }
        return exprStr.str();
    }
};
#endif
namespace jsonexpr
{
    template<>
    void Value<li::Light>::set(const nlohmann::json& jsValue)
    {
        m_subvalue.clear();
        if (jsValue.is_null())
            return;
        std::unique_ptr<Value<glm::vec2>> ptrPos(new Value<glm::vec2>(m_value.m_position));
        std::unique_ptr<Value<glm::vec3>> ptrColor(new Value<glm::vec3>(m_value.m_color));
        // std::unique_ptr<Value<float>> ptrSize(new Value<float>(m_value.m_size));
        // std::unique_ptr<Value<float>> ptrStrenght(new Value<float>(m_value.m_strength));

        ptrPos->set(jsValue["position"]);
        ptrColor->set(jsValue["color"]);
        // ptrSize->set(jsValue["size"]);
        // ptrStrenght->set(jsValue["strenght"]);

        m_subvalue.push_back(std::move(ptrPos));
        m_subvalue.push_back(std::move(ptrColor));
        // m_subvalue.push_back(std::move(ptrSize));
        // m_subvalue.push_back(std::move(ptrStrenght));
    }
    template<>
    void Value<glm::vec2>::set(const nlohmann::json& jsValue)
    {
        m_subvalue.clear();
        if (jsValue.is_null())
            return;
        
        exprtk::parser<float> ExprParser;

        std::unique_ptr<Expression> refExpr(new Expression);
        exprtk::expression<float> &expr = refExpr->get();
        exprtk::symbol_table<float> symbol;
        std::ostringstream exprStr;
        if (jsValue.is_string())
        {
            std::string s = jsValue;
            std::smatch sm;
            if (R"reg(^\s*\(\s*(\w+)\s*,\s*(\w+)\s*\)\s*=>\s*{(.*)}\s*$)reg"_rg.match(s, sm))
            {
                for (int i=0;i<m_value.length();i++)
                    symbol.add_variable(sm[i+1],m_value[i]);
                exprStr << sm[sm.size()];
            }
        }
        else if (jsValue.is_number())
        {
            symbol.add_variable("x",m_value.x);
            symbol.add_variable("y",m_value.y);
            float x = jsValue.get<float>();
            exprStr << "x:=" << jsValue.get<float>() << ";y:=" << x << ";";
        }
        else if (jsValue.is_object())
        {
            std::array<std::array<const char*, 2>, 3> tableName{{{"x", "y"}, {"r", "g"}, {"s", "t"}}};
            for (std::array<const char*, 2> arrName : tableName)
            {
                if (jsValue.find(arrName[0])!=jsValue.end())
                {
                    for (int i=0;i<m_value.length();i++)
                    {
                        symbol.add_variable(arrName[i],m_value[i]);
                        exprStr << arrName[i] << ":=" << get_value(jsValue[arrName[i]]) << ";";
                    }
                    break;
                }
            }

        }
        expr.register_symbol_table(symbol);
        bool test = ExprParser.compile(exprStr.str(), expr);
        m_subvalue.push_back(std::move(refExpr));
    }
    template<>
    void Value<glm::vec3>::set(const nlohmann::json& jsValue)
    {
        m_subvalue.clear();
        if (jsValue.is_null())
            return;
        
        exprtk::parser<float> ExprParser;

        std::unique_ptr<Expression> refExpr(new Expression);
        exprtk::expression<float> &expr = refExpr->get();
        exprtk::symbol_table<float> symbol;
        std::ostringstream exprStr;
        if (jsValue.is_string())
        {
            std::string s = jsValue;
            std::smatch sm;
            if (R"reg(^\s*\(\s*(\w+)\s*,\s*(\w+)\s*,\s*(\w+)\s*\)\s*=>\s*{(.*)}\s*$)reg"_rg.match(s, sm))
            {
                for (int i=0;i<m_value.length();i++)
                    symbol.add_variable(sm[i+1],m_value[i]);
                exprStr << sm[sm.size()];
            }
        }
        else if (jsValue.is_number())
        {
            float x = jsValue.get<float>();
            for (int i=0;i<m_value.length();i++)
            {
                auto name = "v"+std::to_string(i+1);
                symbol.add_variable(name,m_value[i]);
                exprStr << name<<":=" << x<<";";
            }
        }
        else if (jsValue.is_object())
        {
            std::array<std::array<const char*, 3>, 3> tableName{{{"x", "y", "z"}, {"r", "g", "b"}, {"s", "t", "p"}}};
            for (std::array<const char*, 3> arrName : tableName)
            {
                if (jsValue.find(arrName[0])!=jsValue.end())
                {
                    for (int i=0;i<m_value.length();i++)
                    {
                        symbol.add_variable(arrName[i],m_value[i]);
                        exprStr << arrName[i] << ":=" << get_value(jsValue[arrName[i]]) << ";";
                    }
                    break;
                }
            }

        }
        expr.register_symbol_table(symbol);
        bool test = ExprParser.compile(exprStr.str(), expr);
        m_subvalue.push_back(std::move(refExpr));
    }
}