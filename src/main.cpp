#include <iostream>
#include <dotenv.h>

int main() {
    dotenv::env.load_dotenv("../.env");
    std::string api_key = dotenv::env["API_KEY"];
    std::cout << "API_KEY: " << api_key << std::endl;
    return 0;
}
