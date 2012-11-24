classdef FactorNode < ffg.CppObject
   

    methods (Access = protected)
        function this = FactorNode(type_name)
            this = this@ffg.CppObject(type_name);
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
        
        % get all the current messages of the node
        function result = messages(this)
            result = mexfactorgraph('messages', this.type_name, this.cpp_handle);
        end
    end
    
end

