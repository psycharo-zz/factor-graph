classdef DynamicNetwork < ffg.Network
    %DYNAMICNETWORK network with repeated structure
    %   The dynamic network consists of interconnected time slices, each
    %   represented by a smaller static network
    
    methods
        function this = DynamicNetwork()
            this = this@ffg.Network('DynamicNetwork');
        end
        
        function addTemporalEdge(this, src, dst)
            % (ffg) add a temporal edge between two nodes of consequent time slices
            % INPUTS:
            %   src - source node at the step (n-1)
            %   dst - destination node at the step (n)
            mexfactorgraph('addTemporalEdge', this.type_name, this.cpp_handle, src.cpp_handle, dst.cpp_handle);
        end
        
        function result = adjacencyMatrixTemporal(this)
            % (ffg) get an adjacency matrix of the network, for edges between different time slices
            result = mexfactorgraph('adjacencyMatrixTemporal', this.type_name, this.cpp_handle);
        end
        
        function makeStep(this, data, count)
            % (ffg) make several steps with data (or one if no data)
            % INPUTS:
            %   data - array of structs struct('id',node_id, 'message', msg),
            %   where 'id' contains id of the EvidenceNode that will
            %   receive the message contained in 'message'. the size of the
            %   array should be <number_of_steps> * count
            %   count - the number of messages per step 
            if nargin == 3
                mexfactorgraph('step', this.type_name, this.cpp_handle, data, count);
            else
                mexfactorgraph('step', this.type_name, this.cpp_handle);
            end
        end
        
    end
    
end

