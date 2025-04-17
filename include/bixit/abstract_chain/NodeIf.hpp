#pragma once

#include <bixit/logger/Logger.hpp>
#include "ChainNode.hpp"

namespace bixit::abstract_chain {

    class NodeIf : public ChainNode {
    
    public:
        using ConditionVariant = std::variant< bool, double, int, std::string >;

    private:
        std::vector<std::pair<ChainNodeAttribute, std::string>> ruleTable;

        struct Rule {
            int64_t id;
            ChainNodeAttribute value;
            std::string node_id;
            Rule(int64_t id, ChainNodeAttribute value, const std::string& node_id)
                : id(id), value(value), node_id(node_id) {}
        };
        std::vector<Rule> _rules;

        std::string ConditionVariantToString(const ConditionVariant& value) const {
            return std::visit([](const auto& v) -> std::string {
                if constexpr (std::is_same_v<std::decay_t<decltype(v)>, bool>) {
                    return v ? "true" : "false"; 
                } else if constexpr (std::is_same_v<std::decay_t<decltype(v)>, std::string>) {
                    return v; 
                } else {
                    return std::to_string(v); 
                }
            }, value);
        }

        bool verifyStatement(const ConditionVariant& lValue, const std::string& op, const ConditionVariant& rValue) const {
            if (std::holds_alternative<int>(lValue) && std::holds_alternative<int>(rValue)) {
                const auto _lValue = std::get<int>(lValue);
                const auto _rValue = std::get<int>(rValue);
                if (op == "eq") return _lValue == _rValue;
                if (op == "ne") return _lValue != _rValue;
                if (op == "lt") return _lValue < _rValue;
                if (op == "gt") return _lValue > _rValue;
                if (op == "le") return _lValue <= _rValue;
                if (op == "ge") return _lValue >= _rValue;
            }
            if (std::holds_alternative<bool>(lValue) && std::holds_alternative<bool>(rValue)) {
                const auto _lValue = std::get<bool>(lValue);
                const auto _rValue = std::get<bool>(rValue);
                if (op == "and") return _lValue && _rValue;
                if (op == "or") return _lValue || _rValue;
            }
            Logger::getInstance().warning("The type of lvalue and/or rvalue is not managed");                           
            return false;
        }

        ConditionVariant mapJsonToConditionVariant(const nlohmann::ordered_json& j) const {
            if (j.is_boolean()) return j.get<bool>();  
            else if (j.is_number()) {
                if (j.is_number_integer()) return j.get<int>();
                else if (j.is_number_float()) return j.get<double>();  
            }
            else if (j.is_string()) return j.get<std::string>(); 
            throw std::invalid_argument("Impossible to remap invalid json type to condition variant");
        }

        ConditionVariant mapAttributeToConditionVariant(const ChainNodeAttribute& attribute) const {
            if (attribute.isBool()) return attribute.getBool().value(); 
            else if (attribute.isDecimal()) return attribute.getDecimal().value();
            else if (attribute.isInteger()) return (int)(attribute.getInteger().value());
            else if (attribute.isString()) return attribute.getString().value(); 
            else throw std::invalid_argument("Impossible to remap invalid attribute type to condition variant");
        }

        bool verifyConditions(const ChainNodeAttribute& conditions, const nlohmann::json& context) const{
            if (conditions.isObject()) {
                const auto condition = conditions.getObject().value();

                // retrieve operator
                const auto _op = condition.find("op");
                if (_op == condition.end() || !_op->second.isString()) {
                    Logger::getInstance().error("The provided condition in <rule_table> does not contain <op> key or <op> is not a string");                           
                    return false;
                }
                const std::string op = _op->second.getString().value();

                // retrieve left value
                const auto _lvalue = condition.find("lvalue");
                if (_lvalue == condition.end()) {
                    Logger::getInstance().error("The provided condition in <rule_table> does not contain <lvalue> key");                           
                    return false;
                }
                ConditionVariant lvalue;
                if (_lvalue->second.isObject()) {
                    lvalue = verifyConditions(_lvalue->second.getObject().value(), context);
                } else if (_lvalue->second.isString()){
                    if(!context.contains(_lvalue->second.getString().value())){
                        lvalue = false;
                    } else {
                        lvalue = mapJsonToConditionVariant(context[_lvalue->second.getString().value()]);
                    }
                } else {
                    Logger::getInstance().error("The provided condition contains <lvalue> that is not a string or an object");                           
                    return false;
                }
                
                if(op=="NOT"){
                    if (std::holds_alternative<bool>(lvalue)) {
                        const auto val = std::get<bool>(lvalue);
                        return val ? false : true;
                    } else if (std::holds_alternative<std::string>(lvalue)) {
                        const auto val = std::get<std::string>(lvalue);
                        if(val.empty()) return true;
                        return false;
                    } else if (std::holds_alternative<int>(lvalue)) {
                        const auto val = std::get<int>(lvalue);
                        if(val==0) return false;
                        return true;
                    } else if (std::holds_alternative<double>(lvalue)) {
                        const auto val = std::get<double>(lvalue);
                        if(val==0.0) return false;
                        return true;
                    }
                    Logger::getInstance().warning("Not managed lvalue in NOT branch: "+ConditionVariantToString(lvalue));    
                    return false;
                }

                // Check for right value
                const auto _rvalue = condition.find("rvalue");
                if (_rvalue == condition.end()) {
                    Logger::getInstance().error("The provided condition in <rule_table> does not contain <rvalue> key");                           
                    return false;
                }
                ConditionVariant rvalue;
                if (_rvalue->second.isObject()) {
                    rvalue = verifyConditions(_rvalue->second.getObject().value(), context);
                } else {
                    rvalue = mapAttributeToConditionVariant(_rvalue->second);
                }

                static constexpr std::array allowedOperators = { "eq", "ne", "gt", "ge", "lt", "le", "and", "or" };
                if(std::find(allowedOperators.begin(), allowedOperators.end(), op) != allowedOperators.end()){
                    bool isStatementVerified = false;
                    try {
                        isStatementVerified = verifyStatement(lvalue, op, rvalue);
                        /*
                        Logger::getInstance().debug("[if] (" 
                                                    + ConditionVariantToString(lvalue) + ", "
                                                    + op + ", "
                                                    + ConditionVariantToString(rvalue) + ") -> " 
                                                    + std::to_string(isStatementVerified) );
                        */
                    } catch(const std::invalid_argument& e) {
                        Logger::getInstance().error("Failed to remap value: " + std::string(e.what()));
                        return false;
                    }
                    return isStatementVerified;
                }

                Logger::getInstance().error("Condition is an object that has not been evaluated");
                return false;
            }
            Logger::getInstance().error("Conditions is not an object");
            return false;
        }


