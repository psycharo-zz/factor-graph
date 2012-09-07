classdef ls_add_node < factor_node
%LS_ADD3_NODE - linear scaler 3 port equal node
%   LS_ADD3_NODE is a subclass of factor node.
%
% Description of factor_fun
%   input: in_msg, from_id, to_id
%   for linear scaler messages, the 1st and 2nd element are mean &
%   covarient.
%   Vout = Vin1 + Vin2
%   Mout = Min1 - Min2 if in1 is child
%   Mout = Min1 + Min2 otherwise
% where V is covarient and M is mean

   properties
       operation
   end

   methods
       function s = ls_add_node(id)
           s = s@factor_node(id);
       end
       
       function setup_operation(s,op)
           s.operation = op;
       end
       
       function msg = factor_fun(s, in_msg, from_id, to_id)
           in_msg_arry = cell2mat(in_msg);
           mean_arry = in_msg_arry(:,1);
           var_arry = in_msg_arry(:,2);
           var_out = sum(var_arry);
           % sort operation according to the order of from_id
           op_list = [s.operation 1];
           op = zeros(size(s.operation));
           for i = 1:size(s.operation,2)
               op(i) = op_list(s.link_id == from_id(i));
           end
           if to_id == s.link_id(end)
               % forward passing
               mean_out = sum(dot(mean_arry,op));
           else
               % reverse passing
               child_idx = find(from_id==s.link_id(end));
               % seperate the child mean from the mean array
               child_mean = mean_arry(child_idx);
               mean_arry(child_idx) = [];
               op(child_idx) = [];
               mean_out = child_mean - sum(dot(mean_arry,op));
               % adjust sign
               to_op = op_list(s.link_id == to_id);
               mean_out = to_op*mean_out;
           end
           msg = [mean_out var_out];
       end
   end
end
