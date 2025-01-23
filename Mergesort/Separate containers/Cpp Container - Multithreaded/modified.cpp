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

                    converted_inputs[i] = matrix;
                }
                else {
                    // It is a vector
                    json json_vector = json::parse(input_str);

                    std::vector<int> vector = json_vector.get<std::vector<int>>();

                    converted_inputs[i] = vector;
                }
            }
            break;
        }
    }
    return converted_inputs;
}

// Mergesort code references:
// https://www.geeksforgeeks.org/merge-sort/
// https://www1.chapman.edu/~radenski/research/papers/mergesort-pdpta11.pdf

void merge(std::vector<int>& arr, int left,int mid, int right)
{
    int n1 = mid - left + 1;
    int n2 = right - mid;

    std::vector<int> L(n1), R(n2);

    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    int i = 0, j = 0;
    int k = left;

    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void mergeSort_serial(std::vector<int>& arr, int left, int right) {
    if (left >= right)
        return;
    
    int mid = left + (right - left) / 2;

    mergeSort_serial(arr, left, mid);
    mergeSort_serial(arr, mid + 1, right);
    merge(arr, left, mid, right);
}

void mergeSort(std::vector<int>& arr, int left, int right, int threads)
{
    if (left >= right)
        return;

    if (threads == 1) { mergeSort_serial(arr, left, right);}
    else if (threads > 1) {
        int mid = left + (right - left) / 2;
        #pragma omp parallel sections
        {
            #pragma omp section
            {
                mergeSort(arr, left, mid, threads / 2);
            }
            #pragma omp section
            {
                mergeSort(arr, mid + 1, right, threads / 2);
            }
        }
        merge(arr, left, mid, right);
    }
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
	CROW_ROUTE(app, "/mergeSort")
		.methods("POST"_method)
		([](const crow::request& req) {

		std::cout << "Received request for mergeSort" << std::endl;

		crow::json::rvalue jsonBody = crow::json::load(req.body);

		if (!jsonBody) {
			throw std::invalid_argument("Invalid JSON format");
		}

		// ----- Auto generate inputs -----
		std::vector<std::string> inputs = {"arr"};
		// ----- End of auto generate inputs -----

		auto start_conv = std::chrono::high_resolution_clock::now();
		std::vector<std::any> converted_inputs = convertInputs(inputs, jsonBody);

		// ----- Auto generate assigned converted results and function call -----
		std::vector<int> arr = std::any_cast<std::vector<int>>(converted_inputs[0]);
		auto end_conv = std::chrono::high_resolution_clock::now();
		auto duration_conv = std::chrono::duration_cast<std::chrono::milliseconds>(end_conv - start_conv);

		std::cout << "Conversion from request data for mergeSort took: " << duration_conv.count() << std::endl;

		auto start_call = std::chrono::high_resolution_clock::now();
        mergeSort(arr, 0, arr.size() - 1, 4);
		auto end_call = std::chrono::high_resolution_clock::now();
		auto duration_call = std::chrono::duration_cast<std::chrono::milliseconds>(end_call - start_call);
		std::cout << "Call of function mergeSort took: " << duration_call.count() << std::endl;

		// ----- End of auto generate assigned converted results and function call -----

		json json_result = arr;
		crow::response response(json_result.dump());
		response.add_header("Content-Type", "application/json");
		std::cout << "Sending response for mergeSort" << std::endl;

		return response;
	});



    app.port(8080).multithreaded().run();
}
