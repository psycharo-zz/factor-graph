function [state, hmm_out]=sim_hmm(pre_state,cptt,cpte)

% look up cptt to get next state
v = cumsum(cptt(pre_state,:));
rnd = rand();
m = find(v > rnd);
state = m(1);
% look up cpte to get hmm_out
v = cumsum(cpte(state,:));
rnd = rand();
m = find(v > rnd);
hmm_out = zeros(1,size(cpte,2));
hmm_out(m(1)) = 1;

