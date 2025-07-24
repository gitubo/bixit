#include <nlohmann/json.hpp> // Include the nlohmann/json library for JSON handling
#include <bixit.hpp>       // Include the Bixit library header
#include <iostream>

int main(int argc, char* argv[])
{
    // Create a catalog of schemas, loading schemas from the current directory ("./")
    // and setting the logging level to DEBUG for detailed output.
    std::string directory_path = "/app/examples";
    auto catalog = std::make_shared<bixit::catalog::SchemaCatalog>(directory_path, bixit::logger::Logger::Level::DEBUG);
    
    try
    {
        // Retrieve an abstract chain (the decoder) from the catalog
        // using the specific type "rbc_pre_announcement".
        auto chain_node = catalog->getAbstractChain("rbc_pre_announcement");
        if(!chain_node){
            // Check if the requested chain node was successfully retrieved.
            std::cerr << "[Error]: problem in creating the decoder for rbc_pre_announcement message type" << std::endl;
            return 1;
        }

        // Create a BitStream object from a Base64 encoded string and its bit length.
        // This represents the binary data to be decoded.
        auto bit_stream = bixit::bitstream::BitStream("yRCwAIgAEAECAgUARCAICAQICwAIcFADgAAACoA=", 225);
        // Declare a nlohmann::ordered_json object to store the decoded JSON data.
        nlohmann::ordered_json json;
        // Call the bitstream_to_json function of the chain node to decode the binary data
        // into the 'json' object. The return value 'retval' indicates success (0) or failure.
        int retval = chain_node->bitstream_to_json(bit_stream, json);
        if(retval){
            // Check if the decoding process returned an error code.
            std::cerr << "[Error]: bitstream_to_json returned code " << std::to_string(retval)<< std::endl;
            return 2; // Indicate an error during the decoding process.
        }
        
        // Print the decoded JSON to the standard output. The .unflatten() function
        // might be used to restructure the JSON into a more readable format.
        std::cout << json.unflatten().dump(2) << std::endl;

    }
    catch (const nlohmann::json::parse_error& e)
    {
        // Catch any exceptions that might occur during the dissection.
        std::cerr << "[Error]: " << e.what() << std::endl;
        return 3; 
    }

    return 0;
}