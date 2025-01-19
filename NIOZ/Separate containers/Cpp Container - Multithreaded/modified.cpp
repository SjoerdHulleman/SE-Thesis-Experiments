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
std::vector<std::vector<double>> intPP_mixed(
       const std::vector<double>& zmax,
       const std::vector<std::vector<double>>& kd,
       const std::vector<std::vector<double>>& par,
       const std::vector<std::vector<double>>& alfa,
       const std::vector<std::vector<double>>& eopt,
       const std::vector<std::vector<double>>& pmax,
       const std::vector<std::vector<double>>& height) {

  static const double xg[] = {0.04691008, 0.23076534, 0.5, 0.76923466, 0.95308992};
  static const double wg[] = {0.1184634, 0.2393143, 0.2844444, 0.2393143, 0.1184634};

  int nt = kd.size();            // number of rows
  int ns = zmax.size();           // number of columns
  std::vector<std::vector<double>> pp(nt, std::vector<double>(ns, 0.0));

  #pragma omp parallel for collapse(2) shared(pp, zmax, kd, par, alfa, eopt, pmax, height, xg, wg)
  for (int is = 0; is < ns; ++is) {
    for (int it = 0; it < nt; ++it) {
      double zt = zmax[is] + height[it][is];
      double total = 0.0;
      if (zt > 0.0 && (par[it][is] > 0)) {
        double zg = -std::log(0.001) / kd[it][is];
        if (zg > zt) zg = zt;

        double f1 = 1.0 / (alfa[it][is] * std::pow(eopt[it][is], 2.0));
        double f2 = 1.0 / pmax[it][is] - 2.0 / (alfa[it][is] * eopt[it][is]);
        double f3 = 1.0 / alfa[it][is];

        for (int j = 0; j < 5; ++j) {
          double zz = xg[j] * zg;
          double parz = par[it][is] * std::exp(-kd[it][is] * zz);
          double ps = parz / (f1 * std::pow(parz, 2.0) + f2 * parz + f3);
          total += wg[j] * ps * zg;
        }
      }
      pp[it][is] = total;
    }
  }
  return pp;
}


std::vector<std::vector<double>> rad_bot(
    const std::vector<double>& zmax,
    const std::vector<std::vector<double>>& kd,
    const std::vector<std::vector<double>>& par,
    const std::vector<std::vector<double>>& height) {

  int nt = kd.size();           // Number of rows
  int ns = zmax.size();          // Number of columns
  std::vector<std::vector<double>> rad(nt, std::vector<double>(ns, 0.0));

  #pragma omp parallel for collapse(2) shared(rad, zmax, kd, par, height)
  for (int is = 0; is < ns; ++is) {
    for (int it = 0; it < nt; ++it) {
      double zt = zmax[is] + height[it][is];
      if (zt > 0.0) {
        rad[it][is] = par[it][is] * std::exp(-kd[it][is] * zt);
      } else {
        rad[it][is] = par[it][is];
      }
    }
  }
  return rad;
}

