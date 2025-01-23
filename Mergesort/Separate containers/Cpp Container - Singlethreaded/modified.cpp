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
        std::vector<int> resVector;
        std::vector<std::vector<int>> resMatrix;

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

                    std::vector<std::vector<int>> matrix = json_matrix.get<std::vector<std::vector<int>>>();

                    converted_inputs.push_back(matrix);
                }
                else {
                    // It is a vector
                    json json_vector = json::parse(input_str);

                    std::vector<int> vector = json_vector.get<std::vector<int>>();

                    converted_inputs.push_back(vector);
                }
            }
            break;
        }
    }
    return converted_inputs;
}

// # MARKER: TARGET CPP FUNCTIONS #
std::vector<int> mergeSort(const std::vector<int>& arr) {
    int n = arr.size();
    if (n <= 1)
        return arr;

    // Find the midpoint and divide the array
    int mid = n / 2;
    std::vector<int> left(arr.begin(), arr.begin() + mid);
    std::vector<int> right(arr.begin() + mid, arr.end());

    // Recursively sort both halves
    #pragma omp parallel
    {
        #pragma omp single
        {
            #pragma omp task shared(left)
            left = mergeSort(left);

            #pragma omp task shared(right)
            right = mergeSort(right);

            #pragma omp taskwait
        }
    }

    // Merge the sorted halves back into the original array
    std::vector<int> result(n);
    int i = 0, j = 0, k = 0;

    while (i < left.size() && j < right.size()) {
        if (left[i] <= right[j]) {
            result[k++] = left[i++];
        } else {
            result[k++] = right[j++];
        }
    }

    // Copy remaining elements from the left array
    while (i < left.size()) {
        result[k++] = left[i++];
    }

    // Copy remaining elements from the right array
    while (j < right.size()) {
        result[k++] = right[j++];
    }

    return result;
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
	CROW_ROUTE(app, "/mergeSort")
		.methods("POST"_method)
		([](const crow::request& req) {

		std::cout << "Received request for mergeSort" << std::endl;

		crow::json::rvalue jsonBody = crow::json::load(req.body);

		if (!jsonBody) {
			throw std::invalid_argument("Invalid JSON format");
		}

		// ----- Auto generate inputs -----
		std::list<std::string> inputs = {"arr"};
		// ----- End of auto generate inputs -----

		auto start_conv = std::chrono::high_resolution_clock::now();
		std::vector<std::any> converted_inputs = convertInputs(inputs, jsonBody);

		// ----- Auto generate assigned converted results and function call -----
		const std::vector<int> & arr = std::any_cast<const std::vector<int> &>(converted_inputs[0]);
		auto end_conv = std::chrono::high_resolution_clock::now();
		auto duration_conv = std::chrono::duration_cast<std::chrono::milliseconds>(end_conv - start_conv);

		std::cout << "Conversion from request data for mergeSort took: " << duration_conv.count() << std::endl;

		auto start_call = std::chrono::high_resolution_clock::now();
		std::vector<int> result = mergeSort(arr);
		auto end_call = std::chrono::high_resolution_clock::now();
		auto duration_call = std::chrono::duration_cast<std::chrono::milliseconds>(end_call - start_call);
		std::cout << "Call of function mergeSort took: " << duration_call.count() << std::endl;

		// ----- End of auto generate assigned converted results and function call -----

		json json_result = result;
		crow::response response(json_result.dump());
		response.add_header("Content-Type", "application/json");
		std::cout << "Sending response for mergeSort" << std::endl;

		return response;
	});



    app.port(8080).multithreaded().run();
}
