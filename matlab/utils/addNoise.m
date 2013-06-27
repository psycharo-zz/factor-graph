function [noisy, noise] = addNoise(srcSignal, srcNoise, snr)
%ADDNOISE add noise to the signal at the given snr level
    gain = norm(srcSignal) / (norm(srcNoise) * 10^(0.05 * snr));
    noise = gain .* srcNoise;
    noisy = srcSignal + noise;
end

