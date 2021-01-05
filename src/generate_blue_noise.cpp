#include <vector>
#include <cmath>
#include "image.h"
#include "pbar.h"

// random float number in range [0,1]
Float NextFloat() {
	return  static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

// distance between two 2D points
Float DistanceSquared(Float x1, Float y1, Float x2, Float y2) {
	return (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
}

Float DistanceSamples(const std::vector<Float>& p, const std::vector<Float>& q) {
	const int d = p.size();
	Float res = 0.0f;
	for (size_t i = 0; i < p.size(); ++i) {
		res += std::pow(std::abs(p[i] - q[i]), (d / 2.0));
	}
	return res;
}

// compute the loss function now
Float compute_blue_noise_loss(int x_resolution, int y_resolution, const std::vector<std::vector<Float> >& blue_noise, int kernel_size) {
	Float loss = 0;
	const Float sigma_i = 2.1f;
	const Float sigma_s = 1.0f;

#pragma omp parallel for num_threads(6) reduction(+: loss) // use openmp 
	for (int px = 0; px < x_resolution; ++px) {
		for (int py = 0; py < y_resolution; ++py) {
			int i = px * x_resolution + py;
			std::vector<Float> ps = blue_noise[i];

			Float pix = i / x_resolution, piy = i % x_resolution;

			if (kernel_size == -1) {
				// use all pixel pairs to compute the loss
				for (int qx = 0; qx < x_resolution; ++qx) {
					for (int qy = 0; qy < y_resolution; ++qy) {
						Float qix = qx, qiy = qy;

						Float pixel_distance = DistanceSquared(pix, piy, qix, qiy);

						int qqx = (qx + x_resolution) % x_resolution;
						int qqy = (qy + y_resolution) % y_resolution;

						int j = qqx * x_resolution + qqy;
						if (j == i) continue;
						std::vector<Float> qs = blue_noise[j];

						Float sample_distance = DistanceSamples(ps, qs);
						loss += std::exp(
							-(pixel_distance) / (sigma_i * sigma_i)
							- (sample_distance) / (sigma_s * sigma_s));
					}
				}
			}
			else {
				// use all neighborhood around the pixel (px,py) to approximate the loss
				for (int qx = pix - kernel_size; qx <= pix + kernel_size; ++qx) {
					for (int qy = piy - kernel_size; qy <= piy + kernel_size; ++qy) {
						Float qix = qx, qiy = qy;

						Float pixel_distance = DistanceSquared(pix, piy, qix, qiy);

						int qqx = (qx + x_resolution) % x_resolution;
						int qqy = (qy + y_resolution) % y_resolution;

						int j = qqx * x_resolution + qqy;
						if (j == i) continue;
						std::vector<Float> qs = blue_noise[j];

						Float sample_distance = DistanceSamples(ps, qs);
						loss += std::exp(
							-(pixel_distance) / (sigma_i * sigma_i)
							- (sample_distance) / (sigma_s * sigma_s));
					}
				}
			}
			
			
					
					

		}
	}
	return loss;
}


void generate_blue_noise(int x_resolution, int y_resolution, int depth, int max_iter, int kernel_size=-1) {
	std::vector<std::vector<Float> > blue_noise;
	blue_noise.resize(x_resolution * y_resolution);

	// 1. generate white noise.
	for (size_t i = 0; i < x_resolution * y_resolution; ++i) {
		for (size_t d = 0; d < depth; ++d) {
			blue_noise[i].push_back(NextFloat());
		}
	}

	ImageRGB img_white(x_resolution, y_resolution);
	for (int x = 0; x < x_resolution; ++x) {
		for (int y = 0; y < y_resolution; ++y) {
			std::vector<Float> v = blue_noise[y * x_resolution + x];
			img_white.Set(x, y, v);
		}
	}
	img_white.save("white_noise.bmp", 1.0f);

	// 2. random swap two pixels to minimize the loss
	Float loss = compute_blue_noise_loss(x_resolution, y_resolution, blue_noise);
	int size = x_resolution * y_resolution;

	int progress_iters = int(float(max_iter) / 10.0f);

	ProgressBar<std::vector<int>::iterator> pbar(v.begin(), v.end(), 50);
	for (auto i = pbar.begin(); i != pbar.end(); i++) {
		usleep(us);
	}
	for (int iter = 0; iter < max_iter; ++iter) {
		int i = NextFloat() * (size - 1);
		int j = NextFloat() * (size - 1);
		if (i == j) continue;
		
		std::swap(blue_noise[i], blue_noise[j]);

		Float new_loss = compute_blue_noise_loss(x_resolution, y_resolution, blue_noise, kernel_size);

		// swap back.
		if (new_loss > loss) {
			std::swap(blue_noise[i], blue_noise[j]);
		}
		else {
			loss = new_loss;
		}

		if (iter % progress_iters == 0)
			printf("loss: %0.4f, iter:%d\n", loss, iter);

		// save current results
		ImageRGB img(x_resolution, y_resolution);
		for (int x = 0; x < x_resolution; ++x) {
			for (int y = 0; y < y_resolution; ++y) {
				std::vector<Float> v = blue_noise[y * x_resolution + x];
				img.Set(x, y, v);
			}
		}

		img.save("blue_noise.bmp", 1.0f);
	}
}

int main(int argc, char** argv) {
	int x_resolution = 32, y_resolution = 32;
	int depth = 1;
	int max_iters = 4000;
	int kernel_size = -1;

	generate_blue_noise(x_resolution, y_resolution, depth, max_iters, kernel_size);

	return 0;
}
