function [estSpeech, estPowN] = denoiseAlgonquin(noisySignal, nwk)

    P = defaultParams;
    
    FRAME_LEN = 256; 
    % shifting window length
    OVERLAP_LEN = 128;
    
    % the size of DFT
    FFT_SIZE = 256;
    % analysis window TODO: what does periodic mean?
    WINDOW  = hanning(FRAME_LEN);  
    % the number of frequency bins FFT_SIZE/2+1
    NUM_BINS = FFT_SIZE/2+1;
    noisyLog = logPowerDomain(noisySignal(:), P);

    NUM_FRAMES = length(noisyLog);

    estPowS = zeros(P.NUM_BINS, NUM_FRAMES);
    estPowN = zeros(P.NUM_BINS, NUM_FRAMES);

    for f = 1:NUM_FRAMES
        idx = (f-1) * OVERLAP_LEN + 1 : (f-1) * OVERLAP_LEN + FRAME_LEN;

        frame = noisyLog(:, f);
        [logS, logN] = arr.process(frame);

        estPowS(:,f) = exp(logS);
        estPowN(:,f) = exp(logN);
    end


end

