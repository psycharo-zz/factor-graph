classdef EvidenceNode < ffg.FactorNode
    %EVIDENCENODE used to specify observed data/noise
    %   use 'propagate' method to start loopy mode, and 'receive' method for initial
    %   non-loopy mode
   
    methods
       function this = EvidenceNode(nwk)
           this = this@ffg.FactorNode('EvidenceNode', nwk);
       end
      
       function propagate(this, message)
           % (ffg) start propagating messages in non-loopy mode 
           %  message - incoming message
           %  (NOTE: for loopy graphs the behavior is undefined)
           mexfactorgraph('propagate', this.type_name, this.cpp_handle, message);
       end

       function res = evidence(this)
           % (ffg) get the evidence received from 
           res = mexfactorgraph('evidence', this.type_name, this.cpp_handle);
       end
    end
    
end

