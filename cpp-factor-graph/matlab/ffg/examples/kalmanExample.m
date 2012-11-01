function [ ] = kalman_example()

    xin = EvidenceNode;
    xout = EvidenceNode;
    n = EvidenceNode;
    y = EvidenceNode;
    e = EqualityNode;
    a = AddNode;
    u = EvidenceNode;
    b = AddNode;
    
    nwk = Network;
   
    nwk.addEdge(xin, e);

    nwk.addEdge(e, b);
    nwk.addEdge(u, b);
    nwk.addEdge(b, xout);

    nwk.addEdge(e, a);
    nwk.addEdge(a, y);
    nwk.addEdge(n, a);
    

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
     
     xout.propagate(msg);
     n.propagate(struct('mean',0, 'var',sd2, 'type', 1));
     u.propagate(struct('mean',u_const, 'var',0, 'type', 1));
          
     
     N_ITERATIONS = 1000;
     samples = [];
     
     result = zeros(N_ITERATIONS, 2);    
     
     for i = 1:N_ITERATIONS
         xin.propagate(msg);
         samples(i,:) = i+randn()*sd;
         y.propagate(struct('mean', samples(i), 'var',0, 'type', 1));
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
