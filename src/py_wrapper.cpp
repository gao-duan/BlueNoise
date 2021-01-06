#include <pybind11/pybind11.h>
#include "generate_blue_noise.h"
#include <pybind11/stl.h>

namespace py = pybind11;

PYBIND11_MODULE(PyBlueNoiseGenerator, m) {
	m.doc() = "Generate blue noise texture"; 

	py::class_<BlueNoiseGenerator>(m, "BlueNoiseGenerator")
		.def(py::init())
		.def(py::init<int, int, int, int>(), 
			py::arg("x_resolution"), 
			py::arg("y_resolution"), 
			py::arg("depth")=1,
			py::arg("kernel_size")=-1)
		.def("optimize", py::overload_cast<int>(&BlueNoiseGenerator::optimize))

		.def_readwrite("blue_noise_texture", &BlueNoiseGenerator::blue_noise)
		.def_readwrite("white_noise_texture", &BlueNoiseGenerator::white_noise)
		.def_readwrite("x_resolution", &BlueNoiseGenerator::x_resolution)
		.def_readwrite("y_resolution", &BlueNoiseGenerator::y_resolution)
		.def_readwrite("depth", &BlueNoiseGenerator::depth)
		.def_readwrite("kernel_size", &BlueNoiseGenerator::kernel_size)
		.def_readwrite("threads", &BlueNoiseGenerator::threads)

		;
}

