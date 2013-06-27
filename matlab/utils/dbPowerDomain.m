function dbPow = dbPowerDomain(signal, PARAMS)
    fftCoefs = spectrogram(signal(:), PARAMS.WINDOW, PARAMS.OVERLAP_LEN, PARAMS.FFT_SIZE);
    dbPow = 10*log10(abs(fftCoefs).^2);
end

