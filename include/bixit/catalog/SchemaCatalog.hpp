#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <optional>
#include <nlohmann/json.hpp>

#include <bixit/logger/Logger.hpp>
#include <bixit/abstract_chain/ChainFactory.hpp>
#include <bixit/abstract_chain/NodeRoot.hpp>
#include <bixit/abstract_chain/NodeDetour.hpp>
#include <bixit/abstract_chain/NodeRouter.hpp>
#include <bixit/abstract_chain/NodeArray.hpp>
#include <bixit/abstract_chain/NodeIf.hpp>
#include <bixit/abstract_chain/NodeFunctionLua.hpp>
#include <bixit/abstract_chain/NodeUnsignedInteger.hpp>

#include "Schema.hpp"

using ChainNode = bixit::abstract_chain::ChainNode;
using ChainAccess = bixit::abstract_chain::ChainAccess;
using ChainNodeAttribute = bixit::abstract_chain::ChainNodeAttribute;
using Logger = bixit::logger::Logger;

namespace bixit::catalog {

    class SchemaCatalog {
    private:
        std::string originDirectory;
        std::unordered_map<std::string, std::shared_ptr<Schema>> schemaMap;

    public:

        SchemaCatalog(const std::string&, const bixit::logger::Logger::Level);

        ~SchemaCatalog();
        SchemaCatalog(const SchemaCatalog&) = delete;
        SchemaCatalog& operator=(const SchemaCatalog&) = delete;
        SchemaCatalog(SchemaCatalog&&) noexcept = default;
        SchemaCatalog& operator=(SchemaCatalog&&) noexcept = default;
        
        const std::shared_ptr<ChainNode> getAbstractChain(const std::string& key) const;
        const std::shared_ptr<ChainNode> parseSchemaOnTheFly(const nlohmann::json&);
        std::string to_string(size_t) const;

    private:
        int parseSchema(const std::string&, const nlohmann::json&);
        std::shared_ptr<Schema> parseSchema(const nlohmann::json&);

        std::optional<std::shared_ptr<ChainNode>> evalArray(const nlohmann::json&, const std::shared_ptr<ChainAccess>);
        std::optional<std::shared_ptr<ChainNode>> evalObject(const nlohmann::json&, const std::shared_ptr<ChainAccess>);
        std::optional<std::shared_ptr<ChainNodeAttribute>> evalAttribute(const nlohmann::json&);

        std::unordered_map<std::string, std::string> listFilesRecursive(const std::filesystem::path&);

    };

}