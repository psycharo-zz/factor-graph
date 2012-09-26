classdef factornode < matlab.System
   
    % the pointer to the cpp code
    properties (Hidden = true, SetAccess = private)
        cpp_handle;
        type_name;
    end    
    
    methods (Access = protected)
        % constructor
        function this = factornode(id, type_name)
            this.type_name = type_name;
            this.cpp_handle = mexfactorgraph('create', this.type_name, id);
        end
       
    end

    methods
        % destructor
        function delete(this)
            mexfactorgraph('delete', this.type_name, this.cpp_handle);
        end
        
        % get id
        function id = get_id(this)
            id = mexfactorgraph('get_id', this.type_name, this.cpp_handle);
        end
        
        % receive a message
        function receive(this, from, message)
            mexfactorgraph('receive', this.type_name, this.cpp_handle, from, message);
        end
    end
    
end

