#include <nlohmann/json.hpp>
#include <bixit.hpp>


int main(int argc, char* argv[])
{
    auto catalog = std::make_shared<bixit::catalog::SchemaCatalog>("./", bixit::logger::Logger::Level::DEBUG);

    try
    { 
        auto chain_node = catalog->getAbstractChain("rbc_pre_announcement");
        if(!chain_node){
            std::cerr << "[Error]: getAbstractChain" << std::endl;
            return 1;
        }

        auto bit_stream = bixit::bitstream::BitStream("yRCwAIgAEAECAgUARCAICAQICwAIcFADgAAACoA=", 225);
        nlohmann::ordered_json json;
        int retval = chain_node->bitstream_to_json(bit_stream, json);
        if(retval){
            std::cerr << "[Error]: bitstream_to_json returned code " << std::to_string(retval)<< std::endl;
            return 2;
        }
        std::cout << json.dump() << std::endl;

    }
    catch (const nlohmann::json::parse_error& e)
    {
        std::cerr << "[Error]: " << e.what() << std::endl;
        return 3;
    }

    return 0;
}
