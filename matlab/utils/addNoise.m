function result = addNoise(signal, noise, snr)
%ADDNOISE add noise to the signal at the given snr level
    gain = norm(signal) / (norm(noise) * 10^(0.05 * snr));
    result = signal + gain .* noise;
end

