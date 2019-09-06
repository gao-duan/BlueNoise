
#include <vector>
#include <cmath>
#include "image.h"

// random float number in range [0,1]
Float NextFloat() {
    return  static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

// distance between two 2D points
Float DistanceSquared(Float x1, Float y1, Float x2, Float y2) {
    return (x2-x1) * (x2-x1) + (y2-y1) * (y2-y1);
}

// compute the loss function now
Float compute_blue_noise_loss(int x_resolution, int y_resolution, const std::vector<Float>& blue_noise) {
		Float loss = 0;
		const Float sigma_i = 2.1f;
		const Float sigma_s = 1.0f;
		const Float d = 1.0f;

		const int kernel_size = 4;

#pragma omp parallel for num_threads(6) reduction(+: loss) // use openmp 
		for (int px = 0; px < x_resolution; ++px) {
			for (int py = 0; py < y_resolution; ++py) {
				int i = px * x_resolution + py;
				Float ps = blue_noise[i];
			
                Float pix = i / x_resolution, piy = i% x_resolution;

				// use all neighborhood around the pixel (px,py) to approximate the loss
				for (int qx = pix - kernel_size; qx <= pix + kernel_size; ++qx) {
					for (int qy = piy - kernel_size; qy <= piy + kernel_size; ++qy) {
						// use all pixel pairs to compute the loss
						//for(int qx = 0;qx < x_resolution;++qx) {
						//	for(int qy =0;qy<y_resolution;++qy) {

                        Float qix = qx, qiy = qy;

						Float pixel_distance = DistanceSquared(pix, piy, qix, qiy);

						int qqx = (qx + x_resolution) % x_resolution;
						int qqy = (qy + y_resolution) % y_resolution;

						int j = qqx * x_resolution + qqy;
						if (j == i) continue;
						Float qs = blue_noise[j];
						loss += std::exp(
							-(pixel_distance) / (sigma_i * sigma_i)
							- (std::pow(std::abs(ps - qs), (d / 2.0))) / (sigma_s * sigma_s));
					}
				}

			}
		}
		return loss;
	}


void  generate_blue_noise(int x_resolution, int y_resolution) {
        std::vector<Float> blue_noise;
		blue_noise.resize(x_resolution * y_resolution);

		// 1. generate white noise.
		for (size_t i = 0; i < x_resolution * y_resolution; ++i)
			blue_noise[i] = NextFloat();

		ImageRGB img_white(x_resolution, y_resolution);
		for (int x = 0; x < x_resolution; ++x) {
			for (int y = 0; y < y_resolution; ++y) {
				Float v = blue_noise[y * x_resolution + x];
				Color color(v, v, v);
				img_white.Set(x, y, color);
			}
		}
		img_white.save("white_noise.bmp", 1.0f);

		// 2. random swap two pixels to minimize the loss
		Float loss = compute_blue_noise_loss(x_resolution, y_resolution, blue_noise);
		int size = x_resolution * y_resolution;

		const int maxIter = 1000000;
		for(int iter = 0; iter<maxIter;++iter) {
			int i = NextFloat() * (size - 1);
			int j = NextFloat() * (size - 1);
			if (i == j) continue;
			Float tmp = blue_noise[i];
			blue_noise[i] = blue_noise[j];
			blue_noise[j] = tmp;
			Float new_loss = compute_blue_noise_loss(x_resolution, y_resolution, blue_noise);

			// swap back.
			if (new_loss > loss) {
				Float tmp = blue_noise[i];
				blue_noise[i] = blue_noise[j];
				blue_noise[j] = tmp;
			}
			else {
				loss = new_loss;
			}

			if(iter % 1000 == 0)
				printf("loss: %0.4f, iter:%d\n", loss, iter);

				// save current results
				ImageRGB img(x_resolution, y_resolution);
				for (int x = 0; x < x_resolution; ++x) {
					for (int y = 0; y < y_resolution; ++y) {
						Float v = blue_noise[y * x_resolution + x];
						Color color(v, v, v);
						img.Set(x, y, color);
					}
				}
				img.save("blue_noise.bmp", 1.0f);
		}
}

int main() {
	generate_blue_noise(512, 512);
	
	return 0;
}
	