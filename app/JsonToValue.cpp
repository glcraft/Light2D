#include "JsonToValue.hpp"
#include <sstream>
#include <array>
#include <liblight.h>
#include <regex_literals.h>
inline std::string get_value(const nlohmann::json& jV)
{
    return (jV.is_number() ? std::to_string(jV.get<float>()) : jV.get<std::string>());
}
namespace jsonexpr
{
    template <typename ValueType>
    struct Transmission {
        ValueType& m_value;
        std::vector<std::unique_ptr<AbstractValue>>& m_subvalue;
    };
    exprtk::parser<float> ExprParser;
    template<>
    void Value<li::Light>::set(const nlohmann::json& jsValue)
    {
        m_subvalue.clear();
        if (jsValue.is_null())
            return;
        std::unique_ptr<Value<glm::vec2>> ptrPos(new Value<glm::vec2>(m_value.m_position));
        std::unique_ptr<Value<glm::vec3>> ptrColor(new Value<glm::vec3>(m_value.m_color));
        std::unique_ptr<Value<float>> ptrSize(new Value<float>(m_value.m_size));
        std::unique_ptr<Value<float>> ptrStrenght(new Value<float>(m_value.m_strength));

        ptrPos->set(jsValue["position"]);
        ptrColor->set(jsValue["color"]);
        ptrSize->set(jsValue["size"]);
        ptrStrenght->set(jsValue["strenght"]);

        m_subvalue.push_back(std::move(ptrPos));
        m_subvalue.push_back(std::move(ptrColor));
        m_subvalue.push_back(std::move(ptrSize));
        m_subvalue.push_back(std::move(ptrStrenght));
    }
    
    
    template <typename VecX>
    inline auto define_vec(Transmission<VecX> transVal, const nlohmann::json& jsValue)
    {
        transVal.m_subvalue.clear();
        if (jsValue.is_null())
            return;
        constexpr const size_t len = VecX::length();
        std::ostringstream exprStr;
        exprtk::symbol_table<float> symbol;

        VecX& value=transVal.m_value;

        if (jsValue.is_string())
        {
            std::string s = jsValue;
            std::smatch sm;
            std::ostringstream ostrReg;
            ostrReg << "^\\s*\\(\\s*(\\w+)\\s*";
            for (int i=1;i<len;i++)
                ostrReg << ",\\s*(\\w+)\\s*";
            ostrReg << "\\)\\s*=>\\s*\\{(.*)\\}\\s*$";
            std::string regStr=ostrReg.str();
            std::regex reg=std::regex(regStr, std::regex_constants::ECMAScript);
            if (std::regex_match(s, sm, reg))
            {
                for (int i=0;i<len;i++)
                    symbol.add_variable(sm[i+1],value[i]);
                exprStr << sm[sm.size()-1];
            }
        }
        else if (jsValue.is_number())
        {
            float x = jsValue.get<float>();
            for (int i=0;i<len;i++)
            {
                auto name = "v"+std::to_string(i+1);
                symbol.add_variable(name,value[i]);
                exprStr << name<< ":=" << x<<";";
            }
        }
        else if (jsValue.is_object())
        {
            constexpr const std::array<std::array<const char*, 4>, 3> tableVecName{{{"x", "y", "z", "w"}, {"r", "g", "b", "a"}, {"s", "t", "p", "q"}}};
            for (const auto& arrName : tableVecName)
            {
                if (jsValue.find(arrName[0])!=jsValue.end())
                {
                    for (int i=0;i<len;i++)
                    {
                        std::unique_ptr<Value<float>> v(new Value<float>(transVal.m_value[i]));
                        v->set(jsValue[arrName[i]]);
                        transVal.m_subvalue.push_back(std::move(v));
                    }
                    break;
                }
            }
        }
        std::string sexpr=exprStr.str();
        if (!sexpr.empty())
        {
            std::unique_ptr<Expression> refExpr(new Expression);
            exprtk::expression<float> expr=refExpr->get();
            expr.register_symbol_table(symbol);
            bool ok = ExprParser.compile(exprStr.str(), expr);
            transVal.m_subvalue.push_back(std::move(refExpr));
            return;
        }
    }
    template<>
    void Value<glm::vec2>::set(const nlohmann::json& jsValue)
    {
        define_vec(Transmission<value_type>{m_value, m_subvalue}, jsValue);
    }
    template<>
    void Value<glm::vec3>::set(const nlohmann::json& jsValue)
    {
        define_vec(Transmission<value_type>{m_value, m_subvalue}, jsValue);
    }
    template<>
    void Value<glm::vec4>::set(const nlohmann::json& jsValue)
    {
        define_vec(Transmission<value_type>{m_value, m_subvalue}, jsValue);
    }
    template<>
    void Value<float>::set(const nlohmann::json& jsValue)
    {
        exprtk::symbol_table<float> symbol;
        
        std::string exprStr;
        if (jsValue.is_string())
        {
            std::string s = jsValue;
            std::smatch sm;
            std::regex reg=std::regex(R"(^\s*\(\s*(\w+)\s*\)\s*=>\s*\{(.*)\}$)", std::regex_constants::ECMAScript);
            if (std::regex_match(s, sm, reg))
            {
                symbol.add_variable(sm[1],m_value);
                exprStr = sm[sm.size()-1];
            }
        }
        else if (jsValue.is_number())
        {
            std::unique_ptr<StaticValue<float>> refVal(new StaticValue<float>(m_value));
            refVal->set(jsValue);
            m_subvalue.push_back(std::move(refVal));
        }
        if (!exprStr.empty())
        {
            std::unique_ptr<Expression> refExpr(new Expression);
            refExpr->get().register_symbol_table(symbol);
            ExprParser.compile(exprStr, refExpr->get());
            m_subvalue.push_back(std::move(refExpr));
        }
        
    }
}
