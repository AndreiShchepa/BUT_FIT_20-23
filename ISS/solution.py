import librosa
import librosa.display
import scipy as sp
import numpy as np
import matplotlib.pyplot as plt
import soundfile
import math
import scipy.io.wavfile as wavf
#!wget https://www.fit.vutbr.cz/study/courses/ISS/public/proj2021-22/signals/xshcha00.wav

##### Definition of all functions #####
def plot_signal(signal, step, end, xlabel, ylabel, file_name):
    plt.figure(figsize=(20,5))
    plot_a = plt.subplot(111)
    plot_a.set_xlabel(xlabel)
    plot_a.set_ylabel(ylabel)
    plot_a.plot(np.arange(0, end, step), signal)
    plt.savefig(file_name)
    plt.show()

def plot_specgram(signal, Fs, file_name):
    f, t, sgr = sp.signal.spectrogram(signal, Fs)
    sgr_log = 10 * np.log10(sgr + 1e-20)
    plt.figure(figsize=(13, 5))
    plt.pcolormesh(t, f, sgr_log)
    plt.gca().set_xlabel('Time [s]')
    plt.gca().set_ylabel('Frequency [Hz]')
    cbar = plt.colorbar()
    cbar.set_label('Spectral power density [dB]', rotation = 270, labelpad = 15)
    plt.savefig(file_name)
    plt.show()

def dft(x):
    N = len(x)
    n = np.arange(N)
    k = n.reshape((N, 1))
    print()
    e = np.exp(-2j * np.pi * k * n / N) 
    X = np.dot(e, x)
    return X

def find_freq_1_cos(signal, from_fs, to_fs):
    N = 16000
    freq_step = N/1024.0
    n = np.arange(0, N)
    k = np.linspace(from_fs, to_fs, to_fs - from_fs)
    k = np.reshape(k, (len(k), 1))
    e = np.exp(-2j * np.pi * k * n / N) 
    X = np.dot(e, signal)
    X_abs = np.abs(X)
    return from_fs + np.argmax(np.abs(X_abs))

def plot_p_n(z, p):
    ang = np.linspace(0, 2*np.pi,100)

    for j in range(2):
        _, ax = plt.subplots(1, 2, figsize=(14,6))
        for i in range(2):
            ax[i].plot(np.cos(ang), np.sin(ang))
            ax[i].scatter(np.real(z[i+2*j]), np.imag(z[i+2*j]), s=250, marker='o', facecolors='none', edgecolors='r', label='nuly')
            ax[i].scatter(np.real(p[i+2*j]), np.imag(p[i+2*j]), s=80, marker='x', color='black', label='póly')
            ax[i].set_xlabel('Real part $\mathbb{R}\{$z$\}$')
            ax[i].set_ylabel('Imaginary part $\mathbb{I}\{$z$\}$')
            ax[i].set_title('Filter %i' %(i+1+2*j))
            ax[i].grid(alpha=0.5, linestyle='--')
            ax[i].legend(loc='upper left')
            ax[i].set_rasterized(True)

        plt.savefig('np' + '%i' %(j+1) + '.pdf')

    plt.show()

def plot_freq_res(b, a, id):
    w, H = sp.signal.freqz(b, a)

    L, ax = plt.subplots(1, 2, figsize=(15,4))

    ax[0].plot(w / 2 / np.pi * sr, np.abs(H))
    ax[0].set_xlabel('Frequency [Hz]')
    ax[0].set_title('Absolute value $|H(e^{j\omega})|$')
    ax[0].set_rasterized(True)

    ax[1].plot(w / 2 / np.pi * sr, np.angle(H))
    ax[1].set_xlabel('Frequency [Hz]')
    ax[1].set_title('Absolute value $\mathrm{arg}\ H(e^{j\omega})$')
    ax[1].set_rasterized(True)

    for ax1 in ax:
        ax1.grid(alpha=0.5, linestyle='--')

    L.suptitle('Filter %i' %(id+1))
    plt.savefig('freq' + '%i' %(id+1) + '.pdf')
    plt.show()

def impl_resp(b, a):
    N_imp = 32
    imp = [1, *np.zeros(N_imp-1)]

    for j in range(2):
        _, ax = plt.subplots(1, 2, figsize=(20,5))
        for i in range(2):
            h = sp.signal.lfilter(b[i+2*j], a[i+2*j], imp)
            ax[i].stem(np.arange(N_imp), h, use_line_collection=True, basefmt=' ')
            ax[i].set_xlabel('$n$')
            ax[i].set_title('Filter %i' %(i+1+2*j))
            ax[i].grid(alpha=0.5, linestyle='--')
            ax[i].set_rasterized(True)

        plt.savefig('impl' + '%i' %(j+1) + '.pdf')

    plt.show()

def normal_and_convert_16b(sig):
    if np.max(sig) > 1.0 or np.min(sig) < -1.0:
        sig = sig / (np.maximum(np.abs(np.max(sig)), np.abs(np.min(sig))))

    return (sig * np.iinfo(np.int16).max).astype(np.int16)
#######################################

##### The first task #####
arr, sr = soundfile.read('xshcha00.wav')

print('length in samples: ', len(arr))
print('length in seconds: ', len(arr)/sr, 's')
min_val = np.min(arr)
max_val = np.max(arr)
print('Min value: ', min_val)
print('Max value: ', max_val)

