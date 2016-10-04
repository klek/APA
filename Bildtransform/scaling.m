function ut = scaling( X, axl, v)

tump = 0;
counter = 0;
xxxx = 0;
yyyy = 1;
[sx sy] = size(X);
if(v == 1) %x
    M = zeros(axl,sy);
    tx = sx/axl;
    intx = ceil(tx) %3
    decix = intx-tx %0.5
    highx = sx-(ceil(sx*decix));
    highxcount = 1;
    for yy = 1:sy
        for xx = 1:axl
                if(highxcount < highx)
                    for xxx = 1:intx
                        xxxx = xxxx + 1;
                        tump = tump + X(xxxx,yy);
                        
                        highxcount = highxcount + 1;
                        counter = counter + 1;
                    end
                else
                    for xxx = 1:intx-1
                        xxxx = xxxx + 1;
                        tump = tump + X(xxxx,yy);
                        
                        highxcount = highxcount + 1;
                        counter = counter + 1;
                    end
                end
        value = tump/(counter);    
        M(xx,yy) = value;
        counter = 0;
        
        tump = 0;        
        end
    xxxx = 0;    
    highxcount = 1;
    end
    ut = M;
else %y
    M = zeros(sx,axl);
    ty = sy/axl;
    inty = ceil(ty) %3
    deciy = inty-ty %0.5
    highy = sy-(ceil(sy*deciy));
    highycount = 1;
    for xx = 1:sx
        for yy = 1:axl
                if(highycount < highy)
                    for yyy = 1:inty 
                        tump = tump + X(xx,yyyy);
                        yyyy = yyyy + 1;
                        highycount = highycount + 1;
                        counter = counter + 1;
                    end
                else
                    for yyy = 1:inty-1
                        tump = tump + X(xx,yyyy);
                        yyyy = yyyy + 1;
                        highycount = highycount + 1;
                        counter = counter + 1;
                    end
                end
        value = tump/(counter);    
        M(xx,yy) = value;
        counter = 0;
        
        tump = 0;        
        end
    yyyy = 1;
    highycount = 1;
    end
    ut = M;
    
end


end

