#include <bixit/catalog/SchemaCatalog.hpp>


namespace fs = std::filesystem;
using namespace bixit::catalog;
using namespace bixit::abstract_chain;

using ChainNode = bixit::abstract_chain::ChainNode;
using ChainAccess = bixit::abstract_chain::ChainAccess;
using ChainNodeAttribute = bixit::abstract_chain::ChainNodeAttribute;

std::unordered_map<std::string, std::string> listFilesRecursive(const fs::path& directory) {
    std::unordered_map<std::string, std::string> fileMap;

    if (!fs::exists(directory) || !fs::is_directory(directory)) {
        std::cerr << "Errore: La directory non esiste!" << std::endl;
        return fileMap;
    }

    for (const auto& entry : fs::recursive_directory_iterator(directory)) {
        if (fs::is_regular_file(entry)) {
            std::string fullPath = entry.path().string();
            std::string relativePath = fs::relative(entry.path(), directory).string();

            // Sostituisci '/' con '.' nella chiave
            std::replace(relativePath.begin(), relativePath.end(), '/', '.');

            // Rimuovi l'estensione (se presente)
            if (relativePath.find('.') != std::string::npos) {
                relativePath = relativePath.substr(0, relativePath.find_last_of('.'));
            }

            // Inserisci nel map
            fileMap[relativePath] = fullPath;
        }
    }

    return fileMap;
}


SchemaCatalog::SchemaCatalog(const std::string& originDirectory, const Logger::Level logLevel) {
    this->originDirectory = originDirectory;

    Logger::getInstance().setSeverity(logLevel);
    Logger::getInstance().info("Registering nodes...");
    
    ChainFactory::getInstance().registerClass<NodeRoot>("root");
    ChainFactory::getInstance().registerClass<NodeRoot>("goto");
    ChainFactory::getInstance().registerClass<NodeDetour>("detour");

    ChainFactory::getInstance().registerClass<NodeArray>("array");
    ChainFactory::getInstance().registerClass<NodeIf>("if");
    ChainFactory::getInstance().registerClass<NodeRouter>("router");
    ChainFactory::getInstance().registerClass<NodeFunctionLua>("function lua");

    ChainFactory::getInstance().registerClass<NodeUnsignedInteger>("unsigned integer");


    Logger::getInstance().info("Loading catalog from directory <" + this->originDirectory + "> ...");
    auto schemaMap = listFilesRecursive(this->originDirectory);
    Logger::getInstance().info("Collected " + std::to_string(schemaMap.size()) + " schemas");
    Logger::getInstance().info("Parsing schemas...");
    for (const auto& [schemaName, schemaFilePath] : schemaMap) {
        Logger::getInstance().debug("Parsing schema <" + schemaName + "> from <" + schemaFilePath + ">");
        nlohmann::json jsonData;
        std::ifstream inputFile(schemaFilePath);
        if (!inputFile.is_open()) {
            Logger::getInstance().error("Impossible to open file " + schemaFilePath);
        }
        try {
            inputFile >> jsonData;
            inputFile.close();
        } catch (const nlohmann::json::parse_error& e) {
            Logger::getInstance().error("File not correctly loaded: " + std::string(e.what()));
        }
        if(parseSchema(schemaName, jsonData) < 0){
            Logger::getInstance().error("Schema not correctly parsed");
        } else {
            Logger::getInstance().debug("Schema correctly parsed");
        }
    }

};

const std::shared_ptr<ChainNode> SchemaCatalog::getAbstractChain(const std::string& schemaName) const {
    auto it = schemaMap.find(schemaName);
    if (it != schemaMap.end()) {
//        return it->second.getNode("main");//->clone();
        return it->second.getChainStartingNode();
    } else {
        Logger::getInstance().warning("Requested schema <" + schemaName + "> does not exist in the loaded catalog");
    }
    return nullptr;
}

