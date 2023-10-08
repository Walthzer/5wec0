import wave, struct, soundfile, numpy

# Using readlines()
file1 = open('output.txt', 'r')
Lines = file1.readlines()

data = numpy.loadtxt("output.txt")
soundfile.write('my_24bit_file.wav', data, 44100, 'PCM_24')

