function rlsExample()
%AREXAMPLE example of autoregressive parameter estimation

    % loading stuff
    load('test_ar.mat');
    
    NOISE_U = INPUT_AR.noiseU;

    % basic kalman-filter nodes
    x_prev = ffg.EvidenceNode;          
    x_next = ffg.EvidenceNode; 
    y = ffg.EvidenceNode;      
    noise_U = ffg.EvidenceNode;
    noise_W = ffg.EvidenceNode;
    b = ffg.MultiplicationNode;
    A = ffg.EstimateMultiplicationNode;
    add_A_b = ffg.AddNode;   
    equMult = ffg.EquMultNode;
    a_equ = ffg.EqualityNode;
    add_W_c = ffg.AddNode;
    %% TODO: use strings/enums instead of plain integers
    a_equ.setType('PRECISION');
    a_prev = ffg.EvidenceNode;
    a_next = ffg.EvidenceNode;
        
    nwk = ffg.Network;
    nwk.addEdge(x_prev, A);
    nwk.addEdge(A, add_A_b);
    nwk.addEdge(noise_U, b);
    nwk.addEdge(b, add_A_b);
    nwk.addEdge(add_A_b, equMult);
    nwk.addEdge(x_next, equMult);

% without any noise
%     nwk.addEdge(equMult, y);
    nwk.addEdge(equMult, add_W_c);
    nwk.addEdge(add_W_c, y);
    nwk.addEdge(noise_W, add_W_c);
    
    nwk.addEdgeTagged(A, a_equ, 'estimated', '');
    nwk.addEdge(a_prev, a_equ);
    nwk.addEdge(a_equ, a_next);
           
    % RLS schedule
    schedule = {};
      
    schedule = [schedule, {a_next, a_equ}, {a_prev, a_equ}, {a_equ, A}];
    schedule = [schedule, {x_prev, A}, {A, add_A_b}, {noise_U, b}, {b, add_A_b}, {add_A_b, equMult}];
%     schedule = [schedule, {y, equMult}];
    schedule = [schedule, {y, add_W_c}, {noise_W, add_W_c}, {add_W_c, equMult}];
    schedule = [schedule, {equMult, x_next}, {x_next, equMult}];
    schedule = [schedule, {equMult, add_A_b},  {add_A_b, A}];
    schedule = [schedule, {A, a_equ}, {a_prev, a_equ}, {a_equ, a_next}];

    nwk.setSchedule(schedule);
 
    % the parameters of the model
    % noise variances (not estimated in this example)
    REAL_varU = 0.1; 
    REAL_varW = 0.001;
    
     % the real coefficients
    REAL_A = [1.51, -1.08, 0.47, -0.23, 0.91, -1.30, 0.86, -0.32];
    M = length(REAL_A);

    a_prev.receive(ffg.gaussMessage(eye(1,M), eye(M, M), 'PRECISION'));
    a_next.receive(ffg.gaussMessage(zeros(1,M), zeros(M, M), 'PRECISION'));
    
    % number of observations 
    N_OBSERVATIONS = 1000;
 
    b.setMatrix(eye(M, 1));
    equMult.setMatrix(eye(M, 1)');
    
    % matrix for the observed noise W
    REAL_x = eye(1,M);

    % dummy message to start with
    msg = ffg.gaussMessage(REAL_x, eye(M, M) * REAL_varU, 'VARIANCE');
        
    % initial message for noise U (of unobserved x)
    noise_U.receive(ffg.gaussMessage(0, REAL_varU, 'VARIANCE'));
    % intitial message for the observation noise
    noise_W.receive(ffg.gaussMessage(0, REAL_varW, 'VARIANCE'));
     
     for i = 1:N_OBSERVATIONS
          x_new = REAL_A * REAL_x' + randn() * REAL_varU;
          REAL_x = [x_new, REAL_x(1:end-1)];
          x_prev.receive(msg);
          y.receive(ffg.gaussMessage(x_new + randn() * REAL_varW, 0, 'VARIANCE'));
          nwk.makeStep();         
          msg = x_next.evidence();
          a_prev.receive(a_next.evidence());
     end
     
     EST_A = a_next.evidence().mean;
     
     err = 0;
     for i = 1:length(REAL_A)
         err = err + abs((EST_A(i) - REAL_A(i)) / EST_A(i));
     end
     err = err / length(REAL_A);

     disp(sprintf('Relative error:%f', err));

end

