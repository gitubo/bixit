#pragma once

#include <string>
#include <variant>
#include <optional>
#include <unordered_map>
#include <memory>
#include <nlohmann/json.hpp>
#include <bixit/bitstream/BitStream.hpp>
#include <bixit/logger/Logger.hpp>
#include "ChainNode.hpp"
#include "ChainAccess.hpp"

using Logger = bixit::logger::Logger;
using BitStream = bixit::bitstream::BitStream;

namespace bixit::abstract_chain {

    class Chain : public ChainAccess{

    private:
        std::unordered_map<std::string, std::shared_ptr<ChainNode>> chain;
        std::shared_ptr<ChainNode> startingNode;

        // Helper function to perform deep copy of ChainNode
        std::shared_ptr<ChainNode> cloneNode(const std::shared_ptr<ChainNode>& node) const {
            if (!node) return nullptr;
            std::shared_ptr<ChainNode> newNode = node->clone(); // Usa il metodo clone di ChainNode
            return newNode;
        }
        
    public:

        Chain() = default;

        Chain(const Chain& other) {
            for (const auto& [nodeId, node] : other.chain) {
                chain[nodeId] = cloneNode(node); // Usa la funzione helper cloneNode
            }
            startingNode = cloneNode(other.startingNode);
        }

        Chain& operator=(const Chain& other) {
            if (this != &other) {
                chain.clear();
                for (const auto& [nodeId, node] : other.chain) {
                    chain[nodeId] = cloneNode(node); // Usa cloneNode
                }
                startingNode = cloneNode(other.startingNode);
            }
            return *this;
        }

        std::unique_ptr<Chain> clone() const {
            return std::make_unique<Chain>(*this);
        }

        void addNode(const std::string& nodeId, std::shared_ptr<ChainNode> node) {
            std::pair<std::string, std::shared_ptr<ChainNode>> thisPair(nodeId, node);         
            chain.insert(thisPair);
        }
        void clearBrances() { chain.clear(); }

        void setStartingNode(std::shared_ptr<ChainNode> node) { this->startingNode = node; }
        const std::shared_ptr<ChainNode> getStartingNode() const { return this->startingNode; }

        const std::shared_ptr<ChainNode> getNode(const std::string& name) const override {
            auto it = chain.find(name);
            if (it != chain.end()) {
                return it->second;
            }
            return nullptr;
        }

        int json_to_bitstream(nlohmann::ordered_json& inputJson, BitStream& bitStream){
            auto it = chain.find("main");
            if (it != chain.end()) {
                it->second->json_to_bitstream(inputJson, bitStream);
                return 0;
            } else {
                Logger::getInstance().error("No root node <main>");
            }
            return 1;
        };

        int bitstream_to_json(BitStream& bitStream, nlohmann::ordered_json& outputStream){
            auto it = chain.find("main");
            if (it != chain.end()) {
                it->second->bitstream_to_json(bitStream, outputStream);
                return 0;
            } else {
                Logger::getInstance().error("No root node <main>");
            }
            return 1;
        };

        virtual std::string to_string(size_t indent = 0) const { 
            std::ostringstream oss;
            std::string indentStr(indent, ' ');
            oss << indentStr << "\"nodes\": {";
            for(auto it = chain.begin(); it != chain.end(); ++it){
                if (it == chain.begin()) { 
                    oss << "\n";
                }
                oss << indentStr << "  \"" << it->first << "\": " << it->second->to_string();
                if (std::next(it) != chain.end()) { 
                    oss << ",\n";
                } else {
                    oss << "\n";
                    oss << indentStr;
                }
            }
            oss << indentStr << "}\n";
            return oss.str();
        }
    };
}

