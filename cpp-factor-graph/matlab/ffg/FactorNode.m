classdef FactorNode < CppObject
   

    methods (Access = protected)
        function this = FactorNode(type_name)
            this = this@CppObject(type_name);
        end
    end
    
    methods
        % get id
        function result = id(this)
            result = mexfactorgraph('id', this.type_name, this.cpp_handle);
        end
        
        % receive a message
        function receive(this, message)
            mexfactorgraph('receive', this.type_name, this.cpp_handle, message);
        end
    end
    
end

