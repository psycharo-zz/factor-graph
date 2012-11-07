function arExample()
%AREXAMPLE example of autoregressive parameter estimation
    x_prev = EvidenceNode;
    x_next = EvidenceNode;
    y = EvidenceNode;
    noise_U = EvidenceNode;
    b = MultiplicationNode;

    A = MultiplicationNode;
    add_A_b = AddNode;
    equ = EqualityNode;
    c = MultiplicationNode;
    noise_W = EvidenceNode;
    add_c_W = AddNode;

    nwk = Network;

    % 1
    nwk.addEdge(x_prev, A);
    % 2
    nwk.addEdge(A, add_A_b);

    % 3
    nwk.addEdge(noise_U, b);
    % 4
    nwk.addEdge(b, add_A_b);

    nwk.addEdge(add_A_b, equ);
    nwk.addEdge(equ, x_next);
    nwk.addEdge(equ, c);


    nwk.addEdge(c, add_c_W);
    nwk.addEdge(noise_W, add_c_W);
    nwk.addEdge(add_c_W, y);
   
% 
% We then use iterative message updating,
% with several rounds of computing, first, the messages 

% left to right:
% 1, 2, 3, 4, 5, 14, 15, 16, 17, 6, 10, 11, 12, 13, 7, 8, 9 
% right to left:
% 4, 6, 7, 8 
   
    %schedule = {x_prev, A, A, add_A_b, add_A_b, equ, add_c_W, c, equ, x_next, equ, add_A_b, add_A_b, A };
    
    
    % this is schedule for the standard kalman filter
    schedule = {};
    schedule = [schedule, {x_prev, A}, {A, add_A_b}, {noise_U, b}, {b, add_A_b}];
    schedule = [schedule, {add_A_b, equ}, {noise_W, add_c_W}, {y, add_c_W}, {add_c_W, c}, {c, equ}];
    schedule = [schedule, {equ, x_next}];
    
    nwk.setSchedule(schedule);
    
    

    % the parameters of the model
    % noise variances (not estimated in this example)
    REAL_varU = 0.1; 
    REAL_varW = 0.01; % 0.001
    % the real coefficients
    REAL_A = [1.51, -1.08, 0.47, -0.23, 0.91, -1.30, 0.86, -0.32];
    
    
    
    % setting initial messages,etc
    % dimensionality
    M = length(REAL_A);

    matrix_b = eye(M,1);
    matrix_c = matrix_b; 
   
    % number of observations
    N_OBSERVATIONS = 10^4;
 
    % matrix for multiplication node A
    % TODO: zeros or randn?
    %param_a = randn(1, M);
    param_a = REAL_A;
    matrix_A = vertcat(param_a, horzcat(eye(M-1,M-1), zeros(M-1,1)));
    A.setMatrix(matrix_A);
      
    % matrix for the unobserved noise U
    b.setMatrix(matrix_b);
    
    % matrix for the observed noise W
    % NOTE: it is transposed
    c.setMatrix(matrix_c');
    
    % observing a linearly growing function (i with normal noise with var = noise_W)
    result = zeros(N_OBSERVATIONS, 2);
    
    % initialization of x (TODO: all zeros / partial computation)
    REAL_x = M:-1:1;
    
    % dummy message to start with
    msg = gaussMessage(randn(), REAL_varU);
    
    % for x_{n+1}
    x_next.receive(msg);
    % initial message for noise U (of unobserved x)
    noise_U.receive(gaussMessage(0, REAL_varU));
    % initial message for noise W (of observed y)  
    noise_W.receive(gaussMessage(0, REAL_varW));
    
        
    for i = M+1:N_OBSERVATIONS
        % x_n = a_1*x_{n-1} + ... + a_M*x_{n-M} + U_n
        x_new = REAL_A * REAL_x' + randn() * REAL_varU;
        % X_n = [x_n + ... + x_{n-M+1}]
        REAL_x = [x_new, REAL_x(1:end-1)];
        % y_n = x_n + W_n
        observation = x_new + randn() * REAL_varW;
       
        % data from the previous observation
        x_prev.receive(msg);
        
        % observing the data
        y.receive(gaussMessage(observation, 0));

        % doing the inference
        nwk.makeStep();         
         
        % 
        msg = x_next.evidence();
%         result(i,:) = [msg.mean, msg.var];
    end
     
    msg
    REAL_x
    
    


end

