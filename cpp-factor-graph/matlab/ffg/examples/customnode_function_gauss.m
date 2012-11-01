function out_msg = customnode_function_gauss(from, to, msgs, incoming, outgoing)
% to - the destination of the message
% msgs - array of messages (struct)
% incoming - array of ids for incoming nodes
% outgoing - array of ids for outgoing nodes
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    
%%%%%%%%% specifying input types for gaussian (codegen) %%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    assert(isa(from, 'double'));
    assert(isa(to, 'double'));
    assert(isa(incoming, 'double') && size(incoming, 2) <= Inf);
    assert(isa(outgoing, 'double') && size(outgoing, 2) <= Inf);

    % messages
    assert(isstruct(msgs));
    assert(all(size(msgs) <= [1 Inf]));
    assert(isa(msgs(1).from,'double'));
    assert(isa(msgs(1).type,'double'));
    assert(isa(msgs(1).mean, 'double') && size(msgs(1).mean, 2) <= Inf);
    assert(isa(msgs(1).var, 'double') && size(msgs(1).var, 2) <= Inf);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    
%%%%%%%%%%%%%%%%%%%%%%%% your code here %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    out_msg = struct('from', from, 'type',1, 'mean',[], 'var',[]);
    
    isForward = isempty(find(incoming == to));
    if isForward
        out_msg.mean = sum([msgs.mean]);
        out_msg.var = sum([msgs.var]);
    else
        res_idx = find([msgs.from] == outgoing(1));
        out_msg.mean = msgs(res_idx).mean;
        out_msg.var = msgs(res_idx).var;
        
        in_idx = setdiff(find([msgs.from] ~= outgoing(1)), to);
        for i = in_idx
            out_msg.mean = out_msg.mean - msgs(i).mean;
            out_msg.var = out_msg.var + msgs(i).var;
        end
    end
end

