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
    
    
    sd = 10;
    sd2 = sd*sd;
    u_const = 1;

    msg = [1+randn()*sd sd2]
    xout.receive(msg);
    n.receive([0 sd2]);
    u.receive([u_const 0]);
    
    
    for i = 1:200
        xin.receive(msg);
        y.receive([i+randn() 0]);
        msg = xout.evidence();
    end

    
    
end