/*
const std::shared_ptr<ChainNode> SchemaCatalog::parseSchemaOnTheFly(const nlohmann::json& jsonSchema) {
    Schema schema;
    if(jsonSchema.is_object()){
        for (const auto& [key, val] : jsonSchema.items()) {
            if(key=="nodes" && val.type() == nlohmann::json::value_t::object){
                // Check for a valid starting point ("main" root node)
                std::string mainRootNodeId = "";
                for (const auto& [id, node] : val.items()) {
                    if (node.is_object() && node.contains("name") && node.contains("type")) {
                        if (node["name"] == "main" && node["type"] == "root") {
                            mainRootNodeId = id;
                            break;
                        }
                    }
                }
                if(mainRootNodeId.empty()){
                    Logger::getInstance().log("Error in parsing nodes: no 'main' root node found", Logger::Level::ERROR);
                    return nullptr;
                }
                // list all the nodes
                for (auto& [nodeId, nodeStructure] : val.items()) {
                    auto thisNode = evalObject(nodeStructure, schema.getChain());
                    if(!thisNode){
                        Logger::getInstance().error("Error in parsing node <"+nodeId+">");
                        return nullptr;
                    }
                    auto thisNodeValue = thisNode.value();
                    thisNodeValue->setId(nodeId);
                    schema.addNode(nodeId, thisNodeValue);
                    if(nodeId == mainRootNodeId){
                        schema.setChainStartingNode(thisNodeValue);                       
                    }
                }     
            } else if(key=="metadata" && val.type() == nlohmann::json::value_t::object){
                std::unordered_map<std::string, std::string> metadata;
                for(auto& [key, val] : val.items()){
                    if(val.type() == nlohmann::json::value_t::string){
                        metadata[key] = val.get<std::string>();
                    } else {
                        Logger::getInstance().log("Unsupported type for element <" + key + ">: elements in <metadata> section can be only string", Logger::Level::WARNING);
                        return nullptr;
                    }
                }
                schema.setMetadata(metadata);
            } else if(key=="version" && val.type() == nlohmann::json::value_t::string){
                schema.setVersion(val.get<std::string>());
            } else {
                Logger::getInstance().log("Unsupported type: " + std::to_string(static_cast<int>(jsonSchema.type())) + " for element named <" + key + ">", Logger::Level::WARNING);
                return nullptr;
            }
        }
    } else {
        Logger::getInstance().log("Provided JSON Schema is not an object", Logger::Level::ERROR);
        return nullptr;
    } 

    return schema.getChainStartingNode();
}


int SchemaCatalog::parseSchema(const std::string& name, const nlohmann::json& jsonSchema) {
    Schema schema;
    schema.setCatalogName(name);
    if(jsonSchema.is_object()){
        for (const auto& [key, val] : jsonSchema.items()) {
            if(key=="nodes" && val.type() == nlohmann::json::value_t::object){
                // Check for a valid starting point ("main" root node)
                std::string mainRootNodeId = "";
                for (const auto& [id, node] : val.items()) {
                    if (node.is_object() && node.contains("name") && node.contains("type")) {
                        if (node["name"] == "main" && node["type"] == "root") {
                            mainRootNodeId = id;
                            break;
                        }
                    }
                }
                if(mainRootNodeId.empty()){
                    Logger::getInstance().log("Error in parsing nodes: no 'main' root node found", Logger::Level::ERROR);
                    return 4;
                }
                // list all the nodes
                for (auto& [nodeId, nodeStructure] : val.items()) {
                    auto thisNode = evalObject(nodeStructure, schema.getChain());
                    if(!thisNode){
                        Logger::getInstance().error("Error in parsing node <"+nodeId+">");
                        return 4;
                    }
                    auto thisNodeValue = thisNode.value();
                    thisNodeValue->setId(nodeId);
                    schema.addNode(nodeId, thisNodeValue);
                    if(nodeId == mainRootNodeId){
                        schema.setChainStartingNode(thisNodeValue);                       
                    }
                }     
            } else if(key=="metadata" && val.type() == nlohmann::json::value_t::object){
                std::unordered_map<std::string, std::string> metadata;
                for(auto& [key, val] : val.items()){
                    if(val.type() == nlohmann::json::value_t::string){
                        metadata[key] = val.get<std::string>();
                    } else {
                        Logger::getInstance().log("Unsupported type for element <" + key + ">: elements in <metadata> section can be only string", Logger::Level::WARNING);
                        return 3;
                    }
                }
                schema.setMetadata(metadata);
            } else if(key=="version" && val.type() == nlohmann::json::value_t::string){
                schema.setVersion(val.get<std::string>());
            } else {
                Logger::getInstance().log("Unsupported type: " + std::to_string(static_cast<int>(jsonSchema.type())) + " for element named <" + key + "> in file <" + name + ">", Logger::Level::WARNING);
                return 2;
            }
        }
    } else {
        Logger::getInstance().log("Provided JSON Schema is not an object", Logger::Level::ERROR);
        return 1;
    } 

    schemaMap[name] = schema;
    return 0;
}
*/

const std::shared_ptr<ChainNode> SchemaCatalog::parseSchemaOnTheFly(const nlohmann::json& jsonSchema) {
    return parseSchema(jsonSchema)->getChainStartingNode();
}

