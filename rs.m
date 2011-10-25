function [m, s] = rs(out)
  a = size(out, 1);

  rmv = floor(0.1 * a);

  out = out(rmv:(end - rmv), 1);
  
  m = mean(out);

  s = (std(out) / m ) * 100;

end
