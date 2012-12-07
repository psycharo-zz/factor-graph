function drawNetwork(network)
%DRAWNETWORK draw the network graph
%   network - ffg.Network do draw 
% the following notations are used:
%  "=" EqualityNode
%  "+" AddNode
%  "*" MultiplicationNode
%  "X" EvidenceNode
%  "*=" EquMultNode
%  "*" EstimateMultiplicationNode

    nodes = network.nodes();
    labels = {};
    for i = 1:length(nodes)
        if strcmp(nodes{i}.type_name, 'EqualityNode')
            labels{i} = sprintf('%d:=', nodes{i}.id);
        elseif strcmp(nodes{i}.type_name, 'AddNode')
            labels{i} = sprintf('%d:+', nodes{i}.id);
        elseif strcmp(nodes{i}.type_name, 'MultiplicationNode')
            labels{i} = sprintf('%d:*', nodes{i}.id);
        elseif strcmp(nodes{i}.type_name, 'EvidenceNode')
            labels{i} = sprintf('%d:X', nodes{i}.id);
        elseif strcmp(nodes{i}.type_name, 'EquMultNode')
            labels{i} = sprintf('%d:=*', nodes{i}.id);
        elseif strcmp(nodes{i}.type_name, 'EstimateMultiplicationNode')
            labels{i} = sprintf('%d:*', nodes{i}.id);
        else
            labels{i} = nodes{i}.type_name;
        end
    end
    
    draw_layout(network.adjacencyMatrix(),  labels);
    %draw_layout_dbn(network.adjacencyMatrix(),  zeros(length(nodes), length(nodes)), 0, 2, labels);
end

