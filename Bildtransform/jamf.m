function ut = jamf( comp, val )

if(val >= comp)
    ut = comp;
elseif(val <= 1)
    ut = 1;
else
    ut = val+1;
end

end

