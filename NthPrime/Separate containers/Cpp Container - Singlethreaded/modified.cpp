// CppBase.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "crow_all.h"
#include <iostream>
#include <list>
#include <string>
#include "json.hpp"

using json = nlohmann::json;

std::vector<std::any> convertInputs(
    std::list<std::string> inputs,
    crow::json::rvalue jsonBody
) {
    std::vector<std::any> converted_inputs;

    for (const std::string input : inputs) {
        crow::json::rvalue input_rval = jsonBody[input];
        std::vector<double> resVector;
        std::vector<std::vector<double>> resMatrix;

        switch (input_rval.t()) {
        case crow::json::type::Number:
            break;
        case crow::json::type::String:
            std::string input_str = input_rval.s();

            if (input_str[0] == '[') {
                if (input_str[1] == '[') {
                    // It is a matrix
                    json json_matrix = json::parse(input_str);

                    std::vector<std::vector<double>> matrix = json_matrix.get<std::vector<std::vector<double>>>();

                    converted_inputs.push_back(matrix);
                }
                else {
                    if (input_str.find(',') == std::string::npos) {
                        // It is a single number
                        // 
                        // Remove [
                        input_str.erase(0, 1);
                        // Remove ]

                        input_str.erase(input_str.size() - 1, 1);
                        try {
                            // First try to convert to int
                            int result = std::stoi(input_str);
                            converted_inputs.push_back(result);
                        }
                        catch (std::invalid_argument) {
                            try {
                                // If not, then integer
                                double result = std::stod(input_str);
                                converted_inputs.push_back(result);
                            }
                            catch (std::invalid_argument) {
                                // Do nothing
                            }
                        }
                    }
                    else {

                        // It is a vector
                        json json_vector = json::parse(input_str);

                        std::vector<double> vector = json_vector.get<std::vector<double>>();

                        converted_inputs.push_back(vector);
                    }
                }
            }
            else {
                // It is a number
            }
            break;
        }
    }
    return converted_inputs;
}

// # MARKER: TARGET CPP FUNCTIONS #
int findNthPrime(int n) {
  int count = 0;
  int num = 1;
  
  while (count < n) {
    ++num;
    // Check if the number is prime
    bool isPrime = true;
    if (num <= 1) {
      isPrime = false;
    } else {
      for (int i = 2; i * i <= num; ++i) {
        if (num % i == 0) {
          isPrime = false;
          break;
        }
      }
    }
    
    if (isPrime) {
      ++count;
    }
  }
  
  return num;
}


// This file functions as a boiler plate for the CppModifier.
int main()
{

    std::cout << "Hello World!\n";

    crow::SimpleApp app;

    // Endpoints to test API running
    CROW_ROUTE(app, "/")([]() {
        return "Hello, Crow!";
    });


    CROW_ROUTE(app, "/ping")([]() {
        return "pong";
    });

    // # MARKER: API ENDPOINTS #
	CROW_ROUTE(app, "/findNthPrime")
		.methods("POST"_method)
		([](const crow::request& req) {

		std::cout << "Received request for findNthPrime" << std::endl;

		crow::json::rvalue jsonBody = crow::json::load(req.body);

		if (!jsonBody) {
			throw std::invalid_argument("Invalid JSON format");
		}

		// ----- Auto generate inputs -----
		std::list<std::string> inputs = {"n"};
		// ----- End of auto generate inputs -----

		auto start_conv = std::chrono::high_resolution_clock::now();
		std::vector<std::any> converted_inputs = convertInputs(inputs, jsonBody);

		// ----- Auto generate assigned converted results and function call -----
		int n = std::any_cast<int>(converted_inputs[0]);
		auto end_conv = std::chrono::high_resolution_clock::now();
		auto duration_conv = std::chrono::duration_cast<std::chrono::milliseconds>(end_conv - start_conv);

		std::cout << "Conversion from request data for findNthPrime took: " << duration_conv.count() << std::endl;

		auto start_call = std::chrono::high_resolution_clock::now();
		int result = findNthPrime(n);
		auto end_call = std::chrono::high_resolution_clock::now();
		auto duration_call = std::chrono::duration_cast<std::chrono::milliseconds>(end_call - start_call);
		std::cout << "Call of function findNthPrime took: " << duration_call.count() << std::endl;

		// ----- End of auto generate assigned converted results and function call -----

		json json_result = result;
		crow::response response(json_result.dump());
		response.add_header("Content-Type", "application/json");
		std::cout << "Sending response for findNthPrime" << std::endl;

		return response;
	});



    app.port(8080).multithreaded().run();
}
