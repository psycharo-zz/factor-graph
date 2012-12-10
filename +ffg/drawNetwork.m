function drawNetwork(network, numrep)
%DRAWNETWORK draw the network graph
% the following notations are used:
%  "=" EqualityNode
%  "+" AddNode
%  "*" MultiplicationNode
%  "X" EvidenceNode
%  "*=" EquMultNode
%  "*" EstimateMultiplicationNode
% INPUTS:
%   network - ffg.Network do draw 
%   numrep - for dynamic networks only, the number of times to repeat

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

    if strcmp(class(network), 'ffg.DynamicNetwork') && nargin == 2
        draw_layout_dbn(network.adjacencyMatrix(),  network.adjacencyMatrixTemporal(), 0, numrep, labels);
    elseif strcmp(class(network), 'ffg.DynamicNetwork')
        draw_layout_dbn(network.adjacencyMatrix(),  network.adjacencyMatrixTemporal(), 0, 2, labels);
    else
        draw_layout(network.adjacencyMatrix(),  labels);
    end
    %
end

