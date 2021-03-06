import numpy as np
import cv2
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('-i', '--input_file', type=str,  required=True)
parser.add_argument('-o', '--output_file', type=str,  required=True)

args,unknown = parser.parse_known_args()

if __name__ == '__main__':
    input_file = args.input_file
    output_file = args.output_file

    raw_img = cv2.imread(input_file)[...,::-1]
    
    channels = np.split(raw_img, raw_img.shape[2], axis = -1)

    res = []
    for img in channels:
        img_float32 = np.float32(img)
        dft = cv2.dft(img_float32, flags = cv2.DFT_COMPLEX_OUTPUT)
        dft_shift = np.fft.fftshift(dft)
        magnitude_spectrum =  np.log(cv2.magnitude(dft_shift[:,:,0],dft_shift[:,:,1]) + 1e-20)
        magnitude_spectrum = (magnitude_spectrum - np.min(magnitude_spectrum)) / (np.max(magnitude_spectrum) - np.min(magnitude_spectrum) + 1e-10)
        
        res.append(magnitude_spectrum[..., np.newaxis])
    
    res = np.concatenate(res, axis = -1)
    print(np.mean(res))
    cv2.imwrite(output_file, res[..., ::-1] ** (1.2) * 255)