    public:
        NodeIf() : ChainNode() {}

        void addAttribute(const std::string& key, const ChainNodeAttribute& attribute) override {
            ChainNode::addAttribute(key,attribute);
            if(key=="rule_table"){
                if(attribute.isArray()) {
                    auto ruleDefinitions = attribute.getArray().value();
                    _rules.reserve(ruleDefinitions.size());
                    for (auto it = ruleDefinitions.begin(); it != ruleDefinitions.end(); ++it) {
                        /* each rule has the format of an AST */
                        if(!it->isObject()){
                            Logger::getInstance().error("Attribute <rule_table> does not contain objects");                           
                            return;
                        }
                        const auto ruleDefinition = it->getObject().value();
                        const auto _id = ruleDefinition.find("id");
                        if (_id == ruleDefinition.end()) {
                            Logger::getInstance().error("Each object in <rule_table> must contain <id> key");                           
                            return;
                        }
                        if (!_id->second.isInteger()) {
                            Logger::getInstance().error("Each rule in <rule_table> must contain an integer as <id>");                           
                            return;
                        }
                        const auto _value = ruleDefinition.find("value");
                        if (_value == ruleDefinition.end()) {
                            Logger::getInstance().error("Each object in <rule_table> must contain <value> key");                           
                            return;
                        }
                        if (!_value->second.isObject()) {
                            Logger::getInstance().error("Each rule in <rule_table> must contain an object as <value>");                           
                            return;
                        }
                        const auto _node_id = ruleDefinition.find("node_id");
                        if (_node_id == ruleDefinition.end()) {
                            Logger::getInstance().warning("The provided object in <rule_table> does not contain <node_id> key");                           
                            continue;
                        }
                        if (!_node_id->second.isString()) {
                            Logger::getInstance().error("Each rule in <rule_table> must contain a string as <node_id>");                           
                            return;
                        }
                        _rules.emplace_back(_id->second.getInteger().value(), _value->second.getObject().value(), _node_id->second.getString().value());
                    }
                    std::sort(_rules.begin(), _rules.end(), [](const Rule& a, const Rule& b) {
                        return a.id < b.id;
                    });
                    ruleTable.reserve(_rules.size());
                    for (const auto& rule : _rules) {
                        ruleTable.emplace_back(rule.value, rule.node_id);
                    }
                } else {
                    Logger::getInstance().error("Attribute <rule_table> is not an array");
                }
            }
        }

        int bitstream_to_json(BitStream& bitStream, nlohmann::ordered_json& outputJson) const override {
            for (const auto& [conditions, node_id] : ruleTable) {
                if(verifyConditions(conditions, outputJson)){
                    auto chain = getChain()->getNode(node_id);
                    if(chain!=nullptr){
                        chain->bitstream_to_json(bitStream, outputJson);
                    } else {
                        Logger::getInstance().error("The node_id <" + node_id + "> is not present");
                    }
                    break; // condition matched
                }
            }

            // Propagate to the next node (if any)
            auto nextNode = getNextNode();
            if(nextNode){
                nextNode->bitstream_to_json(bitStream, outputJson);
            }

            return 0;
        };

        int json_to_bitstream(nlohmann::ordered_json& inputJson, BitStream& bitStream) override {

            for (const auto& [conditions, node_id] : ruleTable) {
                if(verifyConditions(conditions, inputJson)){
                    auto chain = getChain()->getNode(node_id);
                    if(chain!=nullptr){
                        chain->json_to_bitstream(inputJson, bitStream);
                    } else {
                        Logger::getInstance().error("The node_id <" + node_id + "> is not present");
                    }
                    break; // condition matched
                }
            }

            // Propagate to the next node (if any)
            auto nextNode = getNextNode();
            if(nextNode){
                nextNode->json_to_bitstream(inputJson, bitStream);
            }

            return 0;
        };


    };
}

