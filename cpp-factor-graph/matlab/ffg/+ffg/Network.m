classdef Network < ffg.CppObject
    %NETWORK the whole network. TODO: check if this should be a factor node
    % ADDEDGE
    
    methods
        function this = Network()
            this = this@ffg.CppObject('Network');
        end
        
        function addEdge(this, a, b)
            % (ffg) specify a _directed_ connection between two factor nodes a and b.
            mexfactorgraph('addEdge', this.type_name, this.cpp_handle, a.cpp_handle, b.cpp_handle)
        end
        
        function addEdgeTagged(this, a, b, tagA, tagB)
            % (ffg) same as addEdge but allows manual connection type
            % specification. leave empty ('') for default
            mexfactorgraph('addEdgeTagged', this.type_name, this.cpp_handle, a.cpp_handle, b.cpp_handle, tagA, tagB)
        end
        
        function setSchedule(this, schedule)
            % (ffg) set a schedule a cell array of nodes e.g. {a, b, b, c}
            N = size(schedule,2);
            scheduleArray = zeros(1,N, 'uint64');
            for i = 1:N
                scheduleArray(i) = schedule{i}.cpp_handle;
            end
            mexfactorgraph('setSchedule', this.type_name, this.cpp_handle, scheduleArray);
        end
        
        function makeStep(this)
            % (ffg) make a single message passing iteration on the graph
            mexfactorgraph('step', this.type_name, this.cpp_handle);
        end
            
    end
    
   
end

