function kalmanScheduleExample()

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%       	  +---+ 
%        	  |U_t| 
%        	  +-+-+ 
%               | 
%        	    v 
% +-------+   +---+    +---+    +---+
% |X_{t-1}+-->|+,a+--->|=,e+--->|X_t| 
% +-------+   +---+    +-+-+    +---+ 
%                        | 
%                        v
%              +---+   +---+ 
%      	       |W_t+-->|+,b| 
% 	           +---+   +-+-+ 
%                        | 
%                        v 
%                      +---+ 
%                      |Y_t| 
%                      +---+ 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    nwk = ffg.Network;

    % hidden variable, X_{t-1}
    xin = ffg.EvidenceNode;
    % hidden variable, X_t
    xout = ffg.EvidenceNode;
    % observation, corresponds to a half-edge y
    y = ffg.EvidenceNode;
    % equality node, connects a, b, xout
    e = ffg.EqualityNode;
    % process noise, U_t
    u = ffg.EvidenceNode;
    % observation noise, W_t
    w = ffg.EvidenceNode;
    % zero-sum node, connects xin + u = e
    a = ffg.AddNode;
    % zero-sum node, connects e + w = y
    b = ffg.AddNode;    
    
    % connecting nodes with each other
    nwk.addEdge(xin, a); 
    nwk.addEdge(u, a);
    nwk.addEdge(a, e); 
    nwk.addEdge(e, xout); 
    nwk.addEdge(e, b);
    nwk.addEdge(w, b); 
    nwk.addEdge(b, y); 
    
    % drawing the constructed network
    ffg.drawNetwork(nwk);
    
    % creating message-passing schedule
    nwk.setSchedule({xin, a; ...
                       u, a; ...
                       a, e; ...
                       y, b; ...
                       w, b; ...
                       b, e; ...
                       e, xout});


    % specifying some gaussian parameters               
    sd = 10.0;
    sd2 = sd*sd;
    % initialising X_{t-1}
    xout.receive(ffg.gaussVarianceMessage(1+randn()*sd, sd2));
    % process noise, setting it to constant
    u.receive(ffg.gaussVarianceMessage(1, 0));
    % observation noise
    w.receive(ffg.gaussVarianceMessage(0, sd2))
    
    % the number of timeslices to run
    N_ITERATIONS = 1000;
    % place to store results for each 
    result = zeros(N_ITERATIONS, 2);    
    samples = zeros(N_ITERATIONS, 1);
    msg = ffg.gaussVarianceMessage(randn()*sd, sd2);
    for i = 1:N_ITERATIONS
        samples(i) = i+randn()*sd;

        % setting X_{t-1} to the estimation on the previous step
        xin.receive(msg);
        % receiving subsequent observation
        y.receive(ffg.gaussVarianceMessage(samples(i), 0));
        nwk.makeStep();         
        msg = xout.evidence();
        
        xout.messages
        
        result(i,:) = [msg.mean, msg.var];
    end
    
    % plotting X estimations and the real 
    subplot(2,1,1);
    hold on;
    plot(1:N_ITERATIONS,result(:,1), 'Color', 'b');
    plot(1:N_ITERATIONS,samples(:,1), 'Color', 'r');
    hold off;
    
    title('X value');
    subplot(2,1,2);
    plot(1:N_ITERATIONS,result(:,2));
    title('variance');
end

