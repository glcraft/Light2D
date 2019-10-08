#include "JsonToValue.hpp"
#include <sstream>
#include <array>
#include <exprtk.hpp>
#include <liblight.h>
#include <regex_literals.h>
inline std::string get_value(const nlohmann::json& jV)
{
    return (jV.is_number() ? std::to_string(jV.get<float>()) : jV.get<std::string>());
}
namespace jsonexpr
{
    exprtk::parser<float> ExprParser;
    exprtk::symbol_table<float> ExprGlobalSymbol;

    void add_global_variable(std::string name, float& value)
    {
        ExprGlobalSymbol.add_variable(name, value);
    }

    template <typename ValueType>
    struct Transmission {
        ValueType& m_value;
        std::vector<std::unique_ptr<AbstractValue>>& m_subvalue;
    };
    class Expression : public AbstractValue
    {
    public:
        Expression()
        {
            m_expr.register_symbol_table(ExprGlobalSymbol);
        }
        //Useless
        void set(const nlohmann::json& jsValue) override
        {
            bool ok = ExprParser.compile(jsValue, m_expr);
        }
        void update() override
        { getValue(); }
        exprtk::expression<float>& get()
        { return m_expr; }
    protected:
        float getValue() { return m_expr.value(); }
        exprtk::expression<float> m_expr;
    };
    template <typename ValueType>
    class ExpressionValued : public Expression
    {
    public:
        ExpressionValued(ValueType& v) : m_value(v), Expression()
        {}
        ExpressionValued(ValueType& v, const nlohmann::json& strExpr) : m_value(v), Expression()
        { set(strExpr); }
        void update() override
        { m_value = getValue(); }
    protected:
        ValueType& m_value;
    };
    
    template<>
    void Value<li::Light>::set(const nlohmann::json& jsValue)
    {
        m_subvalue.clear();
        if (jsValue.is_null())
            return;
        m_subvalue.push_back(std::unique_ptr<Value<glm::vec2>>(new Value<glm::vec2>(m_value.m_position, jsValue["position"])));
        m_subvalue.push_back(std::unique_ptr<Value<glm::vec3>>(new Value<glm::vec3>(m_value.m_color, jsValue["color"])));
        m_subvalue.push_back(std::unique_ptr<Value<float>>(new Value<float>(m_value.m_size, jsValue["size"])));
        m_subvalue.push_back(std::unique_ptr<Value<float>>(new Value<float>(m_value.m_strength, jsValue["strenght"])));
    }
    
    template<>
    void Value<li::Wall>::set(const nlohmann::json& jsValue)
    {
        m_subvalue.clear();
        if (jsValue.is_null())
            return;
        m_subvalue.push_back(std::unique_ptr<Value<glm::vec2>>(new Value<glm::vec2>(m_value.m_pos1, jsValue["position1"])));
        m_subvalue.push_back(std::unique_ptr<Value<glm::vec2>>(new Value<glm::vec2>(m_value.m_pos2, jsValue["position2"])));
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
                transVal.m_subvalue.push_back(std::unique_ptr<Value<float>>(new Value<float>(transVal.m_value[i], jsValue)));
        }
        else if (jsValue.is_array())
        {
            for (int i=0;i<len;i++)
                transVal.m_subvalue.push_back(std::unique_ptr<Value<float>>(new Value<float>(transVal.m_value[i], jsValue[i])));
        }
        else if (jsValue.is_object())
        {
            constexpr const std::array<std::array<const char*, 4>, 3> tableVecName{{{"x", "y", "z", "w"}, {"r", "g", "b", "a"}, {"s", "t", "p", "q"}}};
            for (const auto& arrName : tableVecName)
            {
                if (jsValue.find(arrName[0])!=jsValue.end())
                {
                    for (int i=0;i<len;i++)
                        transVal.m_subvalue.push_back(std::make_unique<Value<float>>(transVal.m_value[i], jsValue[arrName[i]]));
                    break;
                }
            }
        }
        std::string sexpr=exprStr.str();
        if (!sexpr.empty())
        {
            std::unique_ptr<Expression> refExpr(new Expression);
            refExpr->get().register_symbol_table(symbol);
            bool ok = ExprParser.compile(sexpr, refExpr->get());
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
            if (R"(^\s*\(\s*(\w+)\s*\)\s*=>\s*\{(.*)\}$)"_rg.match(s, sm))
            {
                symbol.add_variable(sm[1],m_value);
                auto& exprStr = sm[sm.size()-1];
                std::unique_ptr<Expression> refExpr(new Expression);
                refExpr->get().register_symbol_table(symbol);
                ExprParser.compile(exprStr, refExpr->get());
                m_subvalue.push_back(std::move(refExpr));
            }
            else
                m_subvalue.push_back(std::make_unique<ExpressionValued<value_type>>(m_value, jsValue));
            
        }
        else if (jsValue.is_number())
            m_subvalue.push_back(std::make_unique<StaticValue<value_type>>(m_value, jsValue));
        
    }
}
