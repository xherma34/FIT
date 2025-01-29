import numpy as np
import matplotlib.pyplot as plt
import soundfile as sf
import math
import IPython
from statistics import mean
from scipy.signal import spectrogram, lfilter, freqz, tf2zpk

s, fs = sf.read('xherma34.wav')
t = np.arange(s.size) / fs

#TASK 1:

plt.figure(figsize=(6, 3))
plt.plot(t, s)

plt.gca().set_ylabel('$Amplituda$')
plt.gca().set_xlabel('$t[s]$')
plt.gca().set_title('TASK 1 - Input signal')
plt.tight_layout()
#plt.savefig("task1.png", format="png")

print("Delka v sekundach: ", len(s)/fs)
print("Delka ve vzorcich: ", len(s))
print("Minimum: ", min(s))
print("Maximum: ", max(s))

#TASK 2:
# s-meanVal => signal/max(abs(max,min))

meanVal = mean(s)
s = s - meanVal
s = s / max(abs(max(s)), abs(min(s)))

#Vyber pekneho ramce

myFrame = []

i = 0
while 1:
    validFrame = s[i : i+1024]
    myFrame.append(validFrame)
    i = i + 512
    if i + 1024 > len(s):
        break

plotFrame = np.arange(1024) / fs
plt.plot(plotFrame, myFrame[27])
plt.gca().set_ylabel('$Amplituda$')
plt.gca().set_xlabel('$t[s]$')
plt.gca().set_title('TASK 2 - Valid Frame')
#plt.savefig("task2.png", format="png")

#TASK 3:
#DFT = SUM(xn * e^(-j2pi/n)*kn) || SUM(xn*[cos(2pikn/N)] - 1j*sin(2pikn/N))

def DFT(frame):
    N = len(frame)-1
    dftOut = []
    for k in range(N):
        x = 0
        for n in range(N):
            coef = math.cos(2 * np.pi * n * k / N) - 1j * math.sin(2 * np.pi * n * k / N)
            x = x + frame[n] * coef
        dftOut.append(x)
    return dftOut


dftS = DFT(myFrame[27])
dftS = dftS[:512] #oriznuti zaporne strany

# dftS = DFT(myFrame)
# dftS = dftS[:512]

plotFrame = np.arange(0, 8000, 8000/512)
plt.plot(plotFrame, np.abs(dftS))
plt.gca().set_ylabel('$Y$')
plt.gca().set_xlabel('$t[Hz]$')
plt.gca().set_title('TASK 3 - DFT')
#plt.savefig("task3.png", format="png")


#TASK 4:
#Spektogram

f, t, sgr = spectrogram(s, fs)
sgr_log = 10 * np.log10(sgr+1e-20)

plt.figure(figsize=(9,3))
plt.pcolormesh(t,f,sgr_log)
plt.gca().set_xlabel('Čas [s]')
plt.gca().set_ylabel('Frekvence [Hz]')
cbar = plt.colorbar()
cbar.set_label('Spektralní hustota výkonu [dB]', rotation=270, labelpad=15)

plt.tight_layout()
#plt.savefig("task4.png", format="png")

#650, 1300, 1950, 2600

#TASK 5:
#Rusive frekvence

f1, f2, f3, f4 = 650, 1300, 1950, 2600
print("Rusive frekvence jsou: ")
print("f1 :", f1)
print("f2 :", f2)
print("f3 :", f3)
print("f4 :", f4)
N = len(s)
#TASK 6:
#Generovani signalu
af1, af2, af3, af4 = [], [], [], []
af1 = np.linspace(0, 2 * np.pi * f1 * (len(s)/fs), N)
af2 = np.linspace(0, 2 * np.pi * f2 * (len(s)/fs), N)
af3 = np.linspace(0, 2 * np.pi * f3 * (len(s)/fs), N)
af4 = np.linspace(0, 2 * np.pi * f4 * (len(s)/fs), N)
cosaf1, cosaf2, cosaf3, cosaf4, cosgen = [], [], [], [], []
i = 0
while i < N:
    cosaf1.append(math.cos(af1[i]))
    cosaf2.append(math.cos(af2[i]))
    cosaf3.append(math.cos(af3[i]))
    cosaf4.append(math.cos(af4[i]))
    cosgen.append((cosaf1[i] + cosaf2[i] + cosaf3[i] + cosaf4[i]) * 0.15)
    i = i + 1


sf.write('../audio/cosgen.wav', cosgen, fs)

cosS, cosFs = sf.read('../audio/cosgen.wav')

f, t, sgr = spectrogram(cosS, cosFs)
sgr_log = 10 * np.log10(sgr+1e-20)

plt.figure(figsize=(9,3))
plt.pcolormesh(t,f,sgr_log)
plt.gca().set_xlabel('Čas [s]')
plt.gca().set_ylabel('Frekvence [Hz]')
cbar = plt.colorbar()
cbar.set_label('Spektralní hustota výkonu [dB]', rotation=270, labelpad=15)

plt.tight_layout()
#plt.savefig("task6.png", format="png")


#TASK 7:
#Filtr
a = [1, 0, 0, 0, 0, 0, 0, 0]

#wk = 2pi * fk / fs
w1 = 2 * np.pi * (f1 / fs)
w2 = 2 * np.pi * (f2 / fs)
w3 = 2 * np.pi * (f3 / fs)
w4 = 2 * np.pi * (f4 / fs)

#xi = e^jwk
x1 = np.exp(1j * w1)
x2 = np.exp(1j * w2 * 2)
x3 = np.exp(1j * w3 * 3)
x4 = np.exp(1j * w4 * 4)

x5 = np.conj(x1)
x6 = np.conj(x2)
x7 = np.conj(x3)
x8 = np.conj(x4)

filterCoefs = (x1, x2, x3, x4, x5, x6, x7, x8)
b = np.poly(filterCoefs)

N_imp = 32
imp = [1, *np.zeros(N_imp-1)]
h = lfilter(b, a, imp)

w, H = freqz(b, a)

z, p, k = tf2zpk(b, a)

is_stable = (p.size == 0) or np.all(np.abs(p) < 1)

sf = lfilter(b, a, s)
f, t, sfgr = spectrogram(sf, fs)
sfgr_log = 10 * np.log10(sfgr+1e-20)

plt.figure(figsize=(5,3))
plt.stem(np.arange(N_imp), h, basefmt=' ')
plt.gca().set_xlabel('$n$')
plt.gca().set_title('Impulsní odezva $h[n]$')

plt.grid(alpha=0.5, linestyle='--')
#plt.savefig("task7.png", format="png")
plt.tight_layout()

#TASK 8

p = np.roots(a)
z = np.roots(b)

plt.figure(figsize=(4,3.5))

# jednotkova kruznice
ang = np.linspace(0, 2*np.pi,100)
plt.plot(np.cos(ang), np.sin(ang))

# nuly, poly
plt.scatter(np.real(z), np.imag(z), marker='o', facecolors='none', edgecolors='r', label='nuly')
plt.scatter(np.real(p), np.imag(p), marker='x', color='g', label='póly')

plt.gca().set_xlabel('Realná složka $\mathbb{R}\{$z$\}$')
plt.gca().set_ylabel('Imaginarní složka $\mathbb{I}\{$z$\}$')

plt.grid(alpha=0.5, linestyle='--')
plt.legend(loc='upper right')

plt.tight_layout()
#plt.savefig("task8.png", format="png")