plot_signal(arr, 
            1/sr, 
            len(arr)/sr, 
            'Time [s]', 'Amplitude',
            'signal1.pdf')
##########################

##### The second task #####
normal_arr = (arr - np.mean(arr)) / (np.maximum(np.abs(min_val), np.abs(max_val)))
length = 1024
size_overlapping = 512
pad_size = len(normal_arr)/length

import numpy.lib.stride_tricks as stride

# prepare 2D array for frames 
normal_arr = np.pad(normal_arr, (0, length * math.ceil(pad_size)), constant_values = 0)
# create len(arr)/512 frames of 1024 length
frames_2D = stride.as_strided(normal_arr, 
                             (math.ceil(pad_size*2), length), 
                             (size_overlapping * normal_arr.itemsize, normal_arr.itemsize))

window = frames_2D[40] # magic index 40, i think, it is nice
plot_signal(window, 
            1/sr, 
            length/sr, 
            'Time [s]', 'Amplitude',
            'signal2.pdf')
###########################

##### The third task #####
# FFT of the 40th frame
np_fft = np.fft.fft(window)
absolute_fft = np.absolute(np_fft[:length])
plot_signal(absolute_fft[0:513], 
            sr * (1.0 / length), 
            sr * (0.5 + 1.0/length), 
            'Frequency [Hz]', 'Magnitude',
            'signal3.pdf')

# DFT of the 40th frame
np_dft = dft(window)
absolute_dft = np.absolute(np_dft[:length])
plot_signal(absolute_dft[0:513], 
            (1.0 / length) * sr, 
            sr * (0.5 + 1.0/length), 
            'Frequency [Hz]', 'Magnitude',
            'signal4.pdf')

if np.allclose(np_fft, np_dft):
    print('Result of own implementation of DFT is the same with result of numpy function')
##########################

##### The fourth task #####
plot_specgram(arr, sr, 'signal5.pdf')
###########################

##### The fifth task #####
np_fft_noise = np.fft.fft(frames_2D[0])
noise = np.absolute(np_fft_noise[:length])

# frequency of the 1st cosine
first_f = np.argmax(noise[:128])
freq = 16000.0/1024
# try to find precise frequency, because
# FFT is not precise (16000/1024=15.625) 
prec_freq = find_freq_1_cos(arr[0:16000], 
                            np.ceil((first_f-1) * freq).astype(np.int64), 
                            np.ceil((first_f+1) * freq).astype(np.int64))

cos4 = np.zeros(1024).astype(int)
# 4 x COS
for i in range(1,5,1):
    cos4[int(prec_freq/freq*i)] = 20

# real 
plot_signal(noise[0:513], 
            sr * (1.0/length), 
            sr * (0.5 + 1.0/length), 
            'Frequency [Hz]', 'Magnitude',
            'signal6.pdf')

# expected
plot_signal(cos4[0:513], 
            sr * (1.0/length), 
            sr * (0.5 + 1.0/length), 
            'Frequency [Hz]', 'Magnitude',
            'signal7.pdf')
##########################

##### The sixth task #####
# the first 1000 Hz 
f = prec_freq*2*np.pi
t = np.arange(start=0,stop=len(arr)/sr,step=1/sr)
sig_cos = np.zeros(len(arr))

for i in range(1,5,1):
    sig_cos += np.cos(f*i*t)

plt.ylabel('Frequency [Hz]')
plt.xlabel('Time [s]')
plt.specgram(sig_cos, NFFT=length, noverlap=length/2, Fs=sr)
plt.savefig('signal8.pdf')
sig_cos = normal_and_convert_16b(sig_cos)
wavf.write('4cos.wav', sr, sig_cos)
##########################

##### The seventh task #####
nyq = sr*0.5
arr_a = np.ndarray((4, 5), dtype=np.float64)
arr_b = np.ndarray((4, 5), dtype=np.float64)

for i in range(4):
    new_fr = prec_freq*(i+1)
    # stopband  
    ws = np.array([new_fr-15, new_fr+15])/nyq
    # passband
    wp = np.array([new_fr-25, new_fr+25])/nyq
    # 3  - loss in the passband
    # 40 - attenuation in the stopband
    N, wn = sp.signal.buttord(wp, ws, 3.0, 40.0)
    arr_b[i], arr_a[i] = sp.signal.butter(2, 
                                          wn, 
                                          btype='bandstop', 
                                          output='ba')

impl_resp(arr_b, arr_a)
############################

##### The eighth task #####
z = np.ndarray((4, 4), dtype=np.complex128)
p = np.ndarray((4, 4), dtype=np.complex128)

for i in range(4):
    z[i], p[i], k = sp.signal.tf2zpk(arr_b[i], arr_a[i])

plot_p_n(z, p)
###########################

##### The ninth task #####
for i in range(4):
    plot_freq_res(arr_b[i], arr_a[i], i)
##########################

##### The tenth task #####
clean_arr = arr

# filtering
for i in range(4):
    clean_arr = sp.signal.lfilter(arr_b[i], arr_a[i], clean_arr)

clean_arr = normal_and_convert_16b(clean_arr)
plot_specgram(clean_arr, sr, 'filtered.pdf')
plot_signal(clean_arr, 
            1/sr, 
            len(clean_arr)/sr, 
            'Time [s]', 'Amplitude',
            'filtered_plot.pdf')

wavf.write('clean_bandstop.wav', sr, clean_arr)
##########################

