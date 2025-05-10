#include <iostream>
#include <dotenv.h>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main() {
    // Load API key from .env
    dotenv::env.load_dotenv("../.env");
    std::string api_key = dotenv::env["API_KEY"];

    if (api_key.empty()) {
        std::cerr << "Missing API_KEY in .env" << std::endl;
        return 1;
    }

    std::cout << "API_KEY: " << api_key << std::endl;

    // Prepare OpenAI API request payload
    json payload = {
        {"model", "gpt-4"},
        {"messages", {
            {{"role", "system"}, {"content", "You are a helpful assistant."}},
            {{"role", "user"}, {"content", "What is the capital of France?"}}
        }},
        {"temperature", 0.7}
    };

    // Send POST request to OpenAI API
    cpr::Response r = cpr::Post(
        cpr::Url{"https://api.openai.com/v1/chat/completions"},
        cpr::Header{
            {"Authorization", "Bearer " + api_key},
            {"Content-Type", "application/json"}
        },
        cpr::Body{payload.dump()}
    );

    // Handle response
    if (r.status_code == 200) {
        json response = json::parse(r.text);
        std::cout << "LLM response: " << response["choices"][0]["message"]["content"] << std::endl;
    } else {
        std::cerr << "Request failed: " << r.status_code << "\n" << r.text << std::endl;
    }

    return 0;
}
