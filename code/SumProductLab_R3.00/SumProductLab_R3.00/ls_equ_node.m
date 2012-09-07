classdef ls_equ_node < factor_node
%LS_EQU3_NODE - linear scaler 3 port equal node
%   LS_EQU3_NODE is a subclass of factor node.
%
% Description of factor_fun
%   input: in_msg, from_id, to_id
%   for linear scaler messages, the 1st and 2nd element are mean &
%   covarient respectively.
%   Wout = Win1 + Win2 + ...
%   Wout = Win1*Min1 + Win2*Min2 + ...
% where W is 1/covarient and M is mean

   properties
   end

   methods
       function s = ls_equ_node(id)
           s = s@factor_node(id);
       end
       
       function msg = factor_fun(s, in_msg, from_id, to_id)
           in_msg_arry = cell2mat(in_msg);
           mean_arry = in_msg_arry(:,1);
           var_arry = in_msg_arry(:,2);
           cost_arry = 1./var_arry;
           cost_out = sum(cost_arry);
           var_out = 1/cost_out;
           mean_out = dot(cost_arry,mean_arry)*var_out;
           msg = [mean_out var_out];
       end

   end
end
