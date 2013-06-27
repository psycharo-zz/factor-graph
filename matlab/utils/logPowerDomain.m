function logPow = logPowerDomain(signal, PARAMS)
%LOGDOMAIN convert time-domain signal to log-power domain
    fftCoefs = spectrogram(signal(:), PARAMS.WINDOW, PARAMS.OVERLAP_LEN, PARAMS.FFT_SIZE);
    logPow = log(abs(fftCoefs).^2);
end

