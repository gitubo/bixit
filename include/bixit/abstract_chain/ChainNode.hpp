#pragma once

#include <string>
#include <variant>
#include <optional>
#include <nlohmann/json.hpp>
#include <bixit/bitstream/BitStream.hpp>
#include <bixit/logger/Logger.hpp>
#include "ChainNodeAttribute.hpp"
#include "ChainAccess.hpp"
#include "Helper.hpp"

using Logger = bixit::logger::Logger;
using BitStream = bixit::bitstream::BitStream;

namespace bixit::abstract_chain {

    class ChainNode {

    private:
        std::string id;
        std::string name;
        std::string parentName;
        std::unordered_map<std::string, ChainNodeAttribute> attributeMap;
        std::shared_ptr<ChainAccess> Chain;
        std::string nextNode;

    public:

        ChainNode() :
            id(""), 
            name(""), 
            parentName("/"), 
            nextNode(""),
            Chain(nullptr) {}
        
        ChainNode(std::string id, std::string name, std::string parentName) : 
            id(id), 
            name(name), 
            parentName(parentName), 
            nextNode(""),
            Chain(nullptr) {}

        // Costruttore di copia
        ChainNode(const ChainNode& other) 
            : id(other.id),
              name(other.name),
              parentName(other.parentName),
              attributeMap(other.attributeMap),
              nextNode(other.nextNode),
              Chain(other.Chain) {
            // Currently the chain is not copied
        }

        ChainNode& operator=(const ChainNode& other) {
            if (this != &other) {
                id = other.id;
                name = other.name;
                parentName = other.parentName;
                attributeMap = other.attributeMap;
                nextNode = other.nextNode;
                Chain = other.Chain;  // Chain is shared, not copied(!)
            }
            return *this;
        }

        virtual std::shared_ptr<ChainNode> clone() const {
            return std::make_shared<ChainNode>(*this);
        }

        virtual ~ChainNode() = default;
        
        const std::string getId() const { return id; }
        const std::string getName() const { return name; }
        const std::string getParentName() const { return parentName; }
        const std::string getFullName() const { return parentName + name;}
        const std::shared_ptr<ChainAccess> getChain() const { return Chain;}
        const std::unordered_map<std::string, ChainNodeAttribute>& getAttributeMap() const { return attributeMap; }
        std::optional<ChainNodeAttribute> getAttribute(const std::string& key) const {
            auto it = attributeMap.find(key);
            if (it != attributeMap.end()) {
                return it->second;
            }
            return std::nullopt;
        }
        
        void setId(std::string id) { this->id = id; }
        void setName(std::string name) { this->name = name; }
        void setParentName(std::string parentName) { 
            if (!parentName.empty() && parentName.back() != '/') {
                parentName += '/';
            }
            this->parentName = parentName; 
        }
        void setNextNode(std::string nextNode) { this->nextNode = nextNode; }
        void setChain(const std::shared_ptr<ChainAccess> Chain) { this->Chain = Chain; }

        virtual void addAttribute(const std::string& key, const ChainNodeAttribute& attribute) {
            attributeMap[key] = attribute;
        }
        void clearAttributes() { attributeMap.clear(); }

        virtual std::shared_ptr<ChainNode> getNextNode() const {
            if(!this->nextNode.empty() && this->nextNode!=""){
                if (Chain) {
                    return Chain->getNode(nextNode);
                }
                return nullptr;
            }
            return nullptr; 
        };

        virtual int bitstream_to_json(BitStream& bitStream, nlohmann::ordered_json& outputJson) const {
            // Propagate to the next node (if any)
            auto nextNode = getNextNode();
            if(nextNode){
                nextNode->bitstream_to_json(bitStream, outputJson);
            }
            return 0; 
        };

        virtual int json_to_bitstream(nlohmann::ordered_json& inputJson, BitStream& bitStream) {
            // Propagate to the next node (if any)
            auto nextNode = getNextNode();
            if(nextNode){
                nextNode->json_to_bitstream(inputJson, bitStream);
            }

            return 0; 
        };

        const std::string to_string(size_t indent = 0) const { 
            std::ostringstream oss;
            std::string indentStr(indent, ' ');
            oss << indentStr << "{\n"; 
            oss << indentStr << "  \"id\": \"" << id << "\",\n";
            oss << indentStr << "  \"name\": \"" << name << "\",\n";
            oss << indentStr << "  \"parentName\": \"" << parentName << "\",\n";
            oss << indentStr << "  \"next_node\": \"" << nextNode << "\",\n";
            if(Chain){
                oss << indentStr << "  \"nodes\": \"node_list_reference\",\n";
            } else {
                oss << indentStr << "  \"nodes\": null,\n";
            }
            oss << indentStr << "  \"attributeMap\": {";
            for(auto it = attributeMap.begin(); it != attributeMap.end(); ++it){
                if (it == attributeMap.begin()) { 
                    oss << "\n";
                }
                oss << indentStr << "  \"" << it->first << "\": " << it->second.to_string();
                if (std::next(it) != attributeMap.end()) { 
                    oss << ",\n";
                } else {
                    oss << "\n";
                    oss << indentStr;
                }
            }
            oss << "},\n";
            oss << "]\n";
            oss << indentStr << "}\n";
            return oss.str();
        }
    };
}