int SchemaCatalog::parseSchema(const std::string& name, const nlohmann::json& jsonSchema) {
    auto schema = parseSchema(jsonSchema);
    if(schema == nullptr){
        Logger::getInstance().error("Error in parsing schema <"+name+">");
        return 1;
    }
    schema->setCatalogName(name);
    schemaMap[name] = *schema;
    return 0;
}

const std::shared_ptr<Schema> SchemaCatalog::parseSchema(const nlohmann::json& jsonSchema) {
    std::shared_ptr<Schema> schema = std::make_shared<Schema>();
    if(jsonSchema.is_object()){
        for (const auto& [key, val] : jsonSchema.items()) {
            if(key=="nodes" && val.type() == nlohmann::json::value_t::object){
                // Check for a valid starting point ("main" root node)
                std::string mainRootNodeId = "";
                for (const auto& [id, node] : val.items()) {
                    if (node.is_object() && node.contains("name") && node.contains("type")) {
                        if (node["name"] == "main" && node["type"] == "root") {
                            mainRootNodeId = id;
                            break;
                        }
                    }
                }
                if(mainRootNodeId.empty()){
                    Logger::getInstance().log("Error in parsing nodes: no 'main' root node found", Logger::Level::ERROR);
                    return nullptr;
                }
                // list all the nodes
                for (auto& [nodeId, nodeStructure] : val.items()) {
                    auto thisNode = evalObject(nodeStructure, schema->getChain());
                    if(!thisNode){
                        Logger::getInstance().error("Error in parsing node <"+nodeId+">");
                        return nullptr;
                    }
                    auto thisNodeValue = thisNode.value();
                    thisNodeValue->setId(nodeId);
                    schema->addNode(nodeId, thisNodeValue);
                    if(nodeId == mainRootNodeId){
                        schema->setChainStartingNode(thisNodeValue);                       
                    }
                }     
            } else if(key=="metadata" && val.type() == nlohmann::json::value_t::object){
                std::unordered_map<std::string, std::string> metadata;
                for(auto& [key, val] : val.items()){
                    if(val.type() == nlohmann::json::value_t::string){
                        metadata[key] = val.get<std::string>();
                    } else {
                        Logger::getInstance().warning("Unsupported type for element <" + key + ">: elements in <metadata> section can be only string");
                        return nullptr;
                    }
                }
                schema->setMetadata(metadata);
            } else if(key=="version" && val.type() == nlohmann::json::value_t::string){
                schema->setVersion(val.get<std::string>());
            } else {
                Logger::getInstance().warning("Unsupported type: " + std::to_string(static_cast<int>(jsonSchema.type())) + " for element named <" + key + ">");
                return nullptr;
            }
        }
    } else {
        Logger::getInstance().error("Provided JSON Schema is not an object");
        return nullptr;
    } 

    return schema;
}

std::optional<std::shared_ptr<ChainNode>> SchemaCatalog::evalArray(const nlohmann::json& jsonArray, const std::shared_ptr<ChainAccess> chain){
    std::shared_ptr<ChainNode> thisNode = std::make_shared<ChainNode>();
    thisNode->setChain(chain);
    /*
    for (const auto &entry : jsonArray) {
        if(entry.type() == nlohmann::json::value_t::array){
            Logger::getInstance().log("Error not expected an array in array", Logger::Level::ERROR);
            return std::nullopt;
        } else {
            auto child = evalObject(entry, chain);
            if(!child){
                Logger::getInstance().error("Error in parsing structure '/'");
                return std::nullopt;
            }
            child.value()->setChain(chain);
            thisNode->addChild(child.value());
        }
    }
    */
    return thisNode;
}

