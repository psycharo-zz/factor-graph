classdef Network < CppObject
    %NETWORK the whole network. TODO: check if this should be a factor node
    %   Detailed explanation goes here
    
    methods
        function this = Network()
            this = this@CppObject('Network');
        end
        
        function addEdge(this, a, b)
            mexfactorgraph('addEdge', this.type_name, this.cpp_handle, a.cpp_handle, b.cpp_handle)
        end
        
        function setSchedule(this, schedule)
            % schedule - cell array of nodes
            N = size(schedule,2);
            scheduleArray = zeros(1,N, 'uint64');
            for i = 1:N
                scheduleArray(i) = schedule{i}.cpp_handle;
            end
            mexfactorgraph('setSchedule', this.type_name, this.cpp_handle, scheduleArray);
        end
        
        function makeStep(this)
            mexfactorgraph('step', this.type_name, this.cpp_handle);
        end
            
    end
    
   
end

