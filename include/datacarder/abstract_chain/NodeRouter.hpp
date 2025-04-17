#pragma once

#include "ChainNode.hpp"

namespace datacarder {

    class NodeRouter : public ChainNode {
    public: 
        enum class Endianness {
            BIG,
            LITTLE
        };
    private:
        std::string control_node;
        std::map<int64_t, std::string> routingTable;

        struct Route {
            int64_t id;
            int64_t value;
            std::string node_id;
            Route(int64_t id, int64_t value, const std::string& node_id)
                : id(id), value(value), node_id(node_id) {}
        };
        std::vector<Route> _routes;

    public:
        NodeRouter() : ChainNode() {}

        void addAttribute(const std::string& key, const ChainNodeAttribute& attribute) override {
            ChainNode::addAttribute(key,attribute);
            
            if(key=="control_node"){
                if(!attribute.isString()){
                    Logger::getInstance().log("Attribute <control_node> is not a string", Logger::Level::ERROR);
                } else {
                    control_node = attribute.getString().value();
                }
            } else if(key=="routing_table"){
                if(attribute.isArray()) {
                    auto array = attribute.getArray().value();
                    _routes.reserve(array.size());
                    for (auto it = array.begin(); it != array.end(); ++it) {
                        if(!it->isObject()){
                            Logger::getInstance().error("Attribute <routing_table> does not contain objects");                           
                            return;
                        }
                        const auto routeDefinition = it->getObject().value();
                        const auto _id = routeDefinition.find("id");
                        if (_id == routeDefinition.end()) {
                            Logger::getInstance().error("Each object in <routing_table> must contain <id> key");                           
                            return;
                        }
                        const auto _value = routeDefinition.find("value");
                        if (_value == routeDefinition.end()) {
                            Logger::getInstance().error("Each object in <routing_table> must contain <value> key");                           
                            return;
                        }
                        const auto _node_id = routeDefinition.find("node_id");
                        if (_node_id == routeDefinition.end()) {
                            Logger::getInstance().warning("The provided object in <routing_table> does not contain <node_id> key");                           
                            continue;
                        }
                        _routes.emplace_back(_id->second.getInteger().value(), _value->second.getInteger().value(), _node_id->second.getString().value());
                    }
                    std::sort(_routes.begin(), _routes.end(), [](const Route& a, const Route& b) {
                        return a.id < b.id;
                    });
                    for (const auto& route : _routes) {
                        routingTable[route.value] = route.node_id;
                    }
                } else {
                    Logger::getInstance().log("Attribute <routing_table> is not an array", Logger::Level::ERROR);
                }
            }
        }

        int bitstream_to_json(BitStream& bitStream, nlohmann::ordered_json& outputJson) const override {
            if (!outputJson.contains(control_node)) {
                Logger::getInstance().log("Missing control node " + control_node + " in the evaluated json", Logger::Level::ERROR);
                return 1;
            }
            auto jsonControlNode = outputJson[control_node];
            if (!jsonControlNode.is_number_integer()) {
                Logger::getInstance().log("Control node value is not an integer", Logger::Level::ERROR);
                return 2;
            }
            int routingValue = jsonControlNode.get<int>();
            
            // Address the dissector to the right route
            auto it = routingTable.find(routingValue);
            if (it != routingTable.end()) {
                auto chain = getChain()->getNode(routingTable.at(routingValue));
                chain->bitstream_to_json(bitStream, outputJson);
            } else {
                Logger::getInstance().debug("Missing routing rule with value <"+std::to_string(routingValue)+">");
            }

            // Propagate to the next node (if any)
            auto nextNode = getNextNode();
            if(nextNode){
                nextNode->bitstream_to_json(bitStream, outputJson);
            }

            return 0;
        };

        int json_to_bitstream(nlohmann::ordered_json& inputJson, BitStream& bitStream) override {
            if(!inputJson.is_object()){
                Logger::getInstance().error("The provided json is not an object");
                return 100;
            }
            if (!inputJson.contains(control_node)) {
                Logger::getInstance().log("Missing control node " + control_node + " in the evaluated json", Logger::Level::ERROR);
                return 1;
            }
            auto jsonControlNode = inputJson[control_node];
            if (!jsonControlNode.is_number_integer()) {
                Logger::getInstance().log("Control node value is not an integer", Logger::Level::ERROR);
                return 2;
            }
            int routingValue = jsonControlNode.get<int>();
            
            // Address the dissector to the right route
            auto it = routingTable.find(routingValue);
            if (it != routingTable.end()) {
                auto chain = getChain()->getNode(routingTable.at(routingValue));
                chain->json_to_bitstream(inputJson, bitStream);
            } else {
                Logger::getInstance().debug("Missing routing rule with value <"+std::to_string(routingValue)+">");
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

