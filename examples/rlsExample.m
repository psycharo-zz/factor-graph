function rlsExample
%% Example of autoregressive parameter estimation

%% Creating the network
% The network is more or less the same as in kalmanFilterExample.m,
% with additional nodes used for the A matrix estimation

    
nwk = ffg.Network;

x_prev = ffg.EvidenceNode(nwk);          
x_next = ffg.EvidenceNode(nwk); 
y = ffg.EvidenceNode(nwk);      
noise_U = ffg.EvidenceNode(nwk);
noise_W = ffg.EvidenceNode(nwk);
b = ffg.MultiplicationNode(nwk);
A = ffg.EstimateMultiplicationNode(nwk);
add_A_b = ffg.AddNode(nwk);   
equMult = ffg.EquMultNode(nwk);
a_equ = ffg.EqualityNode(nwk);
add_W_c = ffg.AddNode(nwk);
% in this case we need precision matrices (to avoid infinities)
a_equ.setType('PRECISION');
a_prev = ffg.EvidenceNode(nwk);
a_next = ffg.EvidenceNode(nwk);

% the real A coefficients
REAL_A = [1.51, -1.08, 0.47, -0.23, 0.91, -1.30, 0.86, -0.32];
M = length(REAL_A);

b.setMatrix(eye(M, 1));
equMult.setMatrix(eye(M, 1)');


% connecting the nodes
nwk.addEdge(x_prev, A);
nwk.addEdge(A, add_A_b);
nwk.addEdge(noise_U, b);
nwk.addEdge(b, add_A_b);
nwk.addEdge(add_A_b, equMult);
nwk.addEdge(x_next, equMult);

nwk.addEdge(equMult, add_W_c);
nwk.addEdge(add_W_c, y);
nwk.addEdge(noise_W, add_W_c);

% note a special kind of connection for A
nwk.addEdge(A, a_equ, 'estimated', '');
nwk.addEdge(a_prev, a_equ);
nwk.addEdge(a_equ, a_next);

%% Constructing the message-passing schedule schedule
nwk.setSchedule({a_next, a_equ;  ...
            a_prev, a_equ;  ...
            a_equ, A; ...
            x_prev, A; ...
            A, add_A_b; ...
            noise_U, b; ...
            b, add_A_b; ...
            add_A_b, equMult; ...
            y, add_W_c; ...
            noise_W, add_W_c; ...
            add_W_c, equMult; ...
            equMult, x_next; ...
            x_next, equMult; ...
            equMult, add_A_b; ...
            add_A_b, A; ...
            A, a_equ; ...
            a_prev, a_equ; ...
            a_equ, a_next});


%% Setting the initial parameters of the model

% noise variances (not estimated in this example)
REAL_varU = 0.1; 
REAL_varW = 0.001;


% dummy messages for A
a_prev.receive(ffg.messages.gaussPrecision(eye(1,M), eye(M, M)));
a_next.receive(ffg.messages.gaussPrecision(zeros(1,M), zeros(M, M)));

% number of observations 
N_OBSERVATIONS = 10000;

% matrix for the observed noise W
REAL_x = eye(1,M);

% dummy message to start with
msg = ffg.messages.gaussVariance(REAL_x, eye(M, M) * REAL_varU);

% initial message for noise U (of unobserved x)
noise_U.receive(ffg.messages.gaussVariance(0, REAL_varU));
% intitial message for the observation noise
noise_W.receive(ffg.messages.gaussVariance(0, REAL_varW));

%% Iterating through the observations
for i = 1:N_OBSERVATIONS
    % constructing the real observation matrix
    x_new = REAL_A * REAL_x' + randn() * REAL_varU;
    REAL_x = [x_new, REAL_x(1:end-1)];
    x_prev.receive(msg);
    % receiving the noisy observation
    y.receive(ffg.messages.gaussVariance(x_new + randn() * REAL_varW, 0));
    nwk.makeStep();         
    msg = x_next.evidence();
    a_prev.receive(a_next.evidence());
end

% getting the estimated A
EST_A = a_next.evidence().mean;

err = 0;
for i = 1:length(REAL_A)
 err = err + abs((EST_A(i) - REAL_A(i)) / EST_A(i));
end
err = err / length(REAL_A);

disp(sprintf('Relative error:%f', err));



end