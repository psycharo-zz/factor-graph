classdef addnode < factornode
    %ADDNODE Summary of this class goes here
    %   Detailed explanation goes here
    
    methods
       function this = addnode(id)
           this = this@factornode(id, 'addnode');
       end
       
       function setConnections(this, summand1, summand2, result)
           mexfactorgraph('setConnections', this.type_name, this.cpp_handle, summand1.cpp_handle, summand2.cpp_handle, result.cpp_handle);
       end
    end
        
    
end

