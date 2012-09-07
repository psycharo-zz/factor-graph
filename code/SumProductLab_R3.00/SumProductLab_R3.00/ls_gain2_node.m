classdef ls_gain2_node < factor_node
%LS_GAIN2_NODE - linear scaler 3 port equal node
%   LS_GAIN2_NODE is a subclass of factor node.
%
% Description of factor_fun
%   input: in_msg, from_id, to_id
%   for linear scaler messages, the 1st and 2nd element are mean &
%   covarient.
%   if to_id is child
%     Vout = gain^2*Vin1
%     Mout = gain*Vin1
%   else
%     Vout = Vin1/gain^2
%     Mout = Vin1/gain
% where V is covarient and M is mean

   properties
       gain           % gain constant
   end

   methods
       function s = ls_gain2_node(id)
           s = s@factor_node(id);
       end
       
       function setup_gain(s,gain)
           s.gain = gain;
       end
       
       function msg = factor_fun(s, in_msg, from_id, to_id)
           if size(in_msg,1) ~= 1
               sprintf('Node %0.0f ERROR from factor_fun: size of linklist not equal to 2 !!!\n',s.id)
           else
               if to_id == s.link_id(end)
                   % forward passing
                   covar_out = s.gain*(s.gain')*in_msg{1}(2);
                   mean_out = s.gain*in_msg{1}(1);
               else
                   % reverse passing
                   covar_out = in_msg{1}(2)/(s.gain*(s.gain'));
                   mean_out = in_msg{1}(1)/s.gain;
               end
               msg = [mean_out covar_out];
           end
       end

   end
end
