classdef CppObject < matlab.System
    %OBJECT the base class for C++ wrappers
    % All the nodes in this framework should be sublclassed from this one
    
    properties (Hidden = true, SetAccess = private)
        cpp_handle; % The pointer to the cpp object
        type_name;  % The name of the class
    end    
    
    methods (Access = protected)
        function this = CppObject(type_name)
            % (ffg) basic constructor
            % type_name - the name of the cpp type we are creating
            this.type_name = type_name;
            this.cpp_handle = mexfactorgraph('create', this.type_name);
        end
    end
    
    
    methods
        function delete(this)
            % (ffg) basic destructor
            mexfactorgraph('delete', this.type_name, this.cpp_handle);
        end
    end
    
end

