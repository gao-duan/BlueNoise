import numpy as np
import cv2
from matplotlib import pyplot as plt
import sys

img = cv2.imread(sys.argv[1],0)

img_float32 = np.float32(img)

dft = cv2.dft(img_float32, flags = cv2.DFT_COMPLEX_OUTPUT)
dft_shift = np.fft.fftshift(dft)

magnitude_spectrum =  np.log(cv2.magnitude(dft_shift[:,:,0],dft_shift[:,:,1]) + 1e-20)
print(np.min(magnitude_spectrum))
print(np.max(magnitude_spectrum))
magnitude_spectrum = (magnitude_spectrum - np.min(magnitude_spectrum)) / (np.max(magnitude_spectrum) - np.min(magnitude_spectrum))
cv2.imwrite(sys.argv[2], magnitude_spectrum ** (1.2) * 255)