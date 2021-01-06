#include "generate_blue_noise.h"

int main(int argc, char** argv) {
	int x_resolution = 32, y_resolution = 32;
	int depth = 1;
	int max_iters = 4000;
	int kernel_size = -1;
	bool verbose = true;

	BlueNoiseGenerator generator(x_resolution, y_resolution, depth, kernel_size);
	generator.optimize(max_iters, verbose);

	return 0;
}
