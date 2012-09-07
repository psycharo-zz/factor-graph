% Hidden Markov Chain Example
% This example is found in 
% http://en.wikipedia.org/wiki/Hidden_Markov_model

% definition of symbols
% xn - the hidden variable of weather on Bob side
% xn(1) - rain on the nth day
% xn(2) - sunny on the nth day
% yn - the observed variable
% yn(1) - Bob walks on the nth day
% yn(2) - Bob shops on the nth day
% yn(3) - Bob cleans on the nth day

clear all
clc
disp('Hidden Markov Chain Example from:')
disp('http://en.wikipedia.org/wiki/Hidden_Markov_model')
disp(' ')
disp('Using Sum-Product Algorithm to guess the hidden state x')
disp('by the observe output y.')
disp(' ')
disp('x0-t1-x1-t2-x2....tn-xn--xn+1')
disp('      |     |        |')
disp('      e1    e2.....  en')
disp('      |     |        |')
disp('      y1    y2.....  yn')
disp(' ')
% cpt for state transistion
%   xn,xn+1   P(xn+1|xn)
cptt(1,1)   = 0.7;       % rain, rain
cptt(1,2)   = 0.3;       % rain, sunny
cptt(2,1)   = 0.4;       % sunny, rain
cptt(2,2)   = 0.6;       % sunny, sunny
% cpt for observable
%   xn,yn   P(yn|xn)
cpte(1,1) = 0.1;        % rain, walk
cpte(1,2) = 0.4;        % rain, shop
cpte(1,3) = 0.5;        % rain, clean
cpte(2,1) = 0.6;        % sunny, walk
cpte(2,2) = 0.3;        % sunny, shop
cpte(2,3) = 0.1;        % sunny, clean
% simulate Hidden Markov Model
rand('twister',sum(100*clock));
num_iterations = 20;
hmm_state = zeros(num_iterations,1);
hmm_output = zeros(num_iterations,3);
% the prior probability of rain is about 0.62 from simulation
if rand() < 0.62
    init_state = 1;
else
    init_state = 2;
end
for i = 1:num_iterations
    if i == 1
        [hmm_state(i), hmm_output(i,:)] = sim_hmm(init_state,cptt,cpte);
    else
        [hmm_state(i), hmm_output(i,:)] = sim_hmm(hmm_state(i-1),cptt,cpte);
    end
end
% instantiate initial state
id = 1;
s0 = evident_node(id);
id = id + 1;
t = {};
x = {};
e = {};
y = {};
last_node = s0;
% instantiate HMM sections
for i = 1:num_iterations
    % instantiate nodes within a section
    t{i} = cp2_node(id); id = id + 1;
    x{i} = equ_node(id); id = id + 1;
    e{i} = cp2_node(id); id = id + 1;
    y{i} = evident_node(id); id = id + 1;
    % setup connectivity
    connect(last_node,t{i});
    connect(t{i},x{i});
    connect(x{i},e{i});
    connect(e{i},y{i});
    last_node = x{i};
    % setup cpt
    t{i}.init_cp(cptt);
    e{i}.init_cp(cpte);
    % error check
%     t{i}.error_check();
%     x{i}.error_check();
%     e{i}.error_check();
%     y{i}.error_check();
end
% instantiate end state
s1 = evident_node(id);
connect(last_node,s1);
% s0.error_check();
% s1.error_check();
% setup evidents
init_state = [0.62 0.38];
s0.setup_init_msg(init_state);
for i = 1:num_iterations
    y{i}.setup_init_msg(hmm_output(i,:));
end
s1.setup_init_msg([0.5 0.5]);
% find marginals of x
margins = [];
infer_state = [];
for i = 1:num_iterations
    cur_marginal = marginal(t{i},x{i});
    margins = [margins; cur_marginal];
    infer_state = [infer_state; find(cur_marginal==max(cur_marginal))];
end
% margins
% hmm_output
disp('       activities     weather')
disp('    walk shop clean actual predicted')
disp([hmm_output hmm_state infer_state])
accuracy = sum(hmm_state == infer_state)/num_iterations;
s = sprintf('Accuracy = %f',accuracy);
disp(s);