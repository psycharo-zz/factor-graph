classdef FactorNode < ffg.CppObject
    %FACTORNODE base class for all factor nodes

    methods (Access = protected)
        function this = FactorNode(type_name, nwk)
            cpp_handle = mexfactorgraph('create', type_name, nwk.cpp_handle);
            this = this@ffg.CppObject(type_name, cpp_handle);
        end
    end
    
    methods
        
        function result = id(this)
            % (ffg) get node's identifier. mostly needed for debug purposes
            result = mexfactorgraph('id', this.type_name, this.cpp_handle);
        end
        
        
        function receive(this, message)
            % (ffg) receive a single message, without propagating it further
            mexfactorgraph('receive', this.type_name, this.cpp_handle, message);
        end
        
        function result = messages(this)
            % (ffg) get all the current incoming messages of the node. 
            result = mexfactorgraph('messages', this.type_name, this.cpp_handle);
        end
    end
    
end

