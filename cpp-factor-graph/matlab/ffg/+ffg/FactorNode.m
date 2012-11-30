classdef FactorNode < ffg.CppObject
    %FACTORNODE base class for all factor nodes

    methods (Access = protected)
        function this = FactorNode(type_name)
            this = this@ffg.CppObject(type_name);
        end
    end
    
    methods
        
        function result = id(this)
            % (ffg) get node's identifier. mostly needed for debug purposes
            result = mexfactorgraph('id', this.type_name, this.cpp_handle);
        end
        
        
        function receive(this, message)
            % (ffg) receive a single message
            mexfactorgraph('receive', this.type_name, this.cpp_handle, message);
        end
        
        function result = messages(this)
            % (ffg) get all the current messages of the node. mostly needed for
            % debug purposes
            result = mexfactorgraph('messages', this.type_name, this.cpp_handle);
        end
    end
    
end

