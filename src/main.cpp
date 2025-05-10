#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>

using json = nlohmann::json;

// ----------- Helpers ------------

std::map<std::string, std::string> parse_env_file(const std::string& filepath) {
    std::ifstream file(filepath);
    std::map<std::string, std::string> result;
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        size_t pos = line.find('=');
        if (pos == std::string::npos) continue;
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        result[key] = value;
    }

    return result;
}

std::string get_service_prefix(const std::string& key) {
    size_t pos = key.find('_');
    return (pos != std::string::npos) ? key.substr(0, pos) : "";
}

json build_payload_for_service(const std::string& service) {
    if (service == "openai" || service == "mistral") {
        return {
            {"model", service == "openai" ? "gpt-4" : "mistral-large-latest"},
            {"messages", {
                {{"role", "system"}, {"content", "You are a helpful assistant."}},
                {{"role", "user"}, {"content", "What is the capital of France?"}}
            }},
            {"temperature", 0.7}
        };
    } else if (service == "gemini") {
        return {
            {"contents", {
                {
                    {"parts", {
                        {{"text", "Explain how AI works"}}
                    }}
                }
            }},
            {"generationConfig", {
                {"stopSequences", {"Title"}},
                {"temperature", 0.7},
                {"maxOutputTokens", 1024},
                {"topP", 0.8},
                {"topK", 10}
            }}
        };     
    } else if (service == "claude") {
        return {
            {"model", "claude-3-7-sonnet-20250219"},
            {"max_tokens", 1024},
            {"messages", {
                {{"role", "user"}, {"content", "What is the capital of France?"}}
            }}
        };
    }
    return json(); // Empty fallback
}

cpr::Header build_headers_for_service(const std::string& service, const std::string& api_key) {
    if (service == "openai" || service == "mistral") {
        return {
            {"Authorization", "Bearer " + api_key},
            {"Content-Type", "application/json"}
        };
    } else if (service == "gemini") {
        return {
            {"Content-Type", "application/json"}
        };
    } else if (service == "claude") {
        return {
            {"x-api-key", api_key},
            {"anthropic-version", "2023-06-01"},
            {"Content-Type", "application/json"}
        };
    }
    return {};
}

// ----------- Main ------------

int main() {
    auto env_vars = parse_env_file("../.env");
    auto link_vars = parse_env_file("../.links");

    std::map<std::string, std::string> service_api_keys;
    std::map<std::string, std::string> service_urls;

    for (const auto& [key, val] : env_vars) {
        std::string service = get_service_prefix(key);
        if (key == service + "_api_key") {
            service_api_keys[service] = val;
        }
    }

    for (const auto& [key, val] : link_vars) {
        std::string service = get_service_prefix(key);
        if (key == service + "_url") {
            service_urls[service] = val;
        }
    }

    for (const auto& [service, url] : service_urls) {
        if (service_api_keys.find(service) == service_api_keys.end()) {
            std::cerr << "⚠️ Skipping " << service << ": Missing API key\n";
            continue;
        }

        json payload = build_payload_for_service(service);
        cpr::Header headers = build_headers_for_service(service, service_api_keys[service]);

        if (payload.empty() || headers.empty()) {
            std::cerr << "⚠️ Skipping " << service << ": Unsupported payload or headers\n";
            continue;
        }

        std::cout << "🔄 Sending request to [" << service << "] at " << url << "\n";

        // Special case: Gemini puts API key in the URL
        std::string full_url = (service == "gemini")
            ? url + "?key=" + service_api_keys[service]
            : url;

        cpr::Response r = cpr::Post(
            cpr::Url{full_url},
            headers,
            cpr::Body{payload.dump()}
        );

        if (r.status_code == 200) {
            std::cout << "✅ Response from " << service << ":\n" << r.text << "\n";
        } else {
            std::cerr << "❌ " << service << " failed (HTTP " << r.status_code << ")\n";
            std::cerr << "  Response: " << r.text << "\n";
        }
    }

    return 0;
}