std::vector<std::vector<double>> intPP_exp(
        const std::vector<double>& zmax,
        const std::vector<double>& kd,
        const std::vector<double>& pMud,
        const std::vector<std::vector<double>>& par,
        const std::vector<std::vector<double>>& alfa,
        const std::vector<std::vector<double>>& eopt,
        const std::vector<std::vector<double>>& pmax) {

  static const double xg[] = {0.04691008, 0.23076534, 0.5, 0.76923466, 0.95308992};
  static const double wg[] = {0.1184634, 0.2393143, 0.2844444, 0.2393143, 0.1184634};

  int nt = par.size();          // Number of rows
  int ns = zmax.size();          // Number of columns
  std::vector<std::vector<double>> pp(nt, std::vector<double>(ns, 0.0));

  #pragma omp parallel for collapse(2) shared(pp, zmax, kd, pMud, par, alfa, eopt, pmax, xg, wg)
  for (int is = 0; is < ns; ++is) {
    for (int it = 0; it < nt; ++it) {
      double zt = zmax[is];
      double total = 0.0;
      if (zt > 0.0 && (par[it][is] > 0)) {
        double zg = -std::log(0.001) / kd[is];
        if (zg > zt) zg = zt;

        for (int j = 0; j < 5; ++j) {
          double zz = xg[j] * zg;
          double pChl_z = (1 - pMud[is]) + pMud[is] * 2.0 * std::exp(1.0) * std::exp(-2.0 * std::exp(1.0) * zz / zt);
          double alfa_z = alfa[it][is] * pChl_z;
          double pmax_z = pmax[it][is] * pChl_z;
          double eopt_z = eopt[it][is];

          double f1 = 1.0 / (alfa_z * std::pow(eopt_z, 2.0));
          double f2 = 1.0 / pmax_z - 2.0 / (alfa_z * eopt_z);
          double f3 = 1.0 / alfa_z;

          double parz = par[it][is] * std::exp(-kd[is] * zz);
          double ps = parz / (f1 * std::pow(parz, 2.0) + f2 * parz + f3);
          total += wg[j] * ps * zg;
        }
      }
      pp[it][is] = total;
    }
  }
  return pp;
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
	CROW_ROUTE(app, "/intPP_mixed")
		.methods("POST"_method)
		([](const crow::request& req) {

		std::cout << "Received request for intPP_mixed" << std::endl;

		crow::json::rvalue jsonBody = crow::json::load(req.body);

		if (!jsonBody) {
			throw std::invalid_argument("Invalid JSON format");
		}

		// ----- Auto generate inputs -----
		std::vector<std::string> inputs = {"zmax","kd","par","alfa","eopt","pmax","height"};
		// ----- End of auto generate inputs -----

		auto start_conv = std::chrono::high_resolution_clock::now();
		std::vector<std::any> converted_inputs = convertInputs(inputs, jsonBody);

		// ----- Auto generate assigned converted results and function call -----
		const std::vector<double> & zmax = std::any_cast<const std::vector<double> &>(converted_inputs[0]);
		const std::vector<std::vector<double>> & kd = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[1]);
		const std::vector<std::vector<double>> & par = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[2]);
		const std::vector<std::vector<double>> & alfa = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[3]);
		const std::vector<std::vector<double>> & eopt = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[4]);
		const std::vector<std::vector<double>> & pmax = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[5]);
		const std::vector<std::vector<double>> & height = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[6]);
		auto end_conv = std::chrono::high_resolution_clock::now();
		auto duration_conv = std::chrono::duration_cast<std::chrono::milliseconds>(end_conv - start_conv);

		std::cout << "Conversion: " << duration_conv.count() << std::endl;

		auto start_call = std::chrono::high_resolution_clock::now();
		std::vector<std::vector<double>> result = intPP_mixed(zmax,kd,par,alfa,eopt,pmax,height);
		auto end_call = std::chrono::high_resolution_clock::now();
		auto duration_call = std::chrono::duration_cast<std::chrono::milliseconds>(end_call - start_call);
		std::cout << "Function: " << duration_call.count() << std::endl;

		// ----- End of auto generate assigned converted results and function call -----

		json json_result = result;
		crow::response response(json_result.dump());
		response.add_header("Content-Type", "application/json");
		std::cout << "Sending response for intPP_mixed" << std::endl;

		return response;
	});

	CROW_ROUTE(app, "/rad_bot")
		.methods("POST"_method)
		([](const crow::request& req) {

		std::cout << "Received request for rad_bot" << std::endl;

		crow::json::rvalue jsonBody = crow::json::load(req.body);

		if (!jsonBody) {
			throw std::invalid_argument("Invalid JSON format");
		}

		// ----- Auto generate inputs -----
		std::vector<std::string> inputs = {"zmax","kd","par","height"};
		// ----- End of auto generate inputs -----

		auto start_conv = std::chrono::high_resolution_clock::now();
		std::vector<std::any> converted_inputs = convertInputs(inputs, jsonBody);

		// ----- Auto generate assigned converted results and function call -----
		const std::vector<double> & zmax = std::any_cast<const std::vector<double> &>(converted_inputs[0]);
		const std::vector<std::vector<double>> & kd = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[1]);
		const std::vector<std::vector<double>> & par = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[2]);
		const std::vector<std::vector<double>> & height = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[3]);
		auto end_conv = std::chrono::high_resolution_clock::now();
		auto duration_conv = std::chrono::duration_cast<std::chrono::milliseconds>(end_conv - start_conv);

		std::cout << "Conversion: " << duration_conv.count() << std::endl;

		auto start_call = std::chrono::high_resolution_clock::now();
		std::vector<std::vector<double>> result = rad_bot(zmax,kd,par,height);
		auto end_call = std::chrono::high_resolution_clock::now();
		auto duration_call = std::chrono::duration_cast<std::chrono::milliseconds>(end_call - start_call);
		std::cout << "Function: " << duration_call.count() << std::endl;

		// ----- End of auto generate assigned converted results and function call -----

		json json_result = result;
		crow::response response(json_result.dump());
		response.add_header("Content-Type", "application/json");
		std::cout << "Sending response for rad_bot" << std::endl;

		return response;
	});

	CROW_ROUTE(app, "/intPP_exp")
		.methods("POST"_method)
		([](const crow::request& req) {

		std::cout << "Received request for intPP_exp" << std::endl;

		crow::json::rvalue jsonBody = crow::json::load(req.body);

		if (!jsonBody) {
			throw std::invalid_argument("Invalid JSON format");
		}

		// ----- Auto generate inputs -----
		std::vector<std::string> inputs = {"zmax","kd","pMud","par","alfa","eopt","pmax"};
		// ----- End of auto generate inputs -----

		auto start_conv = std::chrono::high_resolution_clock::now();
		std::vector<std::any> converted_inputs = convertInputs(inputs, jsonBody);

		// ----- Auto generate assigned converted results and function call -----
		const std::vector<double> & zmax = std::any_cast<const std::vector<double> &>(converted_inputs[0]);
		const std::vector<double> & kd = std::any_cast<const std::vector<double> &>(converted_inputs[1]);
		const std::vector<double> & pMud = std::any_cast<const std::vector<double> &>(converted_inputs[2]);
		const std::vector<std::vector<double>> & par = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[3]);
		const std::vector<std::vector<double>> & alfa = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[4]);
		const std::vector<std::vector<double>> & eopt = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[5]);
		const std::vector<std::vector<double>> & pmax = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[6]);
		auto end_conv = std::chrono::high_resolution_clock::now();
		auto duration_conv = std::chrono::duration_cast<std::chrono::milliseconds>(end_conv - start_conv);

		std::cout << "Conversion: " << duration_conv.count() << std::endl;

		auto start_call = std::chrono::high_resolution_clock::now();
		std::vector<std::vector<double>> result = intPP_exp(zmax,kd,pMud,par,alfa,eopt,pmax);
		auto end_call = std::chrono::high_resolution_clock::now();
		auto duration_call = std::chrono::duration_cast<std::chrono::milliseconds>(end_call - start_call);
		std::cout << "Function: " << duration_call.count() << std::endl;

		// ----- End of auto generate assigned converted results and function call -----

		json json_result = result;
		crow::response response(json_result.dump());
		response.add_header("Content-Type", "application/json");
		std::cout << "Sending response for intPP_exp" << std::endl;

		return response;
	});



    app.port(8080).multithreaded().run();
}
