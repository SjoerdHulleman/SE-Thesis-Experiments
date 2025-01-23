// CppBase.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "crow_all.h"
#include <iostream>
#include <list>
#include <string>
#include "json.hpp"
#include <omp.h>
#include <mutex>

using json = nlohmann::json;

std::vector<std::any> convertInputs(
    std::vector<std::string> inputs,
    crow::json::rvalue jsonBody
) {
    std::vector<std::any> converted_inputs(inputs.size());
    std::mutex json_mutex;

#pragma omp parallel for
    for (int i = 0; i < inputs.size(); i++) {
        const std::string input = inputs[i];
        crow::json::rvalue input_rval;

        // Lock access to jsonBody, since jsonBody is not thread safe
        {
            std::lock_guard<std::mutex> lock(json_mutex);
            if (!jsonBody.has(input)) {
#pragma omp critical
                std::cerr << "Key not found: " << input << std::endl;
                continue;
            }
            input_rval = jsonBody[input];
        }

        std::vector<double> resVector;
        std::vector<std::vector<double>> resMatrix;

        switch (input_rval.t()) {
        case crow::json::type::Number:
            // Convert to correct number type
            break;
        case crow::json::type::String:
            std::string input_str = input_rval.s();

            if (input_str[0] == '[') {
                if (input_str[1] == '[') {
                    // It is a matrix
                    json json_matrix = json::parse(input_str);

                    std::vector<std::vector<double>> matrix = json_matrix.get<std::vector<std::vector<double>>>();

                    converted_inputs[i] = matrix;
                }
                else {
                    // It is a vector
                    json json_vector = json::parse(input_str);

                    std::vector<double> vector = json_vector.get<std::vector<double>>();

                    converted_inputs[i] = vector;
                }
            }
            break;
        }
    }
    return converted_inputs;
}

// # MARKER: TARGET CPP FUNCTIONS #
std::vector<double> multiply_vectors(const std:: vector<double>& v1, const std::vector<double>& v2) {
    int size = v1.size();
    std::vector<double> result(size);

    #pragma omp parallel for schedule(guided, 2048)
    for (int i = 0; i < size; i++) {
        result[i] = v1[i] * v2[i];
    }

    return result;
}


// This file functions as a boiler plate for the CppModifier.
int main()
{

    std::cout << "Hello World!\n";

    omp_set_num_threads(4);

    crow::SimpleApp app;

    // Endpoints to test API running
    CROW_ROUTE(app, "/")([]() {
        return "Hello, Crow!";
    });


    CROW_ROUTE(app, "/ping")([]() {
        return "pong";
    });

    // # MARKER: API ENDPOINTS #
	CROW_ROUTE(app, "/multiply_vectors")
		.methods("POST"_method)
		([](const crow::request& req) {

		std::cout << "Received request for multiply_vectors" << std::endl;

		crow::json::rvalue jsonBody = crow::json::load(req.body);

		if (!jsonBody) {
			throw std::invalid_argument("Invalid JSON format");
		}

		// ----- Auto generate inputs -----
		std::vector<std::string> inputs = {"v1","v2"};
		// ----- End of auto generate inputs -----

		auto start_conv = std::chrono::high_resolution_clock::now();
		std::vector<std::any> converted_inputs = convertInputs(inputs, jsonBody);

		// ----- Auto generate assigned converted results and function call -----
		const std::vector<double> & v1 = std::any_cast<const std::vector<double> &>(converted_inputs[0]);
		const std::vector<double> & v2 = std::any_cast<const std::vector<double> &>(converted_inputs[1]);
		auto end_conv = std::chrono::high_resolution_clock::now();
		auto duration_conv = std::chrono::duration_cast<std::chrono::milliseconds>(end_conv - start_conv);

		std::cout << "Conversion from request data for multiply_vectors took: " << duration_conv.count() << std::endl;

		auto start_call = std::chrono::high_resolution_clock::now();
		std::vector<double> result = multiply_vectors(v1,v2);
		auto end_call = std::chrono::high_resolution_clock::now();
		auto duration_call = std::chrono::duration_cast<std::chrono::milliseconds>(end_call - start_call);
		std::cout << "Call of function multiply_vectors took: " << duration_call.count() << std::endl;

		// ----- End of auto generate assigned converted results and function call -----

		json json_result = result;
		crow::response response(json_result.dump());
		response.add_header("Content-Type", "application/json");
		std::cout << "Sending response for multiply_vectors" << std::endl;

		return response;
	});



    app.port(8080).multithreaded().run();
}
