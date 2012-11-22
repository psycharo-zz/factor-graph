classdef EvidenceNode < ffg.FactorNode
    %EVIDENCENODE Summary of this class goes here
    %   Detailed explanation goes here
   
    methods
       function this = EvidenceNode()
           this = this@ffg.FactorNode('EvidenceNode');
       end
      
       function propagate(this, message)
           mexfactorgraph('propagate', this.type_name, this.cpp_handle, message);
       end

       function res = evidence(this)
           res = mexfactorgraph('evidence', this.type_name, this.cpp_handle);
       end
    end
    
end

