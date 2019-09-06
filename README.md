# Blue Noise

by [Duan Gao](https://gao-duan.github.io/)

Implement the algorithm introduced in [1].

The blue noise image can be used in Path Tracing to distribute the Monte Carlo noise from white noise to blue noise (I have tested this idea in my own physically based renderer [Elegans](https://github.com/gao-duan/Elegans)) , more details are described in [2]. 

## Dependencies

- [CImg](https://github.com/dtschump/CImg)
- OpenMP support
- C++ 11

## Results

1D blue-noise:

| Resolution | White noise                       | White noise FFT [3]           | Blue noise | Blue noise FFT |
| ---------- | --------------------------------- | -------------------------------------- | ---------- | -------------- |
| 32x32      | ![](./results/white_noise_32.bmp) | ![](./results/white_noise_32_freq.bmp) | ![](./results/blue_noise_32.bmp)              |       ![](./results/blue_noise_32_freq.bmp)      |
| 128x128    | ![](./results/white_noise_128.bmp) | ![](./results/white_noise_128_freq.bmp)      |     ![](./results/blue_noise_128.bmp)       |        ![](./results/blue_noise_128_freq.bmp)        |

For higher resolution, we can generate it by tiling low-resolution result.

For example, use 16 128x128 blue noise generate 512x512:

> ![](./results/blue_noise_128.bmp) (128x128)
>
> ![](./results/blue_noise_512.bmp) (512x512)



[1] Georgiev I, Fajardo M. Blue-noise dithered sampling[C]//ACM SIGGRAPH 2016 Talks. ACM, 2016: 35.

https://www.arnoldrenderer.com/research/dither_abstract.pdf

[2] Heitz E, Belcour L. Distributing Monte Carlo Errors as a Blue Noise in Screen Space by Permuting Pixel Seeds Between Frames[C]//Computer Graphics Forum. 2019, 38(4): 149-158.

https://hal.archives-ouvertes.fr/hal-02158423/document

[3] Use ./results/vis_image_freq.py to generate the frequency visualization image. Please see [OpenCV](https://docs.opencv.org/3.0-beta/doc/py_tutorials/py_imgproc/py_transforms/py_fourier_transform/py_fourier_transform.html) for more details.
