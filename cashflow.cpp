#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

#include <NumCpp.hpp>
#include <cpprest/http_client.h>
#include <cpprest/json.h>


namespace py = pybind11;

class CashFlow{
private:
	std::vector<double> m_flow;
public:
	CashFlow(){}
	
	void add(double x){
		// 创建一个 3x3 的 NumCpp 数组
		nc::NdArray<double> y = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};

		// 使用 NumCpp 的 roll 函数进行向上移位
		nc::NdArray<double> y_roll = nc::roll(y, -1, nc::Axis::ROW);

		// 打印结果
		std::cout << y << std::endl;
		std::cout << y_roll << std::endl;

		m_flow.push_back(x);      
	}
	void addVector(std::vector<double> const& xs){
		for(const auto& x: xs)
			m_flow.push_back(x);
	}	

	void clear(){
		web::http::client::http_client client(U("http://myip.ipip.net"));
		client.request(web::http::methods::GET).then([](web::http::http_response response) {
			if (response.status_code() == web::http::status_codes::OK) {
				return response.extract_string();
			}
			return pplx::task_from_result(std::string());
		}).then([](const std::string& responseString) {
			std::cout << "Your public IP address is: " << responseString << std::endl;
		}).wait();
		
		m_flow.clear();
	}
	
	int size(){
		return m_flow.size();
	}
	double get(int index){
		return m_flow[index];
	}
	double npv(double rate){
		double rr   = 1 + rate;
		double prod = 1.0;
		double acc  = 0;
		for(auto x: m_flow){
			acc  += x / prod;
			prod *= rr;
		}
		return acc;
	}
	void show(){
		int i = 0;
		for(auto x: m_flow){
			std::cout << "i  = " << i
					  << " c  = " << std::setw(10)
					  << std::fixed << std::setprecision(2) << x
					  << std::endl;
			i++;
		}
	}
};

int add(int i, int j) {
    return i + j;
}

PYBIND11_MODULE(cashflow, m) {
	// optional module docstring
    m.doc() = "pybind11 sample CashFlow module."; 
	// Add function to module.
    m.def("add", &add, "A function which adds two numbers");

	auto pyCashFlow = py::class_<CashFlow>(
			m,
			"CashFlow",
			R"pbdoc(Class for performing cash flow computations.)pbdoc"
			);
	pyCashFlow
		.def(py::init<>())             // Bind class constructor 
		.def("add",			&CashFlow::add)
		.def("addVector",	&CashFlow::addVector)
		.def("size",		&CashFlow::size)
		.def("get",			&CashFlow::get)
     	.def("show",		&CashFlow::show)
		.def("clear",		&CashFlow::clear)
		.def("npv",			&CashFlow::npv);
}



// int main(){
	
// 	return 0;
// }
