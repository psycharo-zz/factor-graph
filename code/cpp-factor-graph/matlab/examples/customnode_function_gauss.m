function out_msg = customnode_function_gauss(to, msgs)

    % specifying input types for gaussian
    assert(isa(to, 'double'))
    assert(isstruct(msgs));
    assert(all(size(msgs) <= [1 Inf]));
    assert(isa(msgs(1).from,'double'));
    assert(isa(msgs(1).to,'double'));
    assert(isa(msgs(1).type,'double'));
    assert(isa(msgs(1).mean, 'double') && size(msgs(1).mean, 2) <= Inf);
    assert(isa(msgs(1).var, 'double') && size(msgs(1).var, 2) <= Inf);

    out_msg = struct('from',100500, 'to',200300, 'type',1, 'mean',[], 'var',[]);
        
    out_msg.var = msgs(1).var + msgs(2).var;
    out_msg.mean = msgs(1).mean + msgs(2).mean;
end

