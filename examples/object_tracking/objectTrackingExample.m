%% Applying Kalman Filter to object tracking

% reloading library
clear mexfactorgraph;
load('position.mat');


%% Specifying Kalman Filter network
nwk = ffg.Network;
dt = 1;
% X_next = A * X_prev + U
% Y_next = H * X_next + W

% hidden variable, X_{t-1}
X_prev = ffg.EvidenceNode(nwk);
% transition matrix node
A = ffg.MultiplicationNode(nwk);
A.setMatrix([1 0 dt 0 0 0;...     % [x  ]            
     0 1 0 dt 0 0;...     % [y  ]
     0 0 1 0 dt 0;...     % [Vx]
     0 0 0 1 0 dt;...     % [Vy]
     0 0 0 0 1 0 ;...     % [Ax]
     0 0 0 0 0 1 ]);      % [Ay]

% U + X_prev
add_X_U = ffg.AddNode(nwk);
% process noise
U = ffg.EvidenceNode(nwk);
% observation, corresponds to a half-edge y
Y = ffg.EvidenceNode(nwk);
% hidden variable, X_t
X_next = ffg.EvidenceNode(nwk);
equ = ffg.EqualityNode(nwk);

% observation noise, W_t
W = ffg.EvidenceNode(nwk);

% measurement matrix node
H = ffg.MultiplicationNode(nwk);
H.setMatrix([ 1 0 0 0 0 0; 
              0 1 0 0 0 0 ]);
% adding the noise
add_H_W = ffg.AddNode(nwk);

%% Connecting nodes
nwk.addEdge(X_prev, A);
nwk.addEdge(A, add_X_U);
nwk.addEdge(U, add_X_U);
 
nwk.addEdge(add_X_U, equ);
nwk.addEdge(equ, X_next);
nwk.addEdge(equ, H);

nwk.addEdge(H, add_H_W);
nwk.addEdge(W, add_H_W);
nwk.addEdge(add_H_W, Y);


%% Initialising network evidence
U.propagate(ffg.messages.gaussVariance(zeros(1,6), eye(6)));
W.propagate(ffg.messages.gaussVariance(zeros(1,2), 1./1000 * eye(2)));

NUM_SAMPLES = 300;
results = zeros(2, NUM_SAMPLES);

msg = ffg.messages.gaussVariance(zeros(1,6), 1./1000 * eye(6,6));

%% Drawing
figure;hold;grid;

for i = 1:NUM_SAMPLES
    X_prev.propagate(msg);
    observation = ffg.messages.gaussVariance(position(:,i)', zeros(2,2));
    Y.propagate(observation);
    msg = X_next.evidence();
    
    % the real observation
    z = position(:, i);
    % manually written kalman filter
    y_p = kalmanfilter(z);
    % FFG kalman filter
    y = [msg.mean(1); msg.mean(2)];
    
    % Plotting the results
    plot_trajectory(z,y,y_p);    
end

hold;




