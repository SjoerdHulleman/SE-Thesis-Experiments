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
            // Convert to correct number type
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
                    // It is a vector
                    json json_vector = json::parse(input_str);

                    std::vector<double> vector = json_vector.get<std::vector<double>>();

                    converted_inputs.push_back(vector);
                }
            }
            break;
        }
    }
    return converted_inputs;
}

// # MARKER: TARGET CPP FUNCTIONS #
std::vector<std::vector<double>> est_bc_source(const std::vector<std::vector<double>>& geno) {
    int n_ind = geno.size();
    if (n_ind == 0) return {};
    int n_mar = geno[0].size();

    std::vector<std::vector<double>> rec(n_mar, std::vector<double>(n_mar, 0.0));

    for (int i = 0; i < n_mar - 1; i++) {
        for (int j = i + 1; j < n_mar; j++) {
            double ct = 0.0;
            for (int k = 0; k < n_ind; k++) {
                if (geno[k][i] != geno[k][j]) {
                    ct++;
                }
            }
            double recombination_rate = ct / n_ind;
            rec[i][j] = rec[j][i] = recombination_rate;
        }
    }

    return rec;
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
	CROW_ROUTE(app, "/est_bc_source")
		.methods("POST"_method)
		([](const crow::request& req) {

		std::cout << "Received request for est_bc_source" << std::endl;

		crow::json::rvalue jsonBody = crow::json::load(req.body);

		if (!jsonBody) {
			throw std::invalid_argument("Invalid JSON format");
		}

		// ----- Auto generate inputs -----
		std::list<std::string> inputs = {"geno"};
		// ----- End of auto generate inputs -----

        auto start_conv = std::chrono::high_resolution_clock::now();
		std::vector<std::any> converted_inputs = convertInputs(inputs, jsonBody);

		// ----- Auto generate assigned converted results and function call -----
		const std::vector<std::vector<double>> & geno = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[0]);
        auto end_conv = std::chrono::high_resolution_clock::now();
        auto duration_conv = std::chrono::duration_cast<std::chrono::milliseconds>(end_conv - start_conv);

        std::cout << "Conversion from request data for est_bc_source took: " << duration_conv.count() << std::endl;

        auto start_call = std::chrono::high_resolution_clock::now();
		std::vector<std::vector<double>> result = est_bc_source(geno);
        auto end_call = std::chrono::high_resolution_clock::now();
        auto duration_call = std::chrono::duration_cast<std::chrono::milliseconds>(end_call - start_call);
        std::cout << "Call of function est_bc_source took: " << duration_call.count() << std::endl;

		// ----- End of auto generate assigned converted results and function call -----

		json json_result = result;
		crow::response response(json_result.dump());
		response.add_header("Content-Type", "application/json");
		std::cout << "Sending response for est_bc_source" << std::endl;

		return response;
	});



    app.port(8080).multithreaded().run();
}
