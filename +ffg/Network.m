classdef Network < ffg.CppObject
    %NETWORK the whole network. TODO: check if this should be a factor node
    
    
    properties (Hidden = true, SetAccess = private)
        node_list;  % The list of names
    end    

    
   
    methods
        function this = Network(type)
            if nargin == 0
                type = 'Network';
            end
            this = this@ffg.CppObject(type);
            this.node_list = containers.Map('KeyType', 'int32', 'ValueType', 'any');
        end
        
        function addEdge(this, a, b, tagA, tagB)
            % (ffg) add an edge between two nodes
            %   a - the source node
            %   b - the destination node
            %   tagA - the tag specifying the connection type for the node "a", by default 'OUTGOING'
            %   tagB - the tag specifying the connection type for the node "b", by default 'INCOMING'
            this.node_list(a.id) = a;
            this.node_list(b.id) = b;
            if nargin == 3
                mexfactorgraph('addEdge', this.type_name, this.cpp_handle, a.cpp_handle, b.cpp_handle);
            elseif nargin == 5
                % TODO: make the number of parameters varying?
                mexfactorgraph('addEdge', this.type_name, this.cpp_handle, a.cpp_handle, b.cpp_handle, tagA, tagB);
            end
        end
        
        
        function setSchedule(this, schedule)
            % (ffg) set a schedule a cell array of nodes e.g. {a, b; b, c}
            N = size(schedule,1);
            scheduleArray = zeros(1,2*N,'uint64');
            for i = 1:N
                scheduleArray(2*(i-1)+1) = schedule{i,1}.cpp_handle;
                scheduleArray(2*(i-1)+2) = schedule{i,2}.cpp_handle;
            end
            mexfactorgraph('setSchedule', this.type_name, this.cpp_handle, scheduleArray);
        end
        
        function makeStep(this)
            % (ffg) make a single message passing iteration on the graph
            mexfactorgraph('step', this.type_name, this.cpp_handle);
        end
        
        function result = adjacencyMatrix(this)
            % (ffg) get an adjacency matrix of the network
            % result(i, j) == 1 if there is an edge from node i to node j,
            % where i,j are indices in the array returned by method nodes()
            % result(i, j) == 0 otherwise
            result = mexfactorgraph('adjacencyMatrix', this.type_name, this.cpp_handle);
        end
        
        
        function result = nodes(this)
            % (ffg) get a list of nodes for the network
            % the list is sorted in ascending order
            result = {};
            node_ids = mexfactorgraph('nodes', this.type_name, this.cpp_handle);
            for i = 1:length(node_ids)
                result{i} = this.node_list(node_ids(i));
            end
        end
        
        
        
        function draw(this)
            %(ffg) DRAWNETWORK draw the network graph
            % the following notations are used:
            %  "=" EqualityNode
            %  "+" AddNode
            %  "*" MultiplicationNode
            %  "X" EvidenceNode
            %  "*=" EquMultNode
            %  "*" EstimateMultiplicationNode
            % INPUTS:
            %   network - ffg.Network do draw 
            draw_layout(this.adjacencyMatrix(),  this.labels());
        end
    end
    
    methods (Hidden = true)
        
        function labels = labels(this)
            %(ffg) LABELS get the labels of nodes
            labels = {};
            nodes = this.nodes();
            
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
        end
    end

    
   
end

