function result = normalizeWaveSignal(signal)
%NORMALIZESIGNAL renormalize wave signal to be in [-1, 1]
    result = 2*(signal - min(signal))./ (max(signal) - min(signal)) - 1;
end

