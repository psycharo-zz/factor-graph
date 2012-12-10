function out_msg = customnode_function_gauss(from, to, msgs)
% to - the destination of the message
% msgs - array of messages (struct)
% incoming - array of ids for incoming nodes
% outgoing - array of ids for outgoing nodes
% other - array of ids and tags for other nodes
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    
%%%%%%%%% specifying input types for gaussian (codegen) %%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    assert(isa(from, 'double'));
    assert(isa(to, 'double'));
    % messages
    assert(isstruct(msgs));
    assert(all(size(msgs) <= [1 Inf]));
    assert(isa(msgs(1).from,'double'));
    assert(isa(msgs(1).type,'char') && size(msgs(1).type, 2) <= 1024);
    assert(isa(msgs(1).connection,'char') && size(msgs(1).connection, 2) <= 1024);
    assert(isa(msgs(1).mean, 'double') && size(msgs(1).mean, 2) <= Inf);
    assert(isa(msgs(1).var, 'double') && size(msgs(1).var, 2) <= Inf);
   
   
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    
%%%%%%%%%%%%%%%%%%%%%%%% your code here %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    out_msg = struct('type', 'VARIANCE', 'mean',[], 'var',[]);
    isForward = 1;
    
    forward = -1;
    for i = 1:length(msgs)
        if strcmp(msgs(i).connection, 'OUTGOING') && to ~= msgs(i).from
            isForward = 0;
            forward = i;
            out_msg.mean = msgs(i).mean;
            out_msg.var = msgs(i).var;
            break;
        end
    end
   
    if isForward
        sz = length(msgs(1).mean);
        out_msg.mean = zeros(1, sz);
        out_msg.var = zeros(sz, sz);
        for i = 1:size(msgs, 2)
            out_msg.mean = out_msg.mean + msgs(i).mean;
            out_msg.var = out_msg.var + msgs(i).var;
        end
    else
        for i = 1:size(msgs, 2)
            if to ~= msgs(i).from && forward ~= i
                out_msg.mean = out_msg.mean - msgs(i).mean;
                out_msg.var = out_msg.var + msgs(i).var;
            end
        end
    end
end

