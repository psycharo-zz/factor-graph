classdef Object < matlab.System
    %OBJECT the base class for this wrappers
    
    
    properties (Hidden = true, SetAccess = private)
        % the pointer to the cpp code
        cpp_handle;
        % the name of the class
        type_name;
    end    
    
    methods (Access = protected)
        % constructor
        function this = Object(type_name)
            this.type_name = type_name;
            this.cpp_handle = mexfactorgraph('create', this.type_name);
        end
    end
    
    
    methods
        function delete(this)
            mexfactorgraph('delete', this.type_name, this.cpp_handle);
        end
    end
    
end

