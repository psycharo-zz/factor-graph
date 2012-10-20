function [ ] = kalman_example()

    xin = evidencenode(1);
    xout = evidencenode(2);
    n = evidencenode(3);
    y = evidencenode(4);
    e = equalitynode(5);
    a = addnode(6);
    u = evidencenode(7);
    b = addnode(8);
      
    xin.setDest(e);
    b.setConnections(e, u, xout);
    xout.setDest(b);
    n.setDest(a);
    y.setDest(a);
    e.setConnections(xin, a, b);
    a.setConnections(e, n, y);
    u.setDest(b);
     
     sd = 10.0;
     sd2 = sd*sd;
     u_const = 1.0;
     
     % currently the messages are represented as structs
     % with fields 'from', 'to' (can be skipped), 
     % 'type' stands for type, currently only gaussians are supported
     % for gaussians only:
     % 'mean' - 1xN vector
     % 'var' - NxN matrix
     msg = struct('mean',1+randn()*sd, 'var', sd2, 'type', 1);
     
     xout.setInitial(msg);
     n.setInitial(struct('mean',0, 'var',sd2, 'type', 1));
     u.setInitial(struct('mean',u_const, 'var',0, 'type', 1));
          
     
     N_ITERATIONS = 200;
     samples = [];
     
     result = zeros(N_ITERATIONS, 2);    
     
     for i = 1:N_ITERATIONS
          xin.setInitial(msg);
         samples(i,:) = i+randn()*sd;
         y.setInitial(struct('mean', samples(i), 'var',0, 'type', 1));
         msg = xout.evidence();
         result(i,:) = [msg.mean, msg.var];
     end

     msg

    subplot(2,1,1)
    plot(1:N_ITERATIONS,result(:,1));
    title('xout')
    subplot(2,1,2)
    plot(1:N_ITERATIONS,result(:,2));
    title('var')
    
end

