classdef Network < Object
    %NETWORK the whole network. TODO: check if this should be a factor node
    %   Detailed explanation goes here
    
    methods
        function this = Network()
            this = this@Object('Network');
        end
        
        function addEdge(this, a, b)
            mexfactorgraph('addEdge', this.type_name, this.cpp_handle, a.cpp_handle, b.cpp_handle)
        end
    end
    
end

