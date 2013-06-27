function params = defaultParams
    params = struct();
    params.FREQUENCY = 16000;
    params.FRAME_LEN = 256; 
    params.OVERLAP_LEN = 128;
    params.FFT_SIZE = 256;
    params.WINDOW = hanning(params.FRAME_LEN);
    params.NUM_BINS = params.FFT_SIZE/2+1;
end

