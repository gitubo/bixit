#pragma once

#include "ChainNode.hpp"

using Endianess = bixit::abstract_chain::Endianness;

namespace bixit::abstract_chain {
    class NodeUnsignedInteger : public ChainNode {

    private:
        size_t bitLength;
        Endianness endianness;

    public:
        NodeUnsignedInteger() : ChainNode(), bitLength(0), endianness(Endianness::BIG) {};
        
        void addAttribute(const std::string& key, const ChainNodeAttribute& attribute) override {
            ChainNode::addAttribute(key,attribute);
            if(key=="bit_length"){
                if(!attribute.isInteger()){
                    Logger::getInstance().error("Attribute <bit_length> is not an integer");
                } else {
                    bitLength = attribute.getInteger().value();
                    if(bitLength<=0){
                        Logger::getInstance().error("Attribute <bit_length> is not valid (<=0)");
                        bitLength = 0;
                    }
                }
            } else if(key=="endianness"){
                if(!attribute.isString()){
                    Logger::getInstance().error("Attribute <endianness> is not a string");
                } else {
                    auto endianess_str = attribute.getString().value();
                    if (endianess_str == "big") {
                        endianness = Endianness::BIG;
                    } else if (endianess_str == "little") {
                        endianness = Endianness::LITTLE;
                    } else {
                        endianness = Endianness::BIG;
                        Logger::getInstance().error("Attribute <endianness> is not valid ("+endianess_str+")");
                        Logger::getInstance().warning("Attribute <endianness> forced to 'BIG'");
                    }
                }
            }
        }

        int bitstream_to_json(BitStream& bitStream, nlohmann::ordered_json& outputJson) const override {
            if (bitLength == 0) {
                Logger::getInstance().error("Node <" + getFullName() + "> has invalid bitLength = 0");
                return 1;
            }
            uint64_t value = 0;            
            int convertion_error = 0;
            try{
                convertion_error = bixit::abstract_chain::convert_from_uint8_array_to_uint64(
                    bitStream.consume(bitLength).get(),
                    bitLength,
                    endianness,
                    &value);
            } catch (const std::out_of_range& e) {
                Logger::getInstance().error("Analyzing node <" + this->getFullName() + "> - Failed to consume bitstream: " + std::string(e.what()));
                return 1;
            }

            if(convertion_error){
                Logger::getInstance().error("Failed to convert node <" + this->getFullName() + "> to uint64_t");
                return 1;
            }

            // Prepare the json output
            outputJson[this->getFullName()] = value;

            // Propagate to the next node (if any)
            auto nextNode = getNextNode();
            if(nextNode){
                nextNode->bitstream_to_json(bitStream, outputJson);
            }
            return 0;
        }

        int json_to_bitstream(nlohmann::ordered_json& inputJson, BitStream& bitStream) override { 
            if(!inputJson.is_object()){
                Logger::getInstance().error("The provided json is not an object");
                return 100;
            }
            if(!inputJson.contains(this->getFullName()) || !inputJson[this->getFullName()].is_number_integer()){
                Logger::getInstance().error("Key <"+this->getFullName()+"> not found in the provided json object or the related value is not an integer");
                return 100;      
            }

            uint64_t value = inputJson[this->getFullName()];
            
            uint8_t valueBuffer[8];
            std::memcpy(valueBuffer, reinterpret_cast<const uint8_t*>(&value), 8);

            //memcpy(valueBuffer, &value, 8);   
//            BitStream bsInteger(valueBuffer, bitLength);
//            bitStream.append(bsInteger);
            bitStream.append(valueBuffer, bitLength, 8);

            // Propagate to the next node (if any)
            auto nextNode = getNextNode();
            if(nextNode){
                nextNode->json_to_bitstream(inputJson, bitStream);
            }
            return 0;
        }
            
    };
}