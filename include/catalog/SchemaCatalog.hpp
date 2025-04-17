#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <optional>
#include <nlohmann/json.hpp>

#include "../logger/Logger.hpp"
#include "../abstract_chain/ChainFactory.hpp"
#include "../abstract_chain/NodeRoot.hpp"
#include "../abstract_chain/NodeDetour.hpp"
#include "../abstract_chain/NodeRouter.hpp"
#include "../abstract_chain/NodeArray.hpp"
#include "../abstract_chain/NodeIf.hpp"
#include "../abstract_chain/NodeFunctionLua.hpp"
#include "../abstract_chain/NodeUnsignedInteger.hpp"

#include "Schema.hpp"

namespace datacarder {

    class SchemaCatalog {
    private:
        std::unordered_map<std::string, Schema> schemaMap;
        std::string originDirectory;

    public:

        SchemaCatalog(const std::string&, const Logger::Level);
        /*
        static SchemaCatalog& getInstance(const std::string& proposedOriginDirectory = ".") {
            static SchemaCatalog instance(proposedOriginDirectory); 
            return instance;
        }
        */

        const std::shared_ptr<ChainNode> getAbstractChain(const std::string& key) const;
        const std::shared_ptr<ChainNode> parseSchemaOnTheFly(const nlohmann::json&);
        std::string to_string(size_t) const;

    private:
        int parseSchema(const std::string&, const nlohmann::json&);
        const std::shared_ptr<Schema> parseSchema(const nlohmann::json&);
        //SchemaCatalog& operator=(const SchemaCatalog&) = delete;

        std::optional<std::shared_ptr<ChainNode>> evalArray(const nlohmann::json&, const std::shared_ptr<ChainAccess>);
        std::optional<std::shared_ptr<ChainNode>> evalObject(const nlohmann::json&, const std::shared_ptr<ChainAccess>);
        std::optional<std::shared_ptr<ChainNodeAttribute>> evalAttribute(const nlohmann::json&);
    };

}