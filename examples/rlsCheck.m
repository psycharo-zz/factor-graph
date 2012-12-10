function rlsCheck(  )

    load('test_ar.mat');
    
    NOISE_U = INPUT_AR.noiseU;

    N_OBSERVATIONS = 10000;

    REAL_varU = 0.1; 
     % the real coefficients
    REAL_A = [1.51, -1.08, 0.47, -0.23, 0.91, -1.30, 0.86, -0.32];
    
    M = length(REAL_A);
    matrix_A = vertcat(REAL_A, horzcat(eye(M-1,M-1), zeros(M-1,1)));
    
    
    b = eye(M,1);
    
    REAL_x = eye(1,M);
    msg = ffg.gaussMessage(REAL_x, eye(M, M) * REAL_varU, 'VARIANCE');
    
    mean_a = REAL_A; % zeros(1,M);
    prec_a = eye(M,M);
    
    x_next = msg;
   
    for i = 1:N_OBSERVATIONS
         x_new = REAL_A * REAL_x' + NOISE_U(i) * REAL_varU;
         REAL_x = [x_new, REAL_x(1:end-1)];
         
         %% observation
         y = x_new;
         
         x_prev = x_next;
         
         %% X_n = X_n-1 * A
         %TODO: REAL_A -> a
         matrix_A = vertcat(mean_a, horzcat(eye(M-1,M-1), zeros(M-1,1)));
         curr = multiplyForward(x_next, matrix_A);
         
         %% adding noise
         noiseU = ffg.gaussMessage(zeros(1,M), b * REAL_varU * b', 'VARIANCE');
         curr = addForward(curr, noiseU);
         
         %% the next message based on X_n-1 and Y
         c = eye(M, 1);
         
         A = c';
                  
         %% the complex node forward table
         mean_x = curr.mean';
         var_x = curr.var;
         var_y = 0;
         mean_y = y;
         G = inv(var_y + A * var_x * A');
         mean = (mean_x + var_x * A' * G * (mean_y - A * mean_x))';
         var = var_x - var_x * A' * G * A * var_x;
         
         %% 
         x_next.mean = mean;
         x_next.var = var;
         
         
         %% now estimating A itself
         mean_7 = x_next.mean;
         var_7 = x_next.var;

         mean_8 = mean_7;
         var_8 = var_7 + b * REAL_varU * b';
         
         mean_9 = mean_8(1) * x_prev.mean / sum(x_prev.mean .* x_prev.mean);
         prec_9 = x_prev.mean' * x_prev.mean / var_8(1,1);
         
         %% equality table message
         mean_a = (pinv(prec_9 + prec_a) * (prec_a * mean_a' + prec_9 * mean_9'))';
         prec_a = prec_a + prec_9;
         
    end
    
    
    mean_a
    
   
    
return;

function result = addForward(msga, msgb)    
    result = struct();
    result.mean = msga.mean + msgb.mean;
    result.var = msga.var + msgb.var;  
    result.type = 1;
    return;

function result = multiplyForward(msga, A)
    result = struct();
    result.mean = (A * msga.mean')';
    result.var = A * msga.var * A';
    result.type = 1;    
    return;


function result = multiplyBackward(msga, A)
    result = struct();
    return;
    
   


