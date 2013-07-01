
nwk = trainNetwork;

P = defaultParams;
[speech, freqS] = audioread('bbcnews.wav'); 
speechLog = logPowerDomain(speech, P)';
frame = speechLog(10,:);

nwk.process(frame)