std::optional<std::shared_ptr<ChainNode>> SchemaCatalog::evalObject(const nlohmann::json& jsonObject, const std::shared_ptr<ChainAccess> chain){
    
    // Type
    std::string type;
    if(jsonObject.contains("type") && jsonObject.at("type").is_string()){
        type = jsonObject.at("type").get<std::string>();
    } else {
        Logger::getInstance().error("Impossible to find a value for key 'type'");
        return std::nullopt;
    }

    // Instantiate object
    auto thisNode = ChainFactory::getInstance().create(type);
    thisNode->setChain(chain);
    if(!thisNode){
        Logger::getInstance().error("Node creation failed for type: " + type);
        return std::nullopt;
    }

    // Name
    if(jsonObject.contains("name") && jsonObject.at("name").is_string()){
        thisNode->setName(jsonObject.at("name").get<std::string>());
    }

    // Next node
    if(jsonObject.contains("next_node") && jsonObject.at("next_node").is_string()){
        thisNode->setNextNode(jsonObject.at("next_node").get<std::string>());
    }

    // Attributes
    if(jsonObject.contains("attributes")){
        if(!jsonObject.at("attributes").is_object()){
            Logger::getInstance().error("Invalid type for key 'attributes', object expected");
            return std::nullopt;
        }
        auto attributes = jsonObject.at("attributes");
        for (auto& [attributeName, attributeValue] : attributes.items()) {
            auto attribute = evalAttribute(attributeValue);
            if(!attribute){
                Logger::getInstance().error("Invalid attribute with attribute name <"+attributeName+">");
                return std::nullopt;
            }
            thisNode->addAttribute(attributeName, *attribute.value());
        }
    } 

    // Structure
    /*
    if(jsonObject.contains("structure")){

        if(!jsonObject.at("structure").is_object()){
            Logger::getInstance().error("Invalid type for key 'structure', object expected");
            return std::nullopt;
        }
        auto structure = jsonObject.at("structure");
        for (auto it = structure.begin(); it != structure.end(); ++it) {
            if (!it.value().is_object()) {
                Logger::getInstance().error("Invalid type for element '" + it.key() + "' in 'structure', object expected");
                return std::nullopt;
            }
            auto localRootNode = evalObject(it.value(), chain);
            if(!localRootNode){
                Logger::getInstance().error("Error in parsing structure with key = '"+it.key()+"'");
                return std::nullopt;
            }
            localRootNode.value()->setId(it.key());
            auto child = localRootNode.value();
            child->setChain(chain);
            thisNode->addChild(child);
             
        }                
    } 
    */

    return thisNode;
}

std::optional<std::shared_ptr<ChainNodeAttribute>> SchemaCatalog::evalAttribute(const nlohmann::json& jsonAttribute){
    if(jsonAttribute.is_boolean()){
        std::shared_ptr<ChainNodeAttribute> thisAttribute = std::make_shared<ChainNodeAttribute>(jsonAttribute.get<bool>());
        return thisAttribute;
    } else if(jsonAttribute.is_number_integer()){
        std::shared_ptr<ChainNodeAttribute> thisAttribute = std::make_shared<ChainNodeAttribute>(jsonAttribute.get<int64_t>());
        return thisAttribute;
    } else if(jsonAttribute.is_number_float()){
        std::shared_ptr<ChainNodeAttribute> thisAttribute = std::make_shared<ChainNodeAttribute>(jsonAttribute.get<double>());
        return thisAttribute;
    } else if(jsonAttribute.is_string()){
        std::shared_ptr<ChainNodeAttribute> thisAttribute = std::make_shared<ChainNodeAttribute>(jsonAttribute.get<std::string>());
        return thisAttribute;
    } else if(jsonAttribute.is_array()){
        ChainNodeAttribute::ChainNodeAttributeArray attributeArray;
        attributeArray.reserve(jsonAttribute.size());
        for (auto& [attributeName, attributeValue] : jsonAttribute.items()) {
            auto attribute = evalAttribute(attributeValue);
            if(!attribute){
                Logger::getInstance().error("Invalid attribute in array at position <"+attributeName+">");
                return std::nullopt;
            }
            attributeArray.emplace_back(*attribute.value());
        }
        std::shared_ptr<ChainNodeAttribute> thisAttribute = std::make_shared<ChainNodeAttribute>(attributeArray);
        return thisAttribute;
    } else if(jsonAttribute.is_object()){
        ChainNodeAttribute::ChainNodeAttributeObject attributeObject;
        attributeObject.reserve(jsonAttribute.size());
        for (auto& [attributeName, attributeValue] : jsonAttribute.items()) {
            auto attribute = evalAttribute(attributeValue);
            if(!attribute){
                Logger::getInstance().error("Invalid attribute with key <"+attributeName+">");
                return std::nullopt;
            }
            std::pair<std::string,ChainNodeAttribute> thisPair(attributeName, *attribute.value());
            attributeObject.insert(thisPair);
        }
        std::shared_ptr<ChainNodeAttribute> thisAttribute = std::make_shared<ChainNodeAttribute>(attributeObject);
        return thisAttribute;
    }
    Logger::getInstance().error("Invalid attribute type");
    return std::nullopt;
}

std::string SchemaCatalog::to_string(size_t indent = 0) const {
    std::ostringstream oss;
    std::string indentStr(indent, ' ');
    oss << "{\n" << indentStr; 
    for (const auto& [schemaName, schemaDefinition] : schemaMap) {
        oss << indentStr << "\"" << schemaName << "\": {\n";
        oss << indentStr << schemaDefinition.to_string() << "\n";
        oss << "}\n";
    }
    oss << "}\n";
    return oss.str();
}
